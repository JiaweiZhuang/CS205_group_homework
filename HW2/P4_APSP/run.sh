rm -f floyd.out

# echo 'run FW on GPU (Parallel)'
# pgcc -O2 -acc -ta=tesla -Minfo=accel APSP_Floyd.c -o floyd.out
# export ACC_DEVICE_TYPE=nvidia
# ./floyd.out 64 rmat6.txt 1
# ./floyd.out 256 rmat8.txt 1
# ./floyd.out 1024 rmat10.txt 1
# ./floyd.out 4098 rmat12.txt 1
# ./floyd.out 16384 rmat14.txt 1

# echo 'run FW on host (sequential)'
# pgcc -O2 -acc -ta=host -Minfo=accel APSP_Floyd.c -o floyd.out
# export ACC_DEVICE_TYPE=host
# ./floyd.out 64 rmat6.txt 0
# ./floyd.out 256 rmat8.txt 0
# ./floyd.out 1024 rmat10.txt 0
# ./floyd.out 4098 rmat12.txt 0
# ./floyd.out 16384 rmat14.txt 0

echo 'run Semiring (Parallel)'
pgcc -O2 -acc -ta=tesla -Minfo=accel APSP_Semiring_p.c -o Semiring.out
export ACC_DEVICE_TYPE=nvidia
./Semiring.out 64 rmat6.txt 
./Semiring.out 256 rmat8.txt 
./Semiring.out 1024 rmat10.txt 
# ./Semiring.out 4098 rmat12.txt 
# ./Semiring.out 16384 rmat14.txt 

echo 'run Semiring (sequential)'
pgcc -O2 -acc -ta=tesla -Minfo=accel APSP_Semiring.c -o Semiring.out
export ACC_DEVICE_TYPE=nvidia
./Semiring.out 64 rmat6.txt 
./Semiring.out 256 rmat8.txt 
./Semiring.out 1024 rmat10.txt 
# ./Semiring.out 4098 rmat12.txt 
# ./Semiring.out 16384 rmat14.txt 

