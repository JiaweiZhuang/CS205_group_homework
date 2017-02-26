from distutils.core import setup 
from distutils.extension import Extension
from Cython.Build import cythonize

ext_modules = [ Extension("MxM",["MxM.pyx"],
                          extra_compile_args=['-fopenmp'],
                          extra_link_args=['-fopenmp']) ]

setup(name="MxM",ext_modules=cythonize(ext_modules))
