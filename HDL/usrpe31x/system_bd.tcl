# create board design
# default ports

create_bd_intf_port -mode Master -vlnv xilinx.com:interface:ddrx_rtl:1.0 ddr
create_bd_intf_port -mode Master -vlnv xilinx.com:display_processing_system7:fixedio_rtl:1.0 fixed_io

create_bd_port -dir O spi0_csn_0
create_bd_port -dir O spi0_csn_1
create_bd_port -dir O spi0_csn_2
create_bd_port -dir O spi0_clk
create_bd_port -dir O spi0_mosi
create_bd_port -dir I spi0_miso

create_bd_port -dir I spi1_csn
create_bd_port -dir I spi1_clk
create_bd_port -dir I spi1_mosi
create_bd_port -dir O spi1_miso

create_bd_port -dir I -from 63 -to 0 ps_gpio_i
create_bd_port -dir O -from 63 -to 0 ps_gpio_o
create_bd_port -dir O -from 63 -to 0 ps_gpio_t

# instance: sys_ps7

ad_ip_instance processing_system7 sys_ps7

# ps7 settings

ad_ip_parameter sys_ps7 CONFIG.PCW_PRESET_BANK0_VOLTAGE "LVCMOS 1.8V"
ad_ip_parameter sys_ps7 CONFIG.PCW_PRESET_BANK1_VOLTAGE "LVCMOS 1.8V"
ad_ip_parameter sys_ps7 CONFIG.PCW_USE_S_AXI_HP1 1
ad_ip_parameter sys_ps7 CONFIG.PCW_USE_S_AXI_HP2 1
ad_ip_parameter sys_ps7 CONFIG.PCW_EN_CLK1_PORT 1
ad_ip_parameter sys_ps7 CONFIG.PCW_EN_RST1_PORT 1
ad_ip_parameter sys_ps7 CONFIG.PCW_FPGA0_PERIPHERAL_FREQMHZ 100.0
ad_ip_parameter sys_ps7 CONFIG.PCW_FPGA1_PERIPHERAL_FREQMHZ 200.0
ad_ip_parameter sys_ps7 CONFIG.PCW_GPIO_MIO_GPIO_ENABLE 1
ad_ip_parameter sys_ps7 CONFIG.PCW_GPIO_MIO_GPIO_IO MIO
ad_ip_parameter sys_ps7 CONFIG.PCW_ENET0_PERIPHERAL_ENABLE 1
ad_ip_parameter sys_ps7 CONFIG.PCW_ENET0_ENET0_IO "MIO 16 .. 27"
ad_ip_parameter sys_ps7 CONFIG.PCW_ENET0_RESET_ENABLE 1
ad_ip_parameter sys_ps7 CONFIG.PCW_ENET0_RESET_IO "MIO 11"
ad_ip_parameter sys_ps7 CONFIG.PCW_ENET0_GRP_MDIO_ENABLE 1
ad_ip_parameter sys_ps7 CONFIG.PCW_ENET0_GRP_MDIO_IO "MIO 52 .. 53"
ad_ip_parameter sys_ps7 CONFIG.PCW_USB0_PERIPHERAL_ENABLE 1
ad_ip_parameter sys_ps7 CONFIG.PCW_USB0_USB0_IO "MIO 28 .. 39"
ad_ip_parameter sys_ps7 CONFIG.PCW_USB0_RESET_ENABLE 1
ad_ip_parameter sys_ps7 CONFIG.PCW_USB0_RESET_IO "MIO 9"
ad_ip_parameter sys_ps7 CONFIG.PCW_SD0_PERIPHERAL_ENABLE 1
ad_ip_parameter sys_ps7 CONFIG.PCW_SD0_SD0_IO "MIO 40 .. 45"
ad_ip_parameter sys_ps7 CONFIG.PCW_SD0_GRP_CD_ENABLE 1
ad_ip_parameter sys_ps7 CONFIG.PCW_SD0_GRP_CD_IO "MIO 0"
ad_ip_parameter sys_ps7 CONFIG.PCW_SDIO_PERIPHERAL_FREQMHZ 50
ad_ip_parameter sys_ps7 CONFIG.PCW_SPI0_PERIPHERAL_ENABLE 1
ad_ip_parameter sys_ps7 CONFIG.PCW_SPI0_SPI0_IO EMIO
ad_ip_parameter sys_ps7 CONFIG.PCW_SPI1_PERIPHERAL_ENABLE 1
ad_ip_parameter sys_ps7 CONFIG.PCW_SPI1_SPI1_IO EMIO
ad_ip_parameter sys_ps7 CONFIG.PCW_UART0_PERIPHERAL_ENABLE 1
ad_ip_parameter sys_ps7 CONFIG.PCW_UART0_UART0_IO "MIO 14 .. 15"
ad_ip_parameter sys_ps7 CONFIG.PCW_UART1_PERIPHERAL_ENABLE 1
ad_ip_parameter sys_ps7 CONFIG.PCW_UART1_UART1_IO "MIO 48 .. 49"
ad_ip_parameter sys_ps7 CONFIG.PCW_I2C0_PERIPHERAL_ENABLE 1
ad_ip_parameter sys_ps7 CONFIG.PCW_I2C0_I2C0_IO "MIO 46 .. 47"
ad_ip_parameter sys_ps7 CONFIG.PCW_GPIO_EMIO_GPIO_ENABLE 1
ad_ip_parameter sys_ps7 CONFIG.PCW_USE_FABRIC_INTERRUPT 1
ad_ip_parameter sys_ps7 CONFIG.PCW_IRQ_F2P_INTR 1
ad_ip_parameter sys_ps7 CONFIG.PCW_IRQ_F2P_MODE REVERSE

