#ifndef __PNA_BASE_H__
#define __PNA_BASE_H__

#include <iio.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include "pna-gpio.h"
#include "pna-capture.h"
#include "pna-prepare.h"
#include "pna-interface.h"
#include "pna-siggen.h"
#include "pna-help.h"
#include <stdio.h>

#define HANNING_ENBW 1.50
#define FFT_24_BIT 3 // 24bit = 3byte
// #define FFT_16_BIT
#define UART_LENGTH 1024
#define MAX_BW 56E6
#define PI 3.14159265
#define THIS_DRIVER "AD936X"
#define PHY_DEVICE "ad9361-phy"
#define DDS_DEVICE "cf-ad9361-dds-core-lpc"
#define CAP_DEVICE "cf-ad9361-lpc"
#define UDC_RX_DEVICE "adf4351-udc-rx-pmod"
#define UDC_TX_DEVICE "adf4351-udc-tx-pmod"
#define VCO_CAL_STATUS 0x244
#define CONTROL_OUTPUT_VAL 36
#define __RX 1
#define __TX 0

#define SIGNAL_INPHASE_ENABLED 1
#define SIGNAL_QUAD_ENABLED       2
#define SIGNAL_IQ_ENABLED             3

#define START_OF_PACKET "\r\r\r\r\r"

#define PNT_NUM_CAL 101

struct extra_dev_info
{
	bool input_device;
	struct iio_buffer *buffer;
	unsigned int sample_count;
	unsigned int buffer_size;
	unsigned int channel_trigger;
	bool channel_trigger_enabled;
	bool trigger_falling_edge;
	float trigger_value;
	double adc_freq;
	char adc_scale;
	float **channels_data_copy;
	int *plots_sample_counts;
	float plugin_fft_corr;
};

struct extra_info
{
	struct iio_device *dev;
	float *data_ref;
	off_t offset;
	int shadow_of_enabled;
	bool may_be_enabled;
	double lo_freq;
	unsigned int constraints;
};

unsigned long memCpy_DMA(char *bufferIn, char *bufferOut,
												 unsigned long byteToMove);
unsigned long write_DMA(unsigned char *bufferIn, size_t size);
unsigned long read_DMA(unsigned char *bufferOut, size_t size);
double read_sampling_frequency(const struct iio_device *dev);
ssize_t demux_sample(const struct iio_channel *chn,
					 void *sample, size_t size, void *d);
void init_rx_channel(unsigned int fft_size);
void fill_rx_buffer(unsigned int fft_size);
void fill_rx_buffer_single(unsigned int fft_size);
bool device_type_get(const struct iio_device *dev, int type);
void device_set_rx_sampling_freq(struct iio_device *dev,
								                 long long freq_hz);
long long device_get_rx_sampling_freq(struct iio_device *dev);
bool is_input_device(const struct iio_device *dev);
bool is_output_device(const struct iio_device *dev);
void dds_init(void);
void fmcomms2_init(void);
long long get_frequency(char *token);
void create_dds_buffer(int8_t *data, int sample_size);
void create_adc_buffer(unsigned int fft_size);

void gpio_fft(int gpio_value);
void set_gpio_emio(int base, int nchannel, int gpio_value);
uint8_t get_gpio_emio(int base, int nchannel);
uint8_t gpio_fft_status();
void gpio_fft_reset();
void init_all_gpio();
void gpio_fft_valid();

ssize_t fastlock_store(int direction);
ssize_t fastlock_read_cal(int direction, char *data);
ssize_t fastlock_load(int direction, char* data);
ssize_t fastlock_recall(int direction, int slot);

void set_rx_switches(long long freq);
void set_tx_switches(bool enable, long long freq);
bool get_tx_switches();

int get_sample_size();
void set_bandwidth(int direction, long long bandwidth);
long long get_bandwidth(int direction);
void set_powerdown(int direction, long long value);
long long get_powerdown(int direction);
void set_vga_gain(int channel_num, long long vga_gain);
long long get_vga_gain(int channel_num);
void set_lna_gain(int channel_num, long long lna_gain);
long long get_lna_gain(int channel_num);
void set_gain_control_mode(int channel_num, char *gain_control_mode);
void get_gain_control_mode(int channel_num, char *gain_control_mode);
void set_sample_rate(int direction, long long sampling_frequency);
long long get_sample_rate(int direction);
void set_lo_freq(int direction, long long freq);
long long get_lo_freq(int direction);
int calibrate_tx(long long last_freq);
void set_port(int direction, char* port);
void get_port(int direction, char *port);
void set_fir_en(int direction, bool fir_en);
bool get_fir_en(int direction);
void set_bb_dc(bool bb_dc_en);
bool get_bb_dc();
void set_rf_dc(bool rf_dc_en);
bool get_rf_dc();
void set_quad_track(bool quad_track);
bool get_quad_track();
void read_reg_ad9361(long long address, char *value);
void write_reg_ad9361(long long address, const char *value);

int get_board_id();
void narrow_loop_filter(int nlf_menu);

#endif //__PNA_BASE_H__
