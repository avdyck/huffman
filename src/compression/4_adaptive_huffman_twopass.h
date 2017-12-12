//
// Created by avdyck on 03.10.17.
//

#ifndef HUFFMAN_ADAPTIVE_HUFFMAN_TWOPASS_H
#define HUFFMAN_ADAPTIVE_HUFFMAN_TWOPASS_H

#include <stdio.h>
#include "../io/file_read.h"
#include "../io/file_write.h"

void adaptive_huffman_two_pass_compress(input_wrapper *in, output_wrapper *out);

void adaptive_huffman_two_pass_decompress(input_wrapper *in, output_wrapper *out);

#endif //HUFFMAN_ADAPTIVE_HUFFMAN_TWOPASS_H
