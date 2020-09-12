#include "pna-capture.h"

fastlock_profile *profile_list;

int sweep_count = 0;
int profile_slot = 1;
int CHUNK_C = 1024/6;

int32_t *output_data;
int32_t *fft_phase;
int32_t *fft_abs;
double *window_coef;
uint8_t fft_buffer_in[FFT_24_BIT * 2 * MAX_FFT_LENGTH];
uint8_t fft_buffer_out[FFT_24_BIT * 2 * MAX_FFT_LENGTH];

double avg_I = 0, avg_Q = 0; // set after windowing

int fft_changed(int fft_size)
{
	CHUNK_C = fft_size/6; // 1/6 = SWEEP_SPAN/2/rx_sampling_frequency_mhz
	free(output_data);
	free(fft_abs);
	free(fft_phase);
	output_data = (int32_t *) malloc(CHUNK_C * 4 * sizeof(int32_t));
	if(output_data == NULL)
	{
		printf("memory allocation failed in changed_fft function\r\n");
		return -1;
	}
	fft_abs = (int32_t *) malloc(fft_size * sizeof(int32_t));
	if(fft_abs == NULL)
	{
		printf("memory allocation failed in changed_fft function\r\n");
		free(output_data);
		return -1;
	}
	fft_phase = (int32_t *) malloc(fft_size * sizeof(int32_t));
	if(fft_phase == NULL)
	{
		printf("memory allocation failed in changed_fft function\r\n");
		free(output_data);
		free(fft_abs);
		return -1;
	}
	window_coef = (double *) malloc(fft_size * sizeof(double));
	if(window_coef == NULL)
	{
		printf("memory allocation failed in changed_fft function\r\n");
		free(output_data);
		free(fft_abs);
		free(fft_phase);
		return -1;
	}
	calculate_flat_top_coeff(fft_size);
	return 0;
}

void calc_fft_dma24(int32_t *data_in, int32_t *fft_abs, int32_t *fft_phase,
	                int is_debug, unsigned int fft_size)
{
//   memset(bufferOut, 0, sizeof(uint8_t) * FFT_24_BIT * 2 * fft_size);
//   memset(bufferIn, 0, sizeof(uint8_t) * FFT_24_BIT * 2  * fft_size);
  // pna_printf("fft24 flag1 %d\n",fft_size);
  	struct timeval  tv1, tv2;
	double sweep_time = 0;
	gettimeofday(&tv1, NULL);
	for(int i=0; i<fft_size; i++)
	{
		int16_t dataIn_I = data_in[i] & 0x0000ffff;
		int16_t dataIn_Q = (data_in[i] & 0xffff0000) >> 16;
		fft_buffer_in[6*i] = data_in[i] & 0x000000ff;
		fft_buffer_in[6*i+1] = (data_in[i] & 0x0000ff00) >> 8;
		if(dataIn_I>=0)
		{
			fft_buffer_in[6*i+2] = 0;
		}
		else
		{
			fft_buffer_in[6*i+2] = 0xff;
		}

		fft_buffer_in[6*i+3] = (data_in[i] & 0x00ff0000) >> 16;
		fft_buffer_in[6*i+4] = (data_in[i] & 0xff000000) >> 24;
		if(dataIn_Q>=0)
		{
			fft_buffer_in[6*i+5] = 0;
		}
		else
		{
			fft_buffer_in[6*i+5] = 0xff;
		}
	}
  	gettimeofday(&tv2, NULL);
	sweep_time = (double) (tv2.tv_usec - tv1.tv_usec) / 1000000;
	sweep_time += (double) (tv2.tv_sec - tv1.tv_sec);
	// pna_printf("fftst: %lf \r\n", sweep_time);

	if(is_debug)
	{
		pna_printf("calc_fft_dma #1 \r\n");
	}
	memCpy_DMA((char *)fft_buffer_in, (char *)fft_buffer_out, fft_size * FFT_24_BIT * 2);
	gettimeofday(&tv2, NULL);
	sweep_time = (double) (tv2.tv_usec - tv1.tv_usec) / 1000000;
	sweep_time += (double) (tv2.tv_sec - tv1.tv_sec);
	// pna_printf("fftst: %lf \r\n", sweep_time);
	if(is_debug)
	{
		pna_printf("calc_fft_dma #2 \r\n");
	}

	for (int i=0 ; i<fft_size; i++)
	{
		int8_t fft_output = fft_buffer_out[6*i+2];

		int32_t fft_re = 0;
		fft_re |= fft_buffer_out[6*i];
		fft_re |= fft_buffer_out[6*i+1] << 8;
		fft_re |= fft_buffer_out[6*i+2] << 16;
		if(fft_output < 0)
		{
			fft_re |= 0xff000000;
		}
		fft_output = fft_buffer_out[6*i+5];

		int32_t fft_im = 0;
		fft_im = fft_buffer_out[6*i+3];
		fft_im |= fft_buffer_out[6*i+4] << 8;
		fft_im |= fft_buffer_out[6*i+5] << 16;
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
		// 	pna_printf("%4d : re16 %+6d - im16 %+6d - re %+6d - im %+6d - re_d %6.1lf - "
		// 			"im_d %6.1lf - mag %6.1lf - o %6d\n", i, fft_re_16, fft_im_16, fft_re, fft_im, fft_re_double, fft_im_double, fft_mag, fft_output[i]);
		// }
		fft_abs[i] = floor(fft_mag); //RE
		fft_phase[i] = 0;       //IM
		// if(i < 100)
		// {
		// 	fft_abs[i] = 100;
		// }

	}
	gettimeofday(&tv2, NULL);
	sweep_time = (double) (tv2.tv_usec - tv1.tv_usec) / 1000000;
	sweep_time += (double) (tv2.tv_sec - tv1.tv_sec);
	// pna_printf("fftst: %lf \r\n", sweep_time);
}

