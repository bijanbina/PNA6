#include "pna-base.h"

#include <stdlib.h>
#include <termios.h>

extern int errno;

extern struct iio_context *ctx;
extern struct iio_device *dev, *dds, *cap;

extern struct iio_device *iio_dac;
extern struct iio_buffer *dds_buffer;

extern int32_t rx1_buffer [MAX_FFT_LENGTH*2]; // FIXME: *2 should be carefully removed
extern int32_t rx2_buffer [MAX_FFT_LENGTH*2]; // FIXME: *2 should be carefully removed

#define ERROR_ARG 0
#define ERROR_CH 1

int dds_sample_size;
extern int8_t dac_buf[8*MAX_FFT_LENGTH];
extern int fd_dma;

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
	double sig_pow[2];

	// calculating sweep time
	struct timeval  tv1, tv2;
	double sweep_time = 0;

	int fft_size, dac_max; //16bit(I)+16bit(Q) = 32bit data

	fft_size = load_rx_sample_size();
	if(fft_size <= 0)
	{
		fft_size = 1024;
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
	}
	else
	{
		pna_printf("Error: interface is not set\r\n");
		return 0;
	}
	
	long long rx_freq;
	rx_freq = get_lo_freq(__RX);
#ifdef ETTUS_E310
	set_rx_switches(rx_freq);
	// set_tx_switches(false);
#endif
	long long sw_span;
	///////////////// FIXME: in case of open failure an error should be report
	fd_dma = open("/dev/dma", O_RDWR);

	unsigned char *awg_data = (unsigned char *)malloc(MAX_FFT_LENGTH * sizeof(unsigned char));

	gpio_fft(256);
	gpio_fft_reset();
	gpio_fft(fft_size);
	int ret = fft_changed(fft_size);
	if(ret < 0)
	{
		return -1;
	}
	sig_pow[0] = load_sig_pow(1);
	if(sig_pow[0] > 0.0)
	{
		sig_pow[0] = get_vga_gain(1);
		save_sig_pow(sig_pow[0], 1);
	}
	sig_pow[1] = load_sig_pow(2);
	if(sig_pow[1] > 0.0)
	{
		sig_pow[1] = get_vga_gain(2);
		save_sig_pow(sig_pow[1], 2);
	}
	pna_printf(START_OF_PACKET "\r\n"); // :D
	while(1)
	{
		pna_printf(">>");
		fflush(stdin);
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
		if( strcmp(token, "tx_bandwidth")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf("tx_bandwidth: %lld\r\n", get_bandwidth(__TX));
			}
			else
			{
				long long bandwidth = get_frequency(token);
				set_bandwidth(__TX, bandwidth);
				pna_printf("tx_bandwidth: %lld\r\n", bandwidth);
			}
		}
		else if( strcmp(token, "rx_bandwidth")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf("rx_bandwidth: %lld\r\n",  get_bandwidth(__RX));
			}
			else
			{
				long long bandwidth = get_frequency(token);
				set_bandwidth(__RX, bandwidth);
				pna_printf("rx_bandwidth: %lld\r\n", bandwidth);
			}
		}
		else if( strcmp(token, "sig_rf_out") == 0)
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
#ifdef ETTUS_E310
				pna_printf("sig_rf_out: %d\r\n", get_tx_switches());
#else
				pna_printf("sig_rf_out: 1\r\n");
#endif
			}
			else
			{
				bool enable = atoi(token);
#ifdef ETTUS_E310
				set_tx_switches(enable);
#endif
				pna_printf("sig_rf_out: %d\r\n", enable);
			}
		}
		else if( strcmp(token, "emio")==0 )
		{
			int emio_base;
			int emio_nchannel;
			int emio_value;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("emio", ERROR_ARG);
				continue;
			}
			else
			{
				emio_base = atoi(token);
			}
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("emio", ERROR_ARG);
				continue;
			}
			else
			{
				emio_nchannel = atoi(token);
			}
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("emio", ERROR_ARG);
				continue;
			}
			else
			{
				emio_value = atoi(token);
			}
			set_gpio_emio(emio_base, emio_nchannel, emio_value);
			pna_printf("emio[%d:%d]=%d\r\n", emio_base + emio_nchannel - 1, emio_base, emio_value);
		}
		else if( strcmp(token, "sweep_time")==0 ) // sweep time
		{
			pna_printf("sweep_time: %lf \r\n",  sweep_time);
		}
		else if( strcmp(token, "avg_window")==0 ) // sweep time
		{
			pna_print_avg();
		}
		else if( strcmp(token, "flat_top")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf("flat_top: %d \r\n", window_en);
			}
			else
			{
				window_en = atoi(token);
				pna_printf("flat_top: %d \r\n", window_en);
			}
		}
		else if( strcmp(token, "fillpro")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("fill profile", ERROR_ARG);
				continue;
			}
			else
			{
				rx_freq = get_frequency(token);
			}
			double rx_freq_mhz = rx_freq / 1E6;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("fill profile", ERROR_ARG);
				continue;
			}
			else
			{
				sw_span = get_frequency(token);
			}
			long long rx_sampling_frequency = 30E6; // 30M for ettus | 60M for others
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
			pna_printf("\r\n");
		}
		else if( strcmp(token, "loadpro")==0 ) // sweep time
		{
			load_profile(0);
			pna_printf("\r\n");
		}
		else if( strcmp(token, "sig_pow") == 0)
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("sig pow", ERROR_ARG);
				continue;
			}
			int channel_num = atoi(token);
			if(!(channel_num == 2 || channel_num == 1))
			{
				print_error("sig pow", ERROR_CH);
				continue;
			}
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf("sig_pow: %d, %lf\r\n", channel_num, load_sig_pow(channel_num));
			}
			else
			{
				sig_pow[channel_num-1] = atof(token);
				save_sig_pow(sig_pow[channel_num-1], channel_num);
				pna_printf("sig_pow: %d, %lf\r\n", channel_num, sig_pow[channel_num-1]);
			}
		}
		else if( strcmp(token, "vga_gain")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("vga gain", ERROR_ARG);
				continue;
			}
			int channel_num = atoi(token);
			if(!(channel_num == 2 || channel_num == 1))
			{
				print_error("vga gain", ERROR_CH);
				continue;
			}
			token = strtok(NULL, delim);
