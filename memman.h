#pragma once

#ifndef MEMMAN_H
#define MEMMAN_H

#ifndef MEMMAN_API
#define MEMMAN_API
#endif // !MEMMAN_API

#include <stdint.h>

/// @mainpage memman
/// This is a tiny single-header library for simple memory manupulation with functions provided in standard library
///
/// Usage:
/// - Include this header for declarations.
/// - Include this header with #define MEMMAN_IMPLEMENT before the #include derivative in one and only one source file for definations
///
/// Function families:
/// - @ref mem
/// - @ref membuf
/// - @ref memstr
/// - @ref memerr

/// @addtogroup memerr
/// @brief Error handling
/// @details The memman library itself does not raise any runtime error. The only possible runtime error is "out of memory" which is an OS error, in which case the callback of type @ref memerr_callback_t is called. The default callback behavior is to print error message to @a stderr and call `about()`. You can set your own callback if you can recover from such situation. If you succeed to recover from the error, just return the callback and memman will try to allocate again; If you fail to recover the error, you should abort the program manully.
///
/// All possible logic errors in memman are checked with @a assert in operation functions. You can and should always check before operate.

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
/// @brief Function family for raw memory manupulation.
/// @note All functions starting with `mem_` prefix must apply to the memory initialized with @ref mem_init and released with @ref mem_drop.

/// @{
/// @brief Flags
typedef enum mem_flag
{
    MEM_DEFAULT = 0,

    MEM_MOVE = 1 << 0,
    MEM_DISPLACE = 1 << 1,
} mem_flag_t;

/// @brief Init memory
/// @param[in,out] p_ptr Reference to a pointer for the memory
/// @param[in] init_size Init size
/// @note The `*p_ptr` must either be NULL, or a memory previously initialized with @ref mem_init
MEMMAN_API void mem_init(void** p_ptr, size_t init_size);

/// @brief Drop decorated memory
/// @param[in] p_ptr Reference to the memory
MEMMAN_API void mem_drop(void** p_ptr);

/// @brief Size of the memory
/// @param[in] ptr Memory
/// @return Size
MEMMAN_API size_t mem_size(void* ptr);

/// @brief Resize the memory
/// @param[in,out] p_buf Memmory
/// @param[in] size New size
MEMMAN_API void mem_resize(void** p_buf, size_t size);

/// @brief Extend memory after the end of the memory
/// @param[in,out] p_ptr Reference to the memory
/// @param[in] size Size
MEMMAN_API void mem_extend_back(void** p_ptr, size_t size);

/// @brief Extend memory before the begin of the memory
/// @param[in,out] p_ptr Reference to the memory
/// @param[in] size Size
MEMMAN_API void mem_extend_front(void** p_ptr, size_t size);

/// @brief Shrink memory from the end of the memory
/// @param[in,out] p_ptr Reference to the memory
/// @param[in] size Size
MEMMAN_API void mem_shrink_back(void** p_ptr, size_t size);

/// @brief Shrink memory from the begin of the memory
/// @param[in,out] p_ptr Reference to the memory
/// @param[in] size Size
MEMMAN_API void mem_shrink_front(void** p_ptr, size_t size);

/// @brief Transfer values across two memories
/// @param[in,out] p_dst Reference to the destination memory
/// @param[in] dst_idx Index of start position in destination
/// @param[in,out] p_src Reference to the source memory
/// @param[in] src_idx Index of start position in source
/// @param[in] size Size of values
/// @param[in] flag Flags:
/// - @ref MEM_MOVE Zero out the original values; the default behavior is to leave the original values as is
/// - @ref MEM_DISPLACE Displace the target values towards the end; the default behavior is to overwrite the target values
MEMMAN_API void mem_transfer(void** p_dst, size_t dst_idx, void** p_src, size_t src_idx, size_t size, mem_flag_t flag);

/// @brief Shift values in one memory
/// @param[in,out] p_ptr Reference to the memory
/// @param[in] dst_idx Index of start position of destination
/// @param[in] src_idx Index of start position of source
/// @param[in] size Size of values
/// @param[in] flag Flags
/// - @ref MEM_MOVE Zero out the original values; the default behavior is to leave the original values as is
/// - @ref MEM_DISPLACE Displace the values at target position towards the opposite direction of the shift; the default behavior is to overwrite
MEMMAN_API void mem_shift(void** p_ptr, size_t dst_idx, size_t src_idx, size_t size, mem_flag_t flag);

