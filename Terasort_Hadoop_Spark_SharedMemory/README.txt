=============================================
TERASORT APPLICATION ON JAVA / HADOOP / SPARK
=============================================
The file contains explanation of code, steps to follow to run the applications.

=============================================================================================
                                 SHARED MEMORY APPLICATION
=============================================================================================

The shared memory application is implemented in two parts. The first part splits the file in smaller splits (intermediate files) and sorts the smaller splits. This is done concurrently using thread pool. The second part of the code merges the splits. This is also done concurrently using thread pool.


-----------
Java Files:
-----------

	SharedMemorySort.java:
		- Contains the main class of the application which accepts number of threads as a command line argument.
		- Starts threads which split the file concurrently
		- Starts threads which merge the splits concurrently
	
	SplitWorkerThread.java
		- Implements Runnable
		- Reads the respective block of data from the file, sorts the data and writes it to an intermediate file (split)

	MergeWorkerThread.java
		- Implements Runnable
		- Reads the contents of two intermediate files (splits) and applies merge sort on them
		
	SharedMemoryConstants.java
		- Houses all the constants required in the code

		
-------------
Steps to run:
-------------

	1) Go to source code folder and run below command to copy the source code on the instance
		scp -i 'filename.pem' -r sharedmemory ubuntu@IPAddress:/home/ubuntu
		
	2) SSH to instance
		ssh -i filename.pem ubuntu@IPAddress
		
	3) Install update, java and ant
		sudo apt-get update
		sudo apt-get install default-jdk
		
	4) Go to scr directory
		cd /home/ubuntu/sharedmemory/src
		
	5) Run below script to mount the EBS volume
		./mountvolume
		NOTE : Check the device using command lsblk and update DEVICE variable in the script
		
	6) Copy the sharedmemory folder to the directory where the EBS is mounted
		cp -r /home/ubuntu/sharedmemory /extspace
		
	7) Run below script to execute the program.
		./sharedMemory10GB
		This script will do following -
			- Compile code
			- Generate dataset
			- Execute java code
			- Run valsort on the result
			- Display first 10 sorted lines
			- Displat last 10 sorted lines
			- This script will execute the code for 5 times with increasing number level of concurrency. (1, 2, 4, 6, 8 threads)

	7) There are scripts of separated cases as well
		run1.sh -> runs the code with single thread
		run2.sh -> runs the code with 2 threads
		run4.sh -> runs the code with 4 threads
		run6.sh -> runs the code with 6 threads
		run8.sh -> runs the code with 8 threads


		
=============================================================================================
										TERASORT ON HADOOP
=============================================================================================

The terasort application in implemented in mapReduce.
The mapper accepts line contents in value and splits the key and the value from the line. The key and value from the line are emitted as key and value respectively
The reducer accepts the the key, value from the mapper in sorted order. The sorting is done by mapreduce  framework during sort and shuffle stage. The mapper concatenates the key and value and emits the result as a key.

-----------
Java files:
-----------

	TerasortOnHadoop.java:
		- Houses the main class application. 
		- Configures mapreduce which include job name, Mapper class, Reducer class. Declares key, value datatypes.
		- Accepts input and output directory as command line argument

	TerasortMapper.java
		- It is the mapper class
		- Accepts line contents in value and splits the key and the value from the line. The key and value from the line are emitted as key and value respectively
		
	TerasortReducer.java
		- It is the reducer class
		- Accepts the the key, value from the mapper in sorted order. 
		- The sorting is done by mapreduce  framework during sort and shuffle stage.
		- The mapper concatenates the key and value and emits the result as a key.


----------------------------------------------------
Steps to Run Terasort on Single Node Hadoop Cluster:
----------------------------------------------------

	1) Go to TerasortOnHadoop folder under source code folder
		cd ./SourceCode/TerasortOnHadoop
		
	2) Run below ant command to generate jar
		ant jar

	3) Create master instance on amazon EC2
	
	3) Open ./script0.sh which can be found under /SourceCode/TerasortOnHadoop/scripts and update the public Ip of the master instance
		IP=54.172.86.58
		- Also add master dns in Update_slaves_hosts_on_slaves.sh 
		- Update hadooprun.sh to enter 10GB data in the gensort command
	
	3) Run script0.sh
		./script0.sh
		
		This script will copy all the required files on the insatance which include
			- hadoopInstalltion.sh
			- hadooprun.sh
			- mountvolume.sh
			- gensort
			- valsort
			- TerasortOnHadoop.jar

	4) Login to Master
		ssh -i filename.pem ubuntu@IPadderss
	
	5) Run below script
		./hadoopInstalltion.sh
		This script will install updates, jdk, gcc, ssh and hadoop
		The script will also create rsa key and will add pem file to ssh
		
	6) Run below script
		./mountvolume
		This script will mount the external EBS volume
		
	7) Update all the configuration files which are under /home/ubuntu/hadoop-2.7.2/etc/hadoop
			
	11) Run below script
		./hadooprun
		
		This scripts performes following tasks in sequence 
		- Format namenode
		- Start dfs anf yarn
		- Generate dataset
		- Create input directory in hdfs
		- Copy input (dataset) to hdfs
		- Delete original input file to make space for execution
		- Run Terasort in hadoop
		- Copy result from hdfs to local file system
		- Run valsort on the result
		- Display and store first 10 lines
		- Display and store last 10 lines