#ifdef ETTUS_E310
			channel_num = 1;
#endif
			if(token==NULL)
			{
				pna_printf("vga_gain: %d, %lld\r\n", channel_num, get_vga_gain(channel_num));
			}
			else
			{
				char *sz = NULL;
				long long vga_gain = -strtoll(token, &sz, 10);
				set_vga_gain(channel_num, vga_gain);
				pna_printf("vga_gain: %d, %lld\r\n", channel_num, vga_gain);
			}
		}
		else if( strcmp(token, "lna_gain")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("lna gain", ERROR_ARG);
				continue;
			}
			int channel_num = atoi(token);
			if(!(channel_num == 2 || channel_num == 1))
			{
				print_error("lna gain", ERROR_CH);
				continue;
			}
			token = strtok(NULL, delim);
#ifdef ETTUS_E310
				channel_num = 1;
#endif
			if(token==NULL)
			{
				pna_printf("lna_gain: %d, %lld\r\n", channel_num, get_lna_gain(channel_num));
			}
			else
			{
				char *sz = NULL;
				long long lna_gain = strtoll(token, &sz, 10);
				set_lna_gain(channel_num, lna_gain);
				pna_printf("lna_gain: %d, %lld\r\n", channel_num, lna_gain);
			}
		}
		else if( strcmp(token, "agc")==0 )
		{
			token = strtok(NULL, delim);
			int channel_num;
			if(token==NULL)
			{
				print_error("agc", ERROR_ARG);
				continue;
			}
			channel_num = atoi(token);
			if(!(channel_num == 2 || channel_num == 1))
			{
				print_error("agc", ERROR_CH);
				continue;
			}
			token = strtok(NULL, delim);
			char buf[1024];
#ifdef ETTUS_E310
				channel_num = 1;
#endif
			if(token==NULL)
			{
				get_gain_control_mode(channel_num, buf);
				pna_printf("agc: %d, %s\r\n", channel_num,  buf);
			}
			else
			{
				set_gain_control_mode(channel_num, token);
				pna_printf("agc: %d ,%.*s\r\n", channel_num, 3, token);
			}
		}
		else if( strcmp(token, "tx_sample_rate")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf("tx_sample_rate: %lld\r\n", get_sample_rate(__TX));
			}
			else
			{
				long long sampling_frequency = get_frequency(token);
				set_sample_rate(__TX, sampling_frequency);
				pna_printf("tx_sample_rate: %lld\r\n", sampling_frequency);
			}
		}
		else if( strcmp(token, "rx_sample_rate")==0 )
		{
			long long rx_sampling_frequency;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				rx_sampling_frequency = get_sample_rate(__RX);
				pna_printf("rx_sample_rate: %lld\r\n", rx_sampling_frequency);
			}
			else
			{
				rx_sampling_frequency = get_frequency(token);
				set_sample_rate(__RX, rx_sampling_frequency);
				pna_printf("rx_sample_rate: %lld\r\n", rx_sampling_frequency);
			}
		}
		else if( strcmp(token, "rx_sample_size")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf("rx_sample_size: %d\r\n", fft_size);
			}
			else
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
				pna_printf("rx_sample_size: %d\r\n", fft_size);
			}
		}
		else if( strcmp(token, "tx_sample_size")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf("tx_sample_size: %d\r\n", dds_sample_size);
			}
			else
			{
				dds_sample_size = atoi(token);
				pna_printf("tx_sample_size: %d\r\n", dds_sample_size);
			}
		}
		else if( strcmp(token, "tx_freq")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf("tx_freq: %lld\r\n", get_lo_freq(__TX));
			}
			else
			{
				long long freq = get_frequency(token);
				set_lo_freq(__TX, freq);
				pna_printf("tx_freq: %lld\r\n", freq);
			}
		}
		else if( strcmp(token, "rx_freq")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				rx_freq = get_lo_freq(__RX);
				pna_printf("rx_freq: %lld\r\n", rx_freq);
			}
			else
			{
				rx_freq = get_frequency(token);
				set_lo_freq(__RX, rx_freq);
				pna_printf("rx_freq: %lld\r\n", rx_freq);
			}
		}
		else if( strcmp(token, "rx_port")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				char rx_port[100];
				get_port(__RX, rx_port);
				pna_printf("rx_port: %s\r\n", rx_port);
			}
			else
			{
				set_port(__RX, token);
				pna_printf("rx_port: %s \r\n", token);
			}
		}
		else if( strcmp(token, "tx_port")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				char tx_port[100];
				get_port(__TX, tx_port);
				pna_printf("tx_port: %s \r\n", tx_port);
			}
			else
			{
				set_port(__TX, token);
				pna_printf("tx_port: %s \r\n", token);
			}
		}
		else if( strcmp(token, "sample_size")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				int sample_size = iio_device_get_sample_size(iio_dac);
				pna_printf("sample_size: %d \r\n", sample_size);
			}
			else
			{
				pna_printf("\r\n");
			}
		}
		else if( strcmp(token, "rx_fir_en")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf("rx_fir_en: %d \r\n", get_fir_en(__RX));
			}
			else
			{
				bool fir_en = atoi(token);
				set_fir_en(__RX, fir_en);
				pna_printf("rx_fir_en: %d \r\n", fir_en);
			}
		}
		else if( strcmp(token, "tx_fir_en")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf("tx_fir_en: %d \r\n", get_fir_en(__TX));
			}
			else
			{
				bool fir_en = atoi(token);
				set_fir_en(__TX, fir_en);
				pna_printf("tx_fir_en: %d \r\n", fir_en);
			}
		}
		else if( strcmp(token, "bbdc") == 0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf("bb_dc_enable: %d \r\n", get_bb_dc());
			}
			else
			{
				bool bb_dc_en = atoi(token);
				set_bb_dc(bb_dc_en);
				pna_printf("bb_dc_enable: %d \r\n", bb_dc_en);
			}
		}
		else if( strcmp(token, "rfdc") == 0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf("rf_dc_enable: %d \r\n", get_rf_dc());
			}
			else
			{
				bool rf_dc_en = atoi(token);
				set_rf_dc(rf_dc_en);
				pna_printf("rf_dc_enable: %d \r\n", rf_dc_en);
			}
		}
		else if( strcmp(token, "quad") == 0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf("quad_enable: %d \r\n", get_quad_track());
			}
			else
			{
				bool quad_en = atoi(token);
				set_quad_track(quad_en);
				pna_printf("quad_enable: %d \r\n", quad_en);
			}
		}
		else if(strcmp(token,"test") == 0)
		{
			for( int i=0; i<fft_size; i++ )
			{
				char first_byte = i%256;
				char second_byte = i/256;
				pna_printf("%d %d\r\n",first_byte, second_byte);
			}
			pna_printf("\r\n");
		}
        else if(strcmp(token,"freset") == 0)
		{
			gpio_fft_reset();
			pna_printf("\r\n");
		}
        else if(strcmp(token,"dac_max") == 0)
		{
        	token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf("dac_max: %d \r\n", load_dac_max());
			}
			else
			{
				int dac_max = atoi(token);
				save_dac_max(dac_max);
				pna_printf("dac_max: %d \r\n", dac_max);
			}
		}
		else if(strcmp(token,"adc") == 0)
		{
			int channel_num;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("adc", ERROR_ARG);
				continue;
			}
			else
			{
				channel_num = atoi(token) - 1;
				if(!(channel_num == 1 || channel_num == 0))
				{
					print_error("adc", ERROR_CH);
					continue;
				}
			}
			if(channel_num)
			{
				pna_adc(rx2_buffer, fft_size);
			}
			else
			{
				pna_adc(rx1_buffer, fft_size);
			}
		}
		else if(strcmp(token,"adc_iq") == 0)
		{
			int channel_num;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("adc", ERROR_ARG);
				continue;
			}
			else
			{
				channel_num = atoi(token) - 1;
				if(!(channel_num == 1 || channel_num == 0))
				{
					print_error("adc", ERROR_CH);
					continue;
				}
			}
			bool compression_enable;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				compression_enable = true;
			}
			else
			{
				compression_enable = atoi(token);
			}
