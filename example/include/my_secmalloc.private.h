#ifndef __MY_SECMALLOC_PRIVATE_H
#define __MY_SECMALLOC_PRIVATE_H
#include "my_secmalloc.h"
#include <stddef.h>

#define CHUNK_SIZE sizeof(struct s_chunk)
#define PAGE_SIZE 4096

#define BASE_ADDR PAGE_SIZE

//#define TOTAL_SIZE SMALL_TOTAL_SIZE+TINY_TOTAL_SIZE+LARGE_CHUNK_SIZE


#define BASE_METADATA_ADDR PAGE_SIZE
#define POOL_METADATA_SIZE PAGE_SIZE*(10000)
#define BASE_MEMORY_ADDR BASE_METADATA_ADDR+POOL_METADATA_SIZE
#define POOL_MEMORY_SIZE PAGE_SIZE*100000
#define CANARY_SIZE 16


#define FIXED_CANARY 0xDEADBEEFDEADBEEF//8 BYTES

enum chunk_availlability {
    FREE = 0,
    BUSY = 1
};

typedef struct s_chunk {
    void *memory_addr;//Adresse de la memoire des data de cette metadata
    size_t memory_size;//Taille de l espace des data pour cette metadata
    enum chunk_availlability flag;
    size_t canary;
    size_t memory_mmap_number;//Numero de mmap pour les data
    struct s_chunk *prev;
    struct s_chunk *next;
} t_chunk;

typedef struct s_mmap {
    void *addr;//addresse de retour du mmap pour les data
    size_t size;//taille du mmap
    t_chunk *chunk;//debut des metadata
    struct s_mmap *prev;//Prochain mmap pour les datas
    struct s_mmap *next;//mmap precedent pour les datas
} t_mmap;

//t_mmap *s_mmap = NULL;
extern t_mmap *my_mmap;
extern t_chunk *my_t_chunk;


#endif
