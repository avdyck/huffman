//
// Created by avdyck on 06.10.17.
//

#include <stdlib.h>
#include <stdint.h>
#include "standard_tree.h"
#include "../../util/error.h"
#include "../../io/ra_file.h"

standard_tree *create_tree() {
    return (standard_tree *) calloc_s(1, sizeof(standard_tree));
}


void delete_tree(standard_tree *t) {
    if (t == NULL)
        return;

    if (t->left != NULL)
        delete_tree(t->left);

    if (t->right != NULL)
        delete_tree(t->right);

    free_s(t);
}


standard_tree *merge_tree(standard_tree *left, standard_tree *right) {
    standard_tree *t = create_tree();

    left->data = 0;
    left->parent = t;
    right->data = 1;
    right->parent = t;


    t->left = left;
    t->right = right;
    t->weight = left->weight + right->weight;

    return t;
}


//    _____                                      _                                          //
//   / ____|                                    | |                                         //
//  | |     ___  _ __ ___  _ __   __ _ _ __ __ _| |_ ___  _ __                              //
//  | |    / _ \| '_ ` _ \| '_ \ / _` | '__/ _` | __/ _ \| '__|                             //
//  | |___| (_) | | | | | | |_) | (_| | | | (_| | || (_) | |                                //
//   \_____\___/|_| |_| |_| .__/ \__,_|_|  \__,_|\__\___/|_|                                //
//  Comparator            |_|                                                               //

static int huff_comp(const void *a, const void *b) {
    unsigned long a_weight = (*((standard_tree **) a))->weight;
    unsigned long b_weight = (*((standard_tree **) b))->weight;
    long diff = a_weight - b_weight;

    return diff > 0 ? 1 : (diff < 0 ? -1 : 0);
}


//   _______        _     _______      _______                                            //
//  |__   __|      | |   |__   __|    |__   __|                                           //
//     | | _____  _| |_     | | ___      | |_ __ ___  ___                                 //
//     | |/ _ \ \/ / __|    | |/ _ \     | | '__/ _ \/ _ \                                //
//     | |  __/>  <| |_     | | (_) |    | | | |  __/  __/                                //
//     |_|\___/_/\_\\__|    |_|\___/     |_|_|  \___|\___|                                //
// Text To Tree                                                                           //

#define QUEUE_1 1
#define QUEUE_2 2
#define EMPTY   0

static standard_tree *merge_all_trees(standard_tree **trees);

standard_tree *text_to_tree(standard_tree **trees, ra_file *file) {
    int k;
    unsigned int diff_bytes = 0;

    // The root of the huffman tree
    standard_tree *root = NULL;

    // Get the frequency of each character
    for (size_t i = 0; i < file->size; i++) {
        k = (int) file->buffer[i];

        if (!trees[k]) {
            diff_bytes++;
            trees[k] = create_tree();
            trees[k]->value = k;
        }
        trees[k]->weight++;
    }

    // Merge all trees into a correct huffman tree
    root = merge_all_trees(trees);

    if (diff_bytes == 1) { // Add a null standard_tree if there is only one node
        root = merge_tree(create_tree(), root);
    }

    return root;
}


static int smallest_queue(standard_tree **q1, unsigned int q1_b, unsigned int q1_e,
                          standard_tree **q2, unsigned int q2_b, unsigned int q2_e) {
    if (q2_b == q2_e) { // q2 is empty
        if (q1_b == q1_e)
            return EMPTY; // q1 is also empty
        return QUEUE_1;   // only q2 is empty
    }

    if (q1_b == q1_e)
        return QUEUE_2;   // only q1 is empty

    // Both queues are not empty
    if (q1[q1_b]->weight <= q2[q2_b]->weight)
        return QUEUE_1;
    else
        return QUEUE_2;
}

