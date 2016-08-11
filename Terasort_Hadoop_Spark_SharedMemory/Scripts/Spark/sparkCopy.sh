#!/bin/sh

IP=52.91.126.188
scp -i /home/nikhil/assign_2/hadoop.pem home/nikhil/assign_2/hadoop.pem root@$IP:/root
scp -i /home/nikhil/assign_2/hadoop.pem /home/nikhil/assign_2/Spark/gensort root@$IP:/root
scp -i /home/nikhil/assign_2/hadoop.pem /home/nikhil/assign_2/Spark/valsort root@$IP:/root
scp -i /home/nikhil/assign_2/hadoop.pem /home/nikhil/assign_2/Spark/mountvolume.sh root@$IP:/root
scp -i /home/nikhil/assign_2/hadoop.pem /home/nikhil/assign_2/Spark/TerasortSpark.scala root@$IP:/root
exit 0
