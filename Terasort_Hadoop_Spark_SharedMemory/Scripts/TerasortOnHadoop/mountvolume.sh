clear

echo "=========== Mounting EBS ==========="
DEVICE=/dev/xvdd
sudo mkfs.ext4 $DEVICE
sudo mke2fs -F  -t  ext4 $DEVICE
sudo mkdir /extspace
sudo mount $DEVICE /extspace
sudo chmod 777 /extspace

exit 0
