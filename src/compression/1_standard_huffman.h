//
// Created by avdyck on 03.10.17.
//

#ifndef HUFFMAN_STANDARD_HUFFMAN_H
#define HUFFMAN_STANDARD_HUFFMAN_H

#include <stdio.h>
#include "../io/file_read.h"
#include "../io/file_write.h"

void standard_huffman_compress(input_wrapper *in, output_wrapper *out);

void standard_huffman_decompress(input_wrapper *in, output_wrapper *out);

#endif //HUFFMAN_STANDARD_HUFFMAN_H