--------------------------------------------------
Steps to Run Terasort on Muli-node Hadoop Cluster:
--------------------------------------------------

	1) Go to TerasortOnHadoop folder under source code folder
		cd ./SourceCode/TerasortOnHadoop
		
	2) Run below ant command to generate jar
		ant jar

	3) Create master instance on amazon EC2
	
	3) Open ./script0.sh which can be found under /SourceCode/TerasortOnHadoop/scripts and update the public Ip of the master instance
		IP=54.172.86.58
		- Also add master dns in Update_slaves_hosts_on_slaves.sh 
		- Update hadooprun.sh to enter 10GB data in the gensort command
	
	3) Run script0.sh
		./script0.sh
		
		This script will copy all the required files on the insatance which include
			- hadoopInstalltion.sh
			- hadooprun.sh
			- mountvolume.sh
			- Update_slaves_hosts_on_slaves.sh
			- gensort
			- valsort
			- TerasortOnHadoop.jar

	4) Login to Master
		ssh -i filename.pem ubuntu@IPadderss
	
	5) Run below script
		./hadoopInstalltion.sh
		This script will install updates, jdk, gcc, ssh and hadoop
		The script will also create rsa key and will add pem file to ssh
		
	6) Run below script
		./mountvolume
		This script will mount the external EBS volume
		
	7) Update all the configuration files which are under /home/ubuntu/hadoop-2.7.2/etc/hadoop
	
	8) Create an image of the master instance and create 16 slaves from that image.
	
	9) Run below scripts on each slave (This is done using pssh)
		./mountvolume
			This will mount EBS volume on each slave
		./Update_slaves_hosts_on_slaves
			This will update etc/hosts file and slaves file on each node. RSA key will be generated on each slave.
			
	10) Run below commands on master.
		eval "$(ssh-agent)"
		ssh-keygen  -t rsa
		eval "$(ssh-agent)"
		ssh-add hadoop.pem
		ssh-copy-id -i ~/.ssh/id_rsa.pub ubuntu@SLAVE_1_DNS
		ssh-copy-id -i ~/.ssh/id_rsa.pub ubuntu@SLAVE_2_DNS
		ssh-copy-id -i ~/.ssh/id_rsa.pub ubuntu@SLAVE_2_DNS
		ssh-copy-id -i ~/.ssh/id_rsa.pub ubuntu@SLAVE_3_DNS
		ssh-copy-id -i ~/.ssh/id_rsa.pub ubuntu@SLAVE_4_DNS
		ssh-copy-id -i ~/.ssh/id_rsa.pub ubuntu@SLAVE_5_DNS
		ssh-copy-id -i ~/.ssh/id_rsa.pub ubuntu@SLAVE_6_DNS
		ssh-copy-id -i ~/.ssh/id_rsa.pub ubuntu@SLAVE_7_DNS
		ssh-copy-id -i ~/.ssh/id_rsa.pub ubuntu@SLAVE_8_DNS
		ssh-copy-id -i ~/.ssh/id_rsa.pub ubuntu@SLAVE_9_DNS
		ssh-copy-id -i ~/.ssh/id_rsa.pub ubuntu@SLAVE_10_DNS
		ssh-copy-id -i ~/.ssh/id_rsa.pub ubuntu@SLAVE_11_DNS
		ssh-copy-id -i ~/.ssh/id_rsa.pub ubuntu@SLAVE_12_DNS
		ssh-copy-id -i ~/.ssh/id_rsa.pub ubuntu@SLAVE_13_DNS
		ssh-copy-id -i ~/.ssh/id_rsa.pub ubuntu@SLAVE_14_DNS
		ssh-copy-id -i ~/.ssh/id_rsa.pub ubuntu@SLAVE_15_DNS
		ssh-copy-id -i ~/.ssh/id_rsa.pub ubuntu@SLAVE_16_DNS
		chmod 0600 ~/.ssh/authorized_keys
			
	11) Run below script
		./hadooprun
		
		This scripts performes following tasks in sequence 
		- Format namenode
		- Start dfs anf yarn
		- Generate dataset
		- Create input directory in hdfs
		- Copy input (dataset) to hdfs
		- Delete original input file to make space for execution
		- Run Terasort in hadoop
		- Copy result from hdfs to local file system
		- Run valsort on the result
		- Display and store first 10 lines
		- Display and store last 10 lines
		

		
		
=============================================================================================
										TERASORT ON SPARK
=============================================================================================

- The Terasort application on spark is implemented in scala. 
- The input file is read from the hdfs. The file contents are converted in to map by extracting key and value from each line.
- The map is them sorted based on key.
- The key and value from the map is concatenated.
- The resulting data is saved to a file


