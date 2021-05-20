#ifndef __PNA_SIGGEN_H__
#define __PNA_SIGGEN_H__

#include "pna-base.h"

#define AWG_NUM_WIDTH 4

void send_dc_signal(int dds_sample_size, int amp, int channel_num);
void send_sinc_signal(int dds_sample_size, int amp, double dds_freq, int channel_num);
void send_pulse_signal(int dds_sample_size, int amp, int period_num, double duty_cycle, int sig_iq, int channel_num);
void send_sin_signal(int dds_sample_size, int amp, int period_num, int sig_iq,  int channel_num);
void send_triangle_signal(int dds_sample_size, int amp, int period_num, int channel_num);
void send_awg_signal(int dds_sample_size, double dig_amp, int channel_num);

#endif /* __PNA_SIGGEN_H__ */
