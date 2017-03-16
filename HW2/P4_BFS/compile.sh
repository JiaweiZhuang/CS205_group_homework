#!/bin/bash

pgcc -O2 -acc -ta=multicore,tesla -Minfo=accel BFS.c -o BFS.out
