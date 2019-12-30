
# constraints
# hdmi

set_property -dict {PACKAGE_PIN L16 IOSTANDARD LVCMOS25} [get_ports hdmi_out_clk]
set_property -dict {PACKAGE_PIN H15 IOSTANDARD LVCMOS25 IOB TRUE} [get_ports hdmi_vsync]
set_property -dict {PACKAGE_PIN R18 IOSTANDARD LVCMOS25 IOB TRUE} [get_ports hdmi_hsync]
set_property -dict {PACKAGE_PIN T18 IOSTANDARD LVCMOS25 IOB TRUE} [get_ports hdmi_data_e]
set_property -dict {PACKAGE_PIN AB21 IOSTANDARD LVCMOS25 IOB TRUE} [get_ports {hdmi_data[0]}]
set_property -dict {PACKAGE_PIN AA21 IOSTANDARD LVCMOS25 IOB TRUE} [get_ports {hdmi_data[1]}]
set_property -dict {PACKAGE_PIN AB22 IOSTANDARD LVCMOS25 IOB TRUE} [get_ports {hdmi_data[2]}]
set_property -dict {PACKAGE_PIN AA22 IOSTANDARD LVCMOS25 IOB TRUE} [get_ports {hdmi_data[3]}]
set_property -dict {PACKAGE_PIN V19 IOSTANDARD LVCMOS25 IOB TRUE} [get_ports {hdmi_data[4]}]
set_property -dict {PACKAGE_PIN V18 IOSTANDARD LVCMOS25 IOB TRUE} [get_ports {hdmi_data[5]}]
set_property -dict {PACKAGE_PIN V20 IOSTANDARD LVCMOS25 IOB TRUE} [get_ports {hdmi_data[6]}]
set_property -dict {PACKAGE_PIN U20 IOSTANDARD LVCMOS25 IOB TRUE} [get_ports {hdmi_data[7]}]
set_property -dict {PACKAGE_PIN W21 IOSTANDARD LVCMOS25 IOB TRUE} [get_ports {hdmi_data[8]}]
set_property -dict {PACKAGE_PIN W20 IOSTANDARD LVCMOS25 IOB TRUE} [get_ports {hdmi_data[9]}]
set_property -dict {PACKAGE_PIN W18 IOSTANDARD LVCMOS25 IOB TRUE} [get_ports {hdmi_data[10]}]
set_property -dict {PACKAGE_PIN T19 IOSTANDARD LVCMOS25 IOB TRUE} [get_ports {hdmi_data[11]}]
set_property -dict {PACKAGE_PIN U19 IOSTANDARD LVCMOS25 IOB TRUE} [get_ports {hdmi_data[12]}]
set_property -dict {PACKAGE_PIN R19 IOSTANDARD LVCMOS25 IOB TRUE} [get_ports {hdmi_data[13]}]
set_property -dict {PACKAGE_PIN T17 IOSTANDARD LVCMOS25 IOB TRUE} [get_ports {hdmi_data[14]}]
set_property -dict {PACKAGE_PIN T16 IOSTANDARD LVCMOS25 IOB TRUE} [get_ports {hdmi_data[15]}]

# spdif

set_property -dict {PACKAGE_PIN R15 IOSTANDARD LVCMOS25} [get_ports spdif]

# iic

set_property PACKAGE_PIN W11 [get_ports iic_scl]
set_property IOSTANDARD LVCMOS25 [get_ports iic_scl]
set_property PULLUP true [get_ports iic_scl]
set_property PACKAGE_PIN W8 [get_ports iic_sda]
set_property IOSTANDARD LVCMOS25 [get_ports iic_sda]
set_property PULLUP true [get_ports iic_sda]

# gpio (switches, leds and such)

set_property -dict {PACKAGE_PIN G19 IOSTANDARD LVCMOS25} [get_ports {gpio_bd[0]}]
set_property -dict {PACKAGE_PIN F19 IOSTANDARD LVCMOS25} [get_ports {gpio_bd[1]}]
set_property -dict {PACKAGE_PIN W6 IOSTANDARD LVCMOS25} [get_ports {gpio_bd[2]}]
set_property -dict {PACKAGE_PIN W7 IOSTANDARD LVCMOS25} [get_ports {gpio_bd[3]}]
set_property -dict {PACKAGE_PIN H17 IOSTANDARD LVCMOS25} [get_ports {gpio_bd[4]}]
set_property -dict {PACKAGE_PIN H22 IOSTANDARD LVCMOS25} [get_ports {gpio_bd[5]}]
set_property -dict {PACKAGE_PIN G22 IOSTANDARD LVCMOS25} [get_ports {gpio_bd[6]}]
set_property -dict {PACKAGE_PIN H18 IOSTANDARD LVCMOS25} [get_ports {gpio_bd[7]}]

