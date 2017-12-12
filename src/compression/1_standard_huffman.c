//
// Created by avdyck on 03.10.17.
//

#include "1_standard_huffman.h"
#include "../data/standard_tree/standard_tree.h"

typedef struct c_cache_entry {
    buffer_t caches[CACHES_AMOUNT];
    unsigned int depth;
    unsigned int width;
} c_cache_entry;

typedef struct d_cache_entry {
    buffer_t symbols;
    byte nbits;
    byte next;
} d_cache_entry;


static void fill_compress_caches(c_cache_entry *caches, standard_tree **trees);
static void fill_decompress_caches(d_cache_entry **caches, standard_tree *root);
static void tag_trees(standard_tree *root);
static standard_tree *find_tree_with_tag(standard_tree *root, int tag);

//    _____                                             //
//   / ____|                                            //
//  | |     ___  _ __ ___  _ __  _ __ ___  ___ ___      //
//  | |    / _ \| '_ ` _ \| '_ \| '__/ _ \/ __/ __|     //
//  | |___| (_) | | | | | | |_) | | |  __/\__ \__ \     //
//   \_____\___/|_| |_| |_| .__/|_|  \___||___/___/     //
//  Compress              |_|                           //

void standard_huffman_compress(input_wrapper *in, output_wrapper *out) {
    ra_file file;
    // Caches to store the paths of each character
    // These caches are key to writing huffman codes in an
    // efficient way. The caches will directly map a symbol
    // to its code (and code length).
    c_cache_entry caches[POSSIBLE_VALS] = {0};

    {   // Prepare for main loop
        standard_tree *root;
        standard_tree *trees[POSSIBLE_VALS] = {};

        // Read whole file into a random access file, for later rewinding
        read_whole_file(in->stream, &file);

        // Generate a huffman tree and write it to the output stream
        root = text_to_tree(trees, &file);
        write_standard_tree(out, root);

        to_next_output_byte(out);

        // Fill the caches and remove the trees from memory (they are not needed anymore)
        fill_compress_caches(caches, trees);
        delete_tree(root);
    }

    // MAIN LOOP
    // Write all codes sequentially to the output stream
    byte *eof = file.buffer + file.size;
    for (byte *bp = file.buffer; bp < eof; bp++) {
        write_n_bits(out, caches[*bp].caches[0], caches[*bp].depth);
        for (int i = caches[*bp].width; i > 0; i--) {
            write_n_bits(out, caches[*bp].caches[i], CACHE_SIZE);
        }
    }

    delete_file(&file);
}


//   _____                                                         //
//  |  __ \                                                        //
//  | |  | | ___  ___ ___  _ __ ___  _ __  _ __ ___  ___ ___       //
//  | |  | |/ _ \/ __/ _ \| '_ ` _ \| '_ \| '__/ _ \/ __/ __|      //
//  | |__| |  __/ (_| (_) | | | | | | |_) | | |  __/\__ \__ \      //
//  |_____/ \___|\___\___/|_| |_| |_| .__/|_|  \___||___/___/      //
//  Decompress                      |_|                            //

void standard_huffman_decompress(input_wrapper *in, output_wrapper *out) {
    standard_tree *root, *tree_ptr;
    int k, bit;

    // Caches to store the symbols we have to output and the next huffman
    // node for each input possible, FOR EACH HUFFMAN NODE (except leaves).
    // These caches are (just like with compression) key to efficiently
    // decoding a huffman encoded stream, as they directly map one byte and
    // a state (huffman node) to n <= 8 output bytes.
    d_cache_entry *caches[POSSIBLE_VALS] = {0};

    // Initialization
    init_input_wrapper(in);
    root = read_standard_tree(in);

    // Fill caches for each non-leaf. Leaves don't need caches because
    // they will always output their symbol and wrap back to the root.
    // This method is not at all memory efficient, but it sure is fast.
    fill_decompress_caches(caches, root);

    // Each non-leaf has a tag, so we can find its cache entry faster and with less memory
    // The root has tag 0
    int tag = 0;


    // MAIN LOOP
    while (in->input_size > 2) {
        unsafe_read_byte(in, &k);
        write_n_bits(out, caches[tag][k].symbols, caches[tag][k].nbits);
        tag = caches[tag][k].next;
    }

    // For the last couple of bits we read bit per bit,
    // cause the last byte can be shorter than the rest
    tree_ptr = find_tree_with_tag(root, tag);
    if (start_bit_read(in)) {
        while (read_one_bit(in, &bit)) {
            tree_ptr = bit ? tree_ptr->right : tree_ptr->left;

            if (!tree_ptr->left) {
                write_byte(out, tree_ptr->value);
                tree_ptr = root;
            }
        }
    }

    // Free all filled up caches
    for (int i = 0; i < POSSIBLE_VALS; i++) {
        if (caches[i]) {
            free_s(caches[i]);
        }
    }

    delete_tree(root);
}


