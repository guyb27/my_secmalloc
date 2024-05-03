#ifndef _SECMALLOC_PRIVATE_H
#define _SECMALLOC_PRIVATE_H

#include "my_secmalloc.h"

// sentez vous libre de modifier ce header comme vous le souhaitez

void    *my_malloc(size_t size);
void    my_free(void *ptr);
void    *my_calloc(size_t nmemb, size_t size);
void    *my_realloc(void *ptr, size_t size);

#endif
