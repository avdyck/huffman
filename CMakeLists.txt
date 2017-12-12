cmake_minimum_required(VERSION 3.8)
project(huffman)

set(CMAKE_C_STANDARD 99)


set(TEST_FILES
        tests/test_main.c
        tests/minunit.h
        tests/minunit.c
        tests/compression/test_huffman.c
        tests/compression/test_huffman.h)


set(SOURCE_FILES
        src/compression/1_standard_huffman.h
        src/compression/1_standard_huffman.c
        src/compression/2_adaptive_huffman.h
        src/compression/2_adaptive_huffman.c
        src/compression/3_adaptive_huffman_slide.h
        src/compression/3_adaptive_huffman_slide.c
        src/compression/4_adaptive_huffman_twopass.h
        src/compression/4_adaptive_huffman_twopass.c
        src/compression/5_adaptive_huffman_block.h
        src/compression/5_adaptive_huffman_block.c
        src/data/linked_list/linked_list.c
        src/data/linked_list/linked_list.h
        src/data/adaptive_tree/adaptive_tree.c
        src/data/adaptive_tree/adaptive_tree.h
        src/io/file_read.c
        src/io/file_read.h
        src/io/file_write.c
        src/io/file_write.h
        src/util/util.h
        src/util/error.c
        src/util/error.h
        src/util/memory.c
        src/util/memory.h
        src/data/standard_tree/standard_tree.c
        src/data/standard_tree/standard_tree.h
        src/_main.c
        src/_main.h
        src/io/ra_file.c
        src/io/ra_file.h)

set(MAIN src/main.c)

add_executable(huffman
        ${MAIN}
        ${SOURCE_FILES})

add_executable(huffman_test
        ${TEST_FILES}
        ${SOURCE_FILES})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -O3 -Wall")
target_compile_definitions(huffman_test PUBLIC -D__TESTING__ -DFILE_TESTING)
