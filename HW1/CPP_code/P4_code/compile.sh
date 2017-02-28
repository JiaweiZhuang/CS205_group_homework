#!/bin/bash

# c++11 standard is required for the chrono (timing) library 
# need g++-5 or newer

if [ "$(uname)" == "Darwin" ]; then
	g++ -O2 MxM.cpp -o MxM.out -std=c++11 -fopenmp -lpthread lib/libopenblas_mac.a
else
	g++ -O2 MxM.cpp -o MxM.out -std=c++11 -fopenmp -lpthread lib/libopenblas_linux.a
fi 
	

# g++ -O2 MxM.cpp -o MxM.out -std=c++11 # use this for serial build
