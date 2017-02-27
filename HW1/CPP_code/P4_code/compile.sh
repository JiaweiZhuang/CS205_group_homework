#!/bin/bash

# c++11 standard is required for the chrono (timing) library 
# need g++-5 or newer
g++ -O2 MxM.cpp -o MxM.out -std=c++11 -fopenmp -lpthread
