//
// Created by avdyck on 14.10.17.
//

#include <memory.h>
#include "linked_list.h"
#include "../../util/util.h"

void init_linked_list(linked_list *l) {
    memset(l, 0, sizeof(linked_list));
    l->before = calloc_s(1, sizeof(linked_list_item));
    l->after  = calloc_s(1, sizeof(linked_list_item));
    l->before->next = l->after;
    l-> after->prev = l->before;
}

void free_linked_list(linked_list *l) {
    linked_list_item *tmp;
    linked_list_item *i = l->before;

    while (i) {
        tmp = i->next;
        free_s(i);
        i = tmp;
    }

    memset(l, 0, sizeof(linked_list));
}

void linked_list_add_back(linked_list *l, void *value) {
    linked_list_item *item;

    item = calloc_s(1, sizeof(linked_list_item));
    item->value = value;

    item->next = l->after;
    item->prev = l->after->prev;

    item->next->prev = item;
    item->prev->next = item;

    l->size++;
}

void *linked_list_del(linked_list *l, void *value, equals_fun equals) {
    linked_list_item *cur = l->before;
    void *result = NULL;

    cur = cur->next;
    while (cur != l->after && !equals(cur->value, value)) {
        cur = cur->next;
    }

    if (cur != l->after) {
        cur->prev->next = cur->next;
        cur->next->prev = cur->prev;

        l->size--;

        result = cur->value;
        free_s(cur);
    }

    return result;
}

unsigned int linked_list_size(linked_list *l) {
    return l->size;
}

