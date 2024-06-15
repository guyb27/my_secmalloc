#include <criterion/criterion.h>
#define _GNU_SOURCE
#include <sys/mman.h>
#include <stdio.h>
#include "my_secmalloc.h"
#include "my_secmalloc.private.h"

Test(my_malloc, basic_allocation) {
    printf("== MALLOC basic allocation begin! ==\n");

    void *ptr = my_malloc(100);
    printf("my_malloc(100) -> %p\n", ptr);
    cr_expect(ptr != NULL, "my_malloc(100) failed");
    my_free(ptr);
    printf("my_free(%p)\n", ptr);

    printf("== my_malloc basic allocation done! ==\n");
}

Test(malloc, large_allocation) {
    printf("== my_malloc large allocation begin! ==\n");

    void *ptr = my_malloc(10000);
    printf("my_malloc(10000) -> %p\n", ptr);
    cr_expect(ptr != NULL, "my_malloc(10000) failed");
    my_free(ptr);
    printf("my_free(%p)\n", ptr);

    printf("== my_malloc large allocation done! ==\n");
}

Test(my_calloc, basic_allocation) {
    printf("== my_calloc basic allocation begin! ==\n");

    void *ptr = my_calloc(10, 100);
    printf("my_calloc(10, 100) -> %p\n", ptr);
    cr_expect(ptr != NULL, "my_calloc(10, 100) failed");
    for (size_t i = 0; i < 1000; i++) {
        cr_expect(((char *)ptr)[i] == 0, "Memory not zero-initialized at byte %zu", i);
    }
    my_free(ptr);
    printf("my_free(%p)\n", ptr);

    printf("== my_calloc basic allocation done! ==\n");
}

Test(my_realloc, increase_size) {
    printf("== my_realloc increase size begin! ==\n");

    void *ptr = my_malloc(100);
    printf("my_malloc(100) -> %p\n", ptr);
    void *new_ptr = my_realloc(ptr, 200);
    printf("my_realloc(%p, 200) -> %p\n", ptr, new_ptr);
    cr_expect(new_ptr != NULL, "my_realloc(ptr, 200) failed");
    my_free(new_ptr);
    printf("my_free(%p)\n", new_ptr);

    printf("== my_realloc increase size done! ==\n");
}

Test(my_realloc, decrease_size) {
    printf("== my_realloc decrease size begin! ==\n");

    void *ptr = my_malloc(200);
    printf("my_malloc(200) -> %p\n", ptr);
    void *new_ptr = my_realloc(ptr, 100);
    printf("my_realloc(%p, 100) -> %p\n", ptr, new_ptr);
    cr_expect(new_ptr != NULL, "my_realloc(ptr, 100) failed");
    my_free(new_ptr);
    printf("my_free(%p)\n", new_ptr);

    printf("== my_realloc decrease size done! ==\n");
}

Test(my_realloc, null_ptr) {
    printf("== my_realloc nullptr begin! ==\n");

    void *ptr = my_realloc(NULL, 100);
    printf("my_realloc(NULL, 100) -> %p\n", ptr);
    cr_expect(ptr != NULL, "my_realloc(NULL, 100) failed");
    my_free(ptr);
    printf("my_free(%p)\n", ptr);

    printf("== my_realloc nullptr done! ==\n");
}

Test(my_realloc, zero_size) {
    printf("== my_realloc zero size begin! ==\n");

    void *ptr = my_malloc(100);
    printf("my_malloc(100) -> %p\n", ptr);
    void *new_ptr = my_realloc(ptr, 0);
    printf("my_realloc(%p, 0) -> %p\n", ptr, new_ptr);
    cr_expect(new_ptr == NULL, "my_realloc(ptr, 0) should return NULL");

    printf("== my_realloc zero size done! ==\n");
}

Test(my_malloc, stress_test) {
    printf("== my_malloc stress test begin! ==\n");
    size_t allocation_size = 1024;
    void *allocations[1000];

    for (size_t i = 0; i < 1000; ++i) {
        allocations[i] = my_malloc(allocation_size);
        cr_expect(allocations[i] != NULL, "my_malloc(%zu) failed at iteration %zu", allocation_size, i);
    }

    for (size_t i = 0; i < 1000; ++i) {
        my_free(allocations[i]);
    }
    printf("== my_malloc stress test done! ==\n");
}

Test(my_malloc, intentional_leak) {
    printf("== malloc intentional leak begin! ==\n");

    void *ptr = my_malloc(100);
    printf("my_malloc(100) -> %p\n", ptr);

    cr_assert(ptr != NULL, "Intentional memory leak test");

    printf("== my_malloc intentional leak done! ==\n");
}
