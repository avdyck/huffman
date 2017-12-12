//
// Created by avdyck on 03.10.17.
//

#include <stdlib.h>
#include "file_read.h"


unsigned int read_into_buffer(input_wrapper *wrapper) {
    return (unsigned int) fread(&hidden_buffer(wrapper), 1, INPUT_BUFFER_SIZE, wrapper->stream);
}


void set_input_stream(input_wrapper *wrapper, FILE *in) {
    memset(wrapper, 0, sizeof(input_wrapper));
    wrapper->stream = in;
}


bool start_bit_read(input_wrapper *in) {
    if (!read_byte(in, &in->bit_buffer))
        return false;

    if (in->input_size == 1) {
        // Decode the pad byte.
        int l;
        read_byte(in, &l);

        in->bit_index = 8 - l;
        in->bit_buffer >>= in->bit_index;
    } else {
        in->bit_index = 0;
    }

    return true;
}


void init_input_wrapper(input_wrapper *wrapper) {
    wrapper->input_index = INPUT_BUFFER_SIZE;
    wrapper->input_size  = read_into_buffer(wrapper);
}
