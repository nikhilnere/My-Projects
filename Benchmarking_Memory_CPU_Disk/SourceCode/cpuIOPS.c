/*
 * benchmarkingCPU.c
 *
 *  Created on: Feb 5, 2016
 *      Author: Nikhil
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

FILE* logFile;

/**
 * Performance 20 Integer operations
 */
void *getGIOPS(void* tid){
	int i;
	int add;
	for (i = 0; i < 1000000000; i++){
		add = 43 + 54 +
		62 + 32+
		52 + 55+
		52 + 54+
		52 + 58+
		52 + 52+
		52 + 56+
		52 + 54+
		52 + 59+
		52 ;
	}
	pthread_exit(NULL);
}

/**
 * used to calculate the time for empty for loop
 */
void *getEmptyForLoopTime(void* tid){
	int i;
	for (i = 0; i < 1000000000; i++){
	}
	pthread_exit(NULL);
}

/*
 * Calculates GIOPS
 */
void main(int argc, char *argv[]){
	logFile = fopen ("CPUBenchmarking.log", "a");
	int noThreads;
	void *status;
	double flops;
	double gflops;
	double timeInSec;
	clock_t startTime;
	clock_t endTime;
	clock_t timeToSubtract;
	clock_t actualTimeToConsider;

	if (argc == 2){
		noThreads = atoi(argv[1]);
		//printf("\nArgument is %d", atoi(argv[1]));

		pthread_t* tid = (pthread_t *) malloc (noThreads*sizeof(pthread_t));
		int i = 1;
		fprintf(logFile, "\n\nStarting CPU Benchmarking (IOPS) for %d thread(s)", noThreads);
		printf("\n\nStarting CPU Benchmarking (IOPS) for %d thread(s)", noThreads);
		startTime = clock();
		//Creating threads
		for (i = 0; i < noThreads; i++){
			pthread_create (&tid[i], NULL, getGIOPS,NULL);
		}
		//Waiting for the threads to finish processing
		for (i=0; i<noThreads; i++){
			pthread_join(tid[i], NULL);
		}

		endTime = clock();
		actualTimeToConsider = endTime - startTime;

		/*//Record time for empty for loop
		pthread_t* tid_2 = (pthread_t *) malloc (noThreads*sizeof(pthread_t));
		startTime = clock();
		for (i = 0; i < noThreads; i++){
			pthread_create (&tid_2[i], NULL, getEmptyForLoopTime,NULL);
		}

		for (i=0; i<noThreads; i++){
			pthread_join(tid_2[i], NULL);
		}
		endTime = clock();

		//Subtracting the time taken by the empty for loop
		actualTimeToConsider = actualTimeToConsider - (endTime - startTime);*/

		timeInSec = (double) actualTimeToConsider/CLOCKS_PER_SEC;
		//iops = total number of operations devided by time in seconds
		flops = (noThreads * 20)*(1000000000/timeInSec);
		gflops = flops / 1000000000;
		fprintf(logFile, "\nTime Taken : %lf mSec", timeInSec);
		printf("\nTime Taken : %lf", timeInSec);
		fprintf(logFile, "\nNumber of IOPS : %lf", flops);
		printf("\nNumber of FLOPS : %lf", flops);
		fprintf(logFile, "\nNumber of ILOPS : %lf", gflops);
		printf("\nNumber of GFLOPS : %lf", gflops);

	}else{
		fprintf (logFile, "\nProg Name : %s :: Enter number of threads as command line argument", argv[0]);
	}

	fclose(logFile);
}
