clear
gcc -pthread -o app memoryBenchmarking.c
./app 1 1 100000000
./app 2 1 100000000
./app 1 1024 100000
./app 2 1024 100000
./app 1 1048576 100
./app 2 1048576 100
exit 0