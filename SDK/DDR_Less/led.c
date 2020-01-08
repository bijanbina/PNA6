/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xgpio.h"
#include <sleep.h>

#define GPIO_LED_DEVICE_ID	XPAR_AXI_GPIO_LED_DEVICE_ID
#define GPIO_SW_DEVICE_ID	XPAR_AXI_GPIO_SW_DEVICE_ID
#define GPIO_ATT_DEVICE_ID  XPAR_AXI_GPIO_ATT_DEVICE_ID
#define LED_DELAY	  10000000
#define SPI_GPIO_LE2  0
#define SPI_GPIO_LE1  1
#define SPI_GPIO_MOSI 2
#define SPI_GPIO_SCK  3

XGpio GpioOutputSw;
XGpio GpioOutputLed;
XGpio GpioOutputAtt;

void initAttenuation()
{
	/*
	 * Initialize the GPIO driver so that it's ready to use,
	 * specify the device ID that is generated in xparameters.h
	 */
	int Status = XGpio_Initialize(&GpioOutputAtt, GPIO_ATT_DEVICE_ID);
	if (Status != XST_SUCCESS)  {
	  return XST_FAILURE;
	}
	/* Set the direction for all signals to be outputs */
	XGpio_SetDataDirection(&GpioOutputAtt, 1, 0x0);

	XGpio_DiscreteWrite(&GpioOutputAtt, 1, 0x00); // default value

}

void setAttenuation(int channel, double att)
{
	uint8_t att_int = att*4;
	uint8_t gpio_spi_buf = 0x00;
	for(int i=0; i<7; i++)
	{
		gpio_spi_buf = 0x00;
		XGpio_DiscreteWrite(&GpioOutputAtt, 1, gpio_spi_buf);
		usleep(10);
		// set serial in
		uint8_t att_bit = (att_int >> (6-i)) & 1;
		// gpio_spi_buf &= 0x0B;
		gpio_spi_buf |= att_bit << SPI_GPIO_MOSI;
		XGpio_DiscreteWrite(&GpioOutputAtt, 1, gpio_spi_buf);
		usleep(10);
		// toggle clk
		gpio_spi_buf ^= 1 << SPI_GPIO_SCK;
		XGpio_DiscreteWrite(&GpioOutputAtt, 1, gpio_spi_buf);
		usleep(10);
	}

	gpio_spi_buf ^= 1 << SPI_GPIO_SCK;
	XGpio_DiscreteWrite(&GpioOutputAtt, 1, gpio_spi_buf);
	usleep(10);

	if(channel == 1)
	{
		gpio_spi_buf |= 1 << SPI_GPIO_LE1;
		XGpio_DiscreteWrite(&GpioOutputAtt, 1, gpio_spi_buf);
		usleep(10);
		gpio_spi_buf ^= 1 << SPI_GPIO_LE1;
		XGpio_DiscreteWrite(&GpioOutputAtt, 1, gpio_spi_buf);
	}
	else
	{
		gpio_spi_buf |= 1 << SPI_GPIO_LE2;
		XGpio_DiscreteWrite(&GpioOutputAtt, 1, gpio_spi_buf);
		usleep(10);
		gpio_spi_buf ^= 1 << SPI_GPIO_LE2;
		XGpio_DiscreteWrite(&GpioOutputAtt, 1, gpio_spi_buf);
	}
	usleep(10);
}

int main()
{
    init_platform();

    print("Hello World\n\r");

	volatile int Delay;
	u32 LedBit;
	u32 LedLoop;
	int Status;

	/*
	 * Initialize the GPIO driver so that it's ready to use,
	 * specify the device ID that is generated in xparameters.h
	 */
	 Status = XGpio_Initialize(&GpioOutputLed, GPIO_LED_DEVICE_ID);
	 if (Status != XST_SUCCESS)  {
		  return XST_FAILURE;
	 }

	 /* Set the direction for all signals to be outputs */
	 XGpio_SetDataDirection(&GpioOutputLed, 1, 0x0);

	 /* Set the GPIO outputs to low */
	 XGpio_DiscreteWrite(&GpioOutputLed, 1, 0x0);

	 while(TRUE)
	 {
		 for (LedBit = 0x1; LedBit < 5; LedBit++)
		 {

			for (LedLoop = 0; LedLoop < 2; LedLoop++)
			{

				/* Set the GPIO Output to High */
				XGpio_DiscreteWrite(&GpioOutputLed, 1, ~(1 << LedBit));

				/* Wait a small amount of time so the LED is visible */
				for (Delay = 0; Delay < LED_DELAY; Delay++);

				/* Clear the GPIO Output */
				//XGpio_DiscreteClear(&GpioOutputLed, 1, 1 << LedBit);
				XGpio_DiscreteWrite(&GpioOutputLed, 1, 0x1F);

				/* Wait a small amount of time so the LED is visible */
				for (Delay = 0; Delay < LED_DELAY; Delay++);

			  }

		 }

	 }
    cleanup_platform();
    return 0;
}
