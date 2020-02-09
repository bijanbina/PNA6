#include "pna-capture.h"

void calc_fft_dma24(int32_t *data_in, int32_t *fft_abs, int32_t *fft_phase,
	                int is_debug, unsigned int fft_size)
{
  uint8_t *bufferIn = (uint8_t *) malloc(sizeof(uint8_t) * FFT_24_BIT * 2 * fft_size);
	uint8_t *bufferOut = (uint8_t *) malloc(sizeof(uint8_t) * FFT_24_BIT * 2 * fft_size);

  memset(bufferOut, 0, sizeof(uint8_t) * FFT_24_BIT * 2 * fft_size);
  memset(bufferIn, 0, sizeof(uint8_t) * FFT_24_BIT * 2  * fft_size);
  // printf("fft24 flag1 %d\n",fft_size);
  for(int i=0; i<fft_size; i++)
  {
    int16_t dataIn_I = data_in[i] & 0x0000ffff;
    int16_t dataIn_Q = (data_in[i] & 0xffff0000) >> 16;
    bufferIn[6*i] = data_in[i] & 0x000000ff;
    bufferIn[6*i+1] = (data_in[i] & 0x0000ff00) >> 8;
    if(dataIn_I>=0)
    {
      bufferIn[6*i+2] = 0;
    }
    else
    {
      bufferIn[6*i+2] = 0xff;
    }

    bufferIn[6*i+3] = (data_in[i] & 0x00ff0000) >> 16;
    bufferIn[6*i+4] = (data_in[i] & 0xff000000) >> 24;
    if(dataIn_Q>=0)
    {
      bufferIn[6*i+5] = 0;
    }
    else
    {
      bufferIn[6*i+5] = 0xff;
    }
  }

	if(is_debug)
	{
		printf("calc_fft_dma #1 \r\n");
	}
	memCpy_DMA((char *)bufferIn, (char *)bufferOut, fft_size * FFT_24_BIT * 2);
	if(is_debug)
	{
		printf("calc_fft_dma #2 \r\n");
	}

	for (int i=0 ; i<fft_size; i++)
	{
    int8_t fft_output = bufferOut[6*i+2];
		int32_t fft_re = 0;
    fft_re |= bufferOut[6*i];
    fft_re |= bufferOut[6*i+1] << 8;
    fft_re |= bufferOut[6*i+2] << 16;
    if(fft_output < 0)
    {
      fft_re |= 0xff000000;
    }
    fft_output = bufferOut[6*i+5];
		int32_t fft_im = 0;
    fft_im = bufferOut[6*i+3];
    fft_im |= bufferOut[6*i+4] << 8;
    fft_im |= bufferOut[6*i+5] << 16;
    if(fft_output < 0)
    {
      fft_im |= 0xff000000;
    }
		double fft_re_double = fft_re;
		double fft_im_double = fft_im;
		double fft_mag = sqrt( pow(fft_re_double,2) + pow(fft_im_double,2) );
		//double fft_mag = sqrt( pow(fft_re,2) + pow(fft_im,2) );
		// if(is_debug)
		// {
		// 	printf("%4d : re16 %+6d - im16 %+6d - re %+6d - im %+6d - re_d %6.1lf - "
		// 			"im_d %6.1lf - mag %6.1lf - o %6d\n", i, fft_re_16, fft_im_16, fft_re, fft_im, fft_re_double, fft_im_double, fft_mag, fft_output[i]);
		// }
		fft_abs[i] = floor(fft_mag); //RE
		fft_phase[i] = 0;       //IM
		// if(i < 100)
		// {
		// 	fft_abs[i] = 100;
		// }

	}
	free(bufferOut);
}

