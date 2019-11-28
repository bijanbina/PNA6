#! /bin/bash

source setup_variables.sh

printf "Copy PetaLinux Files from git?[y/N]: "
read response

git pull origin master

if [ "$response" = "y" ]; then
	cp -R Petalinux/pna-iio/ "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/meta-user/recipes-apps/pna-iio/"
	cp -R Petalinux/pnadmc/ "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/meta-user/recipes-modules/pnadmc/"
fi

cd "$PETALINUX_INSTALL_DIR"
source settings.sh

cd "$PETALINUX_PROJECT"

while true; do
	printf "Build $PETALINUX_PROJECT?[y/N]: "
	read response
	if [ "$response" = "y" ]; then
		petalinux-build
	fi

	printf "Package & boot on board?[y/N]: "
	read response

	if [ "$response" = "y" ]; then
		petalinux-package --prebuilt --fpga images/linux/system.bit --force
		petalinux-boot --jtag --prebuilt 3 -v
		petalinux-boot --jtag --fpga --bitstream images/linux/system.bit
	fi
done
