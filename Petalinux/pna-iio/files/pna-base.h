#ifndef __PNA_BASE_H__
#define __PNA_BASE_H__

#include <iio.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include "pna-gpio.h"
#include "pna-fft.h"
#include "pna-fir.h"

#define HANNING_ENBW 1.50
#define MAX_FFT_LENGTH 8192 //FFT output on UART size (should be 1024 FIX)
#define FFT_24_BIT 3 // 24bit = 3byte
// #define FFT_16_BIT
#define UART_LENGTH 1024
#define DAC_MAX_VAL 2047*16
#define MAX_BW 56E6
#define PI 3.14159265
#define THIS_DRIVER "AD936X"
#define PHY_DEVICE "ad9361-phy"
#define DDS_DEVICE "cf-ad9361-dds-core-lpc"
#define CAP_DEVICE "cf-ad9361-lpc"
#define UDC_RX_DEVICE "adf4351-udc-rx-pmod"
#define UDC_TX_DEVICE "adf4351-udc-tx-pmod"

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
unsigned long write_DMA(char *bufferIn, size_t size);
unsigned long read_DMA(char *bufferOut, size_t size);
double read_sampling_frequency(const struct iio_device *dev);
ssize_t demux_sample(const struct iio_channel *chn,
					 void *sample, size_t size, void *d);
void init_rx_channel(unsigned int fft_size);
void fill_rx_buffer(unsigned int fft_size);
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
uint8_t gpio_fft_status();
void gpio_fft_reset();
void init_all_gpio();
void gpio_fft_valid();

#endif //__PNA_BASE_H__
