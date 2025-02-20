#pragma once

#include <stdint.h>

#define INCREMENT 256

/*
 * This is the header for each allocated memory used internally by the
 * allocator. The test cases use this too to get the size of the header.
 */
#pragma pack(push, 1)
struct header {
  uint64_t size;
  struct header *next;
};
#pragma pack(pop)

/*
 * Allocation algorithm options
 */
enum algs { FIRST_FIT, BEST_FIT, WORST_FIT };

/*
 * Allocation statistics. The test cases only use free_size, but other fields
 * are useful for checking the correctness of the implementation.
 */
struct allocinfo {
  uint64_t free_size;
  uint64_t free_chunks;
  uint64_t largest_free_chunk_size;
  uint64_t smallest_free_chunk_size;
};

/*
 * alloc() allocates memory from the heap. The first argument indicates the
 * size. It returns the pointer to the newly-allocated memory. It returns NULL
 * if there is not enough space.
 */
void *alloc(int);

/*
 * dealloc() frees the memory pointed to by the first argument.
 */
void dealloc(void *);

/*
 * allocopt() sets the options for the memory allocator.
 *
 * The first argument sets the algorithm. The second argument sets the size
 * limit.
 */
void allocopt(enum algs, int);

/*
 * allocinfo() returns the current statistics.
 */
struct allocinfo allocinfo(void);
