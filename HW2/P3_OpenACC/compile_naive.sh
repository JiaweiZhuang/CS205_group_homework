#!/bin/bash

pgcc -O2 -acc -ta=multicore,tesla -Minfo=accel MxM_naive_ACC.c -o MxM_naive_ACC.out
