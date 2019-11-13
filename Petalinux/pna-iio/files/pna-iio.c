#include "base-iio.h"

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

int8_t dac_buf[16384];

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
			int i;
			int window_size = FFT_LENGTH/1024.0;
			double avg_adc_window;
			int32_t rx_buffer_i;
			int16_t rx_buffer_i_16;
			unsigned char uart_tx_buffer[2*FFT_LENGTH];
			fill_rx_buffer();
			for( i=0; i<1024; i++ )
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
			fwrite(uart_tx_buffer, 1, 2*1024, stdout);
			printf("\r\n");
		}
		else if(strcmp(token,"fft") == 0)
		{
			//echo 2450000000 > /sys/bus/iio/devices/iio\:device0/out_altvoltage0_RX_LO_frequency
			int i;
			int window_size = FFT_LENGTH/1024.0;
			double avg_fft_window;
			int32_t fft_abs32;
			unsigned char uart_tx_buffer[4*FFT_LENGTH];

			//printf("Debug Flag #1\r\n");
			fill_rx_buffer();
			//printf("Debug Flag #2\r\n");
			calc_fft_dma(rx1_buffer, fft_abs, fft_phase, 0);
			//printf("Debug Flag #3\r\n");
			for( i=0; i<1024; i++ )
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
				uart_tx_buffer[2*i] = first_byte;
				uart_tx_buffer[2*i+1] = second_byte;
				// uart_tx_buffer[4*i+2] = '\r';
				// uart_tx_buffer[4*i+3] = '\r\n';
				// printf("%c%c",first_byte, second_byte);
				//printf("%d : %c%c\r\n",i ,fft_out_abs[0], fft_out_abs[1]);
			}
			fwrite(uart_tx_buffer, 1, 2*1024, stdout);
			printf("\r\n");
			//printf("Debug Flag #4\r\n");
		}
		else if(strcmp(token,"fft_debug") == 0)
		{
			int i; unsigned char uart_tx_buffer[4*FFT_LENGTH];
			fill_rx_buffer();calc_fft_dma(rx1_buffer, fft_abs, fft_phase, 1);
			for( i=0; i<fft_size; i++ )
			{
				char* fft_out_abs = (char *) &fft_abs[i];
				char first_byte = fft_abs[i]%256;
				char second_byte = fft_abs[i]/256;
				uart_tx_buffer[2*i] = first_byte;
				uart_tx_buffer[2*i+1] = second_byte;
			}
			fwrite(uart_tx_buffer, 1, 2*fft_size, stdout);printf("\r\n");
		}
		else if(strcmp(token,"fft2") == 0)
		{
			int i;
			unsigned char uart_tx_buffer[4*FFT_LENGTH];

			fill_rx_buffer();
			calc_fft_dma(rx1_buffer, fft_abs, fft_phase, 0);
			for( i=0; i<fft_size; i++ )
			{
				char first_byte = fft_phase[i]%256;
				char second_byte = fft_phase[i]/256;
				uart_tx_buffer[2*i] = first_byte;
				uart_tx_buffer[2*i+1] = second_byte;
			}
			fwrite(uart_tx_buffer, 1, 2*fft_size, stdout);
			printf("\r\n");
			//printf("Debug Flag #4\r\n");
		}
		else if (strcmp(token, "pulse")==0 )
		{
			int s_size = iio_device_get_sample_size(iio_dac);
			token = strtok(NULL, delim);
			int channel_num = atoi(token);
			token = strtok(NULL, delim);
			double period_num = atof(token);
			token = strtok(NULL, delim);
			double size_div = atof(token);
			token = strtok(NULL, delim);
			double amplitude = atof(token);
			int amplitude_int = amplitude*2047*16;
			int sample_count = FFT_LENGTH/size_div;
			int period_sample_count = sample_count/period_num;

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

			if (dds_buffer)
			{
				iio_buffer_destroy ( dds_buffer );
			}

			dds_buffer = iio_device_create_buffer(iio_dac, sample_count, true);

			if (!dds_buffer)
			{
				fprintf(stderr, "Unable to create buffer: %s\r\n", strerror(errno));
			}

			memcpy(iio_buffer_start(dds_buffer), dac_buf,
					iio_buffer_end(dds_buffer) - iio_buffer_start(dds_buffer));

			iio_buffer_push(dds_buffer);
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
			token = strtok(NULL, delim);
			double size_div = atof(token);
			int size = FFT_LENGTH/size_div;
			int sample_count = size;

			for (int i=0 ; i<sample_count ; i++)
			{
				double x = i-sample_count/2;
				double sinc;
				if ( x==0 )
				{
					sinc = 2047*16;
				}
				else
				{
					sinc = sin(x/dds_freq)/(x/dds_freq)*2047*16;
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
			printf("Sample Count= %d\r\n", sample_count);
			printf("Sample Size = %d\r\n", s_size );
			for ( int i=sample_count/2-5 ; i<sample_count/2+5 ; i++)
			{
				printf("DAC Buffer[%d]= %d ,%d , x= %d\r\n", i*s_size+channel_num*2+1, (uint8_t) dac_buf[i*s_size+channel_num*2+1], (uint8_t)dac_buf[i*s_size+channel_num*2],i-sample_count/2);
			}

			if (dds_buffer)
			{
				iio_buffer_destroy ( dds_buffer );
			}

			dds_buffer = iio_device_create_buffer(iio_dac, sample_count, true);

			if (!dds_buffer)
			{
				fprintf(stderr, "Unable to create buffer: %s\r\n", strerror(errno));
			}

			memcpy(iio_buffer_start(dds_buffer), dac_buf,
					iio_buffer_end(dds_buffer) - iio_buffer_start(dds_buffer));

			iio_buffer_push(dds_buffer);
		}
		else if (strcmp(token, "dma")==0 )
		{
			//pna-iio dma 100 1024
			token = strtok(NULL, delim);
			int sin_freq_in = atoi(token); //default 100
			token = strtok(NULL, delim);
			int data_size = atoi(token); //default: 1024
			token = strtok(NULL, delim);
			int16_t zohdi_arg = atoi(token);
			//double sin_freq = sin_freq_in/10.0;
			int32_t *bufferIn, *bufferOut_DMA;
			//packet size = 32
			//data length = 16384
			unsigned long DATA = 1*data_size; //16bit(I)+16bit(Q) = 32bit data

			bufferIn = (char *) malloc(sizeof(uint32_t) * DATA);
		    bufferOut_DMA = (char *) malloc(sizeof(uint32_t) * DATA);

			int i;

			for(i=0; i<DATA; i++)
			{

				double sin_in = i;
				double sin_out = sin(sin_in/sin_freq_in);
				if(i==1)
				{
					bufferIn[1] = zohdi_arg;
				}
				else
				{
					int32_t sin_scaled = 0;
					bufferIn[i] = sin_scaled;
				}
			}
			printf ("Debug Flag #6\r\n");
		    memset(bufferOut_DMA, 0, sizeof(int32_t) * DATA);
			memCpy_DMA((char *)bufferIn, (char *)bufferOut_DMA, DATA, sizeof(int32_t));
			printf ("Debug Flag #7\r\n");

			int *fft_output = (int *)bufferOut_DMA;
			int16_t fft_abs[8192];
			int16_t fft_phase[8192];

			for (i=0; i<DATA; i++)
			{
				fft_abs[i] = (fft_output[i] & 0xffff0000) >> 16;
				fft_phase[i] = fft_output[i] & 0x0000ffff;
			}

			for (i=0 ; i<DATA; i++)
			{
				if ( i%4==0 )
				{
					printf("O[%4d] = %4d<%4d "    , i  , fft_abs[i]  , fft_phase[i]);
					printf("O[%4d] = %4d<%4d "    , i+1, fft_abs[i+1], fft_phase[i+1]);
					printf("O[%4d] = %4d<%4d "    , i+2, fft_abs[i+2], fft_phase[i+2]);
					printf("O[%4d] = %4d<%4d \r\n", i+3, fft_abs[i+3], fft_phase[i+3]);
				}
			}
		}
		else if( strcmp(token, "exit")==0 )
		{
			return 0;
		}
	}
	return 0;
}
