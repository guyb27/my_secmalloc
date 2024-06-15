#ifndef _MY_SECMALLOC_PRIVATE_H
#define _MY_SECMALLOC_PRIVATE_H

//#define DYNAMIC true

#define META_SIZE sizeof(struct Heap_Metadata_Infos)
#define CANARY_SIZE sizeof(int)

#define PAGE_SIZE 4096

#define BASE_ADDR PAGE_SIZE

#define POOL_METADATA_SIZE PAGE_SIZE*(10000)
//#define POOL_METADATA_SIZE PAGE_SIZE*(100)
#define POOL_MEMORY_SIZE PAGE_SIZE*100000
//#define POOL_MEMORY_SIZE PAGE_SIZE*1000

typedef enum {
    FREE,
    BUSY
} MemoryState;

typedef struct Heap_Metadata_Infos {
    void *data_ptr;//Addresse de la zone memoire associe a cette structure
    size_t size;//taille de la zone memoire associee a cette structure
    MemoryState state;//LIBRE OU OCCUPEE
    long i64_canary;//CANARY DE SECURITE ANTI-BUFFER OVERFLOW POUR LES NEWBIE
    size_t memory_mmap_number;//NUMERO DE CALL A MMAP AFIN D IDENTIFIER FACILEMENT LA STRUCTURE t_mmap_info (desole d avoir crier, CAPSLOCK)
    struct Heap_Metadata_Infos *prev;
    struct Heap_Metadata_Infos *next;
} Heap_Metadata_Infos, *HeapMetadataInfos;

void my_log(const char *format, ...);
int init_my_malloc();
int add_heap_metadata_segment(HeapMetadataInfos current_metadata, int size);
void check_memory_leaks();
unsigned int generate_random_uint();

#endif
