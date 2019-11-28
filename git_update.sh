#! /bin/bash

source setup_variables.sh

printf "Copy PetaLinux Files to git?[y/N]: "
read response

if [ "$response" = "y" ]; then
	cp -R "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/meta-user/recipes-apps/pna-iio/" Petalinux/
	cp -R "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/meta-user/recipes-modules/pnadmc/" Petalinux/
fi

printf "Copy SDK Files to git?[y/N]: "
read response

if [ "$response" = "y" ]; then
	cp -R $SDK_PROJECT/*.h SDK/
	cp -R $SDK_PROJECT/*.c SDK/
fi

git status

printf "Enter git commit comment: "
read response

git commit -a -m "$response"
git push origin master
