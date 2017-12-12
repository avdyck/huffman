//
// Created by avdyck on 07.10.17.
//

#ifndef HUFFMAN_MEMORY_H
#define HUFFMAN_MEMORY_H

//            _ _                 _   _                     //
//      /\   | | |               | | (_)                    //
//     /  \  | | | ___   ___ __ _| |_ _  ___  _ __  ___     //
//    / /\ \ | | |/ _ \ / __/ _` | __| |/ _ \| '_ \/ __|    //
//   / ____ \| | | (_) | (_| (_| | |_| | (_) | | | \__ \    //
//  /_/    \_\_|_|\___/ \___\__,_|\__|_|\___/|_| |_|___/    //
//  Allocations                                             //

/**
 * These functions will 'safely' allocate objects
 * (if there is not enough memory, they will throw an error).
 */

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define realloc_s(ptr, size) REALLOC_S(ptr, size, __LINE__, __PRETTY_FUNCTION__, __FILENAME__)
#define malloc_s(size)       MALLOC_S(      size, __LINE__, __PRETTY_FUNCTION__, __FILENAME__)
#define calloc_s(amt, size)  CALLOC_S(amt,  size, __LINE__, __PRETTY_FUNCTION__, __FILENAME__)


void *REALLOC_S(void *ptr, unsigned int size, int lineNumber, const char *function, const char *file);

void *MALLOC_S(unsigned int size, int lineNumber, const char *function, const char *file);

void *CALLOC_S(unsigned int amt, unsigned int size, int lineNumber, const char *function, const char *file);

void free_s(void *ptr);


//   __  __                _            _      _______        _   _                 //
//  |  \/  |              | |          | |    |__   __|      | | (_)                //
//  | \  / | ___ _ __ ___ | | ___  __ _| | __    | | ___  ___| |_ _ _ __   __ _     //
//  | |\/| |/ _ \ '_ ` _ \| |/ _ \/ _` | |/ /    | |/ _ \/ __| __| | '_ \ / _` |    //
//  | |  | |  __/ | | | | | |  __/ (_| |   <     | |  __/\__ \ |_| | | | | (_| |    //
//  |_|  |_|\___|_| |_| |_|_|\___|\__,_|_|\_\    |_|\___||___/\__|_|_| |_|\__, |    //
//  Memleak Testing                                                       |___/     //

void INIT_MEMLEAK_TESTING();

void GET_MEMLEAKS();

#endif //HUFFMAN_MEMORY_H
