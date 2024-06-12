#ifndef _MY_SECMALLOC_H
#define _MY_SECMALLOC_H

#include <stdlib.h>

#ifdef __GNUC__
#define EXPORT __attribute__((visibility("default")))
#else
#define EXPORT
#endif

// Sélection entre les fonctions dynamiques (privées) et les fonctions de test (publiques)
#if DYNAMIC
    // Fonctions dynamiques (privées)
    #define PRIVATE static
    #define PUBLIC EXPORT
#else
    // Fonctions de test (publiques)
    #define PRIVATE
    #define PUBLIC EXPORT
#endif

// Déclarations des fonctions personnalisées
PRIVATE void *my_malloc(size_t size);
PRIVATE void my_free(void *ptr);
PRIVATE void *my_calloc(size_t nmemb, size_t size);
PRIVATE void *my_realloc(void *ptr, size_t size);

// Déclarations des fonctions standard, toujours publiques
PUBLIC void *malloc(size_t size);
PUBLIC void free(void *ptr);
PUBLIC void *calloc(size_t nmemb, size_t size);
PUBLIC void *realloc(void *ptr, size_t size);

#endif
