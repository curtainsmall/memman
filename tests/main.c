#include <stdio.h>

#define MEMMAN_IMPLEMENT
#include "memman.h"

struct test_struct
{
    int i;
    float f;
};

int main()
{
    // Test `mem` family
    {
        printf("===== Test `mem` family =====\n");

        void* p = NULL;
        mem_init(&p, 0);

        printf("Size of memory: %zu\n", mem_size(p));
        mem_extend_back(&p, 200);
        mem_extend_front(&p, 200);
        printf("Size of memory: %zu\n", mem_size(p));

        mem_make_str(&p, "The address of current memory is 0x%p", p);
        printf("%s\n", (const char*) p);
        printf("Size of memory after making string: %zu\n", mem_size(p));

        mem_init(&p, 0);
        printf("Size of memory after re-init: %zu\n", mem_size(p));
        mem_drop(&p);
    }

    // Test 'membuf` family
    {
        printf("===== Test `membuf` family =====\n");

        struct test_struct tss[] = {
            {.i = 0, .f = 42.123},
            {.i = 1, .f = 42.235}
        };

        struct test_struct* buf = NULL;
        membuf_init(&buf, sizeof(*buf));

        membuf_insert_back_n(&buf, tss, 2);
        printf("Tss[0]: i = %d, f = %f\n", buf[0].i, buf[0].f);
        printf("Tss[1]: i = %d, f = %f\n", buf[1].i, buf[1].f);

        membuf_erase(&buf, 0);
        struct test_struct* elem = membuf_at(buf, 0);
        printf("Tss[0]: i = %d, f = %f\n", elem->i, elem->f);
        elem = membuf_at(&buf, 1);
        assert(elem == NULL);

        printf("Size of buffer memory: %zu, capacity: %zu\n", membuf_size(buf), membuf_capacity(buf));
        membuf_reserve(&buf, 1000);
        printf("Size of buffer memory: %zu, capacity: %zu\n", membuf_size(buf), membuf_capacity(buf));
        membuf_shrink_to_fit(&buf);
        printf("Size of buffer memory: %zu, capacity: %zu\n", membuf_size(buf), membuf_capacity(buf));

        membuf_drop(&buf);
    }

    // Test 'memstr' family
    {
        printf("===== Test `memstr` family =====\n");

        char* str = NULL;
        memstr_init(&str);

        memstr_append(&str, "A test string at address %p\n", str);
        printf(str);
        memstr_append(&str, "Another test string\n");
        memstr_append_char(&str, 'Y');
        memstr_append_char(&str, '\n');
        printf(str);

        memstr_reduce(&str, 10);
        printf(str);

        memstr_drop(&str);
    }

    return 0;
}