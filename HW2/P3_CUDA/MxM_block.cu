# include "../shared/timing.h"
# include <stdlib.h>
# include <stdio.h>
# include <math.h>
# define BSIZE 16

// device code for block MxM
// Omit the d_ prefix because all variables are on device
__global__ void MxM_block(double* A, double* B, double* C, const int N) {

    // store sub-matrix in the shared memory
    // They should be in the L1 cache of each streaming processor
    __shared__ double A_tile[BSIZE][BSIZE];
    __shared__ double B_tile[BSIZE][BSIZE];
    __shared__ double C_tile[BSIZE][BSIZE];
   
    // each thread computes one element of the block sub-matrix
    // initialize the element to 0
    C_tile[threadIdx.y][threadIdx.x] = 0.0;

    // pseudo code:
    // C[i1:i2][j1:j2] += A[i1:i2][k1:k2] * B[k1:k2][j1:j2]
    // For each block, i,j is known:
    // (i ranges from i1 to i2, j ranges from j1 to j2)
    int i = blockIdx.y * blockDim.y + threadIdx.y;
    int j = blockIdx.x * blockDim.x + threadIdx.x;
    // Only k is changing, so we loop over k:
    // each iteration (k1,k2) -> (k1+BSIZE,k2+BSIZE)
    for (int tileIdx = 0; tileIdx < N/BSIZE; tileIdx++){

        // Load the matrices from global memory to shared memory
        // each thread loads one element
		// note that we use 1D index for global matrix but 2D index for a tile
        int k_A = tileIdx*BSIZE+threadIdx.x; // k is the row index for A 
        int k_B = tileIdx*BSIZE+threadIdx.y; // k is the column index for B
        A_tile[threadIdx.y][threadIdx.x] = A[i*N + k_A];
        B_tile[threadIdx.y][threadIdx.x] = B[k_B*N + j];

        // Synchronize to make sure the entire tile is loaded
        __syncthreads();

        // Multiply the two matrices together;
        // each thread computes one element of the block sub-matrix
        for (int k = 0; k < BSIZE; k++) {
            C_tile[threadIdx.y][threadIdx.x] += 
            A_tile[threadIdx.y][k] * B_tile[k][threadIdx.x];
        }

        // Synchronize to make sure that the preceding
        // computation is done before loading two new
        // sub-matrices of A and B in the next iteration
        __syncthreads();
    }

	// write back to global memory
    // each thread writes one element
    C[i*N + j] = C_tile[threadIdx.y][threadIdx.x];

    // Final note:
    // It seems that using C_temp (a scalar)
    // instead of C_tile[threadIdx.y, threadIdx.x]
    // is more memory-efficient.
    // because C_temp can be stored in the register of each thread.
    // So we can save more shared memory for A_tile and B_tile
    // But to follow the homework instruction we still allocate
    // shared memory for C_tile 

}


// host code is almost the same as the naive one
int main() {
    // set up problem size
    int N = pow(2, 12);
    int size = N * N;

    // malloc host memory and initialize data
    double* h_A = (double*)malloc(sizeof(double) * size);
    double* h_B = (double*)malloc(sizeof(double) * size);
    double* h_C = (double*)malloc(sizeof(double) * size);
    for (int i = 0; i < size; i++) {
        h_A[i] = 1.0;
        h_B[i] = 1.0;
    }

	// malloc device global memory and transfer data from host to device
    double *d_A, *d_B, *d_C;
    cudaMalloc((void **)&d_A, size * sizeof(double));
    cudaMalloc((void **)&d_B, size * sizeof(double));
    cudaMalloc((void **)&d_C, size * sizeof(double));

    cudaMemcpy(d_A, h_A, size*sizeof(double), cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, h_B, size*sizeof(double), cudaMemcpyHostToDevice);

	// Invoke kernel at host side
	// One thread-block calculates on block of matrix C
    // Inside the block, one thread handles one element
    dim3 dimBlock(BSIZE, BSIZE);
    dim3 dimGrid(N/dimBlock.x, N/dimBlock.y);

    // warm-up. The first call is significantly slower than the following calls.
    MxM_block<<<dimGrid, dimBlock>>>(d_A, d_B, d_C, N);
	// start timing and execute the kernel function
    double iStart, iElaps;
    iStart = seconds();
    MxM_block<<<dimGrid, dimBlock>>>(d_A, d_B, d_C, N);
    iElaps = seconds() - iStart;

    // copy kernel result back to host side
    cudaMemcpy(h_C, d_C, size*sizeof(double), cudaMemcpyDeviceToHost);

	// print the results
    printf("First element =  %f \n", h_C[0]);
    printf("Time elapse = %f ms \n", iElaps*1000.0);
	
    // clean up
    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);

    return(0);
   
}
