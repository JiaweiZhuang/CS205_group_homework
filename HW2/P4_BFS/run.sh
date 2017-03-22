#!/bin/bash

pgcc -O2 -acc -ta=multicore BFS.c -o BFS.out
echo 'run on host (parallel)'
export ACC_DEVICE_TYPE=host
./BFS.out 16384 rmat14.txt
# ./BFS.out 8192 rmat13.txt

pgcc -O2 -acc -ta=host BFS_SeqRef.c -o BFS.out
echo 'run on host (Sequential)'
export ACC_DEVICE_TYPE=host
./BFS.out 16384 rmat14.txt
# ./BFS.out 8192 rmat13.txt

pgcc -O2 -acc -ta=tesla BFS.c -o BFS.out
echo 'run on GPU (parallel)'
export ACC_DEVICE_TYPE=nvidia
./BFS.out 16384 rmat14.txt
# ./BFS.out 8192 rmat13.txt

pgcc -O2 -acc -ta=tesla BFS_SeqRef.c -o BFS.out
echo 'run on GPU (Sequential)'
export ACC_DEVICE_TYPE=nvidia
./BFS.out 16384 rmat14.txt
# ./BFS.out 8192 rmat13.txt



