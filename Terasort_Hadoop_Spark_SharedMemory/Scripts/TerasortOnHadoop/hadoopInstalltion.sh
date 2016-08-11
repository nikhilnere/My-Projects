clear

echo "================= Installing updates =================" 
sudo apt-get update
echo "================= Installing JDK ====================="
sudo apt-get install default-jdk
echo "================= Installing gcc ====================="
sudo apt-get install gcc -y
echo "================= Installing ssh ====================="
sudo apt-get install ssh
echo "================= Installing vim ====================="
sudo apt-get install vim

echo "================= Generating RSA Key ================="
ssh-keygen -t rsa -P ""
cat $HOME/.ssh/id_rsa.pub >> $HOME/.ssh/authorized_keys

echo "================= Adding pem file to ssh=============="
eval `ssh-agent -s`
chmod 400 hadoop.pem
ssh-add hadoop.pem

echo "================= Downloading hadoop 2.7.2 ==========="
cd /home/ubuntu
wget archive.apache.org/dist/hadoop/common/hadoop-2.7.2/hadoop-2.7.2.tar.gz
tar -zxvf hadoop-2.7.2.tar.gz
chmod 777 hadoop-2.7.2

exit 0
