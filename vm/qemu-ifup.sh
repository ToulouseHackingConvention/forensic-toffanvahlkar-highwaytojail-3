#!/bin/bash

echo "Set up the virtual network interface."
# Set up the virtual interface.
ip l s dev ${IFVIRT} up
# Configure host IP address.
ip a flush dev ${IFVIRT}
ip a a ${HOSTIP}/24 dev ${IFVIRT}

echo "Configure the firewall (enable NAT & allow forwarding)."
# Enable NAT for the guest.
iptables -t nat -C POSTROUTING -o ${IFPHYS} -j MASQUERADE 2>/dev/null
if [ $? -ne 0 ]
then
    iptables -t nat -A POSTROUTING -o ${IFPHYS} -j MASQUERADE
fi

# Allow established forward traffic.
iptables -C FORWARD -m conntrack --ctstate ESTABLISHED,RELATED -j ACCEPT 2>/dev/null
if [ $? -ne 0 ]
then
    iptables -A FORWARD -m conntrack --ctstate ESTABLISHED,RELATED -j ACCEPT
fi

# Allow new forward traffic initiated by the guest.
iptables -C FORWARD -i ${IFVIRT} -m conntrack --ctstate NEW -j ACCEPT 2>/dev/null
if [ $? -ne 0 ]
then
    iptables -A FORWARD -i ${IFVIRT} -m conntrack --ctstate NEW -j ACCEPT
fi

echo "Enable IP forwarding."
# Enable IP forwarding.
echo 1 > /proc/sys/net/ipv4/ip_forward
