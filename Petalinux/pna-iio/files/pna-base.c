#include "pna-base.h"

bool is_2rx_2tx;
bool has_udc_driver;
bool tx_rssi_available;
bool dds_scale_available_mode;
bool dds_is_local;
bool dds_enabled = true;

unsigned int num_glb, num_tx, num_rx;
unsigned int rx_lo, tx_lo;
unsigned int rx_sample_freq, tx_sample_freq;
unsigned int dcxo_coarse_num, dcxo_fine_num;

long long dds_alignment;

char *rx_fastlock_store_name, *rx_fastlock_recall_name, *rx_fastlock_save_name, *rx_fastlock_load_name;
// char *tx_fastlock_store_name, *tx_fastlock_recall_name;

struct iio_context *ctx = NULL;
struct iio_device *dev, *dds, *cap, *udc_rx, *udc_tx;
struct iio_channel *dds_ch0, *dds_ch1;
struct iio_channel *rx_dev_ch0, *rx_dev_ch1;
struct iio_channel *tx_dev_ch0, *tx_dev_ch1;
struct iio_channel *tx_alt_dev_ch0, *tx_alt_dev_ch1;
struct iio_channel *cap_ch0, *cap_ch1;

struct iio_device *iio_dac;
struct iio_channel *iio_ch;
unsigned dds_tones_count;
const char *dds_name;
struct iio_buffer *dds_buffer;
struct iio_buffer *capture_buffer = NULL;

const char *rx_freq_name, *tx_freq_name;
int32_t rx1_buffer [2*MAX_FFT_LENGTH];
int32_t rx2_buffer [2*MAX_FFT_LENGTH];
int8_t dac_buf[8*MAX_FFT_LENGTH]; // I1-Q1-I2-Q2
int rx_indx=0;
int fd_dma; //file descriptor DMA driver

unsigned long memCpy_DMA(char *bufferIn, char *bufferOut, unsigned long byteToMove)
{
	// printf("memcpy len = %d \r\n", byteToMove);

	int write_return = 0;
	// printf("fft_status#1 = %d\r\n", gpio_fft_status());
	write_return = write(fd_dma, bufferIn, byteToMove);
	// printf("fft_status#2 = %d\r\n", gpio_fft_status());
	// printf("write_return=%d \r\n", write_return);

	// printf ("byteToMove = %d \r\n", byteToMove);
	int read_return = 0;
	read_return = read(fd_dma, bufferOut, byteToMove);
	// printf("fft_status#3 = %d\r\n", gpio_fft_status());
	// printf("read_return=%d \r\n", read_return);

	// printf ("Debug Flag #5\r\n");
	return byteToMove;
}

unsigned long write_DMA(char *bufferIn, size_t size)
{
	printf("write_DMA called!\r\n");
	for(int i=0; i<10; i++)
	{
		printf("write_dma[%d]: %d\r\n", i, bufferIn[i]);
	}
	printf("write_DMA print finished!\r\n");
	int write_bytes = write(fd_dma, bufferIn, size);
	return write_bytes;
}

unsigned long read_DMA(char *bufferOut, size_t size)
{
	printf("read_DMA called!\r\n");
	int read_bytes = read(fd_dma, bufferOut, size);
	for(int i=0; i<10; i++)
	{
		printf("read_dma[%d]: %d\r\n", i, bufferOut[i]);
	}
	printf("read_DMA print finished!\r\n");
	return read_bytes;
}

double read_sampling_frequency(const struct iio_device *dev)
{
	double freq = 400.0;
	int ret = -1;
	unsigned int i, nb_channels = iio_device_get_channels_count(dev);
	const char *attr;
	char buf[1024];

	for (i = 0; i < nb_channels; i++) {
		struct iio_channel *ch = iio_device_get_channel(dev, i);

		if (iio_channel_is_output(ch) || strncmp(iio_channel_get_id(ch),
					"voltage", sizeof("voltage") - 1))
			continue;

		ret = iio_channel_attr_read(ch, "sampling_frequency",
				buf, sizeof(buf));
		if (ret > 0)
			break;
	}

	if (ret < 0)
		ret = iio_device_attr_read(dev, "sampling_frequency",
				buf, sizeof(buf));
	if (ret < 0) {
		const struct iio_device *trigger;

		attr = iio_device_find_attr(trigger, "sampling_frequency");
		if (!attr)
			attr = iio_device_find_attr(trigger, "frequency");
		if (attr)
			ret = iio_device_attr_read(trigger, attr, buf,
				sizeof(buf));
		else
			ret = -ENOENT;
	}

	if (ret > 0)
		sscanf(buf, "%lf", &freq);

	if (freq < 0)
		freq += 4294967296.0;

	return freq;
}

