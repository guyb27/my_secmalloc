#ifndef _MY_SECMALLOC_H
#define _MY_SECMALLOC_H

#include <stdlib.h>

// Define visibility for functions based on whether DYNAMIC is defined
#ifdef DYNAMIC
    void *malloc(size_t size);
    void free(void *ptr);
    void *calloc(size_t nmemb, size_t size);
    void *realloc(void *ptr, size_t size);

    // Alias the custom functions to standard names when DYNAMIC is defined
    #define my_malloc malloc
    #define my_free free
    #define my_calloc calloc
    #define my_realloc realloc
#else
    // Define the custom memory allocation functions for testing
    void *my_malloc(size_t size);
    void my_free(void *ptr);
    void *my_calloc(size_t nmemb, size_t size);
    void *my_realloc(void *ptr, size_t size);
#endif

#endif
