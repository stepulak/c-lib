#include "binheap.h"

#include <stdio.h>

#include "str.h"

bool str_len_cmp(const str *s1, const str *s2) { return s1->size < s2->size; }

BINHEAP(myheap, str, str_len_cmp, str_del)

void print(myheap *m) {
    printf("===============\n");
    printf("Size: %lu\n", myheap_size(m));
    if (myheap_size(m) > 0) {
        str head = myheap_head(m);
        printf("Head: %s\n", str_cstr(&head));
    }
    printf("===============\n");
}

int main(void) {
    // Create binary heap
    myheap h = myheap_new();

    // Push values
    myheap_push(&h, str_new_cstr("ccc"));
    myheap_push(&h, str_new_cstr("a"));
    myheap_push(&h, str_new_cstr("bb"));

    print(&h);

    // Pop values (sorted by length)
    str head = myheap_pop(&h);
    str_del(&head);
    head = myheap_pop(&h);
    str_del(&head);

    print(&h);

    // Delete binary heap
    myheap_del(&h);
}
