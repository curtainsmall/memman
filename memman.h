#pragma once

#ifndef MEMMAN_H
#define MEMMAN_H

#ifndef MEMMAN_EXTERN
#define MEMMAN_EXTERN
#endif // !MEMMAN_EXTERN


#include <memory.h>
#include <stdint.h>
#include <string.h>

#define m3_memset(dst, val, size) ((void) memset(dat, val, size))
#define m3_memcpy(dst, src, size) ((void) memcpy(dst, src, size))
#define m3_memmove(dst, src, size) ((void) memmove(dst, src, size))
#define m3_strcpy(dst, src) ((void) strcpy(dst, src))

MEMMAN_EXTERN void* m3_memman(void* ptr, size_t new_size);
MEMMAN_EXTERN size_t m3_memsize(void* ptr);
#define m3_strsize(str) (m3_memsize(str) - 1)

#define m3_alloc(size) m3_memman(NULL, size)
#define m3_realloc(ptr, size) m3_memman(ptr, size)
#define m3_free(ptr) ((void) m3_memman(ptr, 0))

#define m3_memnav(ptr, Type, offset) ((Type*)(ptr) + (offset))

#define m3_buf_tt(Type) Type*
MEMMAN_EXTERN void m3_buf_init(void** p_buf, size_t elemsize);
MEMMAN_EXTERN void m3_buf_drop(void** p_buf);
MEMMAN_EXTERN void m3_buf_clear(void** p_buf);
MEMMAN_EXTERN size_t m3_buf_size(void* buf);
MEMMAN_EXTERN uintmax_t m3_buf_count(void* buf);
MEMMAN_EXTERN void m3_buf_insert_n(void** p_buf, intmax_t idx, void* val, uintmax_t count);
MEMMAN_EXTERN void m3_buf_insert(void** p_buf, intmax_t idx, void* val, uintmax_t count);
MEMMAN_EXTERN void m3_buf_insert_back_n(void** p_buf, void* val, uintmax_t count);
MEMMAN_EXTERN void m3_buf_insert_back(void** p_buf, void* val);
MEMMAN_EXTERN void m3_buf_erase_n(void** p_buf, intmax_t idx, uintmax_t count);
MEMMAN_EXTERN void m3_buf_erase(void** p_buf, intmax_t idx, uintmax_t count);
MEMMAN_EXTERN void m3_buf_erase_back_n(void** p_buf, uintmax_t count);
MEMMAN_EXTERN void m3_buf_erase_back(void** p_buf);

MEMMAN_EXTERN const char* m3_make_str(const char* fmt, ...);
MEMMAN_EXTERN const char* m3_make_str_v(const char* fmt, va_list va);

#ifdef MEMMAN_IMPLEMENT

#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>

void* m3_memman(void* ptr, size_t new_size)
{
    size_t old_size = m3_memsize(ptr);

    if(old_size == new_size)
        return ptr;

    void* mem = ptr;
    if(mem)
        mem = m3_memnav(ptr, size_t, -1);

    if(new_size == 0)
    {
        free(mem);
        return NULL;
    }
    else
    {
        void* tmp = realloc(mem, new_size + sizeof(size_t));
        if(!tmp)
        {
            printf(stderr, "Out of Memory");
            abort();
        }
        *m3_memnav(tmp, size_t, 0) = new_size;
        return m3_memnav(tmp, size_t, 1);
    }
}

size_t m3_memsize(void* buf)
{
    if(!buf)
        return 0;
    return *m3_memnav(buf, size_t, -1);
}

static intmax_t rel2abs(intmax_t idx, uintmax_t count)
{
    if(idx >= 0)
        return idx;

    return (idx %= count) < 0 ? idx + count : idx;
}

struct buf
{
    size_t elemsize;
    uintmax_t count;
    uint8_t values[];
};
#define bufelemsize(ptr) (*m3_memnav(ptr, size_t, -2))
#define bufsize(ptr) (*m3_memnav(ptr, size_t, -1))
#define bufhead(ptr) m3_memnav(ptr, size_t, -2)
#define bufinfosize_v (2 * sizeof(size_t))

