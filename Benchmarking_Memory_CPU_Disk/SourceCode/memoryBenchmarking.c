/*
 * memoryBenchMarking.c
 *
 *  Created on: Feb 8, 2016
 *      Author: Nikhil
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <string.h>

#define MAX_SIZE (1024*1024*100)
FILE * logFile;
char *buffer1;
char *buffer2;
int iterations;

//Read - Write Sequentially
void *readSeq(void* bSize){
	int i, index = 0;
	int size = *(int *)bSize;
	for (i=0; i<iterations;i++){
		memcpy(&buffer1[index], &buffer2[index], size);
		index = (index + size) % (MAX_SIZE);
	}
	pthread_exit(NULL);
}

//Read - Write Randomly
void *readRand(void* bSize){
	int i,index = 0;
	int size = *(int *)bSize;
	for (i=0; i<iterations;i++){
		memcpy(&buffer1[index], &buffer2[index], size);
		index =  rand() % (MAX_SIZE - (size + 1));
	}
	pthread_exit(NULL);
}

void main(int argc, char *argv[]){
	logFile = fopen ("MemoryBenchmarking.log", "a");
	buffer1 = malloc(MAX_SIZE);
	buffer2 = malloc(MAX_SIZE);

	double timeInSec;
	clock_t startTime;
	clock_t endTime;
	int noThreads;
	int i;
	int blockSize = 0;
	double latency;
	double throughPut;
	double timeInMSec;
	double totalData;


	if (argc == 4){
		noThreads = atoi(argv[1]);
		blockSize = atoi(argv[2]);
		iterations = atoi(argv[3]);

		pthread_t* tid = (pthread_t *) malloc (noThreads*sizeof(pthread_t));
		fprintf(logFile, "\n\nStarting Memory Benchmarking for %d thread(s)", noThreads);
		printf("\n\nStarting Memory Benchmarking for %d thread(s)", noThreads);
		fprintf(logFile, "\nSequential Read");
		printf("\nSequential Read");
		fprintf(logFile, "\nBlock Size : %d", blockSize);
		printf("\nBlock Size : %d", blockSize);
		fprintf(logFile, "\nIterations : %d", iterations);
		printf("\nIterations : %d", iterations);

		//Starting sequential read-write
		startTime = clock();
		for (i = 0; i < noThreads; i++){
			pthread_create (&tid[i], NULL, readSeq, &blockSize);
		}
		for (i=0; i<noThreads; i++){
			pthread_join(tid[i], NULL);
		}
		endTime = clock();
		timeInSec = (double) (endTime - startTime) / CLOCKS_PER_SEC;
		timeInMSec = timeInSec * 1000;
		totalData = (double)(noThreads) * (double)(iterations * blockSize) / 1000000;
		throughPut = (double)totalData / timeInSec;
		latency = (double) timeInMSec / totalData;

		printf("\nTime Taken : %lf mSecs", timeInMSec);
		fprintf(logFile, "\nTime Taken : %lf mSecs", timeInMSec);
		printf("\nLatency : %lf mSec/mb", latency);
		fprintf(logFile, "\nLatency : %lf mSecs/mb", latency);
		printf("\nThroughPut : %lf mb/sec", throughPut);
		fprintf(logFile, "\nThroughPut : %lf mb/sec", throughPut);
		printf("\n");

		pthread_t* tid_2 = (pthread_t *) malloc (noThreads*sizeof(pthread_t));
		fprintf(logFile, "\n\nRandom Read");
		printf("\n\nRandom Read");
		fprintf(logFile, "\nBlock Size : %d", blockSize);
		printf("\nBlock Size : %d", blockSize);
		fprintf(logFile, "\nIterations : %d", iterations);
		printf("\nIterations : %d", iterations);

		//Starting random read-write
		startTime = clock();
		for (i = 0; i < noThreads; i++){
			pthread_create (&tid_2[i], NULL, readRand, &blockSize);
		}
		for (i=0; i<noThreads; i++){
			pthread_join(tid_2[i], NULL);
		}
		endTime = clock();

		timeInSec = (double) (endTime - startTime) / CLOCKS_PER_SEC;
		timeInMSec = timeInSec * 1000;
		totalData = (double)(noThreads) * (double)(iterations * blockSize) / 1000000;
		throughPut = (double)totalData / timeInSec;
		latency = (double) timeInMSec / totalData;

		printf("\nTime Taken : %lf mSecs", timeInMSec);
		fprintf(logFile, "\nTime Taken : %lf mSecs", timeInMSec);
		printf("\nLatency : %lf mSec/mb", latency);
		fprintf(logFile, "\nLatency : %lf mSecs/mb", latency);
		printf("\nThroughPut : %lf mb/sec", throughPut);
		fprintf(logFile, "\nThroughPut : %lf mb/sec", throughPut);
		printf("\n");
	}
}

