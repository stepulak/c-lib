#include "str.h"

#include <stdio.h>

static void print(str *s) {
    printf("===============\n");
    // Even empty string is '\0' terminated
    printf("Content: %s\n", str_cstr(s));
    printf("Size: %lu\n", str_size(s));
    printf("Capacity: %lu\n", str_capacity(s));
    printf("IsEmpty: %d\n", str_is_empty(s));
    printf("Hash: %lu\n", str_hash(s));

    if (str_size(s) > 0) {
        // Do not call front, back functions if string is empty
        printf("Front: %c\n", str_front(s));
        printf("Back: %c\n", str_back(s));
    }
    printf("===============\n");
}

int main(void) {
    str s = str_new();
    // Create string from c-like string
    str sc = str_new_cstr("hello world");

    // Like in vector, allocate capacity for string
    str_reserve(&s, 50);

    // Append single character
    str_append_char(&s, 'x');
    // Append c-like string
    str_append_cstr(&s, "-mas ");
    // Append another string
    str_append_str(&s, &sc);

    print(&s);

    // Resize capacity to size of string
    str_shrink_to_fit(&s);

    // Remove single character...
    str_remove_at(&s, 0u);
    // ... or range
    str_remove_range(&s, 1u, 4u);

    // Clear string, able to be used for further operations
    str_clear(&sc);

    // Both clear and reset with c-like string
    str_set_cstr(&sc, "HELLO WORLD");

    // Insert character, c-like string, string
    str_insert_at_char(&s, 'X', 0u);
    str_insert_at_cstr(&s, "MAS", 2u);
    str_insert_at_str(&s, &sc, 6u);

    print(&s);

    // String equality, the string is equal to itself
    /*bool true*/ str_equals_str(&s, &s);
    /*bool false*/ str_equals_cstr(&s, "random string");

    /*size_t NOT_FOUND*/ str_find_char(&s, 'Z');
    /*size_t 0*/ str_find_cstr(&s, "X-MAS");
    /*size_t 6*/ str_find_str(&s, &sc);

    // Create clone with standalone memory
    str clone = str_clone(&s);

    // Reverse it
    str_reverse(&clone);

    // Swap content with another string
    str_swap(&clone, &s);

    print(&s);

    // Create new string with concatenation of two strings
    str concat = str_concat(&clone, &s);

    print(&concat);

    // Create new string from substring
    str substr = str_substr(&s, 1u, 5u);

    print(&substr);

    // Delete string, unable to be used for further operations
    str_del(&s);
    str_del(&sc);
    str_del(&clone);
    str_del(&concat);
    str_del(&substr);
}
