#ifndef __BASE_IIO_H__
#define __BASE_IIO_H__

#include <iio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#define HANNING_ENBW 1.50
#define FFT_LENGTH 1024 //FFT output on UART size (should be 1024 FIX)
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
						 unsigned long elems, size_t size);
double read_sampling_frequency(const struct iio_device *dev);
ssize_t demux_sample(const struct iio_channel *chn,
					 void *sample, size_t size, void *d);
void calc_fft_dma(int32_t *bufferIn, int16_t *fft_abs, int16_t *fft_phase, int is_debug);
void init_rx_channel();
void fill_rx_buffer();
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

#endif //__BASE_IIO_H__
