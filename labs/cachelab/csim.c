#include "cachelab.h"
#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// global variables
int hits;
int misses;
int evictions;

// config variables
typedef struct cache_config {
  int s;           // Number of set index bits
  int E;           // Associativity (lines per set)
  int b;           // Number of block bits
  char* tracefile; // Path to the tracefile
  int verbose;     // Verbose mode flag
} cache_config_t;

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
  // LRU implementation
  //  this tracks how long a line was last accessed
  //  loop increased the time for all lines
  //  once its accessed, reset the time to 0
  //  when evicting, choose the one with the highest time
  unsigned int time;
  // use cache_line_t here as a placeholder for the typedef of the cache_line struct]
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
    new_set->lines[i].time = 0;
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

// Simulate cache behaviour by doing appropriate load and stores
void cache_simulate(cache_t* cache, cache_config_t* config, char operation,
                    unsigned long long address) {

  // if operation is instruction (I), ignore completely
  // use single quotes for char
  if (operation == 'I') {
    return;
  }

  // Extract set index and tag from address

  // creates a mask of ones with s bits all set to one. if s is 3, then mask is 0b111
  unsigned int index_mask = (1 << config->s) - 1;
  // shift to right the block offset bits. then take the s bits
  unsigned int set_index = (address >> config->b) & index_mask;
  unsigned long long tag = address >> (config->s + config->b);
  cache_set_t* set = cache->sets[set_index];

  if (config->verbose) {
    printf(" | requesting set %d, tag %llx | ", set_index, tag);
  }
  // increment time for all
  for (int i = 0; i < cache->E; i++) {
    // add to time first; for all lines
    set->lines[i].time++;
  }

  // Search for hit

  // flag to indicate if hit
  int hit = 0;
  for (int i = 0; i < cache->E; i++) {
    // search for the tag
    if (set->lines[i].valid && set->lines[i].tag == tag) {
      // successfully hit
      hit = 1;
      // reset time
      set->lines[i].time = 0;
      // print if verbose
      if (config->verbose) {
        printf(" hit ");
      }
      hits++;
      // stop searching
      break;
    }
  }

  // Handle miss
  if (!hit) {
    // print if verbose
    if (config->verbose) {
      printf(" miss ");
    }
    misses++;
    int placed = 0;
    for (int i = 0; i < cache->E; i++) {
      // search for an invalid line (empty) and place it there
      if (!set->lines[i].valid) {
        set->lines[i].valid = 1;
        set->lines[i].tag = tag;
        // reset time for placing at empty slot
        set->lines[i].time = 0;
        placed = 1;
        break;
      }
    }

    // If no empty slot, evict
    if (!placed) {
      // print if verbose
      if (config->verbose) {
        printf(" evict ");
      }
      evictions++;

      // do LRU and replace the highest time; oldest line
      int max_idx = -1;
      int max_time = 0;
      for (int i = 0; i < cache->E; i++) {
        if (set->lines[i].time > max_time) {
          max_time = set->lines[i].time;
          max_idx = i;
        }
      }
      if (max_idx != -1) {
        // evict accordingly by replacing tag
        set->lines[max_idx].tag = tag;
        // reset time
        set->lines[max_idx].time = 0;
      } else {
        perror("Error, LRU eviction did not work");
      }
    }
  };
  // Modify operation ('M' means load + store, so always an extra hit)
  // else its just the same as before
  if (operation == 'M') {
    // print if verbose
    if (config->verbose) {
      printf(" hit_m ");
    }
    hits++;
  }
}

void print_usage(char* prog_name) {
  printf("Usage: %s [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n", prog_name);
  printf("  -h  : Print this help message\n");
  printf("  -v  : Enable verbose output\n");
  printf("  -s <s> : Number of set index bits (S = 2^s is the number of sets)\n");
  printf("  -E <E> : Number of lines per set (associativity)\n");
  printf("  -b <b> : Number of block bits (B = 2^b is the block size)\n");
  printf("  -t <tracefile> : Path to the valgrind trace file\n");
}

// modify the config object accordingly, returns nothing
void parse_arguments(int argc, char** argv, cache_config_t* config) {
  int opt;
  while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1) {
    switch (opt) {
    case 'h':
      print_usage(argv[0]);
      exit(0);
    case 'v':
      config->verbose = 1;
      break;
    case 's':
      config->s = atoi(optarg);
      break;
    case 'E':
      config->E = atoi(optarg);
      break;
    case 'b':
      config->b = atoi(optarg);
      break;
    case 't':
      config->tracefile = optarg;
      break;
    default:
      print_usage(argv[0]);
      exit(1);
    }
  }

  // Check if required arguments are provided
  if (config->s == 0 || config->E == 0 || config->b == 0 || config->tracefile == NULL) {
    printf("Error: Missing required arguments\n");
    print_usage(argv[0]);
    exit(1);
  }
}

void parse_trace_file(cache_t* cache, cache_config_t* config) {
  FILE* file = fopen(config->tracefile, "r");
  if (!file) {
    perror("Error opening trace file");
    exit(EXIT_FAILURE);
  }

  char line[256];
  while (fgets(line, sizeof(line), file)) {
    char operation;
    unsigned long long address;
    int size;
    //%c means read a single character
    // leading whitespace skips all white spaces
    //%lx reads a hex number and stores it as unsigned long
    // ,%d ensures , must appear and %d reads an integer
    if (sscanf(line, " %c %llx,%d", &operation, &address, &size) == 3) {
      printf("\nOperation: %c, Address: 0x%llx, Value: %d", operation, address, size);
      // do the cache simulation
      cache_simulate(cache, config, operation, address);
    }
  };
  fclose(file);
}

int main(int argc, char** argv) {
  cache_config_t config = {0, 0, 0, NULL, 0}; // Initialize with defaults
  parse_arguments(argc, argv, &config);

  // Print parsed arguments (for debugging)
  if (config.verbose) {
    printf("Verbose mode enabled\n");
    printf("s = %d, E = %d, b = %d, tracefile = %s\n", config.s, config.E, config.b,
           config.tracefile);
  }
  // create the cache
  cache_t* full_cache = cache_init(config.s, config.E, config.b);
  // do the full simulation
  parse_trace_file(full_cache, &config);

  // print out the final results
  printf("\n");
  printSummary(hits, misses, evictions);

  return 0;
};