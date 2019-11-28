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
extern int32_t rx1_buffer [FFT_LENGTH];

int dds_sample_size = FFT_LENGTH;
int span_number = 1;
extern int8_t dac_buf[16384];

int main (int argc, char **argv)
{
	char buffer[1000];
	const char delim[2] = " ";
	char* token;
	unsigned long fft_size = FFT_LENGTH; //16bit(I)+16bit(Q) = 32bit data
	int i;
	int16_t *fft_abs = (int16_t *) malloc(sizeof(uint16_t) * fft_size);
	int16_t *fft_phase = (int16_t *) malloc(sizeof(uint16_t) * fft_size);

	ctx = iio_create_default_context();
	if (ctx)
	{
		// init_device_list();
		fmcomms2_init();
		dds_init();
	}

	init_rx_channel();

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

	while(1)
	{
		printf(">>");
		//scanf("%s\r", buffer);
		gets(buffer);
		if(strcmp(buffer,"")==0)
		{
			return 0;
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
			// char buf[100];
			if(token==NULL)
			{
				// int ret = iio_channel_attr_read(tx_dev_ch0, "hardwaregain", buf, sizeof(buf));
				long long vga_gain;
				iio_channel_attr_read_longlong(tx_dev_ch0, "hardwaregain", &vga_gain);
				printf("vga_gain_tx1: %lld \r\n", vga_gain);
			}
			else
			{
				size_t sz = NULL;
				long long vga_gain = atof(token);
				iio_channel_attr_write_longlong(tx_dev_ch0, "hardwaregain", vga_gain);
				printf("vga_gain_tx1: %lld \r\n", vga_gain);
			}
		}
		else if( strcmp(token, "lna_gain")==0 )
		{
			token = strtok(NULL, delim);
			char buf[100];
			if(token==NULL)
			{
				long long lna_gain;
				// iio_channel_attr_read_longlong(rx_dev_ch0, "hardware_gain_rx1", buf, sizeof(buf));
				// printf("lna_gain_rx1: %s \r\n", lna_gain);
				iio_channel_attr_read_longlong(rx_dev_ch0, "hardwaregain", &lna_gain);
				printf("lna_gain_rx1: %lld \r\n", lna_gain);
			}
			else
			{
				size_t sz = NULL;
				long long lna_gain = strtoll(token, &sz, 10);
				iio_channel_attr_write_longlong(rx_dev_ch0, "hardwaregain", lna_gain);
				printf("lna_gain_rx1: %lld \r\n", lna_gain);
				// int ret = iio_channel_attr_write_longlong(rx_dev_ch0, "hardware_gain_rx1", token);
				// printf("lna_gain_rx1: %s - return value: %d \r\n", token, ret);
			}
		}
		else if( strcmp(token, "gain_control_mode")==0 )
		{
			token = strtok(NULL, delim);
			char buf[1024];
			if(token==NULL)
			{
				int ret = iio_channel_attr_read(
						rx_dev_ch0, "gain_control_mode", buf, sizeof(buf));
				printf("gain_control_mode: %s - return value = %d \r\n", buf, ret);
			}
			else
			{
				int ret = iio_channel_attr_write(rx_dev_ch0, "gain_control_mode", token);
				printf("gain_control_mode: %s - return value = %d \r\n", token, ret);
			}
		}
		else if( strcmp(token, "tx_sample_rate")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				long long sampling_frequency;
				iio_channel_attr_read_longlong(tx_dev_ch0, "sampling_frequency", &sampling_frequency);
				printf("tx_sampling_frequency: %lld \r\n", sampling_frequency);
			}
			else
			{
				long long sampling_frequency = get_frequency(token);
				iio_channel_attr_write_longlong(tx_dev_ch0, "sampling_frequency", sampling_frequency);
				printf("tx_sampling_frequency: %lld \r\n", sampling_frequency);
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
				FILE * gpio_file;
				fft_size = atoi(token);
				free(fft_abs);
				free(fft_phase);
				fft_abs = (int16_t *) malloc(sizeof(uint16_t) * fft_size);
				fft_phase = (int16_t *) malloc(sizeof(uint16_t) * fft_size);
				printf("fft size changing: %d \r\n", fft_size);
				gpio_fft(fft_size);
				printf("rx_sample_size changed to: %d \r\n", fft_size);
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
				printf("rx_sampling_frequency: %lld \r\n", sampling_frequency);
			}
			else
			{
				long long sampling_frequency = get_frequency(token);
				iio_channel_attr_write_longlong(rx_dev_ch0, "sampling_frequency", sampling_frequency);
				printf("rx_sampling_frequency: %lld \r\n", sampling_frequency);
			}
		}
		else if( strcmp(token, "tx_freq")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				long long freq;
				iio_channel_attr_read_longlong(tx_alt_dev_ch1, tx_freq_name, &freq);
				printf("tx frequency: %lld \r\n", freq);
			}
			else
			{
				long long freq = get_frequency(token);
				iio_channel_attr_write_longlong(tx_alt_dev_ch1, tx_freq_name, freq);
				printf("tx frequency: %lld \r\n", freq);
			}
		}
		else if( strcmp(token, "rx_freq")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				long long freq;
				iio_channel_attr_read_longlong(tx_alt_dev_ch0, rx_freq_name, &freq);
				printf("rx frequency: %lld \r\n", freq);
			}
			else
			{
				long long freq = get_frequency(token);
				iio_channel_attr_write_longlong(tx_alt_dev_ch0, rx_freq_name, freq);
				printf("rx frequency: %lld \r\n", freq);
			}
		}
		else if( strcmp(token, "rx_port")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				char rx_port[100];
				iio_channel_attr_read(iio_device_find_channel(dev, "voltage0", false), "rf_port_select", rx_port, 100);
				printf("rx port: %s\r\n", rx_port);
			}
			else
			{
				char *rx_port = token;
				iio_channel_attr_write(iio_device_find_channel(dev, "voltage0", false),
			   		"rf_port_select", rx_port);
				printf("rx port : %s \r\n", rx_port);
			}
		}
		else if( strcmp(token, "tx_port")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				char tx_port[100];
				iio_channel_attr_read(iio_device_find_channel(dev, "voltage0", true), "rf_port_select", tx_port, 100);
				printf("tx port: %s \r\n", tx_port);
			}
			else
			{
				char *tx_port = token;
				iio_channel_attr_write(iio_device_find_channel(dev, "voltage0", true),
				   "rf_port_select", tx_port);
				printf("tx port : %s \r\n", tx_port);
			}
		}
		else if( strcmp(token, "sample_size")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				int sample_size = iio_device_get_sample_size(iio_dac);
				printf("sample size: %d \r\n", sample_size);
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
				printf("rx fir enable: %d \r\n", fir_en);
			}
			else
			{
				bool fir_en = atoi(token);
				iio_channel_attr_write_bool(rx_dev_ch0, "filter_fir_en", fir_en);
				printf("rx fir enable: %d \r\n", fir_en);
			}
		}
		else if( strcmp(token, "tx_fir_en")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				bool fir_en;
				iio_channel_attr_read_bool(tx_dev_ch0, "filter_fir_en", &fir_en);
				printf("tx fir enable: %d \r\n", fir_en);
			}
			else
			{
				bool fir_en = atoi(token);
				iio_channel_attr_write_bool(tx_dev_ch0, "filter_fir_en", fir_en);
				printf("tx fir enable: %d \r\n", fir_en);
			}
		}
		else if( strcmp(token, "span")==0 )
		{
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				printf("span: %d \r\n", span_number);
			}
			else
			{
				span_number = atoi(token);
				printf("span: %d \r\n", span_number);
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
			unsigned char uart_tx_buffer[2*FFT_LENGTH];
			fill_rx_buffer();
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
			//echo 2450000000 > /sys/bus/iio/devices/iio\:device0/out_altvoltage0_RX_LO_frequency
			int uart_size = 1024;
			if(fft_size*span_number < 1024)
			{
				uart_size = fft_size*span_number;
			}
			int window_size = (fft_size*span_number)/uart_size;
			int span_part = uart_size/span_number;
			double avg_fft_window;
			int32_t fft_abs32;
			long long freq;
			unsigned char uart_tx_buffer[4*FFT_LENGTH];

			//printf("Debug Flag #1\r\n");
			fill_rx_buffer();
			//printf("Debug Flag #2\r\n");
			calc_fft_dma(rx1_buffer, fft_abs, fft_phase, 0);
			//printf("Debug Flag #3\r\n");
			iio_channel_attr_read_longlong(tx_alt_dev_ch0, rx_freq_name, &freq); //rx_freq
			for(int s=0; s<span_number; s++ )
			{
				for(int i=0; i<span_part; i++ )
				{
					avg_fft_window = 0;
					for(int j=0; j<window_size; j++)
					{
						avg_fft_window += fft_abs[window_size*i+j];
					}
					avg_fft_window = avg_fft_window/window_size;
					fft_abs32 = floor(avg_fft_window);
					//printf( "%d: %d\r\n", i, fft_abs[i] );
					char first_byte = fft_abs32%256;
					char second_byte = fft_abs32/256;
					uart_tx_buffer[s*span_part+2*i] = first_byte;
					uart_tx_buffer[s*span_part+2*i+1] = second_byte;
					// uart_tx_buffer[4*i+2] = '\r';
					// uart_tx_buffer[4*i+3] = '\r\n';
					// printf("%c%c",first_byte, second_byte);
					//printf("%d : %c%c\r\n",i ,fft_out_abs[0], fft_out_abs[1]);
				}
				if( s == span_number-1)
				{
					break;
				}
				freq = freq + MAX_BW;
				iio_channel_attr_write_longlong(tx_alt_dev_ch0, rx_freq_name, freq); //rx_freq
				usleep(100);
			}
			freq = freq - (span_number-1)*MAX_BW;
			iio_channel_attr_write_longlong(tx_alt_dev_ch0, rx_freq_name, freq); //rx_freq
			usleep(100);
			fwrite(uart_tx_buffer, 1, 2*uart_size, stdout);
			printf("\r\n");
			//printf("Debug Flag #4\r\n");
		}
		else if(strcmp(token,"fft") == 0)
		{
			//echo 2450000000 > /sys/bus/iio/devices/iio\:device0/out_altvoltage0_RX_LO_frequency
			int uart_size = FFT_LENGTH;
			int window_size = fft_size/FFT_LENGTH;
			double avg_fft_window;
			int32_t fft_abs32;
			unsigned char uart_tx_buffer[4*FFT_LENGTH];

			//printf("Debug Flag #1\r\n");
			fill_rx_buffer();
			//printf("Debug Flag #2\r\n");
			calc_fft_dma(rx1_buffer, fft_abs, fft_phase, 0);
			//printf("Debug Flag #3\r\n");
			for( int i=0; i<uart_size; i++ )
			{
				avg_fft_window = 0;
				for(int j=0; j<window_size; j++)
				{
					avg_fft_window += fft_abs[window_size*i+j];
				}
				avg_fft_window = avg_fft_window/window_size;
				fft_abs32 = floor(avg_fft_window);
				char first_byte = fft_abs32%256;
				char second_byte = fft_abs32/256;
				uart_tx_buffer[2*i] = first_byte;
				uart_tx_buffer[2*i+1] = second_byte;
			}
			fwrite(uart_tx_buffer, 1, 2*uart_size, stdout);
			printf("\r\n");
		}
		else if(strcmp(token,"fft2") == 0)
		{
			//echo 2450000000 > /sys/bus/iio/devices/iio\:device0/out_altvoltage0_RX_LO_frequency
			int uart_size = FFT_LENGTH;
			int32_t fft_abs32;
			unsigned char uart_tx_buffer[4*FFT_LENGTH];

			//printf("Debug Flag #1\r\n");
			fill_rx_buffer();
			//printf("Debug Flag #2\r\n");
			calc_fft_dma(rx1_buffer, fft_abs, fft_phase, 0);
			//printf("Debug Flag #3\r\n");
			for( int i=0; i<uart_size; i++ )
			{
				fft_abs32 = fft_abs[i];
				char first_byte = fft_abs32%256;
				char second_byte = fft_abs32/256;
				uart_tx_buffer[2*i] = first_byte;
				uart_tx_buffer[2*i+1] = second_byte;
			}
			fwrite(uart_tx_buffer, 1, 2*uart_size, stdout);
			printf("\r\n");
		}
		else if (strcmp(token, "pulse")==0 )
		{
			int s_size = iio_device_get_sample_size(iio_dac);
			token = strtok(NULL, delim);
			int channel_num = atoi(token);
			token = strtok(NULL, delim);
			double period_num = atof(token);
			token = strtok(NULL, delim);
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
			int channel_num = atoi(token);
			token = strtok(NULL, delim);
			double period_num = atof(token);
			token = strtok(NULL, delim);
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
			int channel_num = atoi(token);
			token = strtok(NULL, delim);
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
			return 0;
		}
	}
	return 0;
}
