#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

typedef struct {
    char *cstr;
    size_t size;
    size_t capacity;
} str;

static inline str str_new(void) {
    str s;
    s.size = s.capacity = 0u;
    s.cstr = (char *)malloc(sizeof(char));
    s.cstr[0] = '\0';
    return s;
}

static inline str str_new_cstr(const char *cstr) {
    str s;
    s.size = s.capacity = strlen(cstr);
    s.cstr = (char *)malloc(sizeof(char) * (s.size + 1));
    strncpy(s.cstr, cstr, s.size);
    s.cstr[s.size] = '\0';
    return s;
}

static inline void str_del(str *s) {
    free(s->cstr);
    s->size = s->capacity = 0u;
    s->cstr = NULL;
}

static inline size_t str_size(const str *s) { return s->size; }

static inline size_t str_capacity(const str *s) { return s->capacity; }

static inline char *str_cstr(str *s) { return s->cstr; }

static inline char str_at(const str *s, size_t pos) { return s->cstr[pos]; }

static inline char str_front(const str *s) { return s->cstr[0]; }

static inline char str_back(const str *s) { return s->cstr[s->size - 1]; }

static inline bool str_is_empty(const str *s) { return s->size == 0u; }

static inline void str_resize(str *s, size_t new_size) {
    s->cstr = (char *)realloc(s->cstr, sizeof(char) * (new_size + 1));
    s->capacity = s->size = new_size;
    s->cstr[new_size] = '\0';
}

static inline void str_reserve(str *s, size_t new_cap) {
    if (new_cap <= s->capacity) {
        return;
    }
    size_t old_size = s->size;
    str_resize(s, new_cap);
    s->size = old_size;
}

static inline void str_clear(str *s) {
    if (s->size == 0u) {
        return;
    }
    s->size = 0u;
    s->cstr[0] = '\0';
}

static inline void str_shrink_to_fit(str *s) { str_resize(s, s->size); }

static inline void __str_insert(str *s, const char *to_insert, size_t pos, size_t len) {
    size_t min_cap = s->size + len;
    if (min_cap > s->capacity) {
        size_t double_cap = s->capacity * 2u;
        size_t new_cap = min_cap < double_cap ? double_cap : min_cap;
        str_reserve(s, new_cap);
    }
    if (pos < s->size) {
        // terminating char '\0' is included
        size_t bytes = sizeof(char) * (s->size - pos + 1);
        memmove(s->cstr + pos + len, s->cstr + pos, bytes);
    }
    strncpy(s->cstr + pos, to_insert, len);
    s->size += len;
    s->cstr[s->size] = '\0';
}

static inline void str_insert_at_char(str *s, char to_insert, size_t pos) { __str_insert(s, &to_insert, pos, 1u); }

static inline void str_insert_at_str(str *s, const str *to_insert, size_t pos) {
    __str_insert(s, to_insert->cstr, pos, to_insert->size);
}

static inline void str_insert_at_cstr(str *s, const char *to_insert, size_t pos) {
    __str_insert(s, to_insert, pos, strlen(to_insert));
}

static inline void __str_append(str *s, const char *to_append, size_t len) { __str_insert(s, to_append, s->size, len); }

static inline void str_append_cstr(str *s, const char *to_append) { __str_append(s, to_append, strlen(to_append)); }

static inline void str_append_char(str *s, char to_append) { __str_append(s, &to_append, 1u); }

static inline void str_append_str(str *s, const str *to_append) { __str_append(s, to_append->cstr, to_append->size); }

static inline void str_set_cstr(str *s, const char *to_set) {
    str_clear(s);
    str_append_cstr(s, to_set);
}

static inline void str_remove_range(str *s, size_t from, size_t to) {
    memmove(s->cstr + from, s->cstr + to, sizeof(char) * (s->size - to));
    s->size -= (to - from);
    s->cstr[s->size] = '\0';
}

static inline void str_remove_at(str *s, size_t index) { str_remove_range(s, index, index + 1); }

static inline bool str_equals_cstr(const str *s1, const char *s2) { return strcmp(s1->cstr, s2) == 0; }

static inline bool str_equals_str(const str *s1, const str *s2) {
    return s1->size == s2->size && strcmp(s1->cstr, s2->cstr) == 0;
}

static inline bool str_cmp(const str *s1, const str *s2) { return str_equals_str(s1, s2); }

static inline size_t str_find_cstr(const str *s, const char *to_find) {
    const char *pos = strstr(s->cstr, to_find);
    if (pos == NULL) {
        return NOT_FOUND;
    }
    return pos - s->cstr;
}

static inline size_t str_find_char(const str *s, char to_find) {
    char arr[2] = {to_find, '\0'};
    return str_find_cstr(s, arr);
}

static inline size_t str_find_str(const str *s, const str *to_find) { return str_find_cstr(s, to_find->cstr); }

static inline str str_substr(const str *s, size_t from, size_t to) {
    size_t len = to - from;
    str res = str_new();
    str_resize(&res, len);
    strncpy(res.cstr, s->cstr + from, len);
    return res;
}

static inline str str_clone(const str *s) { return str_new_cstr(s->cstr); }

static inline void str_reverse(str *s) {
    if (s->size <= 1) {
        return;
    }
    for (size_t i = 0u, j = s->size - 1; i < s->size / 2u; i++, j--) {
        char tmp = s->cstr[i];
        s->cstr[i] = s->cstr[j];
        s->cstr[j] = tmp;
    }
}

static inline str str_concat(const str *s1, const str *s2) {
    str res = str_new();
    str_append_str(&res, s1);
    str_append_str(&res, s2);
    return res;
}

static inline void str_swap(str *s1, str *s2) {
    str tmp = *s1;
    *s1 = *s2;
    *s2 = tmp;
}

static inline size_t str_hash(const str *s) {
    unsigned char *us = (unsigned char *)s->cstr;
    size_t hash = 5381u;
    unsigned char c;
    while ((c = *us++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}
