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
		echo "----------Install board files in Vivado------------"
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

		BOARD_PART="$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/boards/boards_parts.IDR"
		CHECK_PRE=$(grep 'e310' "$BOARD_PART")
		if [ -z "$CHECK_PRE" ]; then
			WRITE_LN=$(grep -i -n '<dir src="data/boards/board_parts/zynq/zc702"></dir>' "$BOARD_PART" | awk -F : '{printf $1}')
			WRITE_LN=$(($WRITE_LN+2))
			sed -i "$WRITE_LN"'i\ \ <device_rule device="xc7z020" part="xc7z020clg484-3">' "$BOARD_PART"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN"'i\ \ \ \ <dir src="data/boards/board_files/e310"></dir>' "$BOARD_PART"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN"'i\ \ </device_rule>' "$BOARD_PART"
		fi
		
		PART_INFO="$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/sysgen/xlpartinfo.at"
		CHECK_PRE=$(grep 'e310' "$PART_INFO")
		if [ -z "$CHECK_PRE" ]; then
			WRITE_LN=$(grep -i -n 'xilinx_com_zc702_part0_1_4' "$PART_INFO" | awk -F : '{printf $1}')
			WRITE_LN=$(($WRITE_LN+10))
			sed -i "$WRITE_LN""i\ \ \ \ 'ettus_com_e310_part0_1_0' => {" "$PART_INFO"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN""i\ \ \ \ \ \ 'Vendor' => 'ettus.com'," "$PART_INFO"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN""i\ \ \ \ \ \ 'Version' => '1.0'," "$PART_INFO"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN""i\ \ \ \ \ \ 'BoardName' => 'e310'," "$PART_INFO"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN""i\ \ \ \ \ \ 'BoardPartName' => 'xc7z020clg484-3'," "$PART_INFO"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN""i\ \ \ \ \ \ 'DisplayName' => 'Ettus E310'," "$PART_INFO"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN""i\ \ \ \ \ \ 'CompatiblieBoardRevisions' => [" "$PART_INFO"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN""i\ \ \ \ \ \ \ \ '1.0'," "$PART_INFO"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN""i\ \ \ \ \ \ ]," "$PART_INFO"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN""i\ \ \ \ }," "$PART_INFO"
		fi
		
		INSTALLED_DEVICES_PART="$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/parts/installed_devices.txt"
		CHECK_PRE=$(grep 'e310' "$INSTALLED_DEVICES_PART")
		if [ -z "$CHECK_PRE" ]; then
			WRITE_LN=$(grep -i -n 'xilinx_com_zc702_part0_1_4' "$INSTALLED_DEVICES_PART" | awk -F : '{printf $1}')
			WRITE_LN=$(($WRITE_LN+10))
			sed -i "$WRITE_LN""i\ \ \ \ 'ettus_com_e310_part0_1_0' => {" "$INSTALLED_DEVICES_PART"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN""i\ \ \ \ \ \ 'Vendor' => 'ettus.com'," "$INSTALLED_DEVICES_PART"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN""i\ \ \ \ \ \ 'Version' => '1.0'," "$INSTALLED_DEVICES_PART"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN""i\ \ \ \ \ \ 'BoardName' => 'e310'," "$INSTALLED_DEVICES_PART"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN""i\ \ \ \ \ \ 'BoardPartName' => 'xc7z020clg484-3'," "$INSTALLED_DEVICES_PART"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN""i\ \ \ \ \ \ 'DisplayName' => 'Ettus E310'," "$INSTALLED_DEVICES_PART"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN""i\ \ \ \ \ \ 'CompatiblieBoardRevisions' => [" "$INSTALLED_DEVICES_PART"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN""i\ \ \ \ \ \ \ \ '1.0'," "$INSTALLED_DEVICES_PART"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN""i\ \ \ \ \ \ ]," "$INSTALLED_DEVICES_PART"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN""i\ \ \ \ }," "$INSTALLED_DEVICES_PART"
		fi
	fi

	if [[ "$response_main" == *"2"* ]]; then
		echo "----Add Project to Analog Device HDL Repository----"
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

		XILINX_TCL="$ADI_HDL_PROJECT/projects/scripts/adi_project_xilinx.tcl"
		CHECK_PRE=$(grep 'e310' "$XILINX_TCL")
		if [ -z "$CHECK_PRE" ]; then
			WRITE_LN=$(grep -i -n 'regexp "_zc702$" $project_name' "$XILINX_TCL" | awk -F : '{printf $1}')
			WRITE_LN=$(($WRITE_LN+5))
			sed -i "$WRITE_LN"'i\ \ if [regexp "usrpe31x$" $project_name] {' "$XILINX_TCL"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN"'i\ \ \ \ set p_device "xc7z020clg484-3"' "$XILINX_TCL"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN"'i\ \ \ \ set p_board "ettus.com:e310:part0:1.0"' "$XILINX_TCL"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN"'i\ \ \ \ set sys_zynq 1' "$XILINX_TCL"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN"'i\ \ }' "$XILINX_TCL"
		fi
	fi

	if [[ "$response_main" == *"3"* ]]; then
		echo "---------------Synthesize projects-----------------"
		cd "$ADI_HDL_PROJECT/projects/usrpe31x"
		export PATH="$PATH:$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/bin"
		make
	fi
	
	# Modify Meta-ADI project

	if [[ "$response_main" == *"4"* ]]; then
		echo "-------------Modify Meta-ADI project---------------"
		CORE_BB="$META_ADI_PROJECT/meta-adi-xilinx/recipes-core/images/petalinux-user-image.bbappend"
		CHECK_PRE=$(grep "# EXTRA_USERS_PARAMS" "$CORE_BB")
		if [ -z "$CHECK_PRE" ]; then
			sed -i 's/EXTRA_USERS_PARAMS = "	\\/# EXTRA_USERS_PARAMS = "	\\/' "$CORE_BB"
			sed -i 's/	usermod -P analog root;"/#	usermod -P analog root;"/' "$CORE_BB"
		fi
		DEVICE_TREE_BB="$META_ADI_PROJECT/meta-adi-xilinx/recipes-bsp/device-tree/device-tree.bbappend"
		CHECK_PRE=$(grep "file://pl-delete-nodes-zynq-ettus.dtsi" "$DEVICE_TREE_BB")
		if [ -z "$CHECK_PRE" ]; then
			WRITE_LN=$(grep -i -n "file://pl-delete-nodes-vc707_fmcjesdadc1.dtsi" "$DEVICE_TREE_BB" | awk -F : '{printf $1}')
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN"'i\    file://pl-delete-nodes-zynq-ettus.dtsi \\' "$DEVICE_TREE_BB"
		fi
		CHECK_PRE=$(grep 'zynq-e310-ad9361-fmcomms2-3 \\' "$DEVICE_TREE_BB")
		if [ -z "$CHECK_PRE" ]; then
			WRITE_LN=$(grep -i -n 'zynq-adrv9361-z7035-box \\' "$DEVICE_TREE_BB" | awk -F : '{printf $1}')
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN"'i\            zynq-e310-ad9361-fmcomms2-3 \\' "$DEVICE_TREE_BB"
		fi

		#fix echo bug
		sed -i 's/^KERNEL_DTB = .*/KERNEL_DTB = "zynq-ettus"/' "$DEVICE_TREE_BB"
		sed -i 's/echo -e/echo/' "$DEVICE_TREE_BB"
		sed -i 's/echo -e/echo/' "$DEVICE_TREE_BB"
		KERNEL_BB="$META_ADI_PROJECT/meta-adi-xilinx/recipes-kernel/linux/linux-xlnx_%.bbappend"
	#	sed -i 's/SRCREV = "${AUTOREV}"/SRCREV = "6184afd426f0eb2d0fa588da8fe2e21975b18c6f"/' "$KERNEL_BB"
		cp "$CURRENT_DIR/Meta-ADI/linux-xlnx_%.bbappend" "$META_ADI_PROJECT/meta-adi-xilinx/recipes-kernel/linux/"
		sed -i 's/^KBUILD_DEFCONFIG_zynq = .*/KBUILD_DEFCONFIG_zynq = "zynq_e310_defconfig"/' "$KERNEL_BB"
		sed -i "s|LINUX_KERNEL_DIRECTORY|$LINUX_KERNEL_DIR|g" "$KERNEL_BB"
		sed -i 's|SRC_URI.*|SRC_URI = "git://'"$PNA_LINUX_DIR"';protocol=file"|' "$KERNEL_BB"