// AD9361 Specific Configuration
// Size = 2 (16 bit)
// Channel Count = 4 (RX1-I RX1-Q RX2-I RX2-Q)
// format->is_signed = true
ssize_t demux_sample(const struct iio_channel *chn,
		void *sample, size_t size, void *d)
{
	int16_t *sample_int = (int16_t *)sample;
	int16_t val = *(sample_int);
	val = val & 0xFFFF;
	// printf("val: 0x%x, sample: 0x%x \r\n", val , *sample_int);

	if( rx_indx % 4 == 0 )
	{
			rx1_buffer[rx_indx/4] = val;
	}
	else if( rx_indx % 4 == 1 )
	{
		rx1_buffer[rx_indx/4] &= 0x0000FFFF;
		rx1_buffer[rx_indx/4] |= (val << 16);
		//printf("%d: 0x%x\r\n", rx_indx/4, rx1_buffer[rx_indx/4]);
	}
	else if( rx_indx % 4 == 2 )
	{
		rx2_buffer[rx_indx/4] = val;
	}
	else if( rx_indx % 4 == 3 )
	{
		rx2_buffer[rx_indx/4] &= 0x0000FFFF;
		rx2_buffer[rx_indx/4] |= (val << 16);
		//printf("%d: 0x%x\r\n", rx_indx/4, rx1_buffer[rx_indx/4]);
	}
	rx_indx++;

	return size;
}

void init_rx_channel(unsigned int fft_size)
{
	unsigned int i, j;
	double freq;
	struct iio_channel *chn;
	struct extra_info *info;
	struct extra_dev_info *dev_info;
	long long rate;

	int num_devices = iio_context_get_devices_count(ctx);
	//printf("num_devices = %d \n", num_devices );

	for (i = 0; i < num_devices; i++)
	{
		struct iio_device *dev_temp = iio_context_get_device(ctx, i);
		unsigned int channel_count = iio_device_get_channels_count(dev_temp);
///////////////////////// FIXME ////////////////////////////
		struct extra_dev_info *dev_info_temp = calloc(1, sizeof(*dev_info_temp));
		iio_device_set_data(dev_temp, dev_info_temp);
		dev_info_temp->input_device = is_input_device(dev_temp);
		unsigned int sample_size_temp = iio_device_get_sample_size(dev_temp);
		// printf("id=%d, sample_size_temp=%d, input_device=%d \n", i,
				 // sample_size_temp, dev_info_temp->input_device);

		for (j = 0; j < channel_count; j++)
		{
			chn = iio_device_get_channel(dev_temp, j);
			info = calloc(1, sizeof(*info));
			info->dev = dev_temp;
			iio_channel_set_data(chn, info);
		}
	}
	// printf("flag6\r\n");

	dev_info = iio_device_get_data(dev);
	unsigned int sampling_rate = 5000000;
	// printf("Debug flag #1 \n");
	// printf("flag7\r\n");

	//##### initialize capture buffer and channels
	// device_set_rx_sampling_freq(dev, sampling_rate);
	// rate = device_get_rx_sampling_freq(cap);
	// if (rate != sampling_rate)
	// {
	// 	printf("Failed to set the rx sampling rate to %lld"
	// 		"in \n", sampling_rate);
	// }
	// else
	// {
	// 	printf("INIT_RX_CHANNEL: rate=%d, sampling rate=%d\n",
	//  			 rate, sampling_rate);
	// }
	// printf("flag8\r\n");

	dev_info = iio_device_get_data(cap);
	dev_info->sample_count = fft_size;
	// dev_info->adc_freq = rate;
	// if (dev_info->adc_freq >= 1000000)
	// {
	// 	dev_info->adc_scale = 'M';
	// 	dev_info->adc_freq /= 1000000.0;
	// }
	// else if (dev_info->adc_freq >= 1000)
	// {
	// 	dev_info->adc_scale = 'k';
	// 	dev_info->adc_freq /= 1000.0;
	// }
	// else if (dev_info->adc_freq >= 0)
	// {
	// 	dev_info->adc_scale = ' ';
	// }
	// else
	// {
	// 	dev_info->adc_scale = '?';
	// 	dev_info->adc_freq = 0.0;
	// }
	// printf("flag9\r\n");
	for (i = 0; i < iio_device_get_channels_count(cap); i++)
	{
		chn = iio_device_get_channel(cap, i);
		info = iio_channel_get_data(chn);

		iio_channel_enable(chn);
		info->data_ref = (float *) malloc(fft_size);
	}
	// printf("flag10\r\n");
	create_adc_buffer(fft_size);
	// printf("flag11\r\n");
	//printf("refill_rx_buffer\n");
}

