#! /bin/bash

source setup_variables.sh

CURRENT_DIR=$(pwd)

echo "------------------Choose Project-------------------"
echo "1 - PNA6 DDRLESS"
echo "2 - ZC702 FFT"
echo "3 - Ettus E310"
echo "---------------------------------------------------"
read response
if [ "$response" = "1" ]; then
	export DDRLESS_PROJECT="$ADI_HDL_PROJECT/projects/pna6/ddrless_pna6/ddrless_pna6.sdk/pna6_standalone/src"
	Scripts/PNA6_DDRLess.sh
elif [ "$response" = "2" ]; then
	export SDK_PETALINUX_PROJECT="$ADI_HDL_PROJECT/projects/fft/zc702/fft_zc702.sdk/fft_zc702_linux/src"
	export PETALINUX_PROJECT="fft_zc702_linux"
	Scripts/ZC702_FFT.sh
elif [ "$response" = "3" ]; then
	export PETALINUX_PROJECT="fft_usrp_linux"
	Scripts/Ettus_E310.sh
fi