void calc_fft_dma16(int32_t *bufferIn, int16_t *fft_abs, int16_t *fft_phase,
	                int is_debug, unsigned int fft_size)
{
	int i;
	int32_t *bufferOut = (int32_t *) malloc(sizeof(int32_t) * fft_size);

  memset(bufferOut, 0, sizeof(int32_t) * fft_size);

	if(is_debug)
	{
		printf("calc_fft_dma #1 \r\n");
	}
	memCpy_DMA((char *)bufferIn, (char *)bufferOut, fft_size * sizeof(int32_t));
	if(is_debug)
	{
		printf("calc_fft_dma #2 \r\n");
	}

	uint32_t *fft_output = (uint32_t *)bufferOut;

	for (i=0 ; i<fft_size; i++)
	{
		int16_t fft_re_16 = fft_output[i] & 0x0000ffff;
		int16_t fft_im_16 = (fft_output[i] & 0xffff0000) >> 16;
		int32_t fft_re = fft_re_16;
		int32_t fft_im = fft_im_16;
		double fft_re_double = fft_re;
		double fft_im_double = fft_im;
		double fft_mag = sqrt( pow(fft_re_double,2) + pow(fft_im_double,2) );
		//double fft_mag = sqrt( pow(fft_re,2) + pow(fft_im,2) );
		// if(is_debug)
		// {
		// 	printf("%4d : re16 %+6d - im16 %+6d - re %+6d - im %+6d - re_d %6.1lf - "
		// 			"im_d %6.1lf - mag %6.1lf - o %6d\n", i, fft_re_16, fft_im_16, fft_re, fft_im, fft_re_double, fft_im_double, fft_mag, fft_output[i]);
		// }
		fft_abs[i] = floor(fft_mag/sqrt(2.0)); //RE
		fft_phase[i] = fft_output[i] & 0x0000ffff; //IM
		// if(i < 100)
		// {
		// 	fft_abs[i] = 100;
		// }

	}
	free(bufferOut);
}

int compress_data(int32_t *data_in, unsigned char *data_out, int data_size)
{
	int output_size = UART_LENGTH;
	if(data_size < UART_LENGTH)
	{
		output_size = data_size;
	}
	int sample_cntr = 0;
	double window_size = (double)data_size/output_size;
	for(int i=0; i<output_size; i++)
    {
		double avg_fft_window = 0;
		int window_size_int = floor(window_size*(i+1) - sample_cntr);
		if(sample_cntr + window_size_int > data_size)
		{
			window_size_int = data_size - sample_cntr;
		}
		for(int j=0; j<window_size_int; j++)
		{
			avg_fft_window += data_in[sample_cntr+j];
		}
		sample_cntr += window_size_int;
		avg_fft_window = avg_fft_window/window_size_int;
		avg_fft_window = avg_fft_window/32768.0;
		if(avg_fft_window > 0)
		{
			avg_fft_window = 100*20*log10(avg_fft_window);
			int32_t fft_abs32 = floor(avg_fft_window);
			char first_byte = fft_abs32%256;
			char second_byte = fft_abs32/256;
			data_out[2*i] = first_byte;
			data_out[2*i+1] = second_byte;
		}
		else
		{
			int32_t fft_abs32 = -150*100;
			char first_byte = fft_abs32%256;
			char second_byte = fft_abs32/256;
			data_out[2*i] = first_byte;
			data_out[2*i+1] = second_byte;
		}
	}
	return output_size;
}

int32_t* pna_ramp(long long lo_freq, int removed_span, int fft_size)
{
	int lo_freq_mhz = lo_freq/1E6;
	// printf("pna_ramp-lo_freq_mhz: %d\r\n", lo_freq_mhz);
	double y;
	int32_t *fft_abs = (int32_t*) malloc(fft_size * sizeof(int32_t));
	int32_t *output_data = (int32_t*) malloc((fft_size-2*removed_span) * sizeof(int32_t));
	for(int i=0; i<fft_size; i++)
	{
		y = lo_freq_mhz - 15 + (60.0*i/fft_size)  - 920;
		y = y/40;
		double x = pow(10,y);
		fft_abs[i] = floor(x);
	}
	for(int i=0; i<fft_size-2*removed_span; i++)
	{
		output_data[i] = fft_abs[i+removed_span];
	}
	return output_data;
}

