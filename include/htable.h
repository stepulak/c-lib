#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#define __HTABLE_NUM_ELEMS_IMPL(tmpl_name)                                                           \
    UNUSED static inline size_t tmpl_name##_num_elems(const tmpl_name *ht) { return ht->num_elems; } \
    UNUSED static inline size_t tmpl_name##_size(const tmpl_name *ht) { return ht->num_elems; }

#define __HTABLE_NUM_BUCKETS_IMPL(tmpl_name) \
    UNUSED static inline size_t __##tmpl_name##_num_buckets(const tmpl_name *ht) { return ht->num_buckets; }

#define __HTABLE_TABLE_IMPL(tmpl_name)                                                             \
    UNUSED static inline const __##tmpl_name##_node **__##tmpl_name##_table(const tmpl_name *ht) { \
        return (const __##tmpl_name##_node **)ht->table;                                           \
    }

#define __HTABLE_NEW_IMPL(tmpl_name)                       \
    UNUSED static inline tmpl_name tmpl_name##_new(void) { \
        tmpl_name ht;                                      \
        ht.num_elems = ht.num_buckets = 0u;                \
        ht.table = NULL;                                   \
        return ht;                                         \
    }

#define __HTABLE_DEL_IMPL(tmpl_name, key_type, val_type, key_destructor, val_destructor) \
    UNUSED static inline void tmpl_name##_del(tmpl_name *ht) {                           \
        void (*valdestruct)(val_type *) = val_destructor;                                \
        void (*keydestruct)(key_type *) = key_destructor;                                \
        for (size_t i = 0u; i < ht->num_buckets; i++) {                                  \
            for (__##tmpl_name##_node *node = ht->table[i]; node != NULL;) {             \
                if (valdestruct) {                                                       \
                    valdestruct(&(node->value));                                         \
                }                                                                        \
                if (keydestruct) {                                                       \
                    keydestruct(&(node->key));                                           \
                }                                                                        \
                __##tmpl_name##_node *next = node->next;                                 \
                free(node);                                                              \
                node = next;                                                             \
            }                                                                            \
        }                                                                                \
        free(ht->table);                                                                 \
        ht->table = NULL;                                                                \
        ht->num_elems = ht->num_buckets = 0u;                                            \
    }

#define __HTABLE_REHASH_IMPL(tmpl_name, key_type, key_hash)                                                           \
    UNUSED static inline void __##tmpl_name##_rehash(tmpl_name *ht, size_t num_buckets_old, size_t num_buckets_new) { \
        for (size_t i = 0u; i < num_buckets_old; i++) {                                                               \
            __##tmpl_name##_node *node, *prev, *next;                                                                 \
            prev = next = NULL;                                                                                       \
            for (node = ht->table[i]; node != NULL;) {                                                                \
                next = node->next;                                                                                    \
                size_t index = __##tmpl_name##_count_index((const key_type *)&(node->key), num_buckets_new);          \
                if (index == i) {                                                                                     \
                    prev = node;                                                                                      \
                    node = next;                                                                                      \
                    continue;                                                                                         \
                }                                                                                                     \
                if (prev != NULL) {                                                                                   \
                    prev->next = next;                                                                                \
                } else {                                                                                              \
                    ht->table[i] = next;                                                                              \
                }                                                                                                     \
                node->next = NULL;                                                                                    \
                __##tmpl_name##_insert_node(ht, node, index);                                                         \
                node = next;                                                                                          \
            }                                                                                                         \
        }                                                                                                             \
    }

#define __HTABLE_RESIZE_IMPL(tmpl_name)                                                       \
    UNUSED static inline void __##tmpl_name##_resize(tmpl_name *ht, size_t num_buckets_new) { \
        size_t node_ptr_size = sizeof(__##tmpl_name##_node *);                                \
        size_t curr_num_buckets = ht->num_buckets;                                            \
        ht->table = realloc(ht->table, node_ptr_size * num_buckets_new);                      \
        if (num_buckets_new > curr_num_buckets) {                                             \
            size_t bytes = node_ptr_size * (num_buckets_new - curr_num_buckets);              \
            memset(ht->table + curr_num_buckets, 0, bytes);                                   \
        }                                                                                     \
        ht->num_buckets = num_buckets_new;                                                    \
    }

