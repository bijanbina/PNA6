#ifndef __PNA_FIR_H__
#define __PNA_FIR_H__

#include <iio.h>
#include <string.h>
#include <stdio.h>
#include "pna-interface.h"

int load_fir_filter(const char *file_name, struct iio_device *dev1);

#endif /* __PNA_FIR_H__ */