// calculate 40MHz spectrum without DC interference
int32_t* pna_fft_dcfixed(int32_t *rx_buffer, long long start_freq, int fft_size)
{
	long long freq = start_freq + 3*1E6*SWEEP_SPAN/4;
	int sweep_offset = 0;
	// char *sz = NULL;
	// long long cal_reg_val = 0;
	// char buffer_cal[80];
	int CHUNK_C = fft_size/6; // 1/6 = SWEEP_SPAN/2/rx_sampling_frequency_mhz
	set_lo_freq(__RX, freq);

	// write_reg_ad9361(CONTROL_OUTPUT_ADD, "00");
	// read_reg_ad9361(CONTROL_OUTPUT_VAL, buffer_cal);
	// printf("reg cal: %s\r\n", buffer_cal);
	// cal_reg_val = strtoll(&buffer_cal[2], &sz, 16);

	// while(cal_reg_val == 2)
	// {
		usleep(SET_LO_DELAY);
	// 	write_reg_ad9361(CONTROL_OUTPUT_ADD, "00");
	// 	read_reg_ad9361(CONTROL_OUTPUT_VAL, buffer_cal);
	// 	printf("reg cal: %s\r\n", buffer_cal);
	// 	cal_reg_val = strtoll(&buffer_cal[2], &sz, 16);
	// }

	int removed_span = fft_size/4;// 1/4 = (3*SWEEP_SPAN/4)/rx_sampling_frequency_mhz
	int32_t *spectrum = pna_fft(rx_buffer, removed_span, fft_size);
	if(spectrum == NULL)
		return NULL;
	int32_t *output_data = (int32_t*) malloc(CHUNK_C * 4 * sizeof(int32_t));
	for(int j=0; j<CHUNK_C; j++)
	{
		output_data[j] = spectrum[j];
		output_data[j+2*CHUNK_C] = spectrum[j+2*CHUNK_C];
		sweep_offset++;
	}
	free(spectrum);
	freq = freq + SWEEP_SPAN/2*1E6;
	set_lo_freq(__RX, freq);
	usleep(SET_LO_DELAY);
	spectrum = pna_fft(rx_buffer, removed_span, fft_size);
	if(spectrum == NULL)
	{
		free(output_data);
		return NULL;
	}
	for(int j=0; j<CHUNK_C; j++)
	{
		output_data[sweep_offset+j] = spectrum[j];
		output_data[sweep_offset+j+2*CHUNK_C] = spectrum[j+2*CHUNK_C];
	}
	free(spectrum);
	return output_data;
}

int32_t* pna_fft(int32_t *data_in, int removed_span, unsigned int fft_size)
{
#ifdef FFT_24_BIT
	int32_t *fft_abs = (int32_t *) malloc(sizeof(int32_t) * fft_size);
	int32_t *fft_phase = (int32_t *) malloc(sizeof(int32_t) * fft_size);
	int32_t *fft_spanned = (int32_t *) malloc(sizeof(int32_t) * (fft_size-2*removed_span));
#elif FFT_16_BIT
	int16_t *fft_abs = (int16_t *) malloc(sizeof(int16_t) * fft_size);
	int16_t *fft_phase = (int16_t *) malloc(sizeof(int16_t) * fft_size);
	int16_t *fft_spanned = (int16_t *) malloc(sizeof(int16_t) * (fft_size-2*removed_span));
#endif
	for(int i=0; i<5; i++)
	{
		fill_rx_buffer(fft_size);
	}
#ifdef FFT_24_BIT
  	calc_fft_dma24(data_in, fft_abs, fft_phase, 0, fft_size);
#elif FFT_16_BIT
  	calc_fft_dma16(data_in, fft_abs, fft_phase, 0, fft_size);
#endif
    for(int i=0; i<fft_size/2-removed_span; i++)
    {
		if(i<0 || i>fft_size/2)
		{
			printf("span bug!\r\n");
			free(fft_abs);
			free(fft_phase);
			free(fft_spanned);
			return NULL;
		}
		fft_spanned[i] = fft_abs[fft_size/2 + removed_span + i];
		fft_spanned[i+fft_size/2-removed_span] = fft_abs[i];
    }
	free(fft_abs);
	free(fft_phase);
	return fft_spanned;
}

void pna_fft2(int32_t *data_in, unsigned int fft_size)
{
  int uart_size = UART_LENGTH;
  unsigned char uart_tx_buffer[4*UART_LENGTH];
#ifdef FFT_24_BIT
  int32_t *fft_abs = (int32_t *) malloc(sizeof(uint32_t) * fft_size);
  int32_t *fft_phase = (int32_t *) malloc(sizeof(uint32_t) * fft_size);
#elif FFT_16_BIT
  int16_t *fft_abs = (int16_t *) malloc(sizeof(uint16_t) * fft_size);
  int16_t *fft_phase = (int16_t *) malloc(sizeof(uint16_t) * fft_size);
#endif
  fill_rx_buffer(fft_size);
#ifdef FFT_24_BIT
  calc_fft_dma24(data_in, fft_abs, fft_phase, 0, fft_size);
#elif FFT_16_BIT
  calc_fft_dma16(data_in, fft_abs, fft_phase, 0, fft_size);
#endif
  for( int i=0; i<uart_size; i++ )
  {
    if(fft_abs[i] > 0)
    {
	  double fft_abs_double = (double)fft_abs[i]/32768.0;
      fft_abs_double = 100*20*log10(fft_abs_double);
      int32_t fft_abs32 = floor(fft_abs_double);
      char first_byte = fft_abs32%256;
      char second_byte = fft_abs32/256;
      uart_tx_buffer[2*i] = first_byte;
      uart_tx_buffer[2*i+1] = second_byte;
    }
    else
    {
      int32_t fft_abs32 = -150*100;
      char first_byte = fft_abs32%256;
      char second_byte = fft_abs32/256;
      uart_tx_buffer[2*i] = first_byte;
      uart_tx_buffer[2*i+1] = second_byte;
    }
  }
  fwrite(uart_tx_buffer, 1, 2*uart_size, stdout);
  printf("\r\n");
  free(fft_abs);
  free(fft_phase);
}

