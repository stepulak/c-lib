#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include "str.h"
#include "vec.h"

static void int_ptr_free(int **ptr) { free(*ptr); }

VEC(vint, int, NULL)
VEC(vint_ptr, int *, int_ptr_free)

static int *malloc_and_set(int val) {
    int *ptr = malloc(sizeof(int));
    *ptr = val;
    return ptr;
}

static void test_new_delete(void) {
    {
        vint v = vint_new();
        assert(v.capacity == 0u);
        assert(v.size == 0u);
        assert(v.data == NULL);

        v.size = v.capacity = 1;
        v.data = malloc(sizeof(int));

        vint_del(&v);
        assert(v.capacity == 0u);
        assert(v.size == 0u);
        assert(v.data == NULL);
    }
    {
        vint_ptr v = vint_ptr_new();
        assert(v.capacity == 0u);
        assert(v.size == 0u);
        assert(v.data == NULL);

        v.size = v.capacity = 1;
        v.data = malloc(sizeof(int *));
        v.data[0] = malloc(sizeof(int));

        vint_ptr_del(&v);
        assert(v.capacity == 0u);
        assert(v.size == 0u);
        assert(v.data == NULL);
    }
}

static void test_size_capacity_data(void) {
    vint v = vint_new();
    v.size = 123u;
    v.capacity = 321u;
    v.data = (int *)0xbeef;

    assert(vint_size(&v) == 123u);
    assert(vint_capacity(&v) == 321u);
    assert(vint_data(&v) == (int *)0xbeef);
}

static void test_reserve(void) {
    vint v = vint_new();
    vint_reserve(&v, 300u);

    assert(v.size == 0u);
    assert(v.capacity == 300u);

    vint_del(&v);
}

static void test_resize(void) {
    vint v = vint_new();
    vint_resize(&v, 33u);

    assert(v.size == 33u);
    assert(v.capacity == 33u);

    vint_del(&v);

    assert(v.size == 0u);
    assert(v.capacity == 0u);
    assert(v.data == NULL);
}

static void test_push_back(void) {
    vint v = vint_new();
    int val = 1;
    vint_push_back_ptr(&v, &val);

    assert(v.size == 1u);
    assert(v.capacity == 2u);
    assert(v.data[0] == 1);

    vint_push_back(&v, 2);
    vint_push_back(&v, 3);

    assert(v.size == 3u);
    assert(v.capacity == 4u);
    assert(v.data[0] == 1);
    assert(v.data[1] == 2);
    assert(v.data[2] == 3);

    vint_del(&v);
}

static void test_at(void) {
    vint v = vint_new();
    vint_push_back(&v, 1);
    vint_push_back(&v, 2);
    vint_push_back(&v, 3);

    assert(*vint_at(&v, 0) == 1);
    assert(*vint_at(&v, 1) == 2);
    assert(*vint_at(&v, 2) == 3);

    vint_del(&v);
}

static void test_front_back(void) {
    vint v = vint_new();
    vint_push_back(&v, 1);
    vint_push_back(&v, 2);

    assert(*vint_front(&v) == 1);
    assert(*vint_back(&v) == 2);

    vint_del(&v);
}

static void test_pop_back(void) {
    vint_ptr v = vint_ptr_new();
    vint_ptr_push_back(&v, malloc_and_set(1));

    assert(v.size == 1u);
    assert(v.capacity == 2u);
    assert(**vint_ptr_at(&v, 0) == 1);

    vint_ptr_pop_back(&v);  // memory should be freed

    assert(v.size == 0u);
    assert(v.capacity == 2u);

    const int *ptr = malloc_and_set(2);
    vint_ptr_push_back_ptr(&v, &ptr);
    vint_ptr_push_back(&v, malloc_and_set(3));
    vint_ptr_pop_back(&v);

    assert(v.size == 1u);
    assert(v.capacity == 4u);
    assert(**vint_ptr_at(&v, 0) == 2);

    vint_ptr_del(&v);
}

static void test_push_front(void) {
    vint v = vint_new();

    for (size_t i = 0; i < 33; i++) {
        int val = (int)i;
        if (i % 2) {
            vint_push_front_ptr(&v, &val);
        } else {
            vint_push_front(&v, val);
        }
    }

    assert(v.size == 33u);
    assert(v.capacity == 64u);

    for (size_t i = 0; i < 33; i++) {
        assert(*vint_at(&v, i) == (int)(32 - i));
    }

    vint_del(&v);
}

