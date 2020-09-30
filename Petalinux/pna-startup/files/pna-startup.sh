#!/bin/sh

ifconfig eth0 down
ifconfig eth0 hw ether 12:13:14:15:16:17
ifconfig eth0 up
if [ -f /etc/pna_ip ]; then
       PNA_IP=$(cat /etc/pna_ip)
       sleep 1
       ifconfig eth0 $PNA_IP
fi
WRITE_LN=$(grep -i -n 'iface eth0 inet dhcp' "/etc/network/interfaces" | awk -F : '{printf $1}
sed -i 's/iface eth0 inet dhcp/iface eth0 inet static/' "/etc/network/interfaces"
WRITE_LN=$(($WRITE_LN+1))
sed -i "$WRITE_LN"'i\ \ \ \ \ \ \ \ address '"$PNA_IP" "/etc/network/interfaces"
WRITE_LN=$(($WRITE_LN+1))
sed -i "$WRITE_LN"'i\ \ \ \ \ \ \ \ netmask 255.255.255.0' "/etc/network/interfaces"
WRITE_LN=$(($WRITE_LN+1))
sed -i "$WRITE_LN"'i\ \ \ \ \ \ \ \ gateway 192.168.1.1' "/etc/network/interfaces"