# Define SPI clock
create_clock -period 40.000 -name spi0_clk [get_pins -hier */EMIOSPI0SCLKO]
create_clock -period 40.000 -name spi1_clk [get_pins -hier */EMIOSPI1SCLKO]




connect_debug_port u_ila_0/probe0 [get_nets [list {i_system_wrapper/system_i/Net[0]} {i_system_wrapper/system_i/Net[1]} {i_system_wrapper/system_i/Net[2]} {i_system_wrapper/system_i/Net[3]} {i_system_wrapper/system_i/Net[4]} {i_system_wrapper/system_i/Net[5]} {i_system_wrapper/system_i/Net[6]} {i_system_wrapper/system_i/Net[7]} {i_system_wrapper/system_i/Net[8]} {i_system_wrapper/system_i/Net[9]} {i_system_wrapper/system_i/Net[10]} {i_system_wrapper/system_i/Net[11]} {i_system_wrapper/system_i/Net[12]} {i_system_wrapper/system_i/Net[13]} {i_system_wrapper/system_i/Net[14]} {i_system_wrapper/system_i/Net[15]} {i_system_wrapper/system_i/Net[16]}]]
connect_debug_port u_ila_0/probe1 [get_nets [list {i_system_wrapper/system_i/adc_fft_M_AXIS_DATA_TDATA[0]} {i_system_wrapper/system_i/adc_fft_M_AXIS_DATA_TDATA[1]} {i_system_wrapper/system_i/adc_fft_M_AXIS_DATA_TDATA[2]} {i_system_wrapper/system_i/adc_fft_M_AXIS_DATA_TDATA[3]} {i_system_wrapper/system_i/adc_fft_M_AXIS_DATA_TDATA[4]} {i_system_wrapper/system_i/adc_fft_M_AXIS_DATA_TDATA[5]} {i_system_wrapper/system_i/adc_fft_M_AXIS_DATA_TDATA[6]} {i_system_wrapper/system_i/adc_fft_M_AXIS_DATA_TDATA[7]} {i_system_wrapper/system_i/adc_fft_M_AXIS_DATA_TDATA[8]} {i_system_wrapper/system_i/adc_fft_M_AXIS_DATA_TDATA[9]} {i_system_wrapper/system_i/adc_fft_M_AXIS_DATA_TDATA[10]} {i_system_wrapper/system_i/adc_fft_M_AXIS_DATA_TDATA[11]} {i_system_wrapper/system_i/adc_fft_M_AXIS_DATA_TDATA[12]} {i_system_wrapper/system_i/adc_fft_M_AXIS_DATA_TDATA[13]} {i_system_wrapper/system_i/adc_fft_M_AXIS_DATA_TDATA[14]} {i_system_wrapper/system_i/adc_fft_M_AXIS_DATA_TDATA[15]} {i_system_wrapper/system_i/adc_fft_M_AXIS_DATA_TDATA[16]} {i_system_wrapper/system_i/adc_fft_M_AXIS_DATA_TDATA[17]} {i_system_wrapper/system_i/adc_fft_M_AXIS_DATA_TDATA[18]} {i_system_wrapper/system_i/adc_fft_M_AXIS_DATA_TDATA[19]} {i_system_wrapper/system_i/adc_fft_M_AXIS_DATA_TDATA[20]} {i_system_wrapper/system_i/adc_fft_M_AXIS_DATA_TDATA[21]} {i_system_wrapper/system_i/adc_fft_M_AXIS_DATA_TDATA[22]} {i_system_wrapper/system_i/adc_fft_M_AXIS_DATA_TDATA[23]} {i_system_wrapper/system_i/adc_fft_M_AXIS_DATA_TDATA[24]} {i_system_wrapper/system_i/adc_fft_M_AXIS_DATA_TDATA[25]} {i_system_wrapper/system_i/adc_fft_M_AXIS_DATA_TDATA[26]} {i_system_wrapper/system_i/adc_fft_M_AXIS_DATA_TDATA[27]} {i_system_wrapper/system_i/adc_fft_M_AXIS_DATA_TDATA[28]} {i_system_wrapper/system_i/adc_fft_M_AXIS_DATA_TDATA[29]} {i_system_wrapper/system_i/adc_fft_M_AXIS_DATA_TDATA[30]} {i_system_wrapper/system_i/adc_fft_M_AXIS_DATA_TDATA[31]}]]
connect_debug_port u_ila_0/probe2 [get_nets [list {i_system_wrapper/system_i/axis_data_fifo_0_M_AXIS_TDATA[0]} {i_system_wrapper/system_i/axis_data_fifo_0_M_AXIS_TDATA[1]} {i_system_wrapper/system_i/axis_data_fifo_0_M_AXIS_TDATA[2]} {i_system_wrapper/system_i/axis_data_fifo_0_M_AXIS_TDATA[3]} {i_system_wrapper/system_i/axis_data_fifo_0_M_AXIS_TDATA[4]} {i_system_wrapper/system_i/axis_data_fifo_0_M_AXIS_TDATA[5]} {i_system_wrapper/system_i/axis_data_fifo_0_M_AXIS_TDATA[6]} {i_system_wrapper/system_i/axis_data_fifo_0_M_AXIS_TDATA[7]} {i_system_wrapper/system_i/axis_data_fifo_0_M_AXIS_TDATA[8]} {i_system_wrapper/system_i/axis_data_fifo_0_M_AXIS_TDATA[9]} {i_system_wrapper/system_i/axis_data_fifo_0_M_AXIS_TDATA[10]} {i_system_wrapper/system_i/axis_data_fifo_0_M_AXIS_TDATA[11]} {i_system_wrapper/system_i/axis_data_fifo_0_M_AXIS_TDATA[12]} {i_system_wrapper/system_i/axis_data_fifo_0_M_AXIS_TDATA[13]} {i_system_wrapper/system_i/axis_data_fifo_0_M_AXIS_TDATA[14]} {i_system_wrapper/system_i/axis_data_fifo_0_M_AXIS_TDATA[15]} {i_system_wrapper/system_i/axis_data_fifo_0_M_AXIS_TDATA[16]} {i_system_wrapper/system_i/axis_data_fifo_0_M_AXIS_TDATA[17]} {i_system_wrapper/system_i/axis_data_fifo_0_M_AXIS_TDATA[18]} {i_system_wrapper/system_i/axis_data_fifo_0_M_AXIS_TDATA[19]} {i_system_wrapper/system_i/axis_data_fifo_0_M_AXIS_TDATA[20]} {i_system_wrapper/system_i/axis_data_fifo_0_M_AXIS_TDATA[21]} {i_system_wrapper/system_i/axis_data_fifo_0_M_AXIS_TDATA[22]} {i_system_wrapper/system_i/axis_data_fifo_0_M_AXIS_TDATA[23]} {i_system_wrapper/system_i/axis_data_fifo_0_M_AXIS_TDATA[24]} {i_system_wrapper/system_i/axis_data_fifo_0_M_AXIS_TDATA[25]} {i_system_wrapper/system_i/axis_data_fifo_0_M_AXIS_TDATA[26]} {i_system_wrapper/system_i/axis_data_fifo_0_M_AXIS_TDATA[27]} {i_system_wrapper/system_i/axis_data_fifo_0_M_AXIS_TDATA[28]} {i_system_wrapper/system_i/axis_data_fifo_0_M_AXIS_TDATA[29]} {i_system_wrapper/system_i/axis_data_fifo_0_M_AXIS_TDATA[30]} {i_system_wrapper/system_i/axis_data_fifo_0_M_AXIS_TDATA[31]}]]
connect_debug_port u_ila_0/probe3 [get_nets [list {i_system_wrapper/system_i/axis_data_fifo_0_axis_rd_data_count[0]} {i_system_wrapper/system_i/axis_data_fifo_0_axis_rd_data_count[1]} {i_system_wrapper/system_i/axis_data_fifo_0_axis_rd_data_count[2]} {i_system_wrapper/system_i/axis_data_fifo_0_axis_rd_data_count[3]} {i_system_wrapper/system_i/axis_data_fifo_0_axis_rd_data_count[4]} {i_system_wrapper/system_i/axis_data_fifo_0_axis_rd_data_count[5]} {i_system_wrapper/system_i/axis_data_fifo_0_axis_rd_data_count[6]} {i_system_wrapper/system_i/axis_data_fifo_0_axis_rd_data_count[7]} {i_system_wrapper/system_i/axis_data_fifo_0_axis_rd_data_count[8]} {i_system_wrapper/system_i/axis_data_fifo_0_axis_rd_data_count[9]} {i_system_wrapper/system_i/axis_data_fifo_0_axis_rd_data_count[10]} {i_system_wrapper/system_i/axis_data_fifo_0_axis_rd_data_count[11]}]]
connect_debug_port u_ila_0/probe4 [get_nets [list {i_system_wrapper/system_i/axis_data_fifo_0_axis_wr_data_count[0]} {i_system_wrapper/system_i/axis_data_fifo_0_axis_wr_data_count[1]} {i_system_wrapper/system_i/axis_data_fifo_0_axis_wr_data_count[2]} {i_system_wrapper/system_i/axis_data_fifo_0_axis_wr_data_count[3]} {i_system_wrapper/system_i/axis_data_fifo_0_axis_wr_data_count[4]} {i_system_wrapper/system_i/axis_data_fifo_0_axis_wr_data_count[5]} {i_system_wrapper/system_i/axis_data_fifo_0_axis_wr_data_count[6]} {i_system_wrapper/system_i/axis_data_fifo_0_axis_wr_data_count[7]} {i_system_wrapper/system_i/axis_data_fifo_0_axis_wr_data_count[8]} {i_system_wrapper/system_i/axis_data_fifo_0_axis_wr_data_count[9]} {i_system_wrapper/system_i/axis_data_fifo_0_axis_wr_data_count[10]} {i_system_wrapper/system_i/axis_data_fifo_0_axis_wr_data_count[11]}]]
connect_debug_port u_ila_0/probe5 [get_nets [list i_system_wrapper/system_i/adc_fft_M_AXIS_DATA_TREADY]]
connect_debug_port u_ila_0/probe6 [get_nets [list i_system_wrapper/system_i/adc_fft_M_AXIS_DATA_TVALID]]
connect_debug_port u_ila_0/probe7 [get_nets [list i_system_wrapper/system_i/axis_data_fifo_0_M_AXIS_TLAST]]
connect_debug_port u_ila_0/probe8 [get_nets [list i_system_wrapper/system_i/axis_data_fifo_0_M_AXIS_TREADY]]
connect_debug_port u_ila_0/probe9 [get_nets [list i_system_wrapper/system_i/axis_data_fifo_0_M_AXIS_TVALID]]
connect_debug_port u_ila_0/probe10 [get_nets [list i_system_wrapper/system_i/adc_fft_M_AXIS_DATA_TLAST]]