static void test_pop_front(void) {
    vint_ptr v = vint_ptr_new();
    vint_ptr_push_front(&v, malloc_and_set(1));

    assert(v.size == 1u);
    assert(v.capacity == 2u);
    assert(**vint_ptr_at(&v, 0) == 1);

    vint_ptr_pop_back(&v);

    assert(v.size == 0u);
    assert(v.capacity == 2u);

    const int *ptr = malloc_and_set(2);
    vint_ptr_push_front_ptr(&v, &ptr);
    vint_ptr_push_front(&v, malloc_and_set(3));
    vint_ptr_pop_front(&v);

    assert(v.size == 1u);
    assert(v.capacity == 4u);
    assert(**vint_ptr_at(&v, 0) == 2);

    vint_ptr_del(&v);
}

static void test_append_before(void) {
    {
        vint v = vint_new();
        int val = 1;
        vint_append_before_ptr(&v, 0, &val);

        assert(v.size == 1u);
        assert(v.data[0] == 1);

        vint_del(&v);
    }
    {
        vint v = vint_new();
        int val = 1;
        vint_append_before_ptr(&v, END, &val);

        assert(v.size == 1u);
        assert(v.data[0] = 1);

        vint_del(&v);
    }
    {
        vint v = vint_new();
        vint_append_before(&v, END, 1);
        vint_append_before(&v, END, 2);
        vint_append_before(&v, END, 3);

        assert(v.size == 3u);
        assert(v.data[0] == 1);
        assert(v.data[1] == 2);
        assert(v.data[2] == 3);

        vint_del(&v);
    }
    {
        vint v = vint_new();
        vint_append_before(&v, END, 3);
        vint_append_before(&v, 0, 1);
        vint_append_before(&v, 1, 2);

        assert(v.size == 3u);
        assert(v.data[0] == 1);
        assert(v.data[1] == 2);
        assert(v.data[2] == 3);

        vint_del(&v);
    }
}

static void test_remove_at(void) {
    {
        vint_ptr v = vint_ptr_new();
        vint_ptr_push_front(&v, malloc_and_set(1));
        vint_ptr_remove_at(&v, 0u);

        assert(v.size == 0u);
        assert(v.capacity == 2u);

        vint_ptr_del(&v);
    }
    {
        vint v = vint_new();
        vint_push_back(&v, 0);
        vint_push_back(&v, 1);
        vint_push_back(&v, 2);
        vint_remove_at(&v, 1u);

        assert(v.size == 2u);
        assert(v.capacity == 4u);
        assert(*vint_front(&v) == 0);
        assert(*vint_back(&v) == 2);

        vint_del(&v);
    }
}

static void test_remove_range(void) {
    {
        vint_ptr v = vint_ptr_new();
        vint_ptr_push_front(&v, malloc_and_set(1));
        vint_ptr_remove_range(&v, 0u, 1u);

        assert(v.size == 0u);
        assert(v.capacity == 2u);

        vint_ptr_del(&v);
    }
    {
        vint v = vint_new();
        for (size_t i = 0u; i < 100u; i++) {
            vint_push_back(&v, (int)i);
        }
        vint_remove_range(&v, 1u, 99u);

        assert(v.size == 2u);
        assert(v.capacity == 128u);
        assert(*vint_front(&v) == 0);
        assert(*vint_back(&v) == 99);

        vint_del(&v);
    }
}

static void test_shrink_to_fit(void) {
    vint_ptr v = vint_ptr_new();

    for (size_t i = 0u; i < 33u; i++) {
        vint_ptr_push_back(&v, malloc_and_set((int)i));
    }
    vint_ptr_shrink_to_fit(&v);

    assert(v.size == 33u);
    assert(v.capacity == 33u);

    vint_ptr_push_back(&v, malloc_and_set(34));

    assert(v.size == 34u);
    assert(v.capacity == 66u);
    assert(**vint_ptr_front(&v) == 0);
    assert(**vint_ptr_back(&v) == 34);

    vint_ptr_del(&v);
}

static bool find_odd_number(const int *v) { return *v % 2 == 1; }

