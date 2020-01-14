#ifndef __PNA_GPIO_H__
#define __PNA_GPIO_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

#define GPIO_ROOT "/sys/class/gpio"
#define GPIO_BASE_CFFT 1006
#define GPIO_NCHANNEL_CFFT 16
#define GPIO_BASE_RESET GPIO_BASE_CFFT + GPIO_NCHANNEL_CFFT
#define GPIO_NCHANNEL_RESET 1
#define GPIO_BASE_VALID 1023
#define GPIO_NCHANNEL_VALID 1
#define GPIO_BASE_STATUS 999
#define GPIO_NCHANNEL_STATUS 7

int open_gpio_channel(int gpio_base);
int close_gpio_channel(int gpio_base);
int set_gpio_direction(int gpio_base, int nchannel, char *direction);
int set_gpio_value(int gpio_base, int nchannel, int value);
int get_gpio_value(int gpio_base, int nchannel);
void signal_handler(int sig);

#endif //__PNA_GPIO_H__
