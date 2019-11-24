# constraints

# Define SPI clock
create_clock -name spi0_clk      -period 40   [get_pins -hier */EMIOSPI0SCLKO]

# Define SW enable
# Channel 1
set_property  -dict {PACKAGE_PIN  G19  IOSTANDARD LVCMOS33} [get_ports p1_sw]        ; ## switch 1 channel 1
set_property  -dict {PACKAGE_PIN  N15  IOSTANDARD LVCMOS33} [get_ports p1_cp_sw]     ; ## switch 2 channel 1
set_property  -dict {PACKAGE_PIN  R19  IOSTANDARD LVCMOS33} [get_ports tx_a1_sw]     ; ## tx_a_sw channel 1
set_property  -dict {PACKAGE_PIN  H22  IOSTANDARD LVCMOS33} [get_ports tx_b1_sw]     ; ## tx_b_sw channel 1
# Channel 2
set_property  -dict {PACKAGE_PIN  P21  IOSTANDARD LVCMOS33} [get_ports p2_sw]        ; ## switch 1 channel 2
set_property  -dict {PACKAGE_PIN  T18  IOSTANDARD LVCMOS33} [get_ports p2_cp_sw]     ; ## switch 2 channel 2
set_property  -dict {PACKAGE_PIN  T17  IOSTANDARD LVCMOS33} [get_ports tx_a2_sw]     ; ## tx_a_sw channel 2
set_property  -dict {PACKAGE_PIN  N18  IOSTANDARD LVCMOS33} [get_ports tx_b2_sw]     ; ## tx_b_sw channel 2

set_property  -dict {PACKAGE_PIN  M22  IOSTANDARD LVCMOS33} [get_ports lna_sw]       ; ## lna_sw
set_property  -dict {PACKAGE_PIN  J17  IOSTANDARD LVCMOS33} [get_ports cpl_sw]       ; ## cpl_sw
set_property  -dict {PACKAGE_PIN  L17  IOSTANDARD LVCMOS33} [get_ports reg_12v_sw]   ; ## regulator 12v enable
set_property  -dict {PACKAGE_PIN  M21  IOSTANDARD LVCMOS33} [get_ports reg_5v_sw]    ; ## regulator 5v enable

# Define LED
set_property  -dict {PACKAGE_PIN  J21  IOSTANDARD LVCMOS33} [get_ports led_r]        ; ## RGB LED Red
set_property  -dict {PACKAGE_PIN  J20  IOSTANDARD LVCMOS33} [get_ports led_g]        ; ## RGB LED Green
set_property  -dict {PACKAGE_PIN  J22  IOSTANDARD LVCMOS33} [get_ports led_b]        ; ## RGB LED Blue

set_property  -dict {PACKAGE_PIN  D17  IOSTANDARD LVCMOS33} [get_ports led_3]        ; ## LED 3
set_property  -dict {PACKAGE_PIN  D16  IOSTANDARD LVCMOS33} [get_ports led_4]        ; ## LED 4

# Define Attenuator
set_property  -dict {PACKAGE_PIN  P16  IOSTANDARD LVCMOS33} [get_ports att_mosi]      ; ## attenuator mosi
set_property  -dict {PACKAGE_PIN  R21  IOSTANDARD LVCMOS33} [get_ports att_sck]       ; ## attenuator sck
set_property  -dict {PACKAGE_PIN  N19  IOSTANDARD LVCMOS33} [get_ports att_le1]       ; ## attenuator le1
set_property  -dict {PACKAGE_PIN  N17  IOSTANDARD LVCMOS33} [get_ports att_le2]       ; ## attenuator le2