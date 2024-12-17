#include <assert.h>
#include <stdbool.h>

#include "str.h"

static void test_new_delete(void) {
    {
        str s = str_new();

        assert(s.size == 0u);
        assert(s.capacity == 0u);
        assert(s.cstr[0] == '\0');

        str_del(&s);
    }
    {
        str s = str_new_cstr("hello world");

        assert(s.size == 11u);
        assert(s.capacity == 11u);
        assert(strcmp("hello world", s.cstr) == 0);
        assert(str_cstr(&s)[11] == '\0');

        str_del(&s);

        assert(s.size == 0u);
        assert(s.capacity == 0u);
        assert(s.cstr == NULL);
    }
}

static void test_size_capacity_cstr(void) {
    {
        str s = str_new();

        assert(str_size(&s) == 0u);
        assert(str_capacity(&s) == 0u);
        assert(str_cstr(&s)[0] == '\0');

        str_del(&s);
    }
    {
        str s = str_new_cstr("abcdef");

        assert(str_size(&s) == 6u);
        assert(str_capacity(&s) == 6u);
        assert(strcmp("abcdef", str_cstr(&s)) == 0);
        assert(str_cstr(&s)[6] == '\0');

        str_del(&s);

        assert(s.size == 0u);
        assert(s.capacity == 0u);
        assert(s.cstr == NULL);
    }
}

static void test_at_front_back(void) {
    {
        str s = str_new();

        assert(str_front(&s) == '\0');
        assert(str_at(&s, 0) == '\0');
        // str_back fails on empty string

        str_del(&s);
    }
    {
        str s = str_new_cstr("abc");

        assert(str_front(&s) == 'a');
        assert(str_back(&s) == 'c');
        assert(str_at(&s, 0) == 'a');
        assert(str_at(&s, 1) == 'b');
        assert(str_at(&s, 2) == 'c');

        str_del(&s);
    }
}

static void test_is_empty(void) {
    {
        str s = str_new();

        assert(str_is_empty(&s) == true);

        str_del(&s);
    }
    {
        str s = str_new_cstr("X");

        assert(str_is_empty(&s) == false);

        str_del(&s);
    }
}

static void test_resize(void) {
    {
        str s = str_new();

        assert(str_size(&s) == 0u);
        assert(str_at(&s, 0u) == '\0');

        str_resize(&s, 1u);

        assert(str_size(&s) == 1u);
        assert(str_at(&s, 1u) == '\0');

        str_del(&s);
    }
    {
        str s = str_new_cstr("hello world");
        str_resize(&s, 5);

        assert(str_size(&s) == 5);
        assert(str_at(&s, 5) == '\0');
        assert(strcmp(str_cstr(&s), "hello") == 0);

        str_del(&s);
    }
}

static void test_reserve(void) {
    {
        const size_t len = 10 * 1024 * 1024;  // 10 MB
        str s = str_new();

        str_reserve(&s, len);

        assert(str_size(&s) == 0u);
        assert(str_capacity(&s) == len);
        assert(str_at(&s, 0u) == '\0');

        memset(str_cstr(&s), 0, len * sizeof(char));  // cannot overflow

        str_del(&s);
    }
    {
        str s = str_new_cstr("ahoj");
        str_reserve(&s, 2);  // do nothing

        assert(str_size(&s) == 4u);
        assert(str_capacity(&s) == 4u);

        str_del(&s);
    }
}

static void test_clear(void) {
    {
        str s = str_new();
        str_clear(&s);

        assert(str_size(&s) == 0u);
        assert(str_capacity(&s) == 0u);
        assert(str_at(&s, 0u) == '\0');

        str_del(&s);
    }
    {
        str s = str_new_cstr("qwerty");
        str_clear(&s);

        assert(str_size(&s) == 0u);
        assert(str_capacity(&s) == 6u);  // capacity remains
        assert(str_at(&s, 0u) == '\0');

        str_del(&s);
    }
}

static void test_shrink_to_fit(void) {
    {
        str s = str_new();
        str_shrink_to_fit(&s);

        assert(str_size(&s) == 0u);
        assert(str_capacity(&s) == 0u);
        assert(str_at(&s, 0u) == '\0');

        str_del(&s);
    }
    {
        str s = str_new_cstr("ahoj");
        str_reserve(&s, 1000);

        assert(str_size(&s) == 4u);
        assert(str_capacity(&s) == 1000);

        str_shrink_to_fit(&s);

        assert(str_size(&s) == 4u);
        assert(str_capacity(&s) == 4u);
        assert(strcmp(str_cstr(&s), "ahoj") == 0);

        str_del(&s);
    }
}