void m3_buf_init(void** p_buf, size_t elemsize)
{
    assert(p_buf);

    if(*p_buf)
        return;

    struct buf* buf = m3_alloc(sizeof(*buf));
    buf->elemsize = elemsize;
    buf->count = 0;
    *p_buf = buf->values;
}

void m3_buf_drop(void** p_buf)
{
    assert(p_buf);

    if(!*p_buf)
        return;
    m3_free(bufhead(*p_buf));
    *p_buf = NULL;
}

void m3_buf_clear(void** p_buf)
{
    assert(p_buf);

    if(!*p_buf)
        return;
    *p_buf = m3_realloc(bufhead(*p_buf), bufinfosize_v);
}

size_t m3_buf_size(void* ptr)
{
    return bufsize(ptr);
}

uintmax_t m3_buf_count(void* ptr)
{
    return bufsize(ptr) / bufelemsize(ptr);
}

void m3_buf_insert_n(void** p_buf, intmax_t idx, void* val, uintmax_t count)
{
    assert(p_buf);
    assert(val);
    assert(count > 0);

    idx = rel2abs(idx, count);

    struct buf* buf = bufhead(*p_buf);

    size_t value_size = m3_memsize(buf) - bufinfosize_v;
    size_t value_size_wanted = (buf->count + count) * buf->elemsize;

    if(value_size_wanted >= value_size)
    {
        value_size = max(value_size_wanted, value_size * 2);
        buf = m3_realloc(buf, value_size + bufinfosize_v);
        *p_buf = buf->values;
    }

    size_t offset = idx * buf->elemsize;
    size_t inserted_size = buf->elemsize * count;
    uint8_t* target_pos = buf->values + offset;
    m3_memmove(target_pos + inserted_size, target_pos, value_size - offset - inserted_size);
    m3_memcpy(target_pos, val, inserted_size);
    buf->count += count;
}

void m3_buf_insert(void** p_buf, intmax_t idx, void* val, uintmax_t count)
{
    m3_buf_insert_n(p_buf, idx, val, 1);
}

void m3_buf_insert_back_n(void** p_buf, void* val, uintmax_t count)
{
    m3_buf_insert_n(p_buf, m3_buf_count(*p_buf), val, count);
}

void m3_buf_insert_back(void** p_buf, void* val)
{
    m3_buf_insert_back_n(p_buf, val, 1);
}

void m3_buf_erase_n(void** p_buf, intmax_t idx, uintmax_t count)
{
    assert(p_buf);
    assert(count > 0);

    idx = rel2abs(idx, count);

    struct buf* buf = bufhead(*p_buf);

    size_t value_size = m3_memsize(buf) - bufinfosize_v;
    size_t offset = idx * buf->elemsize;
    size_t erased_size = buf->elemsize * count;
    uint8_t* target_pos = buf->values + offset;
    m3_memmove(target_pos, target_pos + erased_size, value_size - offset - erased_size);
    buf->count -= count;

    size_t value_size_wanted = buf->count + count;
    if(value_size_wanted * buf->elemsize * 2 < value_size)
    {
        value_size = min(value_size_wanted, value_size / 2);
        buf = m3_realloc(buf, value_size + bufinfosize_v);
        *p_buf = buf->values;
    }
}

void m3_buf_erase(void** p_buf, intmax_t idx, uintmax_t count)
{
    m3_buf_erase_n(p_buf, idx, 1);
 }

void m3_buf_erase_back_n(void** p_buf, uintmax_t count)
{
    m3_buf_erase_n(p_buf, m3_buf_count(*p_buf) - 1, count);
}

void m3_buf_erase_back(void** p_buf)
{
    m3_buf_erase_back_n(p_buf, 1);
}

const char* m3_make_str(const char* fmt, ...)
{
    va_list va = { 0 };
    va_start(va, fmt);
    const char* res = m3_make_str_v(fmt, va);
    va_end(va);
    return res;
}

const char* m3_make_str_v(const char* fmt, va_list va)
{
    va_list va_tmp;
    va_copy(va_tmp, va);
    int len = vsnprintf(NULL, 0, fmt, va_tmp);
    va_end(va_tmp);

    void* str = m3_alloc((len + 1) * sizeof(*fmt));

    (void) vsnprintf(str, len + 1, fmt, va);
    return str;
}

#endif // MEMMAN_IMPLEMENT


#endif // !MEMMAN_H