#define __HTABLE_RESIZE_REHASH_IMPL(tmpl_name, key_type, key_hash)                                   \
    UNUSED static inline void __##tmpl_name##_resize_rehash(tmpl_name *ht, size_t num_buckets_new) { \
        size_t num_buckets = ht->num_buckets;                                                        \
        if (num_buckets == num_buckets_new) {                                                        \
            return;                                                                                  \
        }                                                                                            \
        if (num_buckets == 0u) {                                                                     \
            __##tmpl_name##_resize(ht, num_buckets_new);                                             \
            return;                                                                                  \
        }                                                                                            \
        if (num_buckets_new > num_buckets) {                                                         \
            __##tmpl_name##_resize(ht, num_buckets_new);                                             \
            __##tmpl_name##_rehash(ht, num_buckets, num_buckets_new);                                \
        } else {                                                                                     \
            __##tmpl_name##_rehash(ht, num_buckets, num_buckets_new);                                \
            __##tmpl_name##_resize(ht, num_buckets_new);                                             \
        }                                                                                            \
    }

#define __HTABLE_INSERT_NODE_IMPL(tmpl_name)                                                                         \
    UNUSED static inline void __##tmpl_name##_insert_node(tmpl_name *ht, __##tmpl_name##_node *node, size_t index) { \
        if (ht->table[index] == NULL) {                                                                              \
            ht->table[index] = node;                                                                                 \
            return;                                                                                                  \
        }                                                                                                            \
        __##tmpl_name##_node *ptr = ht->table[index];                                                                \
        node->next = ptr;                                                                                            \
        ht->table[index] = node;                                                                                     \
    }

#define __HTABLE_COUNT_INDEX_IMPL(tmpl_name, key_type, key_hash, key_cmp)                         \
    UNUSED static inline size_t __##tmpl_name##_count_index(const key_type *key, size_t modulo) { \
        size_t (*keyhash)(const key_type *) = key_hash;                                           \
        if (keyhash == NULL) {                                                                    \
            return 0u;                                                                            \
        }                                                                                         \
        return keyhash(key) % modulo;                                                             \
    }

#define __HTABLE_FIND_NODE(tmpl_name, key_type, key_hash, key_cmp, ht, key, ret_node, ret_parent, ret_bucket, \
                           ret_bucket_index)                                                                  \
    do {                                                                                                      \
        (void)ret_node;                                                                                       \
        (void)ret_parent;                                                                                     \
        (void)ret_bucket;                                                                                     \
        (void)ret_bucket_index;                                                                               \
        ret_node = ret_parent = ret_bucket = NULL;                                                            \
        ret_bucket_index = (size_t)-1;                                                                        \
        bool (*keycmp)(const key_type *, const key_type *) = key_cmp;                                         \
        if (keycmp == NULL) {                                                                                 \
            break;                                                                                            \
        }                                                                                                     \
        if (ht->num_elems == 0u) {                                                                            \
            break;                                                                                            \
        }                                                                                                     \
        ret_bucket_index = __##tmpl_name##_count_index(key, ht->num_buckets);                                 \
        ret_bucket = ht->table[ret_bucket_index];                                                             \
        if (ret_bucket == NULL) {                                                                             \
            break;                                                                                            \
        }                                                                                                     \
        bool found = false;                                                                                   \
        for (ret_node = ret_bucket; ret_node != NULL;) {                                                      \
            if (keycmp(key, (const key_type *)&(node->key))) {                                                \
                found = true;                                                                                 \
                break;                                                                                        \
            }                                                                                                 \
            ret_parent = ret_node;                                                                            \
            ret_node = ret_node->next;                                                                        \
        }                                                                                                     \
        if (!found) {                                                                                         \
            ret_node = NULL;                                                                                  \
            ret_parent = NULL;                                                                                \
        }                                                                                                     \
    } while (0)

#define __HTABLE_SET_PTR_IMPL(tmpl_name, key_type, val_type, key_hash, key_cmp, val_destructor)                  \
    UNUSED static inline void tmpl_name##_set_ptr(tmpl_name *ht, const key_type *key, const val_type *val) {     \
        size_t bucket_index;                                                                                     \
        __##tmpl_name##_node *node, *parent, *bucket;                                                            \
        __HTABLE_FIND_NODE(tmpl_name, key_type, key_hash, key_cmp, ht, key, node, parent, bucket, bucket_index); \
        if (node != NULL) {                                                                                      \
            void (*destruct)(val_type *) = val_destructor;                                                       \
            if (destruct != NULL) {                                                                              \
                destruct(&(node->value));                                                                        \
            }                                                                                                    \
            memcpy(&(node->value), val, sizeof(val_type));                                                       \
            return;                                                                                              \
        }                                                                                                        \
        if (ht->num_elems + 1 >= ht->num_buckets) {                                                              \
            __##tmpl_name##_resize_rehash(ht, ht->num_buckets == 0u ? 1u : ht->num_buckets * 2);                 \
        }                                                                                                        \
        node = malloc(sizeof(__##tmpl_name##_node));                                                             \
        memcpy(&(node->key), key, sizeof(key_type));                                                             \
        memcpy(&(node->value), val, sizeof(val_type));                                                           \
        node->next = NULL;                                                                                       \
        size_t index = __##tmpl_name##_count_index(key, ht->num_buckets);                                        \
        __##tmpl_name##_insert_node(ht, node, index);                                                            \
        ht->num_elems++;                                                                                         \
    }

#define __HTABLE_SET_IMPL(tmpl_name, key_type, val_type, key_hash, key_cmp, val_destructor) \
    UNUSED static inline void tmpl_name##_set(tmpl_name *ht, key_type key, val_type val) {  \
        tmpl_name##_set_ptr(ht, (const key_type *)&key, (const val_type *)&val);            \
    }

