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
long noOfOpsCompleted[4];

/**
 * Performance 20 Floating point operations
 */
void *getGFLOPS(void *tid){

	int i;
	int threadIndex = *(int*) tid;
	float add;
	while (1){

		add = 0.2 + 0.5 +
		0.2 + 0.2+
		0.2 + 0.5+
		0.2 + 0.4+
		0.2 + 0.8+
		0.2 + 0.2+
		0.2 + 0.6+
		0.2 + 0.4+
		0.2 + 0.9+
		0.2;
		noOfOpsCompleted[threadIndex] += 20;
	}
}


void main(int argc, char *argv[]){
	logFile = fopen ("CPU_FLOP_Experiment_2.log", "w");
	int noThreads;
	double flops;
	double gflops;
	double timeInSec;
	clock_t startTime1;
	clock_t startTime2;
	clock_t endTime;
	long operations = 0;
	int index[4] = {0,1,2,3};
	//Initializing array which stores the number of operations completed by each thread
	noOfOpsCompleted[0] = 0;
	noOfOpsCompleted[1] = 0;
	noOfOpsCompleted[2] = 0;
	noOfOpsCompleted[3] = 0;

	pthread_t* tid = (pthread_t *) malloc (4*sizeof(pthread_t));
	int i = 1;
	fprintf(logFile, "\n\nStarting CPU Benchmarking (FLOPS) for 4 thread(s) for 10 mins");
	printf("\n\nStarting CPU Benchmarking (FLOPS) for 4 thread(s) for 10 mins");
	startTime1 = clock();
	startTime2 = clock();
	//Creating threads
	for (i = 0; i < 4; i++){
		pthread_create (&tid[i], NULL, getGFLOPS, &index[i]);
	}
	//Start collecting results after each second
	while(1){
		//Check if 1o mins
		if ((clock()-startTime1) / CLOCKS_PER_SEC >= 600.0){
			//kill the threads
			for (i=0; i<4; i++){
				fclose(logFile);
				pthread_kill(tid[i],1);
			}
			break;
		}else if ((double)(clock() - startTime2) / CLOCKS_PER_SEC >= 1.0){
			//reset startTime2 and add the operations done by each thread
			startTime2 = clock();
			operations += noOfOpsCompleted[0];
			operations += noOfOpsCompleted[1];
			operations += noOfOpsCompleted[2];
			operations += noOfOpsCompleted[3];
			fprintf(logFile, "%ld\n", operations);
			noOfOpsCompleted[0] = 0;
			noOfOpsCompleted[1] = 0;
			noOfOpsCompleted[2] = 0;
			noOfOpsCompleted[3] = 0;
			operations = 0;
		}
	}
	pthread_exit(NULL);
}
