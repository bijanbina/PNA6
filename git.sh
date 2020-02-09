#! /bin/bash

source setup_variables.sh

echo "-------------------------------------"
echo "0 - Pull from Git"
echo "1 - Push to Git"
echo "-------------------------------------"
read response

if [ "$response" = "0" ]; then
	git pull origin master
	printf "Copy PetaLinux Files from git?[p/s/N]: "
	read response

	if [ "$response" = "p" ]; then
		cp -R Petalinux/pna-iio "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/meta-user/recipes-apps"
		cp -R Petalinux/pnadmc "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/meta-user/recipes-modules"
	elif [ "$response" = "s" ]; then
		cp -R Petalinux/pna-iio/files/* $SDK_PETALINUX_PROJECT/
	fi

	printf "Copy SDK Files from git?[y/N]: "
	read response

	if [ "$response" = "y" ]; then
		cp -R SDK/*.c "$SDK_PROJECT/"
		cp -R SDK/*.h "$SDK_PROJECT/"
	fi
elif [ "$response" = "1" ]; then
	printf "Copy PetaLinux Files to git?[p/s/N]: "
	read response

	if [ "$response" = "p" ]; then
		cp -R "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/meta-user/recipes-apps/pna-iio/" Petalinux/
		cp -R "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/meta-user/recipes-modules/pnadmc/" Petalinux/
	elif [ "$response" = "s" ]; then
		cp -R $SDK_PETALINUX_PROJECT/* Petalinux/pna-iio/files/
	fi

	printf "Copy SDK Files to git?[y/N]: "
	read response

	if [ "$response" = "y" ]; then
		rm SDK/*.c
		rm SDK/*.h
		cp -R $SDK_PROJECT/*.h SDK/
		cp -R $SDK_PROJECT/*.c SDK/
	fi

	git status

	printf "Enter git commit comment: "
	read response

	git commit -a -m "$response"

	printf "Git push?[y/N]: "
	read response
	if [ "$response" = "y" ]; then
		git push origin master
	fi
fi
