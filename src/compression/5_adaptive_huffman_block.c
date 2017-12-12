//
// Created by avdyck on 03.10.17.
//

#include "5_adaptive_huffman_block.h"
#include "../data/adaptive_tree/adaptive_tree.h"

#define BLOCK_SIZE (1 << 15)

static int block_size = BLOCK_SIZE;

// Check if the end of the block is reached. If so, reset the adaptive tree.
// Abuses macro for easier reading
#define check_finished_block() i--; if (i <= 0) { i = block_size; reset_adaptive_tree(); }

void adaptive_huffman_block_compress(input_wrapper *in, output_wrapper *out) {
    int k, i;

    init_input_wrapper(in);
    reset_adaptive_tree();

    i = block_size;
    while (read_byte(in, &k)) {
        encode_adaptive(out, k);
        update_adaptive(k);

        check_finished_block();
    }
}

void adaptive_huffman_block_decompress(input_wrapper *in, output_wrapper *out) {
    int k, i;

    init_input_wrapper(in);
    start_bit_read(in);
    reset_adaptive_tree();

    i = block_size;
    while ((k = decode_adaptive(in)) != -1) {
        write_byte(out, k);
        update_adaptive(k);

        check_finished_block();
    }
}

void set_block_size(int size) {
    block_size = size;
}

#undef check_finished_block
#undef BLOCK_SIZE