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

// GLOBALS
static HeapMetadataInfos meta_head = NULL;
static void *datapool_ptr = NULL;
static size_t page_size;
static size_t meta_size;
static int log_fd = -1;

size_t next_mmap_addr = BASE_ADDR;

bool b_isBusy = false;

size_t            next_hexa_base(size_t size)
{
    return (size % 16 ? size + 16 - (size % 16) : size);
}

void init_log()
{
    if (log_fd != -1) {
        return; // Log already initialized
    }
    
    const char *log_filename = getenv("MSM_OUTPUT");
    if (!log_filename)
    {
        log_filename = "default_log.txt";
    }
    log_fd = open(log_filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (log_fd == -1)
    {
        perror("Failed to open log file");
        exit(EXIT_FAILURE);
    }
}

void my_log(const char *format, ...)
{
    if (log_fd == -1)
    {
        init_log();
    }

    va_list args;
    char *buffer;

    va_start(args, format);
    size_t buffer_size = vsnprintf(NULL, 0, format, args);
    va_end(args);

    buffer = alloca(buffer_size + 2);
    va_start(args, format);
    vsnprintf(buffer, buffer_size + 2, format, args);
    va_end(args);

    write(log_fd, buffer, buffer_size);
}

void check_memory_leaks()
{
    my_log("== BEGIN CHECK FOR MEMORY LEAK ==\n");
    HeapMetadataInfos current_meta = meta_head;
    while (current_meta != NULL)
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
    if (fd == -1) {
        my_log("[ERROR] - Unable to open /dev/urandom!\n");
        exit(EXIT_FAILURE);
    }

    unsigned int ui64_random_val;
    if (lseek(fd, -4, SEEK_END) == -1) {
        my_log("[ERROR] - Unable to call lseek!\n");
        exit(EXIT_FAILURE);
    }

    if (read(fd, &ui64_random_val, sizeof(ui64_random_val)) == -1) {
        my_log("[ERROR] - Unable to read /dev/urandom!\n");
        exit(EXIT_FAILURE);
    }

    close(fd);
    return ui64_random_val;
}

int init_my_malloc()
{
    int i64_page_amount = 400;
    page_size = getpagesize() * i64_page_amount;
    meta_size = META_SIZE * 100000;

    atexit(check_memory_leaks);

    my_log("[INFO] - Initializing data and metadata pools.\n");

    meta_head = mmap((void*)(next_mmap_addr), POOL_METADATA_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if (meta_head == MAP_FAILED)
    {
        my_log("[ERROR] - Failed to map metadata pool: %d\n", errno);
        return 1;
    }
    next_mmap_addr+=meta_size;
    my_log("[INFO] - Data pool created at address %p\n", meta_head);

    datapool_ptr = mmap((void*)(next_mmap_addr), POOL_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if (datapool_ptr == MAP_FAILED)
    {
        my_log("[ERROR] - Failed to map data pool: %d\n", errno);
        return 1;
    }
    next_mmap_addr+=page_size;
    my_log("[INFO] - Metadata pool created at address %p\n", datapool_ptr);

    // Filling our chunk
    meta_head->size = page_size;
    meta_head->next = NULL;
    meta_head->prev = NULL;
    meta_head->state = BUSY;
    meta_head->data_ptr = datapool_ptr;
    meta_head->i64_canary = 0;

    my_log("[INFO] - First metadata element created at address %p.\n", meta_head);
    my_log("[INFO] - Data and metadata pools initialization complete.\n");

    return 0;
}

int add_heap_metadata_segment(HeapMetadataInfos new_current_meta, int size) {
    my_log("[INFO] - Add metadata segment at %p.\n", new_current_meta);
    HeapMetadataInfos data_left;
    
    data_left = (HeapMetadataInfos)((char*)new_current_meta + META_SIZE);

    data_left->size = new_current_meta->size - (size + CANARY_SIZE);

    new_current_meta->size = size + CANARY_SIZE;

    data_left->data_ptr = (char*)new_current_meta->data_ptr + size + CANARY_SIZE;

    data_left->state = BUSY;
    new_current_meta->state = FREE;

    // We update the pointers of our metadata blocks
    data_left->next = new_current_meta->next;
    if (new_current_meta->next != NULL)
        new_current_meta->next->prev = data_left;
    new_current_meta->next = data_left;
    data_left->prev = new_current_meta;

    new_current_meta->i64_canary = generate_random_uint();
    *((int*)((char*)new_current_meta->data_ptr + size)) = new_current_meta->i64_canary;
    
    return 0;
}

void* my_malloc(size_t size)
{
  	size = next_hexa_base(size); // alignement sur 16 octets pour éviter les erreurs de segmentation sur les architectures 64 bits

    bool b_isIterating;

    my_log("== BEGIN MALLOC ==\n");
    my_log("[INFO] - MALLOC called with size %zu.\n", size);

    if ((int) size <= 0)
    {
        my_log("[ERROR] - size doesn't possess a valid value.\n");
        return NULL;
    }

    // if our data pool and our metadata pool is not yet created, then we create it before using malloc
    if (b_isBusy == false)
    {
        int res = init_my_malloc();
        if (res != 0)
        {
            my_log("[ERROR] - Unable to spawn a pool: %d\n", errno);
            exit(EXIT_FAILURE);
        }

        b_isBusy = true;
    }

    if (meta_head == NULL)
    {
        my_log("[ERROR] - Unable to read metadata: %d\n", errno);
        exit(EXIT_FAILURE);
    }

    // Looking for a free metadata to allocate the new zone
    my_log("[INFO] - Looking for free metadata area to allocate.\n");
    
    HeapMetadataInfos current_meta = meta_head;
    b_isIterating = true;
    size_t meta_segments_number = 0;
    
    while (b_isIterating == true)
    {
        meta_segments_number++;

        if (current_meta->state && current_meta->size >= (size + CANARY_SIZE))
        {
            my_log("[INFO] - Metadata at %p for data at %p in datapool is free with %zu bytes availables.\n", current_meta, current_meta->data_ptr, current_meta->size);

            // split memory if necessary
            if (current_meta->size > (size + CANARY_SIZE))
                add_heap_metadata_segment(current_meta, size);
            
            my_log("== END MALLOC ==\n");
            // Return a pointer to the allocated area (after the metadata)
            return current_meta->data_ptr;
        }
        
        if (current_meta->next != NULL && current_meta->next != current_meta)
            current_meta = current_meta->next;
        else
            b_isIterating = false;
    }
    my_log("[INFO] - No available existing block found.\n");

    my_log("[INFO] - Checking data pool size to know if it needs to grows.\n");
    if (current_meta->size < size)
    {
        size_t new_datapool_len = page_size + getpagesize();
        my_log("[INFO] - Data pool need more space, growing data pool at %p from %zu bytes to %zu bytes.\n", datapool_ptr, page_size, new_datapool_len);
        datapool_ptr = mremap(datapool_ptr, page_size, new_datapool_len, MREMAP_MAYMOVE);
        if (datapool_ptr == MAP_FAILED)
        {
            my_log("[ERROR] - Attempt to use mremap to expand data pool failed: %d\n", errno);
            return NULL;
        }
        if (datapool_ptr + page_size > next_mmap_addr)
            next_mmap_addr = datapool_ptr + page_size;
        page_size = page_size + new_datapool_len;
    }

    // expand the metadata pool if it is too small to access a new entry
    my_log("[INFO] - Checking metadata pool size to know if it needs to grows.\n");
    if (meta_segments_number >= meta_size)
    {
        my_log("[INFO] - Metadata pool need more space, growing data pool at %p from %zu bytes to %zu bytes.\n", meta_head, meta_size, meta_size + (meta_size*1000));
        //meta_head = mremap(meta_head, meta_size, meta_size + (meta_size*1000), MREMAP_MAYMOVE);
        meta_head = mremap(meta_head, meta_size, meta_size + (meta_size*1000), MREMAP_FIXED);
        if (meta_head == MAP_FAILED)
        {
            my_log("[ERROR] - Attempt to use mremap to expand metadata pool failed: %d\n", errno);
            return NULL;
        }
        if (datapool_ptr + page_size > next_mmap_addr)
            next_mmap_addr = datapool_ptr + page_size;
        meta_size = meta_size + (meta_size * 1000);
    }
    add_heap_metadata_segment(current_meta, size);

    my_log("== END MALLOC ==\n");
    return current_meta->data_ptr;
}

void *secure_memset(void *v, int c, size_t n) {
    volatile unsigned char *p = v;
    while (n--) {
        *p++ = c;
    }
    return v;
}

void my_free(void* ptr) {
    my_log("== BEGIN FREE ==\n");

    if (ptr == NULL) {
        my_log("[INFO] - Null pointer, cannot free.\n");
        my_log("== END FREE ==\n");
        return;
    }

    HeapMetadataInfos current_meta = meta_head;
    while (current_meta != NULL && current_meta->data_ptr != ptr)
        current_meta = current_meta->next;

    if (current_meta == NULL) {
        my_log("[INFO] - Pointer %p not found in metadata pool.\n", ptr);
        my_log("== END FREE ==\n");
        return;
    }

    if (current_meta->state == FREE) {
        my_log("[INFO] - Double free detected for block at %p!\n", ptr);
        my_log("== END FREE ==\n");
        return;
    }

    int *i64_canary_ptr = (int*)((char*)current_meta->data_ptr + current_meta->size - CANARY_SIZE);
    int i64_canary = *i64_canary_ptr;
    if (current_meta->i64_canary != i64_canary) {
        my_log("[ERROR] - Heap overflow detected at %p!\n", ptr);
        my_log("== END FREE ==\n");
        return;
    }

    // Effacer la mémoire avant de la libérer de manière sécurisée
    secure_memset(current_meta->data_ptr, 0, current_meta->size - CANARY_SIZE);

    current_meta->state = FREE;
    current_meta->data_ptr = NULL;
    
    // Combine adjacent free blocks
    if (current_meta->next != NULL && current_meta->next->state == FREE) {
        current_meta->size += current_meta->next->size;
        current_meta->next = current_meta->next->next;
        if (current_meta->next != NULL) {
            current_meta->next->prev = current_meta;
        }
    }

    if (current_meta->prev != NULL && current_meta->prev->state == FREE) {
        current_meta->prev->size += current_meta->size;
        current_meta->prev->next = current_meta->next;
        if (current_meta->next != NULL) {
            current_meta->next->prev = current_meta->prev;
        }
    }

    my_log("== END FREE ==\n");
}

void *my_calloc(size_t nmemb, size_t size)
{
    my_log("== BEGIN CALLOC ==\n");

    size_t i;
    size_t total_size = nmemb * size;
    void* ptr = my_malloc(total_size);
    if (ptr != NULL)
    {
        my_log("[INFO] - Initializing memory block with 0s at %p.\n", ptr);           
        char* byte_ptr = (char*) ptr;           
        for (i = 0; i < size; i++)
            byte_ptr[i] = 0;
    }
    my_log("== END CALLOC ==\n");

    return ptr;
}

void *my_realloc(void *ptr, size_t size)
{
    my_log("== BEGIN REALLOC ==\n");
    void *new_ptr = NULL;

    if(ptr == NULL)
    {
        my_log("[INFO] - NULL pointer (%p).\n", ptr);
        my_log("[INFO] - MALLOC used with size %zu.\n", size);
        new_ptr = my_malloc(size);
        if (new_ptr == NULL)
        {
            my_log("[ERROR] - Malloc failed.\n");
            return NULL;
        }
        my_log("== END REALLOC ==\n");
        return new_ptr;
    }
    else if(size == 0)
    {
        my_log("[INFO] - Size is 0, pointer freed (%p).\n", ptr);
        my_free(ptr);
        my_log("== END REALLOC ==\n");
        return NULL;
    }

    // Search for the metadata associated with this pointer
    HeapMetadataInfos metadata = meta_head;
    while (metadata != NULL && metadata->data_ptr != ptr)
        metadata = metadata->next;

    // Check that the pointer passed as a parameter is valid
    if (metadata == NULL)
    {
        my_log("[ERROR] - Invalid pointer (%p) given to realloc. Realloc aborted!\n", ptr);
        return NULL;
    }

    new_ptr = my_malloc(size);
    if(new_ptr == NULL)
    {
        my_log("[ERROR] - Malloc failed. Old pointer (%p) is not freed.\n", ptr);
        return NULL;
    }

    // Copy old data from old address to new address
    char *data = (char*) metadata->data_ptr;
    char *new_data = (char*) new_ptr;
    // If the new size is larger than the old size, the added memory will not be initialized
    size_t copy_size = (metadata->size < size) ? metadata->size : size;
    memcpy(new_data, data, copy_size);

    // Free the memory located at the old address
    my_log("[INFO] - Memory located at old address %p is freed.\n", ptr);
    my_free(ptr);

    my_log("== END REALLOC ==\n");
    return new_ptr;
}


#if DYNAMIC

PUBLIC void *malloc(size_t size) {
    my_log("[INFO] - malloc called with size %zu.\n", size);
    void *ptr = my_malloc(size);
    my_log("[INFO] - malloc returning pointer %p.\n", ptr);
    return ptr;
}

PUBLIC void free(void *ptr) {
    my_log("[INFO] - free called with pointer %p.\n", ptr);
    my_free(ptr);
    my_log("[INFO] - free completed for pointer %p.\n", ptr);
}

PUBLIC void *calloc(size_t nmemb, size_t size) {
    my_log("[INFO] - calloc called avec nmemb %zu et size %zu.\n", nmemb, size);
    void *ptr = my_calloc(nmemb, size);
    my_log("[INFO] - calloc returning pointer %p.\n", ptr);
    return ptr;
}

PUBLIC void *realloc(void *ptr, size_t size) {
    my_log("[INFO] - realloc called with pointer %p et size %zu.\n", ptr, size);
    void *new_ptr = my_realloc(ptr, size);
    my_log("[INFO] - realloc returning pointer %p.\n", new_ptr);
    return new_ptr;
}

#endif
