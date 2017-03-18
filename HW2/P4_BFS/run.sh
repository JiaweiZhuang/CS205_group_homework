#!/bin/bash
echo 'run on host'
export ACC_DEVICE_TYPE=host
time ./BFS.out 16384 rmat14.txt
#time ./BFS.out 8192 rmat13.txt

echo 'run on GPU'
export ACC_DEVICE_TYPE=nvidia
time ./BFS.out 16384 rmat14.txt
#time ./BFS.out 8192 rmat13.txt