//    _____           _     _                  //
//   / ____|         | |   (_)                 //
//  | |     __ _  ___| |__  _ _ __   __ _      //
//  | |    / _` |/ __| '_ \| | '_ \ / _` |     //
//  | |___| (_| | (__| | | | | | | | (_| |     //
//   \_____\__,_|\___|_| |_|_|_| |_|\__, |     //
//  Caching                         |___/      //

void fill_compress_caches(c_cache_entry *caches, standard_tree **trees) {
    unsigned int k, j, depth;
    standard_tree *next;

    // Cache each character, for faster writing
    for (k = 0; k < POSSIBLE_VALS; k++) {
        if (trees[k]) {
            buffer_t cache = 0;
            depth = 0;
            j = 1;
            next = trees[k];

            while (next->parent) {
                if (depth == CACHE_SIZE) {
                    caches[k].caches[j] = cache;
                    j++;
                    depth = 0;
                }

                ADD_BIT(cache, next->data);
                next = next->parent;
                depth++;
            }

            caches[k].caches[0] = cache;
            caches[k].depth = depth;
            caches[k].width = j - 1;
        }
    }
}


static void fill_decompress_caches_recursive(d_cache_entry **caches, standard_tree *root, standard_tree *tree) {
    // The tree is a leaf, and therefore doesn't need a cached value
    // since a leaf will always output a value
    if (!tree->left)
        return;

    int tag = tree->data;

    caches[tag] = calloc_s(POSSIBLE_VALS, sizeof(d_cache_entry));
    d_cache_entry *cache = caches[tag];

    // For each possible huffman node, we save the symbols we have to output
    // and the next huffman node
    // Reminder: this function is called for each (non-leaf) huffman node.
    // This is not an efficient algorithm, but it works and it doesn't cause
    // too much overhead overall.
    for (int k = 0; k < POSSIBLE_VALS; k++) {
        standard_tree *tree_ptr = tree;

        buffer_t symbols = 0;
        unsigned int index = 0;

        for (int i = 0; i < 8; i++) {
            int bit = k & (1 << (7 - i));
            tree_ptr = bit ? tree_ptr->right : tree_ptr->left;

            if (!tree_ptr->left) {
                index += 8;
                symbols |= ((buffer_t) tree_ptr->value) << (BUFFER_SIZE - index);

                tree_ptr = root;
            }
        }

        cache[k].symbols = symbols;
        cache[k].nbits = index;
        cache[k].next = (byte) tree_ptr->data;
    }

    fill_decompress_caches_recursive(caches, root, tree->left);
    fill_decompress_caches_recursive(caches, root, tree->right);
}


static void fill_decompress_caches(d_cache_entry **caches, standard_tree *root) {
    tag_trees(root);

    fill_decompress_caches_recursive(caches, root, root);
}

//   _______                    //
//  |__   __|                   //
//     | | __ _  __ _ ___       //
//     | |/ _` |/ _` / __|      //
//     | | (_| | (_| \__ \      //
//     |_|\__,_|\__, |___/      //
//  Tags        |___/           //

/**
 * Tags are mappings from bytes to tree pointers, for memory efficiency.
 * Tags run from 0 to (amount of huffman nodes) - (amount of huffman leaves)
 * (Leaves don't need tags and therefore don't get tags).
 */

void tag_trees_recursive(standard_tree *tree, int *tag) {
    tree->data = *tag;

    (*tag)++;
    if (tree->left) {
        if (tree->left->left) {
            tag_trees_recursive(tree->left, tag);
        }
        if (tree->right->left) {
            tag_trees_recursive(tree->right, tag);
        }
    }
}


void tag_trees(standard_tree *root) {
    int tag = 0;
    tag_trees_recursive(root, &tag);
}


standard_tree *find_tree_with_tag(standard_tree *root, int tag) {
    if (!root->left) {
        return NULL;
    }
    if (root->data == tag) {
        return root;
    }
    standard_tree *tmp = find_tree_with_tag(root->left, tag);
    if (tmp) {
        return tmp;
    }
    return find_tree_with_tag(root->right, tag);
}
