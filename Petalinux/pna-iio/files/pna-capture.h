#ifndef __PNA_CAPTURE_H__
#define __PNA_CAPTURE_H__

#include <time.h>
#include "pna-base.h"

#define SWEEP_SPAN 20 // 10 if rxfsampling = 30M | 20 if rxfsampling = 60M
#define SET_LO_DELAY 200

#define TRIG_POS_I   0
#define TRIG_NEG_I   1
#define TRIG_POS_Q  2
#define TRIG_NEG_Q  3

#define TRIG_HYS_BAND 60

typedef struct _fastlock_profile
{
	unsigned int index;
	long long frequency;
	char data[66];
} fastlock_profile;

int fft_changed(int fft_size);
void calc_fft_dma16(int32_t *bufferIn, int16_t *fft_abs, int16_t *fft_phase,
	                int is_debug, unsigned int fft_size);
void calc_fft_dma24(int32_t *bufferIn, int32_t *fft_abs, int32_t *fft_phase,
	                int is_debug, unsigned int fft_size);
void store_profile(int index, long long freq);
void fill_profiles(double start, double sweepspan);
void load_profile(int index);
void fill_output_buffer(int32_t *data_in, unsigned char *data_out, int data_size);
int compress_data(int32_t *data_in, unsigned char *data_out, int data_size);
int32_t* pna_fft(int32_t *data_in, int removed_span, unsigned int fft_size, bool window_en);
int32_t* pna_fft_dcfixed2(int32_t *rx_buffer, int fft_size, int index, bool window_en);
int32_t* pna_fft_dcfixed(int32_t *rx_buffer, long long start_freq, int fft_size, bool window_en);
int32_t* pna_ramp(long long lo_freq, int removed_span, int fft_size);
void pna_fft2(int32_t *data_in, unsigned int fft_size);
void pna_fft3(int32_t *data_in, unsigned int fft_size);
void flat_top_window(int32_t *data_in, unsigned int fft_size);
void calculate_flat_top_coeff(unsigned int fft_size);
void fill_output_buffer_iq(int32_t *data_in, unsigned char *data_out, unsigned int data_size);
void fill_output_buffer_2ch(int32_t *data_ch1, int32_t *data_ch2, unsigned char *data_out, unsigned int data_size);
int compress_data_iq(int32_t *data_in, unsigned char *data_out, unsigned int data_size);
void pna_adc(int32_t *data_in, unsigned int fft_size);
void pna_adc_iq(int32_t *data_in, unsigned int fft_size);
void pna_adc_iq2(int32_t *data_in, unsigned int fft_size);
void pna_adc_fft(int32_t *data_in, unsigned int fft_size);
int32_t* pna_trig_adc(int32_t *data_in, unsigned int fft_size, int mode, int level);
int pna_get_signal(char* awg_data, int samples);

void pna_print_avg();

#endif //__PNA_CAPTURE_H__