#ifdef ETTUS_E310
			fill_rx_buffer_single(fft_size);
#else
			fill_rx_buffer(fft_size);
#endif
			int uart_size;
			int32_t *adc_data;
			unsigned char uart_tx_buffer[4*MAX_FFT_LENGTH];

			if(channel_num)
			{
#ifdef ETTUS_E310
				adc_data = rx1_buffer;
#else
				adc_data = rx2_buffer;
#endif
			}
			else
			{
				adc_data = rx1_buffer;
			}

			if(window_en)
			{
				flat_top_window(adc_data, fft_size);
			}

			if(compression_enable)
			{
				uart_size = compress_data_iq(adc_data, uart_tx_buffer, fft_size);
			}
			else
			{
				fill_output_buffer_iq(adc_data, uart_tx_buffer, fft_size);
				uart_size = fft_size;
			}
			pna_printf("PLOT");
			pna_write(uart_tx_buffer, 4*uart_size);
			pna_printf("\r\n");
		}
		else if(strcmp(token,"adc_fft") == 0)
		{
			int channel_num;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("adc", ERROR_ARG);
				continue;
			}
			else
			{
				channel_num = atoi(token) - 1;
				if(!(channel_num == 1 || channel_num == 0))
				{
					print_error("adc", ERROR_CH);
					continue;
				}
			}
			if(channel_num)
			{
				pna_adc_fft(rx2_buffer, fft_size);
			}
			else
			{
				pna_adc_fft(rx1_buffer, fft_size);
			}
		}
		else if(strcmp(token,"sweep2") == 0)
		{
			int channel_num;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("sweep", ERROR_ARG);
				continue;
			}
			else
			{
				channel_num = atoi(token) - 1;
				if(!(channel_num == 1 || channel_num == 0))
				{
					print_error("sweep", ERROR_CH);
					continue;
				}
			}
			int compression_enable;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("sweep", ERROR_ARG);
				continue;
			}
			else
			{
				compression_enable = atoi(token);
			}
			if(sw_span < 0)
				sw_span = 80E6;
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
			double rx_sampling_frequency_mhz = 30.0; // 30 for Ettus | 60 for others
			
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
			for(int i=0; i<span_num/2; i++)
			{
				if(channel_num)
				{
#ifdef ETTUS_E310
					spectrum = pna_fft_dcfixed2(rx1_buffer, fft_size, i, window_en);
#else
					spectrum = pna_fft_dcfixed2(rx2_buffer, fft_size, i, window_en);
#endif
				}
				else
				{
					spectrum = pna_fft_dcfixed2(rx1_buffer, fft_size, i, window_en);
				}
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
		else if(strcmp(token,"sweep") == 0)
		{
			int channel_num;
			token = strtok(NULL, delim);
			// long long span_sweep;
			if(token==NULL)
			{
				print_error("sweep", ERROR_ARG);
				continue;
			}
			else
			{
				channel_num = atoi(token) - 1;
				if(!(channel_num == 1 || channel_num == 0))
				{
					print_error("sweep", ERROR_CH);
					continue;
				}
			}
			long long sw_span;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("sweep", ERROR_ARG);
				continue;
			}
			else
			{
				sw_span = get_frequency(token);
			}
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
		else if(strcmp(token,"fft_span") == 0)
		{
			double span_ratio;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("fft", ERROR_ARG);
				continue;
			}
			else
			{
				span_ratio = atof(token);
			}
			int channel_num;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("fft", ERROR_ARG);
				continue;
			}
			else
			{
				channel_num = atoi(token) - 1;
				if(!(channel_num == 1 || channel_num == 0))
				{
					print_error("fft", ERROR_CH);
					continue;
				}
			}
			bool compression_enable;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				compression_enable = true;
			}
			else
			{
				compression_enable = atoi(token);
			}

			int32_t *spectrum;
			unsigned char uart_tx_buffer[4*MAX_FFT_LENGTH];
			if(span_ratio>1 || span_ratio<=0)
			{
				span_ratio = 1;
			}
			int removed_span = fft_size*(1-span_ratio)/2;
			// pna_printf("rx-freq %lf, span %f\r\n",rx_sampling_frequency_mhz, span);

			gettimeofday(&tv1, NULL);

			if(channel_num)
			{
#ifdef ETTUS_E310
				spectrum = pna_fft(rx1_buffer, removed_span, fft_size, window_en);
#else
				spectrum = pna_fft(rx2_buffer, removed_span, fft_size, window_en);
#endif
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
			pna_printf("PLOT");
			pna_write(uart_tx_buffer, 2*uart_size);
			pna_printf("\r\n"); // \r\n%d  , 2*uart_size
			free(spectrum);
		}
		else if(strcmp(token,"fft") == 0)
		{
			int32_t *spectrum = pna_fft(rx1_buffer, 0, fft_size, window_en);
			unsigned char uart_tx_buffer[4*UART_LENGTH];
			int uart_size = compress_data(spectrum, uart_tx_buffer, fft_size);
			pna_printf("PLOT");
			pna_write(uart_tx_buffer, 2*uart_size);
			pna_printf("\r\n");
			free(spectrum);
		}
		else if(strcmp(token,"fft2") == 0)
		{
			pna_fft2(rx1_buffer, fft_size);
		}
		else if(strcmp(token,"fft3") == 0)
		{
			pna_fft3(rx1_buffer, fft_size);
		}
		else if(strcmp(token,"fft4") == 0)
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("fft4", ERROR_ARG);
				continue;
			}
			int rwfunc = atoi(token);
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("fft4", ERROR_ARG);
				continue;
			}
			int num_bytes = atoi(token);
			uint32_t uart_tx_buffer[MAX_FFT_LENGTH*2];
			for(int k=0; k<MAX_FFT_LENGTH*2; k++)
			{
				uart_tx_buffer[k] = k;
			}
			pna_printf("fft_status#1 = %d\r\n", gpio_fft_status());
			if(rwfunc)
			{
				pna_printf("write %ld bytes to dma.\r\n", write_DMA(
							(char *)uart_tx_buffer, num_bytes));
			}
			else
			{
				pna_printf("read %ld bytes from dma.\r\n", read_DMA(
							(char *)uart_tx_buffer, num_bytes));
			}
			pna_printf("fft_status#2 = %d\r\n", gpio_fft_status());
			pna_printf("\r\n");
		}
		else if (strcmp(token, "sin_iq") == 0)
		{
			int s_size = iio_device_get_sample_size(iio_dac);
			int sig_iq;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("sin_iq", ERROR_ARG);
				continue;
			}
			else
			{
				sig_iq = atoi(token);
			}
			int period_num;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("sin_iq", ERROR_ARG);
				continue;
			}
			else
			{
				period_num = atoi(token);
			}
			double sig_offset;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("sin_iq", ERROR_ARG);
				continue;
			}
			else
			{
				sig_offset = atof(token);
			}
			int channel_num;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("sin_iq", ERROR_CH);
				continue;
			}
			else
			{
				channel_num = atoi(token) - 1;
				if(!(channel_num == 1 || channel_num == 0))
				{
					print_error("sin_iq", ERROR_CH);
					continue;
				}
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
			int amplitude_int = dig_amp*dac_max/2;

//			double dc_offset = period_num*2 +140;
//			period_num = 1;

			int period_sample_count = dds_sample_size/period_num;
			for (int i=0 ; i<period_num ; i++)
			{
				double sinous, cosinous;
				for(int j=0; j<period_sample_count; j++)
				{
					double x = j*2*PI;
					x = x/period_sample_count;
					if(sig_iq == SIGNAL_QUAD_ENABLED || sig_iq == SIGNAL_IQ_ENABLED)
					{
						sinous = sin(x)*amplitude_int;// - dc_offset;
					}
					else
					{
						sinous = 0;
					}
					if(sig_iq == SIGNAL_INPHASE_ENABLED || sig_iq == SIGNAL_IQ_ENABLED)
					{
						cosinous = cos(x)*amplitude_int;// - dc_offset;
					}
					else
					{
						cosinous = 0;
					}
					int16_t sin_int = (int16_t)(sinous);
					int16_t cos_int = (int16_t)(cosinous);

#ifdef ETTUS_E310
					dac_buf[(j+i*period_sample_count)*s_size] = (int8_t)(sin_int%256);   // LSB
					dac_buf[(j+i*period_sample_count)*s_size+1] = (int8_t)(sin_int/256);     // MSB
					dac_buf[(j+i*period_sample_count)*s_size+2] = (int8_t)(cos_int%256);   // LSB
					dac_buf[(j+i*period_sample_count)*s_size+3] = (int8_t)(cos_int/256);     // MSB
#else
					dac_buf[(j+i*period_sample_count)*s_size+channel_num*4] = (int8_t)(sin_int%256);   // LSB
					dac_buf[(j+i*period_sample_count)*s_size+channel_num*4+1] = (int8_t)(sin_int/256);     // MSB
					dac_buf[(j+i*period_sample_count)*s_size+channel_num*4+2] = (int8_t)(cos_int%256);   // LSB
					dac_buf[(j+i*period_sample_count)*s_size+channel_num*4+3] = (int8_t)(cos_int/256);     // MSB
#endif
				}
			}
			create_dds_buffer(dac_buf, dds_sample_size);
			pna_printf("[I/Q/SSB]: %d | [period]: %d | [port]: %d | [sigpow]: %2.2lf | [dig_pow]: %2.2lf | [dig_amp]: %1.3lf\r\n",
					sig_iq, period_num, channel_num, sig_pow[channel_num]+sig_offset, dig_pow, dig_amp);
		}
		else if (strcmp(token, "pulse_iq") == 0)
		{
			int s_size = iio_device_get_sample_size(iio_dac);
			int sig_iq;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("pulse_iq", ERROR_ARG);
				continue;
			}
			else
			{
				sig_iq = atoi(token);
			}
			int period_num;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("pulse_iq", ERROR_ARG);
				continue;
			}
			else
			{
				period_num = atoi(token);
			}
			float duty_cycle;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("pulse_iq", ERROR_ARG);
				continue;
			}
			else
			{
				duty_cycle = atof(token);
			}
			double sig_offset;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("pulse_iq", ERROR_ARG);
				continue;
			}
			else
			{
				sig_offset = atof(token);
			}
			int channel_num;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("pulse_iq", ERROR_ARG);
				continue;
			}
			else
			{
				channel_num = atoi(token) - 1;
				if(!(channel_num == 1 || channel_num == 0))
				{
					print_error("pulse_iq", ERROR_CH);
					continue;
				}
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

			double pulse, p_hilbert;
			int amplitude_int = dig_amp*dac_max/2;
			int period_samples = dds_sample_size/period_num;
			int on_pulse_samples = duty_cycle*period_samples;

			for (int i=0; i<dds_sample_size; i++)
			{
				//// Real Part
				if(i%period_samples < on_pulse_samples &&
						(sig_iq == SIGNAL_INPHASE_ENABLED || sig_iq == SIGNAL_IQ_ENABLED))
				{
					pulse = amplitude_int;
				}
				else
				{
					pulse = 0;
				}

				//// Imaginary Part
				p_hilbert = 0;
				if(sig_iq == SIGNAL_QUAD_ENABLED || sig_iq == SIGNAL_IQ_ENABLED)
				{
					for(int j=-2; j<period_num+2; j++)
					{
						double num = i - j*period_samples;
						double denum = i -  j*period_samples - on_pulse_samples;
						if(num == 0) // log(0)
						{
							num = (i-1) - j*period_samples;
						}
						if(denum == 0) // log(x/0)
						{
							denum = i-1 -  j*period_samples - on_pulse_samples;
						}
						p_hilbert += amplitude_int*log(fabs(num/denum))/PI;
					}
				}
				else
				{
					p_hilbert = 0;
				}

				int16_t pulse_int = (int16_t)(pulse);
				int16_t p_hilbert_int = (int16_t)(p_hilbert);

#ifdef ETTUS_E310
				dac_buf[i*s_size] = (int8_t)(pulse_int%256);   // LSB
				dac_buf[i*s_size+1] = (int8_t)(pulse_int/256);     // MSB
				dac_buf[i*s_size+2] = (int8_t)(p_hilbert_int%256);   // LSB
				dac_buf[i*s_size+3] = (int8_t)(p_hilbert_int/256);     // MSB
#else
				dac_buf[i*s_size+channel_num*4] = (int8_t)(pulse_int%256);   // LSB
				dac_buf[i*s_size+channel_num*4+1] = (int8_t)(pulse_int/256);     // MSB
				dac_buf[i*s_size+channel_num*4+2] = (int8_t)(p_hilbert_int%256);   // LSB
				dac_buf[i*s_size+channel_num*4+3] = (int8_t)(p_hilbert_int/256);     // MSB
#endif
			}
			create_dds_buffer(dac_buf, dds_sample_size);
			pna_printf("[I/Q/SSB]: %d | [period]: %d | [duty-cycle]: %.2lf | [port]: %d | [sigpow]: %2.2lf | [dig_pow]: %2.2lf | [dig_amp]: %1.3lf\r\n",
								sig_iq, period_num, duty_cycle, channel_num, sig_pow[channel_num]+sig_offset, dig_pow, dig_amp);
		}
		else if(strcmp(token, "awg") == 0)
		{
			int len_awg;
			const int step_receive = 32;
			const int num_width = 4;
			int s_size = iio_device_get_sample_size(iio_dac);

			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("awg", ERROR_ARG);
				continue;
			}
			len_awg = atoi(token);
			int total_len = num_width * len_awg * 2; // 2 for [I/Q]
			if(len_awg < 4 || len_awg > 8192)
			{
				print_error("awg", ERROR_ARG);
				continue;
			}
			double sig_offset;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("awg", ERROR_ARG);
				continue;
			}
			else
			{
				sig_offset = atof(token);
			}
			int channel_num;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("awg", ERROR_ARG);
				continue;
			}
			else
			{
				channel_num = atoi(token) - 1;
				if(!(channel_num == 1 || channel_num == 0))
				{
					print_error("awg", ERROR_CH);
					continue;
				}
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

			pna_printf("\r\n>>");

			for(int i=0; i<total_len-step_receive; i+=step_receive)
			{
				pna_read(awg_data + i, step_receive);
			}

//			printf("data: %s\r\n", awg_data);
//			pna_printf("len_awg=%d, total_len=%d, s_size=%d\n" , len_awg, total_len, s_size);
			for(int i=0; i<len_awg; i++) // 2 for [I/Q]
			{
				char number_str[5];
				memcpy(number_str, awg_data+2*i*num_width, num_width);
				number_str[4] = 0;
				int number_i = atoi(number_str) - 2048;
				double number_float_i = (double)number_i;
				number_float_i = number_float_i*dig_amp;
				number_i = (int)floor(number_float_i);
				int16_t number16_i = (int16_t)(number_i*16);

				memcpy(number_str, awg_data + (2*i+1)*num_width, num_width);
				number_str[4] = 0;
				int number_q = atoi(number_str) - 2048;
				double number_float_q = (double)number_q;
				number_float_q = number_float_q*dig_amp;
				number_q = (int)floor(number_float_q);
				int16_t number16_q = (int16_t)(number_q*16);
//				if(i<200 || i>900)
//				{
//					pna_printf("%d) I: %4d Q: %4d\n", i, number16_i, number16_q);
//				}
#ifdef ETTUS_E310
				dac_buf[i*s_size] = (int8_t)(number16_i%256);   // LSB
				dac_buf[i*s_size+1] = (int8_t)(number16_i/256);      // MSB
				dac_buf[i*s_size+2] = (int8_t)(number16_q%256);   // LSB
				dac_buf[i*s_size+3] = (int8_t)(number16_q/256);     // MSB
#else
				dac_buf[i*s_size+channel_num*4] = (int8_t)(number16_i%256);   // LSB
				dac_buf[i*s_size+channel_num*4+1] = (int8_t)(number16_i/256);     // MSB
				dac_buf[i*s_size+channel_num*4+2] = (int8_t)(number16_q%256);   // LSB
				dac_buf[i*s_size+channel_num*4+3] = (int8_t)(number16_q/256);     // MSB
#endif
			}
			dds_sample_size = len_awg;

			create_dds_buffer(dac_buf, dds_sample_size);
			pna_printf(START_OF_PACKET "[len_awg]: %d | [port]: %d | [sigpow]: %2.2lf | [dig_pow]: %2.2lf | [dig_amp]: %1.3lf\r\n",
					len_awg, channel_num, sig_pow[channel_num]+sig_offset, dig_pow, dig_amp);
		}
		else if (strcmp(token, "pulse")==0 )
		{
			int s_size = iio_device_get_sample_size(iio_dac);
			int period_num;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("pulse", ERROR_ARG);
				continue;
			}
			else
			{
				period_num = atoi(token);
			}
			float duty_cycle;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				duty_cycle = 0.5;
			}
			else
			{
				duty_cycle = atof(token);
			}
			double dig_amp;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("pulse", ERROR_ARG);
				continue;
			}
			else
			{
				dig_amp = atof(token);
			}
			int channel_num;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				channel_num = 0;
			}
			else
			{
				channel_num = atoi(token) - 1;
				if(!(channel_num == 1 || channel_num == 0))
				{
					print_error("pulse", ERROR_CH);
					continue;
				}
			}
			int amplitude_int =dig_amp*dac_max/2;
			int period_sample_count = dds_sample_size/period_num;

			for (int i=0 ; i<period_num ; i++)
			{
				double pulse;
				for(int j=0; j<period_sample_count; j++)
				{
					if ( j<period_sample_count*duty_cycle )
					{
						pulse = amplitude_int;
					}
					else
					{
						pulse = 0;
					}
					int16_t pulse_int = (int16_t)(pulse);

#ifdef ETTUS_E310
					dac_buf[(j+i*period_sample_count)*s_size] = (int8_t)(pulse_int%256);   // LSB
					dac_buf[(j+i*period_sample_count)*s_size+1] = (int8_t)(pulse_int/256);     // MSB
					dac_buf[(j+i*period_sample_count)*s_size+2] = 0;     // LSB
					dac_buf[(j+i*period_sample_count)*s_size+3] = 0;     // MSB
#else
					dac_buf[(j+i*period_sample_count)*s_size+channel_num*4] = (int8_t)(pulse_int%256);   // LSB
					dac_buf[(j+i*period_sample_count)*s_size+channel_num*4+1] = (int8_t)(pulse_int/256);     // MSB
					dac_buf[(j+i*period_sample_count)*s_size+channel_num*4+2] = 0;     // MSB
					dac_buf[(j+i*period_sample_count)*s_size+channel_num*4+3] = 0;     // MSB
#endif
				}
			}
			create_dds_buffer(dac_buf, dds_sample_size);
			pna_printf("[period]: %d | [duty-cycle]: %.2lf | [port]: %d | [dig_amp]: %lf\r\n",
											period_num, duty_cycle, channel_num, dig_amp);
		}
		else if (strcmp(token, "sin")==0 )
		{
			int s_size = iio_device_get_sample_size(iio_dac);
			int period_num;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("sin", ERROR_ARG);
				continue;
			}
			else
			{
				period_num = atoi(token);
			}
			double dig_amp;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("sin", ERROR_ARG);
				continue;
			}
			else
			{
				dig_amp = atof(token);
			}
			int channel_num;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("sin", ERROR_ARG);
				continue;
			}
			else
			{
				channel_num = atoi(token) - 1;
				if(!(channel_num == 1 || channel_num == 0))
				{
					print_error("sin", ERROR_CH);
					continue;
				}
			}
			int amplitude_int = dig_amp*dac_max/2;
			int period_sample_count = dds_sample_size/period_num;

			for (int i=0 ; i<period_num ; i++)
			{
				double sinous, cosinous;
				for(int j=0; j<period_sample_count; j++)
				{
					double x = j*2*PI;
					x = x/period_sample_count;
					sinous = sin(x)*amplitude_int;
					cosinous = cos(x)*amplitude_int;
					int16_t sin_int = (int16_t)(sinous);
					int16_t cos_int = (int16_t)(cosinous);
#ifdef ETTUS_E310
					dac_buf[(j+i*period_sample_count)*s_size] = (int8_t)(sin_int%256);   // LSB
					dac_buf[(j+i*period_sample_count)*s_size+1] = (int8_t)(sin_int/256);     // MSB
					dac_buf[(j+i*period_sample_count)*s_size+2] = (int8_t)(cos_int%256);   // LSB
					dac_buf[(j+i*period_sample_count)*s_size+3] = (int8_t)(cos_int/256);     // MSB
#else
					dac_buf[(j+i*period_sample_count)*s_size+channel_num*4] = (int8_t)(sin_int%256);   // LSB
					dac_buf[(j+i*period_sample_count)*s_size+channel_num*4+1] = (int8_t)(sin_int/256);     // MSB
					dac_buf[(j+i*period_sample_count)*s_size+channel_num*4+2] = (int8_t)(cos_int%256);   // LSB
					dac_buf[(j+i*period_sample_count)*s_size+channel_num*4+3] = (int8_t)(cos_int/256);     // MSB
#endif
				}
			}
			create_dds_buffer(dac_buf, dds_sample_size);
			pna_printf("[amplitude]: %.3lf | [period]: %d | [port]: %d\r\n", dig_amp, period_num, channel_num);
		}
		else if (strcmp(token, "dc")==0 )
		{
			int s_size = iio_device_get_sample_size(iio_dac);
			float amplitude_i, amplitude_q;
			int channel_num;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("dc", ERROR_ARG);
				continue;
			}
			else
			{
				amplitude_i = atof(token);
			}
			if(token==NULL)
			{
				print_error("dc", ERROR_ARG);
				continue;
			}
			else
			{
				amplitude_q = atof(token);
			}
			if(token==NULL)
			{
				channel_num = 0;
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
			int amplitude_i_int = amplitude_i*dac_max/2;
			int amplitude_q_int = amplitude_q*dac_max/2;

			for (int i=0 ; i<dds_sample_size ; i++)
			{
				double inphase = amplitude_i_int;
				int16_t inphase_int = (int16_t)(inphase);
				double quad = amplitude_q_int;
				int16_t quad_int = (int16_t)(quad);
#ifdef ETTUS_E310
				dac_buf[i*s_size] = (int8_t)(pulse_int%256);   // LSB
				dac_buf[i*s_size+1] = (int8_t)(pulse_int/256);     // MSB
				dac_buf[i*s_size+2] = 0;     // MSB
				dac_buf[i*s_size+3] = 0;     // MSB
#else
				dac_buf[i*s_size+channel_num*4] = (int8_t)(inphase_int%256);   // LSB
				dac_buf[i*s_size+channel_num*4+1] = (int8_t)(inphase_int/256);     // MSB
				dac_buf[i*s_size+channel_num*4+2] = (int8_t)(quad_int%256);   // LSB
				dac_buf[i*s_size+channel_num*4+3] = (int8_t)(quad_int/256);     // MSB
#endif
			}
			create_dds_buffer(dac_buf, dds_sample_size);
			pna_printf("[port]: %d | [amp_i]: %2.2lf | [amp_q]: %2.2lf\r\n",
																	channel_num, amplitude_i, amplitude_q);
		}
		else if (strcmp(token, "sinc")==0 )
		{
			//sinc 1 0.9 1               sinc 1 5 1
			int s_size = iio_device_get_sample_size(iio_dac);
			double dds_freq;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("sinc", ERROR_ARG);
				continue;
			}
			else
			{
				dds_freq = atof(token);
			}
			int channel_num;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				channel_num = 0;
			}
			else
			{
				channel_num = atoi(token) - 1;
				if(!(channel_num == 1 || channel_num == 0))
				{
					print_error("sinc", ERROR_CH);
					continue;
				}
			}
			for (int i=0 ; i<dds_sample_size ; i++)
			{
				double x = i-dds_sample_size/2;
				double sinc;
				if ( x==0 )
				{
					sinc = dac_max;
				}
				else
				{
					sinc = sin(x/dds_freq)/(x/dds_freq)*dac_max;
				}
				int16_t sinc_int = (int16_t)(sinc);

#ifdef ETTUS_E310
				dac_buf[i*s_size] = (int8_t)(sinc_int%256);   // LSB
				dac_buf[i*s_size+1] = (int8_t)(sinc_int/256);     // MSB
				dac_buf[i*s_size+2] = 0;     // MSB
				dac_buf[i*s_size+3] = 0;     // MSB
#else
				dac_buf[i*s_size+channel_num*4] = (int8_t)(sinc_int%256);   // LSB
				dac_buf[i*s_size+channel_num*4+1] = (int8_t)(sinc_int/256);     // MSB
				dac_buf[i*s_size+channel_num*4+2] = 0;     // MSB
				dac_buf[i*s_size+channel_num*4+3] = 0;     // MSB
#endif
				// pna_printf("DAC Buffer[%d]= %d ,\tx= %f\t sinc=%f, \t sinc_int=%d\r\n", i*s_size+channel_num*2+1, dac_buf[i*s_size+channel_num*2+1], x, sinc, sinc_int );
			}
			//dac_buf[sample_count/2*s_size + channel_num*2+1] = 127;
			//dac_buf[sample_count/2*s_size + channel_num*2] = 240;

