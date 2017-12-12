//
// Created by avdyck on 23.11.17.
//

#include "ra_file.h"

void read_whole_file(FILE *in, ra_file *file) {
    size_t max_size = 1 << 12;

    byte *buffer = malloc_s(sizeof(byte) * max_size);
    size_t read  = fread(buffer, 1, max_size, in);

    while (max_size == read) {
        max_size <<= 1;
        buffer = realloc_s(buffer, sizeof(byte) * max_size);
        read  += fread(buffer+read, 1, read, in);
    }

    file->buffer = buffer;
    file->size   = read;
}

void delete_file(ra_file *file) {
    free_s(file->buffer);
}
