
# Define SPI clock
create_clock -period 40.000 -name spi0_clk [get_pins -hier */EMIOSPI0SCLKO]
