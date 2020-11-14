#! /bin/bash

source Scripts/common.sh

CURRENT_DIR=$(pwd)

while true; do
	pna_printPrompt "-ETTUS E310-"
	read response_main

	if [[ "$response_main" == *"1"* ]]; then
		echo "----------Install board files in Vivado------------"
		cd $CURRENT_DIR
		
		pna_installBoard "E310"
		
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
		pna_modifyMeta "e310"
	fi

	if [[ "$response_main" == *"5"* ]]; then
		echo "------------Create Petalinux project---------------"
		cd "$PETALINUX_INSTALL_DIR"
		source settings.sh
		pna_createPetalinux "usrpe31x/usrpe31x.sdk"
		# ettus/e310/ettus_e310.sdk
	fi
	
	if [[ "$response_main" == *"0"* ]]; then
		break
	fi
done
