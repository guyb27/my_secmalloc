#include <criterion/criterion.h>
#include <stdio.h>
#include "my_secmalloc.private.h"
#include <sys/mman.h>

Test(mmap, simple) {
    void *ptr = mmap((void*)(BASE_CHUNK_ADDR), 4096, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    cr_expect(ptr != NULL);
    int res = munmap(ptr, 4096);
    cr_expect(res == 0);
}

Test(mmap, simple_malloc) {
    char *ptr1 = (char*)my_malloc(12);
    cr_assert(ptr1 != NULL, "Failed to alloc ptr1");
}
