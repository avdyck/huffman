//
// Created by avdyck on 15.10.17.
//

#include <assert.h>
#include "adaptive_tree.h"

// See mental note for more clarification
static int parent           [    POSSIBLE_VALS + 1]; // parent of 2j and 2j-1 is parent[j]
static int children         [2 * POSSIBLE_VALS + 1]; // children  2j and 2j-1 of k -> children[k] = j
static int byte_order       [    POSSIBLE_VALS    ]; // Order of root is 0 (!! inverted order for easier coding)
static unsigned long weights[2 * POSSIBLE_VALS + 1];

// Current largest order number.
// This will be the place of the null node.
static int order_nr;

// Swap x and y (also swaps the pointers)
#define swap(x, y) if ((x) != (y)) { SWAP(x, y); (x) ^= (y); (y) ^= (x); (x) ^= (y); }
static void SWAP(int tree, int tree_);

// Find the tree with the largest/smallest order number that has the same weight as parameter tree
static int  largest_in_block(int tree);
static int smallest_in_block(int tree);

// Check if the tree is still a huffman tree
bool is_huffman_correct();


//   __  __            _        _   _   _       _                               //
//  |  \/  |          | |      | | | \ | |     | |                              //
//  | \  / | ___ _ __ | |_ __ _| | |  \| | ___ | |_ ___                         //
//  | |\/| |/ _ \ '_ \| __/ _` | | | . ` |/ _ \| __/ _ \                        //
//  | |  | |  __/ | | | || (_| | | | |\  | (_) | ||  __/                        //
//  |_|  |_|\___|_| |_|\__\__,_|_| |_| \_|\___/ \__\___|                        //
//  Mental note                                                                 //
//                                                                              //
//                     ___                                                      //
//                    /   \                                                     //
//                 0 |     |                    parent                          //
//                    \___/                          [0] = 0     (1 & 2 -> 0)   //
//                   / / \ \                         [1] = 1     (3 & 4 -> 1)   //
//                  / /   \ \                        [2] = 4     (5 & 6 -> 4)   //
//                 / /     \ \                                                  //
//                / /       \ \                 children                        //
//              _/_/         \_\_                    [0] = 1     (0 -> 1 & 2)   //
//             /   \         /   \                   [1] = 2     (1 -> 3 & 4)   //
//          2 |  a  |     1 |     |                  [2] = -98   (2 -> 'a')     //
//             \___/         \___/                   [3] = -104  (3 -> 'g')     //
//                          / / \ \                  [4] = 3     (4 -> 5 & 6)   //
//                         / /   \ \                 [5] = -115  (5 -> 'r')     //
//                        / /     \ \                [6] = 0     (6 -> NULL)    //
//                       / /       \ \                                          //
//                     _/_/         \_\_                                        //
//                    /   \         /   \                                       //
//                 4 |     |     3 |  g  |                                      //
//                    \___/         \___/                                       //
//                   / / \ \                                                    //
//                  / /   \ \                                                   //
//                 / /     \ \                                                  //
//                / /       \ \                                                 //
//              _/_/         \_\_                                               //
//             /   \         /   \                                              //
//          6 | NUL |     5 |  r  |                                             //
//             \___/         \___/                                              //
//                                                                              //
//                                                                              //


//   _    _           _       _            //
//  | |  | |         | |     | |           //
//  | |  | |_ __   __| | __ _| |_ ___      //
//  | |  | | '_ \ / _` |/ _` | __/ _ \     //
//  | |__| | |_) | (_| | (_| | ||  __/     //
//   \____/| .__/ \__,_|\__,_|\__\___|     //
//  Update |_|                             //

void update_adaptive( int k)  {
    int tree, tree_;

    weights[0]++;

    tree = byte_order[k];

    if (tree == -1) { // node is not contained in the huffman tree
        parent  [order_nr / 2] = order_nr;

        children[order_nr + 0] = order_nr / 2 + 1; // internal node ((order_nr + 2) / 2)
        children[order_nr + 1] = ~k;               // external node
//      children[order_nr + 2] =  0;               // external node (was already 0)

        weights[order_nr + 0] = 1;
        weights[order_nr + 1] = 1;
//      weights[order_nr + 2] = 0;

        byte_order[k] = order_nr + 1;

        if (order_nr != 0) {
            tree = parent[(order_nr - 1) / 2];
        }

        order_nr += 2;

        if (tree == -1) {
            return;
        }
    }

    // Swap the tree with the smallest tree in the block,
    // move the tree to the next block and repeat for its new parent
    while (tree != 0) {
        tree_ = smallest_in_block(tree);
        if (tree_ != parent[(tree - 1) / 2]) {
            swap(tree, tree_);
        }

        weights[tree]++;
        tree = parent[(tree - 1) / 2];
    }

#ifdef __TESTING__
    assert(is_huffman_correct());
#endif
}


