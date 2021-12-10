#include "pna-prepare.h"

int load_rx_sample_size()
{
	const char rx_sample_size_filename[] = COMMAND_RXSS;
	char rx_sample_size_str[10];

	if(load_from_file(rx_sample_size_filename, rx_sample_size_str) != 0)
	{
		return -1;
	}
	int fft_size = atoi(rx_sample_size_str);
	for(int i=MIN_FFT_LENGTH; i<MAX_FFT_LENGTH; i*=2)
	{
		if(fft_size == i)
		{
			return fft_size;
		}
	}
	pna_printf("no valid fft_size written in file, default value set to 1024\n");
	return -1;
}

int save_rx_sample_size(int fft_size)
{
	char rx_sample_size_filename[20];
	char rx_sample_size_str[10];

	sprintf(rx_sample_size_filename, COMMAND_RXSS);

	sprintf(rx_sample_size_str, "%d", fft_size);

	if(save_to_file(rx_sample_size_filename, rx_sample_size_str) != 0)
	{
		return -1;
	}
	return 0;
}

int load_dac_max()
{
	char dac_max_filename[20];
	char dac_max_str[50];

	sprintf(dac_max_filename, "dac_max");

	if(load_from_file(dac_max_filename, dac_max_str) != 0)
	{
		return -1;
	}
	int dac_max = atoi(dac_max_str);
	if(dac_max <= DAC_MAX_VAL && dac_max >= 0 )
	{
		return dac_max;
	}
	pna_printf("no valid dac_max written in file, default value set to DAC_MAX_VAL\n");
	return -1;
}

int load_board_id()
{
	char board_name_filename[20];
	char board_name_str[50];

	sprintf(board_name_filename, "board_name");

	if(load_from_file(board_name_filename, board_name_str) != 0)
	{
		return -1;
	}
	if(!strcmp(board_name_str, ZC702_NAME))
	{
		return XILINX_ZC702;
	}
	else if(!strcmp(board_name_str, E310_NAME))
	{
		return ETTUS_E310;
	}
	pna_printf("no valid board name written in file, default value set to R310\n");
	return -1;
}

int save_dac_max(int dac_max)
{
	char dac_max_filename[20];
	char dac_max_str[50];

	sprintf(dac_max_filename, "dac_max");

	sprintf(dac_max_str, "%d", dac_max);

	if(save_to_file(dac_max_filename, dac_max_str) != 0)
	{
		return -1;
	}
	return 0;
}

double load_sig_pow(int channel_num)
{
	char sig_pow_filename[20];
	char sig_pow_str[50];

	sprintf(sig_pow_filename, COMMAND_SIGPOW"%d", channel_num);

	if(load_from_file(sig_pow_filename, sig_pow_str) != 0)
	{
		return 100;
	}
	double sig_pow = atof(sig_pow_str);
	if(sig_pow <= MAX_SIG_POW )
	{
		return sig_pow;
	}
	pna_printf("no valid "COMMAND_SIGPOW" written in file, default value set to vga_gain\n");
	return 100;
}

int save_sig_pow(double sig_pow, int channel_num)
{
	char sig_pow_filename[20];
	char sig_pow_str[50];

	sprintf(sig_pow_filename, COMMAND_SIGPOW"%d", channel_num);

	sprintf(sig_pow_str, "%lf", sig_pow);

	if(save_to_file(sig_pow_filename, sig_pow_str) != 0)
	{
		return -1;
	}
	return 0;
}

int load_fir_filter(const char *file_name, struct iio_device *dev)
{
	int ret = -10;
	FILE *f;

	f = fopen(file_name, "r");
	if (f)
	{
		char *buf;
		ssize_t len;

		fseek(f, 0, SEEK_END);
		len = ftell(f);
		buf = malloc(len);
		fseek(f, 0, SEEK_SET);
		len = fread(buf, 1, len, f);
		fclose(f);

		ret = iio_device_attr_write_raw(dev, "filter_fir_config", buf, len);
		free(buf);
	}
	else
	{
		pna_printf("FIR filter file failed to open: %s\n", file_name);
	}

	if (ret < 0)
	{
		pna_printf("FIR filter config failed: %s", file_name);
	}
	else
	{
		pna_printf("Filter loaded: %s (ret = %i)", file_name, ret);
	}
	return ret;
}

int load_from_file(const char *file_name, char *var_str)
{
	if(!isDirectoryExists("/etc/pna_iio"))
	{
		int e;
		e = mkdir("/etc/pna_iio", S_IRWXU);
		if (e != 0)
		{
			printf("mkdir failed; errno=%d\n",errno);
			return -1;
		}
	}

	char path[100];
	FILE *f;

	sprintf(path, "/etc/pna_iio/%s", file_name);
	f = fopen(path, "r");
	if(f != NULL)
	{
		// obtain file size:
		fseek (f , 0 , SEEK_END);
		long file_size = ftell (f);
		rewind (f);
		fread(var_str, 1, file_size, f);
		fclose(f);
	}
	else
	{
		pna_printf("error in openning file %s\n", path);
		return -1;
	}
	return 0;
}

int save_to_file(const char *file_name, char *var_str)
{
	if(!isDirectoryExists("/etc/pna_iio"))
	{
		int e;
		e = mkdir("/etc/pna_iio", S_IRWXU);
		if (e != 0)
		{
			pna_printf("mkdir failed; errno=%d\n",errno);
			return -1;
		}
	}

	char path[100];
	FILE *f;

	sprintf(path, "/etc/pna_iio/%s", file_name);
	f = fopen(path, "w");
	if(f != NULL)
	{
		// obtain file size:
		fwrite(var_str, 1, strlen(var_str), f);
		fclose(f);
	}
	else
	{
		pna_printf("error in openning file %s\n", path);
		return -1;
	}
	return 0;
}

int isDirectoryExists(const char *path)
{
    struct stat stats;

    stat(path, &stats);

    // Check for file existence
    if (S_ISDIR(stats.st_mode))
    {
        return 1;
    }

    return 0;
}
