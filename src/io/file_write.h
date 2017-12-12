//
// Created by avdyck on 09.10.17.
//

#ifndef HUFFMAN_FILE_WRITE_H
#define HUFFMAN_FILE_WRITE_H

#include <stdbool.h>
#include <stdio.h>
#include "../util/util.h"

#define OUTPUT_BUFFER_SIZE (4096)

typedef struct output_wrapper {
    buffer_t small_buffer;
    unsigned int small_index;

    buffer_t big_buffer[OUTPUT_BUFFER_SIZE];
    size_t big_index;

    FILE *stream;
} output_wrapper;

/**
 * Skip to the next output byte.
 */
void to_next_output_byte       (output_wrapper *wrapper);


/**
 * Set the output stream of the bit writer.
 */
void set_output_stream         (output_wrapper *wrapper, FILE *out);


/**
 * Flush the last couple of bytes and write a pad byte if necessary.
 */
void close_bit_writer          (output_wrapper *wrapper, bool pad_byte);


/**
 * Flush the big and small buffer of the output wrapper.
 */
void flush_big_buffer          (output_wrapper *wrapper);
void flush_small_buffer        (output_wrapper *wrapper);

/**
 * Write the last pad byte. This byte will contain the
 * amount of bits the last byte used.
 */
void write_pad_byte(output_wrapper *wrapper);


/**
 * Write n bits to the output stream.
 * The bits have to be the most significant bits.
 */
static inline void write_n_bits(output_wrapper *wrapper, buffer_t k, int n);
static inline void write_byte  (output_wrapper *wrapper, int k);


//   _____       _ _               //
//  |_   _|     | (_)              //
//    | |  _ __ | |_ _ __   ___    //
//    | | | '_ \| | | '_ \ / _ \   //
//   _| |_| | | | | | | | |  __/   //
//  |_____|_| |_|_|_|_| |_|\___|   //
//  Inline                         //

void write_n_bits(output_wrapper *wrapper, buffer_t k, int n) {
    wrapper->small_buffer|= k >> wrapper->small_index;
    wrapper->small_index += n;

    if (wrapper->small_index >= BUFFER_SIZE) {
        // We have reached the end of our small buffer
        wrapper->big_buffer[wrapper->big_index] = Swap8Bytes(wrapper->small_buffer);
        wrapper->big_index++;

        if (wrapper->big_index >= OUTPUT_BUFFER_SIZE) {
            // We have reached the end of our large buffer
            flush_big_buffer(wrapper);
            wrapper->big_index = 0;
        }

        wrapper->small_index -= BUFFER_SIZE;
        if (wrapper->small_index != 0) {
            wrapper->small_buffer = k << (n - wrapper->small_index);
        } else {
            wrapper->small_buffer = 0;
        }
    }
}


void write_byte(output_wrapper *wrapper, int k) {
    write_n_bits(wrapper, ((buffer_t) k) << (BUFFER_SIZE - 8), 8);
}


#endif //HUFFMAN_FILE_WRITE_H