//   _____                          _       _           //
//  |  __ \                        | |     | |          //
//  | |  | | _____      ___ __   __| | __ _| |_ ___     //
//  | |  | |/ _ \ \ /\ / / '_ \ / _` |/ _` | __/ _ \    //
//  | |__| | (_) \ V  V /| | | | (_| | (_| | ||  __/    //
//  |_____/ \___/ \_/\_/ |_| |_|\__,_|\__,_|\__\___|    //
//  Downdate                                            //

void downdate_adaptive( int k) {
    int tree, tree_;

    tree = byte_order[k];

    // Inverse of update_adaptive
    while (tree != 0) {
        tree_ = largest_in_block(tree);
        swap(tree_, tree);

        weights[tree]--;
        tree = parent[(tree - 1) / 2];
    }

    weights[tree]--;

    tree = byte_order[k];

    // Shrink the tree if necessary
    if (weights[tree] == 0) {
        byte_order[k] = -1;
        children[tree] = 0;

        if (tree == order_nr - 1) {
            order_nr -= 2;

            parent  [order_nr / 2] = -1; // The parent of the deleted nodes is now nonexistent
//          children[order_nr + 1] =  0; // We just did that ~5 lines ago
            children[order_nr + 0] =  0; // The new null node

            // The parent of the null node has to be (order_nr - 2) or the balance
            // of the tree can be broken after an update.
            if (parent[(order_nr-1) / 2] != order_nr - 2) {
                SWAP(parent[(order_nr-1) / 2], order_nr - 2);
            }
        }
    }

#ifdef __TESTING__
    assert(is_huffman_correct());
#endif
}


//  ____  _            _            //
// |  _ \| |          | |           //
// | |_) | | ___   ___| | _____     //
// |  _ <| |/ _ \ / __| |/ / __|    //
// | |_) | | (_) | (__|   <\__ \    //
// |____/|_|\___/ \___|_|\_\___/    //
// Blocks                           //

// This can buffer overflow if you give a tree node with weight 0.
// But we never want to find the largest in
int largest_in_block(int tree) {
    unsigned long w = weights[tree];
    while (weights[tree + 1] == w) {
        tree++;
    }

    return tree;
}


int smallest_in_block(int tree) {
    unsigned long w = weights[tree];
    while (weights[tree - 1] == w) {
        tree--;
    }

    return tree;
}


//   _____       _ _       ________                 //
//  |_   _|     (_) |     / /  ____|                //
//    | |  _ __  _| |_   / /| |__ _ __ ___  ___     //
//    | | | '_ \| | __| / / |  __| '__/ _ \/ _ \    //
//   _| |_| | | | | |_ / /  | |  | | |  __/  __/    //
//  |_____|_| |_|_|\__/_/   |_|  |_|  \___|\___|    //
//  Init/Free                                       //

void reset_adaptive_tree() {
    memset(parent,         -1, sizeof(parent));
    memset(weights,         0, sizeof(weights));
    memset(children,        0, sizeof(children));
    memset(byte_order,     -1, sizeof(byte_order));

    order_nr = 0;
}


//     _____                       //
//    / ____|                      //
//   | (_____      ____ _ _ __     //
//    \___ \ \ /\ / / _` | '_ \    //
//    ____) \ V  V / (_| | |_) |   //
//   |_____/ \_/\_/ \__,_| .__/    //
//  Swap                 |_|       //

static inline void SWAP(int tree, int tree_) {
    int child  = children[tree ];
    int child_ = children[tree_];

    if (child  > 0)      parent[child  - 1 ] = tree_;
    else if (child  < 0) byte_order[~child ] = tree_;

    if (child_ > 0)      parent[child_ - 1 ] = tree ;
    else if (child_ < 0) byte_order[~child_] = tree ;

    children[tree ] = child_;
    children[tree_] = child ;
}


//   ______                     _          //
//  |  ____|                   | |         //
//  | |__   _ __   ___ ___   __| | ___     //
//  |  __| | '_ \ / __/ _ \ / _` |/ _ \    //
//  | |____| | | | (_| (_) | (_| |  __/    //
//  |______|_| |_|\___\___/ \__,_|\___|    //
//  Encode                                 //

