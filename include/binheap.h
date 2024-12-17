#pragma once

#include <stdbool.h>
#include <stdlib.h>

#include "vec.h"

#define __BINHEAP_PARENT_INDEX(index) (((index) - 1) / 2)
#define __BINHEAP_LEFT_CHILD_INDEX(index) ((index) * 2 + 1)
#define __BINHEAP_RIGHT_CHILD_INDEX(index) ((index) * 2 + 2)

#define __BINHEAP_NEW_IMPL(tmpl_name)                      \
    UNUSED static inline tmpl_name tmpl_name##_new(void) { \
        tmpl_name bh;                                      \
        bh.heap = __##tmpl_name##_vec_new();               \
        return bh;                                         \
    }

#define __BINHEAP_DEL_IMPL(tmpl_name, type, destructor)                                      \
    UNUSED static inline void tmpl_name##_del(tmpl_name *bh) {                               \
        /* call destructor manually because underlying vector does not have a destructor */  \
        /* otherwise vector's pop would free the memory everytime and we do not want this */ \
        void (*elem_destructor)(type *) = destructor;                                        \
        if (elem_destructor != NULL) {                                                       \
            for (size_t i = 0u; i < __##tmpl_name##_vec_size(&bh->heap); i++) {              \
                elem_destructor(__##tmpl_name##_vec_at(&bh->heap, i));                       \
            }                                                                                \
        }                                                                                    \
        __##tmpl_name##_vec_del(&bh->heap);                                                  \
    }

#define __BINHEAP_NUM_ELEMS_IMPL(tmpl_name)                                  \
    UNUSED static inline size_t tmpl_name##_num_elems(const tmpl_name *bh) { \
        return __##tmpl_name##_vec_size(&bh->heap);                          \
    }                                                                        \
    UNUSED static inline size_t tmpl_name##_size(const tmpl_name *bh) { return __##tmpl_name##_vec_size(&bh->heap); }

#define __BINHEAP_HEAD_IMPL(tmpl_name, type) \
    UNUSED static inline type tmpl_name##_head(const tmpl_name *bh) { return *__##tmpl_name##_vec_front(&bh->heap); }

#define __BINHEAP_SWAP_IMPL(tmpl_name, type)                                              \
    UNUSED static inline void __##tmpl_name##_swap(tmpl_name *bh, size_t ia, size_t ib) { \
        type *elem_a = __##tmpl_name##_vec_at(&bh->heap, ia);                             \
        type *elem_b = __##tmpl_name##_vec_at(&bh->heap, ib);                             \
        type tmp = *elem_a;                                                               \
        *elem_a = *elem_b;                                                                \
        *elem_b = tmp;                                                                    \
    }

#define __BINHEAP_BUBBLE_UP_IMPL(tmpl_name, type, cmp)                                        \
    UNUSED static inline void __##tmpl_name##_bubble_up(tmpl_name *bh, size_t index) {        \
        bool (*comparator)(const type *, const type *) = cmp;                                 \
        size_t parent = __BINHEAP_PARENT_INDEX(index);                                        \
        while (parent < index) {                                                              \
            const type *val_index = (const type *)__##tmpl_name##_vec_at(&bh->heap, index);   \
            const type *val_parent = (const type *)__##tmpl_name##_vec_at(&bh->heap, parent); \
            if (!comparator(val_index, val_parent)) {                                         \
                break;                                                                        \
            }                                                                                 \
            __##tmpl_name##_swap(bh, index, parent);                                          \
            index = parent;                                                                   \
            parent = __BINHEAP_PARENT_INDEX(parent);                                          \
        }                                                                                     \
    }