void pna_fft3(int32_t *data_in, unsigned int fft_size)
{
#ifdef FFT_24_BIT
  int32_t *fft_abs = (int32_t *) malloc(sizeof(uint32_t) * fft_size);
  int32_t *fft_phase = (int32_t *) malloc(sizeof(uint32_t) * fft_size);
#elif FFT_16_BIT
  int16_t *fft_abs = (int16_t *) malloc(sizeof(uint16_t) * fft_size);
  int16_t *fft_phase = (int16_t *) malloc(sizeof(uint16_t) * fft_size);
#endif
  fill_rx_buffer(fft_size);
#ifdef FFT_24_BIT
  calc_fft_dma24(data_in, fft_abs, fft_phase, 0, fft_size);
#elif FFT_16_BIT
  calc_fft_dma16(data_in, fft_abs, fft_phase, 0, fft_size);
#endif
  for( int i=0; i<20; i++ )
  {
    printf("fft3_%d=%d\n", i, fft_abs[i]);
  }
  printf("\r\n");
  free(fft_abs);
  free(fft_phase);
}

int compress_data_iq(int32_t *data_in, unsigned char *data_out, unsigned int data_size)
{
	int output_size = 1024;
	if(data_size < 1024)
	{
		output_size = data_size;
	}
	int window_size = data_size/output_size;
	double avg_adc_window_i;
	double avg_adc_window_q;
	int32_t rx_buffer_i;
	int32_t rx_buffer_q;
	int16_t rx_buffer_i_16;
	int16_t rx_buffer_q_16;
	for(int i=0; i<output_size; i++ )
	{
		avg_adc_window_i = 0;
		avg_adc_window_q = 0;
		for(int j=0; j<window_size; j++)
		{
			rx_buffer_i_16 = data_in[window_size*i+j] & 0x0000FFFF;
			rx_buffer_i = rx_buffer_i_16;
			rx_buffer_q_16 = (data_in[window_size*i+j] >> 16) & 0x0000FFFF;
			rx_buffer_q = rx_buffer_q_16;
			avg_adc_window_i += rx_buffer_i;
			avg_adc_window_q += rx_buffer_q;
		}
		avg_adc_window_i = avg_adc_window_i/window_size;
		rx_buffer_i = floor(avg_adc_window_i);
		avg_adc_window_q = avg_adc_window_q/window_size;
		rx_buffer_q = floor(avg_adc_window_q);
		rx_buffer_i = rx_buffer_i & 0x0000FFFF;
		rx_buffer_q = rx_buffer_q & 0x0000FFFF;
		char first_byte = rx_buffer_i%256;
		char second_byte = rx_buffer_i/256;
		char third_byte = rx_buffer_q%256;
		char fourth_byte = rx_buffer_q/256;
		data_out[4*i] = first_byte;
		data_out[4*i+1] = second_byte;
		data_out[4*i+2] = third_byte;
		data_out[4*i+3] = fourth_byte;
	}
	return output_size;
}

void pna_adc(int32_t *data_in, unsigned int fft_size)
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
			rx_buffer_i_16 = data_in[window_size*i+j] & 0x0000FFFF;
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

