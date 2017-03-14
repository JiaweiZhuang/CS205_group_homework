# include "../shared/timing.h"
# include <stdlib.h>
# include <stdio.h>
# include <math.h>

# include <cuda.h>
# include "cublas_v2.h"

int main() {
    // set up problem size
    int N = pow(2, 10);
    int size = N * N;

    // for CUBLAS
    double alpha=1.0, beta=0.0;
    cublasHandle_t handle = 0;

    // malloc host memory and initialize data
    double* h_A = (double*)malloc(sizeof(double) * size);
    double* h_B = (double*)malloc(sizeof(double) * size);
    double* h_C = (double*)malloc(sizeof(double) * size);
    for (int i = 0; i < size; i++) {
        h_A[i] = 1.0;
        h_B[i] = 1.0;
    }

    // start timing
    double iStart, iElaps;
    iStart = seconds();

    // Create the cuBLAS handle
    cublasCreate(&handle);

    // malloc device global memory 
    double *d_A, *d_B, *d_C;
    cudaMalloc((void **)&d_A, size * sizeof(double));
    cudaMalloc((void **)&d_B, size * sizeof(double));
    cudaMalloc((void **)&d_C, size * sizeof(double));

	// Transfer inputs to the device
	cublasSetMatrix(N, N, sizeof(double), h_A, N, d_A, N);
	cublasSetMatrix(N, N, sizeof(double), h_B, N, d_B, N);

    // Execute the matrix-matrix multiplication
    // C = alpha*A*B + beta*C
    cublasDgemm(handle, CUBLAS_OP_N, CUBLAS_OP_N, N, N, N, 
                &alpha, d_A, N, d_B, N, 
                &beta,  d_C, N); 

	// Retrieve the output matrix from the device	
	cublasGetMatrix(N, N, sizeof(double), d_C, N, h_C, N);

    // end of timing
    iElaps = seconds() - iStart;

    // print the results
    printf("First element =  %f \n", h_C[0]);
    printf("Time elapse = %f ms \n", iElaps*1000.0);

    // clean up
    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);

    return 0;

}