void fill_rx_buffer(unsigned int fft_size)
{
	// unsigned int i;
	// struct iio_channel *chn;
	// struct extra_info *info;

	/* Get captured data */
	ssize_t ret = iio_buffer_refill(capture_buffer);
	//printf("ret=%d\n", ret);
	if (ret < 0)
	{
		printf("Error while refilling iio buffer, return=%d \n", ret);
	}
	else
	{
		// iio_buffer_foreach_sample(capture_buffer, demux_sample, NULL);
		rx_indx = 0;
		int16_t *capture_pointer = (int *)iio_buffer_start(capture_buffer);
		for(int i=0; i<fft_size; i++)
		{
			int16_t val_i1 = *(capture_pointer);
			int16_t val_q1 = *(capture_pointer+1);
			int16_t val_i2 = *(capture_pointer+2);
			int16_t val_q2 = *(capture_pointer+3);

			val_i1 = val_i1 & 0xFFFF;
			val_q1 = val_q1 & 0xFFFF;
			val_i2 = val_i2 & 0xFFFF;
			val_q2 = val_q2 & 0xFFFF;

			rx1_buffer[rx_indx] = val_i1;
			rx1_buffer[rx_indx] &= 0x0000FFFF;
			rx1_buffer[rx_indx] |= (val_q1 << 16);
			rx2_buffer[rx_indx] = val_i2;
			rx2_buffer[rx_indx] &= 0x0000FFFF;
			rx2_buffer[rx_indx] |= (val_q2 << 16);
			rx_indx++;
			capture_pointer += 4;
		}
	}
}

/*
 * Check if a device has scan elements and if it is an output device (type = 0)
 * or an input device (type = 1).
 */
bool device_type_get(const struct iio_device *dev, int type)
{
	struct iio_channel *ch;
	int nb_channels, i;

	if (!dev)
		return false;

	nb_channels = iio_device_get_channels_count(dev);
	for (i = 0; i < nb_channels; i++)
	{
		ch = iio_device_get_channel(dev, i);
		if (iio_channel_is_scan_element(ch) &&
			(type ? !iio_channel_is_output(ch) : iio_channel_is_output(ch)))
			return true;
	}

	return false;
}

void device_set_rx_sampling_freq(struct iio_device *dev, long long freq_hz)
{
	struct iio_channel *ch0;

	ch0 = iio_device_find_channel(dev, "voltage0", false);
	if (ch0)
	{
		iio_channel_attr_write_longlong(ch0, "sampling_frequency", freq_hz);
	}
	else
	{
		printf("Failed to set rx sampling freq \n");
	}
}

long long device_get_rx_sampling_freq(struct iio_device *dev)
{
	long long freq_hz = 0;
	struct iio_channel *ch0;

	ch0 = iio_device_find_channel(dev, "voltage0", false);
	if (ch0)
	{
		iio_channel_attr_read_longlong(ch0, "sampling_frequency", &freq_hz);
	}
	else
	{
		printf("Failed to retrieve iio channel in \n");
	}

	return freq_hz;
}

bool is_input_device(const struct iio_device *dev)
{
	return device_type_get(dev, 1);
}

bool is_output_device(const struct iio_device *dev)
{
	return device_type_get(dev, 0);
}

