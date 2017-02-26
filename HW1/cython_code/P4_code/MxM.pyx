from cython cimport boundscheck, wraparound
from cython.parallel cimport prange
import numpy as np

# serial Matrix multiplication
@boundscheck(False) 
@wraparound(False)
cpdef double[:,:] serial_MxM(double[:,:] A,double[:,:] B,int N):
    cdef:
        int i,j,k
        double[:,:] C

    C = np.zeros([N,N],dtype=np.double)
    for i in range(N):
        for j in range(N):
            for k in range(N):
                C[i,j]+=A[i,k]*B[k,j]
    return C

'''
This block-matrix-multiplication runs, but it is not completed yet, in that:

1) It is not fully cythonized, so GIL cannot be released and OpenMP is 
not allowed. If the matrix "C" is statically typed, then it becomes a 
memoryview type, which is like a C-array. You cannot directly add up 
two C arrays -- two additional loops are needed. 

2) prange only allow one-level loop. To divide the matrix into blocks,
and give them to different threads, we might need to unroll the 2D loop
to 1D (for i and j). An alternative is to use nested parallelism in C, 
but not all OpenMP implementations support nested parallelism. You can
check it by omp_get_nested()
'''
@boundscheck(False) 
@wraparound(False)
cpdef double[:,:] block_MxM(double[:,:] A,double[:,:] B,int N, int q):
    cdef:
        int i,j,k
        #double[:,:] C
        int bsize

    if N%q != 0:
        raise ValueError('N must be a multiple of q!')
    else:
        bsize=N//q
        print('block size:',bsize)

    C = np.zeros([N,N],dtype=np.double)
    #for i in prange(q,nogil=True):
    for i in range(q):
        for j in range(q):
            for k in range(q):
                C[i*bsize:(i+1)*bsize,j*bsize:(j+1)*bsize] += \
                serial_MxM(A[i*bsize:(i+1)*bsize,k*bsize:(k+1)*bsize],
                           B[k*bsize:(k+1)*bsize,j*bsize:(j+1)*bsize],
                           bsize)
    return C

