#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#define __VEC_NEW_IMPL(tmpl_name, type, destructor)        \
    UNUSED static inline tmpl_name tmpl_name##_new(void) { \
        tmpl_name vec;                                     \
        vec.size = vec.capacity = 0u;                      \
        vec.data = NULL;                                   \
        return vec;                                        \
    }

#define __VEC_DEL_IMPL(tmpl_name, type, destructor)             \
    UNUSED static inline void tmpl_name##_del(tmpl_name* vec) { \
        __##tmpl_name##_call_destructor(vec, 0, vec->size);     \
        free(vec->data);                                        \
        vec->data = NULL;                                       \
        vec->size = vec->capacity = 0;                          \
    }

#define __VEC_SIZE_IMPL(tmpl_name) \
    UNUSED static inline size_t tmpl_name##_size(const tmpl_name* vec) { return vec->size; }

#define __VEC_CAPACITY_IMPL(tmpl_name) \
    UNUSED static inline size_t tmpl_name##_capacity(const tmpl_name* vec) { return vec->capacity; }

#define __VEC_DATA_IMPL(tmpl_name, type) \
    UNUSED static inline type* tmpl_name##_data(const tmpl_name* vec) { return vec->data; }

#define __VEC_RESIZE_IMPL(tmpl_name, type, destructor)                          \
    UNUSED static inline void tmpl_name##_resize(tmpl_name* vec, size_t size) { \
        if (size == 0u) {                                                       \
            tmpl_name##_del(vec);                                               \
            return;                                                             \
        }                                                                       \
        if (size < vec->size) {                                                 \
            __##tmpl_name##_call_destructor(vec, size, vec->size);              \
        }                                                                       \
        vec->data = realloc(vec->data, sizeof(type) * size);                    \
        vec->size = vec->capacity = size;                                       \
    }

#define __VEC_RESERVE_IMPL(tmpl_name, type, destructor)                              \
    UNUSED static inline void tmpl_name##_reserve(tmpl_name* vec, size_t capacity) { \
        if (capacity < vec->capacity) {                                              \
            return;                                                                  \
        }                                                                            \
        size_t old_size = vec->size;                                                 \
        tmpl_name##_resize(vec, capacity);                                           \
        vec->size = old_size;                                                        \
    }

#define __VEC_PUSH_BACK_IMPL(tmpl_name, type, destructor)                       \
    UNUSED static inline void tmpl_name##_push_back(tmpl_name* vec, type val) { \
        tmpl_name##_push_back_ptr(vec, (const type*)&val);                      \
    }

#define __VEC_PUSH_BACK_PTR_IMPL(tmpl_name, type, destructor)                              \
    UNUSED static inline void tmpl_name##_push_back_ptr(tmpl_name* vec, const type* val) { \
        __##tmpl_name##_try_inc_capacity(vec, vec->size + 1);                              \
        memcpy(vec->data + vec->size, val, sizeof(type));                                  \
        vec->size++;                                                                       \
    }

#define __VEC_POP_BACK_IMPL(tmpl_name, type, destructor)                \
    UNUSED static inline void tmpl_name##_pop_back(tmpl_name* vec) {    \
        __##tmpl_name##_call_destructor(vec, vec->size - 1, vec->size); \
        vec->size--;                                                    \
    }

#define __VEC_PUSH_FRONT_IMPL(tmpl_name, type, destructor)                       \
    UNUSED static inline void tmpl_name##_push_front(tmpl_name* vec, type val) { \
        tmpl_name##_push_front_ptr(vec, (const type*)&val);                      \
    }

#define __VEC_PUSH_FRONT_PTR_IMPL(tmpl_name, type, destructor)                              \
    UNUSED static inline void tmpl_name##_push_front_ptr(tmpl_name* vec, const type* val) { \
        __##tmpl_name##_try_inc_capacity(vec, vec->size + 1);                               \
        __##tmpl_name##_move_content(vec, 0, 1, vec->size);                                 \
        memcpy(vec->data, val, sizeof(type));                                               \
        vec->size++;                                                                        \
    }

#define __VEC_POP_FRONT_IMPL(tmpl_name, type, destructor)             \
    UNUSED static inline void tmpl_name##_pop_front(tmpl_name* vec) { \
        vec->size--;                                                  \
        __##tmpl_name##_call_destructor(vec, 0u, 1u);                 \
        __##tmpl_name##_move_content(vec, 1, -1, vec->size);          \
    }

