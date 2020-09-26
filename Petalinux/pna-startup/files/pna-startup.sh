#!/bin/sh

ifconfig eth0 down
ifconfig eth0 hw ether 12:13:14:15:16:17
ifconfig eth0 up
