//
// Created by avdyck on 09.10.17.
//

#include "file_write.h"

void set_output_stream(output_wrapper *wrapper, FILE *out) {
    memset(wrapper, 0, sizeof(output_wrapper));
    wrapper->stream = out;
}


void close_bit_writer(output_wrapper *wrapper, bool pad_byte) {
    // Write last bytes
    flush_big_buffer  (wrapper);
    flush_small_buffer(wrapper);

    // Write pad byte
    if (pad_byte) {
        write_pad_byte(wrapper);
    }

    // Reset bit writer
    wrapper->small_index  = 0;
    wrapper->small_buffer = 0;

    fflush(wrapper->stream);
}


void write_pad_byte(output_wrapper *wrapper) {
    char k = (char) (wrapper->small_index & 0b111);
    if (k == 0) k = 8; // If the bit index is 0, our last bit has used 8 bits.

    fwrite(&k, 1, 1, wrapper->stream);
}


void to_next_output_byte(output_wrapper *wrapper) {
    write_n_bits(wrapper, 0, (8 - (wrapper->small_index & 0b111)) & 0b111);
}


void flush_big_buffer(output_wrapper *wrapper) {
    fwrite(&wrapper->big_buffer, sizeof(buffer_t) * wrapper->big_index, 1, wrapper->stream);
}

void flush_small_buffer(output_wrapper *wrapper) {
    wrapper->small_buffer = Swap8Bytes(wrapper->small_buffer);
    fwrite(&wrapper->small_buffer, (wrapper->small_index + 7) / 8, 1, wrapper->stream);
}
