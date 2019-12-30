#include "pna-fft.h"

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

void fft_span(int32_t *data_in, long long span, unsigned int fft_size)
{
  //echo 2450000000 > /sys/bus/iio/devices/iio\:device0/out_altvoltage0_RX_LO_frequency
  int uart_size = UART_LENGTH;
  if(fft_size*span/MAX_BW < UART_LENGTH)
  {
    uart_size = fft_size*span/MAX_BW;
  }
	// printf("%d\n", fft_size, span);
  // if(fft_size*span_number < 1024)
  // {
  // 	uart_size = fft_size*span_number;
  // }
  // int window_size = (fft_size*span_number)/uart_size;
  // int span_part = uart_size/span_number;
  double window_size = (double)span*fft_size/MAX_BW/uart_size;
  int removed_span = fft_size*(MAX_BW - span)/MAX_BW/2;
  int sample_cntr = 0;
  // long long freq;
  unsigned char uart_tx_buffer[4*UART_LENGTH];
#ifdef FFT_24_BIT
  int32_t *fft_abs = (int32_t *) malloc(sizeof(int32_t) * fft_size);
  int32_t *fft_phase = (int32_t *) malloc(sizeof(int32_t) * fft_size);
	int32_t *fft_spanned = (int32_t *) malloc(sizeof(int32_t) * (fft_size-2*removed_span));
#elif FFT_16_BIT
  int16_t *fft_abs = (int16_t *) malloc(sizeof(int16_t) * fft_size);
  int16_t *fft_phase = (int16_t *) malloc(sizeof(int16_t) * fft_size);
	int16_t *fft_spanned = (int16_t *) malloc(sizeof(int16_t) * (fft_size-2*removed_span));
#endif
  // printf("us %d - ws %f - rs %d \r\n", uart_size, window_size, removed_span);

  fill_rx_buffer(fft_size);
#ifdef FFT_24_BIT
  calc_fft_dma24(data_in, fft_abs, fft_phase, 0, fft_size);
#elif FFT_16_BIT
  calc_fft_dma16(data_in, fft_abs, fft_phase, 0, fft_size);
#endif
  // iio_channel_attr_read_longlong(tx_alt_dev_ch0, rx_freq_name, &freq); //rx_freq
  // for(int s=0; s<span_number; s++ )
  // {
    // for(int i=0; i<span_part; i++ )
    for(int i=0; i<fft_size/2-removed_span; i++)
    {
      fft_spanned[i] = fft_abs[fft_size/2 + removed_span + i];
      fft_spanned[i+fft_size/2-removed_span] = fft_abs[i];
    }
		// printf("%d\n", uart_size);
    for(int i=0; i<uart_size; i++)
    {
      double avg_fft_window = 0;
      int window_size_int = floor(window_size*(i+1) - sample_cntr);
      if(sample_cntr + window_size_int > fft_size)
      {
        window_size_int = fft_size - sample_cntr;
      }
      // printf("%d: wsi %4d - sc %4d \r\n", i, window_size_int, sample_cntr);
      for(int j=0; j<window_size_int; j++)
      {
        avg_fft_window += fft_spanned[sample_cntr+j];
      }
      sample_cntr += window_size_int;
      avg_fft_window = avg_fft_window/window_size_int;
			avg_fft_window = avg_fft_window/32768.0;
      if(avg_fft_window > 0)
      {
        avg_fft_window = 100*20*log10(avg_fft_window);
        int32_t fft_abs32 = floor(avg_fft_window);
        //printf( "%d: %d\r\n", i, fft_abs[i] );
        char first_byte = fft_abs32%256;
        char second_byte = fft_abs32/256;
        uart_tx_buffer[2*i] = first_byte;
        uart_tx_buffer[2*i+1] = second_byte;
				// if((i>500 && i<520) || (i<300 && i>280))
					// printf("fft_%d: %lf\r\n", i, avg_fft_window);
      }
      else
      {
				// printf("bib\r\n");
        int32_t fft_abs32 = -150*100;
        char first_byte = fft_abs32%256;
        char second_byte = fft_abs32/256;
        uart_tx_buffer[2*i] = first_byte;
        uart_tx_buffer[2*i+1] = second_byte;
      }

      // uart_tx_buffer[s*span_part+2*i] = first_byte;
      // uart_tx_buffer[s*span_part+2*i+1] = second_byte;
    }
    // if( s == span_number-1)
    // {
    // 	break;
    // }
    // freq = freq + MAX_BW;
    // iio_channel_attr_write_longlong(tx_alt_dev_ch0, rx_freq_name, freq); //rx_freq
    // usleep(100);
  // }
  // freq = freq - (span_number-1)*MAX_BW;
  // iio_channel_attr_write_longlong(tx_alt_dev_ch0, rx_freq_name, freq); //rx_freq
  // usleep(100);
  fwrite(uart_tx_buffer, 1, 2*uart_size, stdout);
  printf("\r\n");
  free(fft_abs);
  free(fft_phase);
	free(fft_spanned);
  //printf("Debug Flag #4\r\n");
}