void encode_adaptive( output_wrapper *out,  int k) {
    int depth, j, o;

    // o is the order number of the tree we need to encode
    o = byte_order[k];
    if (o == -1) {
        o = order_nr;
    }

    // We can't immediately write because we need to write from up to down
    // That's why the character is cached first.
    buffer_t caches[CACHES_AMOUNT];
    buffer_t cache = 0;
    j = 0; depth = 0;

    while (o) {
        if (depth == CACHE_SIZE) {
            caches[j++] = cache;
            depth = 0;
        }
        ADD_BIT(cache, o & 1);
        o = parent[(o - 1)/2];
        depth++;
    }

    // Write the caches to the output stream
    write_n_bits(out, cache, depth);
    while (j > 0) {
        j--;
        write_n_bits(out, caches[j], CACHE_SIZE);
    }

    // Write the character out if we've written the path to the null-node
    if (byte_order[k] == -1) {
        write_byte(out, k);
    }
}


//   _____                     _          //
//  |  __ \                   | |         //
//  | |  | | ___  ___ ___   __| | ___     //
//  | |  | |/ _ \/ __/ _ \ / _` |/ _ \    //
//  | |__| |  __/ (_| (_) | (_| |  __/    //
//  |_____/ \___|\___\___/ \__,_|\___|    //
//  Decode                                //

int decode_adaptive(input_wrapper *in) {
    int child, i, b;

    child = children[0];

    // Read the tree path
    while (child > 0) {
        if (!read_one_bit(in, &b)) return EOF;
        child = children[2 * child - (b ? 1 : 0)];
    }

    // decode zero weight
    if (child == 0) {
        int result = 0;

        for (i = 0; i < 8; i++) {
            if (!read_one_bit(in, &b)) return EOF;
            result |= (b ? 1 : 0) << (7 - i);
        }

        return result;
    }

    return ~child;
}


//    _____                              _                 //
//   / ____|                            (_)                //
//  | |     ___  _ ____   _____ _ __ ___ _  ___  _ __      //
//  | |    / _ \| '_ \ \ / / _ \ '__/ __| |/ _ \| '_ \     //
//  | |___| (_) | | | \ V /  __/ |  \__ \ | (_) | | | |    //
//   \_____\___/|_| |_|\_/ \___|_|  |___/_|\___/|_| |_|    //
//  Conversion                                             //

// This way of converting a standard tree to an adaptive tree assumes the standard tree
// was setup using a deterministic algorithm (where the smallest tree node is either always left or right)
void to_adaptive_tree(standard_tree *t) {
#define n (2* POSSIBLE_VALS)
#define increment(x) ((x) = (n == (x) + 1) ? 0 : (x) + 1)

    standard_tree *queue[n] = {};
    int begin, end;

    begin = 0;
    end = 1;
    queue[0] = t;
    order_nr = -1;

    // Traverse the standard huffman tree in a breadth-first way and add
    // all tree nodes to the adaptive tree.
    while (begin != end) {
        t = queue[begin];
        increment(begin);

        order_nr++;

        weights[order_nr] = t->weight;

        if (t->left) { // The tree has children
            if (t->left->weight >= t->right->weight) {
                queue[end] = t->left;
                increment(end);
                queue[end] = t->right;
                increment(end);
            } else {
                // This can only happen if the standard huffman algorithm slightly changes
                queue[end] = t->right;
                increment(end);
                queue[end] = t->left;
                increment(end);
            }

            // Set the tree code to the current order number, so we can relocate the node in the adaptive tree later
            t->data = order_nr;
        } else if (t->weight > 0) {
            // The tree has no children, so the tree has a value
            int k = t->value;

            children[order_nr] = ~k;
            byte_order[k] = order_nr;
        } else {
            // Null tree
            children[order_nr] = 0;
        }

        if (t->parent) {
            // The parent has already been traversed,
            // so we know the location of the parent in the adaptive tree.
            int par   = t->parent->data;
            int code = (order_nr - 1) / 2;

            children[par] = code + 1;
            parent[code]  = par;
        }
    }

#ifdef __TESTING__
    assert(is_huffman_correct());
#endif

#undef increment
#undef n

}


//   _______        _        //
//  |__   __|      | |       //
//     | | ___  ___| |_      //
//     | |/ _ \/ __| __|     //
//     | |  __/\__ \ |_      //
//     |_|\___||___/\__|     //
//  Test                     //

// Simple test to check if the weights is in descending order
bool is_huffman_correct() {
    for (int i = 0; i < order_nr; i++) {
        if (weights[i] < weights[i + 1]) {
            return false;
        }
    }

    return true;
}