//			pna_printf("Channel Num = %d\r\n", channel_num );
//			pna_printf("DDS Freq = %f\r\n", dds_freq);
			pna_printf("Sample Count= %d\r\n", dds_sample_size);
			pna_printf("Sample Size = %d\r\n", s_size );
			for ( int i=dds_sample_size/2-5 ; i<dds_sample_size/2+5 ; i++)
			{
				pna_printf("DAC Buffer[%d]= %d ,%d , x= %d\r\n", i*s_size+channel_num*2+1, (uint8_t) dac_buf[i*s_size+channel_num*2+1], (uint8_t)dac_buf[i*s_size+channel_num*2],i-dds_sample_size/2);
			}
			create_dds_buffer(dac_buf, dds_sample_size);
			pna_printf("[dds_freq]: %lf, [port]: %d\r\n", dds_freq, channel_num);
		}
		else if( strcmp(token, "dbg")==0 )
		{
			char str[80], buffer_dbg[80];
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("debug", ERROR_ARG);
				continue;
			}
			strcpy(str, token);
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				iio_device_debug_attr_read(dev, str, buffer_dbg, 80);
				pna_printf("%s: %s \r\n", str, buffer_dbg);
			}
			else
			{
				strcpy(buffer_dbg, token);
				while(1)
				{
					token = strtok(NULL, delim);
					if(token == NULL)
						break;
					strcat(buffer_dbg, " ");
					strcat(buffer_dbg, token);
				}
				iio_device_debug_attr_write(dev, str, buffer_dbg);
				pna_printf("%s: %s \r\n", str, buffer_dbg);
			}
		}
		else if( strcmp(token, "reg")==0 )
		{
			char value[80];
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				print_error("register", ERROR_ARG);
				continue;
			}
			char *sz = NULL;
			long long address = strtoll(token, &sz, 16);
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				read_reg_ad9361(address, value);
				pna_printf("reg %llx: %s \r\n", address, value);
			}
			else
			{
				write_reg_ad9361(address, token);
				pna_printf("write to %llx: %s \r\n", address, token);
				
			}
		}
		else if( strcmp(token, "fir_coef")==0 )
		{
			load_fir_filter("filter.ftr", dev);
			pna_printf("\r\n");
		}
		else if( strcmp(token, "exit")==0 )
		{
			close(fd_dma);
			pna_close_interface();
			return 0;
		}
		else
		{
			pna_printf("\r\n");
		}
	}
	close(fd_dma);
	pna_close_interface();
	return 0;
}

