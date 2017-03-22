#!/bin/bash

# we use sequential code on host (-ta=host instead of -ta=multicore),
# because the parallel code on host is buggy. The parallel code
# on GPU is correct, giving the same result as the sequential one.

pgcc -O2 -acc -ta=host,tesla -Minfo=accel BFS.c -o BFS.out
# pgcc -O2 -acc -ta=multicore,tesla -Minfo=accel BFS.c -o BFS.out