/// @brief Make formatted string to memory
/// @param[in,out] p_ptr Reference to the memory
/// @param[in] fmt Format
/// @param[in] ... Arguments
/// @sa For detailed string control, use @ref memstr family
MEMMAN_API void mem_make_str(void** p_ptr, const char* fmt, ...);

/// @brief Make formatted string to memory
/// @param[in,out] p_ptr Reference to the memory
/// @param[in] fmt Format
/// @param[in] va Arguments
/// @sa For detailed string control, use @ref memstr family
MEMMAN_API void mem_make_str_v(void** p_ptr, const char* fmt, va_list va);

/// @}

/// @addtogroup membuf
/// @brief Function family for buffer memory manupulation
/// @note All functions starting with `membuf_` prefix must apply to the memory initialized with @ref membuf_init and released with @ref membuf_drop

/// @{
/// @brief Init buffer
/// @param[in,out] p_buf Reference to a pointer for buffer
/// @param[in] elemsize Size of element size
/// @note The `*p_ptr` must either be NULL, or a memory previously initialized with @ref membuf_init
MEMMAN_API void membuf_init(void** p_buf, size_t elemsize);

/// @brief Drop buffer
/// @param[in, out] p_buf Reference to buffer
MEMMAN_API void membuf_drop(void** p_buf);

/// @brief Clear buffer
/// @param[in,out] p_buf Reference to buffer
MEMMAN_API void membuf_clear(void** p_buf);

/// @brief Get the total size of the buffer, in byte
/// @param[in] buf Buffer
/// @return Buffer size
MEMMAN_API size_t membuf_size(void* buf);

/// @brief Get the capacity of the buffer
/// @param[in] buf Buffer
/// @return Buffer capacity
MEMMAN_API size_t membuf_capacity(void* buf);

/// @brief Get the total size of values in the buffer, in byte
/// @param[in] buf Buffer
/// @return Buffer value size
MEMMAN_API size_t membuf_value_size(void* buf);

/// @brief Get the count of values in the buffer
/// @param[in] buf Buffer
/// @return Buffer value count
MEMMAN_API size_t membuf_value_count(void* buf);

/// @brief Insert @a count elements at @a idx
/// @param[in,out] p_buf Reference to buffer
/// @param[in] idx Insert index
/// @param[in] val Pointer to values
/// @param[in] count Count; it is caller's duty to make sure that there are at least @a count elements in @a val
MEMMAN_API void membuf_insert_n(void** p_buf, size_t idx, void* val, size_t count);

/// @brief Insert @a 1 elements at @a idx
/// @param[in,out] p_buf Reference to buffer
/// @param[in] idx Insert index
/// @param[in] val Pointer to values
MEMMAN_API void membuf_insert(void** p_buf, size_t idx, void* val);

/// @brief Insert @a count elements at back
/// @param[in,out] p_buf Reference to buffer
/// @param[in] val Pointer to values
/// @param[in] count Count; it is caller's duty to make sure that there are at least @a count elements in @a val
MEMMAN_API void membuf_insert_back_n(void** p_buf, void* val, size_t count);

/// @brief Insert @a count elements at back
/// @param[in,out] p_buf Reference to buffer
/// @param[in] val Pointer to values
MEMMAN_API void membuf_insert_back(void** p_buf, void* val);

/// @brief Erase @a count elements at @a idx
/// @param[in,out] p_buf Reference to buffer
/// @param[in] idx Erase index
/// @param[in] count Count; should not be greater than @ref membuf_value_count
MEMMAN_API void membuf_erase_n(void** p_buf, size_t idx, size_t count);

/// @brief Erase @a count elements at @a idx
/// @param[in,out] p_buf Reference to buffer
/// @param[in] idx Erase index
MEMMAN_API void membuf_erase(void** p_buf, size_t idx);

/// @brief Erase @a count elements at @a idx
/// @param[in,out] p_buf Reference to buffer
/// @param[in] count Count; should not be greater than @ref membuf_value_count
MEMMAN_API void membuf_erase_back_n(void** p_buf, size_t count);

/// @brief Erase @a count elements at @a idx
/// @param[in,out] p_buf Reference to buffer
/// @param[in] count Count; should not be greater than @ref membuf_value_count
MEMMAN_API void membuf_erase_back(void** p_buf);

