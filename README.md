# PNA6

6GHz Portable Network Analyzer

## How to Install

### Step 1

Rename `setup_variables.template` to `setup_variables.sh` and set following variables:
- XILINX_INSTALL_DIR : xilinx install directory
- PETALINUX_INSTALL_DIR : PetaLinux install directory
- XILINX_VERSION : xilinx version
- ADI_HDL_PROJECT : clone the ADI HDL project from [here](https://github.com/analogdevicesinc/hdl.git)
- META_ADI_PROJECT : clone the ADI HDL project from [here](https://github.com/analogdevicesinc/meta-adi.git)

### Step 2

run `install.sh`

### Step 3

run `sudo install_rules.sh`
