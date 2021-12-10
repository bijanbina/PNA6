#! /bin/bash

source Scripts/common.sh

CURRENT_DIR=$(pwd)

while true; do
	pna_printPrompt "--ZC702 FFT-"
	read response_main

	if [[ "$response_main" == *"1"* ]]; then
		echo "----------Install board files in Vivado------------"
		echo "Board files are already added for ZC702 board"
	fi
	
	if [[ "$response_main" == *"2"* ]]; then
		echo "----Add Project to Analog Device HDL Repository----"
		cd $CURRENT_DIR

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

		cp HDL/fft/zc702/Makefile 			"$ADI_HDL_PROJECT/projects/fft/zc702"
		cp HDL/fft/zc702/system_bd.tcl 		"$ADI_HDL_PROJECT/projects/fft/zc702"
		cp HDL/fft/zc702/system_constr.xdc 	"$ADI_HDL_PROJECT/projects/fft/zc702"
		cp HDL/fft/zc702/system_project.tcl "$ADI_HDL_PROJECT/projects/fft/zc702"
		cp HDL/fft/zc702/system_top.v 		"$ADI_HDL_PROJECT/projects/fft/zc702"

		cp HDL/fft/common/fft_qsys.tcl 		"$ADI_HDL_PROJECT/projects/fft/common"
		cp HDL/fft/common/fft_bd.tcl 		"$ADI_HDL_PROJECT/projects/fft/common"

		cp HDL/common/fft/edge_detect.v 					"$ADI_HDL_PROJECT/library/edge_detect"

		cp HDL/common/zc702/zc702_no_hdmi_system_bd.tcl 	"$ADI_HDL_PROJECT/projects/common/zc702"
		cp HDL/common/zc702/zc702_no_hdmi_system_constr.xdc "$ADI_HDL_PROJECT/projects/common/zc702"
	fi

	if [[ "$response_main" == *"3"* ]]; then
		echo "---------------Synthesize projects-----------------"
		cd "$ADI_HDL_PROJECT/projects/fft/zc702"
		export PATH="$PATH:$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/bin"
		make
	fi
	
	# Modify Meta-ADI project

	if [[ "$response_main" == *"4"* ]]; then
		echo "-------------Modify Meta-ADI project---------------"
		pna_modifyMeta "zc702-fft"
	fi

	if [[ "$response_main" == *"5"* ]]; then
		echo "------------Create Petalinux project---------------"
		cd "$PETALINUX_INSTALL_DIR"
		source settings.sh
		pna_createPetalinux "fft/zc702/fft_zc702.sdk" "ZC702" "19"
		# fft_fast_sqrt/zc702/fft_zc702.sdk
	fi
	
	if [[ "$response_main" == *"0"* ]]; then
		break
	fi
done
