#include <criterion/criterion.h>
#include "my_secmalloc.private.h"

Test(malloc_test, tiny_allocation) {
    void *ptr = my_malloc(10);
    cr_assert_not_null(ptr, "Tiny allocation failed");
}
Test(malloc_test, small_allocation) {
    void *ptr = my_malloc(20);
    cr_assert_not_null(ptr, "Small allocation failed");
}
Test(malloc_test, large_allocation) {
    void *ptr = my_malloc(250);
    cr_assert_not_null(ptr, "Large allocation failed");
}
/*
Test(secmalloc, canary_initialization) {
    void *ptr = my_malloc(128);
    cr_assert_not_null(ptr, "Allocation should not return NULL");

    size_t canary = *((size_t *)((char *)ptr + 128));
    cr_assert_eq(canary, FIXED_CANARY, "Canary value should be initialized correctly");

    my_free(ptr);
}

Test(secmalloc, canary_detection) {
    void *ptr = my_malloc(128);
    cr_assert_not_null(ptr, "Allocation should not return NULL");

    size_t *canary_ptr = (size_t *)((char *)ptr + 128);
    size_t original_canary = *canary_ptr;

    // Modify the canary to simulate memory corruption
    *canary_ptr ^= 0x1;

    // Check if the canary value is detected as corrupted
    cr_assert_neq(*canary_ptr, original_canary, "Canary value should detect memory corruption");

    my_free(ptr);
}

Test(secmalloc, buffer_overflow_detection) {
    void *ptr = my_malloc(128);
    cr_assert_not_null(ptr, "Allocation should not return NULL");

    size_t *canary_ptr = (size_t *)((char *)ptr + 128);
    size_t original_canary = *canary_ptr;

    // Write beyond the allocated memory to simulate buffer overflow
    char *buffer = (char *)ptr;
    for (size_t i = 0; i < 150; i++) {
        buffer[i] = 'A';
    }

    // Check if the canary value is detected as corrupted
    cr_assert_neq(*canary_ptr, original_canary, "Canary value should detect buffer overflow");

    my_free(ptr);
}

Test(secmalloc, double_free_detection) {
    void *ptr = my_malloc(128);
    cr_assert_not_null(ptr, "Allocation should not return NULL");

    my_free(ptr);

    // Attempt to free the same block again
    // Cette est une simple simulation et dans un scénario réel, vous implémenteriez une logique pour suivre les doubles libérations
    my_free(ptr); // Cette ligne devrait déclencher une erreur si la double libération est correctement détectée.
}
*/