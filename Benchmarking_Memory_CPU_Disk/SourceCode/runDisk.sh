clear
gcc -pthread -o app diskWriteBenchmarking.c
./app 1 1 100000000 temp1
./app 2 1 100000000 temp2
./app 1 1024 100000 temp3
./app 2 1024 100000 temp4
./app 1 1048576 100 temp5
./app 2 1048576 100 temp6
gcc -pthread -o app diskReadBenchmarking.c
./app 1 1 100000000 temp1
./app 2 1 100000000 temp2
./app 1 1024 100000 temp3
./app 2 1024 100000 temp4
./app 1 1048576 100 temp5
./app 2 1048576 100 temp6
exit 0