void fft(int32_t *data_in, unsigned int fft_size)
{
  //echo 2450000000 > /sys/bus/iio/devices/iio\:device0/out_altvoltage0_RX_LO_frequency
  int uart_size = UART_LENGTH;
  if(fft_size < UART_LENGTH)
  {
    uart_size = fft_size;
  }
  int window_size = fft_size/UART_LENGTH;
  unsigned char uart_tx_buffer[4*UART_LENGTH];
#ifdef FFT_24_BIT
  int32_t *fft_abs = (int32_t *) malloc(sizeof(int32_t) * fft_size);
  int32_t *fft_phase = (int32_t *) malloc(sizeof(int32_t) * fft_size);
	int32_t *fft_swapped = (int32_t *) malloc(sizeof(int32_t) * fft_size);
#elif FFT_16_BIT
  int16_t *fft_abs = (int16_t *) malloc(sizeof(int16_t) * fft_size);
  int16_t *fft_phase = (int16_t *) malloc(sizeof(int16_t) * fft_size);
	int16_t *fft_swapped = (int16_t *) malloc(sizeof(int16_t) * fft_size);
#endif
  fill_rx_buffer(fft_size);
#ifdef FFT_24_BIT
  calc_fft_dma24(data_in, fft_abs, fft_phase, 0, fft_size);
#elif FFT_16_BIT
  calc_fft_dma16(data_in, fft_abs, fft_phase, 0, fft_size);
#endif
	for(int i=0; i<fft_size/2; i++)
	{
		fft_swapped[i] = fft_abs[fft_size/2 + i];
		fft_swapped[i+fft_size/2] = fft_abs[i];
	}
  for( int i=0; i<uart_size; i++ )
  {
    double avg_fft_window = 0;
    for(int j=0; j<window_size; j++)
    {
      avg_fft_window += fft_swapped[window_size*i+j];
    }
    avg_fft_window = avg_fft_window/window_size;
		avg_fft_window = avg_fft_window/32768.0;
    if(avg_fft_window > 0)
    {
      avg_fft_window = 100*20*log10(avg_fft_window);
      int32_t fft_abs32 = floor(avg_fft_window);
      char first_byte = fft_abs32%256;
      char second_byte = fft_abs32/256;
      uart_tx_buffer[2*i] = first_byte;
      uart_tx_buffer[2*i+1] = second_byte;
			// if(i>500 && i<520)
			// 	printf("fft_%d: %lf\n", i, avg_fft_window);
    }
    else
    {
			// printf("sag shashide\r\n");
      int32_t fft_abs32 = -150*100;
      char first_byte = fft_abs32%256;
      char second_byte = fft_abs32/256;
      uart_tx_buffer[2*i] = first_byte;
      uart_tx_buffer[2*i+1] = second_byte;
    }
  }
	// for( int i=0; i<20; i++ )
  // {
  //   printf("fft_%d: lo=%d hi=%d\n", i, uart_tx_buffer[2*i], uart_tx_buffer[2*i+1]);
  // }
  fwrite(uart_tx_buffer, 1, 2*uart_size, stdout);
  printf("\r\n");
  free(fft_abs);
  free(fft_phase);
	free(fft_swapped);
}

void fft2(int32_t *data_in, unsigned int fft_size)
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

void fft3(int32_t *data_in, unsigned int fft_size)
{
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
  for( int i=0; i<20; i++ )
  {
    printf("fft3_%d=%d\n", i, fft_abs[i]);
  }
  printf("\r\n");
  free(fft_abs);
  free(fft_phase);
}
