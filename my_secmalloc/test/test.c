#include <criterion/criterion.h>
#define _GNU_SOURCE
#include <sys/mman.h>
#include <stdio.h>
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

Test(calloc, basic_allocation) {
    printf("== calloc basic allocation begin! ==\n");

    void *ptr = calloc(10, 100);
    cr_expect(ptr != NULL, "calloc(10, 100) failed");
    for (size_t i = 0; i < 1000; i++) {
        cr_expect(((char *)ptr)[i] == 0, "Memory not zero-initialized at byte %zu", i);
    }
    free(ptr);

    printf("== calloc basic allocation done! ==\n");
}

Test(realloc, increase_size) {
    printf("== realloc increase size begin! ==\n");

    void *ptr = malloc(100);
    void *new_ptr = realloc(ptr, 200);
    cr_expect(new_ptr != NULL, "realloc(ptr, 200) failed");
    free(new_ptr);

    printf("== realloc increase size done! ==\n");
}

Test(realloc, decrease_size) {
    printf("== realloc decrease size begin! ==\n");

    void *ptr = malloc(200);
    void *new_ptr = realloc(ptr, 100);
    cr_expect(new_ptr != NULL, "realloc(ptr, 100) failed");
    free(new_ptr);

    printf("== realloc decrease size done! ==\n");
}

Test(realloc, null_ptr) {
    printf("== realloc nullptr begin! ==\n");
    void *ptr = realloc(NULL, 100);
    cr_expect(ptr != NULL, "realloc(NULL, 100) failed");
    free(ptr);

    printf("== realloc nullptr done! ==\n");
}

Test(realloc, zero_size) {
    printf("== realloc zero size begin! ==\n");

    void *ptr = malloc(100);
    void *new_ptr = realloc(ptr, 0);
    cr_expect(new_ptr == NULL, "realloc(ptr, 0) should return NULL");
    if (new_ptr != NULL) {
        free(new_ptr);
    }

    printf("== realloc zero size done! ==\n");
}

Test(malloc, stress_test) {
    printf("== malloc stress test begin! ==\n");
    size_t allocation_size = 1024;
    void *allocations[1000];

    for (size_t i = 0; i < 1000; ++i) {
        allocations[i] = malloc(allocation_size);
        cr_expect(allocations[i] != NULL, "malloc(%zu) failed at iteration %zu", allocation_size, i);
    }

    for (size_t i = 0; i < 1000; ++i) {
        free(allocations[i]);
        allocations[i] = NULL;
    }
    printf("== malloc stress test done! ==\n");
}

Test(malloc, intentional_leak) {
    printf("== malloc intentional leak begin! ==\n");

    void *ptr = malloc(100);

    cr_assert(ptr != NULL, "Intentional memory leak test");

    printf("== malloc intentional leak done! ==\n");
}

Test(free, null_pointer) {
    printf("== free null pointer begin! ==\n");

    free(NULL);

    printf("== free null pointer done! ==\n");
}

Test(malloc, repeated_allocations) {
    printf("== malloc repeated allocations begin! ==\n");

    for (size_t i = 0; i < 100; ++i) {
        void *ptr = malloc(100);
        cr_expect(ptr != NULL, "malloc(100) failed at iteration %zu", i);
        free(ptr);
    }

    printf("== malloc repeated allocations done! ==\n");
}

Test(calloc, repeated_allocations) {
    printf("== calloc repeated allocations begin! ==\n");

    for (size_t i = 0; i < 100; ++i) {
        void *ptr = calloc(10, 10);
        cr_expect(ptr != NULL, "calloc(10, 10) failed at iteration %zu", i);
        free(ptr);
    }

    printf("== calloc repeated allocations done! ==\n");
}

Test(realloc, repeated_reallocations) {
    printf("== realloc repeated reallocations begin! ==\n");

    void *ptr = malloc(100);
    for (size_t i = 0; i < 100; ++i) {
        void *new_ptr = realloc(ptr, 100 + i * 10);
        cr_expect(new_ptr != NULL, "realloc(ptr, %zu) failed at iteration %zu", 100 + i * 10, i);
        ptr = new_ptr;
    }
    free(ptr);

    printf("== realloc repeated reallocations done! ==\n");
}

Test(malloc, alignment) {
    printf("== malloc alignment test begin! ==\n");

    size_t alignment = 16;
    void *ptr = malloc(100);
    cr_expect(((size_t)ptr % alignment) == 0, "malloc(100) returned unaligned address %p", ptr);
    free(ptr);

    printf("== malloc alignment test done! ==\n");
}

Test(malloc, large_allocation_fails) {
    printf("== malloc large allocation fails begin! ==\n");

    void *ptr = malloc(SIZE_MAX);
    cr_expect(ptr == NULL, "malloc(SIZE_MAX) should return NULL");

    printf("== malloc large allocation fails done! ==\n");
}