void dds_init(void)
{
	iio_dac = dds;
	dds_name = iio_device_get_name(dds);

	dds_tones_count = 4;
	for (int i = 0, dds_tones_count = 0; i < iio_device_get_channels_count(dds); i++)
	{
		struct iio_channel *chn = iio_device_get_channel(dds, i);
		char *name = iio_channel_get_name(chn);

		if (name && strncmp(name, "TX", 2) == 0)
			dds_tones_count++;
	}

	for(int ch_index=0 ; ch_index<4 ; ch_index++)
	{
		struct iio_channel *channel = iio_device_get_channel(iio_dac, ch_index);

		if (dds_enabled)
			iio_channel_enable(channel);
		else
			iio_channel_disable(channel);
	}


	// int count;
	// struct iio_device *dac_with_scanelems;

	// dac_with_scanelems = manager->dac1.iio_dac;
	// count = device_scan_elements_count(dac_with_scanelems);
	// if (manager->dacs_count == 2 && count == 0)
	// {
	// 	dac_with_scanelems = manager->dac2.iio_dac;
	// 	count = device_scan_elements_count(dac_with_scanelems);
	// }

	struct iio_channel *ch = iio_device_find_channel(iio_dac, "altvoltage0", true);

	if (iio_channel_find_attr(ch, "scale_available"))
		dds_scale_available_mode = true;
	else
		dds_scale_available_mode = false;

	dds_is_local = strcmp(iio_context_get_name(ctx), "local") ? false : true;

	iio_device_buffer_attr_read_longlong(iio_dac,
						 "length_align_bytes",
						 &dds_alignment);
		// manager->hw_reported_alignment = true;
}

void fmcomms2_init(void)
{
	ctx = iio_create_default_context();
	if (!ctx)
		return;

	dev = iio_context_find_device(ctx, PHY_DEVICE);
	dds = iio_context_find_device(ctx, DDS_DEVICE);
	cap = iio_context_find_device(ctx, CAP_DEVICE);
	udc_rx = iio_context_find_device(ctx, UDC_RX_DEVICE);
	udc_tx = iio_context_find_device(ctx, UDC_TX_DEVICE);
	has_udc_driver = (udc_rx && udc_tx);

	rx_dev_ch0 = iio_device_find_channel(dev, "voltage0", false);
	rx_dev_ch1 = iio_device_find_channel(dev, "voltage1", false);

	// dac_tx_manager = dac_data_manager_new(dds, NULL, ctx);

	is_2rx_2tx = rx_dev_ch1 && iio_channel_find_attr(rx_dev_ch1, "hardwaregain");

	num_glb = 0;
	dcxo_coarse_num = num_glb;
	dcxo_fine_num = num_glb;

	tx_alt_dev_ch1 = iio_device_find_channel(dev, "altvoltage1", true);
	if (iio_channel_find_attr(tx_alt_dev_ch1, "frequency"))
		rx_freq_name = "frequency";
	else
		rx_freq_name = "RX_LO_frequency";


	tx_alt_dev_ch0 = iio_device_find_channel(dev, "altvoltage0", true);

	if (iio_channel_find_attr(tx_alt_dev_ch0, "fastlock_store"))
		rx_fastlock_store_name = "fastlock_store";
	else
		rx_fastlock_store_name = "RX_LO_fastlock_store";
	if (iio_channel_find_attr(tx_alt_dev_ch0, "fastlock_recall"))
		rx_fastlock_recall_name = "fastlock_recall";
	else
		rx_fastlock_recall_name = "RX_LO_fastlock_recall";
	if (iio_channel_find_attr(tx_alt_dev_ch0, "fastlock_save"))
		rx_fastlock_save_name = "fastlock_save";
	else
		rx_fastlock_save_name = "RX_LO_fastlock_save";
	if (iio_channel_find_attr(tx_alt_dev_ch0, "fastlock_load"))
		rx_fastlock_load_name = "fastlock_load";
	else
		rx_fastlock_load_name = "RX_LO_fastlock_load";

	/* Transmit Chain */
	tx_dev_ch0 = iio_device_find_channel(dev, "voltage0", true);
	if (is_2rx_2tx)
		tx_dev_ch1 = iio_device_find_channel(dev, "voltage1", true);

	tx_rssi_available = tx_dev_ch0 && iio_channel_find_attr(tx_dev_ch0, "rssi");
	if (is_2rx_2tx)
		tx_rssi_available = tx_rssi_available &&
				(tx_dev_ch1 && iio_channel_find_attr(tx_dev_ch1, "rssi"));

	tx_sample_freq = num_tx;

	tx_lo = num_tx;
	tx_alt_dev_ch1 = iio_device_find_channel(dev, "altvoltage1", true);

	if (iio_channel_find_attr(tx_alt_dev_ch1, "frequency"))
		tx_freq_name = "frequency";
	else
		tx_freq_name = "TX_LO_frequency";

	dds_ch0 = iio_device_find_channel(dds, "voltage0", true);
	if (iio_channel_find_attr(dds_ch0, "sampling_frequency_available"))
	{
		struct iio_channel *chn = dds_ch0;
		double fpga;

		iio_channel_attr_read_double(chn, "sampling_frequency", &fpga);
	}

	cap_ch0 = iio_device_find_channel(cap, "voltage0", false);

	// struct iio_device *adc_dev;
	// struct extra_dev_info *adc_info;

	// adc_dev = iio_context_find_device(get_context_from_osc(), CAP_DEVICE);
	// if (adc_dev)
	// {
	// 	adc_info = iio_device_get_data(adc_dev);
	// 	// if (adc_info)
	// 	// 	adc_info->plugin_fft_corr = 20 * log10(1/sqrt(HANNING_ENBW));
	// }
}
long long get_frequency(char *token)
{
	int token_len = strlen(token);
	char *sz = NULL;
	long long frequency;
	switch (token[token_len-1]) {
		case 'k':
		case 'K':
			token[token_len - 1] = '\0'; // null char
			frequency = strtoll(token, &sz, 10)*1E3;
			break;
		case 'm':
		case 'M':
			token[token_len - 1] = '\0'; // null char
			frequency = strtoll(token, &sz, 10)*1E6;
			break;
		case 'g':
		case 'G':
			token[token_len - 1] = '\0'; // null char
			frequency = strtoll(token, &sz, 10)*1E9;
			break;
		default:
			frequency = strtoll(token, &sz, 10);
	}
	return frequency;
}

