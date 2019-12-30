#include "pna-base.h"

extern struct iio_context *ctx;
extern struct iio_device *dev, *dds, *cap, *udc_rx, *udc_tx;
extern struct iio_channel *dds_ch0, *dds_ch1;
extern struct iio_channel *rx_dev_ch0, *rx_dev_ch1;
extern struct iio_channel *tx_dev_ch0, *tx_dev_ch1;
extern struct iio_channel *tx_alt_dev_ch0, *tx_alt_dev_ch1;
extern struct iio_channel *cap_ch0, *cap_ch1;

extern struct iio_device *iio_dac;
extern struct iio_channel *iio_ch;
extern unsigned dds_tones_count;
extern const char *dds_name;
extern struct iio_buffer *dds_buffer;
extern struct iio_buffer *capture_buffer;

extern const char *rx_freq_name, *tx_freq_name;
extern int32_t rx1_buffer [MAX_FFT_LENGTH*2]; // FIXME: *2 should be carefully removed

int dds_sample_size;
// int span_number = 1;
long long span = MAX_BW;
extern int8_t dac_buf[8*MAX_FFT_LENGTH];
extern int fd_dma;

int main (int argc, char **argv)
{
	char buffer[1000];
	const char delim[2] = " ";
	char* token;
	unsigned int fft_size = 1024; //16bit(I)+16bit(Q) = 32bit data
	// fft_size = atoi(argv[1]);
	dds_sample_size = fft_size;
	// printf("fft_size=%d\n", fft_size );
	int i;

	init_all_gpio();
	ctx = iio_create_default_context();
	// printf("flag3\r\n");
	if (ctx)
	{
		// init_device_list();
		fmcomms2_init();
		// printf("flag4\r\n");
		dds_init();
		// printf("flag5\r\n");
	}
	// printf("flag1\r\n");
	init_rx_channel(fft_size);
	// printf("flag2\r\n");
	if( argc>1 )
	{
		if( strcmp(argv[1], "-v")==0 )
		{
			float version = 0.6;
			printf("Version %f \r\n", version);

			return 0;
		}
		else if ( strcmp(argv[1], "--help")==0 )
		{
			printf("%s\r\n",
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
	}
	///////////////// FIXME: in case of open failure an error should be report
	fd_dma = open("/dev/dma", O_RDWR);

		gpio_fft(256);
	gpio_fft_reset();
		gpio_fft(fft_size);
	///////////////// FIXME
	while(1)
	{
		printf(">>");
		//scanf("%s\r", buffer);
		gets(buffer);
		if(strcmp(buffer,"")==0)
		{
			break;
		}
		//printf("buffer = %s\r\n", buffer);
		token = strtok(buffer, delim);
		//printf("Debug Flag #-4\r\n");
		if( strcmp(token, "tx_bandwidth")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				long long bandwidth;
				iio_channel_attr_read_longlong(tx_dev_ch0, "rf_bandwidth", &bandwidth);
				printf("tx_bandwidth: %lld \r\n", bandwidth);
			}
			else
			{
				long long bandwidth = get_frequency(token);
				iio_channel_attr_write_longlong(tx_dev_ch0, "rf_bandwidth", bandwidth);
				printf("tx_bandwidth: %lld \r\n", bandwidth);
			}
		}
		else if( strcmp(token, "rx_bandwidth")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				long long bandwidth;
				iio_channel_attr_read_longlong(rx_dev_ch0, "rf_bandwidth", &bandwidth);
				printf("rx_bandwidth: %lld \r\n", bandwidth);
			}
			else
			{
				long long bandwidth = get_frequency(token);
				iio_channel_attr_write_longlong(rx_dev_ch0, "rf_bandwidth", bandwidth);
				printf("rx_bandwidth: %lld \r\n", bandwidth);
			}
		}
		else if( strcmp(token, "vga_gain")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				printf("---------------------------------------------------------------\r\n"
								"vga_gain: arguments are not enough.\r\n"
								"Read/Write vga_gain with port and value arguments.\r\n"
								"Usage:\r\n    vga_gain [port#] [value]\r\n");
				continue;
			}
			int channel_num = atoi(token);
			token = strtok(NULL, delim);
			// char buf[100];
			if(token==NULL)
			{
				// int ret = iio_channel_attr_read(tx_dev_ch0, "hardwaregain", buf, sizeof(buf));
				long long vga_gain;
				if(channel_num == 2)
					iio_channel_attr_read_longlong(tx_dev_ch1, "hardwaregain", &vga_gain);
				else if(channel_num == 1)
					iio_channel_attr_read_longlong(tx_dev_ch0, "hardwaregain", &vga_gain);
				else
				{
					printf("---------------------------------------------------------------\r\n"
									"vga_gain: arguments are not valid.\r\n"
									"Read/Write vga_gain with port and value arguments.\r\n"
									"Usage:\r\n    vga_gain [port#] [value]\r\n");
					continue;
				}
				printf("vga_gain: %lld \r\n", vga_gain);
			}
			else
			{
				long long vga_gain = atof(token);
				if(channel_num == 2)
					iio_channel_attr_write_longlong(tx_dev_ch1, "hardwaregain", vga_gain);
				else if(channel_num == 1)
					iio_channel_attr_write_longlong(tx_dev_ch0, "hardwaregain", vga_gain);
				else
				{
					printf("---------------------------------------------------------------\r\n"
									"vga_gain: arguments are not valid.\r\n"
									"Read/Write vga_gain with port and value arguments.\r\n"
									"Usage:\r\n    vga_gain [port#] [value]\r\n");
					continue;
				}
				printf("vga_gain: %lld \r\n", vga_gain);
			}
		}
		else if( strcmp(token, "lna_gain")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				printf("---------------------------------------------------------------\r\n"
								"lna_gain: arguments are not enough.\r\n"
								"Read/Write lna_gain with port and value arguments.\r\n"
								"Usage:\r\n    lna_gain [port#] [value]\r\n");
				continue;
			}
			int channel_num = atoi(token);
			token = strtok(NULL, delim);
			char buf[100];
			if(token==NULL)
			{
				long long lna_gain;
				// iio_channel_attr_read_longlong(rx_dev_ch0, "hardware_gain_rx1", buf, sizeof(buf));
				// printf("lna_gain_rx1: %s \r\n", lna_gain);
				if(channel_num == 2)
					iio_channel_attr_read_longlong(rx_dev_ch1, "hardwaregain", &lna_gain);
				else if(channel_num == 1)
					iio_channel_attr_read_longlong(rx_dev_ch0, "hardwaregain", &lna_gain);
				else
				{
					printf("---------------------------------------------------------------\r\n"
									"lna_gain: arguments are not valid.\r\n"
									"Read/Write lna_gain with port and value arguments.\r\n"
									"Usage:\r\n    lna_gain [port#] [value]\r\n");
					continue;
				}
				printf("lna_gain: %lld \r\n", lna_gain);
			}
			else
			{
				char *sz = NULL;
				long long lna_gain = strtoll(token, &sz, 10);
				if(channel_num == 2)
				{
					iio_channel_attr_write_longlong(rx_dev_ch1, "hardwaregain", lna_gain);
				}
				else if(channel_num == 1)
				{
					iio_channel_attr_write_longlong(rx_dev_ch0, "hardwaregain", lna_gain);
				}
				else
				{
					printf("---------------------------------------------------------------\r\n"
									"lna_gain: arguments are not valid.\r\n"
									"Read/Write lna_gain with port and value arguments.\r\n"
									"Usage:\r\n    lna_gain [port#] [value]\r\n");
					continue;
				}
				printf("lna_gain: %lld \r\n", lna_gain);
				// int ret = iio_channel_attr_write_longlong(rx_dev_ch0, "hardware_gain_rx1", token);
				// printf("lna_gain_rx1: %s - return value: %d \r\n", token, ret);
			}
		}
		else if( strcmp(token, "gain_control_mode")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				printf("---------------------------------------------------------------\r\n"
								"gain_control_mode: arguments are not enough.\r\n"
								"Read/Write gain_control_mode with port and value arguments.\r\n"
								"Usage:\r\n    gain_control_mode [port#] [value]\r\n");
				continue;
			}
			int channel_num = atoi(token);
			token = strtok(NULL, delim);
			char buf[1024];
			int ret;
			if(token==NULL)
			{
				if(channel_num == 2)
					ret = iio_channel_attr_read(rx_dev_ch1, "gain_control_mode", buf, sizeof(buf));
				else if(channel_num == 1)
					ret = iio_channel_attr_read(rx_dev_ch0, "gain_control_mode", buf, sizeof(buf));
				else
				{
					printf("---------------------------------------------------------------\r\n"
									"gain_control_mode: arguments are not valid.\r\n"
									"Read/Write gain_control_mode with port and value arguments.\r\n"
									"Usage:\r\n    gain_control_mode [port#] [value]\r\n");
					continue;
				}
				printf("gain_control_mode: %s \r\n", buf, ret);
			}
			else
			{
				if(channel_num == 2)
					ret = iio_channel_attr_write(rx_dev_ch1, "gain_control_mode", token);
				else if(channel_num == 1)
					ret = iio_channel_attr_write(rx_dev_ch0, "gain_control_mode", token);
				else
				{
					printf("---------------------------------------------------------------\r\n"
									"gain_control_mode: arguments are not valid.\r\n"
									"Read/Write gain_control_mode with port and value arguments.\r\n"
									"Usage:\r\n    gain_control_mode [port#] [value]\r\n");
					continue;
				}
				printf("gain_control_mode: %s \r\n", token, ret);
			}
		}
		else if( strcmp(token, "tx_sample_rate")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				long long sampling_frequency;
				iio_channel_attr_read_longlong(tx_dev_ch0, "sampling_frequency", &sampling_frequency);
				printf("tx_sample_rate: %lld \r\n", sampling_frequency);
			}
			else
			{
				long long sampling_frequency = get_frequency(token);
				iio_channel_attr_write_longlong(tx_dev_ch0, "sampling_frequency", sampling_frequency);
				printf("tx_sample_rate: %lld \r\n", sampling_frequency);
			}
		}
		else if( strcmp(token, "rx_sample_size")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				printf("rx_sample_size: %d \r\n", fft_size);
			}
			else
			{
				fft_size = atoi(token);
				gpio_fft(fft_size);
				init_rx_channel(fft_size);
				printf("rx_sample_size: %d \r\n", fft_size);
			}
		}
		else if( strcmp(token, "tx_sample_size")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				printf("tx_sample_size: %d \r\n", dds_sample_size);
			}
			else
			{
				dds_sample_size = atoi(token);
				printf("tx_sample_size: %d \r\n", dds_sample_size);
			}
		}
		else if( strcmp(token, "rx_sample_rate")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				long long sampling_frequency;
				iio_channel_attr_read_longlong(rx_dev_ch0, "sampling_frequency", &sampling_frequency);
				printf("rx_sample_rate: %lld \r\n", sampling_frequency);
			}
			else
			{
				long long sampling_frequency = get_frequency(token);
				iio_channel_attr_write_longlong(rx_dev_ch0, "sampling_frequency", sampling_frequency);
				printf("rx_sample_rate: %lld \r\n", sampling_frequency);
			}
		}
		else if( strcmp(token, "tx_freq")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				long long freq;
				iio_channel_attr_read_longlong(tx_alt_dev_ch1, tx_freq_name, &freq);
				printf("tx_freq: %lld \r\n", freq);
			}
			else
			{
				long long freq = get_frequency(token);
				iio_channel_attr_write_longlong(tx_alt_dev_ch1, tx_freq_name, freq);
				printf("tx_freq: %lld \r\n", freq);
			}
		}
		else if( strcmp(token, "rx_freq")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				long long freq;
				iio_channel_attr_read_longlong(tx_alt_dev_ch0, rx_freq_name, &freq);
				printf("rx_freq: %lld \r\n", freq);
			}
			else
			{
				long long freq = get_frequency(token);
				iio_channel_attr_write_longlong(tx_alt_dev_ch0, rx_freq_name, freq);
				printf("rx_freq: %lld \r\n", freq);
			}
		}
		else if( strcmp(token, "rx_port")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				char rx_port[100];
				iio_channel_attr_read(iio_device_find_channel(dev, "voltage0", false), "rf_port_select", rx_port, 100);
				printf("rx_port: %s\r\n", rx_port);
			}
			else
			{
				char *rx_port = token;
				iio_channel_attr_write(iio_device_find_channel(dev, "voltage0", false),
			   		"rf_port_select", rx_port);
				printf("rx_port: %s \r\n", rx_port);
			}
		}
		else if( strcmp(token, "tx_port")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				char tx_port[100];
				iio_channel_attr_read(iio_device_find_channel(dev, "voltage0", true), "rf_port_select", tx_port, 100);
				printf("tx_port: %s \r\n", tx_port);
			}
			else
			{
				char *tx_port = token;
				iio_channel_attr_write(iio_device_find_channel(dev, "voltage0", true),
				   "rf_port_select", tx_port);
				printf("tx_port: %s \r\n", tx_port);
			}
		}
		else if( strcmp(token, "sample_size")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				int sample_size = iio_device_get_sample_size(iio_dac);
				printf("sample_size: %d \r\n", sample_size);
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
				bool fir_en;
				iio_channel_attr_read_bool(rx_dev_ch0, "filter_fir_en", &fir_en);
				printf("rx_fir_en: %d \r\n", fir_en);
			}
			else
			{
				bool fir_en = atoi(token);
				iio_channel_attr_write_bool(rx_dev_ch0, "filter_fir_en", fir_en);
				printf("rx_fir_en: %d \r\n", fir_en);
			}
		}
		else if( strcmp(token, "tx_fir_en")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				bool fir_en;
				iio_channel_attr_read_bool(tx_dev_ch0, "filter_fir_en", &fir_en);
				printf("tx_fir_en: %d \r\n", fir_en);
			}
			else
			{
				bool fir_en = atoi(token);
				iio_channel_attr_write_bool(tx_dev_ch0, "filter_fir_en", fir_en);
				printf("tx_fir_en: %d \r\n", fir_en);
			}
		}
		else if( strcmp(token, "span")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				printf("span: %lld \r\n", span);
			}
			else
			{
				span = get_frequency(token);
				printf("span: %lld \r\n", span);
			}
		}
		else if(strcmp(token,"test") == 0)
		{
			int i;
			for( i=0; i<fft_size; i++ )
			{
				char first_byte = i%256;
				char second_byte = i/256;
				printf("%d %d\r\n",first_byte, second_byte);
			}
			printf("\r\n");
		}
		else if(strcmp(token,"freset") == 0)
		{
			gpio_fft_reset();
			printf("\r\n");
		}
		else if(strcmp(token,"adc") == 0)
		{
			int uart_size = 1024;
			if(fft_size < 1024)
			{
				uart_size = fft_size;
			}
			int window_size = fft_size/uart_size;
			double avg_adc_window;
			int32_t rx_buffer_i;
			int16_t rx_buffer_i_16;
			unsigned char uart_tx_buffer[2*UART_LENGTH];
			fill_rx_buffer(fft_size);
			for(int i=0; i<uart_size; i++ )
			{
				avg_adc_window = 0;
				for(int j=0; j<window_size; j++)
				{
					rx_buffer_i_16 = rx1_buffer[window_size*i+j] & 0x0000FFFF;
					rx_buffer_i = rx_buffer_i_16;
					avg_adc_window += rx_buffer_i;
				}
				avg_adc_window = avg_adc_window/window_size;
				rx_buffer_i = floor(avg_adc_window);
				rx_buffer_i = rx_buffer_i & 0x0000FFFF;
				char first_byte = rx_buffer_i%256;
				char second_byte = rx_buffer_i/256;
				uart_tx_buffer[2*i] = first_byte;
				uart_tx_buffer[2*i+1] = second_byte;
				// printf("%c%c\r\n",first_byte, second_byte);
				// printf("%d : %d\r\n", i, rx_buffer_i);
			}
			fwrite(uart_tx_buffer, 1, 2*uart_size, stdout);
			printf("\r\n");
		}
		else if(strcmp(token,"fft_span") == 0)
		{
			fft_span(rx1_buffer, span, fft_size);
		}
		else if(strcmp(token,"fft") == 0)
		{
			fft(rx1_buffer, fft_size);
		}
		else if(strcmp(token,"fft2") == 0)
		{
			fft2(rx1_buffer, fft_size);
		}
		else if(strcmp(token,"fft3") == 0)
		{
			fft3(rx1_buffer, fft_size);
		}
		else if(strcmp(token,"fft4") == 0)
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				printf("---------------------------------------------------------------\r\n"
								"fft4: There is not enough arguments\r\n"
								"Usage:\r\n    fft4 [r0/w1] [bytes]\r\n");
				continue;
			}
			int rwfunc = atoi(token);
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				printf("---------------------------------------------------------------\r\n"
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
			printf("fft_status#1 = %d\r\n", gpio_fft_status());
			if(rwfunc)
			{
				printf("write %d bytes to dma.\r\n", write_DMA(
							(char *)uart_tx_buffer, num_bytes));
			}
			else
			{
				printf("read %d bytes from dma.\r\n", read_DMA(
							(char *)uart_tx_buffer, num_bytes));
			}
			printf("fft_status#2 = %d\r\n", gpio_fft_status());
			printf("\r\n");
		}
		else if (strcmp(token, "pulse")==0 )
		{
			int s_size = iio_device_get_sample_size(iio_dac);
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				printf("---------------------------------------------------------------\r\n"
								"Pulse: arguments are not enough.\r\n"
								"Generate pulse signal with port, period and amplitude arguments.\r\n"
								"Usage:\r\n    pulse [port#] [period] [amplitude]\r\n");
				continue;
			}
			int channel_num = atoi(token);
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				printf("---------------------------------------------------------------\r\n"
								"Pulse: arguments are not enough.\r\n"
								"Generate pulse signal with port, period and amplitude arguments.\r\n"
								"Usage:\r\n    pulse [port#] [period] [amplitude]\r\n");
				continue;
			}
			double period_num = atof(token);
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				printf("---------------------------------------------------------------\r\n"
								"Pulse: arguments are not enough.\r\n"
								"Generate pulse signal with port, period and amplitude arguments.\r\n"
								"Usage:\r\n    pulse [port#] [period] [amplitude]\r\n");
				continue;
			}
			double amplitude = atof(token);
			int amplitude_int = amplitude*DAC_MAX_VAL;
			int period_sample_count = dds_sample_size/period_num;

			for (int i=0 ; i<period_num ; i++)
			{
				double pulse;
				for(int j=0; j<period_sample_count; j++)
				{
					double x = j;
					if ( j<period_sample_count/2 )
					{
						pulse = amplitude_int;
					}
					else
					{
						pulse = 0;
					}
					int16_t pulse_int = (int16_t)(pulse);

					dac_buf[(j+i*period_sample_count)*s_size+channel_num*2] = (int8_t)(pulse_int%256);   // LSB
					dac_buf[(j+i*period_sample_count)*s_size+channel_num*2+1] = (int8_t)(pulse_int/256);     // MSB
				}
			}
			create_dds_buffer(dac_buf, dds_sample_size);
		}
		else if (strcmp(token, "sin")==0 )
		{
			int s_size = iio_device_get_sample_size(iio_dac);
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				printf("---------------------------------------------------------------\r\n"
								"sin: arguments are not enough.\r\n"
								"Generate sinous signal with port, period and amplitude arguments.\r\n"
								"Usage:\r\n    sin [port#] [period] [amplitude]\r\n");
				continue;
			}
			int channel_num = atoi(token);
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				printf("---------------------------------------------------------------\r\n"
								"sin: arguments are not enough.\r\n"
								"Generate sinous signal with port, period and amplitude arguments.\r\n"
								"Usage:\r\n    sin [port#] [period] [amplitude]\r\n");
				continue;
			}
			double period_num = atof(token);
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				printf("---------------------------------------------------------------\r\n"
								"sin: arguments are not enough.\r\n"
								"Generate sinous signal with port, period and amplitude arguments.\r\n"
								"Usage:\r\n    sin [port#] [period] [amplitude]\r\n");
				continue;
			}
			double amplitude = atof(token);
			int amplitude_int = amplitude*DAC_MAX_VAL/2;
			int period_sample_count = dds_sample_size/period_num;

			for (int i=0 ; i<period_num ; i++)
			{
				double sinous;
				for(int j=0; j<period_sample_count; j++)
				{
					double x = j*2*PI;
					x = x/period_sample_count;
					sinous = sin(x)*amplitude_int;
					int16_t sin_int = (int16_t)(sinous);

					dac_buf[(j+i*period_sample_count)*s_size+channel_num*2] = (int8_t)(sin_int%256);   // LSB
					dac_buf[(j+i*period_sample_count)*s_size+channel_num*2+1] = (int8_t)(sin_int/256);     // MSB
				}
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
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				printf("---------------------------------------------------------------\r\n"
								"sinc: arguments are not enough.\r\n"
								"Generate sinc signal with port, period and amplitude arguments.\r\n"
								"Usage:\r\n    sinc [port#] [sinc-frequency]\r\n");
				continue;
			}
			int channel_num = atoi(token);
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				printf("---------------------------------------------------------------\r\n"
								"sinc: arguments are not enough.\r\n"
								"Generate sinc signal with port, period and amplitude arguments.\r\n"
								"Usage:\r\n    sinc [port#] [sinc-frequency]\r\n");
				continue;
			}
			double dds_freq = atof(token);
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

				dac_buf[i*s_size+channel_num*2] = (int8_t)(sinc_int%256);   // LSB
				dac_buf[i*s_size+channel_num*2+1] = (int8_t)(sinc_int/256);     // MSB
				// printf("DAC Buffer[%d]= %d ,\tx= %f\t sinc=%f, \t sinc_int=%d\r\n", i*s_size+channel_num*2+1, dac_buf[i*s_size+channel_num*2+1], x, sinc, sinc_int );
			}
			//dac_buf[sample_count/2*s_size + channel_num*2+1] = 127;
			//dac_buf[sample_count/2*s_size + channel_num*2] = 240;

			printf("Channel Num = %d\r\n", channel_num );
			printf("DDS Freq = %f\r\n", dds_freq);
			printf("Sample Count= %d\r\n", dds_sample_size);
			printf("Sample Size = %d\r\n", s_size );
			for ( int i=dds_sample_size/2-5 ; i<dds_sample_size/2+5 ; i++)
			{
				printf("DAC Buffer[%d]= %d ,%d , x= %d\r\n", i*s_size+channel_num*2+1, (uint8_t) dac_buf[i*s_size+channel_num*2+1], (uint8_t)dac_buf[i*s_size+channel_num*2],i-dds_sample_size/2);
			}
			create_dds_buffer(dac_buf, dds_sample_size);
		}
		else if( strcmp(token, "exit")==0 )
		{
			close(fd_dma);
			return 0;
		}
	}
	close(fd_dma);
	return 0;
}
