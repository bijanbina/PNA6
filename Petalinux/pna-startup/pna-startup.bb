#
# This file is the pna-startup recipe.
#

SUMMARY = "Simple pna-startup application"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://load-config \
		  "
		  
inherit update-rc.d

INITSCRIPT_NAME = "load-config"
INITSCRIPT_PARAMS = "defaults 00"

S = "${WORKDIR}"

do_install() {
	     install -d ${D}${sysconfdir}/init.d/
         install -m 0755 load-config ${D}${sysconfdir}/init.d/load-config
}

#${IMAGE_ROOTFS}/etc/inittab;"
FILES_${PN} += "/home/root/"