static void test_append(void) {
    {
        str s = str_new();
        str_append_char(&s, 'a');

        assert(str_size(&s) == 1u);
        assert(str_capacity(&s) == 1u);
        assert(strcmp(str_cstr(&s), "a") == 0);

        str_append_char(&s, 'b');

        assert(str_size(&s) == 2u);
        assert(str_capacity(&s) == 2u);
        assert(strcmp(str_cstr(&s), "ab") == 0);

        str_del(&s);
    }
    {
        const size_t len = 1024 * 1024;
        str s = str_new();

        for (size_t i = 0; i < len; i++) {
            str_append_char(&s, 'a');
        }

        assert(str_size(&s) == len);
        assert(str_capacity(&s) == len);
        assert(str_at(&s, len) == '\0');

        for (size_t i = 0; i < len; i++) {
            assert(str_at(&s, i) == 'a');
        }

        str_del(&s);
    }
    {
        str s = str_new();
        str_append_cstr(&s, "hello");
        str_append_cstr(&s, " ");
        str_append_cstr(&s, "world");

        assert(str_size(&s) == 11u);
        assert(str_capacity(&s) == 20u);
        assert(str_at(&s, 11u) == '\0');
        assert(strcmp(str_cstr(&s), "hello world") == 0);

        str_del(&s);
    }
    {
        str ss[] = {str_new_cstr("jesus "),   str_new_cstr("christ "),    str_new_cstr("is "),
                    str_new_cstr("the way "), str_new_cstr("the truth "), str_new_cstr("and the life")};
        str s = str_new();

        for (size_t i = 0; i < 6; i++) {
            str_append_str(&s, ss + i);
        }

        assert(strcmp(str_cstr(&s), "jesus christ is the way the truth and the life") == 0);

        for (size_t i = 0; i < 6; i++) {
            str_del(ss + i);
        }
        str_del(&s);
    }
}

static void test_insert(void) {
    {
        str s = str_new_cstr("ac");
        str_insert_at_char(&s, 'b', 1);

        assert(str_size(&s) == 3u);
        assert(str_capacity(&s) == 4u);
        assert(str_at(&s, 3u) == '\0');
        assert(strcmp(str_cstr(&s), "abc") == 0);

        str_del(&s);
    }
    {
        str s = str_new_cstr("ad");
        str_insert_at_cstr(&s, "bc", 1);

        assert(str_size(&s) == 4u);
        assert(str_capacity(&s) == 4u);
        assert(str_at(&s, 4u) == '\0');
        assert(strcmp(str_cstr(&s), "abcd") == 0);

        str_del(&s);
    }
    {
        str pref = str_new_cstr("hello");
        str inf = str_new_cstr(" ");
        str suf = str_new_cstr("world");
        str s = str_new();

        str_insert_at_str(&s, &suf, 0);
        str_insert_at_str(&s, &pref, 0);
        str_insert_at_str(&s, &inf, 5);

        assert(str_size(&s) == 11u);
        assert(str_capacity(&s) == 20u);
        assert(str_at(&s, 11u) == '\0');
        assert(strcmp(str_cstr(&s), "hello world") == 0);

        str_del(&s);
        str_del(&pref);
        str_del(&inf);
        str_del(&suf);
    }
}

static void test_set(void) {
    {
        str s = str_new();
        str_set_cstr(&s, "hello world");

        assert(str_size(&s) == 11u);
        assert(str_capacity(&s) == 11u);
        assert(strcmp(str_cstr(&s), "hello world") == 0);

        str_del(&s);
    }
    {
        str s = str_new_cstr("hello world");
        str_set_cstr(&s, "hello");

        assert(str_size(&s) == 5u);
        assert(str_capacity(&s) == 11u);
        assert(strcmp(str_cstr(&s), "hello") == 0);

        str_del(&s);
    }
}

