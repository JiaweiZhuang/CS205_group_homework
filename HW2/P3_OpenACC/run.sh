#!/bin/bash

echo 'run on host'
export ACC_DEVICE_TYPE=host
./MxM_block_ACC.out

echo 'run on GPU'
export ACC_DEVICE_TYPE=nvidia
./MxM_block_ACC.out
