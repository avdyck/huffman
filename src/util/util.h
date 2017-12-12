//
// Created by avdyck on 06.10.17.
//

#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include "memory.h"

#ifndef HUFFMAN_UTIL_H
#define HUFFMAN_UTIL_H

// Byte stuff
typedef u_int8_t byte;
#define POSSIBLE_VALS (1 << 8)

// Buffer type
typedef uint64_t buffer_t;
#define BUFFER_BYTES (sizeof(buffer_t))
#define BUFFER_SIZE (8*BUFFER_BYTES)
#define LEFT_BIT (((buffer_t) 1) << (BUFFER_SIZE - 1))

// Caches
#define ADD_BIT(cache, bit) cache = ((cache) >> 1) | ((bit) ? LEFT_BIT : 0)
#define CACHE_SIZE (8 * sizeof(buffer_t))
#define CACHES_AMOUNT (POSSIBLE_VALS/CACHE_SIZE)



// Little endian/ Big endian
#if defined(__BYTE_ORDER__)&&(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#define Swap8Bytes(val) \
  ((((val) >> 56) & 0x00000000000000FF) | \
   (((val) >> 40) & 0x000000000000FF00) | \
   (((val) >> 24) & 0x0000000000FF0000) | \
   (((val) >>  8) & 0x00000000FF000000) | \
   (((val) <<  8) & 0x000000FF00000000) | \
   (((val) << 24) & 0x0000FF0000000000) | \
   (((val) << 40) & 0x00FF000000000000) | \
   (((val) << 56) & 0xFF00000000000000) )

#define Swap4Bytes(val) \
   ((((val) >> 24) & 0x000000FF) | \
    (((val) >>  8) & 0x0000FF00) | \
    (((val) <<  8) & 0x00FF0000) | \
    (((val) << 24) & 0xFF000000) )

#define Swap2Bytes(val) \
    ((((val) >> 8) & 0x00FF) | \
     (((val) << 8) & 0xFF00) )
#else
#define Swap8Bytes(val) val
#define Swap4Bytes(val) val
#define Swap2Bytes(val) val
#endif


// Timer methods
#define TIMER_INIT() clock_t start, diff; int msec
#define TIMER_START() start = clock()
#define TIMER_STOP() diff = clock() - start; \
                     msec = diff * 1000 / CLOCKS_PER_SEC; \
                     printf("Time taken %d seconds %d milliseconds\n", msec/1000, msec%1000);

#endif //HUFFMAN_UTIL_H
