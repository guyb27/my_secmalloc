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

t_heap heap = {0, 0, 0, 0};
size_t heap_size = INIT_CHUNK_SIZE;




#define FIXED_CANARY 0xDEADBEEFDEADBEEF  // Définir une valeur fixe pour le canary

int init_heap() {
    if (heap.init == 0) {
        heap.init = 1;

        heap.tiny = (t_chunk*) mmap((void*)(BASE_CHUNK_ADDR), TOTAL_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
        if (heap.tiny == MAP_FAILED)
            return 1;

        heap.small = (void*)((size_t)heap.tiny + TINY_CHUNK_SIZE);
        heap.large = (void*)((size_t)heap.small + SMALL_CHUNK_SIZE);

        heap.tiny->addr = heap.large + LARGE_CHUNK_SIZE;
        heap.tiny->size = TINY_MEMORY_SIZE;
        heap.tiny->flag = FREE;

        heap.small->addr = heap.tiny->addr + TINY_MEMORY_SIZE;
        heap.small->size = SMALL_MEMORY_SIZE;
        heap.small->flag = FREE;
    }
    return 0;
}

t_chunk* manage_memory(t_chunk *item, size_t base_addr, size_t old_size, size_t size) {
    t_chunk *new_heap = NULL;
    int new_size = size;

    if (!item && size > SMALL_MAX) {
        // FAIRE UN MMAP JUST POUR UNE STRUCTURE
    } else if (!item) {
        // FAIRE UN MMAP D UNE POOL DE STRUCTURE POUR LES SMALL ET LES TINY
    }

    if (!item->addr) {
        item->addr = (void*) mmap((void*)(base_addr), new_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    } else if (size <= item->size) {
        new_size = size + old_size;
        new_heap = mremap(item->base_addr, old_size, new_size, 0);
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
    }
    return item;
}

t_chunk* search_free_chunk(size_t s) {
    t_chunk *item = NULL;
    if (s <= SMALL_MAX) {
        item = s <= TINY_MAX ? heap.tiny : heap.small;
        while (item) {
            if (item->flag == FREE && item->size >= s)
                return item;
            item = item->next;
        }
    }
    return NULL;
}

t_chunk* init_chunks(size_t size) {
    t_chunk *item = NULL;
    if (size <= SMALL_MAX) {
        item = size <= TINY_MAX ? heap.tiny : heap.small;
    } else {
        item = heap.large;
    }
    while (item && item->next)
        item = item->next;
    item = manage_memory(item, BASE_ADDR, 0, size);
    return item;
}

t_chunk* new_large_chunk(size_t s) {
}

t_chunk* get_free_chunk(size_t s) {
    if (init_heap()) {
        return NULL;
    }
    t_chunk *item = NULL;
    if (s <= SMALL_MAX)
        item = search_free_chunk(s);
    else
        item = (void*) mmap((void*)(BASE_ADDR), s, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    return item;
}

void* my_malloc(size_t size) {
    // Allouer de la mémoire supplémentaire pour le canary
    size_t total_size = size + sizeof(size_t);
    t_chunk *ch = get_free_chunk(total_size);
    if (ch == NULL) {
        return NULL;
    }
    void *ptr = ch->addr;
    ch->flag = BUSY;
    ch->size = total_size;
    // Placer le canary à la fin de la zone mémoire allouée
    *((size_t *)((char *)ptr + size)) = FIXED_CANARY;
    return ptr;
}

void my_free(void *ptr) {
    if (ptr == NULL) {
        return;
    }

    t_chunk *chunk = (t_chunk *)((char *)ptr - sizeof(t_chunk));
    size_t size = chunk->size - sizeof(size_t);

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
