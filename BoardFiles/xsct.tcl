# xsct tcl file

set hdf_file "system.hdf"
set sw_files "SDK"
sdk setws .

hsi open_hw_design $hdf_file
sdk createhw -name ddrless_hw -hwspec $hdf_file
sdk projects -build -type bsp -name ddrless_bsp
sdk createapp -name ddrless_pna6 -hwproject ddrless_hw -proc ps7_cortexa9_0 -bsp ddrless_bsp -app {Empty Application}

file copy -force $sw_files ddrless_pna6/src/
importsources -name ddrless_pna6 -path ddrless_pna6/src/ -linker-script

sdk configapp -app ddrless_pna6 build-config release
sdk configapp -app sw -set linker-misc {-Xlinker --defsym=_HEAP_SIZE=0x1000 -Xlinker --defsym=_STACK_SIZE=0x400}

configapp -app ddrless_pna6 -set compiler-optimization {Optimize for size (-Os)}

sdk projects -build -type app -name ddrless_pna6

exit
