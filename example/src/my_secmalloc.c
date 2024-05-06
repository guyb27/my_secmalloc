#define _GNU_SOURCE
#include "my_secmalloc.private.h"
#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>

static int print_in_file(const char *func_call, const char *size, const char *addr)
{
    char text[100];
    const char *MSM_OUTPUT = getenv("MSM_OUTPUT");
    if (MSM_OUTPUT)
    {
        memset(text, 0, sizeof(text));
        strcpy(text, func_call);
        strncat(text, size, 16);
        strncat(text, addr, 16);
        int fd = open(MSM_OUTPUT, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (fd == -1) {
            perror("Erreur ouverture fichier");
            return 1; // Retourner avec erreur
        }
        ssize_t bytes_written = write(fd, text, strlen(text));
        if (bytes_written == -1) {
            perror("Erreur lors de l'écriture dans le fichier");
            close(fd);
            return 1;
        }
        if (close(fd) == -1)
        {
            perror("Erreur lors de la fermeture du fichier");
            return 1;
        }
    }
    return 0;
}

void    *my_malloc(size_t size)
{
    (void) size;
    return NULL;
}
void    my_free(void *ptr)
{
    (void) ptr;
}
void    *my_calloc(size_t nmemb, size_t size)
{
    (void) nmemb;
    (void) size;
    return NULL;
}

void    *my_realloc(void *ptr, size_t size)
{
    (void) ptr;
    (void) size;
    return NULL;

}

#ifdef DYNAMIC
void    *malloc(size_t size)
{
    return my_malloc(size);
}
void    free(void *ptr)
{
    my_free(ptr);
}
void    *calloc(size_t nmemb, size_t size)
{
    return my_calloc(nmemb, size);
}

void    *realloc(void *ptr, size_t size)
{
    return my_realloc(ptr, size);

}

#endif
