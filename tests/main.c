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
    m3_buf_tt(struct test_struct) ts = { 0 };
    m3_buf_init(&ts, sizeof(struct test_struct));

    m3_buf_insert_back(&ts, &(struct test_struct) { .i = 42, .f = 42.8 });

    printf("Struct: %d, %f", ts[0].i, ts[0].f);

    m3_buf_drop(&ts);

    return 0;
}