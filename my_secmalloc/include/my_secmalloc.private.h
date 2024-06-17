#ifndef MY_SECMALLOC_PRIVATE_H
#define MY_SECMALLOC_PRIVATE_H

#define META_SIZE next_hexa_base(sizeof(struct Heap_Metadata_Infos))
#define CANARY_SIZE sizeof(long)
#define PAGE_SIZE 4096
#define BASE_ADDR PAGE_SIZE*1000
#define POOL_METADATA_SIZE PAGE_SIZE*(1000)
#define POOL_MEMORY_SIZE PAGE_SIZE*10000

typedef enum {
    FREE,
    BUSY
} MemoryState;

typedef struct Heap_Metadata_Infos {
    void *data_ptr;//Addresse de la zone memoire associe a cette structure
    size_t size;//taille de la zone memoire associee a cette structure + le canary si la struct est free
    //size_t total_size;//taille de la zone memoire associee a cette structure + la taille du canary
    MemoryState state;//LIBRE OU OCCUPEE
    long i64_canary;//CANARY DE SECURITE ANTI-BUFFER OVERFLOW POUR LES NEWBIE
    struct Heap_Metadata_Infos *prev;
    struct Heap_Metadata_Infos *next;
} Heap_Metadata_Infos, *HeapMetadataInfos;

void my_log(const char *format, ...);
int init_my_malloc();
//int add_heap_metadata_segment(HeapMetadataInfos current_metadata, int size);
void check_memory_leaks();
unsigned int generate_random_uint();


extern size_t next_mmap_addr;
extern bool b_isBusy;
extern size_t page_size;
extern size_t meta_size;
extern int log_fd;
//extern HeapMetadataInfos meta_head = NULL;

#endif