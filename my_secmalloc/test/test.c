#include <criterion/criterion.h>
#define _GNU_SOURCE
#include <sys/mman.h>
#include <stdio.h>
#include <pthread.h>
#include <stdint.h>
#include "my_secmalloc.h"
#include "my_secmalloc.private.h"

Test(my_malloc, basic_allocation) {
    printf("== my_malloc basic allocation begin! ==\n");

    void *ptr = my_malloc(100);
    cr_expect(ptr != NULL, "my_malloc(100) failed");
    my_free(ptr);

    printf("== my_malloc basic allocation done! ==\n");
}


Test(my_malloc, large_allocation) {
    printf("== my_malloc large allocation begin! ==\n");

    void *ptr = my_malloc(10000);
    cr_expect(ptr != NULL, "my_malloc(10000) failed");
    my_free(ptr);

    printf("== my_malloc large allocation done! ==\n");
}

Test(my_calloc, basic_allocation) {
    printf("== my_calloc basic allocation begin! ==\n");

    void *ptr = my_calloc(10, 100);
    cr_expect(ptr != NULL, "my_calloc(10, 100) failed");
    for (size_t i = 0; i < 1000; i++) {
        cr_expect(((char *)ptr)[i] == 0, "Memory not zero-initialized at byte %zu", i);
    }
    my_free(ptr);

    printf("== my_calloc basic allocation done! ==\n");
}

Test(my_realloc, increase_size) {
    printf("== my_realloc increase size begin! ==\n");

    void *ptr = my_malloc(100);
    void *new_ptr = my_realloc(ptr, 200);
    cr_expect(new_ptr != NULL, "my_realloc(ptr, 200) failed");
    my_free(new_ptr);

    printf("== my_realloc increase size done! ==\n");
}

Test(my_realloc, decrease_size) {
    printf("== my_realloc decrease size begin! ==\n");

    void *ptr = my_malloc(200);
    void *new_ptr = my_realloc(ptr, 100);
    cr_expect(new_ptr != NULL, "my_realloc(ptr, 100) failed");
    my_free(new_ptr);

    printf("== my_realloc decrease size done! ==\n");
}

Test(my_realloc, null_ptr) {
    printf("== my_realloc nullptr begin! ==\n");
    void *ptr = my_realloc(NULL, 100);
    cr_expect(ptr != NULL, "my_realloc(NULL, 100) failed");
    my_free(ptr);

    printf("== my_realloc nullptr done! ==\n");
}

Test(my_realloc, zero_size) {
    printf("== my_realloc zero size begin! ==\n");

    void *ptr = my_malloc(100);
    void *new_ptr = my_realloc(ptr, 0);
    cr_expect(new_ptr == NULL, "my_realloc(ptr, 0) should return NULL");
    if (new_ptr != NULL) {
        my_free(new_ptr);
    }

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
        allocations[i] = NULL;
    }
    printf("== my_malloc stress test done! ==\n");
}

Test(my_malloc, intentional_leak) {
    printf("== my_malloc intentional leak begin! ==\n");

    void *ptr = my_malloc(100);

    cr_assert(ptr != NULL, "Intentional memory leak test");

    printf("== my_malloc intentional leak done! ==\n");
}

Test(my_free, null_pointer) {
    printf("== my_free null pointer begin! ==\n");

    my_free(NULL);

    printf("== my_free null pointer done! ==\n");
}

Test(my_malloc, repeated_allocations) {
    printf("== my_malloc repeated allocations begin! ==\n");

    for (size_t i = 0; i < 100; ++i) {
        void *ptr = my_malloc(100);
        cr_expect(ptr != NULL, "my_malloc(100) failed at iteration %zu", i);
        my_free(ptr);
    }

    printf("== my_malloc repeated allocations done! ==\n");
}

Test(my_calloc, repeated_allocations) {
    printf("== my_calloc repeated allocations begin! ==\n");

    for (size_t i = 0; i < 100; ++i) {
        void *ptr = my_calloc(10, 10);
        cr_expect(ptr != NULL, "my_calloc(10, 10) failed at iteration %zu", i);
        my_free(ptr);
    }

    printf("== my_calloc repeated allocations done! ==\n");
}

Test(my_realloc, repeated_my_reallocations) {
    printf("== my_realloc repeated my_reallocations begin! ==\n");

    void *ptr = my_malloc(100);
    for (size_t i = 0; i < 100; ++i) {
        void *new_ptr = my_realloc(ptr, 100 + i * 10);
        cr_expect(new_ptr != NULL, "my_realloc(ptr, %zu) failed at iteration %zu", 100 + i * 10, i);
        ptr = new_ptr;
    }
    my_free(ptr);

    printf("== my_realloc repeated my_reallocations done! ==\n");
}

