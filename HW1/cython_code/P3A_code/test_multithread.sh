#!/bin/bash

thread_list='1 2 4 8 16 32 64'

for thread in $thread_list
do
    echo ============================
    echo testing with $thread threads
    export OMP_NUM_THREADS=$thread
    python run.py
done
