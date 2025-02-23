#include "htable.h"

#include <stdio.h>

#include "str.h"

HTABLE(mytbl, str, int, str_hash, str_cmp, str_del, NULL)

bool mytbl_callback(const str* key, int* val, void* userdata) {
    int* agg = (int*)userdata;
    *agg += *val;
    printf("\tKey: %s; Value: %d; Agg value: %d\n", key->cstr, *val, *agg);
    return true;
}

void print(const mytbl* h) {
    printf("===============\n");
    printf("Size: %lu\n", mytbl_size(h));
    if (mytbl_size(h) > 0) {
        int agg = 0;
        mytbl_foreach(h, mytbl_callback, &agg);
        printf("Agg value total: %d\n", agg);
    }
    printf("===============\n");
}

int main(void) {
    // Create hash table
    mytbl h = mytbl_new();

    // Set key-value pair, both by value and by pointer
    // Key is consumed, stored and will be freed by hash table
    str key = str_new_cstr("one");
    int val = 1;
    mytbl_set_ptr(&h, &key, &val);
    mytbl_set(&h, str_new_cstr("two"), 2);

    // Does hash table contain element?
    /*bool true*/ mytbl_contains(&h, key);
    key = str_new_cstr("three");
    /*bool false*/ mytbl_contains(&h, key);

    print(&h);

    // Get element from hash table
    /* NULL */ mytbl_get(&h, key);
    str_set_cstr(&key, "two");
    /* not NULL */ int* pval = mytbl_get(&h, key);

    // Change value
    *pval = 0;

    print(&h);

    // Remove element from hash table
    mytbl_remove(&h, key);

    print(&h);

    str_del(&key);

    // Delete hash table
    mytbl_del(&h);
}

//
// Examples of hash table definitions:
//
// Hash table with int as key and int as value
size_t int_hash(const int* a) { return (size_t)*a; }
bool int_cmp(const int* a, const int* b) { return *a == *b; }

HTABLE(htable_int_int, int, int, int_hash, int_cmp, NULL, NULL)

// Hash table with str as key and custom struct as value with destructor
typedef struct {
    int a;
    float b;
    double c;
    bool d;
    void* ptr;
} custom;

void custom_free(custom* c) { free(c->ptr); }

HTABLE(htable_str_custom, str, custom, str_hash, str_cmp, str_del, custom_free)
