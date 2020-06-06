
source ../../scripts/adi_env.tcl
source $ad_hdl_dir/projects/scripts/adi_project_xilinx.tcl
source $ad_hdl_dir/projects/scripts/adi_board.tcl

adi_project ettus_e310
adi_project_files ettus_e310 [list \
  "system_top.v" \
  "system_constr.xdc" \
  "$ad_hdl_dir/library/xilinx/common/ad_iobuf.v" \
  "$ad_hdl_dir/projects/common/e310/e310_system_constr.xdc" ]

set_property strategy Performance_Explore [get_runs impl_1]

adi_project_run ettus_e310
source $ad_hdl_dir/library/axi_ad9361/axi_ad9361_delay.tcl