/// @brief Read element at @a idx
/// @param[in] buf Buffer
/// @param[in] idx Index
/// @return Pointer to the element, or NULL when out of range
MEMMAN_API void* membuf_at(void* buf, size_t idx);

/// @brief Increase the capacity to at least @a new_cap
/// @param[in,out] p_buf Reference to buffer
/// @param[in,out] new_cap New capacity
MEMMAN_API void membuf_reserve(void** p_buf, size_t new_cap);

/// @brief Reduce the capacity to element count
/// @param[in,out] p_buf Reference to buffer
MEMMAN_API void membuf_shrink_to_fit(void** p_buf);

/// @}

/// @addtogroup memstr
/// @brief Function family for string memory manupulation
/// @note All functions starting with `memstr_` prefix must apply to the memory initialized with @ref memstr_init and released with @ref memstr_drop

/// @{
/// @brief Init string
/// @param[in,out] p_str Reference to a pointer for pointer
/// @note The `*p_str` must either be NULL, or a string previously initialized with @ref memstr_init
MEMMAN_API void memstr_init(char** p_str);

/// @brief Drop string
/// @param[in,out] p_str Reference to pointer
MEMMAN_API void memstr_drop(char** p_str);

/// @brief Get the size of the string memory
/// @param[in] str String
/// @return Size
MEMMAN_API size_t memstr_size(const char* str);

/// @brief Get the length of the string, excluding null-terminator
/// @param[in] str String
/// @return Length
MEMMAN_API size_t memstr_len(const char* str);

/// @brief Append formatted string to current string
/// @param[in,out] p_str Reference to string
/// @param[in] fmt Format
/// @param[in] ... Arguemnts
MEMMAN_API void memstr_append(char** p_str, const char* fmt, ...);

/// @brief Append formatted string to current string
/// @param[in,out] p_str Reference to string
/// @param[in] fmt Format
/// @param[in] va Arguemnts
MEMMAN_API void memstr_append_v(char** p_str, const char* fmt, va_list va);

/// @brief Append a single character to current string
/// @param[in,out] p_str Reference to string
/// @param[in] c Character
MEMMAN_API void memstr_append_char(char** p_str, char c);

/// @brief Reduce @a len characters from current string
/// @param[in,out] p_str Reference to string
/// @param[in] len Length
MEMMAN_API void memstr_reduce(char** p_str, size_t len);

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

#define ptr2head(ptr) ((size_t*) (ptr) - 1)

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

static void acquire(void** p_ptr, size_t new_size)
{
    assert(p_ptr);

    size_t old_size = 0;
    uint8_t* ptr = NULL;
    if(*p_ptr)
    {
        ptr = ptr2head(*p_ptr);
        old_size = *((size_t*) ptr);
    }

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
        (void) memset(ptr + old_size, 0, new_size - old_size);

    *p_ptr = ptr;
}

static void release(void* p)
{
    if(!p)
        return;

    free(ptr2head(p));
}

void memerr_set_callback(memerr_callback_t callback, void* userdata)
{
    err_callback.fn = callback;
    err_callback.ud = userdata;
}

void mem_init(void** p_ptr, size_t init_size)
{
    assert(p_ptr);

    acquire(p_ptr, init_size);
}

void mem_drop(void** p_ptr)
{
    assert(p_ptr);

    release(*p_ptr);
    *p_ptr = NULL;
}

size_t mem_size(void* ptr)
{
    assert(ptr);

    return *ptr2head(ptr);
}

void mem_resize(void** p_buf, size_t size)
{
    assert(p_buf);

    acquire(p_buf, size);
}

void mem_extend_back(void** p_ptr, size_t size)
{
    assert(p_ptr && *p_ptr);

    if(size == 0)
        return;

    acquire(p_ptr, mem_size(*p_ptr) + size);
}

void mem_extend_front(void** p_ptr, size_t size)
{
    assert(p_ptr && *p_ptr);

    mem_extend_back(p_ptr, size);
    mem_shift(p_ptr, size, 0, size, MEM_MOVE);
}

void mem_shrink_back(void** p_ptr, size_t size)
{
    assert(p_ptr && *p_ptr);
    assert(size <= mem_size(*p_ptr));

    if(size == 0)
        return;

    acquire(p_ptr, mem_size(*p_ptr) - size);
}

