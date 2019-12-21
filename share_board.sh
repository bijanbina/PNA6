#! /bin/bash

CURRENT_DIR=$(pwd)

source setup_variables.sh

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
		sudo /usr/lib/linux-tools/4.4.0-170-generic/usbipd &
	fi
	if [[ "$response" == *"1"* ]]; then
		sudo /usr/lib/linux-tools/4.4.0-170-generic/usbip bind -b 1-5
	fi
	if [[ "$response" == *"2"* ]]; then
		sudo /usr/lib/linux-tools/4.4.0-170-generic/usbip unbind -b 1-5
	fi
	if [[ "$response" == *"3"* ]]; then
		cd "$XILINX_INSTALL_DIR/SDK/2018.3/bin"
		nohup ./hw_server -s tcp::3122 &
	fi
done
