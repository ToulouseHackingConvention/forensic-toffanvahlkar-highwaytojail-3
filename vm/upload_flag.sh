#!/bin/bash
export IFPHYS=wlo1
export IFVIRT=tap0

# Guest IP is set in the virtual machine, so don't change the network configuration.
export HOSTIP=10.0.2.1
export GUESTIP=10.0.2.2

echo "Launch the virtual machine."
qemu-system-x86_64 -enable-kvm \
    -nographic \
    -serial tcp:127.0.0.1:5677,server,nowait \
    -monitor tcp:127.0.0.1:5678,server,nowait \
    -m size=128M \
    -hda deb64-flag.qcow2 \
    -net nic,model=virtio \
    -net tap,ifname=${IFVIRT},script=qemu-ifup.sh,downscript=qemu-ifdown.sh &

sleep 2

echo "Copy the import folder (with flag) to the VM."
scp -r -F ./ssh_config import/* pigeon@${GUESTIP}:/home/pigeon/

# read -p "Press [Enter] key to shut down the virtual machine..."

echo "Shut down the virtual machine."
echo $(echo "system_powerdown" | nc 127.0.0.1 5678)

sleep 5