# DDR MT41K256M16 HA-125 (32M, 16bit, 8banks)

ad_ip_parameter sys_ps7 CONFIG.PCW_UIPARAM_DDR_PARTNO Custom
ad_ip_parameter sys_ps7 CONFIG.PCW_UIPARAM_DDR_BUS_WIDTH "32 Bit"
ad_ip_parameter sys_ps7 CONFIG.PCW_UIPARAM_DDR_USE_INTERNAL_VREF 0
ad_ip_parameter sys_ps7 CONFIG.PCW_UIPARAM_DDR_DRAM_WIDTH "16 Bits"
ad_ip_parameter sys_ps7 CONFIG.PCW_UIPARAM_DDR_DEVICE_CAPACITY "4096 MBits"
ad_ip_parameter sys_ps7 CONFIG.PCW_UIPARAM_DDR_CWL 6
ad_ip_parameter sys_ps7 CONFIG.PCW_UIPARAM_DDR_T_RC 48.75
ad_ip_parameter sys_ps7 CONFIG.PCW_UIPARAM_DDR_T_RAS_MIN 35.0
ad_ip_parameter sys_ps7 CONFIG.PCW_UIPARAM_DDR_T_FAW 40.0
ad_ip_parameter sys_ps7 CONFIG.PCW_UIPARAM_DDR_TRAIN_WRITE_LEVEL 1
ad_ip_parameter sys_ps7 CONFIG.PCW_UIPARAM_DDR_TRAIN_READ_GATE 1
ad_ip_parameter sys_ps7 CONFIG.PCW_UIPARAM_DDR_TRAIN_DATA_EYE 1

ad_ip_instance xlconcat sys_concat_intc
ad_ip_parameter sys_concat_intc CONFIG.NUM_PORTS 16

ad_ip_instance xlconstant intc_gnd
ad_ip_parameter intc_gnd CONFIG.CONST_VAL 0

ad_ip_instance proc_sys_reset sys_rstgen
ad_ip_parameter sys_rstgen CONFIG.C_EXT_RST_WIDTH 1

# system reset/clock definitions

ad_connect  sys_cpu_clk sys_ps7/FCLK_CLK0
ad_connect  sys_200m_clk sys_ps7/FCLK_CLK1
ad_connect  sys_cpu_reset sys_rstgen/peripheral_reset
ad_connect  sys_cpu_resetn sys_rstgen/peripheral_aresetn
ad_connect  sys_cpu_clk sys_rstgen/slowest_sync_clk
ad_connect  sys_rstgen/ext_reset_in sys_ps7/FCLK_RESET0_N

