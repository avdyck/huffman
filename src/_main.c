//
// Created by avdyck on 10.10.17.
//


#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>
#include <assert.h>
#include "compression/1_standard_huffman.h"
#include "compression/2_adaptive_huffman.h"
#include "compression/3_adaptive_huffman_slide.h"
#include "compression/4_adaptive_huffman_twopass.h"
#include "compression/5_adaptive_huffman_block.h"
#include "util/error.h"


typedef void (*huffman_funcpointer)(input_wrapper *, output_wrapper *);

static huffman_funcpointer   compress_functions[5] = {
        standard_huffman_compress,
        adaptive_huffman_compress,
        adaptive_huffman_slide_compress,
        adaptive_huffman_two_pass_compress,
        adaptive_huffman_block_compress
};

static huffman_funcpointer decompress_functions[5] = {
        standard_huffman_decompress,
        adaptive_huffman_decompress,
        adaptive_huffman_slide_decompress,
        adaptive_huffman_two_pass_decompress,
        adaptive_huffman_block_decompress
};

static huffman_funcpointer *huffman_functions[5] = {
        compress_functions,
        decompress_functions
};


int _main(int argc, char **argv) {
    FILE *in = stdin, *out = stdout;
    bool in_is_file = false, out_is_file = false;
    int opt, size, type = 1;
    bool compress = true;
    input_wrapper in_wrapper;
    output_wrapper out_wrapper;

    optind = 1;

    while ((opt = getopt(argc, argv, "dct:i:o:l:b:")) != -1) {
        switch (opt) {
            case 't':
                type = (int) strtol(optarg, NULL, 10);
                break;

            case 'c':
                compress = true;
                break;

            case 'd':
                compress = false;
                break;

            case 'i':
                in_is_file = true;
                in = fopen(optarg, "rb");  if (in  == NULL) throw("Input file not defined.\n");
                break;

            case 'o':
                out_is_file = true;
                out = fopen(optarg, "wb"); if (out == NULL) throw("Output file error.\n");
                break;

            case 'l':
            case 'b':
                size = (int) strtol(optarg, NULL, 10);
                if (size > 0) {
                    set_window_length(size);
                    set_block_size   (size);
                }
                break;

            case '?':
                fprintf(stderr, "Usage: %s [-t type] [-dc]\n", argv[0]);
                return EXIT_FAILURE;

            default:
                abort();
        }
    }

    assert(type >= 1 && type <= 5);

    set_input_stream (&in_wrapper,   in);
    set_output_stream(&out_wrapper, out);

    int k = getc(in);
    if (k != EOF) {
        ungetc(k, in);
        huffman_functions[compress ? 0 : 1][type - 1](&in_wrapper, &out_wrapper);
        close_bit_writer(&out_wrapper, compress);
    }

    if (in_is_file)  fclose(in);
    if (out_is_file) fclose(out);

    return 0;
}