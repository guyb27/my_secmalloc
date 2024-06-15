#include <criterion/criterion.h>
#define _GNU_SOURCE
#include <sys/mman.h>
#include <stdio.h>
#include <pthread.h>
#include <stdint.h>
#include "my_secmalloc.h"
#include "my_secmalloc.private.h"

Test(malloc, basic_allocation) {
    printf("== MALLOC basic allocation begin! ==\n");

    void *ptr = malloc(100);
    cr_expect(ptr != NULL, "malloc(100) failed");
    free(ptr);

    printf("== malloc basic allocation done! ==\n");
}


Test(malloc, large_allocation) {
    printf("== malloc large allocation begin! ==\n");

    void *ptr = malloc(10000);
    cr_expect(ptr != NULL, "malloc(10000) failed");
    free(ptr);

    printf("== malloc large allocation done! ==\n");
}
