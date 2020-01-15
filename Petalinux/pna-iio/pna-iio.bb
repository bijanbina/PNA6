#
# This is the PNA-IIO apllication recipe
#
#

SUMMARY = "pna-iio application"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"
SRC_URI = "file://pna-iio.c \
           file://pna-base.c \
           file://pna-base.h \
           file://pna-gpio.c \
           file://pna-gpio.h \
           file://pna-capture.c \
           file://pna-capture.h \
           file://pna-fir.c \
           file://pna-fir.h \
           file://Makefile \
           file://filter.ftr \
        "
DEPENDS = " libiio libxml2"

S = "${WORKDIR}"
CFLAGS_prepend = "-I ${S}/include" 
# LDFLAGS_prepend = "-lz -lm -liio" 

homedir = "/home/root"

do_compile() {
        oe_runmake
}
do_install() {
        install -d ${D}${bindir}
        install -m 0755 ${S}/pna-iio ${D}${bindir}
        install -d ${D}${homedir}
        install -m 0755 ${S}/filter.ftr ${D}${homedir}
}

inherit pkgconfig
FILES_${PN} += "${homedir}/*"
