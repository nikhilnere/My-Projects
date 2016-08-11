package com.terasort;

import java.io.IOException;

import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;


/**
 * 
 * @author nikhil
 * Reducer class for the terasort application 
 */
class TerasortReducer extends Reducer<Text, Text, Text, Text>{
	Text keyPass = new Text();
	Text valPass = new Text();
	/**
	 * Reduce function of mapReduce
	 * @param key
	 * @param value
	 * @param context
	 * @throws IOException
	 * @throws InterruptedException
	 */
	public void reduce (Text key, Text value, Context context) throws IOException, InterruptedException{
		keyPass.set(key.toString() + value.toString());
		valPass.set("");
		context.write(keyPass, valPass);
	}
}
