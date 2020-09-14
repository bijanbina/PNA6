#! /bin/bash

CURRENT_DIR=$(pwd)

while true; do
	echo "-------------------ETTUS E310----------------------"
	echo "1 - Install board files in Vivado"
	echo "2 - Add Project to Analog Device HDL Repository"
	echo "3 - synthesize projects"
	echo "4 - Modify Meta-ADI project"
	echo "5 - Create Petalinux project"
	echo "0 - Exit"
	echo "---------------------------------------------------"
	read response_main

	if [[ "$response_main" == *"1"* ]]; then
		cd $CURRENT_DIR
		cp BoardFiles/component.xml "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/ip/xilinx/processing_system7_v5_5/component.xml"
		cp BoardFiles/e310/E310.tcl "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/ip/xilinx/processing_system7_v5_5/preset/E310.tcl"
		DIR="$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/boards/board_files/e310"
		if [ ! -d "$DIR" ]; then
			mkdir "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/boards/board_files/e310"
		fi
		DIR="$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/boards/board_files/e310/1.0"
		if [ ! -d "$DIR" ]; then
			mkdir "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/boards/board_files/e310/1.0"
		fi
		cp -R BoardFiles/e310/1.0 "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/boards/board_files/e310"

		CHECK_PRE=$(grep 'e310' "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/boards/boards_parts.IDR")
		if [ -z "$CHECK_PRE" ]; then
			WRITE_LN=$(grep -i -n '<dir src="data/boards/board_parts/zynq/zc702"></dir>' "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/boards/boards_parts.IDR" | awk -F : '{printf $1}')
			WRITE_LN=$(($WRITE_LN+2))
			sed -i "$WRITE_LN"'i\ \ <device_rule device="xc7z020" part="xc7z020clg484-3">' "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/boards/boards_parts.IDR"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN"'i\ \ \ \ <dir src="data/boards/board_files/e310"></dir>' "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/boards/boards_parts.IDR"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN"'i\ \ </device_rule>' "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/boards/boards_parts.IDR"
		fi
		
		CHECK_PRE=$(grep 'e310' "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/sysgen/xlpartinfo.at")
		if [ -z "$CHECK_PRE" ]; then
			WRITE_LN=$(grep -i -n 'xilinx_com_zc702_part0_1_4' "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/sysgen/xlpartinfo.at" | awk -F : '{printf $1}')
			WRITE_LN=$(($WRITE_LN+10))
			sed -i "$WRITE_LN""i\ \ \ \ 'ettus_com_e310_part0_1_0' => {" "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/sysgen/xlpartinfo.at"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN""i\ \ \ \ \ \ 'Vendor' => 'ettus.com'," "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/sysgen/xlpartinfo.at"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN""i\ \ \ \ \ \ 'Version' => '1.0'," "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/sysgen/xlpartinfo.at"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN""i\ \ \ \ \ \ 'BoardName' => 'e310'," "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/sysgen/xlpartinfo.at"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN""i\ \ \ \ \ \ 'BoardPartName' => 'xc7z020clg484-3'," "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/sysgen/xlpartinfo.at"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN""i\ \ \ \ \ \ 'DisplayName' => 'Ettus E310'," "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/sysgen/xlpartinfo.at"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN""i\ \ \ \ \ \ 'CompatiblieBoardRevisions' => [" "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/sysgen/xlpartinfo.at"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN""i\ \ \ \ \ \ \ \ '1.0'," "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/sysgen/xlpartinfo.at"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN""i\ \ \ \ \ \ ]," "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/sysgen/xlpartinfo.at"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN""i\ \ \ \ }," "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/sysgen/xlpartinfo.at"
		fi
		
		CHECK_PRE=$(grep 'e310' "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/parts/installed_devices.txt")
		if [ -z "$CHECK_PRE" ]; then
			WRITE_LN=$(grep -i -n 'xilinx_com_zc702_part0_1_4' "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/parts/installed_devices.txt" | awk -F : '{printf $1}')
			WRITE_LN=$(($WRITE_LN+10))
			sed -i "$WRITE_LN""i\ \ \ \ 'ettus_com_e310_part0_1_0' => {" "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/parts/installed_devices.txt"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN""i\ \ \ \ \ \ 'Vendor' => 'ettus.com'," "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/parts/installed_devices.txt"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN""i\ \ \ \ \ \ 'Version' => '1.0'," "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/parts/installed_devices.txt"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN""i\ \ \ \ \ \ 'BoardName' => 'e310'," "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/parts/installed_devices.txt"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN""i\ \ \ \ \ \ 'BoardPartName' => 'xc7z020clg484-3'," "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/parts/installed_devices.txt"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN""i\ \ \ \ \ \ 'DisplayName' => 'Ettus E310'," "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/parts/installed_devices.txt"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN""i\ \ \ \ \ \ 'CompatiblieBoardRevisions' => [" "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/parts/installed_devices.txt"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN""i\ \ \ \ \ \ \ \ '1.0'," "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/parts/installed_devices.txt"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN""i\ \ \ \ \ \ ]," "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/parts/installed_devices.txt"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN""i\ \ \ \ }," "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/parts/installed_devices.txt"
		fi
	fi

	if [[ "$response_main" == *"2"* ]]; then
		cd $CURRENT_DIR

		DIR="$ADI_HDL_PROJECT/projects/usrpe31x"
		if [ ! -d "$DIR" ]; then
			mkdir "$ADI_HDL_PROJECT/projects/usrpe31x"
		fi
		DIR="$ADI_HDL_PROJECT/library/edge_detect"
		if [ ! -d "$DIR" ]; then
			mkdir "$ADI_HDL_PROJECT/library/edge_detect"
		fi

		cp HDL/usrpe31x/Makefile 				"$ADI_HDL_PROJECT/projects/usrpe31x"
		cp HDL/usrpe31x/system_bd.tcl 			"$ADI_HDL_PROJECT/projects/usrpe31x"
		cp HDL/usrpe31x/system_constr.xdc 		"$ADI_HDL_PROJECT/projects/usrpe31x"
		cp HDL/usrpe31x/system_project.tcl 		"$ADI_HDL_PROJECT/projects/usrpe31x"
		cp HDL/usrpe31x/system_top.v 			"$ADI_HDL_PROJECT/projects/usrpe31x"

		cp HDL/common/fft/edge_detect.v 		"$ADI_HDL_PROJECT/library/edge_detect"

		CHECK_PRE=$(grep 'e310' "$ADI_HDL_PROJECT/projects/scripts/adi_project_xilinx.tcl")
		if [ -z "$CHECK_PRE" ]; then
			WRITE_LN=$(grep -i -n 'regexp "_zc702$" $project_name' "$ADI_HDL_PROJECT/projects/scripts/adi_project_xilinx.tcl" | awk -F : '{printf $1}')
			WRITE_LN=$(($WRITE_LN+5))
			sed -i "$WRITE_LN"'i\ \ if [regexp "usrpe31x$" $project_name] {' "$ADI_HDL_PROJECT/projects/scripts/adi_project_xilinx.tcl"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN"'i\ \ \ \ set p_device "xc7z020clg484-3"' "$ADI_HDL_PROJECT/projects/scripts/adi_project_xilinx.tcl"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN"'i\ \ \ \ set p_board "ettus.com:e310:part0:1.0"' "$ADI_HDL_PROJECT/projects/scripts/adi_project_xilinx.tcl"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN"'i\ \ \ \ set sys_zynq 1' "$ADI_HDL_PROJECT/projects/scripts/adi_project_xilinx.tcl"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN"'i\ \ }' "$ADI_HDL_PROJECT/projects/scripts/adi_project_xilinx.tcl"
		fi
	fi

	if [[ "$response_main" == *"3"* ]]; then
		cd "$ADI_HDL_PROJECT/projects/usrpe31x"
		export PATH="$PATH:$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/bin"
		make
	fi
	
	# Modify Meta-ADI project

	if [[ "$response_main" == *"4"* ]]; then
		CHECK_PRE=$(grep "# EXTRA_USERS_PARAMS" "$META_ADI_PROJECT/meta-adi-xilinx/recipes-core/images/petalinux-user-image.bbappend")
		if [ -z "$CHECK_PRE" ]; then
			sed -i 's/EXTRA_USERS_PARAMS = "	\\/# EXTRA_USERS_PARAMS = "	\\/' "$META_ADI_PROJECT/meta-adi-xilinx/recipes-core/images/petalinux-user-image.bbappend"
			sed -i 's/	usermod -P analog root;"/#	usermod -P analog root;"/' "$META_ADI_PROJECT/meta-adi-xilinx/recipes-core/images/petalinux-user-image.bbappend"
		fi
		CHECK_PRE=$(grep "file://pl-delete-nodes-zynq-ettus.dtsi" "$META_ADI_PROJECT/meta-adi-xilinx/recipes-bsp/device-tree/device-tree.bbappend")
		if [ -z "$CHECK_PRE" ]; then
			WRITE_LN=$(grep -i -n "file://pl-delete-nodes-vc707_fmcjesdadc1.dtsi" "$META_ADI_PROJECT/meta-adi-xilinx/recipes-bsp/device-tree/device-tree.bbappend" | awk -F : '{printf $1}')
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN"'i\    file://pl-delete-nodes-zynq-ettus.dtsi \\' "$META_ADI_PROJECT/meta-adi-xilinx/recipes-bsp/device-tree/device-tree.bbappend"
		fi
		CHECK_PRE=$(grep 'zynq-e310-ad9361-fmcomms2-3 \\' "$META_ADI_PROJECT/meta-adi-xilinx/recipes-bsp/device-tree/device-tree.bbappend")
		if [ -z "$CHECK_PRE" ]; then
			WRITE_LN=$(grep -i -n 'zynq-adrv9361-z7035-box \\' "$META_ADI_PROJECT/meta-adi-xilinx/recipes-bsp/device-tree/device-tree.bbappend" | awk -F : '{printf $1}')
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN"'i\            zynq-e310-ad9361-fmcomms2-3 \\' "$META_ADI_PROJECT/meta-adi-xilinx/recipes-bsp/device-tree/device-tree.bbappend"
		fi

		#fix echo bug
		sed -i 's/^KERNEL_DTB = .*/KERNEL_DTB = "zynq-ettus"/' "$META_ADI_PROJECT/meta-adi-xilinx/recipes-bsp/device-tree/device-tree.bbappend"
		sed -i 's/echo -e/echo/' "$META_ADI_PROJECT/meta-adi-xilinx/recipes-bsp/device-tree/device-tree.bbappend"
		sed -i 's/echo -e/echo/' "$META_ADI_PROJECT/meta-adi-xilinx/recipes-bsp/device-tree/device-tree.bbappend"
	#	sed -i 's/SRCREV = "${AUTOREV}"/SRCREV = "6184afd426f0eb2d0fa588da8fe2e21975b18c6f"/' "$META_ADI_PROJECT/meta-adi-xilinx/recipes-kernel/linux/linux-xlnx_%.bbappend"
		cp "$CURRENT_DIR/Meta-ADI/linux-xlnx_%.bbappend" "$META_ADI_PROJECT/meta-adi-xilinx/recipes-kernel/linux/"
		sed -i 's/^KBUILD_DEFCONFIG_zynq = .*/KBUILD_DEFCONFIG_zynq = "zynq_e310_defconfig"/' "$META_ADI_PROJECT/meta-adi-xilinx/recipes-kernel/linux/linux-xlnx_%.bbappend"
		sed -i "s|LINUX_KERNEL_DIRECTORY|$LINUX_KERNEL_DIR|g" "$META_ADI_PROJECT/meta-adi-xilinx/recipes-kernel/linux/linux-xlnx_%.bbappend"

