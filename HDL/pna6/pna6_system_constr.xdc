# constraints

# Define SPI clock
create_clock -name spi0_clk      -period 40   [get_pins -hier */EMIOSPI0SCLKO]