void mem_shrink_front(void** p_ptr, size_t size)
{
    assert(p_ptr && *p_ptr);
    assert(size <= mem_size(*p_ptr));

    mem_shift(p_ptr, 0, size, size, MEM_DEFAULT);
    mem_shrink_back(p_ptr, size);
}

void mem_transfer(void** p_dst, size_t dst_idx, void** p_src, size_t src_idx, size_t size, mem_flag_t flag)
{
    assert(p_dst && *p_dst);
    assert(p_src && *p_src);
    assert(*p_dst != *p_src && "Use mem_shift for operations on a single memory");
    assert(src_idx + size <= mem_size(*p_src) && "Source values out of range");

    if(size == 0)
        return;

    uint8_t* dst = *p_dst;
    uint8_t* src = *p_src;

    size_t dst_remaining_size = mem_size(dst) - dst_idx;
    size_t extend_size = size;
    if(!(flag & MEM_DISPLACE))
        extend_size = size < dst_remaining_size ? 0 : size - dst_remaining_size;

    if(extend_size > 0)
    {
        mem_extend_back(&dst, extend_size);
        *p_dst = dst;
    }

    if(flag & MEM_DISPLACE)
        (void) memcpy(dst + dst_idx + size, dst + dst_idx, dst_remaining_size);

    (void) memcpy(dst + dst_idx, src + src_idx, size);

    if(flag & MEM_MOVE)
        (void) memset(src + src_idx, 0, size);
}

void mem_shift(void** p_ptr, size_t dst_idx, size_t src_idx, size_t size, mem_flag_t flag)
{
    assert(p_ptr && *p_ptr);
    assert(src_idx + size <= mem_size(*p_ptr) && "Source values out of range");

    if(size == 0 || dst_idx == src_idx)
        return;

    uint8_t* ptr = *p_ptr;
    if(flag & MEM_DISPLACE)
    {
        size_t tmp_size = 0;
        uint8_t* tmp_src = NULL;
        uint8_t* tmp_dst = NULL;
        if(dst_idx > src_idx)
        {
            tmp_size = dst_idx - src_idx;
            tmp_src = ptr + src_idx + size;
            tmp_dst = ptr + src_idx;
        }
        else
        {
            tmp_size = src_idx - dst_idx;
            tmp_src = ptr + dst_idx;
            tmp_dst = ptr + dst_idx + size;
        }

        void* tmp = NULL;
        acquire(&tmp, tmp_size);
        (void) memcpy(tmp, tmp_src, tmp_size);
        (void) memmove(ptr + dst_idx, ptr + src_idx, size);
        (void) memcpy(tmp_dst, tmp, tmp_size);
        release(&tmp);
        return;
    }

    size_t dst_remaining_size = mem_size(ptr) - dst_idx;
    size_t extend_size = size < dst_remaining_size ? 0 : size - dst_remaining_size;
    if(extend_size > 0)
    {
        mem_extend_back(&ptr, extend_size);
        *p_ptr = ptr;
    }

    (void) memmove(ptr + dst_idx, ptr + src_idx, size);
    if(flag & MEM_MOVE)
    {
        (void) memset(ptr + src_idx, 0, dst_idx > src_idx ? dst_idx - src_idx : src_idx - dst_idx);
    }
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

    acquire(p_ptr, (len + 1) * sizeof(*fmt));

    (void) vsnprintf(*p_ptr, len + 1, fmt, va);
}

static intmax_t rel2abs(intmax_t idx, size_t count)
{
    if(idx >= 0)
        return idx;

    return (idx %= count) < 0 ? idx + count : idx;
}

#ifndef MEMBUF_DEFAULT_CAPACITY
#define MEMBUF_DEFAULT_CAPACITY 8
#endif // !MEMBUF_DEFAULT_CAPACITY

#define bufinfosize_v (2 * sizeof(size_t))
#define bufsize(buf) (mem_size(buf) - bufinfosize_v)
#define bufinfo(buf) ((uint8_t*)(buf) + bufsize(buf))
#define bufelemsize(buf) (*bufinfo(buf))
#define bufvaluesize(buf) (*(bufinfo(buf) + sizeof(size_t)))

void membuf_init(void** p_buf, size_t elemsize)
{
    assert(p_buf);

    mem_init(p_buf, bufinfosize_v);
    bufelemsize(*p_buf) = elemsize;
}

