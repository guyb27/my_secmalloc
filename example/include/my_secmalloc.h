//my_alloc.h
#ifndef __MY_SECMALLOC_H
#define __MY_SECMALLOC_H

#include <stddef.h>

#define _GNU_SOURCE
#include <sys/mman.h>

#include <stdio.h>

void *my_malloc(size_t);
void my_free(void *ptr);

#endif

/*
#ifndef _SECMALLOC_H
#define _SECMALLOC_H

#include <stddef.h>

void    *malloc(size_t size);
void    free(void *ptr);
//void    *calloc(size_t nmemb, size_t size);
//void    *realloc(void *ptr, size_t size);

#endif
*/