# interface connections

ad_connect  ddr sys_ps7/DDR
ad_connect  ps_gpio_i sys_ps7/GPIO_I
ad_connect  ps_gpio_o sys_ps7/GPIO_O
ad_connect  ps_gpio_t sys_ps7/GPIO_T
ad_connect  fixed_io sys_ps7/FIXED_IO

# spi connections

ad_connect  sys_ps7/SPI0_SS_O spi0_csn_0
ad_connect  sys_ps7/SPI0_SS1_O spi0_csn_1
ad_connect  sys_ps7/SPI0_SS2_O spi0_csn_2
ad_connect  sys_ps7/SPI0_SCLK_O spi0_clk
ad_connect  sys_ps7/SPI0_MOSI_O spi0_mosi
ad_connect  sys_ps7/SPI0_MISO_I spi0_miso
ad_connect  sys_ps7/SPI0_SS_I VCC
ad_connect  sys_ps7/SPI0_SCLK_I GND
ad_connect  sys_ps7/SPI0_MOSI_I GND

ad_connect  sys_ps7/SPI1_SS_I spi1_csn
ad_connect  sys_ps7/SPI1_SCLK_I spi1_clk
ad_connect  sys_ps7/SPI1_MOSI_I spi1_mosi
ad_connect  sys_ps7/SPI1_MISO_O spi1_miso
ad_connect  sys_ps7/SPI1_MISO_I GND

# interrupts

ad_connect  sys_concat_intc/dout sys_ps7/IRQ_F2P

# ad9361

create_bd_port -dir I rx_clk_in
create_bd_port -dir I rx_frame_in
create_bd_port -dir I -from 11 -to 0 rx_data_in

create_bd_port -dir O tx_clk_out
create_bd_port -dir O tx_frame_out
create_bd_port -dir O -from 11 -to 0 tx_data_out

create_bd_port -dir O enable
create_bd_port -dir O txnrx
create_bd_port -dir I up_enable
create_bd_port -dir I up_txnrx

# ad9361 core(s)

ad_ip_instance axi_ad9361 axi_ad9361
ad_ip_parameter axi_ad9361 CONFIG.ID 0
ad_ip_parameter axi_ad9361 CONFIG.CMOS_OR_LVDS_N 1
ad_ip_parameter axi_ad9361 CONFIG.ADC_INIT_DELAY 23

ad_ip_instance axi_dmac axi_ad9361_dac_dma
ad_ip_parameter axi_ad9361_dac_dma CONFIG.DMA_TYPE_SRC 0
ad_ip_parameter axi_ad9361_dac_dma CONFIG.DMA_TYPE_DEST 1
ad_ip_parameter axi_ad9361_dac_dma CONFIG.CYCLIC 1
ad_ip_parameter axi_ad9361_dac_dma CONFIG.AXI_SLICE_SRC 0
ad_ip_parameter axi_ad9361_dac_dma CONFIG.AXI_SLICE_DEST 1
ad_ip_parameter axi_ad9361_dac_dma CONFIG.DMA_2D_TRANSFER 0
ad_ip_parameter axi_ad9361_dac_dma CONFIG.DMA_DATA_WIDTH_DEST 64

ad_ip_instance util_upack2 util_ad9361_dac_upack { \
  NUM_OF_CHANNELS 4 \
  SAMPLE_DATA_WIDTH 16 \
}

ad_ip_instance axi_dmac axi_ad9361_adc_dma
ad_ip_parameter axi_ad9361_adc_dma CONFIG.DMA_TYPE_SRC 2
ad_ip_parameter axi_ad9361_adc_dma CONFIG.DMA_TYPE_DEST 0
ad_ip_parameter axi_ad9361_adc_dma CONFIG.CYCLIC 0
ad_ip_parameter axi_ad9361_adc_dma CONFIG.SYNC_TRANSFER_START 1
ad_ip_parameter axi_ad9361_adc_dma CONFIG.AXI_SLICE_SRC 0
ad_ip_parameter axi_ad9361_adc_dma CONFIG.AXI_SLICE_DEST 0
ad_ip_parameter axi_ad9361_adc_dma CONFIG.DMA_2D_TRANSFER 0
ad_ip_parameter axi_ad9361_adc_dma CONFIG.DMA_DATA_WIDTH_SRC 64