---------------------------------------------------		
Steps to Run Terasort on Single-node Spark Cluster:
---------------------------------------------------

	1) Donwload and unpack Spark on your local system
		wget www-eu.apache.org/dist/spark/spark-1.6.0/spark-1.6.0-bin-hadoop2.6.tgz
		tar -xzvf spark-1.6.0-bin-hadoop2.6.tgz
		
	2) Go to ec2 folder
		cd ./spark-1.6.0-bin-hadoop2.6/ec2

	3) Export your AWS_ACCESS_KEY_ID and AWS_SECRET_ACCESS_KEY so that spark can request for instance through your aws account
		export AWS_ACCESS_KEY_ID=YOUR_AWS_ACCESS_KEY_ID
		export AWS_SECRET_ACCESS_KEY=YOUR_AWS_SECRET_ACCESS_KEY
	
	3) Run below command to start a cluster with one slave
		./spark-ec2 -k hadoop -i /home/nikhil/assign_2/hadoop.pem -s 1 -t c3.large --spot-price=0.03 launch spark_nodes
		
	4) Copy required files to the master. Open sparkCopy.sh and update the IP of master
		./sparkCopy.sh
			This script will copy below files
				- pem file
				- gensort
				- valsort
				- mountvolume.sh
		
	5) Login to master 
		./spark-ec2 -k hadoop -i /home/nikhil/assign_2/hadoop.pem login spark_nodes

	6) Run below script on master
		./mountvolume.sh
		This will mount ESB on the master
	
	7) Generate 10 GB dataset 
		./root/gensort -a 100000000 /extspace/dataset
		
	8) Create input directory in hdfs
		cd /root/ephemeral-hdfs
		bin/hadoop fs -mkdir -p /user/ubuntu/input
		
	9) Put input file from local file system to hdfs input directory. The replication is set to 1
		./hadoop fs -Ddfs.replication=1 -put /extspace/dataset /user/ubuntu/input
	
	10) Run application on Spark
		cd spark/bin
		./spark-shell -i TerasortSpark.scala
		
	11) Copy result from hdfs ouput directory to local filesystem
		cd /root/ephemeral-hdfs
		bin/hadoop dfs -getmerge /user/ubuntu/output /home/ubuntu/output
		
		This command will concatenate all the results into one file
		
	12) Run valsort on output
		./root/valsort output
	
	13) Run below commands to see first and last 10 lines of the result
		head -10 /extspace/output
		tail -10 /extspace/output
	
	
	
	
-------------------------------------------------
Steps to Run Terasort on Muli-node Spark Cluster:
-------------------------------------------------

	1) Donwload and unpack Spark on your local system
		wget www-eu.apache.org/dist/spark/spark-1.6.0/spark-1.6.0-bin-hadoop2.6.tgz
		tar -xzvf spark-1.6.0-bin-hadoop2.6.tgz
		
	2) Go to ec2 folder
		cd ./spark-1.6.0-bin-hadoop2.6/ec2

	3) Export your AWS_ACCESS_KEY_ID and AWS_SECRET_ACCESS_KEY so that spark can request for instance through your aws account
		export AWS_ACCESS_KEY_ID=YOUR_AWS_ACCESS_KEY_ID
		export AWS_SECRET_ACCESS_KEY=YOUR_AWS_SECRET_ACCESS_KEY
	
	3) Run below command to start a cluster with one slave
		./spark-ec2 -k hadoop -i /home/nikhil/assign_2/hadoop.pem -s 16 -t c3.large --spot-price=0.03 launch spark_nodes
		
	4) Copy required files to the master. Open sparkCopy.sh and update the IP of master
		./sparkCopy.sh
			This script will copy below files
				- pem file
				- gensort
				- valsort
				- mountvolume.sh
		
	5) Login to master 
		./spark-ec2 -k hadoop -i /home/nikhil/assign_2/hadoop.pem login spark_nodes

	6) Run below script on master
		./mountvolume.sh
		This will mount ESB on the master
	
	7) Generate 10 GB dataset 
		./root/gensort -a 1000000000 /extspace/dataset
		
	8) Create input directory in hdfs
		cd /root/ephemeral-hdfs
		bin/hadoop fs -mkdir -p /user/ubuntu/input
		
	9) Put input file from local file system to hdfs input directory. The replication is set to 1
		./hadoop fs -Ddfs.replication=1 -put /extspace/dataset /user/ubuntu/input
	
	10) Run application on Spark
		cd spark/bin
		./spark-shell -i TerasortSpark.scala
		
	11) Copy result from hdfs ouput directory to local filesystem
		cd /root/ephemeral-hdfs
		bin/hadoop dfs -getmerge /user/ubuntu/output /home/ubuntu/output
		
		This command will concatenate all the results into one file
		
	12) Run valsort on output
		./root/valsort output
	
	13) Run below commands to see first and last 10 lines of the result
		head -10 /extspace/output
		tail -10 /extspace/output