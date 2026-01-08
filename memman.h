#pragma once

#ifndef MEMMAN_H
#define MEMMAN_H

#ifndef MEMMAN_EXTERN
#define MEMMAN_EXTERN
#endif // !MEMMAN_EXTERN

#include <stdint.h>

typedef enum m3_flag
{
    M3_NONE = 0,
    M3_NO_PANIC = 1 << 0,
    M3_NO_ZERO_INIT = 1 << 1,
} m3_flag_t;

MEMMAN_EXTERN void* m3_memman(void* ptr, size_t new_size, m3_flag_t flag);
MEMMAN_EXTERN size_t m3_memsize(void* ptr);
#define m3_strsize(str) (m3_memsize(str) - 1)

#define m3_alloc(size) m3_memman(NULL, size, M3_NONE)
#define m3_realloc(ptr, size) m3_memman(ptr, size, M3_NONE)
#define m3_free(ptr) ((void) m3_memman(ptr, 0, M3_NONE))

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
#include <memory.h>
#include <string.h>

#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif // !max

#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif // !min

void* m3_memman(void* ptr, size_t new_size, m3_flag_t flag)
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
            if(flag & M3_NO_PANIC)
                return NULL;

            (void) printf(stderr, "Out of Memory");
            abort();
        }

        *m3_memnav(tmp, size_t, 0) = new_size;

        if(!(flag & M3_NO_ZERO_INIT) && new_size > old_size)
            (void) memset(m3_memnav(m3_memnav(tmp, size_t, 1),uint8_t, old_size), 0, new_size - old_size);

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

#define bufinfosize_v sizeof(size_t)
#define bufsize(buf) (*m3_memnav(buf, size_t, -2))
#define bufvaluesize(buf) (bufsize(buf) - bufinfosize_v)
#define bufelemsize(buf) (*m3_memnav(buf, size_t, -1))
#define bufvaluecount(buf) (bufvaluesize(buf) / bufelemsize(buf))

void m3_buf_init(void** p_buf, size_t elemsize)
{
    assert(p_buf);

    if(*p_buf)
        return;

    size_t* ptr = m3_alloc(bufinfosize_v);
    *ptr = elemsize;
    *p_buf = ptr + 1;
}

void m3_buf_drop(void** p_buf)
{
    assert(p_buf);

    if(!*p_buf)
        return;
    m3_free(m3_memnav(*p_buf, size_t, -1));
    *p_buf = NULL;
}

void m3_buf_clear(void** p_buf)
{
    assert(p_buf);

    if(!*p_buf)
        return;
    *p_buf = m3_realloc(m3_memnav(*p_buf, size_t, -1), bufinfosize_v);
}

size_t m3_buf_size(void* buf)
{
    return bufsize(buf);
}

uintmax_t m3_buf_count(void* buf)
{
    return bufvaluecount(buf);
}

void m3_buf_insert_n(void** p_buf, intmax_t idx, void* val, uintmax_t count)
{
    assert(p_buf);
    assert(val);
    assert(count <= INTMAX_MAX);

    if(count == 0)
        return;

    uint8_t* buf = *p_buf;

    size_t value_size = bufvaluesize(buf);

    size_t value_size_wanted = (bufvaluecount(buf) + count) * bufelemsize(buf);
    size_t offset = idx * bufelemsize(buf);
    size_t inserted_size = bufelemsize(buf) * count;

#if 0
    if(idx > bufvaluecount(buf))
    {
        value_size_wanted += (idx - bufvaluecount(buf)) * bufelemsize(buf);
    }
    else if(idx < 0)
    {
        value_size_wanted += -idx * bufelemsize(buf);
        offset = 0;
    }
#endif
    
    if(value_size_wanted >= value_size)
    {
        value_size = max(value_size_wanted, value_size * 2);
        void* new_head = m3_realloc(m3_memnav(buf, size_t, -1), value_size + bufinfosize_v);
        *p_buf = buf = m3_memnav(new_head, size_t, 1);
    }

    uint8_t* target_pos = buf + offset;
    (void) memmove(target_pos + inserted_size, target_pos, value_size - offset - inserted_size);
    (void) memcpy(target_pos, val, inserted_size);
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
    assert(count <= INTMAX_MAX);

    if(count == 0)
        return;

    uint8_t* buf = *p_buf;

    idx = rel2abs(idx, count);

    size_t value_size = bufvaluesize(buf);
    size_t offset = idx * bufelemsize(buf);
    size_t erased_size = bufelemsize(buf) * count;
    uint8_t* target_pos = buf + offset;
    size_t moved_size = value_size - offset - erased_size;
    (void) memmove(target_pos, target_pos + erased_size, moved_size);
    (void) memset(target_pos + moved_size, 0, erased_size);

    size_t value_size_wanted = bufvaluecount(buf) - count;
    if(value_size_wanted * bufelemsize(buf) * 2 < value_size)
    {
        value_size = min(value_size_wanted, value_size / 2);
        void* new_head = m3_realloc(m3_memnav(buf, size_t, -1), value_size + bufinfosize_v);
        *p_buf = m3_memnav(new_head, size_t, 1);
    }
}

void m3_buf_erase(void** p_buf, intmax_t idx, uintmax_t count)
{
    m3_buf_erase_n(p_buf, idx, 1);
 }

void m3_buf_erase_back_n(void** p_buf, uintmax_t count)
{
    m3_buf_erase_n(p_buf, m3_buf_count(*p_buf) - count, count);
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
