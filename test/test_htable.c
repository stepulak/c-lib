#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include "htable.h"
#include "str.h"

// Hash table with INT key and value
size_t int_hash(const int* a) { return (size_t)*a; }
bool int_cmp(const int* a, const int* b) { return *a == *b; }
HTABLE(ht_int, int, int, int_hash, int_cmp, NULL, NULL)

// Hash table with CUSTOM_STRUCT key and value
typedef struct {
    size_t val;
} custom_struct;
size_t custom_struct_hash(const custom_struct* key) { return key->val; }
bool custom_struct_cmp(const custom_struct* a, const custom_struct* b) { return a->val == b->val; }
HTABLE(ht_custom, custom_struct, custom_struct, custom_struct_hash, custom_struct_cmp, NULL, NULL)

// Hash table with INT* key and value
size_t int_ptr_hash(const int** a) { return (size_t)(**a); }
bool int_ptr_cmp(const int** a, const int** b) { return **a == **b; }
void int_ptr_destruct(int** a) { free(*a); }
HTABLE(ht_int_ptr, int*, int*, int_ptr_hash, int_ptr_cmp, int_ptr_destruct, int_ptr_destruct)

// Hash table with worst hash function
size_t int_hash_worst(const int* a) {
    (void)a;
    return 0u;
}
HTABLE(ht_int_worst, int, char, int_hash_worst, int_cmp, NULL, NULL)

static void test_new_delete(void) {
    {
        ht_int ht = ht_int_new();

        assert(ht.num_elems == 0u);
        assert(ht.num_buckets == 0u);
        assert(ht.table == NULL);
        assert(ht_int_size(&ht) == 0u);

        // manual insert, see if deleted properly without error and leak
        ht.num_elems = 1u;
        ht.num_buckets = 1u;
        ht.table = calloc(1u, sizeof(__ht_int_node*));
        ht.table[0] = calloc(1u, sizeof(__ht_int_node));
        ht.table[0]->key = ht.table[0]->value = 1;

        ht_int_del(&ht);
    }
    {
        ht_custom ht = ht_custom_new();

        assert(ht.num_elems == 0u);
        assert(ht.num_buckets == 0u);
        assert(ht.table == NULL);

        ht_custom_del(&ht);
    }
    {
        ht_int_ptr ht = ht_int_ptr_new();

        assert(ht.num_elems == 0u);
        assert(ht.num_buckets == 0u);
        assert(ht.table == NULL);

        // manual insert, see if deleted properly without error and leak
        ht.num_elems = 1u;
        ht.num_buckets = 200u;
        ht.table = calloc(ht.num_buckets, sizeof(__ht_int_ptr_node*));
        ht.table[0] = calloc(1u, sizeof(__ht_int_ptr_node));
        ht.table[0]->key = malloc(sizeof(int*));
        ht.table[0]->value = malloc(sizeof(int*));

        ht_int_ptr_del(&ht);
    }
}

static void test_set(void) {
    {
        ht_int ht = ht_int_new();

        for (size_t i = 0u; i < 1000; i++) {
            ht_int_set(&ht, (int)i, (int)i);
            assert(ht.num_elems == i + 1);
            assert(ht.num_buckets >= i + 1);

            // everything is evently balanced in all buckets
            for (size_t j = 0u; j <= i; j++) {
                assert(ht.table[j] != NULL);
                assert(ht.table[j]->key == (int)j);
                assert(ht.table[j]->value == (int)j);
                assert(ht.table[j]->next == NULL);
            }
        }

        ht_int_del(&ht);
    }
    {
        ht_int_worst ht = ht_int_worst_new();

        for (size_t i = 0u; i < 1000; i++) {
            ht_int_worst_set(&ht, (int)i, 'z');
            assert(ht.num_elems == i + 1);
            assert(ht.num_buckets >= i + 1);

            // everything is in first bucket
            __ht_int_worst_node* n = ht.table[0];
            for (size_t j = 0u; j <= i && n != NULL; j++) {
                assert(n->key == (int)(i - j));
                assert(n->value == 'z');
                n = n->next;
            }
            assert(n == NULL);
        }

        ht_int_worst_del(&ht);
    }
    {
        ht_custom ht = ht_custom_new();
        custom_struct key = {.val = 1};
        custom_struct val = {.val = 0};

        for (size_t i = 1u; i < 10; i++) {
            val.val = i;
            // should overwrite the old value
            ht_custom_set(&ht, key, val);
            assert(ht.num_elems == 1u);
            assert(ht.num_buckets == 1u);
            assert(ht.table[0] != NULL);
            assert(memcmp(&(ht.table[0]->key), &(key), sizeof(custom_struct)) == 0);
            assert(memcmp(&(ht.table[0]->value), &(val), sizeof(custom_struct)) == 0);
            assert(ht.table[0]->next == NULL);
        }

        ht_custom_del(&ht);
    }
}

