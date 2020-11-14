
connect -url tcp:localhost:3121
puts stderr "INFO: Reseting the FPGA..."
targets
target 2
rst -cores
after 500
puts stderr "INFO: Reseting the System..."
rst -system
after 500
puts stderr "INFO: Configuring the FPGA..."
puts stderr "INFO: Downloading bitstream to the target."
targets -set -nocase -filter {name =~ "arm*#0"}
# source /group/bcapps/seemas/cases/10348699/Xilinx-ZC702-2016.1/subsystems/linux/hw-description/ps7_init.tcl; ps7_post_config
source project-spec/hw-description/ps7_init.tcl; ps7_post_config
catch {stop}
set mctrlval [string trim [lindex [split [mrd 0xF8007080] :] 1]]
puts "mctrlval=$mctrlval"
puts stderr "INFO: Downloading ELF file to the target."
# dow "/group/bcapps/seemas/cases/10348699/Xilinx-ZC702-2016.1/pre-built/linux/images/zynq_fsbl.elf"
dow "pre-built/linux/images/zynq_fsbl.elf"
after 2000
con
after 3000; stop
targets -set -nocase -filter {name =~ "arm*#0"}

puts stderr "INFO: Downloading ELF file to the target."
# dow "/group/bcapps/seemas/cases/10348699/Xilinx-ZC702-2016.1/pre-built/linux/images/u-boot.elf"
dow "pre-built/linux/images/u-boot.elf"
after 2000
con
after 2000; stop
targets -set -nocase -filter {name =~ "arm*#0"}
rst -processor; after 2000
#dow -data "/group/bcapps/seemas/cases/10348699/Xilinx-ZC702-2016.1/pre-built/linux/images/system.dtb" 0x02408000
puts stderr "INFO: Loading image: /mnt/hdd/PetaLinux/fft_zc702_linux/pre-built/linux/images/system.dtb at 0x08008000"
dow -data "pre-built/linux/images/system.dtb" 0x08008000
after 2000
rwr r2 0x08008000
targets -set -nocase -filter {name =~ "arm*#0"}
# dow -data "/group/bcapps/seemas/cases/10348699/Xilinx-ZC702-2016.1/pre-built/linux/images/zImage" 0x00008000
dow -data "pre-built/linux/images/zImage" 0x00008000
after 2000
rwr pc 0x00008000
con
after 5000
# fpga "/group/bcapps/seemas/cases/10348699/Xilinx-ZC702-2016.1/pre-built/linux/implementation/download.bit"
fpga "pre-built/linux/implementation/system.bit"
after 2000
exit
puts stderr "INFO: Saving XSDB commands to checking_tcl. You can run 'xsdb checking_tcl' to execute"
