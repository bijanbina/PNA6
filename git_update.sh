#! /bin/bash

source setup_variables.sh

cp -R "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/meta-user/recipes-apps/pna-iio/" Petalinux/pna-iio/
cp -R "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/meta-user/recipes-modules/pnadmc/" Petalinux/pnadmc/

printf "Enter git commit comment: "
read response

git commit -a -m response
git push origin master