static bool find_five(const int *v) { return *v == 5; }

static void test_find(void) {
    vint v = vint_new();
    vint_push_back(&v, 0);
    vint_push_back(&v, 2);
    vint_push_back(&v, 4);
    vint_push_back(&v, 1);
    vint_push_back(&v, 8);

    assert(vint_find(&v, find_odd_number));
    assert(vint_find(&v, find_five) == NOT_FOUND);

    vint_del(&v);
}

static bool cmp_last_digit(const int *a, const int *b) { return *a % 10 == *b % 10; }

static void test_cmp(void) {
    {
        vint v = vint_new();

        assert(vint_cmp(&v, &v));

        vint_del(&v);
    }
    {
        vint v1 = vint_new();
        vint v2 = vint_new();
        vint_push_back(&v1, 0);
        vint_push_back(&v1, 1);

        assert(!vint_cmp(&v1, &v2));

        vint_del(&v1);
        vint_del(&v2);
    }
    {
        vint v1 = vint_new();
        vint v2 = vint_new();
        vint_push_back(&v1, 11);
        vint_push_back(&v1, 2);
        vint_push_back(&v1, 813);
        vint_push_back(&v2, 1);
        vint_push_back(&v2, 2002);
        vint_push_back(&v2, 333);

        assert(vint_cmp_fn(&v1, &v2, cmp_last_digit));

        vint_del(&v1);
        vint_del(&v2);
    }
}

static void test_clone(void) {
    vint v = vint_new();

    for (size_t i = 0; i < 10000; i++) {
        vint_push_back(&v, (int)i);
    }
    vint clone = vint_clone(&v);

    assert(vint_size(&clone) == vint_size(&v));

    for (size_t i = 0u; i < vint_size(&clone); i++) {
        assert(*vint_at(&v, i) == *vint_at(&clone, i));
    }

    vint_del(&clone);
    vint_del(&v);
}

VEC(vvint, vint, vint_del)

static bool vvint_callback(vint *vint, size_t index, void *userdata) {
    assert(vint->size == index + 1u);
    assert(*vint_front(vint) == 0u);
    assert(*vint_back(vint) == (int)index);
    size_t *acc = userdata;
    *acc += index;
    return true;
}

static void test_2d_vec(void) {
    vvint vvint = vvint_new();

    for (size_t i = 0u; i < 100; i++) {
        vint vint = vint_new();
        for (size_t j = 0u; j < i + 1; j++) {
            vint_push_back(&vint, (int)j);
        }
        if (i % 2) {
            vvint_push_back(&vvint, vint);
        } else {
            vvint_push_back_ptr(&vvint, &vint);
        }
    }

    size_t acc = 0u;
    vvint_foreach(&vvint, vvint_callback, &acc);
    assert(acc == 4950u);

    vvint_del(&vvint);
}

VEC(vstr, str, str_del)

bool vstr_callback(str *s, size_t index, void *userdata) {
    (void)index;
    str *appender = (str *)userdata;
    if (str_size(appender) > 0) {
        str_append_char(appender, ' ');
    }
    str_append_str(appender, s);
    return true;
}

static void test_str_vec(void) {
    vstr v = vstr_new();

    vstr_push_back(&v, str_new_cstr("from"));
    vstr_push_back(&v, str_new_cstr("this"));
    vstr_push_back(&v, str_new_cstr("vector"));
    vstr_push_front(&v, str_new_cstr("world"));
    vstr_push_front(&v, str_new_cstr("hello"));

    assert(vstr_size(&v) == 5u);
    assert(vstr_capacity(&v) == 8u);

    str s = str_new();
    vstr_remove_at(&v, 3u);
    vstr_foreach(&v, vstr_callback, &s);

    assert(str_equals_cstr(&s, "hello world from vector"));

    vstr_del(&v);
    str_del(&s);
}

int main(void) {
    test_new_delete();
    test_size_capacity_data();
    test_reserve();
    test_resize();
    test_at();
    test_front_back();
    test_push_back();
    test_pop_back();
    test_push_front();
    test_pop_front();
    test_append_before();
    test_remove_at();
    test_remove_range();
    test_shrink_to_fit();
    test_find();
    test_cmp();
    test_clone();
    test_2d_vec();
    test_str_vec();

    return 0;
}