#define __HTABLE_GET_PTR_IMPL(tmpl_name, key_type, val_type, key_hash, key_cmp)                                  \
    UNUSED static inline val_type *tmpl_name##_get_ptr(const tmpl_name *ht, const key_type *key) {               \
        size_t bucket_index;                                                                                     \
        __##tmpl_name##_node *node, *parent, *bucket;                                                            \
        __HTABLE_FIND_NODE(tmpl_name, key_type, key_hash, key_cmp, ht, key, node, parent, bucket, bucket_index); \
        if (node != NULL) {                                                                                      \
            return &(node->value);                                                                               \
        }                                                                                                        \
        return NULL;                                                                                             \
    }

#define __HTABLE_GET_IMPL(tmpl_name, key_type, val_type, key_hash, key_cmp)             \
    UNUSED static inline val_type *tmpl_name##_get(const tmpl_name *ht, key_type key) { \
        return tmpl_name##_get_ptr(ht, (const key_type *)&key);                         \
    }

#define __HTABLE_CONTAINS_PTR_IMPL(tmpl_name, key_type, val_type, key_hash, key_cmp)                             \
    UNUSED static inline bool tmpl_name##_contains_ptr(const tmpl_name *ht, const key_type *key) {               \
        size_t bucket_index;                                                                                     \
        __##tmpl_name##_node *node, *parent, *bucket;                                                            \
        __HTABLE_FIND_NODE(tmpl_name, key_type, key_hash, key_cmp, ht, key, node, parent, bucket, bucket_index); \
        return node != NULL;                                                                                     \
    }

#define __HTABLE_CONTAINS_IMPL(tmpl_name, key_type, val_type, key_hash, key_cmp)        \
    UNUSED static inline bool tmpl_name##_contains(const tmpl_name *ht, key_type key) { \
        return tmpl_name##_contains_ptr(ht, (const key_type *)&key);                    \
    }

#define __HTABLE_REMOVE_PTR_IMPL(tmpl_name, key_type, val_type, key_hash, key_cmp, key_destructor, val_destructor) \
    UNUSED static inline void tmpl_name##_remove_ptr(tmpl_name *ht, const key_type *key) {                         \
        size_t bucket_index;                                                                                       \
        __##tmpl_name##_node *node, *parent, *bucket;                                                              \
        __HTABLE_FIND_NODE(tmpl_name, key_type, key_hash, key_cmp, ht, key, node, parent, bucket, bucket_index);   \
        if (node == NULL) {                                                                                        \
            return;                                                                                                \
        }                                                                                                          \
        void (*valdestruct)(val_type *) = val_destructor;                                                          \
        void (*keydestruct)(key_type *) = key_destructor;                                                          \
        if (valdestruct) {                                                                                         \
            valdestruct(&(node->value));                                                                           \
        }                                                                                                          \
        if (keydestruct) {                                                                                         \
            keydestruct(&(node->key));                                                                             \
        }                                                                                                          \
        if (parent != NULL) {                                                                                      \
            parent->next = node->next;                                                                             \
        } else {                                                                                                   \
            ht->table[bucket_index] = node->next;                                                                  \
        }                                                                                                          \
        free(node);                                                                                                \
        ht->num_elems--;                                                                                           \
        if (ht->num_elems == 0u) {                                                                                 \
            tmpl_name##_del(ht);                                                                                   \
            return;                                                                                                \
        }                                                                                                          \
        if (ht->num_elems < ht->num_buckets / 2u) {                                                                \
            __##tmpl_name##_resize_rehash(ht, ht->num_buckets / 2u);                                               \
        }                                                                                                          \
    }

