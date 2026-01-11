#pragma once

#ifndef MEMMAN_H
#define MEMMAN_H

#ifndef MEMMAN_API
#define MEMMAN_API
#endif // !MEMMAN_API

#include <stdint.h>

#define mem_tt(Type) Type*

typedef enum memflag
{
    /// @brief Empty flag
    MEMFLAG_NONE = 0,

    /// @brief Do not zero-init allocated memory
    MEMFLAG_NO_ZERO_INIT,
} memflag_t;

/// @brief Init memory
/// @param[in,out] p_ptr Reference to the memory
MEMMAN_API void mem_init(void** p_ptr);

/// @brief Drop decorated memory
/// @param[in] p_ptr Reference to the memory
MEMMAN_API void  mem_drop(void** p_ptr);

/// @brief Size of the memory
/// @param[in] ptr Memory
/// @return Size
MEMMAN_API size_t mem_size(void* ptr);

/// @brief Extend memory
/// @param[in,out] p_ptr Reference to the memory
/// @param[in] extent Size of extension:
/// Positive: extend forwards
/// Negative: extend backwards
/// Zero: do nothing
MEMMAN_API void mem_extend(void** p_ptr, intmax_t extent);

/// @brief Copy memory
/// @param[in,out] p_dst Reference to the destination memory
/// @param[in] dst_idx Index of destination position in @a p_dst
/// @param[in] src Sourcce memory
/// @param[in] src_idx Index of source position in @a src
/// @param[in] size Size of copy memory
/// @note The source memory and destination memory should not be the same; otherwise, use @ref memshift
MEMMAN_API void mem_copy(void** p_dst, size_t dst_idx, const void* src, size_t src_idx, size_t size);

/// @brief Shift memory
/// @param[in, out] p_ptr Reference to the memory
/// @param[in] dst_idx Index of destination position
/// @param[in] src_idx Index of source position
/// @param[in] size Size of shifted memory
MEMMAN_API void mem_shift(void** p_ptr, size_t dst_idx, size_t src_idx, size_t size);

/// @brief Error call type
/// @sa mem_set_err_callback
typedef void (*mem_err_callback_t)(void*);

/// @brief Set the error callback
/// @param[in] err_callback
/// @param[in] userdata
MEMMAN_API void mem_set_err_callback(mem_err_callback_t err_callback, void* userdata);

#if 0
typedef enum m3_flag
{
    M3_NONE = 0,
    M3_NO_PANIC = 1 << 0,
    M3_NO_ZERO_INIT = 1 << 1,
} m3_flag_t;

MEMMAN_API void* m3_memman(void* ptr, size_t new_size, m3_flag_t flag);
MEMMAN_API size_t m3_memsize(void* ptr);
#define m3_strsize(str) (m3_memsize(str) - 1)

#define m3_alloc(size) m3_memman(NULL, size, M3_NONE)
#define m3_realloc(ptr, size) m3_memman(ptr, size, M3_NONE)
#define m3_free(ptr) ((void) m3_memman(ptr, 0, M3_NONE))

#define m3_memnav(ptr, Type, offset) ((Type*)(ptr) + (offset))

#define m3_buf_tt(Type) Type*
MEMMAN_API void m3_buf_init(void** p_buf, size_t elemsize);
MEMMAN_API void m3_buf_drop(void** p_buf);
MEMMAN_API void m3_buf_clear(void** p_buf);
MEMMAN_API size_t m3_buf_size(void* buf);
MEMMAN_API uintmax_t m3_buf_count(void* buf);
MEMMAN_API void m3_buf_insert_n(void** p_buf, intmax_t idx, void* val, uintmax_t count);
MEMMAN_API void m3_buf_insert(void** p_buf, intmax_t idx, void* val, uintmax_t count);
MEMMAN_API void m3_buf_insert_back_n(void** p_buf, void* val, uintmax_t count);
MEMMAN_API void m3_buf_insert_back(void** p_buf, void* val);
MEMMAN_API void m3_buf_erase_n(void** p_buf, intmax_t idx, uintmax_t count);
MEMMAN_API void m3_buf_erase(void** p_buf, intmax_t idx, uintmax_t count);
MEMMAN_API void m3_buf_erase_back_n(void** p_buf, uintmax_t count);
MEMMAN_API void m3_buf_erase_back(void** p_buf);

