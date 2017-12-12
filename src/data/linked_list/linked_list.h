//
// Created by avdyck on 14.10.17.
//

#ifndef HUFFMAN_LINKED_LIST_H
#define HUFFMAN_LINKED_LIST_H

#include <stdbool.h>

// This standard data structure is only used for testing

typedef bool (*equals_fun)(void *list_item, void *compare_item);

typedef struct linked_list_item {
    struct linked_list_item *next;
    struct linked_list_item *prev;
    void *value;
} linked_list_item;

typedef struct linked_list {
    linked_list_item *before;
    linked_list_item *after;
    unsigned int size;
} linked_list;

void init_linked_list(linked_list *l);
void free_linked_list(linked_list *l);

void linked_list_add_back(linked_list *l, void *value);
void *linked_list_del(linked_list *l, void *value, equals_fun equals);

unsigned int linked_list_size(linked_list *l);

#endif //HUFFMAN_LINKED_LIST_H