connect_debug_port u_ila_0/probe0 [get_nets [list {i_system_wrapper/system_i/^gpio_fft[0]} {i_system_wrapper/system_i/^gpio_fft[1]} {i_system_wrapper/system_i/^gpio_fft[2]} {i_system_wrapper/system_i/^gpio_fft[3]} {i_system_wrapper/system_i/^gpio_fft[4]} {i_system_wrapper/system_i/^gpio_fft[5]} {i_system_wrapper/system_i/^gpio_fft[6]} {i_system_wrapper/system_i/^gpio_fft[7]} {i_system_wrapper/system_i/^gpio_fft[8]} {i_system_wrapper/system_i/^gpio_fft[9]} {i_system_wrapper/system_i/^gpio_fft[10]} {i_system_wrapper/system_i/^gpio_fft[11]} {i_system_wrapper/system_i/^gpio_fft[12]} {i_system_wrapper/system_i/^gpio_fft[13]} {i_system_wrapper/system_i/^gpio_fft[14]} {i_system_wrapper/system_i/^gpio_fft[15]} {i_system_wrapper/system_i/^gpio_fft[16]}]]

connect_debug_port u_ila_0/probe1 [get_nets [list {i_system_wrapper/system_i/fft_output_TDATA[0]} {i_system_wrapper/system_i/fft_output_TDATA[1]} {i_system_wrapper/system_i/fft_output_TDATA[2]} {i_system_wrapper/system_i/fft_output_TDATA[3]} {i_system_wrapper/system_i/fft_output_TDATA[4]} {i_system_wrapper/system_i/fft_output_TDATA[5]} {i_system_wrapper/system_i/fft_output_TDATA[6]} {i_system_wrapper/system_i/fft_output_TDATA[7]} {i_system_wrapper/system_i/fft_output_TDATA[8]} {i_system_wrapper/system_i/fft_output_TDATA[9]} {i_system_wrapper/system_i/fft_output_TDATA[10]} {i_system_wrapper/system_i/fft_output_TDATA[11]} {i_system_wrapper/system_i/fft_output_TDATA[12]} {i_system_wrapper/system_i/fft_output_TDATA[13]} {i_system_wrapper/system_i/fft_output_TDATA[14]} {i_system_wrapper/system_i/fft_output_TDATA[15]} {i_system_wrapper/system_i/fft_output_TDATA[16]} {i_system_wrapper/system_i/fft_output_TDATA[17]} {i_system_wrapper/system_i/fft_output_TDATA[18]} {i_system_wrapper/system_i/fft_output_TDATA[19]} {i_system_wrapper/system_i/fft_output_TDATA[20]} {i_system_wrapper/system_i/fft_output_TDATA[21]} {i_system_wrapper/system_i/fft_output_TDATA[22]} {i_system_wrapper/system_i/fft_output_TDATA[23]} {i_system_wrapper/system_i/fft_output_TDATA[24]} {i_system_wrapper/system_i/fft_output_TDATA[25]} {i_system_wrapper/system_i/fft_output_TDATA[26]} {i_system_wrapper/system_i/fft_output_TDATA[27]} {i_system_wrapper/system_i/fft_output_TDATA[28]} {i_system_wrapper/system_i/fft_output_TDATA[29]} {i_system_wrapper/system_i/fft_output_TDATA[30]} {i_system_wrapper/system_i/fft_output_TDATA[31]}]]
connect_debug_port u_ila_0/probe8 [get_nets [list i_system_wrapper/system_i/fft_output_TLAST]]
connect_debug_port u_ila_0/probe9 [get_nets [list i_system_wrapper/system_i/fft_output_TREADY]]
connect_debug_port u_ila_0/probe10 [get_nets [list i_system_wrapper/system_i/fft_output_TVALID]]

