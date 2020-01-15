#ifndef __PNA_FFT_H__
#define __PNA_FFT_H__

#include "pna-base.h"

void calc_fft_dma16(int32_t *bufferIn, int16_t *fft_abs, int16_t *fft_phase,
	                int is_debug, unsigned int fft_size);
void calc_fft_dma24(int32_t *bufferIn, int32_t *fft_abs, int32_t *fft_phase,
	                int is_debug, unsigned int fft_size);
void pna_fft_span(int32_t *data_in, long long span, unsigned int fft_size);
void pna_fft(int32_t *data_in, unsigned int fft_size);
void pna_fft2(int32_t *data_in, unsigned int fft_size);
void pna_fft3(int32_t *data_in, unsigned int fft_size);
void pna_adc(int32_t *data_in, unsigned int fft_size);
void pna_adc_iq(int32_t *data_in, unsigned int fft_size);
void pna_adc_iq2(int32_t *data_in, unsigned int fft_size);
void pna_adc_fft(int32_t *data_in, unsigned int fft_size);

#endif //__PNA_FFT_H__