void membuf_drop(void** p_buf)
{
    assert(p_buf);

    mem_drop(p_buf);
}

void membuf_clear(void** p_buf)
{
    assert(p_buf);

    if(!*p_buf)
        return;

    uint8_t* buf = *p_buf;
    (void) memset(buf, 0, bufvaluesize(buf));
    bufvaluesize(buf) = 0;
}

size_t membuf_size(void* buf)
{
    assert(buf);

    return mem_size(buf) - bufinfosize_v;
}

size_t membuf_capacity(void* buf)
{
    assert(buf);

    return membuf_size(buf) / bufelemsize(buf);
}

size_t membuf_value_size(void* buf)
{
    assert(buf);

    return bufvaluesize(buf);
}

size_t membuf_value_count(void* buf)
{
    assert(buf);

    return bufvaluesize(buf) / bufelemsize(buf);
}

void membuf_insert_n(void** p_buf, size_t idx, void* val, size_t count)
{
    assert(p_buf && *p_buf);
    assert(val);
    assert(count <= INTMAX_MAX);

    if(count == 0)
        return;

    uint8_t* buf = *p_buf;

    size_t value_size = bufvaluesize(buf);
    size_t inserted_size = bufelemsize(buf) * count;
    size_t value_size_wanted = value_size + inserted_size;
    size_t old_size = bufsize(buf);

    if(value_size_wanted >= old_size)
    {
        size_t new_size = old_size > 0 ? old_size * 2 : MEMBUF_DEFAULT_CAPACITY * bufelemsize(buf);
        while(value_size_wanted > new_size)
            new_size *= 2;
        mem_resize(&buf, new_size + bufinfosize_v);
        (void) memmove(buf + new_size, buf + old_size, bufinfosize_v);
        (void) memset(buf + old_size, 0, bufinfosize_v);
        *p_buf = buf;
    }

    size_t offset = idx * bufelemsize(buf);
    uint8_t* target_pos = buf + offset;
    (void) memmove(target_pos + inserted_size, target_pos, value_size - offset);
    (void) memcpy(target_pos, val, inserted_size);
    bufvaluesize(buf) += inserted_size;
}

void membuf_insert(void** p_buf, size_t idx, void* val)
{
    membuf_insert_n(p_buf, idx, val, 1);
}

void membuf_insert_back_n(void** p_buf, void* val, size_t count)
{
    membuf_insert_n(p_buf, membuf_value_count(*p_buf), val, count);
}

void membuf_insert_back(void** p_buf, void* val)
{
    membuf_insert_back_n(p_buf, val, 1);
}

void membuf_erase_n(void** p_buf, size_t idx, size_t count)
{
    assert(p_buf && *p_buf);
    assert(bufvaluesize(*p_buf) > 0);
    assert(count <= membuf_value_count(*p_buf));

    if(count == 0)
        return;

    uint8_t* buf = *p_buf;

    size_t value_size = bufvaluesize(buf);
    size_t offset = idx * bufelemsize(buf);
    size_t erased_size = bufelemsize(buf) * count;
    uint8_t* target_pos = buf + offset;
    size_t moved_size = value_size - offset - erased_size;
    (void) memmove(target_pos, target_pos + erased_size, moved_size);
    (void) memset(target_pos + moved_size, 0, erased_size);
    bufvaluesize(buf) -= erased_size;

    size_t old_size = bufsize(buf);
    size_t value_size_wanted = bufvaluesize(buf);
    if(value_size_wanted < old_size / 2)
    {
        size_t new_size = old_size / 2;
        while(value_size_wanted < new_size / 2)
            new_size /= 2;
        (void) memmove(buf + new_size, buf + old_size, bufinfosize_v);
        mem_resize(&buf, new_size + bufinfosize_v);
        *p_buf = buf;
    }
}

void membuf_erase(void** p_buf, size_t idx)
{
    membuf_erase_n(p_buf, idx, 1);
}

void membuf_erase_back_n(void** p_buf, size_t count)
{
    membuf_erase_n(p_buf, membuf_value_count(*p_buf) - count, count);
}

void membuf_erase_back(void** p_buf)
{
    membuf_erase_back_n(p_buf, 1);
}