void create_dds_buffer(int8_t *data, int sample_size)
{
	if (dds_buffer)
	{
		iio_buffer_destroy ( dds_buffer );
	}

	dds_buffer = iio_device_create_buffer(iio_dac, sample_size, true);

	if (!dds_buffer)
	{
		fprintf(stderr, "Unable to create buffer: %s\r\n", strerror(errno));
	}

	memcpy(iio_buffer_start(dds_buffer), dac_buf,
			iio_buffer_end(dds_buffer) - iio_buffer_start(dds_buffer));

	iio_buffer_push(dds_buffer);
}

void create_adc_buffer(unsigned int fft_size) //, uint8_t is_init
{
	struct iio_channel *chn;
	struct extra_info *info;

	if(capture_buffer)
	{
		// printf("Destroy capture buffer\n");
		iio_buffer_destroy(capture_buffer);
	}
	capture_buffer = iio_device_create_buffer(cap, fft_size, false);
	if (!capture_buffer)
	{
		printf("Could not create iio buffer in capture device\n");
		return;
	}

	//printf("hello=0x%x\n", capture_buffer);
	//printf("entered fill_rx_buffer\n");
	/* Reset the data offset for all channels */
	for (int i = 0; i < iio_device_get_channels_count(cap); i++)
	{
		chn = iio_device_get_channel(cap, i);
		info = iio_channel_get_data(chn);
		info->offset = 0;
	}
}

void gpio_fft(int gpio_value)
{
	int gl_gpio_base = GPIO_BASE_CFFT;
	int nchannel = GPIO_NCHANNEL_CFFT;
	int cntr = 0;

	while(gpio_value>0)
  {
      gpio_value = gpio_value >> 1;
      cntr++;
  }
	set_gpio_value(gl_gpio_base, nchannel, cntr - 1 + 256);
	usleep(10);
	gpio_fft_valid();
}

uint8_t gpio_fft_status()
{
	int gl_gpio_base = GPIO_BASE_STATUS;
	int nchannel = GPIO_NCHANNEL_STATUS;

	return get_gpio_value(gl_gpio_base, nchannel);
}

