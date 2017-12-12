//
// Created by avdyck on 23.11.17.
//

#ifndef HUFFMAN_RA_FILE_H
#define HUFFMAN_RA_FILE_H

#include <stdio.h>
#include "../util/util.h"

/**
 * Random Access file stucture.
 * We have to use this structure to save the file state because:
 *  1) We are using stdin and we can't rewind this input stream
 *  2) The code has to work on Helios and every user on Helios
 *     only gets 1GB of file storage, so tempfiles will fail here
 *     as well.
 */

typedef struct ra_file {
    byte *buffer;
    size_t size;
} ra_file;

/**
 * Read the whole file into the ra_file buffer.
 */
void read_whole_file(FILE *in, ra_file *file);

/**
 * Free the ra_file buffer.
 */
void delete_file(ra_file *file);

#endif //HUFFMAN_RA_FILE_H
