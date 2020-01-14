#include "pna-fir.h"

int load_fir_filter(const char *file_name, struct iio_device *dev1)
{
	int ret = -10;
	FILE *f;

	f = fopen(file_name, "r");
	if (f)
	{
		char *buf;
		ssize_t len;

		fseek(f, 0, SEEK_END);
		len = ftell(f);
		buf = malloc(len);
		fseek(f, 0, SEEK_SET);
		len = fread(buf, 1, len, f);
		fclose(f);

		ret = iio_device_attr_write_raw(dev1,
				"filter_fir_config", buf, len);
		free(buf);
	}
	else
	{
		printf("FIR filter file failed to open: %s\n", file_name);
	}

	if (ret < 0)
	{
		printf("FIR filter config failed: %s\n", file_name);
	}
	else
	{
		printf("Filter loaded: %s (ret = %i)\n", file_name, ret);
	}
	return ret;
}