void gpio_fft_reset()
{
	int gl_gpio_base = GPIO_BASE_RESET;
	int nchannel = GPIO_NCHANNEL_RESET;

	set_gpio_value(gl_gpio_base, nchannel, 0);
	usleep(10);
	set_gpio_value(gl_gpio_base, nchannel, 1);
	usleep(10);
}

void gpio_fft_valid()
{
	int gl_gpio_base = GPIO_BASE_VALID;
	int nchannel = GPIO_NCHANNEL_VALID;

	set_gpio_value(gl_gpio_base, nchannel, 1);
	usleep(10);
	set_gpio_value(gl_gpio_base, nchannel, 0);
	usleep(10);
}

void init_all_gpio()
{
	int gpio_base_reset = GPIO_BASE_RESET, nchannel_reset;
	int gpio_base_valid = GPIO_BASE_VALID, nchannel_valid;
	int gpio_base_cfft = GPIO_BASE_CFFT, nchannel_cfft;
	int gpio_base_status = GPIO_BASE_STATUS, nchannel_status;

	// nchannel_reset = open_gpio_channel(gpio_base_reset);
	// set_gpio_direction(gpio_base_reset, nchannel_reset, "out");
	nchannel_valid = open_gpio_channel(gpio_base_valid);
	set_gpio_direction(gpio_base_valid, nchannel_valid, "out");
	nchannel_cfft = open_gpio_channel(gpio_base_cfft);
	set_gpio_direction(gpio_base_cfft, nchannel_cfft, "out");
	nchannel_status = open_gpio_channel(gpio_base_status);
	set_gpio_direction(gpio_base_status, nchannel_status, "in");
}

ssize_t fastlock_store()
{
	return iio_channel_attr_write_longlong(tx_alt_dev_ch0, rx_fastlock_store_name, 0);
}

ssize_t fastlock_save(char *data)
{
	char data_temp[66];
	ssize_t ret = iio_channel_attr_read(tx_alt_dev_ch0, rx_fastlock_save_name, data_temp, sizeof(data_temp));
	strcpy(data, data_temp);
	return ret;
}

ssize_t fastlock_load(char* data)
{
	// printf("fastlock_load function : %s\r\n", data);
	return iio_channel_attr_write(tx_alt_dev_ch0, rx_fastlock_load_name, data);
}

ssize_t fastlock_recall(int slot)
{
	return iio_channel_attr_write_longlong(tx_alt_dev_ch0, rx_fastlock_recall_name, slot);
}

void set_bandwidth(int direction, long long bandwidth)
{
	if(direction == __RX)
	{
		iio_channel_attr_write_longlong(rx_dev_ch0, "rf_bandwidth", bandwidth);
	}
	else if(direction == __TX)
	{
		iio_channel_attr_write_longlong(tx_dev_ch0, "rf_bandwidth", bandwidth);
	}
	else
	{
		printf("Error with direction when setting bandwidth\r\n");
	}
}

long long get_bandwidth(int direction)
{
	long long bandwidth;
	if(direction == __RX)
	{
		iio_channel_attr_read_longlong(rx_dev_ch0, "rf_bandwidth", &bandwidth);
	}
	else if(direction == __TX)
	{
		iio_channel_attr_read_longlong(tx_dev_ch0, "rf_bandwidth", &bandwidth);
	}
	else
	{
		printf("Error with direction when reading bandwidth\r\n");
		bandwidth = -1;
	}
	return bandwidth;
}

void set_vga_gain(int channel_num, long long vga_gain)
{
	if(channel_num == 2)
		iio_channel_attr_write_longlong(tx_dev_ch1, "hardwaregain", vga_gain);
	else if(channel_num == 1)
		iio_channel_attr_write_longlong(tx_dev_ch0, "hardwaregain", vga_gain);
	else
	{
		printf( "---------------------------------------------------------------\r\n"
				"vga_gain: arguments are not valid.\r\n"
				"Read/Write vga_gain with port and value arguments.\r\n"
				"Usage:\r\n    vga_gain [port#] [value]\r\n");
	}
}

