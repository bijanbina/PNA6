#include "pna-base.h"

#include <stdlib.h>
#include <termios.h>

extern int errno;

extern struct iio_context *ctx;
extern struct iio_device *dev, *dds, *cap;

extern struct iio_buffer *dds_buffer;

extern int32_t rx1_buffer [MAX_FFT_LENGTH*2]; // FIXME: *2 should be carefully removed
extern int32_t rx2_buffer [MAX_FFT_LENGTH*2]; // FIXME: *2 should be carefully removed

int dds_sample_size;
extern int fd_dma;
extern int board_id;

unsigned char awg_data[MAX_FFT_LENGTH];

void print_error(char *function, int error_code);

struct termios saved_attributes;

void reset_input_mode (void)
{
	tcsetattr (STDIN_FILENO, TCSANOW, &saved_attributes);
}

void set_input_mode (void)
{
  struct termios tattr;
//  char *name;

  /* Make sure stdin is a terminal. */
  if (!isatty (STDIN_FILENO))
    {
      fprintf (stderr, "Not a terminal.\n");
      exit (EXIT_FAILURE);
    }

  /* Save the terminal attributes so we can restore them later. */
  tcgetattr (STDIN_FILENO, &saved_attributes);
  atexit (reset_input_mode);

  /* Set the funny terminal modes. */
  tcgetattr (STDIN_FILENO, &tattr);
  tattr.c_lflag &= ~(ICANON|ECHO); /* Clear ICANON and ECHO. */
  tattr.c_cc[VMIN] = 1;
  tattr.c_cc[VTIME] = 0;
  tcsetattr (STDIN_FILENO, TCSAFLUSH, &tattr);
}

