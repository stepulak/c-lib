#include "vec.h"

#include "str.h"

typedef struct {
    int *value;
} mystruct;

mystruct mystruct_create(int value) {
    mystruct ms;
    ms.value = malloc(sizeof(int));
    *ms.value = value;
    return ms;
}

void mystruct_free(mystruct *ms) { free(ms->value); }

bool mystruct_print_agg(mystruct *ms, size_t index, void *userdata) {
    int *agg = (int *)userdata;
    *agg += *ms->value;
    printf("\tValue: %d; At index: %lu; Agg value: %d\n", *ms->value, index, *agg);
    return true;  // continue with processing
}

bool mystruct_find_0(const mystruct *ms) { return *ms->value == 0; }
bool mystruct_find_5(const mystruct *ms) { return *ms->value == 5; }
bool mystruct_cmp(const mystruct *ms1, const mystruct *ms2) { return *ms1->value == *ms2->value; }

// Create vector template for `mystruct`
VEC(
    /*template name*/ myvec,
    /*type*/ mystruct,
    /*value destructor*/ mystruct_free)

static void print(myvec *v) {
    printf("===============\n");
    printf("Size: %lu\n", myvec_size(v));
    printf("Capacity: %lu\n", myvec_capacity(v));
    printf("Data pointer: %p\n", (void *)myvec_data(v));
    if (myvec_size(v) > 0) {
        // Do not call front, back if vector is empty
        printf("Front: %u\n", *myvec_front(v)->value);
        printf("Back: %u\n", *myvec_back(v)->value);

        int agg = 0;
        myvec_foreach(v, mystruct_print_agg, &agg);
        printf("Agg value total: %d\n", agg);
    }
    printf("===============\n");
}

int main(void) {
    // Create vector
    myvec v = myvec_new();

    // Reserve, increase capacity, keep size
    myvec_reserve(&v, 10u);

    // Push values (front, back)
    myvec_push_back(&v, mystruct_create(3));
    myvec_push_front(&v, mystruct_create(2));

    // Push pointer to values (front, back)
    // Good for large structures to avoid unnecessary copying
    mystruct ms = mystruct_create(4);
    myvec_push_back_ptr(&v, &ms);
    ms = mystruct_create(1);
    myvec_push_front_ptr(&v, &ms);

    print(&v);

    // Resize to given size, shrink the capacity to size, free memory if necessary
    myvec_resize(&v, 3u);

    // Pop back, pop front, free memory
    myvec_pop_back(&v);
    myvec_pop_front(&v);

    // Append before (value, pointer to value)
    myvec_append_before(&v, 0u, mystruct_create(0));
    myvec_append_before(&v, 2u, mystruct_create(3));
    ms = mystruct_create(1);
    myvec_append_before_ptr(&v, 1u, &ms);

    print(&v);

    // Remove element at index
    myvec_remove_at(&v, 1u);
    // Remove elements in range
    myvec_remove_range(&v, 1u, 3u);

    // Decrease capacity to match size
    myvec_shrink_to_fit(&v);

    print(&v);

    myvec_push_back(&v, mystruct_create(1));
    myvec_push_back(&v, mystruct_create(2));

    // Find elements
    /*size_t 0*/ myvec_find(&v, mystruct_find_0);
    /*size_t NOT_FOUND*/ myvec_find(&v, mystruct_find_5);

    myvec v2 = myvec_new();
    myvec_push_back(&v2, mystruct_create(33));

    // Compare vectors
    // The vector is equal to itself
    /*bool true*/ myvec_cmp(&v, &v);
    /*bool false*/ myvec_cmp(&v, &v2);

    // Deep compare with function, always use to compare non-primary data types
    /*bool true*/ myvec_cmp_fn(&v, &v, mystruct_cmp);
    /*bool false*/ myvec_cmp_fn(&v, &v2, mystruct_cmp);

    // Delete vector
    myvec_del(&v);
    myvec_del(&v2);
}

//
// Examples of vector definitions:
//
// Vector of strings
VEC(vec_str, str, str_del)

// Vector of ints (no need of element destructor)
VEC(vec_int, int, NULL)

// Vector of vectors of ints
VEC(vec_vec_int, vec_int, vec_int_del)
