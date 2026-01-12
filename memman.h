#pragma once

#ifndef MEMMAN_H
#define MEMMAN_H

#ifndef MEMMAN_API
#define MEMMAN_API
#endif // !MEMMAN_API

#include <stdint.h>

/// @addtogroup memerr
/// Error handling
/// The memman library itself does not raise any runtime error. The only possible runtime error is "out of memory" which is an OS error, in which case the callback of type @ref memerr_callback_t is called. The default callback behavior is to print error message to @a stderr and call `about()`. You can set your own callback if you can recover from such situation. If you succeed to recover from the error, just return the callback and memman will try to allocate again; If you fail to recover the error, you should abort the program manully.
/// All possible logic errors in memman are checked with @a assert in operation functions. You can and should always check before operate

/// @{

/// @brief Error callback type
/// @sa @ref memerr_set_callback
typedef void (*memerr_callback_t)(void* userdata);

/// @brief Set the error callback
/// @param[in] callback Callback function
/// @param[in] userdata Userdata
MEMMAN_API void memerr_set_callback(memerr_callback_t callback, void* userdata);

/// @}

/// @addtogroup mem
/// Function family for raw memory manupulation
/// All function starts with `mem_` prefix must apply to the memory initialized with @ref mem_init and be released with @ref mem_drop

/// @{
/// @brief A help marker which represents that this variable is allocated with @ref mem_init and thus must be freed with @ref mem_drop
/// You can just use raw pointer type if you want
#define mem_tt(Type) Type*

/// @brief Init memory
/// @param[in,out] p_ptr Reference to a pointer for the memory
/// The `*p_ptr` must either be NULL, or a memory previously initialized with @ref mem_init
MEMMAN_API void mem_init(void** p_ptr);

/// @brief Drop decorated memory
/// @param[in] p_ptr Reference to the memory
MEMMAN_API void mem_drop(void** p_ptr);

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

/// @brief Make formatted string to memory
/// @param[in,out] p_ptr Reference to the memory
/// @param[in] fmt Format
/// @param[in] ... Arguments
MEMMAN_API void mem_make_str(void** p_ptr, const char* fmt, ...);

/// @brief Make formatted string to memory
/// @param[in,out] p_ptr Reference to the memory
/// @param[in] fmt Format
/// @param[in] va Arguments
MEMMAN_API void mem_make_str_v(void** p_ptr, const char* fmt, va_list va);

/// @}

/// @addtogroup membuf
/// Function family for buffer memory manupulation
/// All function starts with `membuf_` prefix must apply to the memory initialized with @ref membuf_init and be released with @ref membuf_drop

/// @{

/// @brief A help marker which represents that this variable is allocated with @ref membuf_init and thus must be freed with @ref mem_drop
/// You can just use raw pointer type if you want
#define membuf_tt(Type) Type*

/// @brief Init buffer
/// @param[in,out] p_buf Reference to a pointer for buffer
/// @param[in] elemsize Size of element size
/// The `*p_ptr` must either be NULL, or a memory previously initialized with @ref membuf_init
MEMMAN_API void membuf_init(void** p_buf, size_t elemsize);

/// @brief Drop buffer
/// @param[in, out] p_buf Reference to buffer
MEMMAN_API void membuf_drop(void** p_buf);

/// @brief Clear buffer
/// @param[in,out] p_buf Reference to buffer
MEMMAN_API void membuf_clear(void** p_buf);

/// @brief Get the size of buffer, in byte
/// @param[in] buf Buffer
/// @return Size
MEMMAN_API size_t membuf_size(void* buf);

/// @brief Get the count of elements in buffer
/// @param[in] buf Buffer
/// @return Count
MEMMAN_API uintmax_t membuf_count(void* buf);

/// @brief Insert @a count elements at @a idx
/// @param[in,out] p_buf Reference to buffer
/// @param[in] idx Insert index
/// @param[in] val Pointer to values
/// @param[in] count Count; it is caller's duty to make sure that there are at least @a count elements in @a val
MEMMAN_API void membuf_insert_n(void** p_buf, intmax_t idx, void* val, uintmax_t count);

/// @brief Insert @a 1 elements at @a idx
/// @param[in,out] p_buf Reference to buffer
/// @param[in] idx Insert index
/// @param[in] val Pointer to values
MEMMAN_API void membuf_insert(void** p_buf, intmax_t idx, void* val);