int main (int argc, char **argv)
{
//	pna_printf("Hello world\r\n");
	char buffer[100000];
	const char delim[2] = " ";
	char* token;
	bool is_profile_empty = true;
	bool window_en = true;
	int is_2tx_2rx = 0;

	// calculating sweep time
	struct timeval  tv1, tv2;
	double sweep_time = 0;

	int fft_size, dac_max; //16bit(I)+16bit(Q) = 32bit data

	board_id = load_board_id();
	if(board_id < 0)
	{
		board_id = ETTUS_E310;
	}
	fft_size = load_rx_sample_size();
	if(fft_size <= 0)
	{
		fft_size = 1024;
		save_rx_sample_size(fft_size);
	}
	dac_max = load_dac_max();
	if(dac_max <= 0)
	{
		dac_max = DAC_MAX_VAL;
		save_dac_max(dac_max);
	}

	// fft_size = atoi(argv[1]);
	dds_sample_size = fft_size;
	// pna_printf("fft_size=%d\n", fft_size );

	init_all_gpio();
	ctx = iio_create_default_context();
	// pna_printf("flag3\r\n");
	if (ctx)
	{
		// init_device_list();
		fmcomms2_init();
		// pna_printf("flag4\r\n");
		dds_init();
		// pna_printf("flag5\r\n");
	}
	// pna_printf("flag1\r\n");
	init_rx_channel(fft_size);
	// pna_printf("flag2\r\n");
	if( argc>1 )
	{
		if( strcmp(argv[1], "-v")==0 )
		{
			float version = 0.6;
			pna_printf("Version %f \r\n", version);

			return 0;
		}
		else if( strcmp(argv[1], "--help")==0 )
		{
			pna_printf("%s\r\n",
					"Usage: pna-iio [OPTION] [VALUE] \r\n"
					 "PNA command line interface. \r\n"
					 "\r\n"
					 "   bandwidth, \t\t set RF bandwidth \r\n"
					 "	 sample_rate, \t\t set sampling frequency \r\n"
					 "   freq, \t\t    set frequency \r\n"
					 "   rx_port, \t\t set RX port, A_BALANCED - B_BALANCED \r\n"
					 "   tx_port, \t\t set TX port, A - B \r\n"
					 "   sample_size, \t\t show DDS sample size \r\n"
					 "   -v, \t\t\t show version\r\n");
			return 0;
		}
		else if( strcmp(argv[1], "console")==0 )
		{
			pna_init_interface(PNA_INTERFACE_CONSOLE);
		}
		else if( strcmp(argv[1], "tcp")==0 )
		{
			pna_init_interface(PNA_INTERFACE_TCP);
		}
		else
		{
			pna_printf("Error: interface set wrong\r\n");
			return 0;
		}
	}
	else
	{
		pna_printf("Error: interface is not set\r\n");
		return 0;
	}
	
	struct iio_channel *ch1=NULL;
	ch1 = iio_device_find_channel(dev, "voltage1", true);
	if(ch1 != NULL)
	{
		is_2tx_2rx = 1;
		pna_printf("2rx-2tx");
	}
	else
	{
		is_2tx_2rx = 0;
		pna_printf("1rx-1tx");
	}

	long long diff_freq_vna;
	long long rx_freq;
	rx_freq = get_lo_freq(__RX);
	if(board_id == ETTUS_E310)
	{
		set_rx_switches(rx_freq);
	}
	long long sw_span;
	///////////////// FIXME: in case of open failure an error should be report
	fd_dma = open("/dev/dma", O_RDWR);

	fastlock_profile tx_default_profile;

	gpio_fft(256);
	gpio_fft_reset();
	gpio_fft(fft_size);
	int ret = fft_changed(fft_size);
	if(ret < 0)
	{
		return -1;
	}

	int dc_power_vna;
	double sig_pow[2];
	sig_pow[0] = load_sig_pow(1);
	if(sig_pow[0] > 0.0)
	{
		sig_pow[0] = get_vga_gain(1);
		save_sig_pow(sig_pow[0], 1);
	}
	if(is_2tx_2rx)
	{
		sig_pow[1] = load_sig_pow(2);
		if(sig_pow[1] > 0.0)
		{
			sig_pow[1] = get_vga_gain(2);
			save_sig_pow(sig_pow[1], 2);
		}
	}
	pna_printf(START_OF_PACKET "\r\n"); // :D
	while(1)
	{
		pna_printf(">>");
		fflush(stdout);
		//scanf("%s\r", buffer);
		int ret = pna_gets(buffer, 100000);
		if(ret < 0)
		{
			pna_printf("Error while trying to read from console/tcp\r\n");
			return -1;
		}
		// printf("received: %s --- %d\r\n", buffer, ret);
		
		if(strcmp(buffer,"")==0)
		{
//			pna_printf("ridiiiii");
			break;
		}
		pna_printf(START_OF_PACKET);
		token = strtok(buffer, delim);
		if( strcmp(token, COMMAND_TXBW)==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf(COMMAND_TXBW": %lld\r\n", get_bandwidth(__TX));
			}
			else if(check_argument(token, COMMAND_TXBW, "value"))
			{
				long long bandwidth = get_frequency(token);
				set_bandwidth(__TX, bandwidth);
				pna_printf(COMMAND_TXBW": %lld\r\n", bandwidth);
			}
		}
		else if( strcmp(token, COMMAND_RXBW)==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf(COMMAND_RXBW": %lld\r\n",  get_bandwidth(__RX));
			}
			else if(check_argument(token, COMMAND_RXBW, "value"))
			{
				long long bandwidth = get_frequency(token);
				set_bandwidth(__RX, bandwidth);
				pna_printf(COMMAND_RXBW": %lld\r\n", bandwidth);
			}
		}
		else if( strcmp(token, COMMAND_DUALCH)==0 )
		{
			pna_printf(COMMAND_DUALCH": %d\r\n", is_2tx_2rx);
		}
		else if( strcmp(token, COMMAND_RFOUT) == 0)
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				long long powerdown_en = get_powerdown(__TX);
				if(board_id == ETTUS_E310)
				{
					bool switch_en = get_tx_switches(board_id);
					pna_printf(COMMAND_RFOUT": %d\r\n", (powerdown_en==0L && switch_en));
				}
				else
				{
					pna_printf(COMMAND_RFOUT": %d\r\n", (powerdown_en==0L));
				}
			}
			else if(check_argument(token, COMMAND_RFOUT, "value"))
			{
				bool enable = atoi(token);
				if(board_id == ETTUS_E310)
				{
					long long tx_freq = get_lo_freq(__TX);
					set_tx_switches(enable, tx_freq);
				}
				set_powerdown(__TX, (long long)(!enable));
				pna_printf(COMMAND_RFOUT": %d\r\n", enable);
			}
		}
		else if( strcmp(token, COMMAND_EMIO)==0 )
		{
			token = strtok(NULL, delim);
			if(!check_argument(token, COMMAND_EMIO, "base"))
			{
				continue;
			}
			int emio_base = atoi(token);
			token = strtok(NULL, delim);
			if(!check_argument(token, COMMAND_EMIO, "nch"))
			{
				continue;
			}
			int emio_nchannel = atoi(token);
			token = strtok(NULL, delim);
			if(!check_argument(token, COMMAND_EMIO, "value"))
			{
				continue;
			}
			int emio_value = atoi(token);
			set_gpio_emio(emio_base, emio_nchannel, emio_value);
			pna_printf(COMMAND_EMIO"[%d:%d]=%d\r\n", emio_base + emio_nchannel - 1, emio_base, emio_value);
		}
		else if( strcmp(token, COMMAND_SWEEPTIME)==0 ) // sweep time
		{
			pna_printf(COMMAND_SWEEPTIME": %lf \r\n",  sweep_time);
		}
		else if( strcmp(token, COMMAND_AVGWIN)==0 ) // sweep time
		{
			pna_print_avg();
		}
		else if( strcmp(token, COMMAND_WINENABLE)==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf(COMMAND_WINENABLE": %d \r\n", window_en);
			}
			else if(check_argument(token, COMMAND_WINENABLE, "value"))
			{
				window_en = atoi(token);
				pna_printf(COMMAND_WINENABLE": %d \r\n", window_en);
			}
		}
		else if( strcmp(token, COMMAND_FILLPRO)==0 )
		{
			token = strtok(NULL, delim);
			if(!check_argument(token, COMMAND_FILLPRO, "freq"))
			{
				continue;
			}
			rx_freq = get_frequency(token);
			double rx_freq_mhz = (double)rx_freq / 1E6;
			token = strtok(NULL, delim);
			if(!check_argument(token, COMMAND_FILLPRO, "span"))
			{
				continue;
			}
			sw_span = get_frequency(token);
			long long rx_sampling_frequency = 60E6; // 30M for ettus | 60M for others
			set_sample_rate(__RX, rx_sampling_frequency);
			long long bandwidth = 56E6;
			set_bandwidth(__RX, bandwidth);
			if(fft_size != 1024)
			{
				fft_size = 1024;
				gpio_fft(fft_size);
				init_rx_channel(fft_size);
				usleep(10000);
				int ret = fft_changed(fft_size);
				if(ret < 0)
				{
					return -1;
				}
			}
			usleep(10);
			is_profile_empty = false;
			double span_mhz = sw_span/1E6;
			double lo_start_freq = rx_freq_mhz - span_mhz/2;
			fill_profiles(lo_start_freq, span_mhz);
			pna_printf("Fillpro: Done!\r\n");
		}
		else if( strcmp(token, COMMAND_LOADPRO)==0 ) // sweep time
		{
			load_profile(0);
			pna_printf("\r\n");
		}
		else if( strcmp(token, COMMAND_SIGPOW) == 0)
		{
			token = strtok(NULL, delim);
			if(!check_argument(token, COMMAND_SIGPOW, "channel"))
			{
				continue;
			}
			int channel_num = atoi(token);
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf(COMMAND_SIGPOW": %d, %lf\r\n", channel_num, load_sig_pow(channel_num));
			}
			else if(check_argument(token, COMMAND_SIGPOW, "value"))
			{
				sig_pow[channel_num-1] = atof(token);
				save_sig_pow(sig_pow[channel_num-1], channel_num);
				pna_printf(COMMAND_SIGPOW": %d, %lf\r\n", channel_num, sig_pow[channel_num-1]);
			}
		}
		else if( strcmp(token, COMMAND_VGA)==0 )
		{
			token = strtok(NULL, delim);
			if(!check_argument(token, COMMAND_VGA, "channel"))
			{
				continue;
			}
			int channel_num = atoi(token);
			token = strtok(NULL, delim);

			if(is_2tx_2rx == 0)
			{
				channel_num = 1;
			}

			if(token==NULL)
			{
				pna_printf(COMMAND_VGA": %d, %lld\r\n", channel_num, get_vga_gain(channel_num));
			}
			else if(check_argument(token, COMMAND_VGA, "value"))
			{
				char *sz = NULL;
				long long vga_gain = -strtoll(token, &sz, 10);
				set_vga_gain(channel_num, vga_gain);
				pna_printf(COMMAND_VGA": %d, %lld\r\n", channel_num, vga_gain);
			}
		}
		else if( strcmp(token, COMMAND_LNA)==0 )
		{
			token = strtok(NULL, delim);
			if(!check_argument(token, COMMAND_LNA, "channel"))
			{
				continue;
			}
			int channel_num = atoi(token);
			token = strtok(NULL, delim);

			if(is_2tx_2rx == 0)
			{
				channel_num = 1;
			}

			if(token==NULL)
			{
				pna_printf(COMMAND_LNA": %d, %lld\r\n", channel_num, get_lna_gain(channel_num));
			}
			else if(check_argument(token, COMMAND_LNA, "value"))
			{
				char *sz = NULL;
				long long lna_gain = strtoll(token, &sz, 10);
				set_lna_gain(channel_num, lna_gain);
				pna_printf(COMMAND_LNA": %d, %lld\r\n", channel_num, lna_gain);
			}
		}
		else if( strcmp(token, COMMAND_AGC)==0 )
		{
			token = strtok(NULL, delim);
			int channel_num;
			if(!check_argument(token, COMMAND_AGC, "channel"))
			{
				continue;
			}
			channel_num = atoi(token);
			token = strtok(NULL, delim);
			char buf[1024];

			if(is_2tx_2rx == 0)
			{
				channel_num = 1;
			}

			if(token==NULL)
			{
				get_gain_control_mode(channel_num, buf);
				pna_printf(COMMAND_AGC": %d, %s\r\n", channel_num,  buf);
			}
			else if(check_argument(token, COMMAND_AGC, "value"))
			{
				set_gain_control_mode(channel_num, token);
				pna_printf(COMMAND_AGC": %d ,%.*s\r\n", channel_num, 3, token);
			}
		}
		else if( strcmp(token, COMMAND_VNAFREQ)==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf(COMMAND_VNAFREQ": %lld\r\n", diff_freq_vna);
			}
			else if(check_argument(token, COMMAND_VNAFREQ, "value"))
			{
				diff_freq_vna = get_frequency(token);
				pna_printf(COMMAND_VNAFREQ": %lld\r\n", diff_freq_vna);
			}
		}
		else if( strcmp(token, COMMAND_VNAPOW)==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf(COMMAND_VNAPOW": %d\r\n", dc_power_vna);
			}
			else if(check_argument(token, COMMAND_VNAPOW, "value"))
			{
				dc_power_vna = atoi(token);
				pna_printf(COMMAND_VNAPOW": %d\r\n", dc_power_vna);
			}
		}
		else if( strcmp(token, COMMAND_TXRATE)==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf(COMMAND_TXRATE": %lld\r\n", get_sample_rate(__TX));
			}
			else if(check_argument(token, COMMAND_TXRATE, "value"))
			{
				long long sampling_frequency = get_frequency(token);
				set_sample_rate(__TX, sampling_frequency);
				pna_printf(COMMAND_TXRATE": %lld\r\n", sampling_frequency);
			}
		}
		else if( strcmp(token, COMMAND_RXRATE)==0 )
		{
			long long rx_sampling_frequency;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				rx_sampling_frequency = get_sample_rate(__RX);
				pna_printf(COMMAND_RXRATE": %lld\r\n", rx_sampling_frequency);
			}
			else if(check_argument(token, COMMAND_RXRATE, "value"))
			{
				rx_sampling_frequency = get_frequency(token);
				set_sample_rate(__RX, rx_sampling_frequency);
				pna_printf(COMMAND_RXRATE": %lld\r\n", rx_sampling_frequency);
			}
		}
		else if( strcmp(token, COMMAND_RXSS)==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf(COMMAND_RXSS": %d\r\n", fft_size);
			}
			else if(check_argument(token, COMMAND_RXSS, "value"))
			{
				fft_size = atoi(token);
				gpio_fft(fft_size);
				init_rx_channel(fft_size);
				usleep(10000);
				int ret = fft_changed(fft_size);
				if(ret < 0)
				{
					return -1;
				}
				pna_printf(COMMAND_RXSS": %d\r\n", fft_size);
			}
		}
		else if( strcmp(token, COMMAND_TXSS)==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf(COMMAND_TXSS": %d\r\n", dds_sample_size);
			}
			else if(check_argument(token, COMMAND_TXSS, "value"))
			{
				dds_sample_size = atoi(token);
				pna_printf(COMMAND_TXSS": %d\r\n", dds_sample_size);
			}
		}
		else if( strcmp(token, COMMAND_CALIB)==0 )
		{
			token = strtok(NULL, delim);
			char cal_mode[100];
			if(token==NULL)
			{
				iio_device_attr_read(dev, "calib_mode", cal_mode, 100);
				pna_printf(COMMAND_CALIB": %s\r\n", cal_mode);
			}
			else if(check_argument(token, COMMAND_CALIB, "value"))
			{
//				char qcn[10];
				char qcs[10];
				char *size_null = NULL;

				iio_device_attr_write(dev, "calib_mode", token);
				usleep(10000);
//				read_reg_ad9361(160, qcn); // Quad Cal NCO Freq and Phase Offset | 0xA0
				read_reg_ad9361(168, qcs);  // Quad Cal Status TX2 | 0xA8
				long qcs_l = strtol(qcs+2, &size_null, 16);
//				long qcn_l = strtol(qcn+2, &size_null, 16);
				uint8_t qcs8 = (uint8_t)qcs_l;
//				uint8_t qcn8 = (uint8_t)qcn_l;
				uint8_t conv = qcs8 & 0x01;
//				uint8_t conv_cnt = qcs8 & 0xFC;
//				conv_cnt = conv_cnt >> 2;
//				qcn8 = qcn8 & 0x1F;
//				pna_printf("NCO phase: %d | conv: %d | conv_count: %d\r\n", qcn8, conv, conv_cnt);
				pna_printf(COMMAND_CALIB": %s, %d\r\n", token, conv);
			}
		}
		else if( strcmp(token, COMMAND_TXFREQ_CAL)==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				long long freq = get_lo_freq(__TX);
				int cal_success = calibrate_tx(freq);
				pna_printf(COMMAND_TXFREQ_CAL": %lld, %d\r\n", freq, cal_success);
			}
			else if(check_argument(token, COMMAND_TXFREQ_CAL, "value"))
			{
				long long freq = get_frequency(token);
				set_lo_freq(__TX, freq);
				usleep(10000);
				int cal_success = calibrate_tx(freq);
				pna_printf(COMMAND_TXFREQ_CAL": %lld, %d\r\n", freq, cal_success);
			}
		}
		else if( strcmp(token, COMMAND_TXFREQ)==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf(COMMAND_TXFREQ": %lld\r\n", get_lo_freq(__TX));
			}
			else if(check_argument(token, COMMAND_TXFREQ, "value"))
			{
				long long freq = get_frequency(token);
				set_lo_freq(__TX, freq);
				pna_printf(COMMAND_TXFREQ": %lld\r\n", freq);
			}
		}
		else if( strcmp(token, COMMAND_RXFREQ)==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				rx_freq = get_lo_freq(__RX);
				pna_printf(COMMAND_RXFREQ": %lld\r\n", rx_freq);
			}
			else if(check_argument(token, COMMAND_RXFREQ, "value"))
			{
				rx_freq = get_frequency(token);
				set_lo_freq(__RX, rx_freq);
				pna_printf(COMMAND_RXFREQ": %lld\r\n", rx_freq);
			}
		}
		else if( strcmp(token, COMMAND_RXPORT)==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				char rx_port[100];
				get_port(__RX, rx_port);
				pna_printf(COMMAND_RXPORT": %s\r\n", rx_port);
			}
			else
			{
				set_port(__RX, token);
				pna_printf(COMMAND_RXPORT": %s \r\n", token);
			}
		}
		else if( strcmp(token, COMMAND_TXPORT)==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				char tx_port[100];
				get_port(__TX, tx_port);
				pna_printf(COMMAND_TXPORT": %s \r\n", tx_port);
			}
			else
			{
				set_port(__TX, token);
				pna_printf(COMMAND_TXPORT": %s \r\n", token);
			}
		}
		else if( strcmp(token, COMMAND_SAMSZ)==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				int sample_size = get_sample_size();
				pna_printf("sample_size: %d \r\n", sample_size);
			}
			else
			{
				pna_printf("\r\n");
			}
		}
		else if( strcmp(token, COMMAND_RXFIR)==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf(COMMAND_RXFIR": %d \r\n", get_fir_en(__RX));
			}
			else if(check_argument(token, COMMAND_RXFIR, "value"))
			{
				bool fir_en = atoi(token);
				set_fir_en(__RX, fir_en);
				pna_printf(COMMAND_RXFIR": %d \r\n", fir_en);
			}
		}
		else if( strcmp(token, COMMAND_TXFIR)==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf(COMMAND_TXFIR": %d \r\n", get_fir_en(__TX));
			}
			else if(check_argument(token, COMMAND_TXFIR, "value"))
			{
				bool fir_en = atoi(token);
				set_fir_en(__TX, fir_en);
				pna_printf(COMMAND_TXFIR": %d \r\n", fir_en);
			}
		}
		else if( strcmp(token, COMMAND_BBDC) == 0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf(COMMAND_BBDC": %d \r\n", get_bb_dc());
			}
			else if(check_argument(token, COMMAND_BBDC, "value"))
			{
				bool bb_dc_en = atoi(token);
				set_bb_dc(bb_dc_en);
				pna_printf(COMMAND_BBDC": %d \r\n", bb_dc_en);
			}
		}
		else if( strcmp(token, COMMAND_RFDC) == 0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf("rf_dc_enable: %d \r\n", get_rf_dc());
			}
			else if(check_argument(token, COMMAND_RFDC, "value"))
			{
				bool rf_dc_en = atoi(token);
				set_rf_dc(rf_dc_en);
				pna_printf(COMMAND_RFDC": %d \r\n", rf_dc_en);
			}
		}
		else if( strcmp(token, COMMAND_QUAD) == 0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf(COMMAND_QUAD": %d \r\n", get_quad_track());
			}
			else if(check_argument(token, COMMAND_QUAD, "value"))
			{
				bool quad_en = atoi(token);
				set_quad_track(quad_en);
				pna_printf(COMMAND_QUAD": %d \r\n", quad_en);
			}
		}
		else if(strcmp(token, COMMAND_TEST) == 0)
		{
			pid_t pid;
			int len = 10, ind = 0, kill_bache = 0;
			int *sum;
			int shmid1,shmid2;
			int *shm_ptr1;
			char *shm_ptr2;
			key_t key=1234;
			char *data;

			shmid1=shmget(key, sizeof(int), 0666 | IPC_CREAT);
			shmid2=shmget(key+1, 1024*sizeof(char), 0666 | IPC_CREAT);
			if( shmid1<0 || shmid2<0)
			{
				pna_printf("shared memory get 1, error: %d\r\n", errno);
				continue;
			}

			shm_ptr1=(int *)shmat(shmid1,(void *)0,0);
			shm_ptr2=(char *)shmat(shmid2,(void *)0,0);
			if( shm_ptr1==(int *)(-1) || shm_ptr2==(char *)(-1) )
			{
				pna_printf("shared memory attach error: %d\r\n", errno);
				continue;
			}

			sum = shm_ptr1;
			data = shm_ptr2;

			for( int i=0; i<1; i++ )
			{
				if( kill_bache )
				{
					break;
				}
				*sum = 0;
				pid = fork();

				if( pid==0 ) // child process : get data
				{
					while(*sum < len)
					{
//						int ret = fread(data + *sum, 1, len - *sum, stdin); fread adds \0 to the end of string
						int ret = read(STDIN_FILENO, data + *sum, len - *sum);
						*sum += ret;
					}
					exit(0);
				}
				else // parent process : child watch-dog
				{
					while(1)
					{
						int last_sum = *sum;
						usleep(9E5);
						usleep(9E5);

						if( last_sum>=len )
						{
							pna_printf("\r\nJOB IS DONE! data:%10s", data);
							ind += len;
							break;
						}
						else if( last_sum==*sum )
						{
							pna_printf("\r\nKILLED BACHE!");
							kill(pid, SIGKILL);
							kill_bache = 1;

							break;
						}
					}
				}
			}

			fflush(stdin);
			// u should memmove() shm_ptr
			shmdt(shm_ptr1);
			shmdt(shm_ptr2);
			shmctl(shmid1, IPC_RMID, 0);
			shmctl(shmid2, IPC_RMID, 0);
//			for( int i=0; i<fft_size; i++ )
//			{
//				char first_byte = i%256;
//				char second_byte = i/256;
//				pna_printf("%d %d\r\n",first_byte, second_byte);
//			}
			pna_printf("\r\n");
		}
        else if(strcmp(token, COMMAND_FRESET) == 0)
		{
			gpio_fft_reset();
			pna_printf("\r\n");
		}
        else if(strcmp(token, COMMAND_DACMAX) == 0)
		{
        	token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf(COMMAND_DACMAX": %d \r\n", load_dac_max());
			}
			else if(!check_argument(token, COMMAND_DACMAX, "value"))
			{
				int dac_max = atoi(token);
				save_dac_max(dac_max);
				pna_printf(COMMAND_DACMAX": %d \r\n", dac_max);
			}
		}
		else if(strcmp(token, PLOT_ADC) == 0)
		{
			token = strtok(NULL, delim);
			if(!check_argument(token, PLOT_ADC, "channel"))
			{
				continue;
			}
			int channel_num = atoi(token) - 1;
			if(channel_num)
			{
				pna_adc(rx2_buffer, fft_size);
			}
			else
			{
				pna_adc(rx1_buffer, fft_size);
			}
		}
		else if(strcmp(token, PLOT_ADCTRIG) == 0)
		{
			token = strtok(NULL, delim);
			if(!check_argument(token, PLOT_ADCTRIG, "mode"))
			{
				continue;
			}
			int mode = atoi(token);
			token = strtok(NULL, delim);
			if(!check_argument(token, PLOT_ADCTRIG, "level"))
			{
				continue;
			}
			int level = atoi(token);
			token = strtok(NULL, delim);
			if(!check_argument(token, PLOT_ADCTRIG, "channel"))
			{
				continue;
			}
			int channel_num = atoi(token) - 1;
			token = strtok(NULL, delim);
			if(!check_argument(token, PLOT_ADCTRIG, "zip"))
			{
				continue;
			}
			bool compression_enable = atoi(token);
			if(is_2tx_2rx == 0)
			{
				fill_rx_buffer_single(fft_size);
			}
			else
			{
				fill_rx_buffer(fft_size);
			}
			int uart_size;
			int32_t *adc_data;
			unsigned char uart_tx_buffer[4*MAX_FFT_LENGTH];

			if(channel_num && is_2tx_2rx)
			{
				adc_data = rx2_buffer;
			}
			else
			{
				adc_data = rx1_buffer;
			}

			adc_data = pna_trig_adc(adc_data, fft_size/2, mode, level);

			fill_output_buffer_iq(adc_data, uart_tx_buffer, fft_size/2);
			uart_size = fft_size/2;
			pna_printf("PLOT");
			pna_write(uart_tx_buffer, 4*uart_size);
			pna_printf("\r\n");
		}
		else if(strcmp(token, PLOT_ADCIQ) == 0)
		{
			token = strtok(NULL, delim);
			if(!check_argument(token, PLOT_ADCIQ, "channel"))
			{
				continue;
			}
			int channel_num = atoi(token) - 1;
			token = strtok(NULL, delim);
			if(!check_argument(token, PLOT_ADCIQ, "zip"))
			{
				continue;
			}
			bool compression_enable = atoi(token);
			if(is_2tx_2rx == 0)
			{
				fill_rx_buffer_single(fft_size);
			}
			else
			{
				fill_rx_buffer(fft_size);
			}
			int uart_size;
			int32_t *adc_data;
			unsigned char uart_tx_buffer[4*MAX_FFT_LENGTH];

			if(channel_num && is_2tx_2rx)
			{
				adc_data = rx2_buffer;
			}
			else
			{
				adc_data = rx1_buffer;
			}

//			if(window_en)
//			{
//				flat_top_window(adc_data, fft_size);
//			}

//			if(compression_enable)
//			{
//				uart_size = compress_data_iq(adc_data, uart_tx_buffer, fft_size);
//			}
//			else
//			{
				fill_output_buffer_iq(adc_data, uart_tx_buffer, fft_size);
				uart_size = fft_size;
//			}
			pna_printf("PLOT");
			pna_write(uart_tx_buffer, 4*uart_size);
			pna_printf("\r\n");
		}
		else if(strcmp(token, PLOT_ADCVNA) == 0)
		{
			if(!is_2tx_2rx)
			{
				pna_printf("PLOT\r\n");
				continue;
			}
			fill_rx_buffer(fft_size);
			if(window_en)
			{
				flat_top_window(rx1_buffer, fft_size);
				flat_top_window(rx2_buffer, fft_size);
			}
			unsigned char uart_tx_buffer[8*MAX_FFT_LENGTH];

			fill_output_buffer_2ch(rx1_buffer, rx2_buffer, uart_tx_buffer, fft_size);

			pna_printf("PLOT");
			pna_write(uart_tx_buffer, 8*fft_size);
			pna_printf("\r\n");
		}
		else if(strcmp(token, PLOT_ADCCAL) == 0)
		{
			token = strtok(NULL, delim);
			if(!check_argument(token, PLOT_ADCCAL, "len"))
			{
				continue;
			}
			int cal_data_size = atoi(token);
			token = strtok(NULL, delim);
			if(!check_argument(token, PLOT_ADCCAL, "channel"))
			{
				continue;
			}
			int channel_rx_cal = atoi(token) - 1;
			if(is_2tx_2rx == 0)
			{
				fill_rx_buffer_single(cal_data_size);
			}
			else
			{
				fill_rx_buffer(cal_data_size);
			}
//			if(window_en)
//			{
//				flat_top_window(rx1_buffer, fft_size);
//				flat_top_window(rx2_buffer, fft_size);
//			}
			unsigned char uart_tx_buffer[4*MAX_FFT_LENGTH];
			if(channel_rx_cal && is_2tx_2rx)
			{
				fill_output_buffer_iq(rx2_buffer, uart_tx_buffer, cal_data_size);
			}
			else
			{
				fill_output_buffer_iq(rx1_buffer, uart_tx_buffer, cal_data_size);
			}

			pna_printf("PLOT");
			pna_write(uart_tx_buffer, 4*cal_data_size);
			pna_printf("\r\n");
		}
		else if(strcmp(token, PLOT_ADCFFT) == 0)
		{
			token = strtok(NULL, delim);
			if(!check_argument(token, PLOT_ADCFFT, "channel"))
			{
				continue;
			}
			int channel_num = atoi(token) - 1;
			if(channel_num && is_2tx_2rx)
			{
				pna_adc_fft(rx2_buffer, fft_size);
			}
			else
			{
				pna_adc_fft(rx1_buffer, fft_size);
			}
		}
		else if(strcmp(token, PLOT_SWEEP2) == 0)
		{
			token = strtok(NULL, delim);
			if(!check_argument(token, PLOT_SWEEP2, "channel"))
			{
				continue;
			}
			int channel_num = atoi(token) - 1;
			token = strtok(NULL, delim);
			if(!check_argument(token, PLOT_SWEEP2, "zip"))
			{
				continue;
			}
			int compression_enable = atoi(token);
			if(sw_span < 0)
			{
				sw_span = 80E6;
			}
			double span_mhz = sw_span/1E6;

			if(is_profile_empty)
			{
				double rx_freq_mhz = rx_freq/1E6;
				double lo_start_freq = rx_freq_mhz - span_mhz/2;
				is_profile_empty = false;
				fill_profiles(lo_start_freq, span_mhz);
			}

			int32_t *spectrum;
			int sweep_index = 0;
			// unsigned char uart_tx_buffer[2*UART_LENGTH];
			double rx_sampling_frequency_mhz = 60.0; // 30 for Ettus | 60 for others
			
			int CHUNK_C = fft_size/6; // 1/6 = SWEEP_SPAN/rx_sampling_frequency_mhz/2
			int span_num = 2*floor(span_mhz / 2 / SWEEP_SPAN);
			int span_int = (int)span_mhz;

			gettimeofday(&tv1, NULL);

			if(span_int % (2*SWEEP_SPAN) > 0)
			{
				span_num += 2;
			}
			int span_count = span_num * CHUNK_C * 2;
			unsigned char *uart_tx_buffer = (unsigned char *) malloc(2*span_count * sizeof(unsigned char));
			if(uart_tx_buffer == NULL)
			{
				printf("memory allocation failed in sweep2\r\n");
				return -1;
			}
			int32_t *sweep_buf = (int32_t*) malloc(span_count * sizeof(int32_t));
			if(sweep_buf == NULL)
			{
				printf("memory allocation failed in sweep2\r\n");
				free(uart_tx_buffer);
				return -1;
			}

			double spur_count = span_num * SWEEP_SPAN - span_mhz;
			spur_count = spur_count*fft_size/rx_sampling_frequency_mhz;
			spur_count = floor(spur_count);
			// pna_printf("span_num: %d, span_mhz: %lf, rx_sampling_frequency_mhz: %lf\r\n"
			// 		"span_count: %d, spur_count: %lf\r\n", span_num, span_mhz, 
			// 		rx_sampling_frequency_mhz, span_count, spur_count);
			int32_t *adc_data;
			if(channel_num && is_2tx_2rx)
			{
				adc_data = rx2_buffer;
			}
			else
			{
				adc_data = rx1_buffer;
			}
			for(int i=0; i<span_num/2; i++)
			{
				spectrum = pna_fft_dcfixed2(adc_data, fft_size, i, window_en);

				if(spectrum == NULL)
				{
					free(sweep_buf);
					free(uart_tx_buffer);
					printf("spectrum is null from pna_fft_dcfixed2 function\r\n");
					return -1;
				}
				for(int j=0; j<4*CHUNK_C; j++)
				{
					sweep_buf[sweep_index] = spectrum[j];
					sweep_index++;
				}
			}

			int uart_size;
			if(compression_enable)
			{
				uart_size = compress_data(sweep_buf, uart_tx_buffer, span_count - spur_count);
			}
			else
			{
				fill_output_buffer(sweep_buf, uart_tx_buffer, span_count - spur_count);
				uart_size = span_count - spur_count;
			}
			
			gettimeofday(&tv2, NULL);
			sweep_time = (double) (tv2.tv_usec - tv1.tv_usec) / 1000000;
			sweep_time += (double) (tv2.tv_sec - tv1.tv_sec);

			pna_printf("PLOT");
			pna_write(uart_tx_buffer, 2*uart_size);
			pna_printf("\r\n");
			free(sweep_buf);
			free(uart_tx_buffer);
		}
		else if(strcmp(token, PLOT_SWEEP) == 0)
		{
			token = strtok(NULL, delim);
			// long long span_sweep;
			if(!check_argument(token, PLOT_SWEEP, "channel"))
			{
				continue;
			}
			int channel_num = atoi(token) - 1;
			token = strtok(NULL, delim);
			if(!check_argument(token, PLOT_SWEEP, "span"))
			{
				continue;
			}
			long long sw_span = get_frequency(token);
			// pna_printf("sw_span : %lld\r\n", sw_span);

			int32_t *spectrum;
			int sweep_index = 0;
			unsigned char uart_tx_buffer[2*UART_LENGTH];
			double rx_sampling_frequency_mhz = 60.0;
			if(sw_span < 0)
				sw_span = 80E6;
			double span_mhz = sw_span / 1E6;
			int CHUNK_C = fft_size/6; // 1/6 = SWEEP_SPAN/rx_sampling_frequency_mhz/2
			long long lo_start_freq = rx_freq - sw_span/2.0;
			int span_num = 2*floor(span_mhz / 2 / SWEEP_SPAN);
			int span_int = (int)span_mhz;

			gettimeofday(&tv1, NULL);

			if(span_int % (2*SWEEP_SPAN) > 0)
			{
				span_num += 2;
			}
			int span_count = span_num * CHUNK_C * 2;
			int32_t *sweep_buf = (int32_t*) malloc(span_count * sizeof(int32_t));
			double spur_count = span_num * SWEEP_SPAN - span_mhz;
			
			spur_count = spur_count*fft_size/rx_sampling_frequency_mhz;
			spur_count = floor(spur_count);
			// pna_printf("span_num: %d, span_mhz: %lf, rx_sampling_frequency_mhz: %lf\r\n"
			// 		"span_count: %d, spur_count: %lf\r\n", span_num, span_mhz, 
			// 		rx_sampling_frequency_mhz, span_count, spur_count);
			for(int i=0; i<span_num/2; i++)
			{
				if(channel_num)
				{
					spectrum = pna_fft_dcfixed(rx2_buffer, lo_start_freq, fft_size, window_en);
				}
				else
				{
					spectrum = pna_fft_dcfixed(rx1_buffer, lo_start_freq, fft_size, window_en);
				}
				if(spectrum == NULL)
				{
					free(sweep_buf);
					return -1;
				}
				for(int j=0; j<4*CHUNK_C; j++)
				{
					sweep_buf[sweep_index] = spectrum[j];
					sweep_index++;
				}
				lo_start_freq += 2*SWEEP_SPAN*1E6;
			}
			int uart_size = compress_data(sweep_buf, uart_tx_buffer, span_count - spur_count);

			gettimeofday(&tv2, NULL);
			sweep_time = (double) (tv2.tv_usec - tv1.tv_usec) / 1000000;
			sweep_time += (double) (tv2.tv_sec - tv1.tv_sec);

			pna_printf("PLOT");
			pna_write(uart_tx_buffer, 2*uart_size);
			set_lo_freq(__RX, rx_freq);
			usleep(SET_LO_DELAY);
			pna_printf("\r\n");
			free(sweep_buf);
			// free(uart_tx_buffer);
		}
		else if(strcmp(token, PLOT_FFTSPAN) == 0)
		{
			token = strtok(NULL, delim);
			if(!check_argument(token, PLOT_FFTSPAN, "span"))
			{
				continue;
			}
			double span_ratio = atof(token);
			token = strtok(NULL, delim);
			if(!check_argument(token, PLOT_FFTSPAN, "channel"))
			{
				continue;
			}
			int channel_num = atoi(token) - 1;
			token = strtok(NULL, delim);
			if(!check_argument(token, PLOT_FFTSPAN, "zip"))
			{
				continue;
			}
			bool compression_enable = atoi(token);

			int32_t *spectrum;
			unsigned char uart_tx_buffer[4*MAX_FFT_LENGTH];
			if(span_ratio>1 || span_ratio<=0)
			{
				span_ratio = 1;
			}
			int removed_span = fft_size*(1-span_ratio)/2;
			// pna_printf("rx-freq %lf, span %f\r\n",rx_sampling_frequency_mhz, span);

			gettimeofday(&tv1, NULL);

			if(channel_num && is_2tx_2rx)
			{
				spectrum = pna_fft(rx2_buffer, removed_span, fft_size, window_en);
			}
			else
			{
				spectrum = pna_fft(rx1_buffer, removed_span, fft_size, window_en);
			}
			if(spectrum == NULL)
				return -1;
			int spectrum_size = fft_size*span_ratio;
			int uart_size;
			if(compression_enable)
			{
				uart_size = compress_data(spectrum, uart_tx_buffer, spectrum_size);
			}
			else
			{
				fill_output_buffer(spectrum, uart_tx_buffer, spectrum_size);
				uart_size = spectrum_size;
			}
			gettimeofday(&tv2, NULL);
			sweep_time = (double) (tv2.tv_usec - tv1.tv_usec) / 1000000;
			sweep_time += (double) (tv2.tv_sec - tv1.tv_sec);
			/////////////////////// DELETE ME
//			for(int i=0;i<2*uart_size; i++)
//			{
//				uart_tx_buffer[i]=0;
//			}
			/////////////////////////////////////////
			pna_printf("PLOT");
			pna_write(uart_tx_buffer, 2*uart_size);
			pna_printf("\r\n"); // \r\n%d  , 2*uart_size
			free(spectrum);
		}
		else if(strcmp(token, PLOT_FFT) == 0)
		{
			int32_t *spectrum = pna_fft(rx1_buffer, 0, fft_size, window_en);
			unsigned char uart_tx_buffer[4*UART_LENGTH];
			int uart_size = compress_data(spectrum, uart_tx_buffer, fft_size);
			pna_printf("PLOT");
			pna_write(uart_tx_buffer, 2*uart_size);
			pna_printf("\r\n");
			free(spectrum);
		}
		else if(strcmp(token, PLOT_FFT2) == 0)
		{
			pna_fft2(rx1_buffer, fft_size);
		}
		else if(strcmp(token, PLOT_FFT3) == 0)
		{
			pna_fft3(rx1_buffer, fft_size);
		}
		else if(strcmp(token, PLOT_FFT4) == 0)
		{
			token = strtok(NULL, delim);
			if(!check_argument(token, PLOT_FFT4, "rw"))
			{
				continue;
			}
			int rwfunc = atoi(token);
			token = strtok(NULL, delim);
			if(!check_argument(token, PLOT_FFT4, "len"))
			{
				continue;
			}
			int num_bytes = atoi(token);
			uint32_t uart_tx_buffer[MAX_FFT_LENGTH*2];
			pna_printf("fft_status#1 = %d\r\n", gpio_fft_status());
			if(rwfunc)
			{
				for(int k=0; k<num_bytes; k++)
				{
					uart_tx_buffer[k] = k % 256;
				}
				long dma_ret = write_DMA((unsigned char *)uart_tx_buffer, num_bytes);
				pna_printf("write %ld bytes to dma.\r\n", dma_ret);
			}
			else
			{
				long dma_ret = read_DMA((unsigned char *)uart_tx_buffer, num_bytes);
				pna_printf("read %ld bytes from dma.\r\n", dma_ret);
			}
			pna_printf("fft_status#2 = %d\r\n", gpio_fft_status());
			pna_printf("\r\n");
		}
		else if(strcmp(token, COMMAND_ACC) == 0)
		{
			token = strtok(NULL, delim);
			int input_a = atoi(token);
			token = strtok(NULL, delim);
			int input_b = atoi(token);

			char c_input[64];

			for(int i=0; i<8; i++)
			{
				c_input[8*i] = 0;
				c_input[8*i+1] = 0;
				c_input[8*i+2] = (input_a & 0x000000ff);
				c_input[8*i+3] = 1;

				c_input[8*i+4] = 0;
				c_input[8*i+5] = 0;
				c_input[8*i+6] = (input_b & 0x000000ff);
				c_input[8*i+7] = 1;
			}

			write_DMA((char *)c_input, 64);
			pna_printf("############");
			read_DMA((char *)c_input, 64);
		}
		else if(strcmp(token, COMMAND_SCAL) == 0)
		{
			token = strtok(NULL, delim);
			if(!check_argument(token, COMMAND_SCAL, "freq"))
			{
				continue;
			}
			long long rx_freq_cal = get_frequency(token);

			token = strtok(NULL, delim);
			if(!check_argument(token, COMMAND_SCAL, "point"))
			{
				continue;
			}
			int point_cal = atoi(token);

			int channel_rx_cal, channel_tx_cal;
			token = strtok(NULL, delim);
			if(!check_argument(token, COMMAND_SCAL, "channel"))
			{
				continue;
			}
			channel_rx_cal = atoi(token)-1;
			token = strtok(NULL, delim);
			if(!check_argument(token, COMMAND_SCAL, "channel"))
			{
				continue;
			}
			channel_tx_cal = atoi(token)-1;

			token = strtok(NULL, delim);
			if(!check_argument(token, COMMAND_SCAL, "lna gain"))
			{
				continue;
			}
			char *size_null = NULL;
			long long lna_gain_cal = strtoll(token, &size_null, 10);

			token = strtok(NULL, delim);
			if(!check_argument(token, COMMAND_SCAL, "power"))
			{
				continue;
			}
			double power = atof(token);
			if(is_2tx_2rx == 0)
			{
				channel_rx_cal = 0;
				channel_tx_cal = 0;
			}

			double freq_mhz = 61.44*((double)point_cal/PNT_NUM_CAL - 0.5);
			long long tx_freq_cal = rx_freq_cal + (long long)(freq_mhz * 1e6);
			set_lo_freq(__RX, rx_freq_cal);
			set_lo_freq(__TX, tx_freq_cal);
			set_lna_gain(channel_rx_cal+1, lna_gain_cal);

			double dig_pow, dig_amp;
			if(power > -70.0)
			{
				long long vga_part = (long long)ceil(power);
				if(vga_part < -70)
				{
					vga_part = -70;
					pna_printf("vga set to -70, cannot set values less than -70\n");
				}
				else if(vga_part > 0)
				{
					pna_printf("vga set to 0, cannot set values more than 0\n");
					vga_part = 0.0;
				}
				set_vga_gain(channel_tx_cal+1, vga_part);
				dig_pow = power - ceil(power);
			}
			else
			{
				set_vga_gain(channel_tx_cal+1, -70);
				dig_pow = power + 70.0;
			}
			dig_amp = pow(10.0, dig_pow / 20.0);

			int amplitude_int = dig_amp*dac_max/2;

			send_dc_signal(dds_sample_size, amplitude_int, channel_tx_cal);

			pna_printf(COMMAND_SCAL": Done!\r\n");
		}
		else if(strcmp(token, COMMAND_SVNA) == 0)
		{
			token = strtok(NULL, delim);
			if(!check_argument(token, COMMAND_SVNA, "freq"))
			{
				continue;
			}
			long long vna_freq = get_frequency(token);

			token = strtok(NULL, delim);
			if(!check_argument(token, COMMAND_SVNA, "lna gain"))
			{
				continue;
			}
			char *size_null = NULL;
			long long lna_gain1 = strtoll(token, &size_null, 10);

			token = strtok(NULL, delim);
			if(!check_argument(token, COMMAND_SVNA, "lna gain"))
			{
				continue;
			}
			long long lna_gain2 = strtoll(token, &size_null, 10);

			token = strtok(NULL, delim);
			if(!check_argument(token, COMMAND_SVNA, "vga gain"))
			{
				continue;
			}
			long long vga_gain1 = strtoll(token, &size_null, 10);

			token = strtok(NULL, delim);
			if(!check_argument(token, COMMAND_SVNA, "vga gain"))
			{
				continue;
			}
			if(!is_2tx_2rx)
			{
				pna_printf("Board has only one channel and cannot run VNA commands.\r\n");
				continue;
			}
			long long vga_gain2 = strtoll(token, &size_null, 10);

			set_lo_freq(__RX, vna_freq);
			set_lo_freq(__TX, vna_freq + diff_freq_vna);

			set_vga_gain(1, vga_gain1);
			set_vga_gain(2, vga_gain2);
			set_lna_gain(1, lna_gain1);
			set_lna_gain(2, lna_gain2);

			send_dc_signal(dds_sample_size, dc_power_vna, 0);
			pna_printf(COMMAND_SVNA": Done!\r\n");
		}
		else if(strcmp(token, WAVE_SINIQ) == 0)
		{
			int sig_iq;
			token = strtok(NULL, delim);
			if(!check_argument(token, WAVE_SINIQ, "sig_iq"))
			{
				continue;
			}
			sig_iq = atoi(token);
			int period_num;
			token = strtok(NULL, delim);
			if(!check_argument(token, WAVE_SINIQ, "period"))
			{
				continue;
			}
			period_num = atoi(token);
			double sig_offset;
			token = strtok(NULL, delim);
			if(!check_argument(token, WAVE_SINIQ, "offset"))
			{
				continue;
			}
			sig_offset = atof(token);
			int channel_num;
			token = strtok(NULL, delim);
			if(!check_argument(token, WAVE_SINIQ, "channel"))
			{
				continue;
			}
			channel_num = atoi(token) - 1;
			if(is_2tx_2rx == 0)
			{
				channel_num = 0;
			}
			double dig_pow, dig_amp;
			if(sig_pow[channel_num] + sig_offset > -70.0)
			{
				long long vga_part = (long long)ceil(sig_pow[channel_num] + sig_offset);
				set_vga_gain(channel_num+1, vga_part);
				dig_pow =sig_pow[channel_num] + sig_offset - ceil(sig_pow[channel_num] + sig_offset);
			}
			else
			{
				set_vga_gain(channel_num+1, -70);
				dig_pow = sig_pow[channel_num] + sig_offset + 70.0;
			}
			dig_amp = pow(10.0, dig_pow / 20.0);
			token = strtok(NULL, delim);
			if(token != NULL)
			{
				if(!check_argument(token, WAVE_SINIQ, "dig_amp"))
				{
					dig_amp = atof(token);
				}
			}
			int amplitude_int = dig_amp*dac_max/2;
			send_sin_signal(dds_sample_size, amplitude_int, period_num, sig_iq,  channel_num);
			pna_printf("[I/Q/SSB]: %d | [period]: %d | [port]: %d | [sigpow]: %2.2lf | [dig_pow]: %2.2lf | [dig_amp]: %1.3lf\r\n",
					sig_iq, period_num, channel_num, sig_pow[channel_num]+sig_offset, dig_pow, dig_amp);
		}
		else if(strcmp(token, WAVE_PULSEIQ) == 0)
		{
			token = strtok(NULL, delim);
			if(!check_argument(token, WAVE_PULSEIQ, "sig_iq"))
			{
				continue;
			}
			int sig_iq = atoi(token);
			token = strtok(NULL, delim);
			if(!check_argument(token, WAVE_PULSEIQ, "period"))
			{
				continue;
			}
			int period_num = atoi(token);
			token = strtok(NULL, delim);
			if(!check_argument(token, WAVE_PULSEIQ, "duty_cycle"))
			{
				continue;
			}
			float duty_cycle = atof(token);
			token = strtok(NULL, delim);
			if(!check_argument(token, WAVE_PULSEIQ, "offset"))
			{
				continue;
			}
			double sig_offset = atof(token);
			token = strtok(NULL, delim);
			if(!check_argument(token, WAVE_PULSEIQ, "channel"))
			{
				continue;
			}
			int channel_num = atoi(token) - 1;
			if(is_2tx_2rx == 0)
			{
				channel_num = 0;
			}
			double dig_pow, dig_amp;
			if(sig_pow[channel_num] + sig_offset > -70.0)
			{
				long long vga_part = (long long)ceil(sig_pow[channel_num] + sig_offset);
				set_vga_gain(channel_num+1, vga_part);
				dig_pow =sig_pow[channel_num] + sig_offset - ceil(sig_pow[channel_num] + sig_offset);
			}
			else
			{
				set_vga_gain(channel_num+1, -70);
				dig_pow = sig_pow[channel_num] + sig_offset + 70.0;
			}
			dig_amp = pow(10.0, dig_pow / 20.0);
			token = strtok(NULL, delim);
			if(token != NULL)
			{
				if(check_argument(token, WAVE_PULSEIQ, "dig_amp"))
				{
					dig_amp = atof(token);
				}
			}

			int amplitude_int = dig_amp*dac_max/2;
			send_pulse_signal(dds_sample_size, amplitude_int, period_num, duty_cycle, sig_iq, channel_num);
			pna_printf("[I/Q/SSB]: %d | [period]: %d | [duty-cycle]: %.2lf | [port]: %d | [sigpow]: %2.2lf | [dig_pow]: %2.2lf | [dig_amp]: %1.3lf\r\n",
								sig_iq, period_num, duty_cycle, channel_num, sig_pow[channel_num]+sig_offset, dig_pow, dig_amp);
		}
		else if(strcmp(token, WAVE_TRIANGLE) == 0)
		{
			token = strtok(NULL, delim);
			if(!check_argument(token, WAVE_TRIANGLE, "period"))
			{
				continue;
			}
			int period_num = atoi(token);
			token = strtok(NULL, delim);
			if(!check_argument(token, WAVE_TRIANGLE, "channel"))
			{
				continue;
			}
			int channel_num = atoi(token) - 1;
			if(is_2tx_2rx == 0)
			{
				channel_num = 0;
			}

			int amplitude_int = dac_max/2;
			send_triangle_signal(dds_sample_size, amplitude_int, period_num, channel_num);
			pna_printf("[period]: %d | [port]: %d\r\n", period_num, channel_num, sig_pow[channel_num]);
		}
		else if(strcmp(token, WAVE_AWG) == 0)
		{
			int len_awg;
			const int step_receive = 32;
			const int packet_len = 256;
			char progress_bar[33];
			strcpy(progress_bar, "|------------------------------|");

			token = strtok(NULL, delim);
			if(!check_argument(token, WAVE_AWG, "len"))
			{
				continue;
			}
			len_awg = atoi(token);
			int total_len = AWG_NUM_WIDTH * len_awg * 2; // 2 for [I/Q]
			double sig_offset;
			token = strtok(NULL, delim);
			if(!check_argument(token, WAVE_AWG, "offset"))
			{
				continue;
			}
			sig_offset = atof(token);
			int channel_num;
			token = strtok(NULL, delim);
			if(!check_argument(token, WAVE_AWG, "channel"))
			{
				continue;
			}
			channel_num = atoi(token) - 1;
			if(is_2tx_2rx == 0)
			{
				channel_num = 0;
			}
			double dig_pow, dig_amp;
			if(sig_pow[channel_num] + sig_offset > -70.0)
			{
				long long vga_part = (long long)ceil(sig_pow[channel_num] + sig_offset);
				set_vga_gain(channel_num+1, vga_part);
				dig_pow =sig_pow[channel_num] + sig_offset - ceil(sig_pow[channel_num] + sig_offset);
			}
			else
			{
				set_vga_gain(channel_num+1, -70);
				dig_pow = sig_pow[channel_num] + sig_offset + 70.0;
			}
			dig_amp = pow(10.0, dig_pow / 20.0);
			token = strtok(NULL, delim);
			if(token != NULL)
			{
				dig_amp = atof(token);
			}

			for(int i=0; i<total_len; i++)
			{
				awg_data[i] = 0;
			}

			unsigned char chert[2];

			int escape_char_set = 1, escape_char_received = 0;
			int total_packets = total_len / packet_len;
			int rem_packet_len = total_len % packet_len;
			int receives_per_packet = packet_len / step_receive;
			int awg_data_index = 0;

			pna_printf("Packets:%d Rem:%d RPP:%d\r\n>>", total_packets, rem_packet_len, receives_per_packet);
			fflush(stdout);

			int i;
			for(i=0; i<total_packets; i++)
			{
				for(int j=0; j<receives_per_packet; j++)
				{
					awg_data_index = packet_len*i + step_receive*j;
					escape_char_received = pna_read(awg_data + awg_data_index, step_receive, escape_char_set);
//					pna_printf("got %d from %d is %c%c%c%c\r\n", awg_data_index + step_receive, total_len,
//							awg_data[awg_data_index+step_receive-5], awg_data[awg_data_index+step_receive-4],
//							awg_data[awg_data_index+step_receive-3], awg_data[awg_data_index+step_receive-2]);
					if(escape_char_received)
					{
						break;
					}
				}
				if(escape_char_received)
				{
					break;
				}
				pna_read(chert, 1, !escape_char_set); // reading \n
				progress_bar[i*30/total_packets+1] = '#';
				pna_printf("%s %d\r\n|",progress_bar,(i+1)*100/total_packets);
			}
			if(rem_packet_len > 0 && !escape_char_received)
			{
				receives_per_packet = rem_packet_len / step_receive;
				for(int j=0; j<receives_per_packet; j++)
				{
					awg_data_index = packet_len*total_packets + step_receive*j;
					escape_char_received = pna_read(awg_data + awg_data_index, step_receive, escape_char_set);
//					pna_printf("got %d from %d is %c%c%c%c\r\n", awg_data_index + step_receive, total_len,
//							awg_data[awg_data_index+step_receive-5], awg_data[awg_data_index+step_receive-4],
//							awg_data[awg_data_index+step_receive-3], awg_data[awg_data_index+step_receive-2]);
					if(escape_char_received)
					{
						break;
					}
				}
				if(!escape_char_received)
				{
					pna_read(chert, 1, !escape_char_set); // reading \n
					pna_printf("%s%d\r\n|",progress_bar,i*100/total_packets);
				}
			}
			if(escape_char_received)
			{
				pna_printf(START_OF_PACKET"escape char received.\r\n");
				continue;
			}

			send_awg_signal(len_awg, dig_amp, channel_num);
			dds_sample_size = len_awg;
			// get last done
			int ret = pna_gets(buffer, 100000);
			if(ret < 0)
			{
				pna_printf("Error while trying to read from console/tcp\r\n");
				return -1;
			}
			pna_printf(START_OF_PACKET"[total len]: %d | [len_awg]: %d | [port]: %d | [sigpow]: %2.2lf | [dig_pow]: %2.2lf | [dig_amp]: %1.3lf\r\n",
					total_len, len_awg, channel_num, sig_pow[channel_num]+sig_offset, dig_pow, dig_amp);
		}
		else if(strcmp(token, WAVE_PULSE)==0 )
		{
			token = strtok(NULL, delim);
			if(!check_argument(token, WAVE_PULSE, "channel"))
			{
				continue;
			}
			int period_num = atoi(token);
			token = strtok(NULL, delim);
			if(!check_argument(token, WAVE_PULSE, "channel"))
			{
				continue;
			}
			float duty_cycle = atof(token);
			token = strtok(NULL, delim);
			if(!check_argument(token, WAVE_PULSE, "channel"))
			{
				continue;
			}
			double dig_amp = atof(token);
			token = strtok(NULL, delim);
			if(!check_argument(token, WAVE_PULSE, "channel"))
			{
				continue;
			}
			int channel_num = atoi(token) - 1;
			if(is_2tx_2rx == 0)
			{
				channel_num = 0;
			}
			int amplitude_int =dig_amp*dac_max/2;
			send_pulse_signal(dds_sample_size, amplitude_int, period_num, duty_cycle,
											SIGNAL_INPHASE_ENABLED, channel_num);
			pna_printf("[period]: %d | [duty-cycle]: %.2lf | [port]: %d | [dig_amp]: %lf\r\n",
											period_num, duty_cycle, channel_num, dig_amp);
		}
		else if(strcmp(token, WAVE_SIN)==0 )
		{
			token = strtok(NULL, delim);
			if(!check_argument(token, WAVE_SIN, "period"))
			{
				continue;
			}
			int period_num = atoi(token);
			token = strtok(NULL, delim);
			if(!check_argument(token, WAVE_SIN, "dig_amp"))
			{
				continue;
			}
			double dig_amp = atof(token);
			token = strtok(NULL, delim);
			if(!check_argument(token, WAVE_SIN, "channel"))
			{
				continue;
			}
			int channel_num = atoi(token) - 1;
			if(is_2tx_2rx == 0)
			{
				channel_num = 0;
			}
			int amplitude_int = dig_amp*dac_max/2;
			send_sin_signal(dds_sample_size, amplitude_int, period_num, SIGNAL_IQ_ENABLED, channel_num);
			pna_printf("[amplitude]: %.3lf | [period]: %d | [port]: %d\r\n", dig_amp, period_num, channel_num);
		}
		else if(strcmp(token, WAVE_DC)==0 )
		{
			token = strtok(NULL, delim);
			if(!check_argument(token, WAVE_DC, "vga_gain"))
			{
				continue;
			}
			char *sz = NULL;
			long long power = -strtoll(token, &sz, 10);
			token = strtok(NULL, delim);
			if(!check_argument(token, WAVE_DC, "vga_gain"))
			{
				continue;
			}
			int channel_num = atoi(token) - 1;
			set_vga_gain(channel_num+1, power);
			send_dc_signal(dds_sample_size, dc_power_vna, channel_num);
			/*float amplitude_i, amplitude_q;
			int channel_num;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("DC", ERROR_ARG);
				continue;
			}
			else
			{
				amplitude_i = atof(token);
			}
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("DC", ERROR_ARG);
				continue;
			}
			else
			{
				amplitude_q = atof(token);
			}
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("DC", ERROR_CH);
				continue;
			}
			else
			{
				channel_num = atoi(token) - 1;
				if(!(channel_num == 1 || channel_num == 0))
				{
					print_error("DC", ERROR_CH);
					continue;
				}
			}
			if(is_2tx_2rx == 0)
			{
				channel_num = 0;
			}
			send_dc_signal(dds_sample_size, amplitude_i, amplitude_q, channel_num);
			*/
			pna_printf("[port]: %d | [vga_pow]: %lld\r\n", channel_num, power);
		}
		else if(strcmp(token, "dac_dc")==0 )
		{
			int channel_num = 0;
			token = strtok(NULL, delim);
			if(token == NULL)
			{
				continue;
			}
			int dc_dac_amp = atoi(token);
			send_dc_signal(dds_sample_size, dc_dac_amp, channel_num);
			pna_printf("\r\n");
		}
		else if(strcmp(token, WAVE_SINDC)==0 )
		{
			int channel_num;
			double sig_offset;
			token = strtok(NULL, delim);
			if(!check_argument(token, WAVE_SINDC, "offset"))
			{
				continue;
			}
			sig_offset = atof(token);
			token = strtok(NULL, delim);
			if(!check_argument(token, WAVE_SINDC, "channel"))
			{
				continue;
			}
			channel_num = atoi(token) - 1;
			if(is_2tx_2rx == 0)
			{
				channel_num = 0;
			}

			double dig_pow, dig_amp;
			if(sig_pow[channel_num] + sig_offset > -70.0)
			{
				long long vga_part = (long long)ceil(sig_pow[channel_num] + sig_offset);
				if(vga_part < -70)
				{
					vga_part = -70;
					pna_printf("vga set to -70, cannot set values less than -70\n");
				}
				else if(vga_part > 0)
				{
					pna_printf("vga set to 0, cannot set values more than 0\n");
					vga_part = 0.0;
				}
				set_vga_gain(channel_num+1, vga_part);
				dig_pow =sig_pow[channel_num] + sig_offset - ceil(sig_pow[channel_num] + sig_offset);
//				pna_printf("1) vga: %lld , dig: %lf, sig: %lf\n",vga_part, dig_pow, sig_pow[channel_num]);
			}
			else
			{
				set_vga_gain(channel_num+1, -70);
				dig_pow = sig_pow[channel_num] + sig_offset + 70.0;
//				pna_printf("2) vga: -70 , dig: %lf, sig: %lf\n",dig_pow, sig_pow[channel_num]);
			}
			dig_amp = pow(10.0, dig_pow / 20.0);
			token = strtok(NULL, delim);
			if(token!=NULL)
			{
				if(check_argument(token, WAVE_SINDC, "dig_amp"))
				{
					dig_amp = atof(token);
				}
			}

			int amplitude_int = dig_amp*dac_max/2;

			send_dc_signal(dds_sample_size, amplitude_int, channel_num);
			pna_printf("[port]: %d | [dig_amp]: %2.2lf | [dig_pow]: %2.2lf\r\n",
																	channel_num, dig_amp, dig_pow);
		}
		else if(strcmp(token, WAVE_SINC)==0 )
		{//sinc 1 0.9 1               sinc 1 5 1
			int s_size = get_sample_size();
			token = strtok(NULL, delim);
			if(!check_argument(token, WAVE_SINC, "dds_freq"))
			{
				continue;
			}
			double dds_freq = atof(token);
			token = strtok(NULL, delim);
			if(!check_argument(token, WAVE_SINC, "channel"))
			{
				continue;
			}
			int channel_num = atoi(token) - 1;
			if(is_2tx_2rx == 0)
			{
				channel_num = 0;
			}
			send_sinc_signal(dds_sample_size, dac_max, dds_freq, channel_num);
			pna_printf("[sample count]: %d, [sample size]: %d, [dds_freq]: %lf, [port]: %d\r\n", dds_sample_size, s_size, dds_freq, channel_num);
		}
		else if( strcmp(token, COMMAND_DBG)==0 )
		{
			char attr[80], buffer_dbg[80];
			token = strtok(NULL, delim);
			if(!check_argument(token, COMMAND_DBG, "attribute"))
			{
				continue;
			}
			strcpy(attr, token);
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				iio_device_debug_attr_read(dev, attr, buffer_dbg, 80);
				pna_printf("%s: %s \r\n", attr, buffer_dbg);
			}
			else
			{
				strcpy(buffer_dbg, token);
				while(1)
				{
					token = strtok(NULL, delim);
					if(token == NULL)
					{
						break;
					}
					strcat(buffer_dbg, " ");
					strcat(buffer_dbg, token);
				}
				iio_device_debug_attr_write(dev, attr, buffer_dbg);
				pna_printf("%s: %s \r\n", attr, buffer_dbg);
			}
		}
		else if( strcmp(token, COMMAND_REGISTER)==0 )
		{
			char value[80];
			token = strtok(NULL, delim);
			if(!check_argument(token, COMMAND_REGISTER, "address"))
			{
				continue;
			}
			char *sz = NULL;
			long long address = strtoll(token, &sz, 16);
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				read_reg_ad9361(address, value);
				pna_printf(COMMAND_REGISTER" %llx: %s \r\n", address, value);
			}
			else if(check_argument(token, COMMAND_REGISTER, "value"))
			{
				write_reg_ad9361(address, token);
				pna_printf(COMMAND_REGISTER", write to %llx: %s \r\n", address, token);
				
			}
		}
		else if( strcmp(token, COMMAND_NLF)==0 )
		{
			token = strtok(NULL, delim);
			if(!check_argument(token, COMMAND_NLF, "value"))
			{
				continue;
			}

			int enable = atoi(token);
			char buffer_cal[80];
			char *sz = NULL;
			long long curr_tx_freq = get_lo_freq(__TX);

			if(enable == 0)
			{
				if(tx_default_profile.frequency == curr_tx_freq)
				{
					tx_default_profile.data[0] = '0';
					ret = fastlock_load(__TX, tx_default_profile.data);
					if (ret < 0)
					{
						pna_printf("error on fastlock load\r\n");
					}

					ret = fastlock_recall(__TX, 0);
					if (ret < 0)
					{
						pna_printf("error on fastlock recall\r\n");
					}
				}
				pna_printf(COMMAND_NLF": %d\r\n", enable);
			}
			else
			{
				if(tx_default_profile.frequency != curr_tx_freq)
				{
					long long cal_reg_val = 0;
					read_reg_ad9361(VCO_CAL_STATUS, buffer_cal);
					// pna_printf("reg cal: %s\r\n", buffer_cal);
					cal_reg_val = strtoll(&buffer_cal[2], &sz, 16);
					while((cal_reg_val & 0xA0) != 0xA0)
					{
						usleep(SET_LO_DELAY);
						read_reg_ad9361(VCO_CAL_STATUS, buffer_cal);
						// pna_printf("reg cal: %s\r\n", buffer_cal);
						cal_reg_val = strtoll(&buffer_cal[2], &sz, 16);
					}

					tx_default_profile.frequency = curr_tx_freq;
					tx_default_profile.index = 0;
					ret = fastlock_store(__TX);
					if(ret != 0)
					{
						pna_printf("error on fastlock store\r\n");
					}
					ret = fastlock_read_cal(__TX, tx_default_profile.data);
					if(ret < 0)
					{
						pna_printf("error on fastlock save\r\n");
					}
					usleep(SET_LO_DELAY);
				}
				narrow_loop_filter(enable);
				pna_printf(COMMAND_NLF": %d\r\n", enable);
			}
		}
		else if( strcmp(token, COMMAND_FIR_COEF)==0 )
		{
			token = strtok(NULL, delim);
			if(!check_argument(token, COMMAND_FIR_COEF, "value"))
			{
				continue;
			}
			load_fir_filter(token, dev);
			pna_printf("\r\n");
		}
		else if( strcmp(token, COMMAND_EXIT)==0 )
		{
			close(fd_dma);
			pna_close_interface();
			return 0;
		}
		else
		{
			pna_printf("command \"%s\" not found!\r\n", token);
		}
	}
	close(fd_dma);
	pna_close_interface();
	return 0;
}
