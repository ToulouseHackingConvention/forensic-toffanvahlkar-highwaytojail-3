#!/bin/bash
IF_WLAN=wlo1
IF_ETH=enp3s0
export IFPHYS="$IF_ETH"
export IFVIRT=tap0

# Guest IP is set in the virtual machine, so don't change the network configuration.
export HOSTIP=10.0.2.1
export GUESTIP=10.0.2.2

echo "Launch the virtual machine."
sudo -E qemu-system-x86_64 -enable-kvm \
    -machine "pc-i440fx-2.8" \
    -cpu "qemu64" \
    -m size=128M \
    -hda allnightlong.qcow2 \
    -monitor tcp:127.0.0.1:5200,server,nowait \
    -serial tcp:127.0.0.1:5201,server,nowait \
    -net nic,model=virtio \
    -net tap,ifname=${IFVIRT},script=qemu-ifup.sh,downscript=qemu-ifdown.sh
    # -cdrom "debian.iso" \
    # -boot once=d \