long long get_vga_gain(int channel_num)
{
	long long vga_gain;
	if(channel_num == 2)
		iio_channel_attr_read_longlong(tx_dev_ch1, "hardwaregain", &vga_gain);
	else if(channel_num == 1)
		iio_channel_attr_read_longlong(tx_dev_ch0, "hardwaregain", &vga_gain);
	else
	{
		printf( "---------------------------------------------------------------\r\n"
				"vga_gain: arguments are not valid.\r\n"
				"Read/Write vga_gain with port and value arguments.\r\n"
				"Usage:\r\n    vga_gain [port#] [value]\r\n");
	}
	return vga_gain;
}

void set_lna_gain(int channel_num, long long lna_gain)
{
	printf("channel num : %d \r\n", channel_num);
	if(channel_num == 2)
	{
		iio_channel_attr_write_longlong(rx_dev_ch1, "hardwaregain", lna_gain);
	}
	else if(channel_num == 1)
	{
		iio_channel_attr_write_longlong(rx_dev_ch0, "hardwaregain", lna_gain);
	}
	else
	{
		printf( "---------------------------------------------------------------\r\n"
				"lna_gain: arguments are not valid.\r\n"
				"Read/Write lna_gain with port and value arguments.\r\n"
				"Usage:\r\n    lna_gain [port#] [value]\r\n");
	}
}

long long get_lna_gain(int channel_num)
{
	long long lna_gain;
	if(channel_num == 2)
		iio_channel_attr_read_longlong(rx_dev_ch1, "hardwaregain", &lna_gain);
	else if(channel_num == 1)
		iio_channel_attr_read_longlong(rx_dev_ch0, "hardwaregain", &lna_gain);
	else
	{
		printf( "---------------------------------------------------------------\r\n"
				"lna_gain: arguments are not valid.\r\n"
				"Read/Write lna_gain with port and value arguments.\r\n"
				"Usage:\r\n    lna_gain [port#] [value]\r\n");
	}
	return lna_gain;
}

void set_gain_control_mode(int channel_num, char *gain_control_mode)
{
	if(channel_num == 2)
		iio_channel_attr_write(rx_dev_ch1, "gain_control_mode", gain_control_mode);
	else if(channel_num == 1)
		iio_channel_attr_write(rx_dev_ch0, "gain_control_mode", gain_control_mode);
	else
	{
		printf( "---------------------------------------------------------------\r\n"
				"gain_control_mode: arguments are not valid.\r\n"
				"Read/Write gain_control_mode with port and value arguments.\r\n"
				"Usage:\r\n    gain_control_mode [port#] [value]\r\n");
	}
}

void get_gain_control_mode(int channel_num, char *gain_control_mode)
{
	if(channel_num == 2)
		iio_channel_attr_read(rx_dev_ch1, "gain_control_mode", gain_control_mode, sizeof(gain_control_mode));
	else if(channel_num == 1)
		iio_channel_attr_read(rx_dev_ch0, "gain_control_mode", gain_control_mode, sizeof(gain_control_mode));
	else
	{
		printf( "---------------------------------------------------------------\r\n"
				"gain_control_mode: arguments are not valid.\r\n"
				"Read/Write gain_control_mode with port and value arguments.\r\n"
				"Usage:\r\n    gain_control_mode [port#] [value]\r\n");
	}
}

void set_sample_rate(int direction, long long sampling_frequency)
{
	if(direction == __RX)
	{
		iio_channel_attr_write_longlong(rx_dev_ch0, "sampling_frequency", sampling_frequency);
	}
	else if(direction == __TX)
	{
		iio_channel_attr_write_longlong(tx_dev_ch0, "sampling_frequency", sampling_frequency);
	}
	else
	{
		printf("Error with direction when setting sample rate\r\n");
	}
}

long long get_sample_rate(int direction)
{
	long long sampling_frequency;
	if(direction == __RX)
	{
		iio_channel_attr_read_longlong(rx_dev_ch0, "sampling_frequency", &sampling_frequency);
	}
	else if(direction == __TX)
	{
		iio_channel_attr_read_longlong(tx_dev_ch0, "sampling_frequency", &sampling_frequency);
	}
	else
	{
		printf("Error with direction when reading sample rate\r\n");
		sampling_frequency = -1;
	}
	return sampling_frequency;
}

