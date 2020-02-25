#include "pna-base.h"

extern struct iio_context *ctx;
extern struct iio_device *dev, *dds, *cap;

extern struct iio_device *iio_dac;
extern struct iio_buffer *dds_buffer;

extern int32_t rx1_buffer [MAX_FFT_LENGTH*2]; // FIXME: *2 should be carefully removed
extern int32_t rx2_buffer [MAX_FFT_LENGTH*2]; // FIXME: *2 should be carefully removed

int dds_sample_size;
// int span_number = 1;
extern int8_t dac_buf[8*MAX_FFT_LENGTH];
extern int fd_dma;

int main (int argc, char **argv)
{
	char buffer[1000];
	const char delim[2] = " ";
	char* token;
	bool is_profile_empty = true;

	// calculating sweep time
	struct timeval  tv1, tv2;
	double sweep_time = 0;

	unsigned int fft_size = 1024; //16bit(I)+16bit(Q) = 32bit data
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
	
	long long span;
	long long rx_sampling_frequency;
	long long rx_freq;
	rx_freq = get_lo_freq(__RX);
	rx_sampling_frequency = get_sample_rate(__RX);
	// span = rx_sampling_frequency;
	///////////////// FIXME: in case of open failure an error should be report
	fd_dma = open("/dev/dma", O_RDWR);

	gpio_fft(256);
	gpio_fft_reset();
	gpio_fft(fft_size);
	int ret = fft_changed(fft_size);
	if(ret < 0)
	{
		return -1;
	}
	while(1)
	{
		pna_printf(">>");
		//scanf("%s\r", buffer);
		int ret = pna_gets(buffer, 1000);
		// printf("received: %s --- %d\r\n", buffer, ret);
		
		if(strcmp(buffer,"")==0)
		{
			break;
		}
		token = strtok(buffer, delim);
		if( strcmp(token, "tx_bandwidth")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf("tx_bandwidth: %lld \r\n", get_bandwidth(__TX));
			}
			else
			{
				long long bandwidth = get_frequency(token);
				set_bandwidth(__TX, bandwidth);
				pna_printf("tx_bandwidth: %lld \r\n", bandwidth);
			}
		}
		else if( strcmp(token, "rx_bandwidth")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf("rx_bandwidth: %lld \r\n",  get_bandwidth(__RX));
			}
			else
			{
				long long bandwidth = get_frequency(token);
				set_bandwidth(__RX, bandwidth);
				pna_printf("rx_bandwidth: %lld \r\n", bandwidth);
			}
		}
		else if( strcmp(token, "st")==0 ) // sweep time
		{
			pna_printf("sweep_time: %lf \r\n",  sweep_time);
		}
		else if( strcmp(token, "fillpro")==0 ) // sweep time
		{
			double rx_freq_mhz = rx_freq / 1E6;
			long long sw_span;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf("---------------------------------------------------------------\r\n"
								"sweep: arguments are not enough.\r\n"
								"Capture spectrum of signal from port argument.\r\n"
								"Usage:\r\n    sweep [port#][span][profile_flag]\r\n");
				continue;
			}
			else
			{
				sw_span = get_frequency(token);
			}
			is_profile_empty = false;
			double span_mhz = sw_span/1E6;
			double lo_start_freq = rx_freq_mhz - span_mhz/2;
			fill_profiles(lo_start_freq, span_mhz);
		}
		else if( strcmp(token, "loadpro")==0 ) // sweep time
		{
			load_profile(0);
		}
		else if( strcmp(token, "vga_gain")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf("---------------------------------------------------------------\r\n"
								"vga_gain: arguments are not enough.\r\n"
								"Read/Write vga_gain with port and value arguments.\r\n"
								"Usage:\r\n    vga_gain [port#] [value]\r\n");
				continue;
			}
			int channel_num = atoi(token);
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf("vga_gain: %lld \r\n", get_vga_gain(channel_num));
			}
			else
			{
				char *sz = NULL;
				long long vga_gain = strtoll(token, &sz, 10);
				set_vga_gain(channel_num, vga_gain);
				pna_printf("vga_gain: %lld \r\n", vga_gain);
			}
		}
		else if( strcmp(token, "lna_gain")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf("---------------------------------------------------------------\r\n"
								"lna_gain: arguments are not enough.\r\n"
								"Read/Write lna_gain with port and value arguments.\r\n"
								"Usage:\r\n    lna_gain [port#] [value]\r\n");
				continue;
			}
			int channel_num = atoi(token);
			token = strtok(NULL, delim);
			// char buf[100];
			if(token==NULL)
			{
				pna_printf("lna_gain: %lld \r\n", get_lna_gain(channel_num));
			}
			else
			{
				char *sz = NULL;
				long long lna_gain = strtoll(token, &sz, 10);
				set_lna_gain(channel_num, lna_gain);
				pna_printf("lna_gain: %lld \r\n", lna_gain);
			}
		}
		else if( strcmp(token, "gain_control_mode")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf("---------------------------------------------------------------\r\n"
								"gain_control_mode: arguments are not enough.\r\n"
								"Read/Write gain_control_mode with port and value arguments.\r\n"
								"Usage:\r\n    gain_control_mode [port#] [value]\r\n");
				continue;
			}
			int channel_num = atoi(token);
			token = strtok(NULL, delim);
			char buf[1024];
			if(token==NULL)
			{
				get_gain_control_mode(channel_num, buf);
				pna_printf("gain_control_mode: %s\r\n", buf);
			}
			else
			{
				set_gain_control_mode(channel_num, token);
				pna_printf("gain_control_mode: %s\r\n", token);
			}
		}
		else if( strcmp(token, "tx_sample_rate")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf("tx_sample_rate: %lld \r\n", get_sample_rate(__TX));
			}
			else
			{
				long long sampling_frequency = get_frequency(token);
				set_sample_rate(__TX, sampling_frequency);
				pna_printf("tx_sample_rate: %lld \r\n", sampling_frequency);
			}
		}
		else if( strcmp(token, "rx_sample_rate")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				rx_sampling_frequency = get_sample_rate(__RX);
				pna_printf("rx_sample_rate: %lld \r\n", rx_sampling_frequency);
			}
			else
			{
				rx_sampling_frequency = get_frequency(token);
				set_sample_rate(__RX, rx_sampling_frequency);
				pna_printf("rx_sample_rate: %lld \r\n", rx_sampling_frequency);
			}
		}
		else if( strcmp(token, "rx_sample_size")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf("rx_sample_size: %d \r\n", fft_size);
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
				pna_printf("rx_sample_size: %d \r\n", fft_size);
			}
		}
		else if( strcmp(token, "tx_sample_size")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf("tx_sample_size: %d \r\n", dds_sample_size);
			}
			else
			{
				dds_sample_size = atoi(token);
				pna_printf("tx_sample_size: %d \r\n", dds_sample_size);
			}
		}
		else if( strcmp(token, "tx_freq")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf("tx_freq: %lld \r\n", get_lo_freq(__TX));
			}
			else
			{
				long long freq = get_frequency(token);
				set_lo_freq(__TX, freq);
				pna_printf("tx_freq: %lld \r\n", freq);
			}
		}
		else if( strcmp(token, "rx_freq")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				rx_freq = get_lo_freq(__RX);
				pna_printf("rx_freq: %lld \r\n", rx_freq);
			}
			else
			{
				rx_freq = get_frequency(token);
				set_lo_freq(__RX, rx_freq);
				pna_printf("rx_freq: %lld \r\n", rx_freq);
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
				;
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
		else if( strcmp(token, "span")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf("span: %lld \r\n", span);
			}
			else
			{
				span = get_frequency(token);
				pna_printf("span: %lld \r\n", span);
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
		else if(strcmp(token,"adc") == 0)
		{
			int channel_num;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf("---------------------------------------------------------------\r\n"
								"adc: arguments are not enough.\r\n"
								"Capture receiver signal from port argument.\r\n"
								"Usage:\r\n    adc [port#]\r\n");
				continue;
			}
			else
			{
				channel_num = atoi(token) - 1;
				if(channel_num > 1)
				{
					pna_printf("---------------------------------------------------------------\r\n"
									"adc: Channel number should be 1 or 2.\r\n"
									"Capture receiver signal from port argument.\r\n"
									"Usage:\r\n    adc [port#]\r\n");
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
				pna_printf("---------------------------------------------------------------\r\n"
								"adc: arguments are not enough.\r\n"
								"Capture receiver signal from port argument.\r\n"
								"Usage:\r\n    adc_iq [port#]\r\n");
				continue;
			}
			else
			{
				channel_num = atoi(token) - 1;
				if(channel_num > 1)
				{
					pna_printf("---------------------------------------------------------------\r\n"
									"adc: Channel number should be 1 or 2.\r\n"
									"Capture receiver signal from port argument.\r\n"
									"Usage:\r\n    adc_iq [port#]\r\n");
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
			fill_rx_buffer(fft_size);
			int uart_size;
			int32_t *adc_data;
			unsigned char uart_tx_buffer[4*MAX_FFT_LENGTH];

			if(channel_num)
			{
				adc_data = rx2_buffer;
			}
			else
			{
				adc_data = rx1_buffer;
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
			pna_write(uart_tx_buffer, 4*uart_size);
			pna_printf("\r\n");
		}
		else if(strcmp(token,"adc_fft") == 0)
		{
			int channel_num;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf("---------------------------------------------------------------\r\n"
								"adc: arguments are not enough.\r\n"
								"Capture receiver signal from port argument.\r\n"
								"Usage:\r\n    adc_fft [port#]\r\n");
				continue;
			}
			else
			{
				channel_num = atoi(token) - 1;
				if(channel_num > 1)
				{
					pna_printf("---------------------------------------------------------------\r\n"
									"adc: Channel number should be 1 or 2.\r\n"
									"Capture receiver signal from port argument.\r\n"
									"Usage:\r\n    adc_fft [port#]\r\n");
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
			// long long span_sweep;
			if(token==NULL)
			{
				pna_printf("---------------------------------------------------------------\r\n"
								"sweep: arguments are not enough.\r\n"
								"Capture spectrum of signal from port argument.\r\n"
								"Usage:\r\n    sweep [port#][span]\r\n");
				continue;
			}
			else
			{
				channel_num = atoi(token) - 1;
				if(channel_num > 1)
				{
					pna_printf("---------------------------------------------------------------\r\n"
									"sweep: Channel number should be 1 or 2.\r\n"
									"Capture spectrum of signal from port argument.\r\n"
									"Usage:\r\n    sweep [port#][span]\r\n");
					continue;
				}
			}
			long long sw_span;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf("---------------------------------------------------------------\r\n"
								"sweep: arguments are not enough.\r\n"
								"Capture spectrum of signal from port argument.\r\n"
								"Usage:\r\n    sweep [port#][span]\r\n");
				continue;
			}
			else
			{
				sw_span = get_frequency(token);
			}
			int compression_enable;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				compression_enable = 1;
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
			double rx_sampling_frequency_mhz = rx_sampling_frequency/1E6;
			
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
					spectrum = pna_fft_dcfixed2(rx2_buffer, fft_size, i);
				}
				else
				{
					spectrum = pna_fft_dcfixed2(rx1_buffer, fft_size, i);
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

			pna_write(uart_tx_buffer, 2*uart_size);
			set_lo_freq(__RX, rx_freq);
			usleep(SET_LO_DELAY);
			pna_printf("\r\n", 2*uart_size);
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
				pna_printf("---------------------------------------------------------------\r\n"
								"sweep: arguments are not enough.\r\n"
								"Capture spectrum of signal from port argument.\r\n"
								"Usage:\r\n    sweep [port#]\r\n");
				continue;
			}
			else
			{
				channel_num = atoi(token) - 1;
				if(channel_num > 1)
				{
					pna_printf("---------------------------------------------------------------\r\n"
									"sweep: Channel number should be 1 or 2.\r\n"
									"Capture spectrum of signal from port argument.\r\n"
									"Usage:\r\n    sweep [port#]\r\n");
					continue;
				}
			}
			long long sw_span;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf("---------------------------------------------------------------\r\n"
								"sweep: arguments are not enough.\r\n"
								"Capture spectrum of signal from port argument.\r\n"
								"Usage:\r\n    sweep [port#]\r\n");
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
			double rx_sampling_frequency_mhz = rx_sampling_frequency/1E6;
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
					spectrum = pna_fft_dcfixed(rx2_buffer, lo_start_freq, fft_size);
				}
				else
				{
					spectrum = pna_fft_dcfixed(rx1_buffer, lo_start_freq, fft_size);
				}
				if(spectrum == NULL)
				{
					free(sweep_buf);
					free(uart_tx_buffer);
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

			pna_write(uart_tx_buffer, 2*uart_size);
			set_lo_freq(__RX, rx_freq);
			usleep(SET_LO_DELAY);
			pna_printf("\r\n");
			free(sweep_buf);
			// free(uart_tx_buffer);
		}
		else if(strcmp(token,"fft_span") == 0)
		{
			int channel_num;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf("---------------------------------------------------------------\r\n"
								"fft: arguments are not enough.\r\n"
								"Capture spectrum of signal from port argument.\r\n"
								"Usage:\r\n    fft_span [port#]\r\n");
				continue;
			}
			else
			{
				channel_num = atoi(token) - 1;
				if(channel_num > 1)
				{
					pna_printf("---------------------------------------------------------------\r\n"
									"fft: Channel number should be 1 or 2.\r\n"
									"Capture spectrum of signal from port argument.\r\n"
									"Usage:\r\n    fft_span [port#]\r\n");
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
			if(span > rx_sampling_frequency || span < 0)
				span = rx_sampling_frequency;
			int span_mhz = span/1E6;
			int rx_sampling_frequency_mhz = rx_sampling_frequency/1E6;
			int removed_span = fft_size*(rx_sampling_frequency_mhz - span_mhz)/rx_sampling_frequency_mhz/2;

			gettimeofday(&tv1, NULL);

			if(channel_num)
			{
				spectrum = pna_fft(rx2_buffer, removed_span, fft_size);
			}
			else
			{
				spectrum = pna_fft(rx1_buffer, removed_span, fft_size);
			}
			if(spectrum == NULL)
				return -1;
			int spectrum_size = fft_size*span_mhz/rx_sampling_frequency_mhz;
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
			pna_write(uart_tx_buffer, 2*uart_size);
			pna_printf("\r\n");
			free(spectrum);
		}
		else if(strcmp(token,"fft") == 0)
		{
			int32_t *spectrum = pna_fft(rx1_buffer, 0, fft_size);
			unsigned char uart_tx_buffer[4*UART_LENGTH];
			int uart_size = compress_data(spectrum, uart_tx_buffer, fft_size);
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
				pna_printf("---------------------------------------------------------------\r\n"
								"fft4: There is not enough arguments\r\n"
								"Usage:\r\n    fft4 [r0/w1] [bytes]\r\n");
				continue;
			}
			int rwfunc = atoi(token);
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf("---------------------------------------------------------------\r\n"
								"fft4: There is not enough arguments\r\n"
								"Usage:\r\n    fft4 [r0/w1] [bytes]\r\n");
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
		else if (strcmp(token, "pulse")==0 )
		{
			int s_size = iio_device_get_sample_size(iio_dac);
			int period_num;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf("---------------------------------------------------------------\r\n"
								"pulse: arguments are not enough.\r\n"
								"Generate pulse signal with period, amplitude and port arguments.\r\n"
								"Usage:\r\n    pulse [period] [amplitude] [port#]\r\n");
				continue;
			}
			else
			{
				period_num = atoi(token);
			}
			float amplitude;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				amplitude = 0.5;
			}
			else
			{
				amplitude = atof(token);
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
			int channel_num;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				channel_num = 0;
			}
			else
			{
				channel_num = atoi(token) - 1;
				if(channel_num > 1)
				{
					pna_printf("---------------------------------------------------------------\r\n"
									"pulse: Channel number should be 1 or 2.\r\n"
									"Generate pulse signal with period, amplitude and port arguments.\r\n"
									"Usage:\r\n    pulse [period] [amplitude] [port#]\r\n");
					continue;
				}
			}
			int amplitude_int = amplitude*DAC_MAX_VAL;
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

					dac_buf[(j+i*period_sample_count)*s_size+channel_num*4] = (int8_t)(pulse_int%256);   // LSB
					dac_buf[(j+i*period_sample_count)*s_size+channel_num*4+1] = (int8_t)(pulse_int/256);     // MSB
					dac_buf[(j+i*period_sample_count)*s_size+channel_num*4+2] = 0;     // MSB
					dac_buf[(j+i*period_sample_count)*s_size+channel_num*4+3] = 0;     // MSB
				}
			}
			create_dds_buffer(dac_buf, dds_sample_size);
		}
		else if (strcmp(token, "sin")==0 )
		{
			int s_size = iio_device_get_sample_size(iio_dac);
			int period_num;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf("---------------------------------------------------------------\r\n"
								"sin: arguments are not enough.\r\n"
								"Generate sinous signal with port, period and amplitude arguments.\r\n"
								"Usage:\r\n    sin [port#] [period] [amplitude]\r\n");
				continue;
			}
			else
			{
				period_num = atoi(token);
			}
			float amplitude;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				amplitude = .5;
			}
			else
			{
				amplitude = atof(token);
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
				if(channel_num > 1)
				{
					pna_printf("---------------------------------------------------------------\r\n"
									"sin: Channel number should be 1 or 2.\r\n"
									"Generate sinous signal with port, period and amplitude arguments.\r\n"
									"Usage:\r\n    sin [period] [amplitude] [port#]\r\n");
					continue;
				}
			}
			int amplitude_int = amplitude*DAC_MAX_VAL/2;
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

					dac_buf[(j+i*period_sample_count)*s_size+channel_num*2] = (int8_t)(sin_int%256);   // LSB
					dac_buf[(j+i*period_sample_count)*s_size+channel_num*2+1] = (int8_t)(sin_int/256);     // MSB
					dac_buf[(j+i*period_sample_count)*s_size+channel_num*2+2] = (int8_t)(cos_int%256);   // LSB
					dac_buf[(j+i*period_sample_count)*s_size+channel_num*2+3] = (int8_t)(cos_int/256);     // MSB
				}
			}
			create_dds_buffer(dac_buf, dds_sample_size);
		}
		else if (strcmp(token, "dc")==0 )
		{
			int s_size = iio_device_get_sample_size(iio_dac);
			float amplitude;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf("---------------------------------------------------------------\r\n"
								"dc: arguments are not enough.\r\n"
								"Generate DC voltage with amplitude and port arguments.\r\n"
								"Usage:\r\n    dc [amplitude] [port#]\r\n");
				continue;
			}
			else
			{
				amplitude = atof(token);
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
				if(channel_num > 1)
				{
					pna_printf("---------------------------------------------------------------\r\n"
									"dc: Channel number should be 1 or 2.\r\n"
									"Generate DC voltage with amplitude and port arguments.\r\n"
									"Usage:\r\n    dc [amplitude] [port#]\r\n");
					continue;
				}
			}
			int amplitude_int = amplitude*DAC_MAX_VAL;

			for (int i=0 ; i<dds_sample_size ; i++)
			{
				double pulse = amplitude_int;
				int16_t pulse_int = (int16_t)(pulse);

				dac_buf[i*s_size+channel_num*4] = (int8_t)(pulse_int%256);   // LSB
				dac_buf[i*s_size+channel_num*4+1] = (int8_t)(pulse_int/256);     // MSB
				dac_buf[i*s_size+channel_num*4+2] = 0;     // MSB
				dac_buf[i*s_size+channel_num*4+3] = 0;     // MSB
			}
			create_dds_buffer(dac_buf, dds_sample_size);
		}
		else if (strcmp(token, "sinc")==0 )
		{
			//echo 2450000000 > /sys/bus/iio/devices/iio\:device0/out_altvoltage0_RX_LO_frequency
			//stty -F /dev/ttyPS0 -onlcr
			//sample_rate 61400000
			//bandwidth 56000000
			//sinc 1 0.9 1               sinc 1 5 1
			//echo 61400000 > /sys/bus/iio/devices/iio\:device0/out_voltage_sampling_frequency;pna-iio bandwidth 56000000;pna-iio sinc 1 0.9 1
			int s_size = iio_device_get_sample_size(iio_dac);
			double dds_freq;
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf("---------------------------------------------------------------\r\n"
								"sinc: arguments are not enough.\r\n"
								"Generate sinc signal with frequency and port arguments.\r\n"
								"Usage:\r\n    sinc [frequency] [port#]\r\n");
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
				if(channel_num > 1)
				{
					pna_printf("---------------------------------------------------------------\r\n"
									"sinc: Channel number should be 1 or 2.\r\n"
									"Generate sinc signal with frequency and port arguments.\r\n"
									"Usage:\r\n    sinc [frequency] [port#]\r\n");
					continue;
				}
			}
			for (int i=0 ; i<dds_sample_size ; i++)
			{
				double x = i-dds_sample_size/2;
				double sinc;
				if ( x==0 )
				{
					sinc = DAC_MAX_VAL;
				}
				else
				{
					sinc = sin(x/dds_freq)/(x/dds_freq)*DAC_MAX_VAL;
				}
				int16_t sinc_int = (int16_t)(sinc);

				dac_buf[i*s_size+channel_num*4] = (int8_t)(sinc_int%256);   // LSB
				dac_buf[i*s_size+channel_num*4+1] = (int8_t)(sinc_int/256);     // MSB
				dac_buf[i*s_size+channel_num*4+2] = 0;     // MSB
				dac_buf[i*s_size+channel_num*4+3] = 0;     // MSB
				// pna_printf("DAC Buffer[%d]= %d ,\tx= %f\t sinc=%f, \t sinc_int=%d\r\n", i*s_size+channel_num*2+1, dac_buf[i*s_size+channel_num*2+1], x, sinc, sinc_int );
			}
			//dac_buf[sample_count/2*s_size + channel_num*2+1] = 127;
			//dac_buf[sample_count/2*s_size + channel_num*2] = 240;

			pna_printf("Channel Num = %d\r\n", channel_num );
			pna_printf("DDS Freq = %f\r\n", dds_freq);
			pna_printf("Sample Count= %d\r\n", dds_sample_size);
			pna_printf("Sample Size = %d\r\n", s_size );
			for ( int i=dds_sample_size/2-5 ; i<dds_sample_size/2+5 ; i++)
			{
				pna_printf("DAC Buffer[%d]= %d ,%d , x= %d\r\n", i*s_size+channel_num*2+1, (uint8_t) dac_buf[i*s_size+channel_num*2+1], (uint8_t)dac_buf[i*s_size+channel_num*2],i-dds_sample_size/2);
			}
			create_dds_buffer(dac_buf, dds_sample_size);
		}
		else if( strcmp(token, "dbg")==0 )
		{
			char str[80], buffer_dbg[80];
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				pna_printf("---------------------------------------------------------------\r\n"
								"debug: arguments are not enough.\r\n"
								"debug mode with attribute and value arguments.\r\n"
								"Usage:\r\n    dbg [attribute] [value]\r\n");
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
				pna_printf("---------------------------------------------------------------\r\n"
								"reg: arguments are not enough.\r\n"
								"debug mode with address and value arguments.\r\n"
								"Usage:\r\n    reg [address] [value]\r\n");
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
		}
		else if( strcmp(token, "exit")==0 )
		{
			close(fd_dma);
			pna_close_interface();
			return 0;
		}
	}
	close(fd_dma);
	pna_close_interface();
	return 0;
}
