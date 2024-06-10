#define _GNU_SOURCE
#include "my_secmalloc.private.h"
#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <stdint.h>  // Inclusion de stdint.h pour uintptr_t
#include <stdarg.h>
#include <alloca.h>

//t_heap heap = {0, 0};
//t_mmap *s_mmap = {0,NULL,NULL,NULL,NULL};

//size_t heap_size = INIT_CHUNK_SIZE;

t_mmap *my_mmap = NULL;
t_chunk *my_t_chunk = NULL;
size_t next_mmap_addr = BASE_METADATA_ADDR;



#define FIXED_CANARY 0xDEADBEEFDEADBEEF  // Définir une valeur fixe pour le canary

void log_message(const char *format, ...)
{
    FILE *log_file = fopen("log.txt", "a");
    if (!log_file) {
        perror("Unable to open log file");
        return;
    }

    va_list args;
    va_start(args, format);

    // Calcule la taille nécessaire pour le buffer
    size_t size = vsnprintf(NULL, 0, format, args) + 1;
    va_end(args);

    // Alloue un buffer sur la pile
    char *buffer = (char *)alloca(size);

    // Remplit le buffer avec le message formaté
    va_start(args, format);
    vsnprintf(buffer, size, format, args);
    va_end(args);

    // Écrit le message dans le fichier de log
    fprintf(log_file, "%s", buffer);

    // Ferme le fichier de log
    fclose(log_file);
}

void *get_new_metadata_addr()
{
    void *ret = NULL;
    t_mmap *tmp_s_mmap = my_mmap;
    t_chunk *tmp_s_chunk = NULL;

    while (tmp_s_mmap)
    {
        ret = tmp_s_mmap+sizeof(t_mmap);//ret vaut l adress de la deniere structure t_mmap+sa taille
        tmp_s_chunk = tmp_s_mmap->chunk;
        while(tmp_s_chunk)
        {
            ret = tmp_s_chunk+sizeof(t_chunk);//L adresse de la derniere stucture t_shunk + sa taille
            tmp_s_chunk=tmp_s_chunk->next;
        }
        tmp_s_mmap = tmp_s_mmap->next;
    }
    return ret;
}

void* manage_memory(t_chunk *ch, size_t size)
{
    if (ch && size == ch->memory_size)
    {
        ch->flag = BUSY;
    }
    else if (ch && size < ch->memory_size)
    {
        ch->next = get_new_metadata_addr();
        ch->next->prev = ch;
        ch->next->next = NULL;
        ch->next->memory_addr = ch->memory_addr+size;
        ch->next->memory_size = ch->memory_size-size;
        ch->next->flag = FREE;
        ch->next->canary = FIXED_CANARY;
        ch->next->memory_mmap_number = ch->memory_mmap_number;

        *((size_t *)((char *)ch->memory_addr + size)) = FIXED_CANARY;

        ch->memory_size=ch->memory_size-size;
        ch->flag = BUSY;
        dprintf(2, "memory_addr: [%p], next: [%p]\n", ch->memory_addr, ch->next->memory_addr);
    }
    else
    {/*
        new_size = size + old_size;
        new_heap = mremap(s_mmap->addr, old_size, new_size, 0);
        if (new_heap == MAP_FAILED) {
            new_size = size;
            item->next->addr = (void*) mmap((void*)(base_addr), new_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
            if (new_heap == MAP_FAILED) {
                item->next->addr = NULL;
                return NULL;
            }
            item->next->prev = item;
            item = item->next;
            item->next = NULL;
            item->base_addr = new_heap;
        }
        else
        {

        }*/
    }
    return ch->memory_addr;
    //return NULL;
}

