/*
 * diskWriteBenchmarking.c
 *
 *  Created on: Feb 7, 2016
 *      Author: Nikhil
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <string.h>

FILE * logFile;
char buffer [1024*1024];
int iterations;
char* fileName;
char* fileName_rand;

//Writes Sequentially to the file
void *writeSeq(void* bSize){
	int i;
	int size = *(int *)bSize;
	FILE *filePtr = fopen (fileName, "a");
	for (i=0; i<iterations;i++){
		fwrite(buffer, 1, size, filePtr);
	}
	pthread_exit(NULL);
}

//Writes Randomly to the file
void *writeRand(void* bSize){
	int i;
	int size = *(int *)bSize;
	FILE *filePtr = fopen (fileName_rand, "a");
	for (i=0; i<iterations;i++){
		//Randomly moving the file pointer
		fseek (filePtr, (rand()%10000), SEEK_SET);
		fwrite(buffer, 1, size, filePtr);
	}
	pthread_exit(NULL);
}

/**
 * Accepts four command line arguments
 * 1) No of Threads
 * 2) Block Size
 * 3) Number of Iterations
 * 4) File Name
 */
void main(int argc, char *argv[]){
	logFile = fopen ("DiskBenchmarking.log", "a");
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

	//Initialize the buffer
	for (i=0; i<(1024*1024); i++){
		buffer[i] = 'n';
	}
	if (argc == 5){
		//Rendering command line arguments
		noThreads = atoi(argv[1]);
		blockSize = atoi(argv[2]);
		iterations = atoi(argv[3]);
		fileName = argv[4];
		fileName_rand = (char *) malloc (strlen(fileName));
		strcpy(fileName_rand, argv[4]);
		strcat(fileName_rand, "_rand");

		pthread_t* tid = (pthread_t *) malloc (noThreads*sizeof(pthread_t));
		fprintf(logFile, "\n\nStarting Disk Write Benchmarking for %d thread(s)", noThreads);
		printf("\n\nStarting Disk Write Benchmarking for %d thread(s)", noThreads);
		fprintf(logFile, "\nSequential Write");
		printf("\nSequential Write");
		fprintf(logFile, "\nBlock Size : %d", blockSize);
		printf("\nBlock Size : %d", blockSize);
		fprintf(logFile, "\nIterations : %d", iterations);
		printf("\nIterations : %d", iterations);
		fprintf(logFile, "\nFile Name : %s", fileName);
		printf("\nFile Name : %s", fileName);

		//Starting threads for sequential writes
		startTime = clock();
		for (i = 0; i < noThreads; i++){
			pthread_create (&tid[i], NULL, writeSeq, &blockSize);
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


		//Staring threads for random Writes
		pthread_t* tid_2 = (pthread_t *) malloc (noThreads*sizeof(pthread_t));
		fprintf(logFile, "\n\nRandom Write");
		printf("\n\nRandom Write");
		fprintf(logFile, "\nBlock Size : %d", blockSize);
		printf("\nBlock Size : %d", blockSize);
		fprintf(logFile, "\nIterations : %d", iterations);
		printf("\nIterations : %d", iterations);
		fprintf(logFile, "\nFile Name : %s", fileName_rand);
		printf("\nFile Name : %s", fileName_rand);

		startTime = clock();
		for (i = 0; i < noThreads; i++){
			pthread_create (&tid_2[i], NULL, writeRand, &blockSize);
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

	}else{
		fprintf (logFile, "\nProg Name : %s :: Enter number of threads as command line argument", argv[0]);
	}
}