static void test_remove_range(void) {
    {
        str s = str_new_cstr("0123");
        str_remove_range(&s, 0u, 2u);

        assert(str_size(&s) == 2u);
        assert(str_capacity(&s) == 4u);
        assert(strcmp(str_cstr(&s), "23") == 0);

        str_del(&s);
    }
    {
        str s = str_new_cstr("0123");
        str_remove_range(&s, 1u, 3u);

        assert(str_size(&s) == 2u);
        assert(str_capacity(&s) == 4u);
        assert(strcmp(str_cstr(&s), "03") == 0);

        str_del(&s);
    }
    {
        str s = str_new_cstr("0123");
        str_remove_range(&s, 2u, 4u);

        assert(str_size(&s) == 2u);
        assert(str_capacity(&s) == 4u);
        assert(strcmp(str_cstr(&s), "01") == 0);

        str_del(&s);
    }
    {
        str s = str_new_cstr("0123");
        str_remove_range(&s, 0u, 4u);

        assert(str_size(&s) == 0u);
        assert(str_capacity(&s) == 4u);
        assert(str_at(&s, 0u) == '\0');

        str_del(&s);
    }
}

static void test_remove_at(void) {
    {
        str s = str_new_cstr("012");
        str_remove_at(&s, 0u);

        assert(str_size(&s) == 2u);
        assert(str_capacity(&s) == 3u);
        assert(strcmp(str_cstr(&s), "12") == 0);

        str_del(&s);
    }
    {
        str s = str_new_cstr("012");
        str_remove_at(&s, 1u);

        assert(str_size(&s) == 2u);
        assert(str_capacity(&s) == 3u);
        assert(strcmp(str_cstr(&s), "02") == 0);

        str_del(&s);
    }
    {
        str s = str_new_cstr("012");
        str_remove_at(&s, 2u);

        assert(str_size(&s) == 2u);
        assert(str_capacity(&s) == 3u);
        assert(strcmp(str_cstr(&s), "01") == 0);

        str_del(&s);
    }
}

static void test_equals(void) {
    {
        str s = str_new();

        assert(str_equals_str(&s, &s));
        assert(str_equals_cstr(&s, ""));

        str_del(&s);
    }
    {
        str s = str_new_cstr("sample string");

        assert(str_equals_str(&s, &s));
        assert(str_equals_cstr(&s, s.cstr));

        str_del(&s);
    }
    {
        str s1 = str_new_cstr("dell latitude");
        str s2 = str_new_cstr("dell latitude");

        assert(str_equals_str(&s1, &s2));
        assert(str_equals_cstr(&s1, s2.cstr));

        str_del(&s1);
        str_del(&s2);
    }
    {
        str s1 = str_new_cstr("dell latitude");
        str s2 = str_new_cstr("dell latitude");
        str_resize(&s1, 100u);

        assert(!str_equals_str(&s1, &s2));
        assert(str_equals_cstr(&s1, s2.cstr));

        str_del(&s1);
        str_del(&s2);
    }
    {
        str s1 = str_new_cstr("dell latitude");
        str s2 = str_new_cstr("apple macbook");

        assert(!str_equals_str(&s1, &s2));
        assert(!str_equals_cstr(&s1, s2.cstr));

        str_del(&s1);
        str_del(&s2);
    }
}

static void test_find(void) {
    {
        str s = str_new();

        assert(str_find_char(&s, 'a') == NOT_FOUND);

        str_del(&s);
    }
    {
        str s = str_new_cstr("012345");

        for (size_t i = 0u; i < 6; i++) {
            assert(str_find_char(&s, (char)('0' + i)) == i);
        }

        str_del(&s);
    }
    {
        str s = str_new_cstr("abcabc");

        assert(str_find_cstr(&s, "abc") == 0u);
        assert(str_find_cstr(&s, "abcabc") == 0u);
        assert(str_find_cstr(&s, "bc") == 1u);
        assert(str_find_cstr(&s, "ca") == 2u);
        assert(str_find_cstr(&s, "bca") == 1u);
        assert(str_find_cstr(&s, "abcd") == NOT_FOUND);

        str_del(&s);
    }
    {
        str s = str_new_cstr("hello world");
        str n1 = str_new_cstr("o w");
        str n2 = str_new_cstr("lol");

        assert(str_find_str(&s, &n1) == 4u);
        assert(str_find_str(&s, &n2) == NOT_FOUND);

        str_del(&s);
        str_del(&n1);
        str_del(&n2);
    }
}

