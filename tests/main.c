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
    struct test_struct tss[] = {
        {.i = 0, .f = 42},
        {.i = 1, .f = 42}
    };

    void* p = NULL;
    mem_init(&p);

    printf("Size of memory: %zu\n", mem_size(p));
    mem_extend(&p, -200);
    mem_extend(&p, 200);
    printf("Size of memory: %zu\n", mem_size(p));

    mem_make_str(&p, "The address of current memory is 0x%p", p);
    printf("%s\n", p);
    printf("Size of memory after making string: %zu\n", mem_size(p));

    mem_drop(&p);

#if 0
    m3_buf_tt(struct test_struct) ts = { 0 };
    m3_buf_init(&ts, sizeof(struct test_struct));

    m3_buf_insert_back(&ts, tss);
    printf("Struct: %d, %f\n", ts[0].i, ts[0].f);
    m3_buf_erase_back(&ts);

    m3_buf_insert_back_n(&ts, tss, 2);
    printf("Struct: %d, %f\n", ts[0].i, ts[0].f);
    printf("Struct: %d, %f\n", ts[1].i, ts[1].f);
    m3_buf_erase_back_n(&ts, 2);

    m3_buf_drop(&ts);
#endif

    return 0;
}