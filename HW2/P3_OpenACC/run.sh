#!/bin/bash

echo 'run on host'
export ACC_DEVICE_TYPE=host
./MxM_ACC.out

echo 'run on GPU'
export ACC_DEVICE_TYPE=nvidia
./MxM_ACC.out
