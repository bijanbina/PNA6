
source ../../scripts/adi_env.tcl
source $ad_hdl_dir/projects/scripts/adi_project_xilinx.tcl
source $ad_hdl_dir/projects/scripts/adi_board.tcl

adi_project ddrless_pna6
adi_project_files ddrless_pna6 [list \
  "system_top.v" \
  "system_constr.xdc" \
  "$ad_hdl_dir/library/xilinx/common/ad_iobuf.v" \
  "$ad_hdl_dir/projects/common/pna6/pna6_system_constr.xdc" ]

set_property strategy Performance_Explore [get_runs impl_1]

adi_project_run ddrless_pna6
source $ad_hdl_dir/library/axi_ad9361/axi_ad9361_delay.tcl