static void test_get(void) {
    {
        ht_int ht = ht_int_new();

        ht.num_elems = 2u;
        ht.num_buckets = 2u;
        ht.table = calloc(2u, sizeof(__ht_int_node*));
        ht.table[0] = calloc(1u, sizeof(__ht_int_node));
        ht.table[0]->key = ht.table[0]->value = 0;
        ht.table[1] = calloc(1u, sizeof(__ht_int_node));
        ht.table[1]->key = ht.table[1]->value = 1;

        assert(*ht_int_get(&ht, 0) == 0);
        assert(*ht_int_get(&ht, 1) == 1);

        ht_int_del(&ht);
    }
    {
        ht_int ht = ht_int_new();

        ht.num_elems = 2u;
        ht.num_buckets = 2u;
        ht.table = calloc(2u, sizeof(__ht_int_node*));
        ht.table[0] = calloc(1u, sizeof(__ht_int_node));
        ht.table[0]->key = ht.table[0]->value = 0;

        ht.table[0]->next = calloc(1u, sizeof(__ht_int_node));
        ht.table[0]->next->key = ht.table[0]->next->value = 1;  // wrong index

        assert(*ht_int_get(&ht, 0) == 0);
        assert(ht_int_get(&ht, 1) == NULL);

        ht_int_del(&ht);
    }
}

static void test_get_set_contains(void) {
    {
        ht_int ht = ht_int_new();
        int key = 1, val = 10;
        ht_int_set(&ht, key, val);

        assert(ht_int_contains(&ht, key));
        assert(ht_int_contains_ptr(&ht, &key));
        assert(!ht_int_contains(&ht, 2));
        assert(!ht_int_contains(&ht, 3));
        assert(*ht_int_get(&ht, key) == val);
        assert(*ht_int_get_ptr(&ht, &key) == val);
        assert(ht_int_get(&ht, 2) == NULL);
        assert(ht_int_get(&ht, 3) == NULL);

        key = 2;
        val = 20;
        ht_int_set_ptr(&ht, &key, &val);
        assert(*ht_int_get(&ht, key) == val);
        assert(*ht_int_get_ptr(&ht, &key) == val);
        assert(*ht_int_get(&ht, 1) == 10);
        assert(ht_int_get(&ht, 3) == NULL);

        assert(ht_int_size(&ht) == 2u);

        ht_int_del(&ht);
    }
    {
        ht_custom ht = ht_custom_new();

        for (size_t i = 0u; i < 10000u; i++) {
            custom_struct s = {.val = i};
            ht_custom_set_ptr(&ht, &s, &s);
        }
        assert(ht_custom_size(&ht) == 10000u);
        assert(__ht_custom_num_buckets(&ht) == 16384);  // power of two

        for (size_t i = 0u; i < 10000u; i++) {
            custom_struct s = {.val = i};
            assert(ht_custom_contains(&ht, s));
            assert(ht_custom_contains_ptr(&ht, &s));
            assert(ht_custom_get(&ht, s)->val == i);
            assert(ht_custom_get_ptr(&ht, &s)->val == i);
        }
        ht_custom_del(&ht);
    }
    {
        ht_int_ptr ht = ht_int_ptr_new();

        int* key = malloc(sizeof(int));
        int* val = malloc(sizeof(int));
        *key = 3;
        *val = 33;
        ht_int_ptr_set(&ht, key, val);
        assert(**ht_int_ptr_get(&ht, key) == 33);

        val = malloc(sizeof(int));
        *val = 3;
        ht_int_ptr_set(&ht, key, val);
        assert(**ht_int_ptr_get(&ht, key) == 3);

        ht_int_ptr_del(&ht);
    }
}

