//
// Created by avdyck on 07.10.17.
//

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "memory.h"
#include "error.h"
#include "../data/linked_list/linked_list.h"


static linked_list allocations;
bool aLock = false;

typedef struct alloc_item {
    void *ptr;
    int   line;
    const char *fun;
    const char *file;
} alloc_item;

void free_allocations();

bool alloc_equals(void *alloc_it, void *ptr) {
    return ((alloc_item *) alloc_it)->ptr == ptr;
}

alloc_item *create_alloc_item(void *ptr, int line, const char *fun, const char *file) {
    alloc_item *item = (alloc_item *) malloc(sizeof(alloc_item));
    item->ptr = ptr;
    item->line = line;
    item->fun = fun;
    item->file = file;
    return item;
}


void INIT_MEMLEAK_TESTING() {
#ifdef __TESTING__
    aLock = true;
    init_linked_list(&allocations);
    aLock = false;
#endif
}

void GET_MEMLEAKS() {
#ifdef __TESTING__
    unsigned int size = linked_list_size(&allocations);

    if (size == 0) {
        printf("No memory leaks detected!\n");
        return;
    }

    printf("\n -- MEMORY LEAKS DETECTED --\n");
    printf(" File             | Function         | Line    | Pointer \n");

    {
        alloc_item *item;
        linked_list_item *list_item = allocations.before->next;

        while (list_item != allocations.after) {
            item = (alloc_item *) list_item->value;
            printf(" %-16s | %-16s | %-7d | %p\n", item->file, item->fun, item->line, item->ptr);
            list_item = list_item->next;
        }
    }

    free_allocations();
#endif
}


void add_allocation(void *ptr, int lineNumber, const char *funName, const char *file) {
    if (aLock == false) {
        aLock = true;

        alloc_item *item = create_alloc_item(ptr, lineNumber, funName, file);
        linked_list_add_back(&allocations, item);

        aLock = false;
    }
}

void remove_allocation(void *ptr, bool needFree) {
    if (aLock == false) {
        aLock = true;

        alloc_item *item = (alloc_item *) linked_list_del(&allocations, ptr, alloc_equals);
        if (needFree) {
            free(item);
        }

        aLock = false;
    }
}

void free_allocations() {
    aLock = true;

    free_linked_list(&allocations);
}


void *REALLOC_S(void *ptr, unsigned int size, const int lineNumber, const char *funName, const char *file) {
    void *result = realloc(ptr, size);
    if (result == NULL) {
        throw("Not enough memory while trying to realloc.\n");
    }

#ifdef __TESTING__
    if (result != ptr) {
        remove_allocation(ptr, false);
        add_allocation(result, lineNumber, funName, file);
    }
#endif

    return result;
}

void *CALLOC_S(unsigned int amt, unsigned int size, const int lineNumber, const char *funName, const char *file) {
    void *ptr = calloc(size, amt);
    if (ptr == NULL) {
        throw("Not enough memory while trying to calloc.\n");
    }

#ifdef __TESTING__
    add_allocation(ptr, lineNumber, funName, file);
#endif

    return ptr;
}

void *MALLOC_S(unsigned int size, const int lineNumber, const char *funName, const char *file) {
    void *ptr = malloc(size);
    if (ptr == NULL) {
        throw("Not enough memory while trying to malloc.\n");
    }

#ifdef __TESTING__
    add_allocation(ptr, lineNumber, funName, file);
#endif

    return ptr;
}

void free_s(void *ptr) {

#ifdef __TESTING__
    remove_allocation(ptr, true);
#endif

    free(ptr);
}