static void test_substr(void) {
    {
        str s = str_new();
        str sub = str_substr(&s, 0u, 0u);

        assert(str_size(&sub) == 0u);
        assert(str_capacity(&sub) == 0u);
        assert(str_at(&sub, 0u) == '\0');
        assert(str_cstr(&s) != str_cstr(&sub));

        str_del(&sub);
        str_del(&s);
    }
    {
        str s = str_new_cstr("ahoj");
        str sub = str_substr(&s, 0u, 4u);

        assert(str_size(&sub) == 4u);
        assert(str_capacity(&sub) == 4u);
        assert(str_at(&sub, 4u) == '\0');
        assert(strcmp(str_cstr(&s), "ahoj") == 0);
        assert(str_cstr(&s) != str_cstr(&sub));

        str_del(&sub);
        str_del(&s);
    }
    {
        str s = str_new_cstr("aabbccddeeff");
        str sub = str_substr(&s, 4u, 8u);

        assert(str_size(&sub) == 4u);
        assert(str_capacity(&sub) == 4u);
        assert(str_at(&sub, 4u) == '\0');
        assert(strcmp(str_cstr(&sub), "ccdd") == 0);
        assert(str_cstr(&s) != str_cstr(&sub));

        str_del(&sub);
        str_del(&s);
    }
}

static void test_clone(void) {
    {
        str s = str_new();
        str cl = str_clone(&s);

        assert(str_size(&cl) == 0u);
        assert(str_capacity(&cl) == 0u);
        assert(str_at(&cl, 0u) == '\0');

        str_del(&s);
        str_del(&cl);
    }
    {
        str s = str_new_cstr("qwerty");
        str cl = str_clone(&s);

        assert(str_size(&cl) == 6u);
        assert(str_capacity(&cl) == 6u);
        assert(str_at(&cl, 6u) == '\0');
        assert(strcmp(str_cstr(&cl), "qwerty") == 0);
        assert(str_cstr(&s) != str_cstr(&cl));

        str_del(&s);
        str_del(&cl);
    }
}

static void test_reverse(void) {
    {
        str s = str_new();
        str_reverse(&s);

        assert(str_size(&s) == 0u);
        assert(str_capacity(&s) == 0u);
        assert(str_at(&s, 0u) == '\0');

        str_del(&s);
    }
    {
        str s = str_new_cstr("qwerty");
        str_reverse(&s);

        assert(str_size(&s) == 6u);
        assert(str_capacity(&s) == 6u);
        assert(str_at(&s, 6u) == '\0');
        assert(strcmp(str_cstr(&s), "ytrewq") == 0);

        str_del(&s);
    }
    {
        str s = str_new();
        for (char c = 'a'; c <= 'z'; c++) {
            str_append_char(&s, c);
        }
        str_reverse(&s);

        assert(str_size(&s) == ('z' - 'a' + 1));
        assert(str_capacity(&s) == 32u);

        for (char c = 'a'; c <= 'z'; c++) {
            assert(str_at(&s, c - 'a') == ('z' + 'a' - c));
        }
        str_del(&s);
    }
}

static void test_concat(void) {
    str a = str_new_cstr("foo");
    str b = str_new_cstr("bar");
    str s = str_concat(&a, &b);

    assert(str_size(&s) == 6u);
    assert(str_capacity(&s) == 6u);
    assert(str_at(&s, 6u) == '\0');
    assert(strcmp(str_cstr(&s), "foobar") == 0);
    assert(strcmp(str_cstr(&a), "foo") == 0);
    assert(strcmp(str_cstr(&b), "bar") == 0);

    str_del(&a);
    str_del(&b);
    str_del(&s);
}

static void test_swap(void) {
    str a = str_new_cstr("aa");
    str b = str_new_cstr("bbb");
    str_swap(&a, &b);

    assert(str_size(&a) == 3u);
    assert(str_capacity(&a) == 3u);
    assert(str_size(&b) == 2u);
    assert(str_capacity(&b) == 2u);
    assert(strcmp(str_cstr(&a), "bbb") == 0);
    assert(strcmp(str_cstr(&b), "aa") == 0);

    str_del(&a);
    str_del(&b);
}

static void test_hash(void) {
    {
        str s = str_new();

        assert(str_hash(&s) == 5381u);

        str_del(&s);
    }
    {
        str s = str_new_cstr("hello world");

        assert(str_hash(&s) == 13876786532495509697u);

        str_del(&s);
    }
    {
        str s = str_new_cstr("very very very very long message");

        assert(str_hash(&s) == 15904841810548646514u);

        str_del(&s);
    }
}

int main(void) {
    test_new_delete();
    test_size_capacity_cstr();
    test_at_front_back();
    test_is_empty();
    test_resize();
    test_reserve();
    test_clear();
    test_shrink_to_fit();
    test_append();
    test_insert();
    test_set();
    test_remove_range();
    test_remove_at();
    test_equals();
    test_find();
    test_substr();
    test_clone();
    test_reverse();
    test_concat();
    test_swap();
    test_hash();

    return 0;
}
