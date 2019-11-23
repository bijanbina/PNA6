#! /bin/bash

XILINX_INSTALL_DIR="/mnt/hdd/Vivado"
PETALINUX_INSTALL_DIR="/mnt/hdd/PetaLinux"
XILINX_VERSION="2018.3"
ADI_HDL_PROJECT="/home/bijan/Projects/ADI/hdl"
META_ADI_PROJECT="/home/bijan/Projects/ADI/meta-adi"
CURRENT_DIR=$(pwd)

printf "Install PNA6 board files in Vivado?[y/N]: "
read response

if [ "$response" = "y" ]; then
	cp BoardFiles/component.xml "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/ip/xilinx/processing_system7_v5_5/component.xml"
	cp BoardFiles/PNA6.tcl "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/ip/xilinx/processing_system7_v5_5/preset/PNA6.tcl"
	cp -R BoardFiles/pna6 "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/boards/board_files/pna6"

	CHECK_PRE=$(grep 'pna6' "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/boards/boards_parts.IDR")
	#echo "check : $CHECK_PRE"
	if [ -z "$CHECK_PRE" ]; then
		WRITE_LN=$(grep -i -n '<dir src="data/boards/board_parts/zynq/zc702"></dir>' "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/boards/boards_parts.IDR" | awk -F : '{printf $1}')
		WRITE_LN=$(($WRITE_LN+2))
		sed -i "$WRITE_LN"'i\ \ <device_rule device="xc7z020" part="xc7z020clg484-3">' "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/boards/boards_parts.IDR"
		WRITE_LN=$(($WRITE_LN+1))
		sed -i "$WRITE_LN"'i\ \ \ \ <dir src="data/boards/board_files/pna6"></dir>' "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/boards/boards_parts.IDR"
		WRITE_LN=$(($WRITE_LN+1))
		sed -i "$WRITE_LN"'i\ \ </device_rule>' "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/boards/boards_parts.IDR"
	fi
fi

printf "Add PNA6 Project to Analog Device HDL Repository?[y/N]: "
read response

if [ "$response" = "y" ]; then
	DIR="$ADI_HDL_PROJECT/projects/ddrless"
	if [ ! -d "$DIR" ]; then
		mkdir "$ADI_HDL_PROJECT/projects/ddrless"
	fi

	cp -R HDL/pna6 "$ADI_HDL_PROJECT/projects/common/pna6"
	cp -R HDL/ddrless_pna6 "$ADI_HDL_PROJECT/projects/ddrless/pna6"
	cp -R HDL/common "$ADI_HDL_PROJECT/projects/ddrless/common"

	CHECK_PRE=$(grep 'pna6' "$ADI_HDL_PROJECT/projects/scripts/adi_project_xilinx.tcl")
	#echo "check2 : $CHECK_PRE"
	if [ -z "$CHECK_PRE" ]; then
		WRITE_LN=$(grep -i -n 'regexp "_zc702$" $project_name' "$ADI_HDL_PROJECT/projects/scripts/adi_project_xilinx.tcl" | awk -F : '{printf $1}')
		WRITE_LN=$(($WRITE_LN+5))
		sed -i "$WRITE_LN"'i\ \ if [regexp "_pna6$" $project_name] {' "$ADI_HDL_PROJECT/projects/scripts/adi_project_xilinx.tcl"
		WRITE_LN=$(($WRITE_LN+1))
		sed -i "$WRITE_LN"'i\ \ \ \ set p_device "xc7z020clg484-3"' "$ADI_HDL_PROJECT/projects/scripts/adi_project_xilinx.tcl"
		WRITE_LN=$(($WRITE_LN+1))
		sed -i "$WRITE_LN"'i\ \ \ \ set p_board "binaee:pna6:part0:1.0"' "$ADI_HDL_PROJECT/projects/scripts/adi_project_xilinx.tcl"
		WRITE_LN=$(($WRITE_LN+1))
		sed -i "$WRITE_LN"'i\ \ \ \ set sys_zynq 1' "$ADI_HDL_PROJECT/projects/scripts/adi_project_xilinx.tcl"
		WRITE_LN=$(($WRITE_LN+1))
		sed -i "$WRITE_LN"'i\ \ }' "$ADI_HDL_PROJECT/projects/scripts/adi_project_xilinx.tcl"
	fi
fi

printf "Add FFT Project to Analog Device HDL Repository?[y/N]: "
read response