ad_ip_instance util_cpack2 util_ad9361_adc_pack { \
  NUM_OF_CHANNELS 4 \
  SAMPLE_DATA_WIDTH 16 \
}

# connections

ad_connect  rx_clk_in axi_ad9361/rx_clk_in
ad_connect  rx_frame_in axi_ad9361/rx_frame_in
ad_connect  rx_data_in axi_ad9361/rx_data_in
ad_connect  tx_clk_out axi_ad9361/tx_clk_out
ad_connect  tx_frame_out axi_ad9361/tx_frame_out
ad_connect  tx_data_out axi_ad9361/tx_data_out
ad_connect  enable axi_ad9361/enable
ad_connect  txnrx axi_ad9361/txnrx
ad_connect  up_enable axi_ad9361/up_enable
ad_connect  up_txnrx axi_ad9361/up_txnrx

ad_connect  sys_200m_clk axi_ad9361/delay_clk
ad_connect  axi_ad9361/l_clk axi_ad9361/clk

ad_connect  axi_ad9361/l_clk util_ad9361_adc_pack/clk
ad_connect  axi_ad9361/rst util_ad9361_adc_pack/reset

ad_connect  axi_ad9361/adc_valid_i0 util_ad9361_adc_pack/fifo_wr_en
ad_connect  axi_ad9361/adc_enable_i0 util_ad9361_adc_pack/enable_0
ad_connect  axi_ad9361/adc_data_i0 util_ad9361_adc_pack/fifo_wr_data_0
ad_connect  axi_ad9361/adc_enable_q0 util_ad9361_adc_pack/enable_1
ad_connect  axi_ad9361/adc_data_q0 util_ad9361_adc_pack/fifo_wr_data_1
ad_connect  axi_ad9361/adc_enable_i1 util_ad9361_adc_pack/enable_2
ad_connect  axi_ad9361/adc_data_i1 util_ad9361_adc_pack/fifo_wr_data_2
ad_connect  axi_ad9361/adc_enable_q1 util_ad9361_adc_pack/enable_3
ad_connect  axi_ad9361/adc_data_q1 util_ad9361_adc_pack/fifo_wr_data_3
ad_connect  axi_ad9361/adc_dovf util_ad9361_adc_pack/fifo_wr_overflow

ad_connect  axi_ad9361/l_clk axi_ad9361_adc_dma/fifo_wr_clk
ad_connect  util_ad9361_adc_pack/packed_fifo_wr axi_ad9361_adc_dma/fifo_wr

ad_connect  axi_ad9361/l_clk util_ad9361_dac_upack/clk
ad_connect  axi_ad9361/rst util_ad9361_dac_upack/reset

ad_connect  axi_ad9361/dac_valid_i0 util_ad9361_dac_upack/fifo_rd_en
ad_connect  axi_ad9361/dac_enable_i0 util_ad9361_dac_upack/enable_0
ad_connect  axi_ad9361/dac_data_i0 util_ad9361_dac_upack/fifo_rd_data_0
ad_connect  axi_ad9361/dac_enable_q0 util_ad9361_dac_upack/enable_1
ad_connect  axi_ad9361/dac_data_q0 util_ad9361_dac_upack/fifo_rd_data_1
ad_connect  axi_ad9361/dac_enable_i1 util_ad9361_dac_upack/enable_2
ad_connect  axi_ad9361/dac_data_i1 util_ad9361_dac_upack/fifo_rd_data_2
ad_connect  axi_ad9361/dac_enable_q1 util_ad9361_dac_upack/enable_3
ad_connect  axi_ad9361/dac_data_q1 util_ad9361_dac_upack/fifo_rd_data_3
ad_connect  axi_ad9361/dac_dunf util_ad9361_dac_upack/fifo_rd_underflow

