#!/bin/bash
IF_WLAN=wlo1
IF_ETH=enp3s0
export IFPHYS="$IF_ETH"
export IFVIRT=tap0

# Guest IP is set in the virtual machine, so don't change the network configuration.
export HOSTIP=10.0.2.1
export GUESTIP=10.0.2.2

QEMU_MONITOR=5210
QEMU_SERIAL=5211

USERNAME=gru

echo "Launch the virtual machine."
sudo -bE qemu-system-x86_64 -enable-kvm \
    -snapshot \
    -nographic \
    -machine "pc-i440fx-2.8" \
    -cpu "qemu64" \
    -m size=128M \
    -hda allnightlong.qcow2 \
    -monitor "tcp:127.0.0.1:${QEMU_MONITOR},server,nowait" \
    -serial "tcp:127.0.0.1:${QEMU_SERIAL},server,nowait" \
    -net "nic,model=virtio" \
    -net "tap,ifname=${IFVIRT},script=qemu-ifup.sh,downscript=qemu-ifdown.sh"

sleep 2
echo "Loading, please wait..."
sleep 3

echo "Copy the malware to the VM."
scp -F ./ssh_config build/cryptolock "${USERNAME}@${GUESTIP}:/tmp/"

printf "\n"
echo "Launch the malware."
printf "\n"
ssh -F ./ssh_config "${USERNAME}@${GUESTIP}" chmod +x /tmp/cryptolock
ssh -nf -F ./ssh_config "${USERNAME}@${GUESTIP}" /tmp/cryptolock -e

sleep 2

# Get back the key (DEBUG)
scp -F ./ssh_config "${USERNAME}@${GUESTIP}:/tmp/key" build/key

printf "\n"
echo "Dump the guest memory..."
echo $(echo "pmemsave 0 0x8000000 export/guest_dump" | nc 127.0.0.1 "${QEMU_MONITOR}")
sleep 5
sudo chown "$(whoami)" export/guest_dump
echo "Done!"

# read -p "Press [Enter] key to shut down the virtual machine..."

echo "Shut down the virtual machine."
echo $(echo "system_powerdown" | nc 127.0.0.1 "${QEMU_MONITOR}")

sleep 5