#		cd "$CURRENT_DIR"
		cp Meta-ADI/pl-delete-nodes-zynq-e310.dtsi "$META_ADI_PROJECT/meta-adi-xilinx/recipes-bsp/device-tree/files/"
	#	file "zynq-zc702-adv7511-ad9361-fmcomms2-3.dts" is not necessary
	fi

	if [[ "$response_main" == *"5"* ]]; then
		printf "Enter Petalinux project name?[$PETALINUX_PROJECT]: "
		read response

		if [ -n "$response" ]; then
			PETALINUX_PROJECT="$response"
		fi

		cd "$PETALINUX_INSTALL_DIR"
		source settings.sh
		petalinux-create --type project --template zynq --name $PETALINUX_PROJECT
		cd $PETALINUX_PROJECT
		#petalinux-config --get-hw-description="$ADI_HDL_PROJECT/projects/fft/zc702/fft_zc702.sdk"
	#	petalinux-config --oldconfig --get-hw-description="$ADI_HDL_PROJECT/projects/fmcomms2/zc702/fmcomms2_zc702.sdk"
#	    petalinux-config --get-hw-description="$ADI_HDL_PROJECT/projects/ettus/e310/ettus_e310.sdk"
		petalinux-config --get-hw-description="$ADI_HDL_PROJECT/projects/usrpe31x/usrpe31x.sdk"
		
		CHECK_PRE=$(grep "CONFIG_imagefeature-debug-tweaks=y" "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/configs/rootfs_config")
		if [ -z "$CHECK_PRE" ]; then
			WRITE_LN=$(grep -i -n '# CONFIG_peekpoke is not set' "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/configs/rootfs_config" | awk -F : '{printf $1}')
			sed -i 's/# CONFIG_imagefeature-debug-tweaks is not set/CONFIG_imagefeature-debug-tweaks=y/' "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/configs/rootfs_config"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN"'iCONFIG_pna-iio=y' "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/configs/rootfs_config"
			WRITE_LN=$(($WRITE_LN+2))
			sed -i "$WRITE_LN"'i#' "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/configs/rootfs_config"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN"'i# modules' "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/configs/rootfs_config"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN"'i#' "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/configs/rootfs_config"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN"'iCONFIG_pnadmc=y' "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/configs/rootfs_config"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN"'iCONFIG_pnadmm=y' "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/configs/rootfs_config"
		fi

		CHECK_PRE=$(grep "$META_ADI_PROJECT" "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/configs/config")
		if [ -z "$CHECK_PRE" ]; then
			WRITE_LN=$(grep -i -n 'CONFIG_USER_LAYER_0=""' "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/configs/config" | awk -F : '{printf $1}')
			sed -i 's/CONFIG_USER_LAYER_0=""/ /' "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/configs/config"
			sed -i "$WRITE_LN"'iCONFIG_USER_LAYER_0="'"$META_ADI_PROJECT"'/meta-adi-core"' "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/configs/config"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN"'iCONFIG_USER_LAYER_1="'"$META_ADI_PROJECT"'/meta-adi-xilinx"' "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/configs/config"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN"'iCONFIG_USER_LAYER_2=""' "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/configs/config"
			sed -i 's/CONFIG_SUBSYSTEM_SERIAL_PS7_UART_1_BAUDRATE_115200=y/# CONFIG_SUBSYSTEM_SERIAL_PS7_UART_1_BAUDRATE_115200 is not set/' "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/configs/config"
			sed -i 's/# CONFIG_SUBSYSTEM_SERIAL_PS7_UART_1_BAUDRATE_921600 is not set/CONFIG_SUBSYSTEM_SERIAL_PS7_UART_1_BAUDRATE_115200=y/' "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/configs/config"
		fi

		CHECK_PRE=$(grep 'IMAGE_INSTALL_append = " pna-iio"' "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/meta-user/recipes-core/images/petalinux-image-full.bbappend")
		if [ -z "$CHECK_PRE" ]; then
			WRITE_LN=$(grep -i -n 'IMAGE_INSTALL_append = " gpio-demo"' "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/meta-user/recipes-core/images/petalinux-image-full.bbappend" | awk -F : '{printf $1}')
			sed -i "$WRITE_LN"'iIMAGE_INSTALL_append = " pna-iio"' "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/meta-user/recipes-core/images/petalinux-image-full.bbappend"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN"'iIMAGE_INSTALL_append = " pnadmc"' "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/meta-user/recipes-core/images/petalinux-image-full.bbappend"
		fi

		cd "$CURRENT_DIR"
		DIR="$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/meta-user/recipes-modules"
		if [ ! -d "$DIR" ]; then
			mkdir "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/meta-user/recipes-modules"
		fi
		cp -R Petalinux/pna-iio/ "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/meta-user/recipes-apps/pna-iio/"
		cp -R Petalinux/pnadmc/ "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/meta-user/recipes-modules/pnadmc/"

		# add support for offline build
		printf "Add support for offline build?[y/N]: "
		read response

		if [ "$response" = "y" ]; then
			echo 'DL_DIR = "'"$PETALINUX_INSTALL_DIR"'/mirror/downloads"' >> "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/build/conf/local.conf"
			echo 'SSTATE_DIR = "'"$PETALINUX_INSTALL_DIR"'/mirror/sstate-cache"' >> "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/build/conf/local.conf"
			echo 'BB_NO_NETWORK = "1"' >> "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/build/conf/local.conf"
			echo '# BB_GENERATE_MIRROR_TARBALLS = "1"' >> "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/build/conf/local.conf"

			printf "Modify Meta-ADI to support offline?[y/N]: "
			read response

			if [ "$response" = "y" ]; then
				cp -R Meta-ADI/fru-tools "$META_ADI_PROJECT/meta-adi-core/recipes-core/"
				cp -R Meta-ADI/libad9361-iio "$META_ADI_PROJECT/meta-adi-core/recipes-core/"
				cp -R Meta-ADI/jesd-status "$META_ADI_PROJECT/meta-adi-core/recipes-core/"
			fi
		fi

		cd "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT"
		petalinux-config --oldconfig
		petalinux-build
	fi
	
	if [[ "$response_main" == *"0"* ]]; then
		break
	fi
done