#		cd "$CURRENT_DIR"
		cp Meta-ADI/pl-delete-nodes-zynq-e310.dtsi "$META_ADI_PROJECT/meta-adi-xilinx/recipes-bsp/device-tree/files/"
	#	file "zynq-zc702-adv7511-ad9361-fmcomms2-3.dts" is not necessary
	fi

	if [[ "$response_main" == *"5"* ]]; then
		echo "------------Create Petalinux project---------------"
		printf "Enter Petalinux project name?[$PETALINUX_PROJECT]: "
		read response

		if [ -n "$response" ]; then
			PETALINUX_PROJECT="$response"
		fi
		
		printf "Add support for offline build?[y/N]: "
		read response_offline
		
		if [ "$response_offline" = "y" ]; then
			printf "Modify Meta-ADI to support offline?[y/N]: "
			read response_meta_offline
		fi
		
		printf "Need any modification in petalinux-config?(if no, continues with oldconfig)[y/N]: "
		read response_oldconfig
		
		printf "Config Dynamic ip for PetaLinux?[y/N]: "
		read response_dynamic_ip

		cd "$PETALINUX_INSTALL_DIR"
		source settings.sh
		petalinux-create --type project --template zynq --name $PETALINUX_PROJECT
		cd $PETALINUX_PROJECT
		