static void test_remove(void) {
    {
        ht_int ht = ht_int_new();

        ht_int_set(&ht, 0xfeed, 0xbeef);
        ht_int_remove(&ht, 0xfeed);

        assert(ht_int_size(&ht) == 0u);
        assert(ht.table == NULL);

        ht_int_remove(&ht, 0xf00d);  // do nothing

        ht_int_del(&ht);
    }
    {
        ht_int_ptr ht = ht_int_ptr_new();

        for (size_t i = 0u; i < 1000; i++) {
            int* key = malloc(sizeof(int));
            int* val = malloc(sizeof(int));
            *key = (int)i;
            *val = (int)i * 10;
            ht_int_ptr_set(&ht, key, val);
        }
        for (size_t i = 0u; i < 1000; i += 2) {
            int key = (int)i;
            ht_int_ptr_remove(&ht, &key);
        }
        for (size_t i = 0u; i < 1000; i++) {
            int key = (int)i;
            if (i % 2) {
                assert(ht_int_ptr_contains(&ht, &key));
            } else {
                assert(!ht_int_ptr_contains(&ht, &key));
            }
        }
        assert(ht_int_ptr_size(&ht) == 500);

        ht_int_ptr_del(&ht);
    }
}

struct userdata {
    size_t key_acc;
    size_t val_acc;
};

static bool custom_foreach_callback(const custom_struct* key, custom_struct* val, void* userdata_) {
    struct userdata* userdata = (struct userdata*)userdata_;
    userdata->key_acc += key->val;
    userdata->val_acc += val->val;
    return true;
}

static void test_foreach(void) {
    ht_custom ht = ht_custom_new();

    for (size_t i = 0; i < 100; i++) {
        custom_struct key = {.val = i};
        custom_struct val = {.val = i * 10};
        ht_custom_set_ptr(&ht, &key, &val);
    }

    struct userdata ud = {.key_acc = 0u, .val_acc = 0u};
    ht_custom_foreach(&ht, custom_foreach_callback, &ud);

    assert(ud.key_acc == 4950);
    assert(ud.val_acc == 49500);

    ht_custom_del(&ht);
}

static void test_clone(void) {
    ht_int ht = ht_int_new();

    for (size_t i = 0u; i < 10; i++) {
        ht_int_set(&ht, (int)i, (int)i);
    }

    ht_int cl_ht = ht_int_clone(&ht);
    assert(ht_int_size(&cl_ht) == 10u);

    for (size_t i = 0u; i < 10; i++) {
        assert(*ht_int_get(&cl_ht, (int)i) == (int)i);
    }

    ht_int_remove(&ht, 0);
    ht_int_set(&ht, 1, 10);

    assert(*ht_int_get(&cl_ht, 0) == 0);
    assert(*ht_int_get(&cl_ht, 1) == 1);

    ht_int_del(&cl_ht);
    ht_int_del(&ht);
}

HTABLE(ht_str, str, str, str_hash, str_equals_str, str_del, str_del)

static void test_str(void) {
    ht_str ht = ht_str_new();

    ht_str_set(&ht, str_new_cstr("a"), str_new_cstr("1"));
    ht_str_set(&ht, str_new_cstr("b"), str_new_cstr("2"));
    ht_str_set(&ht, str_new_cstr("c"), str_new_cstr("3"));
    ht_str_set(&ht, str_new_cstr("d"), str_new_cstr("4"));
    ht_str_set(&ht, str_new_cstr("e"), str_new_cstr("5"));

    assert(ht_str_size(&ht) == 5u);

    str key = str_new_cstr("c");
    ht_str_remove(&ht, key);

    assert(ht_str_get(&ht, key) == NULL);

    str_set_cstr(&key, "d");

    assert(ht_str_get(&ht, key)->cstr[0] == '4');

    ht_str_del(&ht);
    str_del(&key);
}

int main(void) {
    test_new_delete();
    test_set();
    test_get();
    test_get_set_contains();
    test_remove();
    test_foreach();
    test_clone();
    test_str();
    return 0;
}
