#include <criterion/criterion.h>
#include "my_secmalloc.private.h"
/*
Test(malloc_test, basic_allocation) {
    void *ptr = my_malloc(10);
    cr_assert_not_null(ptr, "Basic allocation failed");
}

*/

Test(malloc_test, multiple_allocation) {
    void *ptr = my_malloc(10);
    void *ptr2 = my_malloc(10);
    void *ptr3 = my_malloc(10);
    cr_assert_not_null(ptr, "Basic Multiple ptr is NULL");
    cr_assert_not_null(ptr2, "Basic multiple ptr2 is NULL");
    cr_assert_neq(ptr, ptr2, "Basic multiple ptr and ptr2 are equal");
    dprintf(2, "ptr:[%p], ptr2:[%p], ptr3:[%p]\n", ptr, ptr2, ptr3);
    cr_assert_eq(ptr+10+8, ptr2, "Basic multiple ptr+10 not equal to ptr2");
}

Test(malloc_test, canary_detection) {
    void *ptr = my_malloc(10);
    void *ptr2 = my_malloc(108);
    void *ptr3 = my_malloc(2);
    size_t *canary_ptr = (size_t *)((char *)ptr + 10);
    cr_assert_eq(*canary_ptr, FIXED_CANARY, "ptr Canary value is incorrect");
    canary_ptr = (size_t *)((char *)ptr2 + 108);
    cr_assert_eq(*canary_ptr, FIXED_CANARY, "ptr2 Canary value is incorrect");
    canary_ptr = (size_t *)((char *)ptr3 + 12);
    cr_assert_eq(*canary_ptr, FIXED_CANARY, "ptr3 Canary value is incorrect");
}




/*
Test(malloc_test, basic_multiple_allocation) {
    void *ptr = my_malloc(10);
    void *ptr2 = my_malloc(10);

    cr_assert_not_null(ptr2, "Basic multiple allocation failed");
    cr_assert_neq(ptr, ptr2, "Canary value should detect memory corruption");
}
*/
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