#define __VEC_APPEND_BEFORE_PTR_IMPL(tmpl_name, type, destructor)                                            \
    UNUSED static inline void tmpl_name##_append_before_ptr(tmpl_name* vec, size_t index, const type* val) { \
        if (index == END) {                                                                                  \
            tmpl_name##_push_back_ptr(vec, val);                                                             \
            return;                                                                                          \
        }                                                                                                    \
        __##tmpl_name##_try_inc_capacity(vec, vec->size + 1);                                                \
        __##tmpl_name##_move_content(vec, index, 1, vec->size - index);                                      \
        memcpy(vec->data + index, val, sizeof(type));                                                        \
        vec->size++;                                                                                         \
    }

#define __VEC_APPEND_BEFORE_IMPL(tmpl_name, type, destructor)                                     \
    UNUSED static inline void tmpl_name##_append_before(tmpl_name* vec, size_t index, type val) { \
        tmpl_name##_append_before_ptr(vec, index, (const type*)&val);                             \
    }

#define __VEC_AT_IMPL(tmpl_name, type, destructor) \
    UNUSED static inline type* tmpl_name##_at(const tmpl_name* vec, size_t index) { return vec->data + index; }

#define __VEC_FRONT_IMPL(tmpl_name, type, destructor) \
    UNUSED static inline type* tmpl_name##_front(const tmpl_name* vec) { return vec->data; }

#define __VEC_BACK_IMPL(tmpl_name, type, destructor) \
    UNUSED static inline type* tmpl_name##_back(const tmpl_name* vec) { return vec->data + vec->size - 1; }

#define __VEC_REMOVE_RANGE_IMPL(tmpl_name, type, destructor)                                     \
    UNUSED static inline void tmpl_name##_remove_range(tmpl_name* vec, size_t from, size_t to) { \
        __##tmpl_name##_call_destructor(vec, from, to);                                          \
        int offset = to - from;                                                                  \
        size_t rest = vec->size - to;                                                            \
        __##tmpl_name##_move_content(vec, to, -offset, rest);                                    \
        vec->size -= offset;                                                                     \
    }

#define __VEC_REMOVE_AT_IMPL(tmpl_name, type, destructor)                           \
    UNUSED static inline void tmpl_name##_remove_at(tmpl_name* vec, size_t index) { \
        tmpl_name##_remove_range(vec, index, index + 1);                            \
    }

#define __VEC_SHRINK_TO_FIT_IMPL(tmpl_name, type, destructor) \
    UNUSED static inline void tmpl_name##_shrink_to_fit(tmpl_name* vec) { tmpl_name##_resize(vec, vec->size); }

#define __VEC_FOREACH_IMPL(tmpl_name, type, destructor)                                                         \
    UNUSED static inline void tmpl_name##_foreach(const tmpl_name* vec, bool (*callback)(type*, size_t, void*), \
                                                  void* userdata) {                                             \
        for (size_t i = 0u; i < vec->size; i++) {                                                               \
            if (!callback(vec->data + i, i, userdata)) {                                                        \
                break;                                                                                          \
            }                                                                                                   \
        }                                                                                                       \
    }

#define __VEC_FIND_IMPL(tmpl_name, type, destructor)                                                  \
    UNUSED static inline size_t tmpl_name##_find(const tmpl_name* vec, bool (*pred)(const type* t)) { \
        for (size_t i = 0u; i < vec->size; i++) {                                                     \
            if (pred((const type*)vec->data + i)) {                                                   \
                return i;                                                                             \
            }                                                                                         \
        }                                                                                             \
        return NOT_FOUND;                                                                             \
    }

#define __VEC_CMP_IMPL(tmpl_name, type, destructor)                                             \
    UNUSED static inline bool tmpl_name##_cmp(const tmpl_name* vec_a, const tmpl_name* vec_b) { \
        if (vec_a->size != vec_b->size) {                                                       \
            return false;                                                                       \
        }                                                                                       \
        return memcmp(vec_a->data, vec_b->data, vec_a->size) == 0;                              \
    }

#define __VEC_CMP_FN_IMPL(tmpl_name, type, destructor)                                           \
    UNUSED static inline bool tmpl_name##_cmp_fn(const tmpl_name* vec_a, const tmpl_name* vec_b, \
                                                 bool (*cmp)(const type*, const type*)) {        \
        if (vec_a->size != vec_b->size) {                                                        \
            return false;                                                                        \
        }                                                                                        \
        for (size_t i = 0u; i < vec_a->size; i++) {                                              \
            if (!cmp((const type*)vec_a->data + i, (const type*)vec_b->data + i)) {              \
                return false;                                                                    \
            }                                                                                    \
        }                                                                                        \
        return true;                                                                             \
    }

