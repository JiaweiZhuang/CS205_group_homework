# include <stdlib.h>
# include <stdio.h>
# include <time.h>
# include <math.h>
# define BSIZE 32

// Forward declaration of the device multiplication function
__global__ void blockMxM(double*, double*, double*, int);
// Host multiplication function
double* cuda_MxM(double* A, double* B, int N) {
    double* AA;
    double* BB;
    double* CC;
    int size = N * N;

    // Put into device
    cudaMalloc((void **)&AA, size * sizeof(double));
    cudaMemcpy(AA, A, size * sizeof(double), cudaMemcpyHostToDevice);
    cudaMalloc((void **)&BB, size * sizeof(double));
    cudaMemcpy(BB, B, size * sizeof(double), cudaMemcpyHostToDevice);
    cudaMalloc((void **)&CC, size * sizeof(double));

    // Each device is responsible for one block
    dim3 dimBlock(BSIZE, BSIZE);
    dim3 dimGrid(N / dimBlock.x, N / dimBlock.y);

    blockMxM<<<dimGrid, dimBlock>>>(AA, BB, CC, N);

    // Read result from the device
    double* C = (double*)malloc(size * sizeof(double));
    
    cudaMemcpy(C, CC, size * sizeof(double), cudaMemcpyDeviceToHost);

    cudaFree(AA);
    cudaFree(BB);
    // cudaFree(CC);
    return C;
}

// Device code for mutiplication on each block
__global__ void blockMxM(double* A, double* B, double* C, int N) {

    int stripeA = (blockIdx.y * BSIZE) * N;
    int stripeB = blockIdx.x * BSIZE;
    int blockxy = threadIdx.y * N + threadIdx.x;
    // printf("Hello from block %d, %d, thread %d, %d, sA: %d, sB: %d \n", blockIdx.x, blockIdx.y, threadIdx.x, threadIdx.y, stripeA, stripeB);
    
    double Csub = 0;
    
    for (int i = 0; i < (N-1) / BSIZE + 1; i++) {
        // Shared memory 
        
        __shared__ double AA[BSIZE][BSIZE];
        __shared__ double BB[BSIZE][BSIZE];

        int j = stripeA + i * BSIZE + blockxy;
        int k = stripeB + i * (BSIZE * N) + blockxy;

        AA[threadIdx.y][threadIdx.x] = A[j];
        BB[threadIdx.y][threadIdx.x] = B[k];
        __syncthreads();
        

        for (int ii = 0; ii < BSIZE; ii++) {
            Csub += AA[threadIdx.y][ii] * BB[ii][threadIdx.x];
        }
        __syncthreads();
    }
    // Write the block sub-matrix to global memory;
    // each thread writes one element
    int l = stripeA + stripeB + blockxy;
    C[l] = Csub;
}

int main() {
    int N = pow(2, 10);
    int size = N * N;
    double* A = (double*)malloc(sizeof(double) * size);
    double* B = (double*)malloc(sizeof(double) * size);
    for (int i = 0; i < size; i++) {
        A[i] = 1.0;
        B[i] = 1.0;
    }
    clock_t start = clock();
    double* C = cuda_MxM(A, B, N);
    clock_t end = clock();
    int msec = (end - start) * 1000 / CLOCKS_PER_SEC;
    printf("%d milliseconds \n", msec);
    // for (int i = 0; i < size; i++) {
    //     printf("%lf\n", C[i]);
    // } 
}