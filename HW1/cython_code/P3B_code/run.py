import numpy as np
import time
from MxV import MxV

N = 2**10 # problem size

# use random initial condition
A = np.random.rand(N,N) 
x = np.random.rand(N)

print('\n numpy.dot')
t1 = time.time()
y0 = np.dot(A,x)  
t2 = time.time()
print('Sum of elements = ',np.sum(y0))
print((t2-t1)*1000,'ms')

print('\n cython:')
t1 = time.time()
y1 = MxV(A,x) 
t2 = time.time()
# should be the same as the result from np.dot
print('Sum of elements = ',np.sum(y1)) 
print((t2-t1)*1000,'ms')
