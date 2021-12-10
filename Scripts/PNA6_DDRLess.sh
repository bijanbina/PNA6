#! /bin/bash

source Scripts/common.sh

CURRENT_DIR=$(pwd)

while true; do
	pna_printPrompt "PNA_DDRLESS-"
	read response_main

	if [[ "$response_main" == *"1"* ]]; then
		echo "----------Install board files in Vivado------------"
		cd $CURRENT_DIR

		pna_installBoard "PNA6"
	fi

	if [[ "$response_main" == *"2"* ]]; then
		echo "----Add Project to Analog Device HDL Repository----"
		cd $CURRENT_DIR
		
		DIR="$ADI_HDL_PROJECT/projects/pna6"
		if [ ! -d "$DIR" ]; then
			mkdir "$ADI_HDL_PROJECT/projects/pna6"
		fi
		DIR="$ADI_HDL_PROJECT/projects/pna6/ddrless"
		if [ ! -d "$DIR" ]; then
			mkdir "$ADI_HDL_PROJECT/projects/pna6/ddrless"
		fi
		DIR="$ADI_HDL_PROJECT/projects/pna6/common"
		if [ ! -d "$DIR" ]; then
			mkdir "$ADI_HDL_PROJECT/projects/pna6/common"
		fi
		cp -R HDL/common/pna6 "$ADI_HDL_PROJECT/projects/common"
		cp -R HDL/pna6/ddrless "$ADI_HDL_PROJECT/projects/pna6"
		cp -R HDL/pna6/common "$ADI_HDL_PROJECT/projects/pna6"

		XILINX_TCL="$ADI_HDL_PROJECT/projects/scripts/adi_project_xilinx.tcl"
		CHECK_PRE=$(grep 'pna6' "$XILINX_TCL")
		#echo "check2 : $CHECK_PRE"
		if [ -z "$CHECK_PRE" ]; then
			WRITE_LN=$(grep -i -n 'regexp "_zc702$" $project_name' "$XILINX_TCL" | awk -F : '{printf $1}')
			WRITE_LN=$(($WRITE_LN+5))
			sed -i "$WRITE_LN"'i\ \ if [regexp "_pna6$" $project_name] {' "$XILINX_TCL"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN"'i\ \ \ \ set p_device "xc7z020clg484-3"' "$XILINX_TCL"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN"'i\ \ \ \ set p_board "binaee:pna6:part0:1.0"' "$XILINX_TCL"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN"'i\ \ \ \ set sys_zynq 1' "$XILINX_TCL"
			WRITE_LN=$(($WRITE_LN+1))
			sed -i "$WRITE_LN"'i\ \ }' "$XILINX_TCL"
		fi

	fi

	if [[ "$response_main" == *"3"* ]]; then
		echo "---------------Synthesize projects-----------------"
		cd "$ADI_HDL_PROJECT/projects/pna6/ddrless"
		export PATH="$PATH:$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/bin"
		make
	fi
	# Modify Meta-ADI project

	if [[ "$response_main" == *"4"* ]]; then
		echo "-------------Modify Meta-ADI project---------------"
		pna_modifyMeta "pna6"
	fi

	if [[ "$response_main" == *"5"* ]]; then
		echo "------------Create Petalinux project---------------"
		cd "$PETALINUX_INSTALL_DIR"
		source settings.sh
		pna_createPetalinux "fmcomms2/zc702/fmcomms2_zc702.sdk" "pna6" "18"
	fi
	
	if [[ "$response_main" == *"0"* ]]; then
		break
	fi
done