void* membuf_at(void* buf, size_t idx)
{
    assert(buf);

    if(idx >= membuf_value_count(buf))
        return NULL;

    return (uint8_t*) buf + idx * bufelemsize(buf);
}

void membuf_reserve(void** p_buf, size_t new_cap)
{
    assert(p_buf && *p_buf);

    uint8_t* buf = *p_buf;

    size_t size_wanted = new_cap * bufelemsize(buf);
    size_t old_size = bufsize(buf);
    if(size_wanted > old_size)
    {
        mem_resize(&buf, size_wanted + bufinfosize_v);
        (void) memmove(buf + size_wanted, buf + old_size, bufinfosize_v);
        (void) memset(buf + old_size, 0, bufinfosize_v);
        *p_buf = buf;
    }
}

void membuf_shrink_to_fit(void** p_buf)
{
    assert(p_buf && *p_buf);

    uint8_t* buf = *p_buf;

    (void) memmove(buf + bufvaluesize(buf), buf + bufsize(buf), bufinfosize_v);
    mem_resize(&buf, bufvaluesize(buf) + bufinfosize_v);
}

#ifndef MEMSTR_EXTEND_STEP
#define MEMSTR_EXTEND_STEP 8
#endif // !MEMSTR_EXTEND_STEP

#define strinfosize_v (sizeof(char) + sizeof(size_t)) // Null-terminator included
#define strsize(str) (mem_size(str) - strinfosize_v)
#define strinfo(str) ((uint8_t*)(str) + strsize(str))
#define strcount(str) (*(strinfo(str) + sizeof(char)))

void memstr_init(char** p_str)
{
    assert(p_str);

    mem_init(p_str, strinfosize_v);
}

void memstr_drop(char** p_str)
{
    assert(p_str);

    mem_drop(p_str);
}

size_t memstr_size(const char* str)
{
    assert(str);

    return strsize(str);
}

size_t memstr_len(const char* str)
{
    assert(str);

    return strcount(str);
}

void memstr_append(char** p_str, const char* fmt, ...)
{
    assert(p_str && *p_str);

    va_list va;
    va_start(va, fmt);
    memstr_append_v(p_str, fmt, va);
    va_end(va);
}

void memstr_append_v(char** p_str, const char* fmt, va_list va)
{
    assert(p_str && *p_str);

    va_list va_tmp;
    va_copy(va_tmp, va);
    int extend_len = vsnprintf(NULL, 0, fmt, va_tmp);
    va_end(va_tmp);

    char* str = *p_str;
    size_t old_size = strsize(str);
    size_t new_size = old_size + extend_len;

    mem_resize(&str, new_size + strinfosize_v);
    (void) memmove(str + new_size, str + old_size, strinfosize_v);
    (void) memset(str + old_size, 0, strinfosize_v);

    (void) vsnprintf(str + strcount(str), extend_len + 1, fmt, va);
    strcount(str) += extend_len;

    *p_str = str;
}

void memstr_append_char(char** p_str, char c)
{
    assert(p_str && *p_str);

    char* str = *p_str;
    size_t old_len = strcount(*p_str);
    size_t old_size = strsize(str);

    if(old_len + 1 >= strsize(str))
    {
        size_t new_size = old_size + MEMSTR_EXTEND_STEP * sizeof(char);
        mem_resize(&str, new_size + strinfosize_v);
        (void) memmove(str + new_size, str + old_size, strinfosize_v);
        (void) memset(str + old_size, 0, strinfosize_v);
        *p_str = str;
    }

    str[old_len] = c;
    strcount(str) += 1;
}

void memstr_reduce(char** p_str, size_t len)
{
    assert(p_str && *p_str);

    if(len == 0)
        return;

    char* str = *p_str;
    assert(len <= strcount(str));

    size_t old_len = strcount(str);
    size_t len_wanted = old_len - len;
    size_t old_size = strsize(str);

    strcount(str) -= len;
    (void) memset(str + strcount(str), 0, len);

    if(len_wanted < old_size / 2)
    {
        size_t new_size = old_size / 2;
        while(len_wanted < new_size / 2)
            new_size /= 2;
        (void) memmove(str + new_size, str + old_size, strinfosize_v);
        mem_resize(&str, new_size + strinfosize_v);
        *p_str = str;
    }
}

#endif // MEMMAN_IMPLEMENT

#endif // !MEMMAN_H
