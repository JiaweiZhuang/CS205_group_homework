#!/bin/bash

pgcc -acc -ta=multicore,tesla -Minfo=accel MxM_ACC.c -o MxM_ACC.out
