#!/bin/bash
clear
gcc -pthread -o app cpuIOPS.c
./app 1
./app 2
./app 4
gcc -pthread -o app cpuFLOPS.c
./app 1
./app 2
./app 4
gcc -pthread -o app cpuFLOPSExp_2.c
./app
gcc -pthread -o app cpuIOPSExp_2.c
./app
exit 0
