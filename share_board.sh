#! /bin/bash

CURRENT_DIR=$(pwd)

source setup_variables.sh
sudo modprobe usbip_host
sudo /usr/lib/linux-tools/4.4.0-170-generic/usbipd &
sudo /usr/lib/linux-tools/4.4.0-170-generic/usbip bind -b 1-5
cd "$XILINX_INSTALL_DIR/SDK/2018.3/bin"
./hw_server -s tcp::3122