/// @brief Insert @a count elements at back
/// @param[in,out] p_buf Reference to buffer
/// @param[in] val Pointer to values
/// @param[in] count Count; it is caller's duty to make sure that there are at least @a count elements in @a val
MEMMAN_API void membuf_insert_back_n(void** p_buf, void* val, uintmax_t count);

/// @brief Insert @a count elements at back
/// @param[in,out] p_buf Reference to buffer
/// @param[in] val Pointer to values
MEMMAN_API void membuf_insert_back(void** p_buf, void* val);

/// @brief Erase @a count elements at @a idx
/// @param[in,out] p_buf Reference to buffer
/// @param[in] idx Erase index
/// @param[in] count Count; should not be greater than @ref membuf_count
MEMMAN_API void membuf_erase_n(void** p_buf, intmax_t idx, uintmax_t count);

/// @brief Erase @a count elements at @a idx
/// @param[in,out] p_buf Reference to buffer
/// @param[in] idx Erase index
MEMMAN_API void membuf_erase(void** p_buf, intmax_t idx);

/// @brief Erase @a count elements at @a idx
/// @param[in,out] p_buf Reference to buffer
/// @param[in] count Count; should not be greater than @ref membuf_count
MEMMAN_API void membuf_erase_back_n(void** p_buf, uintmax_t count);

/// @brief Erase @a count elements at @a idx
/// @param[in,out] p_buf Reference to buffer
/// @param[in] count Count; should not be greater than @ref membuf_count
MEMMAN_API void membuf_erase_back(void** p_buf);

/// @brief Read element at @a idx
/// @param[in,out] p_buf Reference to buffer
/// @param[in] idx Index
/// @return Pointer to the element, or NULL when out of range
MEMMAN_API void* membuf_at(void** p_buf, intmax_t idx);

/// @}

#ifdef MEMMAN_IMPLEMENT

#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>
#include <memory.h>
#include <inttypes.h>
#include <string.h>

#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif // !max

#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif // !min

#define head(ptr) ((size_t*) (ptr) - 1)

static int default_err_callback(void* ud)
{
    (void) ud;
    fprintf(stderr, "[memman] Out of Memory");
    abort();
    return 0;
}

static struct
{
    memerr_callback_t fn;
    void* ud;
} err_callback = {
     .fn = default_err_callback,
     .ud = NULL
};

static void makemem(void** p_ptr, size_t new_size)
{
    assert(p_ptr);

    size_t old_size = 0;
    uint8_t* ptr = NULL;
    if(*p_ptr)
        ptr = head(*p_ptr);

    for(;;)
    {
        size_t* tmp = realloc(ptr, new_size + sizeof(size_t));
        if(tmp)
        {
            *tmp = new_size;
            ptr = tmp + 1;
            break;
        }
        err_callback.fn(err_callback.ud);
    }

    if(new_size > old_size)
        (void) memset((uint8_t*) ptr + old_size, 0, new_size - old_size);

    *p_ptr = ptr;
}

static void release(void* p)
{
    if(!p)
        return;

    free(head(p));
}

void memerr_set_callback(memerr_callback_t callback, void* userdata)
{
    err_callback.fn = callback;
    err_callback.ud = userdata;
}

void mem_init(void** p_ptr)
{
    assert(p_ptr);

    makemem(p_ptr, 0);
}

