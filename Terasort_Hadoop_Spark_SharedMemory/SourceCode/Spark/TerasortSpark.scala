val start_time = System.currentTimeMillis()
val inputfile=sc.textFile("hdfs:////user/ubuntu/input")
val sortedlines = inputfile.toSeq.sortBy { line => (line.take(10)) }
val lines=sortedlines.foreach {line => println(line)}
lines.saveAsTextFile("/user/ubuntu/output")
val end_time = System.currentTimeMillis()
println ("Total time taken :" + (end_time - start_time) + "ms")