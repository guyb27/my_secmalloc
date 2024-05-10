#include <criterion/criterion.h>
#include <stdio.h>
#include "my_secmalloc.private.h"
#include <sys/mman.h>

Test(mmap, simple) {
    void *ptr = mmap((void*)(4096), 4096, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    cr_expect(ptr != NULL);
    int res = munmap(ptr, 4096);
    cr_expect(res == 0);
}

Test(mmap, simple_malloc) {
    char *ptr1 = (char*)my_malloc(12);
    cr_assert(ptr1 != NULL, "Failed to alloc ptr1");
}


Test(simple, simple_map_01)
{
    // utilisation simple d'un mmap
    char *ptr = mmap((void*)(4096*100000), 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    cr_assert(ptr != NULL, "Failed to mmap");
}

Test(simple, simple_map_02)
{
    // utilisation simple d'un mmap
    char *ptr1 = (char*)my_malloc(12);
    cr_assert(ptr1 != NULL, "Failed to alloc ptr1");
    char *ptr2 = (char*)my_malloc(25);
    cr_assert(ptr2 != NULL, "Failed to alloc ptr2");
    //cr_assert(ptr1 != ptr2, "Failed to alloc ptr2 != ptr1");
    printf("ptr1: [%p]\nptr2: [%p]\nsizeof(struct chunk): [%lx]\n",
        &ptr1, &ptr2, CHUNK_SIZE
    );
    cr_assert((size_t)ptr2 == (size_t)ptr1 + 12 + CHUNK_SIZE,
        "Failed to alloc: %lx - %lx", (size_t)ptr2, (size_t)ptr1+12+CHUNK_SIZE); /* : %lx - %lx", (size_t)ptr2, (size_t)ptr1 + 12 + (sizeof (struct chunk)); */
    char *ptr3 = (char*)my_malloc(55);
    cr_assert((size_t)ptr3 == (size_t)ptr2 + 25 + CHUNK_SIZE,
        "Failed to alloc: %lx - %lx", (size_t)ptr3, (size_t)ptr2+25+CHUNK_SIZE);/*  : %lx - %lx", (size_t)ptr3, (size_t)ptr2 + 25 + (sizeof (struct chunk))); */
}

Test(simple, simple_map_03)
{
    // utilisation simple d'un mmap
    char *ptr1 = (char*)my_malloc(12);
    cr_assert(ptr1 != NULL, "Failed to alloc ptr1");
    char *ptr2 = (char*)my_malloc(25);
    cr_assert(ptr2 != NULL, "Failed to alloc ptr2");
    //cr_assert(ptr1 != ptr2, "Failed to alloc ptr2 != ptr1");
    printf("ptr1: [%p]\nptr2: [%p]\nsizeof(struct chunk): [%lx]\n",
        &ptr1, &ptr2, CHUNK_SIZE
    );
    cr_assert((size_t)ptr2 == (size_t)ptr1 + 12 + CHUNK_SIZE,
        "Failed to alloc: %lx - %lx", (size_t)ptr2, (size_t)ptr1+12+CHUNK_SIZE); /* : %lx - %lx", (size_t)ptr2, (size_t)ptr1 + 12 + (sizeof (struct chunk)); */
    char *ptr3 = (char*)my_malloc(55);
    cr_assert((size_t)ptr3 == (size_t)ptr2 + 25 + CHUNK_SIZE,
        "Failed to alloc: %lx - %lx", (size_t)ptr3, (size_t)ptr2+25+CHUNK_SIZE);/*  : %lx - %lx", (size_t)ptr3, (size_t)ptr2 + 25 + (sizeof (struct chunk))); */
    printf("my_free ptr2\n");
    my_free(ptr2);
    printf("my_free ptr3\n");
    my_free(ptr3);
    struct chunk *t = (struct chunk*)((size_t)ptr2 - CHUNK_SIZE);
    printf("t: %lu\n", t->size);
    //cr_assert((size_t)t->size==25+55+3940+2*sizeof(struct chunk), "Failed to my_free");
    dprintf(2, "SIZE: [%zu]\n", t->size);
    cr_assert((size_t)t->size==4096-12-(2*CHUNK_SIZE), "Failed to my_free");

}

Test(simple, simple_map_04)
{
    // utilisation simple d'un mmap
    char *ptr1 = (char*)my_malloc(12);
    cr_assert(ptr1 != NULL, "Failed to alloc ptr1");
    char *ptr2 = (char*)my_malloc(25);
    cr_assert(ptr2 != NULL, "Failed to alloc ptr2");
    //cr_assert(ptr1 != ptr2, "Failed to alloc ptr2 != ptr1");
    printf("ptr1: [%p]\nptr2: [%p]\nsizeof(struct chunk): [%lx]\n",
        &ptr1, &ptr2, CHUNK_SIZE
    );
    cr_assert((size_t)ptr2 == (size_t)ptr1 + 12 + CHUNK_SIZE,
        "Failed to alloc: %lx - %lx", (size_t)ptr2, (size_t)ptr1+12+CHUNK_SIZE); /* : %lx - %lx", (size_t)ptr2, (size_t)ptr1 + 12 + (sizeof (struct chunk)); */
    char *ptr3 = (char*)my_malloc(55);
    cr_assert((size_t)ptr3 == (size_t)ptr2 + 25 + CHUNK_SIZE,
        "Failed to alloc: %lx - %lx", (size_t)ptr3, (size_t)ptr2+25+CHUNK_SIZE);/*  : %lx - %lx", (size_t)ptr3, (size_t)ptr2 + 25 + (sizeof (struct chunk))); */
    printf("my_free ptr1\n");
    my_free(ptr1);
    printf("my_free ptr2\n");
    my_free(ptr2);
    struct chunk *t = (struct chunk*)((size_t)ptr1 - CHUNK_SIZE);
    printf("t: %lu\n", t->size);
    cr_assert((size_t)t->size==12+25+CHUNK_SIZE, "Failed to my_free");

}

Test(simple, simple_map_05)
{
    // utilisation simple d'un mmap
    char *ptr1 = (char*)my_malloc(8192);
    cr_assert(ptr1 != NULL, "Failed to alloc ptr1");
}