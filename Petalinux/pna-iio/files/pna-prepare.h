#ifndef __PNA_PREPARE_H__
#define __PNA_PREPARE_H__

#include <iio.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include "pna-interface.h"

// board name and id
#define XILINX_ZC702 2
#define ZC702_NAME "ZC702"
#define ETTUS_E310 3
#define E310_NAME "R310"

#define MAX_FFT_LENGTH    8192
#define MIN_FFT_LENGTH     4
#define MAX_SIG_POW          0.0
#define DAC_MAX_VAL          2047*16

int load_fir_filter(const char *file_name, struct iio_device *dev1);

int load_rx_sample_size();
int load_dac_max();
int save_dac_max(int dac_max);
int load_board_id();
double load_sig_pow(int channel_num);
int save_sig_pow(double sig_pow, int channel_num);

int load_from_file(const char *file_name, char *var_str);
int save_to_file(const char *file_name, char *var_str);
int isDirectoryExists(const char *path);

#endif /* __PNA_PREPARE_H__ */
