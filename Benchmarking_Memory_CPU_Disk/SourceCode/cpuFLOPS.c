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
 * Performance 20 floating point operations
 */
void *getGFLOPS(void* tid){
	int i;
	float add;
	for (i = 0; i < 1000000000; i++){
		add = 0.2 + 0.5 +
		0.2 + 0.2+
		0.2 + 0.5+
		0.2 + 0.4+
		0.2 + 0.8+
		0.2 + 0.2+
		0.2 + 0.6+
		0.2 + 0.4+
		0.2 + 0.9+
		0.2 ;
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
 * Calculates GFLOPS
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
		fprintf(logFile, "\n\nStarting CPU Benchmarking (FLOPS) for %d thread(s)", noThreads);
		printf("\n\nStarting CPU Benchmarking (FLOPS) for %d thread(s)", noThreads);

		startTime = clock();
		//Creating threads
		for (i = 0; i < noThreads; i++){
			pthread_create (&tid[i], NULL, getGFLOPS,NULL);
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
		//flops = total number of operations devided by time in seconds
		flops = (noThreads * 20)*(1000000000/timeInSec);
		gflops = flops / 1000000000;
		fprintf(logFile, "\nTime Taken : %lf mSec", timeInSec);
		printf("\nTime Taken : %lf", timeInSec);
		fprintf(logFile, "\nNumber of FLOPS : %lf", flops);
		printf("\nNumber of FLOPS : %lf", flops);
		fprintf(logFile, "\nNumber of GFLOPS : %lf", gflops);
		printf("\nNumber of GFLOPS : %lf", gflops);

	}else{
		fprintf (logFile, "\nProg Name : %s :: Enter number of threads as command line argument", argv[0]);
	}

	fclose(logFile);
}
