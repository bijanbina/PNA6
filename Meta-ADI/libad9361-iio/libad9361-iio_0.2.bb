SUMMARY = "Library to manage multi-chip sync on FMCOMMS5 platforms with multiple AD9361 devices"
SECTION = "libs"
LICENSE = "LGPLv2.1+"
LIC_FILES_CHKSUM = "file://LICENSE;md5=40d2542b8c43a3ec2b7f5da31a697b88"
BRANCH = "master"

# just pick the last revision on master
# SRCREV = "${AUTOREV}"
# PV_append = "+git${SRCPV}"

SRC_URI = "file://libad9361-iio-master.zip"

S = "${WORKDIR}/libad9361-iio-master"

DEPENDS = "libiio"

inherit cmake pkgconfig
