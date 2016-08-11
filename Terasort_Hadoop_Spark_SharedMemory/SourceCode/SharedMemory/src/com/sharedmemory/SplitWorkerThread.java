package com.sharedmemory;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.Arrays;
import java.util.TreeMap;

public class SplitWorkerThread implements Runnable {

	int threadNo;
	String fileName;
	
	public SplitWorkerThread(int threadNo, String fileName) {
		this.threadNo = threadNo;
		this.fileName = fileName;
	}
	
	@Override
	public void run() {
		RandomAccessFile raf;
		String line;
		int size = 100000 * 100;
		byte[] splitContent = new byte[size];
		byte[] byteLine = new byte[100];
		try {
			//BufferedWriter out = new BufferedWriter(new FileWriter("splits/split_" + threadNo +".txt"));
			//PrintWriter writer = new PrintWriter(new FileWriter("splits/split_" + threadNo +".txt"));
			FileOutputStream out = new FileOutputStream("splits/split_" + threadNo );
			raf = new RandomAccessFile(fileName, "r");
			raf.seek(threadNo * 100 * SharedMemoryConstants.LINES_PER_SPLIT);	//seek to respective chunk
			
			raf.read(splitContent, 0, size);
			
			//sorting splits
			TreeMap<String, String> sortMap = new TreeMap<String, String>();
			
			for(int i =0 ; i < SharedMemoryConstants.LINES_PER_SPLIT; i++){
				int from = i*100;
				int to = from + 100;
				byteLine = Arrays.copyOfRange(splitContent, from, to);
				line = new String(byteLine);
				sortMap.put(line.substring(0, 10), line.substring(10));
			}
			int j = 0;
			for (String key : sortMap.keySet()){
				int from = j*100;
				int to = from + 100;
				line = key + sortMap.get(key);
				//System.out.println(line.length());
				
				byteLine = line.getBytes();
				for (int k = 0; k < byteLine.length; k++){
					splitContent[from] = byteLine[k];
					from++;
				}
				j++;
			}
			
			out.write(splitContent, 0, size);
/*			for (int i=1; i <= SharedMemoryConstants.LINES_PER_SPLIT && null!=line; i++){

				out.write(line);
				if (i != SharedMemoryConstants.LINES_PER_SPLIT)
					out.newLine();
				line = raf.readLine();
			}*/
			out.flush();
			out.close();
						
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		
	}

}