int init_memory_chunk() {
    //t_chunk *item = NULL;

    if (!my_t_chunk)
    {
        my_mmap->addr = (void*) mmap((void*)(next_mmap_addr), POOL_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
        if (my_mmap->addr == MAP_FAILED) {
            return 1;
        }
        my_t_chunk = my_mmap->addr;
        my_mmap->size = POOL_MEMORY_SIZE;

        next_mmap_addr+=POOL_MEMORY_SIZE;

        my_mmap->chunk = (void*)(my_mmap + sizeof(t_mmap));
        my_mmap->chunk->memory_addr=my_mmap->addr;
        my_mmap->chunk->memory_size=POOL_MEMORY_SIZE;
        my_mmap->chunk->flag=FREE;
        my_mmap->chunk->canary=FIXED_CANARY;
        my_mmap->chunk->memory_mmap_number=0;
        my_mmap->chunk->prev=NULL;
        my_mmap->chunk->next=NULL;
    }
    return 0;
}

int init_metadata() {
    if (my_mmap == 0) {
        dprintf(2, "INIT_METADATA\n");
        my_mmap = (t_mmap*) mmap((void*)(next_mmap_addr), POOL_METADATA_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
        if (my_mmap == MAP_FAILED)
            return 1;
        next_mmap_addr += POOL_METADATA_SIZE;
        //s_mmap->addr = s_mmap + sizeof(s_mmap);
        my_mmap->addr = NULL;
        my_mmap->size = 0;
        my_mmap->prev = NULL;
        my_mmap->next = NULL;
    }
    return 0;
}



t_chunk* search_free_chunk(size_t s) {
    t_chunk *item = NULL;
    printf("search_free_chunk()\n");
    t_mmap *tmp_s_mmap = my_mmap;
    while (tmp_s_mmap)
    {
        item = my_mmap->chunk;
        while (item) {
            printf("ADDR: [%p], FLAG: [%s], SIZE: [%zu], S: [%zu]\n", item->memory_addr, item->flag == FREE ?"FREE":"BUSY", item->memory_size, s);
            item = item->next;
        }
        item = tmp_s_mmap->chunk;
        while (item) {
            //printf("FLAG: [%s], SIZE: [%zu], S: [%zu]\n", item->flag == FREE ?"FREE":"BUSY", item->memory_size, s);
            if (item->flag == FREE && item->memory_size >= s)
            {
              //  printf("ON RETURN\n");
                return item;
            }
            item = item->next;
        }
        tmp_s_mmap=tmp_s_mmap->next;
    }
    return NULL;
}


t_chunk* get_free_chunk(size_t s) {
    dprintf(2, "GET_FREE_CHUNK\n");
    t_chunk *item = NULL;
    item = search_free_chunk(s);
    if (item)
        printf("ITEM ADDR: [%p]\n", item->memory_addr);
    else
        printf("NO ITEM IN get_free_chunk\n");
    return item;
}

void* my_malloc(size_t size) {
    dprintf(2, "=============\n");
    if (init_metadata())
        return NULL;
    if (init_memory_chunk())
        return NULL;
    // Allouer de la mémoire supplémentaire pour le canary
    size_t total_size = size + sizeof(size_t);
    //printf("total size: [%d]\n", (int)total_size);
    t_chunk *ch = get_free_chunk(total_size);
    if (ch == NULL) {
        return NULL;
    }
    void *ptr = manage_memory(ch, total_size);
    //ptr = ch->memory_addr;
    //ch->flag = BUSY;
    //ch->next = 
    //ch->memory_size = total_size;
    // Placer le canary à la fin de la zone mémoire allouée
    //*((size_t *)((char *)ptr + size)) = FIXED_CANARY;
    dprintf(2, "return PTR: [%p], FLAG: [%s], CANARY: [%p]\n",
        ptr,
        ch->flag == FREE?"FREE":"BUSY",
        ((size_t *)((char *)ptr + size))
    );
    return ptr;
}

void my_free(void *ptr) {
    if (ptr == NULL) {
        return;
    }


    t_chunk *chunk = (t_chunk *)((char *)ptr - sizeof(t_chunk));
    size_t size = chunk->memory_size - sizeof(size_t);

    // Vérifiez l'intégrité du canary
    size_t canary = *((size_t *)((char *)ptr + size));
    if (canary != FIXED_CANARY) {
        fprintf(stderr, "Memory corruption detected (canary mismatch)!\n");
        abort(); // Vous pouvez choisir une autre manière de gérer cette erreur
    }

    // Vérifiez si le chunk est déjà libre
    if (chunk->flag == FREE) {
        fprintf(stderr, "Double free detected!\n");
        abort(); // Vous pouvez choisir une autre manière de gérer cette erreur
    }

    // Marquez le chunk comme libre
    chunk->flag = FREE;

    // Fusionner les chunks libres consécutifs (vous pouvez ajouter cette logique)
    // ...
}