void set_lo_freq(int direction, long long freq)
{
	if(direction == __RX)
	{
		iio_channel_attr_write_longlong(tx_alt_dev_ch0, rx_freq_name, freq);
	}
	else if(direction == __TX)
	{
		iio_channel_attr_write_longlong(tx_alt_dev_ch1, tx_freq_name, freq);
	}
	else
	{
		printf("Error with direction when setting LO frequency\r\n");
	}
}

long long get_lo_freq(int direction)
{
	long long freq;
	if(direction == __RX)
	{
		iio_channel_attr_read_longlong(tx_alt_dev_ch0, rx_freq_name, &freq);
	}
	else if(direction == __TX)
	{
		iio_channel_attr_read_longlong(tx_alt_dev_ch1, tx_freq_name, &freq);
	}
	else
	{
		printf("Error with direction when reading LO frequency\r\n");
		freq = -1;
	}
	return freq;
}

void set_port(int direction, char* port)
{
	if(direction == __RX)
	{
		iio_channel_attr_write(iio_device_find_channel(dev, "voltage0", false), "rf_port_select", port);
	}
	else if(direction == __TX)
	{
		iio_channel_attr_write(iio_device_find_channel(dev, "voltage0", true), "rf_port_select", port);
	}
	else
	{
		printf("Error with direction when setting port\r\n");
	}
}

void get_port(int direction, char *port)
{
	if(direction == __RX)
	{
		iio_channel_attr_read(iio_device_find_channel(dev, "voltage0", false), "rf_port_select", port, 100);
	}
	else if(direction == __TX)
	{
		iio_channel_attr_read(iio_device_find_channel(dev, "voltage0", true), "rf_port_select", port, 100);
	}
	else
	{
		printf("Error with direction when reading port\r\n");
		port = NULL;
	}
}

void set_fir_en(int direction, bool fir_en)
{
	if(direction == __RX)
	{
		iio_channel_attr_write_bool(rx_dev_ch0, "filter_fir_en", fir_en);
	}
	else if(direction == __TX)
	{
		iio_channel_attr_write_bool(tx_dev_ch0, "filter_fir_en", fir_en);
	}
	else
	{
		printf("Error with direction when setting fir enable\r\n");
	}
}

bool get_fir_en(int direction)
{
	bool fir_en;
	if(direction == __RX)
	{
		iio_channel_attr_read_bool(rx_dev_ch0, "filter_fir_en", &fir_en);
	}
	else if(direction == __TX)
	{
		iio_channel_attr_read_bool(tx_dev_ch0, "filter_fir_en", &fir_en);
	}
	else
	{
		printf("Error with direction when reading fir enable\r\n");
		fir_en = false;
	}
	return fir_en;
}

void write_reg_ad9361(long long address, const char *value)
{
	char buffer[80];
	sprintf(buffer, "0x%llx 0x%s", address, value);
	iio_device_debug_attr_write(dev, "direct_reg_access", buffer);
}

// read value from address and store it in the value
void read_reg_ad9361(long long address, char *value)
{
	iio_device_debug_attr_write_longlong(dev, "direct_reg_access", address);
	iio_device_debug_attr_read(dev, "direct_reg_access", value, 80);
}

void set_bb_dc(bool bb_dc_en)
{
	iio_channel_attr_write_bool(rx_dev_ch0, "bb_dc_offset_tracking_en", bb_dc_en);
}

bool get_bb_dc()
{
	bool bb_dc_en;
	iio_channel_attr_read_bool(rx_dev_ch0, "bb_dc_offset_tracking_en", &bb_dc_en);
	return bb_dc_en;
}

void set_rf_dc(bool rf_dc_en)
{
	iio_channel_attr_write_bool(rx_dev_ch0, "rf_dc_offset_tracking_en", rf_dc_en);
}

bool get_rf_dc()
{
	bool rf_dc_en;
	iio_channel_attr_read_bool(rx_dev_ch0, "rf_dc_offset_tracking_en", &rf_dc_en);
	return rf_dc_en;
}

void set_quad_track(bool quad_track)
{
	iio_channel_attr_write_bool(rx_dev_ch0, "quadrature_tracking_en", quad_track);
}

bool get_quad_track()
{
	bool quad_track;
	iio_channel_attr_read_bool(rx_dev_ch0, "rf_dc_offset_tracking_en", &quad_track);
	return quad_track;
}