Test(calloc, large_allocation_fails) {
    printf("== calloc large allocation fails begin! ==\n");

    void *ptr = calloc(SIZE_MAX, 1);
    cr_expect(ptr == NULL, "calloc(SIZE_MAX, 1) should return NULL");

    ptr = calloc(1, SIZE_MAX);
    cr_expect(ptr == NULL, "calloc(1, SIZE_MAX) should return NULL");

    printf("== calloc large allocation fails done! ==\n");
}

Test(realloc, large_allocation_fails) {
    printf("== realloc large allocation fails begin! ==\n");

    void *ptr = malloc(100);
    void *new_ptr = realloc(ptr, SIZE_MAX);
    cr_expect(new_ptr == NULL, "realloc(ptr, SIZE_MAX) should return NULL");
    free(ptr);

    printf("== realloc large allocation fails done! ==\n");
}

Test(malloc, free_and_reuse) {
    printf("== malloc free and reuse begin! ==\n");

    void *ptr = malloc(100);
    cr_expect(ptr != NULL, "malloc(100) failed");

    free(ptr);

    void *ptr2 = malloc(100);
    cr_expect(ptr2 != NULL, "malloc(100) failed on reuse");
    cr_expect(ptr == ptr2, "malloc(100) did not reuse the freed block");

    free(ptr2);

    printf("== malloc free and reuse done! ==\n");
}

Test(realloc, same_size) {
    printf("== realloc same size begin! ==\n");

    void *ptr = malloc(100);
    void *new_ptr = realloc(ptr, 100);
    cr_expect(new_ptr == ptr, "realloc(ptr, 100) should return the same pointer");
    free(new_ptr);

    printf("== realloc same size done! ==\n");
}

Test(realloc, reduce_to_zero) {
    printf("== realloc reduce to zero begin! ==\n");

    void *ptr = malloc(100);
    void *new_ptr = realloc(ptr, 0);
    cr_expect(new_ptr == NULL, "realloc(ptr, 0) should return NULL");

    printf("== realloc reduce to zero done! ==\n");
}

Test(malloc, fragmented_allocation) {
    printf("== malloc fragmented allocation begin! ==\n");

    void *ptrs[10];
    for (int i = 0; i < 10; ++i) {
        ptrs[i] = malloc(100);
        cr_expect(ptrs[i] != NULL, "malloc(100) failed at iteration %d", i);
    }

    for (int i = 0; i < 10; i += 2) {
        free(ptrs[i]);
    }

    void *ptr = malloc(500);
    cr_expect(ptr != NULL, "malloc(500) failed");
    free(ptr);

    for (int i = 1; i < 10; i += 2) {
        free(ptrs[i]);
    }

    printf("== malloc fragmented allocation done! ==\n");
}

Test(malloc, many_small_allocations) {
    printf("== malloc many small allocations begin! ==\n");

    void *ptrs[1000];
    for (int i = 0; i < 1000; ++i) {
        ptrs[i] = malloc(1);
        cr_expect(ptrs[i] != NULL, "malloc(1) failed at iteration %d", i);
    }

    for (int i = 0; i < 1000; ++i) {
        free(ptrs[i]);
    }

    printf("== malloc many small allocations done! ==\n");
}

Test(malloc, alternating_malloc_free) {
    printf("== malloc alternating malloc free begin! ==\n");

    for (int i = 0; i < 1000; ++i) {
        void *ptr = malloc(1);
        cr_expect(ptr != NULL, "malloc(1) failed at iteration %d", i);
        free(ptr);
    }

    printf("== malloc alternating malloc free done! ==\n");
}

Test(calloc, many_small_allocations) {
    printf("== calloc many small allocations begin! ==\n");

    void *ptrs[1000];
    for (int i = 0; i < 1000; ++i) {
        ptrs[i] = calloc(1, 1);
        cr_expect(ptrs[i] != NULL, "calloc(1, 1) failed at iteration %d", i);
    }

    for (int i = 0; i < 1000; ++i) {
        free(ptrs[i]);
    }

    printf("== calloc many small allocations done! ==\n");
}

Test(malloc, large_number_of_allocations) {
    printf("== malloc large number of allocations begin! ==\n");

    void *ptrs[10000];
    for (int i = 0; i < 10000; ++i) {
        ptrs[i] = malloc(10);
        cr_expect(ptrs[i] != NULL, "malloc(10) failed at iteration %d", i);
    }

    for (int i = 0; i < 10000; ++i) {
        free(ptrs[i]);
    }

    printf("== malloc large number of allocations done! ==\n");
}

Test(malloc, large_fragmented_allocations) {
    printf("== malloc large fragmented allocations begin! ==\n");

    void *ptrs[100];
    for (int i = 0; i < 100; ++i) {
        ptrs[i] = malloc(1024);
        cr_expect(ptrs[i] != NULL, "malloc(1024) failed at iteration %d", i);
    }

    for (int i = 0; i < 100; i += 2) {
        free(ptrs[i]);
    }

    void *ptr = malloc(51200);  // 100 * 1024 / 2
    cr_expect(ptr != NULL, "malloc(51200) failed");
    free(ptr);

    for (int i = 1; i < 100; i += 2) {
        free(ptrs[i]);
    }

    printf("== malloc large fragmented allocations done! ==\n");
}