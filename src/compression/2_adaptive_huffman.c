//
// Created by avdyck on 03.10.17.
//

#include "2_adaptive_huffman.h"
#include "../data/adaptive_tree/adaptive_tree.h"

void adaptive_huffman_compress(input_wrapper *in, output_wrapper *out) {
    int k;

    init_input_wrapper(in);
    reset_adaptive_tree();

    while (read_byte(in, &k)) {
        encode_adaptive(out, k);
        update_adaptive(k);
    }
}

void adaptive_huffman_decompress(input_wrapper *in, output_wrapper *out) {
    int k;

    init_input_wrapper(in);
    reset_adaptive_tree();

    start_bit_read(in);
    while ((k = decode_adaptive(in)) != -1) {
        write_byte(out, k);
        update_adaptive(k);
    }
}
