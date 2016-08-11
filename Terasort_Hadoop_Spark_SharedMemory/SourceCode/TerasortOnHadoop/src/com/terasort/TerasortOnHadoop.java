package com.terasort;
import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;

/**
 * 
 * @author nikhil
 *
 */
public class TerasortOnHadoop {

	private TerasortOnHadoop() {
		super();
		// TODO Auto-generated method stub50070/dfshealth.html#tab-overview
	}
	/**
	 * @param args
	 * @throws IOException 
	 * @throws InterruptedException 
	 * @throws ClassNotFoundException 
	 */
	public static void main(String[] args) throws IOException, ClassNotFoundException, InterruptedException {
		long startTime = System.currentTimeMillis();
		Configuration conf = new Configuration();
		Job job = new Job(conf, "TerasortOnHadoop");
		job.setJarByClass(TerasortOnHadoop.class);	//set the application class
		job.setMapperClass(TerasortMapper.class);	//set the mapper class
		//job.setCombinerClass(IntSumReducer.class);
		job.setReducerClass(TerasortReducer.class);	//set the reducer class
		job.setOutputKeyClass(Text.class);	//set output key type
		job.setOutputValueClass(Text.class);	//set output value type
		FileInputFormat.addInputPath(job, new Path(args[0]));	//set input path (first command line argument)
		FileOutputFormat.setOutputPath(job, new Path(args[1]));	//set output  path (second command line argument)
		job.waitForCompletion(true);	//start and wait for job completion 
		long endTime = System.currentTimeMillis();
		System.out.println("Total Time for Execution :" + (endTime-startTime));
	}

}

