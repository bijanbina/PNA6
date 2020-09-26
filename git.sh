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

	if [[ "$response" == *"p"* ]]; then
		cp -R Petalinux/pna-iio "$PETALINUX_INSTALL_DIR/$GIT_PETALINUX_PROJECT/project-spec/meta-user/recipes-apps"
		cp -R Petalinux/pnadmc "$PETALINUX_INSTALL_DIR/$GIT_PETALINUX_PROJECT/project-spec/meta-user/recipes-modules"
		cp -R Petalinux/pna-startup "$PETALINUX_INSTALL_DIR/$GIT_PETALINUX_PROJECT/project-spec/meta-user/recipes-apps"
	fi
	if [[ "$response" == *"s"* ]]; then
		cp -R Petalinux/pna-iio/files/* $GIT_SDK_PETALINUX_PROJECT/
	fi

	printf "Copy SDK DDRLess Files from git?[y/N]: "
	read response

	if [ "$response" = "y" ]; then
		cp -R SDK/DDRLess/*.c "$GIT_DDRLESS_PROJECT/"
		cp -R SDK/DDRLess/*.h "$GIT_DDRLESS_PROJECT/"
	fi
elif [ "$response" = "1" ]; then
	printf "Copy Files to git?[p/s/N]: "
	read response

	if [[ "$response" == *"p"* ]]; then
		cp -R "$PETALINUX_INSTALL_DIR/$GIT_PETALINUX_PROJECT/project-spec/meta-user/recipes-apps/pna-iio/" Petalinux/
		cp -R "$PETALINUX_INSTALL_DIR/$GIT_PETALINUX_PROJECT/project-spec/meta-user/recipes-modules/pnadmc/" Petalinux/
		cp -R "$PETALINUX_INSTALL_DIR/$GIT_PETALINUX_PROJECT/project-spec/meta-user/recipes-apps/pna-startup/" Petalinux/
	fi
	if [[ "$response" == *"s"* ]]; then
		cp -R $GIT_SDK_PETALINUX_PROJECT/* Petalinux/pna-iio/files/
	fi

	printf "Copy DDRLess SDK Files to git?[y/N]: "
	read response

	if [ "$response" = "y" ]; then
		rm SDK/DDRLess/*.c
		rm SDK/DDRLess/*.h
		cp -R $GIT_DDRLESS_PROJECT/*.h SDK/DDRLess/
		cp -R $GIT_DDRLESS_PROJECT/*.c SDK/DDRLess/
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
