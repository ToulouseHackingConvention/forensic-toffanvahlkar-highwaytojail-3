#!/bin/bash

# echo "Disable IP forwarding."
# Disable IP forwarding.
# echo 0 > /proc/sys/net/ipv4/ip_forward

echo "Disable NAT in firewall."
# Disable NAT for the guest.
iptables -t nat -D POSTROUTING -o ${IFPHYS} -j MASQUERADE

echo "Disable forwarding in firewall."
# Reject established forward traffic.
iptables -D FORWARD -m conntrack --ctstate ESTABLISHED,RELATED -j ACCEPT

# Reject forward traffic from the guest.
iptables -D FORWARD -i ${IFVIRT} -m conntrack --ctstate NEW -j ACCEPT

echo "Unconfigure the virtual network interface."
# Remove host IP address.
ip a flush dev ${IFVIRT}
# Set down the virtual interface.
ip l s dev ${IFVIRT} down