#	    petalinux-config --get-hw-description="$ADI_HDL_PROJECT/projects/ettus/e310/ettus_e310.sdk"
		if [ "$response_oldconfig" = "y" ]; then
			petalinux-config --get-hw-description="$ADI_HDL_PROJECT/projects/usrpe31x/usrpe31x.sdk"
		else
			petalinux-config --oldconfig --get-hw-description="$ADI_HDL_PROJECT/projects/usrpe31x/usrpe31x.sdk"
		fi
		
		ROOTFS_CONFIG="$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/configs/rootfs_config"
		CHECK_PRE=$(grep "CONFIG_imagefeature-debug-tweaks=y" "$ROOTFS_CONFIG")
		if [ -z "$CHECK_PRE" ]; then
			WRITE_LN=$(grep -i -n '# CONFIG_peekpoke is not set' "$ROOTFS_CONFIG" | awk -F : '{printf $1}')
			sed -i 's/# CONFIG_imagefeature-debug-tweaks is not set/CONFIG_imagefeature-debug-tweaks=y/' "$ROOTFS_CONFIG"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN"'iCONFIG_pna-iio=y' "$ROOTFS_CONFIG"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN"'iCONFIG_pna-startup=y' "$ROOTFS_CONFIG"
			WRITE_LN=$(($WRITE_LN+2))
			sed -i "$WRITE_LN"'i#' "$ROOTFS_CONFIG"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN"'i# modules' "$ROOTFS_CONFIG"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN"'i#' "$ROOTFS_CONFIG"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN"'iCONFIG_pnadmc=y' "$ROOTFS_CONFIG"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN"'iCONFIG_pnadmm=y' "$ROOTFS_CONFIG"
		fi

		SPEC_CONFIG="$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/configs/config"
		CHECK_PRE=$(grep "$META_ADI_PROJECT" "$SPEC_CONFIG")
		if [ -z "$CHECK_PRE" ]; then
			WRITE_LN=$(grep -i -n 'CONFIG_USER_LAYER_0=""' "$SPEC_CONFIG" | awk -F : '{printf $1}')
			sed -i 's/CONFIG_USER_LAYER_0=""/ /' "$SPEC_CONFIG"
			sed -i "$WRITE_LN"'iCONFIG_USER_LAYER_0="'"$META_ADI_PROJECT"'/meta-adi-core"' "$SPEC_CONFIG"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN"'iCONFIG_USER_LAYER_1="'"$META_ADI_PROJECT"'/meta-adi-xilinx"' "$SPEC_CONFIG"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN"'iCONFIG_USER_LAYER_2=""' "$SPEC_CONFIG"
			UART_115200="CONFIG_SUBSYSTEM_SERIAL_PS7_UART_1_BAUDRATE_115200"
			UART_921600="CONFIG_SUBSYSTEM_SERIAL_PS7_UART_1_BAUDRATE_921600"
			sed -i "s/$UART_115200=y/# $UART_115200 is not set/" "$SPEC_CONFIG"
			sed -i "s/# $UART_921600 is not set/$UART_921600=y/" "$SPEC_CONFIG"
		fi

		PETA_BB="$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/meta-user/recipes-core/images/petalinux-image-full.bbappend"
		CHECK_PRE=$(grep 'IMAGE_INSTALL_append = " pna-iio"' "$PETA_BB")
		if [ -z "$CHECK_PRE" ]; then
			WRITE_LN=$(grep -i -n 'IMAGE_INSTALL_append = " gpio-demo"' "$PETA_BB" | awk -F : '{printf $1}')
			sed -i "$WRITE_LN"'iIMAGE_INSTALL_append = " pna-iio"' "$PETA_BB"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN"'iIMAGE_INSTALL_append = " pna-startup"' "$PETA_BB"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN"'iIMAGE_INSTALL_append = " pnadmc"' "$PETA_BB"
		fi

		cd "$CURRENT_DIR"
		DIR="$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/meta-user/recipes-modules"
		if [ ! -d "$DIR" ]; then
			mkdir "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/meta-user/recipes-modules"
		fi
		cp -R Petalinux/pna-iio/ "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/meta-user/recipes-apps/pna-iio/"
		cp -R Petalinux/pnadmc/ "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/meta-user/recipes-modules/pnadmc/"
		cp -R Petalinux/pna-startup/ "$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/meta-user/recipes-apps/pna-startup/"

		# config static/dynamic ip
		STARTUP_BB="$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/project-spec/meta-user/recipes-apps/pna-startup/pna-startup.bb"
		ETH_BB="$PETALINUX_INSTALL_DIR/components/yocto/source/arm/layers/core/meta/recipes-core/init-ifupdown/init-ifupdown_1.0.bb"
		if [ "$response_dynamic_ip" = "y" ]; then
			sed -i 's/install -m 0644 interfaces/#install -m 0644 interfaces/' "$STARTUP_BB"
			sed -i 's|#install -m 0644 ${WORKDIR}/interfaces|install -m 0644 ${WORKDIR}/interfaces|' "$ETH_BB"
		else
			sed -i 's/#install -m 0644 interfaces/install -m 0644 interfaces/' "$STARTUP_BB"
			sed -i 's|install -m 0644 ${WORKDIR}/interfaces|#install -m 0644 ${WORKDIR}/interfaces|' "$ETH_BB"
		fi

		# add support for offline build
		LOCAL_CONF="$PETALINUX_INSTALL_DIR/$PETALINUX_PROJECT/build/conf/local.conf"
		if [ "$response_offline" = "y" ]; then
			echo 'DL_DIR = "'"$PETALINUX_INSTALL_DIR"'/mirror/downloads"' >> "$LOCAL_CONF"
			echo 'SSTATE_DIR = "'"$PETALINUX_INSTALL_DIR"'/mirror/sstate-cache"' >> "$LOCAL_CONF"
			echo 'BB_NO_NETWORK = "1"' >> "$LOCAL_CONF"
			echo '# BB_GENERATE_MIRROR_TARBALLS = "1"' >> "$LOCAL_CONF"

			if [ "$response_meta_offline" = "y" ]; then
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
