clear

echo "================= Formatting namenode ================"
cd /home/ubuntu/hadoop-2.7.2/bin
./hdfs namenode -format
echo "================= starting dfs and yarn =============="
cd /home/ubuntu/hadoop-2.7.2/sbin/
./start-dfs.sh 
./start-yarn.sh

echo "================= Generating dataset ================="
cd /home/ubuntu
tar -xzvf gensort-linux-1.5.tar.gz
cd 64
./gensort -a 100000000 /extspace/dataset

echo "============= making input directory in hdfs=========="
cd /home/ubuntu/hadoop-2.7.2/
bin/hadoop fs -mkdir -p /user/ubuntu/input

echo "============= coping dataset from local fs to hdfs=========="
bin/hadoop dfs -copyFromLocal /extspace/dataset /user/ubuntu/input
rm /extspace/dataset

echo "============= Running Terasort app on hadoop =============="
bin/hadoop jar /home/ubuntu/TerasortOnHadoop.jar /user/ubuntu/input /user/ubuntu/output

echo "============= Coping Result from hdfs to local fs ==========="
bin/hadoop dfs -getmerge /user/ubuntu/output /extspace/output

echo "============= Runnig valsort on result =============="
cd /home/ubuntu/64
./valsort /extspace/output


cd /extspace
echo "============= First 10 lines from the sorted dataset ============"
head -10 output
echo "============= Last 10 lines from the sorted dataset ============"
tail -10 output
head -10 output > hadoopSingleFirstTen.txt
tail -10 output > hadoopSingleLastTen.txt

echo "============ Coping first and last 10 lines from instance to local syatem =========="
scp ./hadoopSingleFirstTen.txt nikhil@216.47.142.91:/home/nikhil/assign_2/runoutput
scp ./hadoopSingleLastTen.txt nikhil@216.47.142.91:/home/nikhil/assign_2/runoutput

echo "============ Experiment Completed Succefully!!  ============"
exit 0
