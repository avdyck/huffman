//
// Created by avdyck on 03.10.17.
//

#include <stdio.h>
#include <stdbool.h>
#include "../util/util.h"
#include "ra_file.h"

#ifndef HUFFMAN_FILE_READ_H
#define HUFFMAN_FILE_READ_H

#define INPUT_BUFFER_SIZE (4096)

/**
 * The input stream has two buffers: a hidden buffer and an input buffer. Only the input buffer can be read from.
 * The other buffer is used to know when we have reached the end of a file because we have to use a pad byte.
 */
#define input_buffer(wrapper)  (wrapper)->buffer[(wrapper)->buffer_i]
#define hidden_buffer(wrapper) (wrapper)->buffer[(wrapper)->buffer_i ^ 1]
#define swap_buffer(wrapper)   (wrapper)->buffer_i ^= 1
#define is_eof(in) (in)->input_size == 0

typedef struct input_wrapper {
    FILE *stream;

    int bit_buffer;
    int bit_index;

    byte buffer[2][INPUT_BUFFER_SIZE];
    int buffer_i;

    unsigned int input_size;
    unsigned int input_index;
} input_wrapper;

/**
 * This function will (re)set the input stream of the buffer. It
 * also invalidates the other fields, as they were only valid for the
 * previous input stream.
 */
void set_input_stream(input_wrapper *wrapper, FILE *in);

/**
 * This function will initialize the input wrapper. This will also fill up
 * the hidden buffer, so beware when using this in combination with other readers
 * (ra_file in particular).
 */
void init_input_wrapper(input_wrapper *wrapper);

/**
 * Read the next couple of bytes into the input buffer.
 */
unsigned int read_into_buffer(input_wrapper *wrapper);

/**
 * Read the next byte into a bit buffer (skips to the next byte!).
 */
bool start_bit_read(input_wrapper *in);


/**
 * This function is called unsafe because it can cause buffer overflows.
 * Use only when sure the end of file has not been reached.
 */
static inline void unsafe_read_byte(input_wrapper *in, int *dest);

/**
 * These functions will read respectively one bit/byte into dest.
 * The return value of this function is whether we can keep reading.
 * !! The bit reader won't always read into the least significant bit !!
 */
static inline bool read_one_bit(input_wrapper *in, int *dest);
static inline bool read_byte(input_wrapper *in, int *dest);


//   _____       _ _               //
//  |_   _|     | (_)              //
//    | |  _ __ | |_ _ __   ___    //
//    | | | '_ \| | | '_ \ / _ \   //
//   _| |_| | | | | | | | |  __/   //
//  |_____|_| |_|_|_|_| |_|\___|   //
//  Inline                         //

/**
 * Inline functions in headers might be yucky yucky,
 * but it makes reading every byte/bit WAY faster (trust me it's worth it).
 */

void unsafe_read_byte(input_wrapper *in, int *dest) {
    if (in->input_index >= INPUT_BUFFER_SIZE) {
        // We have reached the end of our input big_buffer.

        in->input_index  = 0;
        swap_buffer(in);
        in->input_size  += read_into_buffer(in);
    }

    in->input_size--;
    *dest = input_buffer(in)[in->input_index++];
}


bool read_byte(input_wrapper *in, int *dest) {
    if (!is_eof(in)) {
        // We know the end of file isn't reached so we can just read bytes.
        unsafe_read_byte(in, dest);

        return true;
    }

    *dest = EOF;
    return false;
}


bool read_one_bit(input_wrapper *in, int *dest) {
    if (in->bit_index == 8 && !start_bit_read(in)) {
        // We ran out of bytes :(
        return false;
    }

    // Read in the next bit
    *dest = in->bit_buffer & (0x80 >> in->bit_index);
    in->bit_index++;

    return true;
}


#endif //HUFFMAN_FILE_READ_H