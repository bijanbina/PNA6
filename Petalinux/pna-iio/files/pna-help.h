#ifndef __PNA_HELP_H__
#define __PNA_HELP_H__

#include "pna-base.h"

#define ERROR_ARG 0
#define ERROR_CH 1

#define COMMAND_TXBW        "tx_bandwidth"
#define COMMAND_RXBW        "rx_bandwidth"
#define COMMAND_VGA         "vga_gain"
#define COMMAND_LNA         "lna_gain"
#define COMMAND_AGC         "agc"
#define COMMAND_TXRATE      "tx_sample_rate"
#define COMMAND_RXRATE      "rx_sample_rate"
#define COMMAND_TXSS        "tx_sample_size"
#define COMMAND_RXSS        "rx_sample_size"
#define COMMAND_TXFREQ      "tx_freq"
#define COMMAND_RXFREQ      "rx_freq"
#define COMMAND_TXFIR       "tx_fir_en"
#define COMMAND_RXFIR       "rx_fir_en"
#define COMMAND_RFOUT       "sig_rf_out"
#define COMMAND_WINENABLE   "flat_top"
#define COMMAND_SIGPOW      "sig_pow"
#define COMMAND_VNAFREQ     "vna_diff_freq"
#define COMMAND_VNAPOW      "vna_dc_power"
#define COMMAND_BBDC    "bbdc"
#define COMMAND_RFDC     "rfdc"
#define COMMAND_QUAD    "quad"
#define COMMAND_SVNA        "vna_setting"
#define COMMAND_SCAL        "cal_setting"
#define COMMAND_DUALCH      "is_dual"
#define COMMAND_SWEEPTIME   "sweep_time"
#define COMMAND_FILLPRO     "fillpro"
#define COMMAND_LOADPRO "loadpro"
#define COMMAND_FRESET "freset"
#define COMMAND_FIR_COEF "fir_coef"
#define COMMAND_NLF         "nlf"
#define COMMAND_REGISTER "reg"
#define COMMAND_EMIO "emio"
#define COMMAND_DBG "dbg"
#define COMMAND_EXIT "exit"
#define COMMAND_AVGWIN "avg_window"
#define COMMAND_TXPORT      "tx_port"
#define COMMAND_RXPORT      "rx_port"
#define COMMAND_SAMSZ "sample_size"
#define COMMAND_DACMAX "dac_max"
#define COMMAND_ACC "acc"
#define COMMAND_TEST "test"

#define WAVE_SINDC       "sin_dc"
#define WAVE_PULSEIQ     "pulse_iq"
#define WAVE_SINIQ       "sin_iq"
#define WAVE_AWG         "awg"
#define WAVE_PULSE  "pulse"
#define WAVE_DC     "dc"
#define WAVE_SIN    "sin"
#define WAVE_SINC   "sinc"
#define WAVE_TRIANGLE "triangle"

#define PLOT_ADC "adc"
#define PLOT_ADCIQ "adc_iq"
#define PLOT_ADCTRIG "adc_trig"
#define PLOT_ADCCAL "adc_cal"
#define PLOT_ADCFFT "adc_fft"
#define PLOT_ADCVNA "adc_vna"
#define PLOT_FFT "fft"
#define PLOT_FFT2 "fft2"
#define PLOT_FFT3 "fft3"
#define PLOT_FFT4 "fft4"
#define PLOT_FFTSPAN "fft_span"
#define PLOT_SWEEP "sweep"
#define PLOT_SWEEP2 "sweep2"

#define BW_MAX 56E6
#define BW_MIN  2E5
#define VGA_MAX 0
#define VGA_MIN  -89
#define LNA_MAX 71
#define LNA_MIN  -3
#define RATE_MAX 61440000
#define RATE_MIN  2083333
#define FRQ_MAX  6E9
#define FRQ_MIN   70E6
#define SIGPOW_MAX 0
#define SIGPOW_MIN  -130
#define DIFFRQ_MAX 56E6
#define DIFFRQ_MIN  1E5
#define VNAPOW_MAX 8192
#define VNAPOW_MIN 0
#define CALPNT_MAX 100
#define CALPNT_MIN  0
#define SWSPAN_MAX 6E9
#define SWSPAN_MIN 20
#define AWGLEN_MAX 8192
#define AWGLEN_MIN  4
#define POWOFFSET_MAX 100
#define POWOFFSET_MIN -100
#define PERIOD_MAX 8192
#define PERIOD_MIN  0
#define SAMPLESIZE_MAX 8192
#define SAMPLESIZE_MIN 4
#define TRIG_LVL_MAX 4096
#define TRIG_LVL_MIN -4096

void print_error(char *function, int error_code);
void print_help(char *function, char *argument);
int check_argument(char *input, char *function, char *argument);

#endif /* __PNA_HELP_H__ */
