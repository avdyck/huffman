//
// Created by avdyck on 03.10.17.
//

#include "3_adaptive_huffman_slide.h"
#include "../data/adaptive_tree/adaptive_tree.h"

#define DEFAULT_WINDDOW_LENGTH (1 << 12)

// Updates the tree using a sliding window. Only downdates if the
// window is completely filled and the update character is not the same.
// Abuses macro for easier reading
#define update_sliding_window() {       \
    if (did_rotation) {                 \
        down = window[i];               \
        if (down != up) {               \
            downdate_adaptive(down);    \
            update_adaptive(up);        \
        }                               \
    } else {                            \
        update_adaptive(up);            \
        if (i == 0) {                   \
            did_rotation = true;        \
        }                               \
    }                                   \
    window[i] = up;                     \
    i = i == 0 ? window_length-1 : i-1; \
}


static int window_length = DEFAULT_WINDDOW_LENGTH;

void adaptive_huffman_slide_compress(input_wrapper *in, output_wrapper *out) {
    int i, down, up;
    int *window = malloc_s(sizeof(int) * window_length);
    bool did_rotation = false;

    init_input_wrapper(in);
    reset_adaptive_tree();

    i = window_length - 1;
    while (read_byte(in, &up)) {
        encode_adaptive(out, up);

        update_sliding_window();
    }

    free_s(window);
}

void adaptive_huffman_slide_decompress(input_wrapper *in, output_wrapper *out) {
    int i, down, up;
    int *window = malloc_s(sizeof(int) * window_length);
    bool did_rotation = false;

    init_input_wrapper(in);
    reset_adaptive_tree();

    i = window_length - 1;
    if (start_bit_read(in)) {
        while ((up = decode_adaptive(in)) != -1) {
            write_byte(out, up);

            update_sliding_window();
        }
    }

    free_s(window);
}

void set_window_length(int length) {
    window_length = length;
}

#undef update_sliding_window
#undef DEFAULT_WINDDOW_LENGTH