#include "pna_peripheral.h"
#include "xuartps_hw.h"
#include <stdio.h>


int32_t pna_init_gpio_sw(XGpio *GpioOutputSw, int channel) {
	int Status = XGpio_Initialize(GpioOutputSw, GPIO_SW_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	uint16_t value = 0;
	if (channel == 2) {
		value |= GPIO_SW_TX_A1;
		value |= GPIO_SW_P1;
		value &= ~GPIO_SW_TX_B1;
	}
	if (channel == 1) {
		value |= GPIO_SW_TX_A2;
		value |= GPIO_SW_P2;
		value &= ~GPIO_SW_TX_B2;
	}
	XGpio_SetDataDirection(GpioOutputSw, 1, 0x0);
	XGpio_DiscreteWrite(GpioOutputSw, 1, value);
	return value;
}

void pna_sw_lna_pow12(XGpio *gpio_sw, uint8_t en, uint16_t *value)
{
	uint16_t temp = *value;
	if(en == 1)
	{
		temp |= GPIO_SW_12V;
	}
	else
	{
		temp &= ~GPIO_SW_12V;
	}
	*value = temp;
	XGpio_DiscreteWrite(gpio_sw, 1, *value);
}

void pna_sw_lna_pow5(XGpio *gpio_sw, uint8_t en, uint16_t *value)
{
	uint16_t temp = *value;
	if(en == 1)
	{
		temp |= GPIO_SW_5V;
	}
	else
	{
		temp &= ~GPIO_SW_5V;
	}
	*value = temp;
	XGpio_DiscreteWrite(gpio_sw, 1, *value);
}

void pna_rx1_test(struct ad9361_rf_phy *phy, XGpio *gpio_sw, uint16_t *value)
{
	uint16_t temp = *value;
	temp = pna_sw_cp_p1(temp, (uint8_t) PORT_2);
	temp = pna_sw_p1(temp, (uint8_t) PORT_2);
	temp = pna_sw_tx_a2(temp, (uint8_t) PORT_2);
	temp = pna_sw_tx_b2(temp, (uint8_t) PORT_1);
	temp = pna_sw_tx_a1(temp, (uint8_t) PORT_2);
	temp = pna_sw_tx_b1(temp, (uint8_t) PORT_2);
	temp = pna_sw_p2(temp, (uint8_t) PORT_1);
	temp = pna_sw_cp_p2(temp, (uint8_t) PORT_1);
	temp = pna_sw_cpl(temp, (uint8_t) PORT_1);
	temp = pna_sw_lna(temp,(uint8_t) PORT_1);
	*value = temp;
	XGpio_DiscreteWrite(gpio_sw, 1, *value);
	ad9361_spi_write(phy->spi, REG_INPUT_SELECT, 0x03);//Rx1_A, Rx2_A balanced
}

void pna_rx2_test(struct ad9361_rf_phy *phy, XGpio *gpio_sw, uint16_t *value)
{
	uint16_t temp = *value;
	temp = pna_sw_cp_p1(temp, (uint8_t) PORT_1);
	temp = pna_sw_p1(temp, (uint8_t) PORT_1);
	temp = pna_sw_tx_a2(temp, (uint8_t) PORT_2);
	temp = pna_sw_tx_b2(temp, (uint8_t) PORT_1);
	temp = pna_sw_tx_a1(temp, (uint8_t) PORT_2);
	temp = pna_sw_tx_b1(temp, (uint8_t) PORT_2);
	temp = pna_sw_p2(temp, (uint8_t) PORT_2);
	temp = pna_sw_cp_p2(temp, (uint8_t) PORT_2);
	temp = pna_sw_cpl(temp, (uint8_t) PORT_2);
	temp = pna_sw_lna(temp,(uint8_t) PORT_2);
	*value = temp;
	XGpio_DiscreteWrite(gpio_sw, 1, *value);
	ad9361_spi_write(phy->spi, REG_INPUT_SELECT, 0x03);//Rx1_A, Rx2_A balanced
}

void pna_s11(struct ad9361_rf_phy *phy, XGpio *gpio_sw, uint16_t *value)
{
	uint16_t temp = *value;
	temp = pna_sw_cp_p1(temp, (uint8_t) PORT_2);
	temp = pna_sw_p1(temp, (uint8_t) PORT_2);
	temp = pna_sw_tx_a2(temp, (uint8_t) PORT_1);
	temp = pna_sw_tx_b2(temp, (uint8_t) PORT_2);
	temp = pna_sw_p2(temp, (uint8_t) PORT_1);
	temp = pna_sw_cp_p2(temp, (uint8_t) PORT_1);
	temp = pna_sw_cpl(temp, (uint8_t) PORT_2);
	temp = pna_sw_lna(temp,(uint8_t) PORT_1);
	*value = temp;
	XGpio_DiscreteWrite(gpio_sw, 1, *value);
	ad9361_set_rx_rf_port_input(phy, 0);//Rx1_A, Rx2_A balanced
}

void pna_s12(struct ad9361_rf_phy *phy, XGpio *gpio_sw, uint16_t *value)
{
	uint16_t temp = *value;
	temp = pna_sw_p2(temp, (uint8_t) PORT_2);
	temp = pna_sw_tx_a1(temp, (uint8_t) PORT_1);
	temp = pna_sw_tx_b1(temp, (uint8_t) PORT_2);
	temp = pna_sw_p1(temp, (uint8_t) PORT_1);
	temp = pna_sw_cp_p1(temp, (uint8_t) PORT_2);
	temp = pna_sw_cp_p2(temp, (uint8_t) PORT_1);
	temp = pna_sw_cpl(temp, (uint8_t) PORT_1);
	temp = pna_sw_lna(temp,(uint8_t) PORT_1);
	*value = temp;
	XGpio_DiscreteWrite(gpio_sw, 1, *value);
	ad9361_set_rx_rf_port_input(phy, 1);//Rx1_B, Rx2_B balanced
}

void pna_s21(struct ad9361_rf_phy *phy, XGpio *gpio_sw, uint16_t *value)
{
	uint16_t temp = *value;
	temp = pna_sw_tx_a2(temp, (uint8_t) PORT_1);
	temp = pna_sw_tx_b2(temp, (uint8_t) PORT_2);
	temp = pna_sw_p2(temp, (uint8_t) PORT_1);
	temp = pna_sw_cp_p1(temp, (uint8_t) PORT_1);
	temp = pna_sw_cp_p2(temp, (uint8_t) PORT_2);
	temp = pna_sw_cpl(temp, (uint8_t) PORT_2);
	temp = pna_sw_lna(temp,(uint8_t) PORT_1);
	temp = pna_sw_p1(temp, (uint8_t) PORT_2);
	*value = temp;
	XGpio_DiscreteWrite(gpio_sw, 1, *value);
	ad9361_set_rx_rf_port_input(phy, 0);//Rx1_A, Rx2_A balanced
}

void pna_s22(struct ad9361_rf_phy *phy, XGpio *gpio_sw, uint16_t *value)
{
	uint16_t temp = *value;
	temp = pna_sw_cp_p2(temp, (uint8_t) PORT_2);
	temp = pna_sw_p2(temp, (uint8_t) PORT_2);
	temp = pna_sw_tx_a1(temp, (uint8_t) PORT_1);
	temp = pna_sw_tx_b1(temp, (uint8_t) PORT_2);
	temp = pna_sw_p1(temp, (uint8_t) PORT_1);
	temp = pna_sw_cp_p1(temp, (uint8_t) PORT_1);
	temp = pna_sw_cpl(temp, (uint8_t) PORT_2);
	temp = pna_sw_lna(temp,(uint8_t) PORT_2);
	*value = temp;
	XGpio_DiscreteWrite(gpio_sw, 1, *value);
	ad9361_set_rx_rf_port_input(phy, 1);//Rx1_B, Rx2_B balanced
}

uint16_t pna_sw_tx_a1(uint16_t value,uint8_t port)
{
	return pna_sw_config_port(value,port,(uint16_t)GPIO_SW_TX_A1);
}

uint16_t pna_sw_tx_b1(uint16_t value,uint8_t port)
{
	return pna_sw_config_port(value,port,(uint16_t)GPIO_SW_TX_B1);
}

uint16_t pna_sw_p1(uint16_t value,uint8_t port)
{
	return pna_sw_config_port(value,port,(uint16_t)GPIO_SW_P1);
}

uint16_t pna_sw_cp_p1(uint16_t value,uint8_t port)
{
	return pna_sw_config_port(value,port,(uint16_t)GPIO_SW_CP_P1);
}

uint16_t pna_sw_tx_a2(uint16_t value,uint8_t port)
{
	return pna_sw_config_port(value,port,(uint16_t)GPIO_SW_TX_A2);
}

uint16_t pna_sw_tx_b2(uint16_t value,uint8_t port)
{
	return pna_sw_config_port(value,port,(uint16_t)GPIO_SW_TX_B2);
}

uint16_t pna_sw_p2(uint16_t value,uint8_t port)
{
	return pna_sw_config_port(value,port,(uint16_t)GPIO_SW_P2);
}

uint16_t pna_sw_cp_p2(uint16_t value,uint8_t port)
{
	return pna_sw_config_port(value,port,(uint16_t)GPIO_SW_CP_P2);
}

uint16_t pna_sw_cpl(uint16_t value, uint8_t port)
{
	return  pna_sw_config_port(value,port,(uint16_t)GPIO_SW_CPL);
}

uint16_t pna_sw_lna(uint16_t value, uint8_t port)
{
	return pna_sw_config_port(value,port,(uint16_t)GPIO_SW_LNA);
}

uint16_t pna_sw_config_port(uint16_t value, uint8_t port, uint16_t sw)
{
	uint16_t temp = value;
	if(port ==1)
	{
		temp |= sw;
	}
	else if(port == 2)
	{
		temp &= ~sw;
	}
	return temp;
}

/**
 * Enable/disable the desired TX channel.
 * @param phy The AD9361 state structure.
 * @param channel The desired channel number [1, 2].
 * @param enable Enable/disable option.
 */
void pna_enb_dis_tx_channel(struct ad9361_rf_phy *phy, uint8_t channel, uint8_t enable)
{
	if( channel==1 )
	{
		ad9361_en_dis_tx(phy, TX_1, enable);
	}
	else if( channel==2 )
	{
		ad9361_en_dis_tx(phy, TX_2, enable);
	}
}

void pna_get_command(char* command) {
	char		  received_char	= 0;
	unsigned char char_number	= 0;
	while( (received_char!='\n') && (received_char!='\r') ) {
		received_char=XUartPs_RecvByte(XPS_UART1_BASEADDR);
		if(received_char!=0 && received_char<128 && (received_char!='\r') && (received_char!='\n') ) {
			command[char_number++] = received_char;
			printf("%c",received_char);
			fflush(stdout);
		}
//		else if()
	}
	XUartPs_SendByte(XPS_UART1_BASEADDR,'\n');
	command[char_number] = '\0';
}

int pna_get_signal(char* awg_data, int samples) {
	char		  received_char	= 0;
	unsigned int  char_number	= 0;
	while(char_number < samples) {
		received_char=XUartPs_RecvByte(XPS_UART1_BASEADDR);
		awg_data[char_number++] = received_char;
//		else if()
	}
	received_char=XUartPs_RecvByte(XPS_UART1_BASEADDR);
	if(received_char != '<')
		return -1;
	received_char=XUartPs_RecvByte(XPS_UART1_BASEADDR);
	if(received_char != '>')
		return -1;
//	int16_t test = (int16_t) awg_data[4*512+1];
//	test = test & 0x0F;
//	test = test << 8;
//	test = test | awg_data[4*512];
//	printf("512.lsb: %u, msb: %u, data:%d , sample: %d\r\n", awg_data[4*10], awg_data[4*10+1], test, samples);
	return char_number;
}

void initAttenuation(XGpio *GpioOutputAtt) {
	/*
	 * Initialize the GPIO driver so that it's ready to use,
	 * specify the device ID that is generated in xparameters.h
	 */
	int Status = XGpio_Initialize(GpioOutputAtt, GPIO_ATT_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	/* Set the direction for all signals to be outputs */
	XGpio_SetDataDirection(GpioOutputAtt, 1, 0x0);

	XGpio_DiscreteWrite(GpioOutputAtt, 1, 0x00); // default value

}

void setAttenuation(XGpio *GpioOutputAtt, int channel, uint8_t att_int) {
	uint8_t gpio_spi_buf = 0x00;
	for (int i = 0; i < 7; i++) {
		gpio_spi_buf = 0x00;
		XGpio_DiscreteWrite(GpioOutputAtt, 1, gpio_spi_buf);
		usleep(10);
		// set serial in
		uint8_t att_bit = (att_int >> (6 - i)) & 1;
		// gpio_spi_buf &= 0x0B;
		gpio_spi_buf |= att_bit << SPI_GPIO_MOSI;
		XGpio_DiscreteWrite(GpioOutputAtt, 1, gpio_spi_buf);
		usleep(10);
		// toggle clk
		gpio_spi_buf ^= 1 << SPI_GPIO_SCK;
		XGpio_DiscreteWrite(GpioOutputAtt, 1, gpio_spi_buf);
		usleep(10);
	}

	gpio_spi_buf ^= 1 << SPI_GPIO_SCK;
	XGpio_DiscreteWrite(GpioOutputAtt, 1, gpio_spi_buf);
	usleep(10);

	if (channel == 0) {
		gpio_spi_buf |= 1 << SPI_GPIO_LE1;
		XGpio_DiscreteWrite(GpioOutputAtt, 1, gpio_spi_buf);
		usleep(10);
		gpio_spi_buf ^= 1 << SPI_GPIO_LE1;
		XGpio_DiscreteWrite(GpioOutputAtt, 1, gpio_spi_buf);
	} else {
		gpio_spi_buf |= 1 << SPI_GPIO_LE2;
		XGpio_DiscreteWrite(GpioOutputAtt, 1, gpio_spi_buf);
		usleep(10);
		gpio_spi_buf ^= 1 << SPI_GPIO_LE2;
		XGpio_DiscreteWrite(GpioOutputAtt, 1, gpio_spi_buf);
	}
	usleep(10);
}
