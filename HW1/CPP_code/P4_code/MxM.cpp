# include <omp.h>
# include <iostream>
# include <cmath>
// # include <cassert>
# include <chrono>
# include <vector>
#include <utility> 

using namespace std;

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

	// we have qxq blocks
	// make sure that p is a square number
	int block_num = sqrt(thread_num);
	cout << "Number of blocks along one dimension= " << block_num << endl;

	// block size must be an integer 
	int bsize = N/block_num;
	cout << "block size = " << bsize << endl;

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
	    shared(A,B,C,bsize,block_num,thread_num) private(p,q,i,j,k,idx) firstprivate(a,b,c)
	for (idx=0; idx<thread_num; idx++){

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


int main(){

	unsigned long int N=pow(2,10); //problem size

	// set initial value for A and B
	vector<double> a(N,1);
	vector< vector<double> >  A(N,a);


	vector<double> b(N,1);
	vector< vector<double> >  B(N,b);


	// dgemm in the level-3 BLAS for reference 
	// The library (OpenBLAS) only provides C interface, and does some low-level vectorization, so it turns out to be 
	// very fast.
	cout << "degmm:" << endl;
	unsigned long size = N * N;
	double* AA = (double*)malloc(sizeof(double) * size);
	double* BB = (double*)malloc(sizeof(double) * size);
	for (int i = 0; i < N*N; i++) {
		AA[i] = A[i / N][i % N];
		BB[i] = B[i / N][i % N];
	}
	auto t1 = chrono::high_resolution_clock::now();
	double* C = ref_MxM(AA, BB, N);
	auto t2 = chrono::high_resolution_clock::now();
	chrono::duration<double>  time_span = t2 - t1;
	cout << "first element = " << C[0] << endl; //should equal to N
	cout << "time use = " << time_span.count()*1000 << "ms"<< endl;



	
	// cout << "serial version:" << endl;
	// auto t1 = chrono::high_resolution_clock::now();
	// vector< vector<double> >  C1 = serial_MxM(A,B,N); 
	// auto t2 = chrono::high_resolution_clock::now();
	// chrono::duration<double>  time_span = t2 - t1;
	// cout << "first element = " << C1[0][0] << endl; //should equal to N
	// cout << "time use = " << time_span.count()*1000 << "ms"<< endl;
	

	// cout << "block version:" << endl;
	// auto t1 = chrono::high_resolution_clock::now();
	// vector< vector<double> >  C2 = block_MxM(A,B,N); 
	// auto t2 = chrono::high_resolution_clock::now();
	// chrono::duration<double> time_span = t2 - t1;
	// cout << "first element = " << C2[0][0] << endl; //should equal to N
	// cout << "time use = " << time_span.count()*1000 << "ms"<< endl;

	return 0;

}
