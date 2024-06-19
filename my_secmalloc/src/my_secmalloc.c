#define _GNU_SOURCE

#include <sys/mman.h>
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "my_secmalloc.h"
#include "my_secmalloc.private.h"
#include <stdint.h>


HeapMetadataInfos meta_head = NULL;
//meta_head = NULL;
void * datapool_ptr = NULL;
size_t data_size = POOL_MEMORY_SIZE;
size_t meta_size = POOL_METADATA_SIZE;
size_t malloc_number = 1;

void * next_meta_ptr = NULL;
int log_fd = - 1;

size_t            next_hexa_base(size_t size)
{
    int base = 16;
    return (size % base ? size + base - (size % base) : size);
}

void init_log()
{
    const char * log_filename = getenv("MSM_OUTPUT");

    if (log_fd != - 1)
        return;
    if ( ! log_filename)
        log_filename = "default_log.txt";
    log_fd = open(log_filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (log_fd == - 1)
    {
        perror("Failed to open log file");
        exit(EXIT_FAILURE);
    }
}

void my_log(const char *format, ...)
{
    //return;
    va_list args;
    char * buffer;
    size_t buffer_size;

    if (log_fd == - 1)
        init_log();
    va_start(args, format);
    buffer_size = vsnprintf(NULL, 0, format, args);
    va_end(args);
    buffer = alloca(buffer_size + 2);
    va_start(args, format);
    vsnprintf(buffer, buffer_size + 2, format, args);
    va_end(args);
    write(log_fd, buffer, buffer_size);
}

void check_memory_leaks()
{
    HeapMetadataInfos current_meta = meta_head;

    my_log("== BEGIN CHECK FOR MEMORY LEAK ==\n");
    while (current_meta)
    {
        if (current_meta->state == BUSY)
        {
            my_log("Memory leak detected at address %p. Auto-freeing.\n", current_meta->data_ptr);
            my_free(current_meta->data_ptr);
        }
        current_meta = current_meta->next;
    }
    my_log("== END CHECK FOR MEMORY LEAK ==\n");
}

unsigned int generate_random_uint()
{
    int fd = open("/dev/urandom", O_RDONLY);
    unsigned int ui64_random_val;

    if (fd == - 1)
    {
        my_log("[ERROR] - Unable to open /dev/urandom!\n");
        close(fd);
        exit(EXIT_FAILURE);
    }
    if (lseek(fd, - 4, SEEK_END) == - 1)
    {
        my_log("[ERROR] - Unable to call lseek!\n");
        close(fd);
        exit(EXIT_FAILURE);
    }
    if (read(fd, & ui64_random_val, sizeof (ui64_random_val)) == - 1)
    {
        my_log("[ERROR] - Unable to read /dev/urandom!\n");
        close(fd);
        exit(EXIT_FAILURE);
    }
    close(fd);
    return ui64_random_val;
}

int init_my_malloc()
{
    size_t next_mmap_addr = BASE_ADDR;

    //atexit(check_memory_leaks);
    my_log("[INFO] - Initializing data and metadata pools.\n");
    meta_head = mmap(
        (void*)(next_mmap_addr), 
        meta_size, 
        PROT_READ | PROT_WRITE, 
        MAP_PRIVATE | MAP_ANON, 
         - 1, 
        0
    );
    if (meta_head == MAP_FAILED)
    {
        my_log("[ERROR] - Failed to map metadata pool: %d\n", errno);
        return 1;
    }
    next_meta_ptr = meta_head+ sizeof (Heap_Metadata_Infos);
    next_mmap_addr += meta_size;
    my_log("[INFO] - Data pool created at address %p\n", meta_head);
    datapool_ptr = mmap(
        (void*)(next_mmap_addr), 
        POOL_MEMORY_SIZE, 
        PROT_READ | PROT_WRITE, 
        MAP_PRIVATE | MAP_ANON, 
         - 1, 
        0
    );
    if (datapool_ptr == MAP_FAILED)
    {
        my_log("[ERROR] - Failed to map data pool: %d\n", errno);
        return 1;
    }
    my_log("[INFO] - Metadata pool created at address %p\n", datapool_ptr);
    meta_head->size = POOL_MEMORY_SIZE;
    //meta_head->total_size = meta_head->size + CANARY_SIZE;
    meta_head->next = NULL;
    meta_head->prev = NULL;
    meta_head->state = FREE;
    meta_head->data_ptr = datapool_ptr;
    meta_head->i64_canary = 0;
    my_log("[INFO] - First metadata element created at address %p.\n", meta_head);
    my_log("[INFO] - Data and metadata pools initialization complete.\n");
    return 0;
}

int add_heap_metadata_segment(HeapMetadataInfos current_meta, int size)
{
    HeapMetadataInfos next_struct;
    size_t size_with_canary = size + CANARY_SIZE;

    my_log("[INFO] - Add metadata segment at %p.\n", current_meta);
    next_struct = (HeapMetadataInfos)(next_meta_ptr);
    next_meta_ptr += sizeof (Heap_Metadata_Infos);
    next_struct->size = current_meta->size - size_with_canary;
    //next_struct->total_size = next_struct->size + CANARY_SIZE;
    current_meta->size = size_with_canary;
    //current_meta->total_size = size + CANARY_SIZE;
    next_struct->data_ptr = (char*)current_meta->data_ptr + size_with_canary;
    next_struct->state = FREE;
    current_meta->state = BUSY;
    next_struct->next = current_meta->next;
    current_meta->next = next_struct;
    next_struct->prev = current_meta;
    if (next_struct->next != NULL)
        next_struct->next->prev = next_struct;
    current_meta->i64_canary = generate_random_uint();
    //current_meta->i64_canary = 0x11111111;
    //*((int* )((char*)current_meta->data_ptr + size)) = current_meta->i64_canary;
    //my_log("[INFO] - BEFORE WRITE CANARY\n");
    //my_log("[INFO] - BEFORE WRITE CANARY [%zx]\n", *((size_t *)((char *)current_meta->data_ptr + size)));

    //my_log("[INFO] - data-pool start at [0x%.8zx], end at [0x%.8zx]\n", datapool_ptr, datapool_ptr + data_size);
    //my_log("[INFO] - return data address at [+%zu] [0x%.8zx], size: [%zu]\n", current_meta->data_ptr - datapool_ptr, current_meta->data_ptr, current_meta->size);
    *((long *)((char *)current_meta->data_ptr + size)) = current_meta->i64_canary;
    //my_log("[INFO] - AFTER WRITE CANARY.\n");
    //my_log("[INFO] - END Add metadata segment at %p.\n", current_meta);
    return 0;
}

Heap_Metadata_Infos * create_a_new_metadata_struct(Heap_Metadata_Infos *current_meta)
{
    HeapMetadataInfos tmp_meta = meta_head;

    my_log("[INFO] - ");
    my_log(
        "Metadata pool need more space, growing data pool at %p from %zu bytes to %zu bytes.\n", 
        meta_head, meta_size, meta_size + POOL_METADATA_SIZE
    );
    meta_head = mremap(meta_head, meta_size, meta_size + POOL_METADATA_SIZE, MREMAP_MAYMOVE);
    if (meta_head == MAP_FAILED)
    {
        my_log("[ERROR] - Attempt to use mremap to expand metadata pool failed: %d\n", errno);
        exit(EXIT_FAILURE);
    }
    if (tmp_meta != meta_head)
    {
        memcpy(tmp_meta, meta_head, meta_size);
        current_meta = meta_head + (current_meta - tmp_meta);
    }
    meta_size += POOL_METADATA_SIZE;
    tmp_meta = current_meta + sizeof (Heap_Metadata_Infos);
    current_meta->next = tmp_meta;
    tmp_meta->prev=current_meta;
    tmp_meta->next=NULL;
    tmp_meta->i64_canary = 0x0;
    tmp_meta->state = FREE;
    tmp_meta->size = 0x0;
    //tmp_meta->total_size = 0x0 + CANARY_SIZE;
    tmp_meta->data_ptr = 0x0;
    return tmp_meta;
}

Heap_Metadata_Infos * get_new_data_memory(Heap_Metadata_Infos * current_meta, size_t size)
{
    HeapMetadataInfos tmp_meta = meta_head;
    void * tmp_datapool = NULL;

    while (size + CANARY_SIZE > current_meta->size)
    {
        my_log("[INFO] - Checking data pool size to know if it needs to grows.\n");
        size_t new_datapool_len = data_size + POOL_MEMORY_SIZE;
        my_log("[INFO] - ");
        my_log(
            "Data pool need more space, growing data pool at %p from %zu bytes to %zu bytes.\n", 
            datapool_ptr, 
            data_size, 
            new_datapool_len
        );
        tmp_datapool = datapool_ptr;
        datapool_ptr = mremap(datapool_ptr, data_size, new_datapool_len, MREMAP_MAYMOVE);
        if (datapool_ptr == MAP_FAILED)
        {
            my_log("[ERROR] - Attempt to use mremap to expand data pool failed: %d\n", errno);
            return NULL;
        }
        if (tmp_datapool != datapool_ptr)
        {
            memcpy(tmp_datapool, datapool_ptr, data_size);
            //Update data's addresses in metadata structs
            while (tmp_meta)
            {
                tmp_meta->data_ptr = (tmp_meta->data_ptr - tmp_datapool) + datapool_ptr;
                tmp_meta = tmp_meta->next;
            }
        }
        data_size += POOL_MEMORY_SIZE;
        current_meta->size += POOL_METADATA_SIZE;
        //current_meta->total_size += POOL_METADATA_SIZE;
    }
    return current_meta;
}

Heap_Metadata_Infos * get_free_metadata_struct(size_t size)
{
    Heap_Metadata_Infos * current_meta = meta_head;
    size_t struct_number=0;

    my_log("[INFO] - Looking for free metadata area to allocate.\n");
    while (current_meta && current_meta->next)
    {
        if (current_meta->state == FREE && current_meta->size >= size + CANARY_SIZE)
        {
            my_log("[INFO] - ");
            my_log(
                "Metadata at %p for data at %p in datapool is free with %zu bytes availables.\n", 
                current_meta, current_meta->data_ptr, current_meta->size
            );
            break;
        }
        struct_number++;
        current_meta = current_meta->next;
    }
    if (current_meta->state == BUSY)
        current_meta = create_a_new_metadata_struct(current_meta);
    if (current_meta->size < size + CANARY_SIZE)
        current_meta = get_new_data_memory(current_meta, size);
    //my_log("[INFO] - Struct number [%zu] found\n", struct_number);
    //my_log("data_ptr: [%.8zx], size: [%zu], state: [%s], canary: [%zx], next: [%s]\n",
    //    current_meta->data_ptr, current_meta->size, current_meta->state == FREE ? "FREE" : "BUSY", (size_t)current_meta->i64_canary, current_meta->next ? "Y" : "N"
    //);
    return current_meta;
}

void* my_malloc(size_t size)
{
    size_t actual_malloc_number = malloc_number;
    malloc_number++;
    HeapMetadataInfos current_meta = NULL;
    size_t real_size = size;

    my_log("== BEGIN MALLOC [%zu] ==\n", actual_malloc_number);
    my_log("[INFO] - MALLOC called with size %zu rounded to %zu.\n", real_size, size);
    if ((int) size <= 0)
    {
        my_log("[ERROR] - size doesn't possess a valid value.\n");
        return NULL;
    }
    size = next_hexa_base(size);
    if ( ! meta_head)
    {
        if (init_my_malloc())
        {
            my_log("[ERROR] - Unable to spawn a pool: %d\n", errno);
            exit(EXIT_FAILURE);
        }
        if (meta_head == NULL)
        {
            my_log("[ERROR] - Unable to read metadata: %d\n", errno);
            exit(EXIT_FAILURE);
        }
    }
    my_log("== meta_head ==\n");
    my_log("data_ptr: [%.8zx], size: [%zu], state: [%s], canary: [%zx], next: [%s]\n",
        meta_head->data_ptr, meta_head->size, meta_head->state == FREE ? "FREE" : "BUSY", (size_t)meta_head->i64_canary, meta_head->next ? "Y" : "N"
    );
    current_meta = get_free_metadata_struct(size);
    //Ca arrive jamais mais c est pour test:
    //if (!current_meta || !current_meta->data_ptr)
    //{
    //    my_log("[ERROR] - No struct found\n");
    //    return NULL;
    //}
    add_heap_metadata_segment(current_meta, size);
    my_log("[INFO] - data-pool start at [0x%.8zx], end at [0x%.8zx]\n", datapool_ptr, datapool_ptr + data_size);
    my_log("[INFO] - return data address at [+%zu] [0x%.8zx], size: [%zu]\n", current_meta->data_ptr - datapool_ptr, current_meta->data_ptr, current_meta->size);
    my_log("== END MALLOC [%zu] ==\n", actual_malloc_number);
    return current_meta->data_ptr;
}

void * secure_memset(void * v, int c, size_t n)
{
    volatile unsigned char * p = v;

    while (n --)
        * p ++ = c;
    return v;
}

void my_free(void * ptr)
{
    HeapMetadataInfos current_meta = meta_head;
    size_t number_free = 0;

    my_log("== BEGIN FREE ==\n");
    if (ptr == NULL)
    {
        my_log("[INFO] - Null pointer, cannot free.\n");
        my_log("== END FREE ==\n");
        return;
    }
    while (current_meta != NULL && current_meta->data_ptr != ptr)
    {
        current_meta = current_meta->next;
        number_free++;
    }
    if (current_meta == NULL)
    {
        my_log("[INFO] - Pointer %p not found in metadata pool.\n", ptr);
        my_log("== END FREE ==\n");
        return;
    }
    if (current_meta->state == FREE)
    {
        my_log("[INFO] - Double free detected for block at %p!\n", ptr);
        my_log("== END FREE ==\n");
        return;
    }
    my_log("[INFO] - Data found at [+%zu] [0x%.8zx], struct number [%zu]\n", current_meta->data_ptr - datapool_ptr, current_meta->data_ptr, number_free);
    long canary = * ((long * )((char * )current_meta->data_ptr + current_meta->size - CANARY_SIZE));
    if (current_meta->i64_canary != canary)
    {
        my_log("[ERROR] - Heap overflow detected at %p!\n", ptr);
        my_log("== END FREE ==\n");
        return;
    }
    // Clean the memory before free
    secure_memset(current_meta->data_ptr, 0, current_meta->size);
    current_meta->state = FREE;
    //current_meta->data_ptr = NULL;
    current_meta->i64_canary = 0x0;
    // Join next and prev free structs
    if (current_meta->next && current_meta->next->state == FREE)
    {
        current_meta->size += current_meta->next->size;
        //current_meta->total_size += current_meta->next->total_size;
        current_meta->next = current_meta->next->next;
        if (current_meta->next)
            current_meta->next->prev = current_meta;
    }
    if (current_meta->prev && current_meta->prev->state == FREE)
    {
        current_meta->prev->size += current_meta->size;
        //current_meta->prev->total_size += current_meta->total_size;
        current_meta->prev->next = current_meta->next;
        if (current_meta->next)
            current_meta->next->prev = current_meta->prev;
    }
    //my_log("[INFO] - Data found at [+%zu] [0x%.8zx], struct number [%zu]\n", current_meta->data_ptr - datapool_ptr, current_meta->data_ptr, number_free);
    my_log("== END FREE ==\n");
}

void *my_calloc(size_t nmemb, size_t size)
{
    size_t total_size = nmemb * size;
    void* ptr = my_malloc(total_size);

    my_log("== BEGIN CALLOC ==\n");
    if (ptr != NULL)
    {
        my_log("[INFO] - Initializing memory block with 0s at %p.\n", ptr);
        char * byte_ptr = (char*) ptr;
        for (size_t i = 0; i < size; i++)
            byte_ptr[i] = 0;
    }
    my_log("== END CALLOC ==\n");
    return ptr;
}

void *my_realloc(void *ptr, size_t size)
{
    void *new_ptr = NULL;
    HeapMetadataInfos metadata = NULL;
    HeapMetadataInfos new_metadata = NULL;

    my_log("== BEGIN REALLOC ==\n");
    if (ptr == NULL)
    {
        my_log("[INFO] - NULL pointer (%p).\n");
        my_log("[INFO] - MALLOC used with size %zu.\n", size);
        new_ptr = my_malloc(size);
        return new_ptr;
    }
    else if (size == 0)
    {
        my_log("[INFO] - Size is 0, pointer freed.\n");
        my_free(ptr);
        return NULL;
    }
    // search for the metadata associated with this pointer
    metadata = meta_head;
    while (metadata && metadata->data_ptr != ptr)
        metadata = metadata->next;
    // check that the pointer passed as a parameter is valid
    if (metadata == NULL)
    {
        my_log("[ERROR] Invalid pointer (%p) given to realloc. realloc aborted!\n", ptr);
        return NULL;
    }
    if (metadata->size == size + CANARY_SIZE)
    {
        my_log("[INFO] metadata->size == size + CANARY_SIZE\n");
        return metadata->data_ptr;
    }
    size_t next_free_size = 0;
    if (metadata->next && metadata->next->state == FREE)
        next_free_size = metadata->next->size;
    if (metadata->size + next_free_size >= size + CANARY_SIZE)
    {
        my_log("[INFO] metadata->size - CANARY_SIZE + metadata->next->size <= size + CANARY_SIZE\n");
        secure_memset(metadata->data_ptr+metadata->size-CANARY_SIZE, 0, CANARY_SIZE);
        //secure_memset(metadata->data_ptr+metadata->size+CANARY_SIZE, 0, CANARY_SIZE+metadata->next->size);
        metadata->size += next_free_size;
        if (metadata->size - (size + CANARY_SIZE) > 0 && metadata->next && metadata->next->state == FREE)
        {
            //Si il reste de la place et qu'il y a un maillon->next qui est free
            my_log("[INFO] next struct is FREE and is still free memory\n");
            metadata->next->size = metadata->size - size - CANARY_SIZE;
        }
        else if (metadata->size - (size + CANARY_SIZE) == 0 && metadata->next && metadata->next->state == FREE)
        //Si il n y a plus de place et qu'il y a un maillon->next qui est free
        {
            my_log("[INFO] next struct is FREE and is not still free memory\n");
            metadata->next = metadata->next->next;
            if (metadata->next)
                    metadata->next->prev = metadata;
        }
        else if (metadata->size - size > 0)
        //Si il reste de la place et qu'il n'y a pas de maillon->next qui est free
        {
            my_log("[INFO] next struct is BUSY and is still free memory \n");
            //AJOUT D UN MAILLON FREE
            new_metadata = next_meta_ptr;
            next_meta_ptr += sizeof(Heap_Metadata_Infos);
            new_metadata->next = metadata->next;
            new_metadata->prev = metadata;
            if (metadata->next)
                metadata->next->prev = new_metadata;
            metadata->next = new_metadata;
            new_metadata->data_ptr=metadata->data_ptr+size + CANARY_SIZE;
            new_metadata->size=metadata->size - (size + CANARY_SIZE);
            new_metadata->state = FREE;
            new_metadata->i64_canary = 0x0;
        }
        metadata->size = size + CANARY_SIZE;
        *((long *)((char *)metadata->data_ptr + size)) = metadata->i64_canary;
        new_ptr = metadata->data_ptr;
    }
    else
    {
        my_log("[INFO] Not enouht space in the last FREE metadata structure, a new malloc is call with a memcpy\n");
        new_ptr = my_malloc(size);
        if(new_ptr == NULL) 
        {
            my_log("[ERROR] - my_realloc don't memcpy the data and return a NULL pointer\n", ptr);
            my_free(ptr);
            return NULL;
        }
        // Copy old data from old address to new address
        char *data = (char*) metadata->data_ptr;
        char *new_data = (char*) new_ptr;
        memcpy(new_data, data, metadata->size);
        // We free the memory located at the old address
        my_log("[INFO] - Memory located to old address %p is freed.\n", ptr);
        my_free(ptr);
    }
    my_log("== END REALLOC ==\n");
    return new_ptr;
}
