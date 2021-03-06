// The completed version of matrix multiplication
// Including BLAS-dgemm, serial MxM and parallel-block MxM

# include <omp.h>
# include <iostream>
# include <cmath>
// # include <cassert>
# include <chrono>
# include <vector>
#include <utility> 

using namespace std;

// Loading C-BLAS Library
extern "C" {
  #include "stdio.h"
  #include "stdlib.h"
  void dgemm_(char*, char*, int*, int*,int*, double*, double*, int*, double*, int*, double*, double*, int*);
}

double* ref_MxM(double* A, double* B, int N){

  unsigned long size = N * N;
  double* C = (double*)malloc(sizeof(double) * size);

  // Use reference BLAS library to compute the multiplciation
  char option = 'N';
  double alpha = 1.0;
  double beta = 0.0;
  dgemm_(&option, &option, &N, &N, &N, &alpha, A, &N, B, &N, &beta, C, &N);

  free(A);
  free(B);

  // result return
  // for (int i = 0; i < N*N; i++) {
  //   printf("%lf\n", C[i]);
  // }
  return C;
}

// function for serial matrix multiplication 
vector< vector<double> > serial_MxM(vector< vector<double> > A, vector< vector<double> > B, unsigned long int N){
	vector<double> c(N,0);
	vector< vector<double> >  C(N,c);
	for(int i=0; i<N; i++)
		for(int j=0; j<N; j++)
			for(int k=0; k<N; k++){
				C[i][j]+=A[i][k] * B[k][j];
			}
	return C;
}

// function for adding two matices 
vector< vector<double> > matrix_add( vector< vector<double> > A, vector< vector<double> > B){
	unsigned long int N = A.size();
	vector<double> c(N,0);
	vector< vector<double> >  C(N,c);
	for(int i=0; i<N; i++)
		for(int j=0; j<N; j++){
			C[i][j]=A[i][j]+B[i][j];
		}
	return C;
}

// function for parallel block matrix multiplication 
vector< vector<double> > block_MxM(vector< vector<double> > A, vector< vector<double> > B, unsigned long int N){
	vector<double> C_temp(N,0);
	vector< vector<double> >  C(N,C_temp);


	// determine the block size, based on N and the number of threads
	int thread_num=1; // assume single thread first  
	#pragma omp parallel shared(thread_num) 
	{
	  #pragma omp master
	  {
          #if defined(_OPENMP)
	      thread_num = omp_get_num_threads();
          #endif 
	      cout << "Number of threads = " << thread_num << endl;
	  }
	}

	// block_num is the number of blocks along 1 dimension 
	int block_num_min = sqrt(thread_num); // one process handle one block
	int bsize_max = N/block_num_min;  // this might be too big for cache

    int bsize = 256; // choose a size that fits the cache. 
    // bsize must be N's factor! And it should be less than bsize_max otherwise we will have idle threads

    if (bsize > bsize_max){
    cout << "warning: bsize is too large so that there will be idle threads" << endl;
    cout << "you should decrease bsize, or increase problem size, or decrease OMP_NUM_THREADS" << endl << endl;
    }

    int block_num = N/bsize; // one thread might handle many blocks 

    cout << "largest possible bsize (might be too large for cache) = " << bsize_max << endl;
	cout << "block size we choose = " << bsize << endl;
	cout << "Number of blocks along one dimension= " << block_num << endl;
	cout << "Each thread handles " << (block_num*block_num)/thread_num << " blocks" << endl;

	//temp storage
	vector<double> temp(bsize,0);
	vector< vector<double> >  a(bsize,temp);
	vector< vector<double> >  b(bsize,temp);
	vector< vector<double> >  c(bsize,temp);


	/* block matrix multiplication 
	A,B,C (NxN) are the original matrices shared by all cpus
	a,b,c (qxq) are block matrices on individual cpus
	idx unrolls the 2D index (i,j) to 1D
	*/
	int i,j,k,idx,p,q;

    #pragma omp parallel for default(none) schedule(static) \
	    shared(A,B,C,bsize,block_num) private(p,q,i,j,k,idx) firstprivate(a,b,c)
	for (idx=0; idx<block_num*block_num; idx++){

	    // i,j = convert_1D_to_2D(idx)`
		i = idx/block_num;
		j = idx%block_num;

		for(k=0; k<block_num ; k++){
			// vector< vector<double> > a = A[i*bsize:i*bsize+bsize,k*bsize:k*bsize+bsize] 
			// vector< vector<double> > b = B[k*bsize:k*bsize+bsize,j*bsize:j*bsize+bsize]
			for (p=0; p<bsize; p++)
				for (q=0; q<bsize; q++){
					a[p][q] = A[i*bsize+p][k*bsize+q];
					b[p][q] = B[k*bsize+p][j*bsize+q];
			}
		
			c  = matrix_add(serial_MxM(a,b,bsize),c);
			
		}
		
	    // write back to global matrix
	    // no OpenMP reduction needed 
	    // C[i*bsize:i*bsize+bsize,j*bsize:j*bsize+bsize] = c
		for (p=0; p<bsize; p++)
			for (q=0; q<bsize; q++){
				C[i*bsize+p][j*bsize+q] = c[p][q];
		}
	} //-- End of parallel region --

    return C;
}


// main program. Compare the performance of dgemm, serial MxM and parallel-block MxM
int main(int argc, char** argv){

	unsigned long int N=pow(2,10); //problem size

	// set initial value for A and B
	vector<double> a(N,1);
	vector< vector<double> >  A(N,a);

	vector<double> b(N,1);
	vector< vector<double> >  B(N,b);

    auto t1=chrono::high_resolution_clock::now();
    auto t2=chrono::high_resolution_clock::now();
    chrono::duration<double>  time_span;

    if (argc == 1){ //if arguments provided, skip serial and dgemm for scalability tests
    
	// dgemm in the level-3 BLAS for reference 
	// The library (OpenBLAS) only provides C interface. On its github wiki page, it says that the "dgemm_" 
	// funciton is called from Fortran interface, but I believe it's optimized since it turns out to be super 
	// fast.
	cout << "dgemm:" << endl;
	unsigned long size = N * N;
	double* AA = (double*)malloc(sizeof(double) * size);
	double* BB = (double*)malloc(sizeof(double) * size);
	for (int i = 0; i < N*N; i++) {
		AA[i] = A[i / N][i % N];
		BB[i] = B[i / N][i % N];
	}
	t1 = chrono::high_resolution_clock::now();
	double* C = ref_MxM(AA, BB, N);
	t2 = chrono::high_resolution_clock::now();
	time_span = t2 - t1;
	cout << "first element = " << C[0] << endl; //should equal to N
	cout << "time use = " << time_span.count()*1000 << "ms"<< endl << endl ;

	// run naive MxM (3-level nested loops) 
    cout << "serial version:" << endl;
    t1 = chrono::high_resolution_clock::now();
    vector< vector<double> >  C1 = serial_MxM(A,B,N); 
    t2 = chrono::high_resolution_clock::now();
    time_span = t2 - t1;
    cout << "first element = " << C1[0][0] << endl; //should equal to N
    cout << "time use = " << time_span.count()*1000 << "ms"<< endl << endl;
    
    }

	// run parallel block MxM 
    cout << "block version:" << endl;
    t1 = chrono::high_resolution_clock::now();
    vector< vector<double> >  C2 = block_MxM(A,B,N); 
    t2 = chrono::high_resolution_clock::now();
    time_span = t2 - t1;
    cout << "first element = " << C2[0][0] << endl; //should equal to N
    cout << "time use = " << time_span.count()*1000 << "ms"<< endl;

	return 0;

}
