import numpy as np
import time
from vector_sum import sum_v

N = 2**20 # problem size

a = np.ones(N,dtype=np.double) 
t1 = time.time()
s = sum_v(a) 
t2 = time.time()

print('result=',s) # result should equal to N
print((t2-t1)*1000,'ms')