ad_connect  axi_ad9361/l_clk axi_ad9361_dac_dma/m_axis_aclk
ad_connect  util_ad9361_dac_upack/s_axis axi_ad9361_dac_dma/m_axis

# interconnects

ad_cpu_interconnect 0x79020000 axi_ad9361
ad_cpu_interconnect 0x7C400000 axi_ad9361_adc_dma
ad_cpu_interconnect 0x7C420000 axi_ad9361_dac_dma
ad_mem_hp1_interconnect sys_cpu_clk sys_ps7/S_AXI_HP1
ad_mem_hp1_interconnect sys_cpu_clk axi_ad9361_adc_dma/m_dest_axi
ad_mem_hp2_interconnect sys_cpu_clk sys_ps7/S_AXI_HP2
ad_mem_hp2_interconnect sys_cpu_clk axi_ad9361_dac_dma/m_src_axi
ad_connect  sys_cpu_resetn axi_ad9361_adc_dma/m_dest_axi_aresetn
ad_connect  sys_cpu_resetn axi_ad9361_dac_dma/m_src_axi_aresetn

# tdd-sync

ad_ip_instance util_tdd_sync util_ad9361_tdd_sync
ad_ip_parameter util_ad9361_tdd_sync CONFIG.TDD_SYNC_PERIOD 10000000

create_bd_port -dir I tdd_sync

ad_connect  tdd_sync util_ad9361_tdd_sync/sync_in
ad_connect  sys_cpu_clk util_ad9361_tdd_sync/clk
ad_connect  sys_cpu_resetn util_ad9361_tdd_sync/rstn
ad_connect  util_ad9361_tdd_sync/sync_out axi_ad9361/tdd_sync
ad_connect  util_ad9361_tdd_sync/sync_mode axi_ad9361/tdd_sync_cntr

# gpio

create_bd_port -dir I -from 31 -to 0 pl_gpio_i
create_bd_port -dir O -from 31 -to 0 pl_gpio_o
create_bd_port -dir O -from 31 -to 0 pl_gpio_t

ad_ip_instance axi_gpio axi_gpio
ad_ip_parameter axi_gpio CONFIG.C_GPIO_WIDTH 32
ad_ip_parameter axi_gpio CONFIG.C_INTERRUPT_PRESENT 1

ad_connect  pl_gpio_i axi_gpio/gpio_io_i
ad_connect  pl_gpio_o axi_gpio/gpio_io_o
ad_connect  pl_gpio_t axi_gpio/gpio_io_t

ad_cpu_interconnect 0x41600000 axi_gpio

ad_connect  sys_concat_intc/In15 axi_gpio/ip2intc_irpt
ad_connect  sys_concat_intc/In14 intc_gnd/dout
ad_connect  sys_concat_intc/In13 axi_ad9361_adc_dma/irq
ad_connect  sys_concat_intc/In12 axi_ad9361_dac_dma/irq
ad_connect  sys_concat_intc/In11 intc_gnd/dout
ad_connect  sys_concat_intc/In10 intc_gnd/dout
# ad_connect  sys_concat_intc/In9 intc_gnd/dout
# ad_connect  sys_concat_intc/In8 intc_gnd/dout
ad_connect  sys_concat_intc/In7 intc_gnd/dout
ad_connect  sys_concat_intc/In6 intc_gnd/dout
ad_connect  sys_concat_intc/In5 intc_gnd/dout
ad_connect  sys_concat_intc/In4 intc_gnd/dout
ad_connect  sys_concat_intc/In3 intc_gnd/dout
ad_connect  sys_concat_intc/In2 intc_gnd/dout
ad_connect  sys_concat_intc/In1 intc_gnd/dout
ad_connect  sys_concat_intc/In0 intc_gnd/dout

# FFT S2MM Converter
ad_ip_instance axis_dwidth_converter fft_s2mm_converter
ad_ip_parameter fft_s2mm_converter CONFIG.M_TDATA_NUM_BYTES 8
ad_connect sys_cpu_clk fft_s2mm_converter/aclk
ad_connect sys_cpu_resetn fft_s2mm_converter/aresetn

