
# constraints
# ad9361

set_property  -dict {PACKAGE_PIN  L18  IOSTANDARD LVCMOS25} [get_ports rx_clk_in]
set_property  -dict {PACKAGE_PIN  N18  IOSTANDARD LVCMOS25} [get_ports rx_frame_in]
set_property  -dict {PACKAGE_PIN  N19  IOSTANDARD LVCMOS25} [get_ports {rx_data_in[0]}]
set_property  -dict {PACKAGE_PIN  M21  IOSTANDARD LVCMOS25} [get_ports {rx_data_in[1]}]
set_property  -dict {PACKAGE_PIN  P20  IOSTANDARD LVCMOS25} [get_ports {rx_data_in[2]}]
set_property  -dict {PACKAGE_PIN  P15  IOSTANDARD LVCMOS25} [get_ports {rx_data_in[3]}]
set_property  -dict {PACKAGE_PIN  P17  IOSTANDARD LVCMOS25} [get_ports {rx_data_in[4]}]
set_property  -dict {PACKAGE_PIN  P18  IOSTANDARD LVCMOS25} [get_ports {rx_data_in[5]}]
set_property  -dict {PACKAGE_PIN  R20  IOSTANDARD LVCMOS25} [get_ports {rx_data_in[6]}]
set_property  -dict {PACKAGE_PIN  P21  IOSTANDARD LVCMOS25} [get_ports {rx_data_in[7]}]
set_property  -dict {PACKAGE_PIN  M22  IOSTANDARD LVCMOS25} [get_ports {rx_data_in[8]}]
set_property  -dict {PACKAGE_PIN  T17  IOSTANDARD LVCMOS25} [get_ports {rx_data_in[9]}]
set_property  -dict {PACKAGE_PIN  N22  IOSTANDARD LVCMOS25} [get_ports {rx_data_in[10]}]
set_property  -dict {PACKAGE_PIN  N15  IOSTANDARD LVCMOS25} [get_ports {rx_data_in[11]}]
set_property  -dict {PACKAGE_PIN  R21  IOSTANDARD LVCMOS25} [get_ports tx_clk_out]
set_property  -dict {PACKAGE_PIN  P22  IOSTANDARD LVCMOS25} [get_ports tx_frame_out]
set_property  -dict {PACKAGE_PIN  L16  IOSTANDARD LVCMOS25} [get_ports {tx_data_out[0]}]
set_property  -dict {PACKAGE_PIN  K18  IOSTANDARD LVCMOS25} [get_ports {tx_data_out[1]}]
set_property  -dict {PACKAGE_PIN  L17  IOSTANDARD LVCMOS25} [get_ports {tx_data_out[2]}]
set_property  -dict {PACKAGE_PIN  J17  IOSTANDARD LVCMOS25} [get_ports {tx_data_out[3]}]
set_property  -dict {PACKAGE_PIN  M16  IOSTANDARD LVCMOS25} [get_ports {tx_data_out[4]}]
set_property  -dict {PACKAGE_PIN  J15  IOSTANDARD LVCMOS25} [get_ports {tx_data_out[5]}]
set_property  -dict {PACKAGE_PIN  K16  IOSTANDARD LVCMOS25} [get_ports {tx_data_out[6]}]
set_property  -dict {PACKAGE_PIN  J16  IOSTANDARD LVCMOS25} [get_ports {tx_data_out[7]}]
set_property  -dict {PACKAGE_PIN  N20  IOSTANDARD LVCMOS25} [get_ports {tx_data_out[8]}]
set_property  -dict {PACKAGE_PIN  K15  IOSTANDARD LVCMOS25} [get_ports {tx_data_out[9]}]
set_property  -dict {PACKAGE_PIN  N17  IOSTANDARD LVCMOS25} [get_ports {tx_data_out[10]}]
set_property  -dict {PACKAGE_PIN  M17  IOSTANDARD LVCMOS25} [get_ports {tx_data_out[11]}]

set_property  -dict {PACKAGE_PIN  AB5  IOSTANDARD LVCMOS25} [get_ports {gpio_status[0]}]
set_property  -dict {PACKAGE_PIN  AB6  IOSTANDARD LVCMOS25} [get_ports {gpio_status[1]}]
set_property  -dict {PACKAGE_PIN  AB7  IOSTANDARD LVCMOS25} [get_ports {gpio_status[2]}]
set_property  -dict {PACKAGE_PIN  AA4  IOSTANDARD LVCMOS25} [get_ports {gpio_status[3]}]
set_property  -dict {PACKAGE_PIN  K20  IOSTANDARD LVCMOS25} [get_ports {gpio_status[4]}]
set_property  -dict {PACKAGE_PIN  L19  IOSTANDARD LVCMOS25} [get_ports {gpio_status[5]}]
set_property  -dict {PACKAGE_PIN  V12  IOSTANDARD LVCMOS25} [get_ports {gpio_status[6]}]
set_property  -dict {PACKAGE_PIN  W12  IOSTANDARD LVCMOS25} [get_ports {gpio_status[7]}]
set_property  -dict {PACKAGE_PIN  V4   IOSTANDARD LVCMOS25} [get_ports {gpio_ctl[0]}]
set_property  -dict {PACKAGE_PIN  V5   IOSTANDARD LVCMOS25} [get_ports {gpio_ctl[1]}]
set_property  -dict {PACKAGE_PIN  U5   IOSTANDARD LVCMOS25} [get_ports {gpio_ctl[2]}]
set_property  -dict {PACKAGE_PIN  U6   IOSTANDARD LVCMOS25} [get_ports {gpio_ctl[3]}]
set_property  -dict {PACKAGE_PIN  AB2  IOSTANDARD LVCMOS25} [get_ports gpio_en_agc]
set_property  -dict {PACKAGE_PIN  U11  IOSTANDARD LVCMOS25} [get_ports gpio_resetb]
set_property  -dict {PACKAGE_PIN  AB4  IOSTANDARD LVCMOS25} [get_ports enable]
set_property  -dict {PACKAGE_PIN  AB1  IOSTANDARD LVCMOS25} [get_ports txnrx]

set_property  -dict {PACKAGE_PIN  W6  IOSTANDARD LVCMOS25  PULLTYPE PULLUP} [get_ports spi_csn]
set_property  -dict {PACKAGE_PIN  W5  IOSTANDARD LVCMOS25} [get_ports spi_clk]
set_property  -dict {PACKAGE_PIN  V7  IOSTANDARD LVCMOS25} [get_ports spi_mosi]
set_property  -dict {PACKAGE_PIN  W7  IOSTANDARD LVCMOS25} [get_ports spi_miso]

# clocks

create_clock -name rx_clk       -period  4 [get_ports rx_clk_in]

# uart
# MIO 14 15

