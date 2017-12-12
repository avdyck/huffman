//
// Created by avdyck on 06.10.17.
//

#ifndef HUFFMAN_HUFFMAN_TREE_H
#define HUFFMAN_HUFFMAN_TREE_H

#include <stdio.h>
#include "../../util/util.h"
#include "../../io/file_read.h"
#include "../../io/file_write.h"
#include "../../io/ra_file.h"

typedef struct standard_tree {
    struct standard_tree *left;
    struct standard_tree *right;
    int value;
    unsigned long weight;

    struct standard_tree *parent;

    // Some data the tree can hold
    int data;
} standard_tree;

/**
 * Allocate a new tree node
 */
standard_tree *create_tree();

/**
 * Free the allocated tree node
 */
void delete_tree(standard_tree *t);

/**
 * Return a new tree node with the left and right tree as left and right children respectively.
 */
standard_tree *merge_tree(standard_tree *left, standard_tree *right);

/**
 * Read/write the huffman tree configuration to the output stream.
 */
void write_standard_tree(output_wrapper *out, standard_tree *t);
standard_tree *read_standard_tree(input_wrapper *in);

/**
 * Convert the standard input to a standard huffman tree,
 * using a random access file.
 */
standard_tree *text_to_tree(standard_tree **trees, ra_file *file);

/**
 * Read/write the frequencies of the huffman tree to the output stream.
 */
void write_frequencies(output_wrapper *out, standard_tree *t);
void  read_frequencies( input_wrapper *in,  standard_tree *t);


#endif //HUFFMAN_HUFFMAN_TREE_H
