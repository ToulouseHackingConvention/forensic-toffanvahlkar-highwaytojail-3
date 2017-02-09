#!/bin/bash
IF_WLAN=wlo1
IF_ETH=enp3s0
export IFPHYS="$IF_ETH"
export IFVIRT=tap0

# Guest IP is set in the virtual machine, so don't change the network configuration.
export HOSTIP=10.0.2.1
export GUESTIP=10.0.2.2

QEMU_MONITOR=5200
QEMU_SERIAL=5201

USERNAME=root

echo "Launch the virtual machine."
sudo -E qemu-system-x86_64 -enable-kvm \
    -machine "pc-i440fx-2.8" \
    -cpu "qemu64" \
    -m size=128M \
    -hda allnightlong.qcow2 \
    -monitor "tcp:127.0.0.1:${QEMU_MONITOR},server,nowait" \
    -serial "tcp:127.0.0.1:${QEMU_SERIAL},server,nowait" \
    -net "nic,model=virtio" \
    -net "tap,ifname=${IFVIRT},script=qemu-ifup.sh,downscript=qemu-ifdown.sh"
    # -cdrom "debian.iso" \
    # -boot once=d \
