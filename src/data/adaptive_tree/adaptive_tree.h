//
// Created by avdyck on 15.10.17.
//

#ifndef HUFFMAN_ADAPTIVE_TREE_H
#define HUFFMAN_ADAPTIVE_TREE_H

#include "../../util/util.h"
#include "../standard_tree/standard_tree.h"

/**
 * Update the adaptive huffman tree by adding k to the tree and rebalancing if necessary.
 */
void   update_adaptive(int k);

/**
 * The inverse of update_adaptive.
 */
void downdate_adaptive(int k);

/**
 * Encodes character k by writing its huffman path to the output stream.
 */
void encode_adaptive(output_wrapper *out, int k);

/**
 * Decodes a character from the input stream and returns the character.
 */
int decode_adaptive(input_wrapper *in);

/**
 * Resets/initializes all values of the adaptive tree.
 */
void reset_adaptive_tree();

/**
 * Convert a standard huffman tree to an adaptive one.
 */
void to_adaptive_tree(standard_tree *t);

#endif //HUFFMAN_ADAPTIVE_TREE_H
