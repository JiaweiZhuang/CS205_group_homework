from cython cimport boundscheck, wraparound
from cython.parallel cimport prange
import numpy as np

@boundscheck(False) 
@wraparound(False)
cpdef double[:] MxV(double[:,:] A,double[:] x):
    cdef:
        int i,j,N=len(x)
        double[:] y
    y = np.zeros(N,dtype=np.double)

    '''
    1) Simply parallelize row-by-row. It should be cost-optimal if 
    the p<=N, where p is the number of cores. If p>N, the rest of cores 
    will be idle.
    2) There is no reduction taking place as each thread handles different i.
    The summation happens in the inner loop with no parallelism.
    '''
    for i in prange(N,nogil=True,schedule='static'):
        for j in range(N):
            y[i] += A[i,j]*x[j]

    return y

