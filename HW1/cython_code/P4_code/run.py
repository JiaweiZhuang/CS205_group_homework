import numpy as np
import scipy.linalg as la
import time
from MxM import serial_MxM,block_MxM 

N = 2**10 # problem size

# use random initial condition
A = np.random.rand(N,N) 
B = np.random.rand(N,N)

# dgemm in the level-3 BLAS for reference
print('\n dgemm:')
t1 = time.time()
C0 = la.blas.dgemm(1.0,A,B)
t2 = time.time()
print('Sum of elements = ',np.sum(C0))
print((t2-t1)*1000,'ms')

print('\n serial version:')
t1 = time.time()
C1 = serial_MxM(A,B,N) 
t2 = time.time()
print('Sum of elements = ',np.sum(C1))
print((t2-t1)*1000,'ms')

print('\n block version:')
t1 = time.time()
C2 = block_MxM(A,B,N,4) # divide into 4x4=16 blocks 
t2 = time.time()
print('Sum of elements = ',np.sum(C2))
print((t2-t1)*1000,'ms')
