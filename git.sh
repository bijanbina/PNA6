#! /bin/bash

source setup_variables.sh

CURRENT_DIR=$(pwd)

while true; do
	echo "-------------------------------------"
	echo "1 - Pull from Git"
	echo "2 - Push to Git"
	echo "3 - Meld pna-iio folders[PL-SDK-GIT]"
	echo "4 - Meld pna-startup folders[PL-GIT]"
	echo "5 - Update $GIT_PETALINUX_PROJECT from SDK"
	echo "6 - Update SDK from $GIT_PETALINUX_PROJECT"
	echo "0 - Exit"
	echo "-------------------------------------"
	read response

	if [[ "$response" == *"1"* ]]; then
		git pull origin master
		printf "Copy Files from git?[p/s/N]: "
		read response

		if [[ "$response" == *"p"* ]]; then
			cp -R Petalinux/pna-iio "$PETALINUX_INSTALL_DIR/$GIT_PETALINUX_PROJECT/project-spec/meta-user/recipes-apps"
			cp -R Petalinux/pnadmc "$PETALINUX_INSTALL_DIR/$GIT_PETALINUX_PROJECT/project-spec/meta-user/recipes-modules"
			cp -R Petalinux/pna-startup "$PETALINUX_INSTALL_DIR/$GIT_PETALINUX_PROJECT/project-spec/meta-user/recipes-apps"
		fi
		if [[ "$response" == *"s"* ]]; then
			cp -R Petalinux/pna-iio/files/* "$GIT_SDK_PETALINUX_PROJECT/src/"
		fi

		printf "Copy SDK DDRLess Files from git?[y/N]: "
		read response

		if [ "$response" = "y" ]; then
			cp -R SDK/DDRLess/*.c "$GIT_DDRLESS_PROJECT/src/"
			cp -R SDK/DDRLess/*.h "$GIT_DDRLESS_PROJECT/src/"
		fi
	fi
	if [[ "$response" == *"2"* ]]; then
		printf "Copy Files to git?[p/s/N]: "
		read response

		if [[ "$response" == *"p"* ]]; then
			cp -R "$PETALINUX_INSTALL_DIR/$GIT_PETALINUX_PROJECT/project-spec/meta-user/recipes-apps/pna-iio/" Petalinux/
			cp -R "$PETALINUX_INSTALL_DIR/$GIT_PETALINUX_PROJECT/project-spec/meta-user/recipes-modules/pnadmc/" Petalinux/
			cp -R "$PETALINUX_INSTALL_DIR/$GIT_PETALINUX_PROJECT/project-spec/meta-user/recipes-apps/pna-startup/" Petalinux/
		fi
		if [[ "$response" == *"s"* ]]; then
			cp -R "$GIT_SDK_PETALINUX_PROJECT/src/*" Petalinux/pna-iio/files/
		fi

		printf "Copy DDRLess SDK Files to git?[y/N]: "
		read response

		if [ "$response" = "y" ]; then
			rm SDK/DDRLess/*.c
			rm SDK/DDRLess/*.h
			cp -R "$GIT_DDRLESS_PROJECT/src/*.h" SDK/DDRLess/
			cp -R "$GIT_DDRLESS_PROJECT/src/*.c" SDK/DDRLess/
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
	if [[ "$response" == *"3"* ]]; then
		meld "$PETALINUX_INSTALL_DIR/$GIT_PETALINUX_PROJECT/project-spec/meta-user/recipes-apps/pna-iio/files" "$GIT_SDK_PETALINUX_PROJECT/src/" "Petalinux/pna-iio/files"
	fi
	if [[ "$response" == *"4"* ]]; then
		meld "$PETALINUX_INSTALL_DIR/$GIT_PETALINUX_PROJECT/project-spec/meta-user/recipes-apps/pna-startup" "Petalinux/pna-startup"
	fi
	if [[ "$response" == *"5"* ]]; then
		cp -R "$GIT_SDK_PETALINUX_PROJECT/src/*" "$PETALINUX_INSTALL_DIR/$GIT_PETALINUX_PROJECT/project-spec/meta-user/recipes-apps/pna-iio/files/"
	fi
	if [[ "$response" == *"6"* ]]; then
		cp -R "$PETALINUX_INSTALL_DIR/$GIT_PETALINUX_PROJECT/project-spec/meta-user/recipes-apps/pna-iio/files/" "$GIT_SDK_PETALINUX_PROJECT/src/"
	fi
	if [[ "$response" == *"0"* ]]; then
		break
	fi
done
