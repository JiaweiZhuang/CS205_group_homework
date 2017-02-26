from cython cimport boundscheck, wraparound
from cython.parallel cimport prange

@boundscheck(False) 
@wraparound(False)
cpdef double sum_v(double[:] v):
    cdef:
        int i,N=len(v)
        double s=0.0
    '''
    1) Within prange, cython will treat an in-place operator as an 
    OpenMP-reduction. In this case, it means each thread computes 
    the sum for a subset of the loop indices, and then all threads 
    combine their individual sums into the resulting total.
    2) Whether the algorithm is fast or cost-optimal should only 
    depend on the threads/cores allocated. The high-level implementation 
    should be the same.
    3) static schedule seems to have less overhead than dynamic schedule
    '''
    for i in prange(N,nogil=True,schedule='static'):
        s += v[i] # in-place operator means a reduction
    return s
