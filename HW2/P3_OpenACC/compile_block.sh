#!/bin/bash

pgcc -O2 -acc -ta=multicore,tesla -Minfo=accel MxM_block_ACC.c -o MxM_block_ACC.out
