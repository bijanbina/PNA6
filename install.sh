#! /bin/sh

XILINX_INSTALL_DIR="/mnt/hdd1/Vivado"
XILINX_VERSION="2018.3"
ADI_HDL_PROJECT="/home/esi/Projects/ADI/hdl"

cp component.xml "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/ip/xilinx/processing_system7_v5_5/component.xml"
cp PNA6.tcl "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/ip/xilinx/processing_system7_v5_5/preset/PNA6.tcl"
cp -R pna6 "$XILINX_INSTALL_DIR/Vivado/$XILINX_VERSION/data/boards/board_files/pna6"

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

DIR="$ADI_HDL_PROJECT/projects/pna6"
if [ ! -d "$DIR" ]; then
	mkdir "$ADI_HDL_PROJECT/projects/pna6"
fi

cp -R HDL/pna6 "$ADI_HDL_PROJECT/projects/common/pna6"
cp -R HDL/ddrless_pna6 "$ADI_HDL_PROJECT/projects/pna6/ddrless_pna6"
cp -R HDL/common "$ADI_HDL_PROJECT/projects/pna6/common"

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
