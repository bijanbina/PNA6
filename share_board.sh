#! /bin/bash

CURRENT_DIR=$(pwd)

source setup_variables.sh
BUSNUM="1-5"
while true; do
	echo "-------------------------------------"
	echo "0 - Initialize USB-IP"
	echo "1 - Bind USB-IP"
	echo "2 - Unbind USB-IP"
	echo "3 - Initialize JTAG server"
	echo "-------------------------------------"
	read response
	if [[ "$response" == *"0"* ]]; then
		sudo modprobe usbip_host
		sudo /usr/lib/linux-tools/4.4.0-171-generic/usbip list -l > usbip_list
		WRITE_LN=$(grep -i -n 'Cygnal' usbip_list | awk '{printf $1}')
		WRITE_LN=$(($WRITE_LN-1))
		BUSNUM=$(sed -n -e "$WRITE_LN"p usbip_list | awk '{printf $3}')
		rm usbip_list
		sudo /usr/lib/linux-tools/4.4.0-170-generic/usbipd &
	fi
	if [[ "$response" == *"1"* ]]; then
		sudo /usr/lib/linux-tools/4.4.0-170-generic/usbip bind -b $BUSNUM
	fi
	if [[ "$response" == *"2"* ]]; then
		sudo /usr/lib/linux-tools/4.4.0-170-generic/usbip unbind -b $BUSNUM
	fi
	if [[ "$response" == *"3"* ]]; then
		cd "$XILINX_INSTALL_DIR/SDK/2018.3/bin"
		./hw_server -s tcp::3122 &
	fi
done
