#! /bin/bash

source setup_variables.sh

echo "-------------------------------------"
echo "0 - Pull from Git"
echo "1 - Push to Git"
echo "-------------------------------------"
read response

if [ "$response" = "0" ]; then
	git pull origin master
	printf "Copy Files from git?[p/s/N]: "
	read response

	if [ "$response" = "p" ]; then
		cp -R Petalinux/pna-iio "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/meta-user/recipes-apps"
		cp -R Petalinux/pnadmc "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/meta-user/recipes-modules"
	elif [ "$response" = "s" ]; then
		cp -R Petalinux/pna-iio/files/* $SDK_PETALINUX_PROJECT/
	fi

	printf "Copy SDK ZC702 Files from git?[y/N]: "
	read response

	if [ "$response" = "y" ]; then
		cp -R SDK/ZC702/*.c "$SDK_PROJECT/"
		cp -R SDK/ZC702/*.h "$SDK_PROJECT/"
	fi
	printf "Copy SDK DDRLess Files from git?[y/N]: "
	read response

	if [ "$response" = "y" ]; then
		cp -R SDK/DDRLess/*.c "$DDRLESS_PROJECT/"
		cp -R SDK/DDRLess/*.h "$DDRLESS_PROJECT/"
	fi
elif [ "$response" = "1" ]; then
	printf "Copy Files to git?[p/s/N]: "
	read response

	if [ "$response" = "p" ]; then
		cp -R "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/meta-user/recipes-apps/pna-iio/" Petalinux/
		cp -R "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/meta-user/recipes-modules/pnadmc/" Petalinux/
	elif [ "$response" = "s" ]; then
		cp -R $SDK_PETALINUX_PROJECT/* Petalinux/pna-iio/files/
	fi

	printf "Copy ZC702 SDK Files to git?[y/N]: "
	read response

	if [ "$response" = "y" ]; then
		rm SDK/ZC702/*.c
		rm SDK/ZC702/*.h
		cp -R $SDK_PROJECT/*.h SDK/ZC702/
		cp -R $SDK_PROJECT/*.c SDK/ZC702/
	fi

	printf "Copy DDRLess SDK Files to git?[y/N]: "
	read response

	if [ "$response" = "y" ]; then
		rm SDK/DDRLess/*.c
		rm SDK/DDRLess/*.h
		cp -R $DDRLESS_PROJECT/*.h SDK/DDRLess/
		cp -R $DDRLESS_PROJECT/*.c SDK/DDRLess/
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
