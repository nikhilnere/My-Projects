package com.terasort;

import java.io.IOException; 

import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;


/**
 * 
 * @author nikhil
 * Mapper class for the terasort application
 */
class TerasortMapper extends Mapper<Object, Text, Text, Text>{
	Text keyPass = new Text();
	Text valPass = new Text();

	/**
	 * Map function of the mapReduce
	 * @param key
	 * @param value
	 * @param context
	 * @throws IOException
	 * @throws InterruptedException
	 */
	public void map (Object key, Text value, Context context) throws IOException, InterruptedException{
		String str = value.toString();
		String k = str.substring(0,10);
		String v = str.substring(10);
		keyPass.set(k);
		valPass.set(v);
		context.write(keyPass, valPass);
	}
}