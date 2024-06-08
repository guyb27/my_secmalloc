//my_alloc.private.h
#ifndef __MY_SECMALLOC_PRIVATE_H
#define __MY_SECMALLOC_PRIVATE_H
#include "my_secmalloc.h"
#include <stddef.h>

#define CHUNK_SIZE sizeof(struct s_chunk)
#define PAGE_SIZE 4096

#define TINY_CHUNK_SIZE CHUNK_SIZE*1000
#define TINY_MEMORY_SIZE PAGE_SIZE*1000
#define TINY_TOTAL_SIZE TINY_MEMORY_SIZE+TINY_CHUNK_SIZE

#define SMALL_CHUNK_SIZE CHUNK_SIZE*1000
#define SMALL_MEMORY_SIZE PAGE_SIZE*1000
#define SMALL_TOTAL_SIZE SMALL_MEMORY_SIZE+SMALL_CHUNK_SIZE

#define LARGE_CHUNK_SIZE CHUNK_SIZE*1000

#define TOTAL_SIZE SMALL_TOTAL_SIZE+TINY_TOTAL_SIZE+LARGE_CHUNK_SIZE



#define BASE_CHUNK_ADDR PAGE_SIZE
#define INIT_CHUNK_SIZE PAGE_SIZE*(100000)
#define BASE_MEMORY_ADDR BASE_CHUNK_ADDR+INIT_CHUNK_SIZE
#define INIT_MEMORY_SIZE PAGE_SIZE*100000
#define TOTAL_MEMORY_SIZE INIT_CHUNK_SIZE+INIT_MEMORY_SIZE
#define CANARY_SIZE 16

# define TINY_MAX 16
# define SMALL_MAX 128

enum chunk_availlability
{
    FREE=0,
    BUSY=1
};

enum chunk_type
{
    TINY=0,
    SMALL=1,
    LARGE=2
};

typedef struct			s_chunk
{
	void *addr;
	size_t size;
    void *base_addr;
    enum chunk_availlability flag;
    enum chunk_type type;
    int canary;
	struct s_chunk *prev;
	struct s_chunk *next;
}						t_chunk;

typedef struct
{
    int init;
	t_chunk *tiny;
	t_chunk *small;
	t_chunk *large;
} t_heap;

//t_heap heap = {0,0,0,0};

//struct chunk *init_heap();
//struct chunk *get_free_chunk(size_t s);

#endif

/*
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
*/