#define __HTABLE_REMOVE_IMPL(tmpl_name, key_type, val_type, key_hash, key_cmp, key_destructor, val_destructor) \
    UNUSED static inline void tmpl_name##_remove(tmpl_name *ht, key_type key) {                                \
        tmpl_name##_remove_ptr(ht, (const key_type *)&key);                                                    \
    }

#define __HTABLE_FOREACH_IMPL(tmpl_name, key_type, val_type)                                           \
    UNUSED static inline void tmpl_name##_foreach(                                                     \
        const tmpl_name *ht, bool (*callback)(const key_type *, val_type *, void *), void *userdata) { \
        for (size_t i = 0u; i < ht->num_buckets; i++) {                                                \
            for (__##tmpl_name##_node *n = ht->table[i]; n != NULL; n = n->next) {                     \
                if (!callback((const key_type *)&(n->key), &(n->value), userdata)) {                   \
                    return;                                                                            \
                }                                                                                      \
            }                                                                                          \
        }                                                                                              \
    }

#define __HTABLE_CLONE_IMPL(tmpl_name, key_type, val_type)                                               \
    UNUSED static inline bool __##tmpl_name##_clone_callback(const key_type *k, val_type *v, void *ud) { \
        tmpl_name##_set_ptr((tmpl_name *)ud, (const key_type *)k, (const val_type *)v);                  \
        return true;                                                                                     \
    }                                                                                                    \
    UNUSED static inline tmpl_name tmpl_name##_clone(const tmpl_name *ht) {                              \
        tmpl_name cl = tmpl_name##_new();                                                                \
        tmpl_name##_foreach(ht, __##tmpl_name##_clone_callback, &cl);                                    \
        return cl;                                                                                       \
    }

#define HTABLE(tmpl_name, key_type, val_type, key_hash, key_cmp, key_destructor, val_destructor)               \
    typedef struct __##tmpl_name##_node {                                                                      \
        key_type key;                                                                                          \
        val_type value;                                                                                        \
        struct __##tmpl_name##_node *next;                                                                     \
    } __##tmpl_name##_node;                                                                                    \
    typedef struct {                                                                                           \
        size_t num_elems;                                                                                      \
        size_t num_buckets;                                                                                    \
        __##tmpl_name##_node **table;                                                                          \
    } tmpl_name;                                                                                               \
    __HTABLE_NUM_ELEMS_IMPL(tmpl_name)                                                                         \
    __HTABLE_NUM_BUCKETS_IMPL(tmpl_name)                                                                       \
    __HTABLE_TABLE_IMPL(tmpl_name)                                                                             \
    __HTABLE_NEW_IMPL(tmpl_name)                                                                               \
    __HTABLE_DEL_IMPL(tmpl_name, key_type, val_type, key_destructor, val_destructor)                           \
    __HTABLE_COUNT_INDEX_IMPL(tmpl_name, key_type, key_hash, key_cmp)                                          \
    __HTABLE_INSERT_NODE_IMPL(tmpl_name)                                                                       \
    __HTABLE_RESIZE_IMPL(tmpl_name)                                                                            \
    __HTABLE_REHASH_IMPL(tmpl_name, key_type, key_hash)                                                        \
    __HTABLE_RESIZE_REHASH_IMPL(tmpl_name, key_type, key_hash)                                                 \
    __HTABLE_SET_PTR_IMPL(tmpl_name, key_type, val_type, key_hash, key_cmp, val_destructor)                    \
    __HTABLE_SET_IMPL(tmpl_name, key_type, val_type, key_hash, key_cmp, val_destructor)                        \
    __HTABLE_GET_PTR_IMPL(tmpl_name, key_type, val_type, key_hash, key_cmp)                                    \
    __HTABLE_GET_IMPL(tmpl_name, key_type, val_type, key_hash, key_cmp)                                        \
    __HTABLE_CONTAINS_PTR_IMPL(tmpl_name, key_type, val_type, key_hash, key_cmp)                               \
    __HTABLE_CONTAINS_IMPL(tmpl_name, key_type, val_type, key_hash, key_cmp)                                   \
    __HTABLE_REMOVE_PTR_IMPL(tmpl_name, key_type, val_type, key_hash, key_cmp, key_destructor, val_destructor) \
    __HTABLE_REMOVE_IMPL(tmpl_name, key_type, val_type, key_hash, key_cmp, key_destructor, val_destructor)     \
    __HTABLE_FOREACH_IMPL(tmpl_name, key_type, val_type)                                                       \
    __HTABLE_CLONE_IMPL(tmpl_name, key_type, val_type)