void calc_fft_dma16(int32_t *bufferIn, int16_t *fft_abs, int16_t *fft_phase,
	                int is_debug, unsigned int fft_size)
{
	int32_t *bufferOut = (int32_t *) malloc(sizeof(int32_t) * fft_size);
	if(bufferOut == NULL)
	{
		printf("memory allocation failed in calc_fft_dma16 function\r\n");
		return;
	}

  	memset(bufferOut, 0, sizeof(int32_t) * fft_size);

	if(is_debug)
	{
		pna_printf("calc_fft_dma #1 \r\n");
	}
	memCpy_DMA((char *)bufferIn, (char *)bufferOut, fft_size * sizeof(int32_t));
	if(is_debug)
	{
		pna_printf("calc_fft_dma #2 \r\n");
	}

	uint32_t *fft_output = (uint32_t *)bufferOut;

	for (int i=0 ; i<fft_size; i++)
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
		// 	pna_printf("%4d : re16 %+6d - im16 %+6d - re %+6d - im %+6d - re_d %6.1lf - "
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

void store_profile(int index, long long freq)
{
	unsigned char prev_alc = 0;
	unsigned char alc;
	char *last_byte;
	ssize_t ret;

	ret = fastlock_store();
	if(ret != 0)
	{
		pna_printf("error on fastlock store\r\n");
	}
	ret = fastlock_read_cal(profile_list[index].data);
	if(ret < 0)
	{
		pna_printf("error on fastlock save\r\n");
	}
	profile_list[index].frequency = freq;
	profile_list[index].index = index;

	/* Make sure two consecutive profiles do not have the same ALC.
		* Disregard the LBS of the ALC when comparing.
		More on: https://ez.analog.com/message/151702#151702 */
	last_byte = strrchr(profile_list[index].data, ',');
	if(!last_byte)
	{
		pna_printf("< , > not found\r\n");
	}
	last_byte++;
	alc = atoi(last_byte);
	if (abs(alc - prev_alc) < 2)
		alc += 2;
	prev_alc = alc;
	sprintf(last_byte, "%d", alc);
	// pna_printf("cal-%d: %s\r\n", index, profile_list[index].data);
}

// start_freq & sweepspan are in mhz scale
void fill_profiles(double start_freq, double sweepspan)
{
	double step = 40.0, f;
	unsigned int i = 0;
	long long freq;
	char *sz = NULL;
	long long cal_reg_val = 0;
	char buffer_cal[80];

	start_freq = start_freq + 15.0;

	if(profile_list)
	{
		free(profile_list);
	}

	sweep_count = sweepspan / step;
	if((int)sweepspan % (int)step > 0)
	{
		sweep_count++;
	}

	profile_list = (fastlock_profile *)malloc(2*sweep_count*sizeof(fastlock_profile));
	if(!profile_list)
	{
		pna_printf("error on profile_list memory allocation\r\n");
		return;
	}

	for (i = 0, f = start_freq; i < sweep_count; f += step, i++)
	{
		freq = f*1E6;
		set_lo_freq(__RX, freq);
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
		store_profile(2*i, freq);

		freq = (f+10)*1E6;
		set_lo_freq(__RX, freq);
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
		store_profile(2*i+1, freq);
	}
}

void load_profile(int index)
{
	ssize_t ret;
	if(!profile_list)
	{
		pna_printf("no profile list found\r\n");
	}

	profile_list[index].data[0] = '0' + profile_slot;
	// pna_printf("%d:freq=%lld profile_data %s\r\n", profile->index, profile->frequency, profile->data);
	ret = fastlock_load(profile_list[index].data);
	if (ret < 0)
	{
		pna_printf("error on fastlock load\r\n");
	}

	ret = fastlock_recall(profile_slot);
	if (ret < 0)
	{
		pna_printf("error on fastlock recall\r\n");
	}
	profile_slot = ( profile_slot+1 ) % 2;
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

void fill_output_buffer(int32_t *data_in, unsigned char *data_out, int data_size)
{
	char first_byte, second_byte;
	double fft_double;
	int32_t fft_abs32;
	for(int i=0; i<data_size; i++)
    {
		fft_double = data_in[i]/32768.0;
		fft_double = 100*20*log10(fft_double);
		fft_abs32 = floor(fft_double);
		first_byte = fft_abs32%256;
		second_byte = fft_abs32/256;
		data_out[2*i] = first_byte;
		data_out[2*i+1] = second_byte;
	}
}

int32_t* pna_ramp(long long lo_freq, int removed_span, int fft_size)
{
	int lo_freq_mhz = lo_freq/1E6;
	// pna_printf("pna_ramp-lo_freq_mhz: %d\r\n", lo_freq_mhz);
	double y;
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
int32_t* pna_fft_dcfixed2(int32_t *rx_buffer, int fft_size, int index)
{
	int sweep_offset = 0;
	int removed_span = fft_size/4;// 1/4 = (3*SWEEP_SPAN/4)/rx_sampling_frequency_mhz

	load_profile(2*index);
	// usleep(SET_LO_DELAY);

	int32_t *spectrum = pna_fft(rx_buffer, removed_span, fft_size);
	if(spectrum == NULL)
	{
		return NULL;
	}

	for(int j=0; j<CHUNK_C; j++)
	{
		output_data[j] = spectrum[j];
		output_data[j+2*CHUNK_C] = spectrum[j+2*CHUNK_C];
		sweep_offset++;
	}
	free(spectrum);

	load_profile(2*index+1);
	// usleep(SET_LO_DELAY);
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

// calculate 40MHz spectrum without DC interference
int32_t* pna_fft_dcfixed(int32_t *rx_buffer, long long start_freq, int fft_size)
{
	long long freq = start_freq + 3*1E6*SWEEP_SPAN/4;
	int sweep_offset = 0;
	char *sz = NULL;
	long long cal_reg_val = 0;
	char buffer_cal[80];
//	struct timeval  tv1, tv2;

//	gettimeofday(&tv1, NULL);
	set_lo_freq(__RX, freq);
//	gettimeofday(&tv2, NULL);
//	pna_printf("time: %lf\r\n", (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
//	         (double) (tv2.tv_sec - tv1.tv_sec));

//	gettimeofday(&tv1, NULL);
	usleep(SET_LO_DELAY);
//	gettimeofday(&tv2, NULL);
//	pna_printf("usleep time : %lf\r\n", (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
//	         (double) (tv2.tv_sec - tv1.tv_sec));

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

	int removed_span = fft_size/4;// 1/4 = (3*SWEEP_SPAN/4)/rx_sampling_frequency_mhz
	int32_t *spectrum = pna_fft(rx_buffer, removed_span, fft_size);
	if(spectrum == NULL)
		return NULL;
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
	int32_t *fft_spanned = (int32_t *) malloc(sizeof(int32_t) * (fft_size-2*removed_span));
#elif FFT_16_BIT
	int16_t *fft_spanned = (int16_t *) malloc(sizeof(int16_t) * (fft_size-2*removed_span));
#endif
	if(fft_spanned == NULL)
	{
		printf("memory allocation failed in pna_fft function\r\n");
		return NULL;
	}
	struct timeval  tv1, tv2;
	double sweep_time = 0;
	gettimeofday(&tv1, NULL);
	for(int i=0; i<5; i++)
	{
#ifdef ETTUS_E310
		fill_rx_buffer_single(fft_size);
#else
		fill_rx_buffer(fft_size);
#endif
		gettimeofday(&tv2, NULL);
		sweep_time = (double) (tv2.tv_usec - tv1.tv_usec) / 1000000;
		sweep_time += (double) (tv2.tv_sec - tv1.tv_sec);
		// pna_printf("st: %lf \r\n", sweep_time);
	}
//	flat_top_window(data_in, fft_size);
#ifdef FFT_24_BIT
  	calc_fft_dma24(data_in, fft_abs, fft_phase, 0, fft_size);
#elif FFT_16_BIT
  	calc_fft_dma16(data_in, fft_abs, fft_phase, 0, fft_size);
#endif
  	gettimeofday(&tv2, NULL);
	sweep_time = (double) (tv2.tv_usec - tv1.tv_usec) / 1000000;
	sweep_time += (double) (tv2.tv_sec - tv1.tv_sec);
	// pna_printf("st: %lf \r\n", sweep_time);
    for(int i=0; i<fft_size/2-removed_span; i++)
    {
		if(i<0 || i>fft_size/2)
		{
			pna_printf("span bug!\r\n");
			free(fft_abs);
			free(fft_phase);
			free(fft_spanned);
			return NULL;
		}
		fft_spanned[i] = 1024*fft_abs[fft_size/2 + removed_span + i]/fft_size;
		fft_spanned[i+fft_size/2-removed_span] = 1024*fft_abs[i]/fft_size;
    }
	return fft_spanned;
}

void pna_fft2(int32_t *data_in, unsigned int fft_size)
{
  int uart_size = UART_LENGTH;
  unsigned char uart_tx_buffer[4*UART_LENGTH];
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
  pna_printf("PLOT");
  pna_write(uart_tx_buffer, 2*uart_size);
  pna_printf("\r\n");
}

void pna_fft3(int32_t *data_in, unsigned int fft_size)
{
	int uart_size = UART_LENGTH;
	unsigned char uart_tx_buffer[4*UART_LENGTH];
	//printf("umadim inja\r\n");
	for (int j = 0; j < 1024; j++)
	{
		    int chert = j*5 - 20*512;
		    uint16_t ch16u = (uint16_t)chert;
		    uint8_t first_byte = ch16u%256;
		    uint8_t second_byte = ch16u/256;
			uart_tx_buffer[2*j] = first_byte;
			uart_tx_buffer[2*j+1] = second_byte;
	}
//	for(int j=0; j<8; j++)
//	{
//		for(int i=0; i<256; i++)
//		{
//			uart_tx_buffer[256*j+i] = i;
//		}
//	}
	pna_printf("PLOT");
	pna_write(uart_tx_buffer, 2*uart_size);
	pna_printf("\r\n");
}

void flat_top_window(int32_t *data, unsigned int fft_size)
{
	double winOut_I, winOut_Q;
	int16_t dataIn_I, dataIn_Q;
	int16_t dataOut_I, dataOut_Q;
	int16_t avgLast_I = (int16_t)floor(avg_I);
	int16_t avgLast_Q = (int16_t)floor(avg_Q);

	avg_I = 0;
	avg_Q = 0;

	for(int i=0; i < fft_size; i++)
	{
		dataIn_I = data[i] & 0x0000ffff;
		dataIn_Q = (data[i] & 0xffff0000) >> 16;
		avg_I += dataIn_I;
		avg_Q += dataIn_Q;
		dataIn_I -= avgLast_I;
		dataIn_Q -= avgLast_Q;
		winOut_I = dataIn_I * window_coef[i];
		winOut_Q = dataIn_Q * window_coef[i];
//		winOut_I = 2048 * window_coef[i];
//		winOut_Q = 2048 * window_coef[i];
		dataOut_I = (int16_t) floor(winOut_I);
		dataOut_Q = (int16_t) floor(winOut_Q);
//		dataOut_I -= avgLast_I;
//		dataOut_Q -= avgLast_Q;
		data[i] = dataOut_I;
		data[i] &= 0x0000FFFF;
		data[i] |= (dataOut_Q << 16);
	}
	avg_I = avg_I / fft_size;
	avg_Q = avg_Q / fft_size;
}

void pna_print_avg()
{
	pna_printf("I: %lf , Q: %lf \r\n>>", avg_I, avg_Q);
}

void calculate_flat_top_coeff(unsigned int fft_size)
{
//	int max_size = 4096;

//	const double a0 = 0.21557895 * max_size;
//	const double a1 = 0.41663158 * max_size;
//	const double a2 = 0.27726315 * max_size;
//	const double a3 = 0.08357894 * max_size;
//	const double a4 = 0.00694736 * max_size;
	double a0 = 0.21557895;
	double a1 = 0.41663158;
	double a2 = 0.27726315;
	double a3 = 0.08357894;
	double a4 = 0.00694736;


	for(int i=0; i < fft_size; i++)
	{
		window_coef[i] = a0 - a1*cos(2.0*PI*i/fft_size) + a2*cos(4.0*PI*i/fft_size) - a3*cos(6.0*PI*i/fft_size) + a4*cos(8.0*PI*i/fft_size);
	}
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
	uint32_t rx_buffer_iu;
	uint32_t rx_buffer_qu;
	int16_t rx_buffer_i_16;
	int16_t rx_buffer_q_16;
	for(int i=0; i<output_size; i++)
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
		rx_buffer_iu = (uint32_t)rx_buffer_i;
		rx_buffer_qu = (uint32_t)rx_buffer_q;
//		uint16_t iu= (uint16_t)i;
		uint8_t first_byte = rx_buffer_iu%256;
		uint8_t second_byte = rx_buffer_iu/256;
		uint8_t third_byte = rx_buffer_qu%256;
		uint8_t fourth_byte = rx_buffer_qu/256;
		data_out[4*i] = first_byte;
		data_out[4*i+1] = second_byte;
		data_out[4*i+2] = third_byte;
		data_out[4*i+3] = fourth_byte;
	}
	return output_size;
}

void fill_output_buffer_iq(int32_t *data_in, unsigned char *data_out, unsigned int data_size)
{
	int32_t rx_buffer_i;
	int32_t rx_buffer_q;
	uint32_t rx_buffer_iu;
	uint32_t rx_buffer_qu;
	int16_t rx_buffer_i_16;
	int16_t rx_buffer_q_16;
	char first_byte, second_byte, third_byte, fourth_byte;
	for(int i=0; i<data_size; i++)
	{
		rx_buffer_i_16 = data_in[i] & 0x0000FFFF;
		rx_buffer_i = rx_buffer_i_16;
		rx_buffer_q_16 = (data_in[i] >> 16) & 0x0000FFFF;
		rx_buffer_q = rx_buffer_q_16;
		rx_buffer_i = rx_buffer_i & 0x0000FFFF;
		rx_buffer_iu = (uint32_t)rx_buffer_i;
		rx_buffer_q = rx_buffer_q & 0x0000FFFF;
		rx_buffer_qu = (uint32_t)rx_buffer_q;
		first_byte = rx_buffer_iu%256;
		second_byte = rx_buffer_iu/256;
		third_byte = rx_buffer_qu%256;
		fourth_byte = rx_buffer_qu/256;
		data_out[4*i] = first_byte;
		data_out[4*i+1] = second_byte;
		data_out[4*i+2] = third_byte;
		data_out[4*i+3] = fourth_byte;
	}
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
		// pna_printf("%c%c\r\n",first_byte, second_byte);
		// pna_printf("%d : %d\r\n", i, rx_buffer_i);
	}
	pna_printf("PLOT");
	pna_write(uart_tx_buffer, 2*uart_size);
	pna_printf("\r\n");
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
		// pna_printf("%c%c\r\n",first_byte, second_byte);
		// pna_printf("%d : %d\r\n", i, rx_buffer_i);
	}
	pna_printf("PLOT");
	pna_write(uart_tx_buffer, 4*uart_size);
	pna_printf("\r\n");
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
		// pna_printf("%c%c\r\n",first_byte, second_byte);
		// pna_printf("%d : %d\r\n", i, rx_buffer_i);
	}
	pna_printf("PLOT");
	pna_write(uart_tx_buffer, 4*uart_size);
	pna_printf("\r\n");
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
		// pna_printf("%c%c\r\n",first_byte, second_byte);
		// pna_printf("%d : %d\r\n", i, rx_buffer_i);
	}
	pna_printf("PLOT");
	pna_write(uart_tx_buffer, 4*uart_size);
	pna_printf("\r\n");
}

//Chizama dahanet
int pna_get_signal(char* awg_data, int samples)
{
	unsigned char received_char = 0;
	unsigned int  char_number = 0;

//	size_t number = pna_read((unsigned char *) awg_data, samples + 2);
//	pna_printf("\nread: %d, %d\n", number, samples +2);

	fflush(stdin);
	while(char_number<samples+1)
	{
		fread(&received_char, sizeof(unsigned char), 1, stdin);
		printf("%d,", received_char);
//		fflush(stdout);
//		pna_read(&received_char, 1);
//		pna_printf("sl0");
		awg_data[char_number++] = received_char;
//		if(received_char=='\0')
//		{
//			pna_printf("fl0");
//		}
	}

//	pna_read((unsigned char*)&received_char, 1);
//	if(received_char != '<')
//		return -1;
//	pna_read((unsigned char*)&received_char, 1);
//	if(received_char != '>')
//		return -1;
//	pna_printf("flag1");
//	pna_read((unsigned char*)&received_char, 1);
//	if(received_char != '\n')
//		return -1;
//	pna_printf("flag2");

	return char_number;
}

