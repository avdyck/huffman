//
// Created by avdyck on 03.10.17.
//

#include "4_adaptive_huffman_twopass.h"
#include "../data/adaptive_tree/adaptive_tree.h"
#include "../io/ra_file.h"

void adaptive_huffman_two_pass_compress(input_wrapper *in, output_wrapper *out) {
    int k;
    ra_file file;

    // Read whole file into a random access file, for later rewinding
    read_whole_file(in->stream, &file);

    // First generate a standard huffman tree
    standard_tree *root;
    standard_tree *trees[POSSIBLE_VALS] = {};
    root = text_to_tree(trees, &file);

    // Write the tree and the frequencies to the output
    write_standard_tree(out, root);
    write_frequencies(out, root);

    // Convert the tree to an adaptive tree
    reset_adaptive_tree();
    to_adaptive_tree(root);

    to_next_output_byte(out);

    for (size_t i = 0; i < file.size; i++) {
        k = file.buffer[i];
        encode_adaptive(out, k);
        downdate_adaptive(k);
    }

    delete_file(&file);
    delete_tree(root);
}

void adaptive_huffman_two_pass_decompress(input_wrapper *in, output_wrapper *out) {
    int k;
    standard_tree *t;

    // Read the huffman tree from input
    init_input_wrapper(in);
    t = read_standard_tree(in);
    read_frequencies(in, t);

    // Convert the standard huffman tree to an adaptive tree
    reset_adaptive_tree();
    to_adaptive_tree(t);
    delete_tree(t);

    if (start_bit_read(in)) {
        while ((k = decode_adaptive(in)) != -1) {
            write_byte(out, k);
            downdate_adaptive(k);
        }
    }
}

