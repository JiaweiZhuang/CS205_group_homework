import matplotlib.pyplot as plt
import numpy as np

x = 1
y = 20

def S(n,p):
    T1 = x*n
    Tp = x*n/p + (x+y)*np.log2(p)
    return T1/Tp

def S_scale(n,p):
    return S(n*p,p)

n = 256
p_list = np.array([1,2**2,2**4,2**5,2**8])

plt.plot(p_list,S(n,p_list),marker='o',label='standard speed-up')
plt.plot(p_list,S_scale(n,p_list),marker='o',label='scaled speed-up')
plt.legend(loc=2)
plt.xscale('log')
plt.yscale('log')
plt.show()