# FFT MM2S Converter
ad_ip_instance axis_dwidth_converter fft_mm2s_converter
ad_ip_parameter fft_mm2s_converter CONFIG.M_TDATA_NUM_BYTES 6
ad_connect sys_cpu_clk fft_mm2s_converter/aclk
ad_connect sys_cpu_resetn fft_mm2s_converter/aresetn

# GPIO FFT Config Valid
ad_ip_instance axi_gpio fft_config_valid
ad_ip_parameter fft_config_valid CONFIG.C_GPIO_WIDTH 1
ad_ip_parameter fft_config_valid CONFIG.C_ALL_OUTPUTS 1
ad_ip_parameter fft_config_valid CONFIG.C_DOUT_DEFAULT  0x00000000
ad_connect sys_cpu_clk fft_config_valid/s_axi_aclk
ad_connect sys_cpu_resetn fft_config_valid/s_axi_aresetn
ad_cpu_interconnect 0x41230000 fft_config_valid

# GPIO FFT Status
ad_ip_instance axi_gpio gpio_fft_status
ad_ip_parameter gpio_fft_status CONFIG.C_GPIO_WIDTH 7
ad_ip_parameter gpio_fft_status CONFIG.C_ALL_INPUTS 1
ad_connect sys_cpu_clk gpio_fft_status/s_axi_aclk
ad_connect sys_cpu_resetn gpio_fft_status/s_axi_aresetn
ad_cpu_interconnect 0x41220000 gpio_fft_status

# FFT Status Concat
ad_ip_instance xlconcat status_concat
ad_ip_parameter status_concat CONFIG.NUM_PORTS 7
ad_connect gpio_fft_status/gpio_io_i status_concat/dout

# GPIO FFT
ad_ip_instance axi_gpio gpio_fft
ad_ip_parameter gpio_fft CONFIG.C_GPIO_WIDTH 17
ad_ip_parameter gpio_fft CONFIG.C_ALL_OUTPUTS 0
ad_ip_parameter gpio_fft CONFIG.C_DOUT_DEFAULT  0x0000010A
ad_connect sys_cpu_clk gpio_fft/s_axi_aclk
ad_connect sys_cpu_resetn gpio_fft/s_axi_aresetn
ad_cpu_interconnect 0x41200000 gpio_fft

# FFT Config Slice
ad_ip_instance xlslice fft_config
# 0-4: NFFT, 8: FWD_INV, 16: RESET
ad_ip_parameter fft_config CONFIG.DOUT_WIDTH 16
ad_ip_parameter fft_config CONFIG.DIN_FROM 15
ad_ip_parameter fft_config CONFIG.DIN_WIDTH 17
ad_ip_parameter fft_config CONFIG.DIN_TO 0
ad_connect gpio_fft/gpio_io_o fft_config/Din
ad_connect gpio_fft/gpio_io_i fft_config/Din

# FFT Reset Slice
ad_ip_instance xlslice fft_reset
ad_ip_parameter fft_reset CONFIG.DOUT_WIDTH 1
ad_ip_parameter fft_reset CONFIG.DIN_FROM 16
ad_ip_parameter fft_reset CONFIG.DIN_WIDTH 17
ad_ip_parameter fft_reset CONFIG.DIN_TO 16
ad_connect gpio_fft/gpio_io_o fft_reset/Din