standard_tree *merge_all_trees(standard_tree **trees) {
    // Two (circular) arrays used as queues by using a sliding window
    standard_tree *q1[POSSIBLE_VALS] = {}, *q2[POSSIBLE_VALS] = {};
    unsigned int q1_begin, q1_end; // q1 begin and end index
    unsigned int q2_begin, q2_end; // same for q2

    q1_begin = (q1_end = 0);
    q2_begin = (q2_end = 0);

    // Enqueue all (single) tree nodes into queue 1
    for (int i = 0; i < POSSIBLE_VALS; i++) {
        if (trees[i]) {
            q1[q1_end] = trees[i];
            q1_end++;
        }
    }

    // Sort queue 1 by weight (smallest weights first)
    qsort(q1, q1_end, sizeof(standard_tree *), huff_comp);

    // For each step in this loop, we dequeue the two 'smallest' trees
    // from both queues combined, and we requeue the merged tree of those two
    // trees into queue 2. The last step is when we only have one node left.
    // This node will be the root of the huffman tree.
    standard_tree *t[2] = {};
    for (;;) {
        for (int i = 0; i < 2; i++) {
            int q = smallest_queue(q1, q1_begin, q1_end, q2, q2_begin, q2_end);
            switch(q) {
                case QUEUE_1:
                    t[i] = q1[q1_begin++];
                    break;
                case QUEUE_2:
                    t[i] = q2[q2_begin++];
                    if (q2_begin == POSSIBLE_VALS)
                        q2_begin = 0;
                    break;
                default:
                    return t[0];
            }
        }

        q2[q2_end++] = merge_tree(t[0], t[1]);
        if (q2_end == POSSIBLE_VALS)
            q2_end = 0;
    }
}


#undef QUEUE_1
#undef QUEUE_2
#undef EMPTY


//   _____                _                                                             //
//  |  __ \              | |                                                            //
//  | |__) |___  __ _  __| |                                                            //
//  |  _  // _ \/ _` |/ _` |                                                            //
//  | | \ \  __/ (_| | (_| |                                                            //
//  |_|  \_\___|\__,_|\__,_|                                                            //
//  Read                                                                                //

standard_tree *read_paths(input_wrapper *in) {
    int bit;

    if (read_one_bit(in, &bit)) {
        if (bit) {
            standard_tree *t = create_tree();
            return t;
        } else {
            standard_tree *t1 = read_paths(in);
            standard_tree *t2 = read_paths(in);
            return merge_tree(t1, t2);
        }
    } else {
        throw("Unexpected EOF");
    }

    // Unreachable
    return NULL;
}


void read_codes(input_wrapper *in, standard_tree *t) {
    if (t->left == 0) {
        read_byte(in, &t->value);
    } else {
        read_codes(in, t->left);
        read_codes(in, t->right);
    }
}


standard_tree *read_standard_tree(input_wrapper *in) {
    standard_tree *t = NULL;

    if (start_bit_read(in)) {
        t = read_paths(in);
        read_codes(in, t);
    }

    return t;
}


void read_frequencies(input_wrapper *in, standard_tree *t) {
    if (t->left == 0) {
        int i, k;

        union {
            uint32_t i;
            byte b[sizeof(uint32_t)];
        } un;

        for (i = 0; i < sizeof(uint32_t); i++) {
            read_byte(in, &k);
            un.b[i] = (byte) k;
        }

        un.i = Swap4Bytes(un.i);

        t->weight = un.i;
    } else {
        read_frequencies(in, t->left);
        read_frequencies(in, t->right);
        t->weight = t->left->weight + t->right->weight;
    }
}


//   __          __   _ _                                                               //
//   \ \        / /  (_) |                                                              //
//    \ \  /\  / / __ _| |_ ___                                                         //
//     \ \/  \/ / '__| | __/ _ \                                                        //
//      \  /\  /| |  | | ||  __/                                                        //
//       \/  \/ |_|  |_|\__\___|                                                        //
//  Write                                                                               //

void write_paths(output_wrapper *out, standard_tree *t) {
    if (t->left == 0) {
        write_n_bits(out, LEFT_BIT, 1);
    } else {
        write_n_bits(out, 0, 1);
        write_paths(out, t->left);
        write_paths(out, t->right);
    }
}


void write_codes(standard_tree *t, output_wrapper *out) {
    if (t->left == 0) {
        write_byte(out, t->value);
    } else {
        write_codes(t->left, out);
        write_codes(t->right, out);
    }
}


void write_standard_tree(output_wrapper *out, standard_tree *t) {
    write_paths(out, t);

    to_next_output_byte(out);
    write_codes(t, out);
}


void write_frequencies(output_wrapper *out, standard_tree *t) {
    if (t->left) {
        write_frequencies(out, t->left);
        write_frequencies(out, t->right);
    } else {
        write_n_bits(out, ((buffer_t) t->weight) << (BUFFER_SIZE - (8*sizeof(uint32_t))), (8*sizeof(uint32_t)));
    }
}