void mem_drop(void** p_ptr)
{
    assert(p_ptr);

    if(!*p_ptr)
        return;

    release(*p_ptr);
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

void mem_make_str(void** p_ptr, const char* fmt, ...)
{
    assert(p_ptr);

    va_list va = { 0 };
    va_start(va, fmt);
    mem_make_str_v(p_ptr, fmt, va);
    va_end(va);
}

void mem_make_str_v(void** p_ptr, const char* fmt, va_list va)
{
    assert(p_ptr);

    va_list va_tmp;
    va_copy(va_tmp, va);
    int len = vsnprintf(NULL, 0, fmt, va_tmp);
    va_end(va_tmp);

    makemem(p_ptr, (len + 1) * sizeof(*fmt));

    (void) vsnprintf(*p_ptr, len + 1, fmt, va);
}

static intmax_t rel2abs(intmax_t idx, uintmax_t count)
{
    if(idx >= 0)
        return idx;

    return (idx %= count) < 0 ? idx + count : idx;
}

#define bufinfocount_v (2)
#define bufinfosize_v (bufinfocount_v * sizeof(size_t))
#define bufhead(buf) ((size_t*)(buf) - 2)
#define bufsize(buf) (mem_size(bufhead(buf)))
#define bufelemsize(buf) (*bufhead(buf))
#define bufvaluesize(buf) (*((size_t*)(buf) - 1))

void membuf_init(void** p_buf, size_t elemsize)
{
    assert(p_buf);

    size_t* ptr = NULL;
    makemem(&ptr, bufinfosize_v);
    *ptr = elemsize;
    *p_buf = ptr + bufinfocount_v;
}

void membuf_drop(void** p_buf)
{
    assert(p_buf);

    if(!*p_buf)
        return;

    mem_drop(&(void*) { (size_t*) *p_buf - bufinfocount_v});
    *p_buf = NULL;
}

void membuf_clear(void** p_buf)
{
    assert(p_buf);

    if(!*p_buf)
        return;

    makemem(p_buf, 0);
}

size_t membuf_size(void* buf)
{
    assert(buf);

    return bufvaluesize(buf);
}

uintmax_t membuf_count(void* buf)
{
    assert(buf);
    assert(bufvaluesize(buf) % bufelemsize(buf) == 0);

    return bufvaluesize(buf) / bufelemsize(buf);
}

void membuf_insert_n(void** p_buf, intmax_t idx, void* val, uintmax_t count)
{
    assert(p_buf && *p_buf);
    assert(val);
    assert(count <= INTMAX_MAX);
    assert(idx >= 0 && "Pre-begin insertion is not supported yet");

    if(count == 0)
        return;

    uint8_t* buf = *p_buf;

    size_t value_size = bufvaluesize(buf);

    size_t value_size_wanted = value_size + count * bufelemsize(buf);
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
        size_t elemsize = bufelemsize(buf);
        size_t* new_head = bufhead(buf);
        makemem(&new_head, value_size + bufinfosize_v);
        *new_head = elemsize;
        *p_buf = buf = new_head + bufinfocount_v;
    }

    uint8_t* target_pos = buf + offset;
    (void) memmove(target_pos + inserted_size, target_pos, value_size - offset - inserted_size);
    (void) memcpy(target_pos, val, inserted_size);
    bufvaluesize(buf) += inserted_size;
}

void membuf_insert(void** p_buf, intmax_t idx, void* val)
{
    membuf_insert_n(p_buf, idx, val, 1);
}

void membuf_insert_back_n(void** p_buf, void* val, uintmax_t count)
{
    membuf_insert_n(p_buf, membuf_count(*p_buf), val, count);
}

void membuf_insert_back(void** p_buf, void* val)
{
    membuf_insert_back_n(p_buf, val, 1);
}

void membuf_erase_n(void** p_buf, intmax_t idx, uintmax_t count)
{
    assert(p_buf && *p_buf);
    assert(count <= INTMAX_MAX);
    assert(count <= membuf_count(*p_buf));

    if(count == 0)
        return;

    uint8_t* buf = *p_buf;

    idx = rel2abs(idx, membuf_count(buf));

    size_t value_size = bufvaluesize(buf);
    size_t offset = idx * bufelemsize(buf);
    size_t erased_size = bufelemsize(buf) * count;
    uint8_t* target_pos = buf + offset;
    size_t moved_size = value_size - offset - erased_size;
    (void) memmove(target_pos, target_pos + erased_size, moved_size);
    (void) memset(target_pos + moved_size, 0, erased_size);
    bufvaluesize(buf) -= erased_size;

    size_t value_size_wanted = bufvaluesize(buf);
    if(value_size_wanted * 2 < value_size)
    {
        value_size = min(value_size_wanted, value_size / 2);
        size_t* new_head = NULL;
        makemem(&new_head, value_size + bufinfosize_v);
        *p_buf = new_head + bufinfocount_v;
    }
}

void membuf_erase(void** p_buf, intmax_t idx)
{
    membuf_erase_n(p_buf, idx, 1);
}

void membuf_erase_back_n(void** p_buf, uintmax_t count)
{
    membuf_erase_n(p_buf, membuf_count(*p_buf) - count, count);
}

void membuf_erase_back(void** p_buf)
{
    membuf_erase_back_n(p_buf, 1);
}

void* membuf_at(void** p_buf, intmax_t idx)
{
    assert(p_buf && *p_buf);

    uint8_t* buf = *p_buf;

    idx = rel2abs(idx, membuf_count(buf));
    assert(idx >= 0);

    if(idx >= membuf_count(buf))
        return NULL;

    return buf + idx * bufelemsize(buf);
}

#endif // MEMMAN_IMPLEMENT

#endif // !MEMMAN_H
