#include <criterion/criterion.h>
#include <stdio.h>
#include "my_secmalloc.private.h"
#include <sys/mman.h>

Test(mmap, simple) {
    void *ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    cr_expect(ptr != NULL);
    int res = munmap(ptr, 4096);
    cr_expect(res == 0);
}
