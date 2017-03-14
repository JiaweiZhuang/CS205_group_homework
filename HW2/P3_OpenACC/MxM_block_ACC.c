# include "../shared/timing.h"
# include <stdlib.h>
# include <stdio.h>
# include <math.h>
# define BSIZE 16 //block size must be larger than 16 (otherwise a weird bug occurs)

// for dynamically allocating 2D array in pure-C environment
double** Make2DDoubleArray(int arraySizeX, int arraySizeY) {
    double** theArray;
    theArray = (double**) malloc(arraySizeX*sizeof(double*));
    for (int i = 0; i < arraySizeX; i++)
        theArray[i] = (double*) malloc(arraySizeY*sizeof(double));
    return theArray;
} 

int main() {

    int N=pow(2,10); //problem size
    int block_num = N/BSIZE; // number of block along one dimension
    int i,j,k; // interative

    // Initialize arrays
    // Note:
    // 1) For transfering data from host to device, it's more 
    // convenient to use pure-C arrays than C++ objects.
    // 2) Can't use A[N][N] even if N is a pure number. This will
    // cause stack overflow for large N.
    // 3) OpenACC uses the same variable for both host and devide.
    double** A = Make2DDoubleArray(N, N);
    double** B = Make2DDoubleArray(N, N);
    double** C = Make2DDoubleArray(N, N);

    // Initial condition
    for (i=0; i<N; i++){
    for (j=0; j<N; j++){
        A[i][j]=1.0;
        B[i][j]=1.0;
        C[i][j]=0.0;
    }
    }

    // A tile of the global matrix.
    // Although they are only needed on the device, 
    // we still declare them on the host for consistency.
    // We must use static allocation for privatizing a,b,c in the parallel region
    // The issue is recorded at http://www.pgroup.com/userforum/viewtopic.php?t=5542
    double a[BSIZE][BSIZE],b[BSIZE][BSIZE],c[BSIZE][BSIZE];

    // Initial condition
    for (i=0; i<BSIZE; i++){
    for (j=0; j<BSIZE; j++){
        a[i][j]=0.0;
        b[i][j]=0.0;
        c[i][j]=0.0;
    }
    }

    // timing MxM
    double iStart, iElaps;
    iStart = seconds();

    // Although OpenACC supports automatic parallelization over nested loops 
    // (the first "acc loop" is for gangs and the second "acc loop" is for vector elements),
    // for each specific loop clause (e.g. "acc loop gang" or "acc loop vector"), 
    // it seems that only the outer loop can be parallelized. In other words, the second
    // "loop gang" clause will be regarded as sequential.
    // However, the collapse clause can let one loop clause affect multiple
    // adjacent loops (similar to CUDA's threadIdx.x and threadIdx.y).
    // This avoids unrolling 2D index to 1D as we did in HW1 with OpenMP

    #pragma acc data copyin(A[:N][:N],B[:N][:N]) copy(C[:N][:N]) 
    {

    // p, q, r  are automatically set private. 
    // Unlike OpenMP, here you can't set private before declaring a variable.
    #pragma acc parallel private(a,b) firstprivate(c) num_gangs(64) vector_length(128)
    {

        // assign blocks to gangs
        // collapse 2-level nested loops to increase parallelism
        #pragma acc loop independent gang collapse(2)
        for (i=0; i<block_num; i++){
        for (j=0; j<block_num; j++){

            // One interation accumalates a*b to c
            for(k=0; k<block_num; k++){

                // load part of the global matrix into tile
                // parallelize the data loading over threads
                // collapse 2-level nested loops to increase parallelism
                #pragma acc loop independent vector collapse(2)
                for (int p=0; p<BSIZE; p++){
                for (int q=0; q<BSIZE; q++){
                    a[p][q] = A[i*BSIZE+p][k*BSIZE+q];
                    b[p][q] = B[k*BSIZE+p][j*BSIZE+q];
                }
                }

                // After the tiles are loaded, perform a*b and accumulates to c
                // each thread computes on element of c
                #pragma acc loop independent vector collapse(2)
                for (int p=0; p<BSIZE; p++){
                for (int q=0; q<BSIZE; q++){

                    for (int r=0; r<BSIZE; r++) {
                        c[p][q] += a[p][r]*b[r][q];
                    }
                }
                }
            }

            // After tile c is finished, write back to global matrix
            // each thread writes on element of c
            #pragma acc loop independent vector collapse(2)
            for (int p=0; p<BSIZE; p++){
            for (int q=0; q<BSIZE; q++){
                C[i*BSIZE+p][j*BSIZE+q] = c[p][q];
            }
            }


        }
        }

    }
    }

    iElaps = seconds() - iStart;

    // print the results
    printf("First element =  %f \n", C[0][0]);
    printf("Time elapse = %f ms \n", iElaps*1000.0);

    return 0;

}