#define __VEC_TRY_INC_CAPACITY_IMPL(tmpl_name, type, destructor)                                         \
    UNUSED static inline void __##tmpl_name##_try_inc_capacity(tmpl_name* vec, size_t capacity) {        \
        if (capacity >= vec->capacity) {                                                                 \
            size_t new_capacity = capacity > (vec->capacity * 2) ? (capacity * 2) : (vec->capacity * 2); \
            tmpl_name##_reserve(vec, new_capacity);                                                      \
        }                                                                                                \
    }

#define __VEC_CALL_DESTRUCTOR_IMPL(tmpl_name, type, destructor)                                         \
    UNUSED static inline void __##tmpl_name##_call_destructor(tmpl_name* vec, size_t from, size_t to) { \
        void (*destructor_fn)(type*) = destructor;                                                      \
        if (destructor_fn) {                                                                            \
            for (size_t i = from; i < to; i++) {                                                        \
                destructor_fn(vec->data + i);                                                           \
            }                                                                                           \
        }                                                                                               \
    }

#define __VEC_MOVE_CONTENT_IMPL(tmpl_name, type, destructor)                                                        \
    UNUSED static inline void __##tmpl_name##_move_content(tmpl_name* vec, size_t index, int offset, size_t size) { \
        if (size == 0u) {                                                                                           \
            return;                                                                                                 \
        }                                                                                                           \
        memmove(vec->data + index + offset, vec->data + index, size * sizeof(type));                                \
    }

#define __VEC_CLONE_IMPL(tmpl_name, type, destructor)                        \
    UNUSED static inline tmpl_name tmpl_name##_clone(const tmpl_name* vec) { \
        tmpl_name cl = {.size = vec->size, .capacity = vec->capacity};       \
        cl.data = malloc(sizeof(type) * cl.capacity);                        \
        memcpy(cl.data, vec->data, sizeof(type) * cl.size);                  \
        return cl;                                                           \
    }

#define VEC(tmpl_name, type, destructor)                      \
    typedef struct {                                          \
        size_t size;                                          \
        size_t capacity;                                      \
        type* data;                                           \
    } tmpl_name;                                              \
    __VEC_CALL_DESTRUCTOR_IMPL(tmpl_name, type, destructor)   \
    __VEC_MOVE_CONTENT_IMPL(tmpl_name, type, destructor)      \
    __VEC_NEW_IMPL(tmpl_name, type, destructor)               \
    __VEC_DEL_IMPL(tmpl_name, type, destructor)               \
    __VEC_SIZE_IMPL(tmpl_name)                                \
    __VEC_CAPACITY_IMPL(tmpl_name)                            \
    __VEC_DATA_IMPL(tmpl_name, type)                          \
    __VEC_RESIZE_IMPL(tmpl_name, type, destructor)            \
    __VEC_RESERVE_IMPL(tmpl_name, type, destructor)           \
    __VEC_TRY_INC_CAPACITY_IMPL(tmpl_name, type, destructor)  \
    __VEC_PUSH_BACK_PTR_IMPL(tmpl_name, type, destructor)     \
    __VEC_PUSH_BACK_IMPL(tmpl_name, type, destructor)         \
    __VEC_POP_BACK_IMPL(tmpl_name, type, destructor)          \
    __VEC_PUSH_FRONT_PTR_IMPL(tmpl_name, type, destructor)    \
    __VEC_PUSH_FRONT_IMPL(tmpl_name, type, destructor)        \
    __VEC_POP_FRONT_IMPL(tmpl_name, type, destructor)         \
    __VEC_APPEND_BEFORE_PTR_IMPL(tmpl_name, type, destructor) \
    __VEC_APPEND_BEFORE_IMPL(tmpl_name, type, destructor)     \
    __VEC_AT_IMPL(tmpl_name, type, destructor)                \
    __VEC_FRONT_IMPL(tmpl_name, type, destructor)             \
    __VEC_BACK_IMPL(tmpl_name, type, destructor)              \
    __VEC_REMOVE_RANGE_IMPL(tmpl_name, type, destructor)      \
    __VEC_REMOVE_AT_IMPL(tmpl_name, type, destructor)         \
    __VEC_FIND_IMPL(tmpl_name, type, destructor)              \
    __VEC_CMP_IMPL(tmpl_name, type, destructor)               \
    __VEC_CMP_FN_IMPL(tmpl_name, type, destructor)            \
    __VEC_SHRINK_TO_FIT_IMPL(tmpl_name, type, destructor)     \
    __VEC_FOREACH_IMPL(tmpl_name, type, destructor)           \
    __VEC_CLONE_IMPL(tmpl_name, type, destructor)
