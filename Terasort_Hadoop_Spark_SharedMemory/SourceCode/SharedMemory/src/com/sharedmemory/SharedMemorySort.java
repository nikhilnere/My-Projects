package com.sharedmemory;
import java.io.File;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class SharedMemorySort {

	public static void main(String[] args) {

		//System.out.println(Runtime.getRuntime().freeMemory());
		//System.out.println(Runtime.getRuntime().totalMemory());
		long startTime = System.currentTimeMillis();
		String fileName = args[0];
		int threadPool = Integer.parseInt(args[1]);
		File file = new File(fileName);
		long noOfSplits = getNoOfSplits(file.length());
		
		//splitting files
		ExecutorService executor = Executors.newFixedThreadPool(threadPool); //pool of 10 threads
		
		for (int i =0 ; i < noOfSplits; i++){
			Runnable splitThread = new SplitWorkerThread(i,fileName);
			executor.execute(splitThread);
		}
		executor.shutdown();
		while (!executor.isTerminated()){}

		System.out.println("Files splitted successfully...Starting to merge!!");
		
		
		
		File folder = new File ("splits");
		File[] splitList = folder.listFiles();
		
		int mergeIteration = 0;
		while (splitList.length > 1){
			int i=0;
			int threadNo = 0;
			ExecutorService mergeExecutor = Executors.newFixedThreadPool(threadPool);
			while ((i+1) < splitList.length){
				Runnable mergeThread = new MergeWorkerThread(mergeIteration,threadNo,splitList[i], splitList[i+1]);
				mergeExecutor.execute(mergeThread);
				i = i + 2;
				threadNo++;
			}
			mergeExecutor.shutdown();
			while (!mergeExecutor.isTerminated()){}
			mergeIteration ++;
			folder = new File("splits");
			splitList = folder.listFiles();
		}
		
		long endTime = System.currentTimeMillis();
		System.out.println("Total time taken : " + (endTime - startTime));
	}

	private static long getNoOfSplits(long fileLength) {
		
		long totalNoLines = fileLength / SharedMemoryConstants.BYTES_PER_LINE;
		long totalNoOfFiles = totalNoLines / SharedMemoryConstants.LINES_PER_SPLIT;	//10Mb split (100000 lines)
		return totalNoOfFiles;	//10MB split
	}

}