Test(my_malloc, alignment) {
    printf("== my_malloc alignment test begin! ==\n");

    size_t alignment = 16;
    void *ptr = my_malloc(100);
    cr_expect(((size_t)ptr % alignment) == 0, "my_malloc(100) returned unaligned address %p", ptr);
    my_free(ptr);

    printf("== my_malloc alignment test done! ==\n");
}

Test(my_malloc, large_allocation_fails1) {
    printf("== my_malloc large allocation fails begin! ==\n");

    void *ptr = my_malloc(SIZE_MAX);
    cr_expect(ptr == NULL, "my_malloc(SIZE_MAX) should return NULL");

    printf("== my_malloc large allocation fails done! ==\n");
}

Test(my_calloc, large_allocation_fails2) {
    printf("== my_calloc large allocation fails begin! ==\n");

    void *ptr = my_calloc(SIZE_MAX, 1);
    cr_expect(ptr == NULL, "my_calloc(SIZE_MAX, 1) should return NULL");

    ptr = my_calloc(1, SIZE_MAX);
    cr_expect(ptr == NULL, "my_calloc(1, SIZE_MAX) should return NULL");

    printf("== my_calloc large allocation fails done! ==\n");
}

Test(my_realloc, reduce_to_zero) {
    printf("== my_realloc reduce to zero begin! ==\n");

    void *ptr = my_malloc(100);
    void *new_ptr = my_realloc(ptr, 0);
    cr_expect(new_ptr == NULL, "my_realloc(ptr, 0) should return NULL");

    printf("== my_realloc reduce to zero done! ==\n");
}

Test(my_malloc, fragmented_allocation) {
    printf("== my_malloc fragmented allocation begin! ==\n");

    void *ptrs[10];
    for (int i = 0; i < 10; ++i) {
        ptrs[i] = my_malloc(100);
        cr_expect(ptrs[i] != NULL, "my_malloc(100) failed at iteration %d", i);
    }

    for (int i = 0; i < 10; i += 2) {
        my_free(ptrs[i]);
    }

    void *ptr = my_malloc(500);
    cr_expect(ptr != NULL, "my_malloc(500) failed");
    my_free(ptr);

    for (int i = 1; i < 10; i += 2) {
        my_free(ptrs[i]);
    }

    printf("== my_malloc fragmented allocation done! ==\n");
}

Test(my_malloc, many_small_allocations) {
    printf("== my_malloc many small allocations begin! ==\n");

    void *ptrs[1000];
    for (int i = 0; i < 1000; ++i) {
        ptrs[i] = my_malloc(1);
        cr_expect(ptrs[i] != NULL, "my_malloc(1) failed at iteration %d", i);
    }

    for (int i = 0; i < 1000; ++i) {
        my_free(ptrs[i]);
    }

    printf("== my_malloc many small allocations done! ==\n");
}

Test(my_malloc, alternating_my_malloc_my_free) {
    printf("== my_malloc alternating my_malloc my_free begin! ==\n");

    for (int i = 0; i < 1000; ++i) {
        void *ptr = my_malloc(1);
        cr_expect(ptr != NULL, "my_malloc(1) failed at iteration %d", i);
        my_free(ptr);
    }

    printf("== my_malloc alternating my_malloc my_free done! ==\n");
}

Test(my_calloc, many_small_allocations) {
    printf("== my_calloc many small allocations begin! ==\n");

    void *ptrs[1000];
    for (int i = 0; i < 1000; ++i) {
        ptrs[i] = my_calloc(1, 1);
        cr_expect(ptrs[i] != NULL, "my_calloc(1, 1) failed at iteration %d", i);
    }

    for (int i = 0; i < 1000; ++i) {
        my_free(ptrs[i]);
    }

    printf("== my_calloc many small allocations done! ==\n");
}

Test(my_malloc, large_number_of_allocations) {
    printf("== my_malloc large number of allocations begin! ==\n");

    void *ptrs[10000];
    for (int i = 0; i < 10000; ++i) {
        ptrs[i] = my_malloc(10);
        cr_expect(ptrs[i] != NULL, "my_malloc(10) failed at iteration %d", i);
    }

    for (int i = 0; i < 10000; ++i) {
        my_free(ptrs[i]);
    }

    printf("== my_malloc large number of allocations done! ==\n");
}

Test(my_malloc, large_fragmented_allocations) {
    printf("== my_malloc large fragmented allocations begin! ==\n");

    void *ptrs[100];
    for (int i = 0; i < 100; ++i) {
        ptrs[i] = my_malloc(1024);
        cr_expect(ptrs[i] != NULL, "my_malloc(1024) failed at iteration %d", i);
    }

    for (int i = 0; i < 100; i += 2) {
        my_free(ptrs[i]);
    }

    void *ptr = my_malloc(51200);  // 100 * 1024 / 2
    cr_expect(ptr != NULL, "my_malloc(51200) failed");
    my_free(ptr);

    for (int i = 1; i < 100; i += 2) {
        my_free(ptrs[i]);
    }

    printf("== my_malloc large fragmented allocations done! ==\n");
}