# FFT DMA
ad_ip_instance axi_dma dma_fft
ad_ip_parameter dma_fft CONFIG.c_addr_width 32
ad_ip_parameter dma_fft CONFIG.c_m_axi_mm2s_data_width 64
ad_ip_parameter dma_fft CONFIG.c_m_axi_s2mm_data_width 64
ad_ip_parameter dma_fft CONFIG.c_m_axis_mm2s_tdata_width 64
ad_ip_parameter dma_fft CONFIG.c_mm2s_burst_size 8
ad_ip_parameter dma_fft CONFIG.c_s2mm_burst_size 16
# ad_ip_parameter dma_fft CONFIG.c_s_axis_s2mm_tdata_width 64
ad_ip_parameter dma_fft CONFIG.c_sg_length_width 20
ad_ip_parameter dma_fft CONFIG.c_include_sg 0
ad_ip_parameter dma_fft CONFIG.c_sg_include_stscntrl_strm 0
ad_connect sys_cpu_clk dma_fft/s_axi_lite_aclk
ad_connect sys_cpu_clk dma_fft/m_axi_mm2s_aclk
ad_connect sys_cpu_clk dma_fft/m_axi_s2mm_aclk
ad_connect sys_cpu_resetn dma_fft/axi_resetn
ad_connect fft_s2mm_converter/M_AXIS dma_fft/S_AXIS_S2MM
ad_connect fft_mm2s_converter/S_AXIS dma_fft/M_AXIS_MM2S
ad_cpu_interconnect 0x40400000 dma_fft
ad_mem_hp3_interconnect sys_cpu_clk sys_ps7/S_AXI_HP3
ad_mem_hp3_interconnect sys_cpu_clk dma_fft/M_AXI_MM2S
ad_mem_hp3_interconnect sys_cpu_clk dma_fft/M_AXI_S2MM
# ad_cpu_interrupt ps-8 mb-8 dma_fft/s2mm_introut
# ad_cpu_interrupt ps-9 mb-9 dma_fft/mm2s_introut
ad_connect  sys_concat_intc/In8 dma_fft/s2mm_introut
ad_connect  sys_concat_intc/In9 dma_fft/mm2s_introut

# Edge Detect
add_files -norecurse ../../library/edge_detect/edge_detect.v
create_bd_cell -type module -reference edge_detect fft_edge_detect
ad_ip_parameter fft_edge_detect CONFIG.N 16
ad_ip_parameter fft_edge_detect CONFIG.RST_DELAY 100
ad_connect sys_cpu_clk fft_edge_detect/clk
ad_connect fft_edge_detect/din fft_config/Dout
ad_connect fft_edge_detect/bvalid fft_config_valid/gpio_io_o

# FFT
# report_property [get_ips ip_instance_name]
ad_ip_instance xfft fft
ad_ip_parameter fft CONFIG.data_format fixed_point
ad_ip_parameter fft CONFIG.implementation_options pipelined_streaming_io
ad_ip_parameter fft CONFIG.input_width 24
ad_ip_parameter fft CONFIG.output_ordering natural_order
ad_ip_parameter fft CONFIG.ovflo false
ad_ip_parameter fft CONFIG.phase_factor_width 24
ad_ip_parameter fft CONFIG.rounding_modes truncation
ad_ip_parameter fft CONFIG.run_time_configurable_transform_length true
ad_ip_parameter fft CONFIG.scaling_options block_floating_point
ad_ip_parameter fft CONFIG.throttle_scheme nonrealtime
ad_ip_parameter fft CONFIG.transform_length 8192
ad_ip_parameter fft CONFIG.target_clock_frequency 100
ad_ip_parameter fft CONFIG.number_of_stages_using_block_ram_for_data_and_phase_factors 6
ad_ip_parameter fft CONFIG.aresetn true

ad_connect fft/S_AXIS_DATA fft_mm2s_converter/M_AXIS
ad_connect fft/s_axis_config_tdata fft_edge_detect/din
ad_connect fft/s_axis_config_tvalid fft_edge_detect/edge_detected
ad_connect sys_cpu_clk fft/aclk
ad_connect fft/M_AXIS_DATA fft_s2mm_converter/S_AXIS
ad_connect fft/aresetn fft_reset/Dout
ad_connect fft/aresetn fft_edge_detect/nreset
ad_connect fft/event_data_out_channel_halt status_concat/In0
ad_connect fft/event_data_in_channel_halt status_concat/In1
ad_connect fft/event_status_channel_halt status_concat/In2
ad_connect fft/event_tlast_missing status_concat/In3
ad_connect fft/event_tlast_unexpected status_concat/In4
ad_connect fft/event_frame_started status_concat/In5
ad_connect fft/s_axis_config_tready status_concat/In6
