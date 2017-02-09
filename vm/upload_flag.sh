#!/bin/bash
IF_WLAN=wlo1
IF_ETH=enp3s0
export IFPHYS="$IF_ETH"
export IFVIRT=tap0

# Guest IP is set in the virtual machine, so don't change the network configuration.
export HOSTIP=10.0.2.1
export GUESTIP=10.0.2.2

QEMU_MONITOR=5220
QEMU_SERIAL=5221

USERNAME=gru

echo "Launch the virtual machine."
sudo -bE qemu-system-x86_64 -enable-kvm \
    -nographic \
    -machine "pc-i440fx-2.8" \
    -cpu "qemu64" \
    -m size=128M \
    -hda allnightlong-flag.qcow2 \
    -monitor "tcp:127.0.0.1:${QEMU_MONITOR},server,nowait" \
    -serial "tcp:127.0.0.1:${QEMU_SERIAL},server,nowait" \
    -net "nic,model=virtio" \
    -net "tap,ifname=${IFVIRT},script=qemu-ifup.sh,downscript=qemu-ifdown.sh"

sleep 2

echo "Copy the import folder (with flag) to the VM."
scp -r -F ./ssh_config import/* "${USERNAME}@${GUESTIP}:/home/${USERNAME}/"

# read -p "Press [Enter] key to shut down the virtual machine..."

echo "Shut down the virtual machine."
echo $(echo "system_powerdown" | nc 127.0.0.1 "${QEMU_MONITOR}")

sleep 5
