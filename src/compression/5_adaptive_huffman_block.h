//
// Created by avdyck on 03.10.17.
//

#ifndef HUFFMAN_ADAPTIVE_HUFFMAN_BLOCK_H
#define HUFFMAN_ADAPTIVE_HUFFMAN_BLOCK_H

#include <stdio.h>
#include "../io/file_read.h"
#include "../io/file_write.h"

void adaptive_huffman_block_compress(input_wrapper *in, output_wrapper *out);

void adaptive_huffman_block_decompress(input_wrapper *in, output_wrapper *out);

void set_block_size(int size);

#endif //HUFFMAN_ADAPTIVE_HUFFMAN_BLOCK_H
