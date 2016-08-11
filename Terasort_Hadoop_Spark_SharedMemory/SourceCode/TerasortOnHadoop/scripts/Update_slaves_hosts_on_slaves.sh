#!/bin/bash

MASTER_DNS="ec2-54-172-86-58.compute-1.amazonaws.com"
SLAVE_IP=$(hostname -I)
SLAVE_DNS=$(curl -s http://169.254.169.254/latest/meta-data/public-hostname)
HOST_FILE_PATH="/etc/hosts"
SLAVES_FILE_PATH="hadoop-2.7.2/etc/hadoop/slaves"

sudo sed -i -e 's/127.0.0.1 localhost/'"$SLAVE_IP $SLAVE_DNS"'/g' $HOST_FILE_PATH
sed -i -e 's/localhost/<!--localhost-->/g' $SLAVES_FILE_PATH
echo $SLAVE_DNS >> $SLAVES_FILE_PATH
eval "$(ssh-agent)"
ssh-keygen  -t rsa
eval "$(ssh-agent)"
ssh-add hadoop.pem
ssh-copy-id -i ~/.ssh/id_rsa.pub ubuntu@$MASTER_DNS
chmod 0600 ~/.ssh/authorized_keys

exit 0