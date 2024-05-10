#ifndef _SECMALLOC_PRIVATE_H
#define _SECMALLOC_PRIVATE_H

#define CHUNK_SIZE sizeof(struct chunk)
#define PAGE_SIZE 4096
#define BASE_CHUNK_ADDR PAGE_SIZE
#define INIT_CHUNK_SIZE PAGE_SIZE*(100000-2)
#define INIT_MEMORY_SIZE PAGE_SIZE*100000
#define CANARY_SIZE 16

#include "my_secmalloc.h"

// sentez vous libre de modifier ce header comme vous le souhaitez


enum chunk_type
{
    FREE=0,
    BUSY=1
};

typedef struct chunk
{
    void *addr;
    size_t size;
    enum chunk_type flags;
    int canary;
    struct chunk *next;
    struct chunk *prev;
} s_chunk;




void    *my_malloc(size_t size);
void    my_free(void *ptr);
void    *my_calloc(size_t nmemb, size_t size);
void    *my_realloc(void *ptr, size_t size);

#endif