void pna_adc_iq2(int32_t *data_in, unsigned int fft_size)
{
	int uart_size = 1024;
	if(fft_size < 1024)
	{
		uart_size = fft_size;
	}
	int window_size = fft_size/uart_size;
	double avg_adc_window_i;
	double avg_adc_window_q;
	int32_t rx_buffer_i;
	int32_t rx_buffer_q;
	int16_t rx_buffer_i_16;
	int16_t rx_buffer_q_16;
	unsigned char uart_tx_buffer[4*UART_LENGTH];
	fill_rx_buffer(fft_size);
	for(int i=0; i<uart_size; i++ )
	{
		avg_adc_window_i = 0;
		avg_adc_window_q = 0;
		for(int j=0; j<window_size; j++)
		{
			rx_buffer_i_16 = data_in[window_size*i+j] & 0x0000FFFF;
			rx_buffer_i = rx_buffer_i_16;
			rx_buffer_q_16 = (data_in[window_size*i+j] >> 16) & 0x0000FFFF;
			rx_buffer_q = rx_buffer_q_16;
			avg_adc_window_i += rx_buffer_i;
			avg_adc_window_q += rx_buffer_q;
		}
		avg_adc_window_i = avg_adc_window_i/window_size;
		rx_buffer_i = floor(avg_adc_window_i);
		avg_adc_window_q = avg_adc_window_q/window_size;
		rx_buffer_q = floor(avg_adc_window_q);
		rx_buffer_i = rx_buffer_i & 0x0000FFFF;
		rx_buffer_q = rx_buffer_q & 0x0000FFFF;
		char first_byte = rx_buffer_i%256;
		char second_byte = rx_buffer_i/256;
		char third_byte = rx_buffer_q%256;
		char fourth_byte = rx_buffer_q/256;
		uart_tx_buffer[4*i] = first_byte;
		uart_tx_buffer[4*i+1] = second_byte;
		uart_tx_buffer[4*i+2] = third_byte;
		uart_tx_buffer[4*i+3] = fourth_byte;
		// printf("%c%c\r\n",first_byte, second_byte);
		// printf("%d : %d\r\n", i, rx_buffer_i);
	}
	fwrite(uart_tx_buffer, 1, 4*uart_size, stdout);
	printf("\r\n");
}

void pna_adc_iq(int32_t *data_in, unsigned int fft_size)
{
	int uart_size = 1024;
	if(fft_size < 1024)
	{
		uart_size = fft_size;
	}
	int32_t rx_buffer_i;
	int32_t rx_buffer_q;
	int16_t rx_buffer_i_16;
	int16_t rx_buffer_q_16;
	unsigned char uart_tx_buffer[4*UART_LENGTH];
	fill_rx_buffer(fft_size);
	for(int i=0; i<uart_size; i++ )
	{
		rx_buffer_i_16 = data_in[i] & 0x0000FFFF;
		rx_buffer_i = rx_buffer_i_16;
		rx_buffer_q = data_in[i] >> 16;
		rx_buffer_q_16 = data_in[i] & 0x0000FFFF;
		rx_buffer_q = rx_buffer_q_16;
		char first_byte = rx_buffer_i%256;
		char second_byte = rx_buffer_i/256;
		char third_byte = rx_buffer_q%256;
		char fourth_byte = rx_buffer_q/256;
		uart_tx_buffer[4*i] = first_byte;
		uart_tx_buffer[4*i+1] = second_byte;
		uart_tx_buffer[4*i+2] = third_byte;
		uart_tx_buffer[4*i+3] = fourth_byte;
		// printf("%c%c\r\n",first_byte, second_byte);
		// printf("%d : %d\r\n", i, rx_buffer_i);
	}
	fwrite(uart_tx_buffer, 1, 4*uart_size, stdout);
	printf("\r\n");
}

void pna_adc_fft(int32_t *data_in, unsigned int fft_size)
{
	int uart_size = 1024;
	if(fft_size < 1024)
	{
		uart_size = fft_size;
	}
	int32_t rx_buffer_i, rx_buffer_q;
	int16_t rx_buffer_i_16, rx_buffer_q_16;
	unsigned char uart_tx_buffer[4*UART_LENGTH];
	fill_rx_buffer(uart_size);
	for(int i=0; i<uart_size; i++ )
	{
		rx_buffer_i_16 = data_in[i] & 0x0000FFFF;
		rx_buffer_i = rx_buffer_i_16;
		// rx_buffer_i = rx_buffer_i & 0x0000FFFF;
		rx_buffer_q_16 = (data_in[i] & 0xFFFF0000)>>16;
		rx_buffer_q = rx_buffer_q_16;
		// rx_buffer_q = rx_buffer_q & 0xFFFF0000;
		char first_byte = rx_buffer_i%256;
		char second_byte = rx_buffer_i/256;
		char third_byte = rx_buffer_q%256;
		char fourth_byte = rx_buffer_q/256;
		uart_tx_buffer[4*i] = first_byte;
		uart_tx_buffer[4*i+1] = second_byte;
		uart_tx_buffer[4*i+2] = third_byte;
		uart_tx_buffer[4*i+3] = fourth_byte;
		// printf("%c%c\r\n",first_byte, second_byte);
		// printf("%d : %d\r\n", i, rx_buffer_i);
	}
	fwrite(uart_tx_buffer, 1, 4*uart_size, stdout);
	printf("\r\n");
}
