package com.sharedmemory;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;

public class MergeWorkerThread implements Runnable{

	File file1, file2;
	int threadNo, mergeIteration;
	public MergeWorkerThread(int mergeIteration, int threadNo, File file1, File file2) {
		this.file1 = file1;
		this.file2 = file2;
		this.threadNo = threadNo;
		this.mergeIteration = mergeIteration;
	}
	@Override
	public void run() {
		BufferedReader reader1 = null;
		BufferedReader reader2 = null;
		try {
			
			reader1 = new BufferedReader(new FileReader(file1));
			reader2 = new BufferedReader(new FileReader(file2));
			
			BufferedWriter out = new BufferedWriter(new FileWriter("splits/merge_" +mergeIteration+ "_" + threadNo));
			String line1 = reader1.readLine();
			String line2 = reader2.readLine();
			String key1;
			String key2;
			while (null != line1 && null != line2){
				key1 = line1.substring(0, 10);
				key2 = line2.substring(0, 10);
				if (key1.compareTo(key2) <= 0){
					out.write(line1);
					//out.newLine();
					out.write('\r');
					out.write('\n');
					line1 = reader1.readLine();
				}else{
					out.write(line2);
					//out.newLine();
					out.write('\r');
					out.write('\n');
					line2 = reader2.readLine();
				}
			}
			
			while(null != line1){
				out.write(line1);
				//out.newLine();
				out.write('\r');
				out.write('\n');
				line1 = reader1.readLine();
			}
			while(null != line2){
				out.write(line2);
				//out.newLine();
				out.write('\r');
				out.write('\n');
				line2 = reader2.readLine();
			}
			out.flush();
			out.close();
			
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} finally{
			try {
				reader1.close();
				reader2.close();
				Files.delete(Paths.get(file1.getPath()));
				Files.delete(Paths.get(file2.getPath()));
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
		}
	}

}
