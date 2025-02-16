/*
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
#include "cachelab.h"
#include <stdio.h>

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded.
 */
/**
 * (s=5, E=1, b=5) always, which means each block is 32 bytes, and cache has 32 sets
 * Instead of reading from memory again, since stuff in cached in a block, can just read from the
 * cache
 * Each loop iteration stores the blocks
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N]) {
  int i, j, bi, bj, tmp;
  int block_size = 8; // For 32x32 matrix

  for (bi = 0; bi < N; bi += block_size) {
    for (bj = 0; bj < M; bj += block_size) {
      // simply  blocking like that reduces cache misses!
      for (i = bi; i < bi + block_size && i < N; i++) {
        for (j = bj; j < bj + block_size && j < M; j++) {
          // Handle the transpose here
          tmp = A[i][j];
          B[j][i] = tmp;
        }
      }
    }
  }
}

char transpose_block_8_desc[] = "Simple blocking with block size of 8";
void transpose_block_8(int M, int N, int A[N][M], int B[M][N]) {
  int i, j, bi, bj, tmp;
  int block_size = 8; // For 32x32 matrix

  for (bi = 0; bi < N; bi += block_size) {
    for (bj = 0; bj < M; bj += block_size) {
      // simply  blocking like that reduces cache misses!
      //  will access stuff nearby spatially
      for (i = bi; i < bi + block_size && i < N; i++) {
        for (j = bj; j < bj + block_size && j < M; j++) {
          // Handle the transpose here
          tmp = A[i][j];
          B[j][i] = tmp;
        }
      }
    }
  }
}

/**
 * IMPT: Effect of Different Block Sizes
 *
 * For (s=5, E=1, b=5) always, which means each block is 32 bytes, and cache has 32 sets
 * one int is 4 bytes, hence one block can hold 8 integers
 * therefore integer 8 elements apart in the matrix map to the same cache set (rmb we only have one
 * line per set)
 *
 * For a 64x64 matrix:
 * Each row is 256 bytes (64 ints × 4 bytes)
 * This means elements 8 positions apart map to the same cache set
 * With 8x8 blocks, A[i][j] and B[j][i] often map to the same cache set
 *
 * With 4x4 blocks:
 * process fewer elements at a time
 * elements within the block less likely to map to same cache set
 * Larger blocks may increase the chance of cache conflicts as elems further apart may map to the
 * same cache set
 *
 * Too large blocks will cause issues, but block size of 1 theres no point also very slow!
 */
char transpose_block_4_desc[] = "Simple blocking with block size of 4";
void transpose_block_4(int M, int N, int A[N][M], int B[M][N]) {
  int i, j, bi, bj, tmp;
  int block_size = 4; // For 64x64 matrix

  for (bi = 0; bi < N; bi += block_size) {
    for (bj = 0; bj < M; bj += block_size) {
      // simply  blocking like that reduces cache misses!
      //  will access stuff nearby spatially
      for (i = bi; i < bi + block_size && i < N; i++) {
        for (j = bj; j < bj + block_size && j < M; j++) {
          // Handle the transpose here
          tmp = A[i][j];
          B[j][i] = tmp;
        }
      }
    }
  }
}

char transpose_var_desc[] = "blocking and storing temporary variables";
void transpose_var(int M, int N, int A[N][M], int B[M][N]) {
  int i, bi, bj;
  int tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
  // For 32x32, use 8x8 blocking
  for (bi = 0; bi < N; bi += 8) {
    for (bj = 0; bj < M; bj += 8) {
      for (i = bi; i < bi + 8; i++) {
        // Load row from A into temporary variables
        tmp0 = A[i][bj];
        // A[i] here is stored in tmp0, spatial locality
        tmp1 = A[i][bj + 1];
        tmp2 = A[i][bj + 2];
        tmp3 = A[i][bj + 3];
        tmp4 = A[i][bj + 4];
        tmp5 = A[i][bj + 5];
        tmp6 = A[i][bj + 6];
        tmp7 = A[i][bj + 7];

        // Store into B
        B[bj][i] = tmp0;
        B[bj + 1][i] = tmp1;
        B[bj + 2][i] = tmp2;
        B[bj + 3][i] = tmp3;
        B[bj + 4][i] = tmp4;
        B[bj + 5][i] = tmp5;
        B[bj + 6][i] = tmp6;
        B[bj + 7][i] = tmp7;
      }
    }
  }
}

/*
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started.
 */

/*
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N]) {
  int i, j, tmp;

  for (i = 0; i < N; i++) {
    for (j = 0; j < M; j++) {
      tmp = A[i][j];
      B[j][i] = tmp;
    }
  }
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions() {
  /* Register your solution function */
  registerTransFunction(transpose_submit, transpose_submit_desc);

  /* Register any additional transpose functions */
  registerTransFunction(trans, trans_desc);

  registerTransFunction(transpose_block_8, transpose_block_8_desc);
  registerTransFunction(transpose_block_4, transpose_block_4_desc);
  registerTransFunction(transpose_var, transpose_var_desc);
}

/*
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N]) {
  int i, j;

  for (i = 0; i < N; i++) {
    for (j = 0; j < M; ++j) {
      if (A[i][j] != B[j][i]) {
        return 0;
      }
    }
  }
  return 1;
}
