#include "cachelab.h"
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>

// Global variables
int s, E, b;     // Cache parameters
char* tracefile; // Trace file name
int verbose = 0; // Verbose flag

/** Chapter 6 Part A: Cache Simulator
 *
 * Takes a valgrind memory trace as input, simulate the hit/miss behaviour of a
 cache memory on this trace, and output the number of hits, misses and
 evictions.
 * Uses LRU replacenent policy when choosing which cache line to evict.
 *
 * Usage: ./csim-ref [-hv] -s <s> -E <E> -b <b> -t <tracefile>
    • -h: Optional help flag that prints usage info
    • -v: Optional verbose flag that displays trace info
    • -s <s>: Number of set index bits (S = 2s is the number of sets)
    • -E <E>: Associativity (number of lines per set)
    • -b <b>: Number of block bits (B = 2b is the block size)
    • -t <tracefile>: Name of the valgrind trace to replay

 * in valgrind, each data load (L) or data store (S) does one operation, so at
 most one cache miss the data modify operation (M) is treated as a load followed
 by a store to the same address, hence (M) operation can result in 2 cache hits,
 or miss+ possible eviction + hit
 *
 */

/**
 * Simulates a line in a cache
 */
typedef struct cache_line {
  // 0 for invalid, 1 for valid
  int valid;
  // tag bits are always 64bit here
  // unsigned long long guarantees 64 bits on all platforms
  unsigned long long tag;
  // use cache_line_t here as a placeholder for the typedef of the cache_line struct
} cache_line_t;

/**
 * Simulates a set of lines in a cache
 */
typedef struct cache_set {
  // array of lines
  cache_line_t* lines;
} cache_set_t;

// init method
cache_set_t* cache_set_init(int E) {
  // First allocate the set structure itself
  // This is literally allocating memory for a single pointer!
  /** What happens if we used automatic allocation (on the stack)
   *
   * cache_set_t new_set;  // on stack
    return &new_set;      // DANGEROUS! This memory will be invalid after function returns
   */
  cache_set_t* new_set = malloc(sizeof(cache_set_t));
  // error handling
  if (new_set == NULL) {
    printf("Error! Set memory not allocated");
    return NULL;
  }

  // Then allocate array of E lines; one contiguous block of memory
  // requires only only one malloc call per set
  new_set->lines = malloc(E * sizeof(cache_line_t));
  if (new_set->lines == NULL) {
    // Handle malloc failure
    free(new_set);
    printf("Error! Line memory not allocated");
    return NULL;
  }
  // Initialize each line directly, instead of having separate constructor for line
  for (int i = 0; i < E; i++) {
    new_set->lines[i].valid = 0;
    new_set->lines[i].tag = 0;
  }
  return new_set;
}

/**
 * Simulates the full cache
 */
typedef struct cache {
  // double pointer
  // pointer to an array of sets, each elem itself is a set pointer
  cache_set_t** sets;
  int S; // Number of sets (2^s)
  int E; // Number of lines per set
  int B; // Block size (2^b)
} cache_t;

// init the full cache
cache_t* cache_init(int s, int E, int b) {
  cache_t* cache = malloc(sizeof(cache_t));
  if (cache == NULL)
    return NULL;

  // Calculate S and B
  cache->S = 1 << s; // 2^s sets
  cache->E = E;
  cache->B = 1 << b; // 2^b bytes per block

  // Allocate array of sets as an array of pointers
  cache->sets = malloc(cache->S * sizeof(cache_set_t*));
  if (cache->sets == NULL) {
    free(cache);
    return NULL;
  }

  // Initialize each set
  for (int i = 0; i < cache->S; i++) {
    // moves i step forward from the pointer address
    // * dereferences the pointer
    cache->sets[i] = cache_set_init(cache->E);
  }
  return cache;
}

// helper function to print usage
void print_usage(char* prog_name) {
  // program name is the %s
  printf("Usage: %s [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n", prog_name);
  printf("  -h  : Print this help message\n");
  printf("  -v  : Enable verbose output\n");
  printf("  -s <s> : Number of set index bits (S = 2^s is the number of sets)\n");
  printf("  -E <E> : Number of lines per set (associativity)\n");
  printf("  -b <b> : Number of block bits (B = 2^b is the block size)\n");
  printf("  -t <tracefile> : Path to the valgrind trace file\n");
}

// helper function to parse arguments
void parse_arguments(int argc, char** argv) {
  int opt;
  while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1) {
    switch (opt) {
    case 'h':
      print_usage(argv[0]);
      // exit successfully
      exit(0);
    case 'v':
      verbose = 1;
      break;
    case 's':
      s = atoi(optarg);
      break;
    case 'E':
      E = atoi(optarg);
      break;
    case 'b':
      b = atoi(optarg);
      break;
    case 't':
      tracefile = optarg;
      break;
    default:
      print_usage(argv[0]);
      // exit unsuccessfully
      exit(1);
    }
  }

  // Check if required arguments are provided
  if (s == 0 || E == 0 || b == 0 || tracefile == NULL) {
    printf("Error: Missing required arguments\n");
    print_usage(argv[0]);
    exit(1);
  }
}

main() {
  printSummary(0, 0, 0);
  return 0;
}