#define __BINHEAP_BUBBLE_DOWN_IMPL(tmpl_name, type, cmp)                                    \
    UNUSED static inline void __##tmpl_name##_bubble_down(tmpl_name *bh, size_t index) {    \
        bool (*comparator)(const type *, const type *) = cmp;                               \
        size_t heap_size = __##tmpl_name##_vec_size(&bh->heap);                             \
        while (index < heap_size) {                                                         \
            size_t parent = index;                                                          \
            size_t left = __BINHEAP_LEFT_CHILD_INDEX(index);                                \
            size_t right = __BINHEAP_RIGHT_CHILD_INDEX(index);                              \
            const type *val_index = (const type *)__##tmpl_name##_vec_at(&bh->heap, index); \
            const type *val_left = (const type *)__##tmpl_name##_vec_at(&bh->heap, left);   \
            const type *val_right = (const type *)__##tmpl_name##_vec_at(&bh->heap, right); \
            bool go_left = left < heap_size && comparator(val_left, val_index);             \
            bool go_right = right < heap_size && comparator(val_right, val_index);          \
            if (go_left && go_right) {                                                      \
                index = comparator(val_left, val_right) ? left : right;                     \
            } else if (go_left) {                                                           \
                index = left;                                                               \
            } else if (go_right) {                                                          \
                index = right;                                                              \
            } else {                                                                        \
                break;                                                                      \
            }                                                                               \
            __##tmpl_name##_swap(bh, parent, index);                                        \
        }                                                                                   \
    }

#define __BINHEAP_PUSH_PTR_IMPL(tmpl_name, type, cmp)                                \
    UNUSED static inline void tmpl_name##_push_ptr(tmpl_name *bh, const type *val) { \
        __##tmpl_name##_vec_push_back_ptr(&bh->heap, val);                           \
        __##tmpl_name##_bubble_up(bh, __##tmpl_name##_vec_size(&bh->heap) - 1u);     \
    }

#define __BINHEAP_PUSH_IMPL(tmpl_name, type, cmp)                         \
    UNUSED static inline void tmpl_name##_push(tmpl_name *bh, type val) { \
        tmpl_name##_push_ptr(bh, (const type *)&val);                     \
    }

#define __BINHEAP_POP_IMPL(tmpl_name, type, cmp)               \
    UNUSED static inline type tmpl_name##_pop(tmpl_name *bh) { \
        type *front = __##tmpl_name##_vec_front(&bh->heap);    \
        type *back = __##tmpl_name##_vec_back(&bh->heap);      \
        type res = *front;                                     \
        *front = *back;                                        \
        __##tmpl_name##_vec_pop_back(&bh->heap);               \
        __##tmpl_name##_bubble_down(bh, 0u);                   \
        return res;                                            \
    }

#define __BINHEAP_CLONE_IMPL(tmpl_name)                                     \
    UNUSED static inline tmpl_name tmpl_name##_clone(const tmpl_name *bh) { \
        tmpl_name cl = {.heap = __##tmpl_name##_vec_clone(&bh->heap)};      \
        return cl;                                                          \
    }

#define BINHEAP(tmpl_name, type, cmp, destructor)    \
    VEC(__##tmpl_name##_vec, type, NULL)             \
    typedef struct {                                 \
        __##tmpl_name##_vec heap;                    \
    } tmpl_name;                                     \
    __BINHEAP_NEW_IMPL(tmpl_name)                    \
    __BINHEAP_DEL_IMPL(tmpl_name, type, destructor)  \
    __BINHEAP_NUM_ELEMS_IMPL(tmpl_name)              \
    __BINHEAP_HEAD_IMPL(tmpl_name, type)             \
    __BINHEAP_SWAP_IMPL(tmpl_name, type)             \
    __BINHEAP_BUBBLE_UP_IMPL(tmpl_name, type, cmp)   \
    __BINHEAP_BUBBLE_DOWN_IMPL(tmpl_name, type, cmp) \
    __BINHEAP_PUSH_PTR_IMPL(tmpl_name, type, cmp)    \
    __BINHEAP_PUSH_IMPL(tmpl_name, type, cmp)        \
    __BINHEAP_POP_IMPL(tmpl_name, type, cmp)         \
    __BINHEAP_CLONE_IMPL(tmpl_name)