if [ "$response" = "y" ]; then
	DIR="$ADI_HDL_PROJECT/projects/fft"
	if [ ! -d "$DIR" ]; then
		mkdir "$ADI_HDL_PROJECT/projects/fft"
	fi
	DIR="$ADI_HDL_PROJECT/projects/fft/zc702"
	if [ ! -d "$DIR" ]; then
		mkdir "$ADI_HDL_PROJECT/projects/fft/zc702"
	fi
	DIR="$ADI_HDL_PROJECT/projects/fft/common"
	if [ ! -d "$DIR" ]; then
		mkdir "$ADI_HDL_PROJECT/projects/fft/common"
	fi
	DIR="$ADI_HDL_PROJECT/library/edge_detect"
	if [ ! -d "$DIR" ]; then
		mkdir "$ADI_HDL_PROJECT/library/edge_detect"
	fi

	cp HDL/fft/Makefile "$ADI_HDL_PROJECT/projects/fft/zc702"
	cp HDL/fft/system_bd.tcl "$ADI_HDL_PROJECT/projects/fft/zc702"
	cp HDL/fft/system_constr.xdc "$ADI_HDL_PROJECT/projects/fft/zc702"
	cp HDL/fft/system_project.tcl "$ADI_HDL_PROJECT/projects/fft/zc702"
	cp HDL/fft/system_top.v "$ADI_HDL_PROJECT/projects/fft/zc702"
	
	cp HDL/fft/fft_qsys.tcl "$ADI_HDL_PROJECT/projects/fft/common"
	cp HDL/fft/fft_bd.tcl "$ADI_HDL_PROJECT/projects/fft/common"
	
	cp HDL/fft/edge_detect.v "$ADI_HDL_PROJECT/library/edge_detect"
fi

printf "synthesize FFT project?[y/N]: "
read response

if [ "$response" = "y" ]; then
	cd "$ADI_HDL_PROJECT/projects/fft/zc702"
	export PATH="$PATH:$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/bin"
	make
fi

printf "Modify Meta-ADI project?[y/N]: "
read response

