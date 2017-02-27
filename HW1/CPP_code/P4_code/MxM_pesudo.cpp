// This is the pesudo code for Problem 4 (parallel block MxM)
// NOT a valid c++ code!! 

// load library

# include <omp.h>
# include <iostream>
# include <cmath>
# include <cassert>
# include <chrono>

// allocate NxN matrix
A[N][N]
B[N][N]
C[N][N]

// set initial value for A and B
TBD

// determine the block size, based on N and the number of threads
int p // how many threads 
#pragma omp parallel shared(p) 
{
  #pragma omp master
  {
      p = omp_get_num_threads();
      cout << "Number of threads = " << nthreads << endl;
  }
}


// we have qxq blocks
// make sure that p is a square number
q = sqrt(p)

// block size must be an integer 
bsize = N/q 

/* block matrix multiplication 
A,B,C (NxN) are the original matrices shared by all cpus
a,b,c (qxq) are block matrices on individual cpus
idx unrolls the 2D index (i,j) to 1D
*/ 
#pragma omp parallel for default(none) schedule(static) \
    shared(N,A,B,C) private(a,b,c,i,j,k,idx)
for (idx=0; i<q*q; idx++){

    i,j = convert_1D_to_2D(idx)`


    // calculate local block c
    for k in range(0:q):
        // read part of global matrix into smaller local matrix
        // you need two more loops to transfer matrix
        a  = A[i:i+bsize,k:k+bsize] 
        b  = B[k:k+bsize,j:j+bsize] 
        c += serial_MxM(a,b)

    // write back to global matrix
    // no OpenMP reduction needed 
    C[i:i+bsize,j:j+bsize] = c
} //-- End of parallel region --


return C