void print_error(char *function, int error_code)
{
	char arg_buf[200];
	char err_massage[200];
	char usage[200];
	char cmd_name[20];

	if(error_code == ERROR_ARG)
	{
		strcpy(err_massage, "arguments are not enough.");
	}
	else if(error_code == ERROR_CH)
	{
		strcpy(err_massage, "Channel number should be 1 or 2.");
	}

	if(!strcmp(function, "sweep"))
	{
		strcpy(arg_buf, "[port#][compression enable]");
		strcpy(usage, "Capture spectrum of signal while sweeping.");
		strcpy(cmd_name, "sweep");
	}
	else if(!strcmp(function, "fft"))
	{
		strcpy(arg_buf, "[port#][compression enable]");
		strcpy(usage, "Capture spectrum of signal.");
		strcpy(cmd_name, "fft_span");
	}
	else if(!strcmp(function, "fft4"))
	{
		strcpy(arg_buf, "[r0/w1][bytes]");
		strcpy(usage, "FFT module read/write test.");
		strcpy(cmd_name, "fft4");
	}
	else if(!strcmp(function, "adc"))
	{
		strcpy(arg_buf, "[port#][compression enable]");
		strcpy(usage, "Capture receiver signal from port.");
		strcpy(cmd_name, "adc_iq");
	}
	else if(!strcmp(function, "awg"))
	{
		strcpy(arg_buf, "[sample_size][offset][port#]");
		strcpy(usage, "Arbitrary wave generator with samples, i/q and port arguments.");
		strcpy(cmd_name, "awg");
	}
	else if(!strcmp(function, "sin_iq"))
	{
		strcpy(arg_buf, "[i/q][period][offset][port#]");
		strcpy(usage, "generates sinous.");
		strcpy(cmd_name, "sin_iq");
	}
	else if(!strcmp(function, "pulse_iq"))
	{
		strcpy(arg_buf, "[i/q][period][duty cycle][offset][port#]");
		strcpy(usage, "generates pulse.");
		strcpy(cmd_name, "pulse_iq");
	}
	else if(!strcmp(function, "fill profile"))
	{
		strcpy(arg_buf, "[rx_freq][span]");
		strcpy(usage, "Fill FastLock profiles before sweep.");
		strcpy(cmd_name, "fillpro");
	}
	else if(!strcmp(function, "vga gain"))
	{
		strcpy(arg_buf, "[port#][value]");
		strcpy(usage, "Read/Write vga_gain with port and value arguments.");
		strcpy(cmd_name, "vga_gain");
	}
	else if(!strcmp(function, "sig pow"))
	{
		strcpy(arg_buf, "[port#][value]");
		strcpy(usage, "Read/Write sig_pow with port and value arguments.");
		strcpy(cmd_name, "sig_pow");
	}
	else if(!strcmp(function, "lna gain"))
	{
		strcpy(arg_buf, "[port#][value]");
		strcpy(usage, "Read/Write lna gain with port and value arguments.");
		strcpy(cmd_name, "lna_gain");
	}
	else if(!strcmp(function, "agc"))
	{
		strcpy(arg_buf, "[port#][value]");
		strcpy(usage, "Read/Write agc with port and value arguments.");
		strcpy(cmd_name, "agc");
	}
	else if(!strcmp(function, "emio"))
	{
		strcpy(arg_buf, "[base][nchannel][value]");
		strcpy(usage, "Write emio with base, nchannel and value arguments.");
		strcpy(cmd_name, "emio");
	}
	else if(!strcmp(function, "register"))
	{
		strcpy(arg_buf, "[address][value]");
		strcpy(usage, "Debug mode.");
		strcpy(cmd_name, "reg");
	}
	else if(!strcmp(function, "debug"))
	{
		strcpy(arg_buf, "[attribute][value]");
		strcpy(usage, "Debug mode.");
		strcpy(cmd_name, "dbg");
	}
	else if(!strcmp(function, "sinc"))
	{
		strcpy(usage, "Generate sinc signal.");
		strcpy(arg_buf, "[frequency][port#]");
		strcpy(cmd_name, "sinc");
	}
	else if(!strcmp(function, "DC"))
	{
		strcpy(usage, "Generate DC signal.");
		strcpy(arg_buf, "[amplitude_i][amplitude_q][port#]");
		strcpy(cmd_name, "dc");
	}
	else if(!strcmp(function, "pulse"))
	{
		strcpy(usage, "Generate pulse signal with period, amplitude, duty cycle and port arguments.");
		strcpy(arg_buf, "[period][duty cycle][amplitude][port#]");
		strcpy(cmd_name, "pulse");
	}
	else if(!strcmp(function, "sin"))
	{
		strcpy(usage, "Generate sinous signal with period, amplitude and port arguments.");
		strcpy(arg_buf, "[period][amplitude][port#]");
		strcpy(cmd_name, "sin");
	}

	pna_printf("---------------------------------------------------------------\r\n"
						"%s: %s\r\n%s\r\nUsage:\r\n    %s %s\r\n"
						"---------------------------------------------------------------\r\n" ,
						function, err_massage, usage, cmd_name, arg_buf);
}
