# include "../shared/timing.h"
# include <stdlib.h>
# include <stdio.h>
# include <math.h>

// device code for naive MxM
// Omit the d_ prefix because all variables are on device
// We don't make use of shared memory (for each block) in this naive version
__global__ void MxM_naive(double* A, double* B, double* C, const int N) {

    int i = blockIdx.y * blockDim.y + threadIdx.y;    // Row i of matrix C
    int j = blockIdx.x * blockDim.x + threadIdx.x;    // Column j of matrix C

    double C_temp = 0;
    for (int k=0; k<N; k++) {
        // use 1D indexing
        C_temp += A[i*N + k] * B[k*N + j];
    }

	// write back to global memory
	// no synchronization needed here because one thread handles one element
    C[i*N + j] = C_temp;

}


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
	// One thread calculates one element of matrix C.
	// There is limit (512 or 1024) on the number of threads in a block,
 	// so we use many blocks to generate N*N threads in total.
	// The 'block' here has nothing to do with the block matrix multiplication,
	// because block ids are immediately converted back to matrix indices.
    dim3 dimBlock(16, 16);
    dim3 dimGrid(N/dimBlock.x, N/dimBlock.y);

    // warm-up. The first call is significantly slower than the following calls.
    MxM_naive<<<dimGrid, dimBlock>>>(d_A, d_B, d_C, N);
	// start timing and execute the kernel function
    double iStart, iElaps;
    iStart = seconds();
    MxM_naive<<<dimGrid, dimBlock>>>(d_A, d_B, d_C, N);
    iElaps = seconds() - iStart;

    // copy kernel result back to host side
    cudaMemcpy(h_C, d_C, size*sizeof(double), cudaMemcpyDeviceToHost);

	// print the results
    printf("First element =  %f \n", h_A[0]);
    printf("First element =  %f \n", h_C[0]);
    printf("Time elapse = %f ms \n", iElaps*1000.0);
	
    // clean up
    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);

    return(0);
   
}