Test(my_malloc, my_free_and_reuse) {
    printf("== my_malloc my_free and reuse begin! ==\n");

    void *ptr = my_malloc(100);
    printf("my_malloc(100) -> %p\n", ptr);
    cr_expect(ptr != NULL, "my_malloc(100) failed");

    my_free(ptr);
    printf("my_free\n");

    void *ptr2 = my_malloc(100);
    printf("my_malloc(100) -> %p\n", ptr2);
    cr_expect(ptr2 != NULL, "my_malloc(100) failed on reuse");
    cr_expect(ptr == ptr2, "my_malloc(100) did not reuse the my_freed block");

    my_free(ptr2);
    printf("my_free\n");

    printf("== my_malloc my_free and reuse done! ==\n");
}

Test(my_realloc, shrink_and_grow) {
    printf("== my_realloc shrink and grow begin! ==\n");

    void *ptr = my_malloc(200);
    cr_expect(ptr != NULL, "my_malloc(200) failed");
    
    void *shrink_ptr = my_realloc(ptr, 100);
    cr_expect(shrink_ptr != NULL, "my_realloc(ptr, 100) failed");
    
    void *grow_ptr = my_realloc(shrink_ptr, 300);
    cr_expect(grow_ptr != NULL, "my_realloc(shrink_ptr, 300) failed");
    
    my_free(grow_ptr);

    printf("== my_realloc shrink and grow done! ==\n");
} 

Test(my_malloc, large_alternating_my_malloc_my_free) {
    printf("== my_malloc large alternating my_malloc my_free begin! ==\n");

    for (int i = 0; i < 500; ++i) {
        void *ptr = my_malloc(1000);
        cr_expect(ptr != NULL, "my_malloc(1000) failed at iteration %d", i);
        my_free(ptr);
    }

    printf("== my_malloc large alternating my_malloc my_free done! ==\n");
}

Test(my_calloc, alternating_my_malloc_my_free) {
    printf("== my_calloc alternating my_malloc my_free begin! ==\n");

    for (int i = 0; i < 500; ++i) {
        void *ptr = my_calloc(1, 1000);
        cr_expect(ptr != NULL, "my_calloc(1, 1000) failed at iteration %d", i);
        my_free(ptr);
    }

    printf("== my_calloc alternating my_malloc my_free done! ==\n");
}

Test(my_realloc, alternating_my_malloc_my_free) {
    printf("== my_realloc alternating my_malloc my_free begin! ==\n");

    void *ptr = my_malloc(100);
    cr_expect(ptr != NULL, "my_malloc(100) failed");

    for (int i = 0; i < 500; ++i) {
        void *new_ptr = my_realloc(ptr, 100 + i * 10);
        cr_expect(new_ptr != NULL, "my_realloc(ptr, %zu) failed at iteration %d", 100 + i * 10, i);
        ptr = new_ptr;
    }
    my_free(ptr);

    printf("== my_realloc alternating my_malloc my_free done! ==\n");
}

Test(my_malloc, small_and_large_allocations) {
    printf("== my_malloc small and large allocations begin! ==\n");

    void *ptr1 = my_malloc(10);
    cr_expect(ptr1 != NULL, "my_malloc(10) failed");

    void *ptr2 = my_malloc(10000);
    cr_expect(ptr2 != NULL, "my_malloc(10000) failed");

    my_free(ptr1);
    my_free(ptr2);

    printf("== my_malloc small and large allocations done! ==\n");
}

Test(my_malloc, interleaved_allocations) {
    printf("== my_malloc interleaved allocations begin! ==\n");

    void *ptr1 = my_malloc(100);
    cr_expect(ptr1 != NULL, "my_malloc(100) failed");

    void *ptr2 = my_malloc(200);
    cr_expect(ptr2 != NULL, "my_malloc(200) failed");

    void *ptr3 = my_malloc(50);
    cr_expect(ptr3 != NULL, "my_malloc(50) failed");

    my_free(ptr2);
    my_free(ptr1);
    my_free(ptr3);

    printf("== my_malloc interleaved allocations done! ==\n");
}

Test(my_realloc, my_free_null) {
    printf("== my_realloc my_free null begin! ==\n");

    void *ptr = my_realloc(NULL, 100);
    cr_expect(ptr != NULL, "my_realloc(NULL, 100) failed");
    my_free(ptr);

    printf("== my_realloc my_free null done! ==\n");
}

Test(my_malloc, small_and_large_my_free) {
    printf("== my_malloc small and large my_free begin! ==\n");

    void *ptr1 = my_malloc(100);
    cr_expect(ptr1 != NULL, "my_malloc(100) failed");

    void *ptr2 = my_malloc(10000);
    cr_expect(ptr2 != NULL, "my_malloc(10000) failed");

    my_free(ptr1);
    my_free(ptr2);

    printf("== my_malloc small and large my_free done! ==\n");
}
