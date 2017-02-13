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
    -hda vm/allnightlong.qcow2 \
    -monitor "tcp:127.0.0.1:${QEMU_MONITOR},server,nowait" \
    -serial "tcp:127.0.0.1:${QEMU_SERIAL},server,nowait" \
    -net "nic,model=virtio" \
    -net "tap,ifname=${IFVIRT},script=vm/qemu-ifup.sh,downscript=vm/qemu-ifdown.sh"

sleep 2

echo "Copy the files to the VM."
scp -F ./ssh_config ./crack.sh "${USERNAME}@${GUESTIP}:/tmp/"
scp -F ./ssh_config out/cryptolock "${USERNAME}@${GUESTIP}:/tmp/"
scp -F ./ssh_config out/key_* "${USERNAME}@${GUESTIP}:/tmp/"
scp -F ./ssh_config out/kiwi.mp4 "${USERNAME}@${GUESTIP}:/home/${USERNAME}/"

printf "\n"
echo "Try to decrypt the home."
printf "\n"
ssh -F ./ssh_config "${USERNAME}@${GUESTIP}" chmod +x /tmp/crack.sh
ssh -F ./ssh_config "${USERNAME}@${GUESTIP}" chmod +x /tmp/cryptolock
ssh -F ./ssh_config "${USERNAME}@${GUESTIP}" /tmp/crack.sh

echo "Get back the flag."
scp -F ./ssh_config "${USERNAME}@${GUESTIP}:/home/${USERNAME}/kiwi.mp4" out/kiwi_dec.mp4

# read -p "Press [Enter] key to shut down the virtual machine..."

echo "Shut down the virtual machine."
echo $(echo "system_powerdown" | nc 127.0.0.1 "${QEMU_MONITOR}")

sleep 5
