#!/bin/bash
nvcc -O2 -lcublas MxM_cublas.cu -o MxM_cublas.out
