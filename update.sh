#! /bin/bash

source setup_variables.sh

printf "Pull from git?[y/N]: "
read response

if [ "$response" = "y" ]; then
	git pull origin master
fi

printf "Copy PetaLinux Files from git?[y/N]: "
read response

if [ "$response" = "y" ]; then
	cp -R Petalinux/pna-iio/ "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/meta-user/recipes-apps/pna-iio/"
	cp -R Petalinux/pnadmc/ "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/meta-user/recipes-modules/pnadmc/"
fi

printf "Copy SDK Files from git?[y/N]: "
read response

if [ "$response" = "y" ]; then
	cp -R SDK/*.c "$SDK_PROJECT/"
	cp -R SDK/*.h "$SDK_PROJECT/"
fi
