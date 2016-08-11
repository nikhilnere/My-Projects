clear
sudo wget -q --no-check-certificate https://raw.githubusercontent.com/panticz/installit/master/install.linpack.sh -O - | bash -
URL=http://registrationcenter.intel.com/irc_nas/3914/l_lpk_p_11.1.2.005.tgz
sudo wget ${URL} -O /tmp/l_lpk.tgz
sudo tar -xzf /tmp/l_lpk.tgz -C /tmp/
sudo cp -a /tmp/linpack_11.1.2/benchmarks/linpack/ /usr/share/
sudo ln -sf /usr/share/linpack/runme_xeon64 /usr/sbin/
sudo ln -sf /usr/share/linpack/xlinpack_xeon64 /usr/sbin/
sudo sed -i s'|./xlinpack_$arch lininput_$arch|/usr/sbin/xlinpack_$arch /usr/share/linpack/lininput_$arch|g' /usr/sbin/runme_xeon64
sudo runme_xeon64
exit 1