create_debug_core u_ila_0 ila
set_property ALL_PROBE_SAME_MU true [get_debug_cores u_ila_0]
set_property ALL_PROBE_SAME_MU_CNT 4 [get_debug_cores u_ila_0]
set_property C_ADV_TRIGGER true [get_debug_cores u_ila_0]
set_property C_DATA_DEPTH 4096 [get_debug_cores u_ila_0]
set_property C_EN_STRG_QUAL false [get_debug_cores u_ila_0]
set_property C_INPUT_PIPE_STAGES 0 [get_debug_cores u_ila_0]
set_property C_TRIGIN_EN false [get_debug_cores u_ila_0]
set_property C_TRIGOUT_EN false [get_debug_cores u_ila_0]
set_property port_width 1 [get_debug_ports u_ila_0/clk]
connect_debug_port u_ila_0/clk [get_nets [list i_system_wrapper/system_i/sys_ps7/inst/FCLK_CLK0]]
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe0]
set_property port_width 64 [get_debug_ports u_ila_0/probe0]
connect_debug_port u_ila_0/probe0 [get_nets [list {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[0]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[1]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[2]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[3]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[4]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[5]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[6]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[7]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[8]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[9]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[10]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[11]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[12]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[13]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[14]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[15]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[16]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[17]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[18]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[19]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[20]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[21]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[22]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[23]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[24]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[25]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[26]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[27]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[28]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[29]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[30]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[31]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[32]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[33]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[34]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[35]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[36]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[37]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[38]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[39]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[40]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[41]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[42]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[43]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[44]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[45]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[46]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[47]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[48]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[49]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[50]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[51]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[52]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[53]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[54]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[55]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[56]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[57]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[58]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[59]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[60]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[61]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[62]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TDATA[63]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe1]
set_property port_width 8 [get_debug_ports u_ila_0/probe1]
connect_debug_port u_ila_0/probe1 [get_nets [list {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TKEEP[0]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TKEEP[1]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TKEEP[2]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TKEEP[3]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TKEEP[4]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TKEEP[5]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TKEEP[6]} {i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TKEEP[7]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe2]
set_property port_width 12 [get_debug_ports u_ila_0/probe2]
connect_debug_port u_ila_0/probe2 [get_nets [list {i_system_wrapper/system_i/fifo_wr_cnt[0]} {i_system_wrapper/system_i/fifo_wr_cnt[1]} {i_system_wrapper/system_i/fifo_wr_cnt[2]} {i_system_wrapper/system_i/fifo_wr_cnt[3]} {i_system_wrapper/system_i/fifo_wr_cnt[4]} {i_system_wrapper/system_i/fifo_wr_cnt[5]} {i_system_wrapper/system_i/fifo_wr_cnt[6]} {i_system_wrapper/system_i/fifo_wr_cnt[7]} {i_system_wrapper/system_i/fifo_wr_cnt[8]} {i_system_wrapper/system_i/fifo_wr_cnt[9]} {i_system_wrapper/system_i/fifo_wr_cnt[10]} {i_system_wrapper/system_i/fifo_wr_cnt[11]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe3]
set_property port_width 12 [get_debug_ports u_ila_0/probe3]
connect_debug_port u_ila_0/probe3 [get_nets [list {i_system_wrapper/system_i/fifo_rd_cnt[0]} {i_system_wrapper/system_i/fifo_rd_cnt[1]} {i_system_wrapper/system_i/fifo_rd_cnt[2]} {i_system_wrapper/system_i/fifo_rd_cnt[3]} {i_system_wrapper/system_i/fifo_rd_cnt[4]} {i_system_wrapper/system_i/fifo_rd_cnt[5]} {i_system_wrapper/system_i/fifo_rd_cnt[6]} {i_system_wrapper/system_i/fifo_rd_cnt[7]} {i_system_wrapper/system_i/fifo_rd_cnt[8]} {i_system_wrapper/system_i/fifo_rd_cnt[9]} {i_system_wrapper/system_i/fifo_rd_cnt[10]} {i_system_wrapper/system_i/fifo_rd_cnt[11]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe4]
set_property port_width 19 [get_debug_ports u_ila_0/probe4]
connect_debug_port u_ila_0/probe4 [get_nets [list {i_system_wrapper/system_i/simaye_fft[0]} {i_system_wrapper/system_i/simaye_fft[1]} {i_system_wrapper/system_i/simaye_fft[2]} {i_system_wrapper/system_i/simaye_fft[3]} {i_system_wrapper/system_i/simaye_fft[4]} {i_system_wrapper/system_i/simaye_fft[5]} {i_system_wrapper/system_i/simaye_fft[6]} {i_system_wrapper/system_i/simaye_fft[7]} {i_system_wrapper/system_i/simaye_fft[8]} {i_system_wrapper/system_i/simaye_fft[9]} {i_system_wrapper/system_i/simaye_fft[10]} {i_system_wrapper/system_i/simaye_fft[11]} {i_system_wrapper/system_i/simaye_fft[12]} {i_system_wrapper/system_i/simaye_fft[13]} {i_system_wrapper/system_i/simaye_fft[14]} {i_system_wrapper/system_i/simaye_fft[15]} {i_system_wrapper/system_i/simaye_fft[16]} {i_system_wrapper/system_i/simaye_fft[17]} {i_system_wrapper/system_i/simaye_fft[18]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe5]
set_property port_width 1 [get_debug_ports u_ila_0/probe5]
connect_debug_port u_ila_0/probe5 [get_nets [list i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TLAST]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe6]
set_property port_width 1 [get_debug_ports u_ila_0/probe6]
connect_debug_port u_ila_0/probe6 [get_nets [list i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TREADY]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe7]
set_property port_width 1 [get_debug_ports u_ila_0/probe7]
connect_debug_port u_ila_0/probe7 [get_nets [list i_system_wrapper/system_i/dma_fft_M_AXIS_MM2S_TVALID]]
set_property C_CLK_INPUT_FREQ_HZ 300000000 [get_debug_cores dbg_hub]
set_property C_ENABLE_CLK_DIVIDER false [get_debug_cores dbg_hub]
set_property C_USER_SCAN_CHAIN 1 [get_debug_cores dbg_hub]
connect_debug_port dbg_hub/clk [get_nets clk]
