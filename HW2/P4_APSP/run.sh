rm -f floyd.out

# echo 'run on GPU (Parallel)'
# pgcc -O2 -acc -ta=tesla -Minfo=accel APSP_Floyd.c -o floyd.out
# export ACC_DEVICE_TYPE=nvidia
# ./floyd.out 16384 rmat14.txt 1

echo 'run on host (sequential)'
pgcc -O2 -acc -ta=host -Minfo=accel APSP_Floyd.c -o floyd.out
export ACC_DEVICE_TYPE=host
./floyd.out 16384 rmat14.txt 0
