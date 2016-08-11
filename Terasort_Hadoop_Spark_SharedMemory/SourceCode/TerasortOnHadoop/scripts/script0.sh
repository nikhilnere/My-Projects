#!/bin/sh

IP=54.172.86.58
scp -i /home/nikhil/assign_2/hadoop.pem /home/nikhil/assign_2/hadoop.pem ubuntu@$IP:/home/ubuntu
scp -i /home/nikhil/assign_2/hadoop.pem /home/nikhil/TerasortOnHadoop/scripts/gensort ubuntu@$IP:/home/ubuntu
scp -i /home/nikhil/assign_2/hadoop.pem /home/nikhil/TerasortOnHadoop/scripts/valsort ubuntu@$IP:/home/ubuntu
scp -i /home/nikhil/assign_2/hadoop.pem /home/nikhil/TerasortOnHadoop/build/jar/TerasortOnHadoop.jar ubuntu@$IP:/home/ubuntu
scp -i /home/nikhil/assign_2/hadoop.pem /home/nikhil/TerasortOnHadoop/scripts/hadoopInstalltion.sh ubuntu@$IP:/home/ubuntu
scp -i /home/nikhil/assign_2/hadoop.pem /home/nikhil/TerasortOnHadoop/scripts/hadooprun.sh ubuntu@$IP:/home/ubuntu
scp -i /home/nikhil/assign_2/hadoop.pem /home/nikhil/TerasortOnHadoop/scripts/mountvolume.sh ubuntu@$IP:/home/ubuntu
scp -i /home/nikhil/assign_2/hadoop.pem /home/nikhil/TerasortOnHadoop/scripts/Update_slaves_hosts_on_slaves.sh ubuntu@$IP:/home/ubuntu
exit 0
