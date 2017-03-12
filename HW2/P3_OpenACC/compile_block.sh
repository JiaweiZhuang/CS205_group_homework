#!/bin/bash

#pgcc -acc -ta=multicore,tesla -Minfo=accel MxM_block_ACC.c -o MxM_block_ACC.out
pgcc -g -acc -ta=multicore,tesla -Minfo=accel MxM_block_debug.c -o MxM_block_debug.out

#pgcc MxM_block_ACC.c -o MxM_block_ACC.out
