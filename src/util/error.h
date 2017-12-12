//
// Created by avdyck on 07.10.17.
//

#ifndef HUFFMAN_ERROR_H
#define HUFFMAN_ERROR_H

/**
 * Throw an error with a message. The error will display the current line, file and function.
 */
#define throw(message) THROW(__PRETTY_FUNCTION__, __FILE__, __LINE__, message)

void THROW(const char *fun, const char *file, int line, char *message);

#endif //HUFFMAN_ERROR_H