if [ "$response" = "y" ]; then
	CHECK_PRE=$(grep "# EXTRA_USERS_PARAMS" "$META_ADI_PROJECT/meta-adi-xilinx/recipes-core/images/petalinux-user-image.bbappend")
	if [ -z "$CHECK_PRE" ]; then
		sed -i 's/EXTRA_USERS_PARAMS = "	\\/# EXTRA_USERS_PARAMS = "	\\/' "$META_ADI_PROJECT/meta-adi-xilinx/recipes-core/images/petalinux-user-image.bbappend"
		sed -i 's/	usermod -P analog root;"/#	usermod -P analog root;"/' "$META_ADI_PROJECT/meta-adi-xilinx/recipes-core/images/petalinux-user-image.bbappend"
	fi
	CHECK_PRE=$(grep "file://pl-delete-nodes-zynq-zc702-adv7511-ad9361-fmcomms2-3.dtsi" "$META_ADI_PROJECT/meta-adi-xilinx/recipes-bsp/device-tree/device-tree.bbappend")
	if [ -z "$CHECK_PRE" ]; then
		WRITE_LN=$(grep -i -n "file://pl-delete-nodes-vc707_fmcjesdadc1.dtsi" "$META_ADI_PROJECT/meta-adi-xilinx/recipes-bsp/device-tree/device-tree.bbappend" | awk -F : '{printf $1}')
		WRITE_LN=$(($WRITE_LN+1))
		sed -i "$WRITE_LN"'i\ \ \ \ file://pl-delete-nodes-zynq-zc702-adv7511-ad9361-fmcomms2-3.dtsi \\' "$META_ADI_PROJECT/meta-adi-xilinx/recipes-bsp/device-tree/device-tree.bbappend"
	fi
	CHECK_PRE=$(grep "zynq-zc702-adv7511-ad9361-fmcomms2-3" "$META_ADI_PROJECT/meta-adi-xilinx/recipes-bsp/device-tree/device-tree.bbappend")
	if [ -z "$CHECK_PRE" ]; then
		WRITE_LN=$(grep -i -n 'zynq-adrv9361-z7035-box \' "$META_ADI_PROJECT/meta-adi-xilinx/recipes-bsp/device-tree/device-tree.bbappend" | awk -F : '{printf $1}')
		WRITE_LN=$(($WRITE_LN+1))
		sed -i "$WRITE_LN"'i\ \ \ \ \ \ \ \ \ \ \ \ zynq-zc702-adv7511-ad9361-fmcomms2-3 \' "$META_ADI_PROJECT/meta-adi-xilinx/recipes-bsp/device-tree/device-tree.bbappend"
	fi
	
	#FIXME: fix echo bug
	sed -i 's/KERNEL_DTB = "zynq-zed-adv7511-ad9361-fmcomms2-3"/KERNEL_DTB = "zynq-zc702-adv7511-ad9361-fmcomms2-3"/' "$META_ADI_PROJECT/meta-adi-xilinx/recipes-bsp/device-tree/device-tree.bbappend"
	sed -i 's/echo -e/echo/' "$META_ADI_PROJECT/meta-adi-xilinx/recipes-bsp/device-tree/device-tree.bbappend"
	sed -i 's/echo -e/echo/' "$META_ADI_PROJECT/meta-adi-xilinx/recipes-bsp/device-tree/device-tree.bbappend"
	
	cd "$CURRENT_DIR" 
	cp -R Meta-ADI/* "$META_ADI_PROJECT/meta-adi-xilinx/recipes-bsp/device-tree/files/"
fi

printf "Create Petalinux project?[y/N]: "
read response

if [ "$response" = "y" ]; then
	cd "$PETALINUX_INSTALL_DIR"
	source settings.sh
	petalinux-create --type project --template zynq --name fft_zc702_linux
	cd fft_zc702_linux
	petalinux-config --oldconfig --get-hw-description="$ADI_HDL_PROJECT/projects/fft/zc702/fft_zc702.sdk"
#	petalinux-config --oldconfig --get-hw-description="$ADI_HDL_PROJECT/projects/fmcomms2/zc702/fmcomms2_zc702.sdk"
	
	CHECK_PRE=$(grep "$META_ADI_PROJECT" "$PETALINUX_INSTALL_DIR/fft_zc702_linux/project-spec/configs/config")
	if [ -z "$CHECK_PRE" ]; then
		WRITE_LN=$(grep -i -n 'CONFIG_USER_LAYER_0=""' "$PETALINUX_INSTALL_DIR/fft_zc702_linux/project-spec/configs/config" | awk -F : '{printf $1}')
		sed -i 's/CONFIG_USER_LAYER_0=""/ /' "$PETALINUX_INSTALL_DIR/fft_zc702_linux/project-spec/configs/config"
		sed -i "$WRITE_LN"'iCONFIG_USER_LAYER_0="'"$META_ADI_PROJECT"'/meta-adi-core"' "$PETALINUX_INSTALL_DIR/fft_zc702_linux/project-spec/configs/config"
		WRITE_LN=$(($WRITE_LN+1))
		sed -i "$WRITE_LN"'iCONFIG_USER_LAYER_1="'"$META_ADI_PROJECT"'/meta-adi-xilinx"' "$PETALINUX_INSTALL_DIR/fft_zc702_linux/project-spec/configs/config"
		WRITE_LN=$(($WRITE_LN+1))
		sed -i "$WRITE_LN"'iCONFIG_USER_LAYER_2=""' "$PETALINUX_INSTALL_DIR/fft_zc702_linux/project-spec/configs/config"
	fi
	
	CHECK_PRE=$(grep 'IMAGE_INSTALL_append = " pna-iio"' "$PETALINUX_INSTALL_DIR/fft_zc702_linux/project-spec/meta-user/recipes-core/images/petalinux-image-full.bbappend")
	if [ -z "$CHECK_PRE" ]; then
		WRITE_LN=$(grep -i -n 'IMAGE_INSTALL_append = " gpio-demo"' "$PETALINUX_INSTALL_DIR/fft_zc702_linux/project-spec/meta-user/recipes-core/images/petalinux-image-full.bbappend" | awk -F : '{printf $1}')
		sed -i "$WRITE_LN"'iIMAGE_INSTALL_append = " pna-iio"' "$PETALINUX_INSTALL_DIR/fft_zc702_linux/project-spec/meta-user/recipes-core/images/petalinux-image-full.bbappend"
		WRITE_LN=$(($WRITE_LN+1))
		sed -i "$WRITE_LN"'iIMAGE_INSTALL_append = " pnadmc"' "$PETALINUX_INSTALL_DIR/fft_zc702_linux/project-spec/meta-user/recipes-core/images/petalinux-image-full.bbappend"
	fi
	
	cd "$CURRENT_DIR"
	DIR="$PETALINUX_INSTALL_DIR/fft_zc702_linux/project-spec/meta-user/recipes-modules"
	if [ ! -d "$DIR" ]; then
		mkdir "$PETALINUX_INSTALL_DIR/fft_zc702_linux/project-spec/meta-user/recipes-modules"
	fi
	cp -R Petalinux/pna-iio/ "$PETALINUX_INSTALL_DIR/fft_zc702_linux/project-spec/meta-user/recipes-apps/pna-iio/"
	cp -R Petalinux/pnadmc/ "$PETALINUX_INSTALL_DIR/fft_zc702_linux/project-spec/meta-user/recipes-modules/pnadmc/"
	
	cd "$PETALINUX_INSTALL_DIR/fft_zc702_linux"
	petalinux-build
fi
