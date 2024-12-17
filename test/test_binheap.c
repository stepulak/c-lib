#include <assert.h>
#include <limits.h>
#include <stdbool.h>

#include "binheap.h"

static bool int_cmp_min(const int* a, const int* b) { return *a < *b; }
static bool int_cmp_max(const int* a, const int* b) { return *a > *b; }
static bool pint_cmp_min(const int** a, const int** b) { return **a < **b; }
static void int_ptr_free(int** ptr) { free(*ptr); }

BINHEAP(bh_min_int, int, int_cmp_min, NULL)
BINHEAP(bh_max_int, int, int_cmp_max, NULL)
BINHEAP(bh_min_pint, int*, pint_cmp_min, int_ptr_free)

static int* malloc_and_set(int val) {
    int* ptr = malloc(sizeof(int));
    *ptr = val;
    return ptr;
}

static void test_new_delete(void) {
    {
        bh_min_int bh = bh_min_int_new();
        assert(bh.heap.size == 0u);
        bh_min_int_del(&bh);
    }
    {
        bh_min_pint bh = bh_min_pint_new();
        assert(bh.heap.size == 0u);
        bh_min_pint_del(&bh);
    }
}

static void test_size_head(void) {
    {
        bh_min_int bh = bh_min_int_new();
        __bh_min_int_vec_push_back(&bh.heap, 1);
        __bh_min_int_vec_push_back(&bh.heap, 2);
        __bh_min_int_vec_push_back(&bh.heap, 3);

        assert(bh_min_int_size(&bh) == 3u);
        assert(bh_min_int_num_elems(&bh) == 3u);
        assert(bh_min_int_head(&bh) == 1);

        bh_min_int_del(&bh);
    }
    {
        bh_min_pint bh = bh_min_pint_new();
        __bh_min_pint_vec_push_back(&bh.heap, malloc_and_set(33));

        assert(bh_min_pint_size(&bh) == 1u);
        assert(bh_min_pint_num_elems(&bh) == 1u);
        assert(*bh_min_pint_head(&bh) == 33);

        bh_min_pint_del(&bh);
    }
}

static void test_push(void) {
    {
        bh_min_int bh = bh_min_int_new();
        int dataset[] = {5, 3, 8, 1, -4, 0, 15, 6, -6, 22, -11};
        int min = INT_MAX;

        for (size_t i = 0u; i < sizeof(dataset) / sizeof(dataset[0]); i++) {
            if (dataset[i] < min) {
                min = dataset[i];
            }
            bh_min_int_push(&bh, dataset[i]);

            assert(bh_min_int_head(&bh) == min);
        }

        bh_min_int_del(&bh);
    }
    {
        bh_max_int bh = bh_max_int_new();
        int dataset[] = {33, 15, -64, 334, 432, 44, -5423, 123, 874};
        int max = INT_MIN;

        for (size_t i = 0u; i < sizeof(dataset) / sizeof(dataset[0]); i++) {
            if (dataset[i] > max) {
                max = dataset[i];
            }
            bh_max_int_push_ptr(&bh, &dataset[i]);

            assert(bh_max_int_head(&bh) == max);
        }

        bh_max_int_del(&bh);
    }
    {
        bh_min_pint bh = bh_min_pint_new();

        for (size_t i = 0; i < 1000; i++) {
            bh_min_pint_push(&bh, malloc_and_set((int)i));

            assert(*bh_min_pint_head(&bh) == 0);
        }

        bh_min_pint_del(&bh);
    }
}

static void test_pop(void) {
    {
        bh_min_int bh = bh_min_int_new();
        bh_min_int_push(&bh, 3);
        bh_min_int_push(&bh, 2);
        bh_min_int_push(&bh, 1);

        assert(bh_min_int_size(&bh) == 3u);
        assert(bh_min_int_pop(&bh) == 1);
        assert(bh_min_int_pop(&bh) == 2);

        bh_min_int_push(&bh, 2);
        bh_min_int_push(&bh, -1);

        assert(bh_min_int_pop(&bh) == -1);
        assert(bh_min_int_pop(&bh) == 2);
        assert(bh_min_int_pop(&bh) == 3);
        assert(bh_min_int_size(&bh) == 0u);

        bh_min_int_del(&bh);
    }
    {
        bh_max_int bh = bh_max_int_new();
        bh_max_int_push(&bh, 1);
        bh_max_int_push(&bh, 2);
        bh_max_int_push(&bh, 3);
        bh_max_int_push(&bh, 2);
        bh_max_int_push(&bh, 1);

        assert(bh_max_int_size(&bh) == 5u);
        assert(bh_max_int_pop(&bh) == 3);
        assert(bh_max_int_pop(&bh) == 2);
        assert(bh_max_int_pop(&bh) == 2);
        assert(bh_max_int_pop(&bh) == 1);

        bh_max_int_push(&bh, 2);
        bh_max_int_push(&bh, -1);

        assert(bh_max_int_pop(&bh) == 2);
        assert(bh_max_int_pop(&bh) == 1);
        assert(bh_max_int_pop(&bh) == -1);
        assert(bh_max_int_size(&bh) == 0u);

        bh_max_int_del(&bh);
    }
    {
        bh_min_pint bh = bh_min_pint_new();

        for (size_t i = 1000; i > 0; i--) {
            bh_min_pint_push(&bh, malloc_and_set((int)i));

            assert(*bh_min_pint_head(&bh) == (int)i);
        }
        for (size_t i = 1; i < 1000; i++) {
            assert(*bh_min_pint_head(&bh) == (int)i);
            assert(bh_min_pint_num_elems(&bh) == (1000u - i + 1));

            free(bh_min_pint_pop(&bh));
        }
        bh_min_pint_del(&bh);
    }
}

static void test_clone(void) {
    bh_min_int bh = bh_min_int_new();
    __bh_min_int_vec_push_back(&bh.heap, 1);
    __bh_min_int_vec_push_back(&bh.heap, 2);
    __bh_min_int_vec_push_back(&bh.heap, 3);
    bh_min_int clone = bh_min_int_clone(&bh);

    assert(bh_min_int_size(&clone) == 3u);
    assert(bh_min_int_pop(&clone) == 1);
    assert(bh_min_int_pop(&clone) == 2);
    assert(bh_min_int_pop(&clone) == 3);
    assert(bh_min_int_size(&clone) == 0u);

    bh_min_int_del(&bh);
    bh_min_int_del(&clone);
}

int main(void) {
    test_new_delete();
    test_size_head();
    test_push();
    test_pop();
    test_clone();

    return 0;
}