MEMMAN_API const char* mem_make_str(const char* fmt, ...);
MEMMAN_API const char* mem_make_str_v(const char* fmt, va_list va);
#endif

#ifdef MEMMAN_IMPLEMENT

#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>
#include <memory.h>
#include <string.h>
#include <inttypes.h>

#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif // !max

#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif // !min

#define head(ptr) ((size_t*) (ptr) - 1)
#define CHECK_PTR_OR_RETURN(ptr) if(!ptr) return MEMERR_MEM;

static int default_err_callback(void* ud)
{
    (void) ud;
    fprintf(stderr, "MemMan out of memory");
    abort();
    return 0;
}

static struct
{
    mem_err_callback_t fn;
    void* ud;
} err_callback = {
     .fn = default_err_callback,
     .ud = NULL
};

static void makemem(void** p_ptr, size_t new_size)
{
    assert(p_ptr);

    uint8_t* p = *p_ptr;
    size_t old_size = 0;
    if(p)
    {
        old_size = mem_size(p);
        p = head(p);
    }

    if(new_size == 0)
        free(p);

    for(;;)
    {
        size_t* tmp = realloc(p, new_size + sizeof(size_t));
        if(tmp)
        {
            *tmp = new_size;
            p = tmp + 1;
            break;
        }
        err_callback.fn(err_callback.ud);
    }

    if(new_size > old_size)
        (void) memset(p + old_size, 0, new_size - old_size);

    *p_ptr = p;
}

void mem_init(void** p_ptr)
{
    assert(p_ptr);

    *p_ptr = (size_t*) calloc(1, sizeof(size_t)) + 1;
}

void mem_drop(void** p_ptr)
{
    assert(p_ptr);

    free(head(*p_ptr));
    *p_ptr = NULL;
}

size_t mem_size(void* ptr)
{
    assert(ptr);

    return *head(ptr);
}

void mem_extend(void** p_ptr, intmax_t extent)
{
    assert(p_ptr);

    if(extent == 0)
        return;

    uint8_t* p = *p_ptr;
    assert(p);

    size_t size = mem_size(p);
    size_t extended_size = imaxabs(extent);
    size_t new_size = size + extended_size;

    makemem(&p, new_size);

    void* extended_begin = p + size;
    if(extent < 0)
    {
        (void) memmove(extended_begin, p, size);
        extended_begin = p;
    }

    (void) memset(extended_begin, 0, extended_size);

    *p_ptr = p;
}

void mem_copy(void** p_dst, size_t dst_idx, const void* src, size_t src_idx, size_t size)
{
    assert(p_dst && *p_dst);
    assert(src);
}

void mem_shift(void** p_ptr, size_t dst_idx, size_t src_idx, size_t size)
{
    assert(p_ptr);
}

void mem_set_err_callback(mem_err_callback_t callback, void* userdata)
{
    err_callback.fn = callback;
    err_callback.ud = userdata;
}

#if 0
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
            (void) memset(m3_memnav(m3_memnav(tmp, size_t, 1), uint8_t, old_size), 0, new_size - old_size);

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

const char* mem_make_str(const char* fmt, ...)
{
    va_list va = { 0 };
    va_start(va, fmt);
    const char* res = mem_make_str_v(fmt, va);
    va_end(va);
    return res;
}

const char* mem_make_str_v(const char* fmt, va_list va)
{
    va_list va_tmp;
    va_copy(va_tmp, va);
    int len = vsnprintf(NULL, 0, fmt, va_tmp);
    va_end(va_tmp);

    void* str = NULL;
    makemem(&str, (len + 1) * sizeof(*fmt));

    (void) vsnprintf(str, len + 1, fmt, va);
    return str;
}
#endif

#endif // MEMMAN_IMPLEMENT

#endif // !MEMMAN_H
