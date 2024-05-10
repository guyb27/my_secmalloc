#define _GNU_SOURCE
#include "my_secmalloc.private.h"
#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>

s_chunk *heap = NULL;

static int print_in_file(const char *func_call, size_t size, size_t addr)
{
    const char *MSM_OUTPUT = getenv("MSM_OUTPUT");
    if (MSM_OUTPUT)
    {
        FILE *fichier = fopen(MSM_OUTPUT, "a");
        if (fichier == NULL) {
            perror("Erreur à l'ouverture du fichier");
            return 1;
        }
        fprintf(fichier, "Nom: %s, Age: %zu, %zx\n", func_call, size, addr);
        fclose(fichier);
    }
    return 0;
}

int init_heap()
{
	if (heap == NULL)
	{
		heap = (s_chunk*) mmap((void*)(BASE_CHUNK_ADDR), INIT_CHUNK_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
        if (heap == MAP_FAILED)
            return 1;
        heap->addr = mmap((void*)(BASE_CHUNK_ADDR), INIT_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
        if (heap->addr == MAP_FAILED)
            return 1;
		heap->size = INIT_MEMORY_SIZE;
		heap->flags = FREE;
        heap->prev=NULL;
        heap->next=NULL;
	}
    return 0;
}

static size_t ft_align_size(size_t size)
{
    return (size / 16 * 16) + (size % 16 > 0 ? 1 : 0) + CANARY_SIZE;
}

/*
static int generate_random_bytes(unsigned char *buffer, size_t size) {
    FILE *fp;
    fp = fopen("/dev/urandom", "rb"); // Ouvre le fichier spécial en mode lecture binaire

    if (fp == NULL) {
        perror("Erreur lors de l'ouverture de /dev/urandom");
        return 1;
    }
    if (fread(buffer, 1, size, fp) != size) {
        perror("Erreur lors de la lecture de /dev/urandom");
        fclose(fp);
        return 1;
    }
    fclose(fp);
    return 0;
}
*/

static void *my_get_free_chunk()
{
    s_chunk *tmp = NULL;
    int i = 0;
    while(i < INIT_CHUNK_SIZE)
    {
        tmp = (s_chunk *)(heap+i);
        if (!tmp)
        {
            return tmp;
        }
        i+=CHUNK_SIZE;
    }
    return NULL;
}

static void *my_chunk_alloc(size_t size)
{
    //P-E check si 
    s_chunk *curr = heap;
    s_chunk *tmp=NULL;
    dprintf(1, "my_chunk_alloc size [%d]\n", (int)size);
    while (curr)
    {
        if (curr->size >= size)
        {
            if (curr->size >= size)
            {
                tmp = curr->next;
                curr->next = my_get_free_chunk();
                dprintf(1, "my_chunk_alloc 0 [%p] next [%p]\n", curr);
                if (!curr->next)
                    return NULL;
                curr->next->next = tmp;
                tmp->prev=curr->next->next;
                curr->next->prev=curr;
                break;
            }
            else
                dprintf(1, "my_chunk_alloc 1 [%p] size [%zu]\n", curr, curr->size);

        }
        curr=curr->next;
    }
    dprintf(1, "my_chunk_alloc last ret [%p]\n", curr);
	return NULL;
}

void    *my_malloc(size_t size)
{

    char func_call[]="malloc";
    //unsigned char random_bytes[16];
    void *ret_ptr = NULL;
    if (heap == NULL)
        if (init_heap())
            return NULL;
    size = ft_align_size(size);
    s_chunk *curr = my_chunk_alloc(size);
    dprintf(1, "my_malloc 0 [%p]\n", curr);
    if (curr == NULL)
        return NULL;
    print_in_file(func_call, size, (size_t)ret_ptr);
    return ret_ptr;
}

void    my_free(void *ptr)
{
    (void) ptr;
}

void    *my_calloc(size_t nmemb, size_t size)
{
    size_t total_size = nmemb * size;
    void *ptr = my_malloc(total_size);
    memset(ptr, 0, total_size);
    return ptr;
}

void    *my_realloc(void *ptr, size_t size)
{
    void *ret=NULL;
    if (ptr && !size)
        my_free(ptr);
    else if (!ptr && size)
        ret = malloc(size);
    else if (size)
        ret=NULL;//EN ATTANDANT
    return ret;

}

#ifdef DYNAMIC
void    *malloc(size_t size)
{
    return my_malloc(size);
}
void    free(void *ptr)
{
    my_free(ptr);
}
void    *calloc(size_t nmemb, size_t size)
{
    return my_calloc(nmemb, size);
}

void    *realloc(void *ptr, size_t size)
{
    return my_realloc(ptr, size);

}

#endif
