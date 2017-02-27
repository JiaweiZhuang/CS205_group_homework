#!/bin/bash

# must be square number
thread_list='1 4 16 64'

for thread in $thread_list
do
    echo ============================
    echo testing with $thread threads
    export OMP_NUM_THREADS=$thread
    ./MxM.out 1 # provide an argument to surpress serial and dgemm 
done
