#ifndef _MY_SECMALLOC_PRIVATE_H
#define _MY_SECMALLOC_PRIVATE_H

#define META_SIZE sizeof(struct Heap_Metadata_Infos)
#define CANARY_SIZE sizeof(int)

typedef enum {
    FREE,
    BUSY
} MemoryState;

typedef struct Heap_Metadata_Infos {
    void *data_ptr;//Addresse de la zone memoire associe a cette structure
    size_t size;//taille de la zone memoire associee a cette structure
    MemoryState state;//LIBRE OU OCCUPEE
    int i64_canary;//CANARY DE SECURITE ANTI-BUFFER OVERFLOW POUR LES NEWBIE
    size_t memory_mmap_number;//NUMERO DE CALL A MMAP AFIN D IDENTIFIER FACILEMENT LA STRUCTURE t_mmap_info (desole d avoir crier, CAPSLOCK)
    struct Heap_Metadata_Infos *prev;
    struct Heap_Metadata_Infos *next;
} Heap_Metadata_Infos, *HeapMetadataInfos;

typedef struct s_mmap_info
{
    void *addr;//addresse de retour du mmap pour les data
    size_t size;//taille du mmap
    Heap_Metadata_Infos *meta_head;//debut des metadata
    struct s_mmap_info *prev;//Prochain mmap pour les datas
    struct s_mmap_info *next;//mmap precedent pour les datas
} t_mmap_info;



void my_log(const char *format, ...);
int init_my_malloc();
int add_heap_metadata_segment(HeapMetadataInfos current_metadata, int size);
void check_memory_leaks();
unsigned int generate_random_uint();

#endif
