#ifndef VNA_H_
#define VNA_H_

#include <stdint.h>
#include "xgpio.h"
#include "ad9361.h"

#define GPIO_SW_P1 1<<11
#define GPIO_SW_CP_P1 1<<10
#define GPIO_SW_TX_A1 1<<9
#define GPIO_SW_TX_B1 1<<8

#define GPIO_SW_P2 1<<7
#define GPIO_SW_CP_P2 1<<6
#define GPIO_SW_TX_A2 1<<5
#define GPIO_SW_TX_B2 1<<4

#define GPIO_SW_LNA 1<<3
#define GPIO_SW_CPL 1<<2

#define GPIO_SW_12V 1<<1
#define GPIO_SW_5V 1<<0


#define GPIO_LED_DEVICE_ID	XPAR_AXI_GPIO_LED_DEVICE_ID
#define GPIO_SW_DEVICE_ID	XPAR_AXI_GPIO_SW_DEVICE_ID
#define GPIO_ATT_DEVICE_ID  XPAR_AXI_GPIO_ATT_DEVICE_ID
#define LED_DELAY	  10000000
#define SPI_GPIO_LE2  0
#define SPI_GPIO_LE1  1
#define SPI_GPIO_MOSI 2
#define SPI_GPIO_SCK  3

enum pna_sw_port {
	PORT_1=1,
	PORT_2=2
};

enum pna_ad9361_channel {
	CHANNEL_1=1,
	CHANNEL_2=2
};

enum pna_enable_disable {
	ENABLED=1,
	DISABLED=0
};

int32_t pna_init_gpio_sw(XGpio *GpioOutputSw,int channel);

void pna_sw_lna_pow12(XGpio *gpio_sw, uint8_t en, uint16_t *value);
void pna_sw_lna_pow5(XGpio *gpio_sw, uint8_t en, uint16_t *value);

void pna_s11(struct ad9361_rf_phy *phy, XGpio *gpio_sw, uint16_t *value);
void pna_s12(struct ad9361_rf_phy *phy, XGpio *gpio_sw, uint16_t *value);
void pna_s21(struct ad9361_rf_phy *phy, XGpio *gpio_sw, uint16_t *value);
void pna_s22(struct ad9361_rf_phy *phy, XGpio *gpio_sw, uint16_t *value);

uint16_t pna_sw_tx_a1(uint16_t value, uint8_t port);
uint16_t pna_sw_tx_b1(uint16_t value, uint8_t port);
uint16_t pna_sw_p1(uint16_t value, uint8_t port);
uint16_t pna_sw_cp_p1(uint16_t value, uint8_t port);
uint16_t pna_sw_tx_a2(uint16_t value, uint8_t port);
uint16_t pna_sw_tx_b2(uint16_t value, uint8_t port);
uint16_t pna_sw_p2(uint16_t value, uint8_t port);
uint16_t pna_sw_cp_p2(uint16_t value, uint8_t port);
uint16_t pna_sw_cpl(uint16_t value, uint8_t port);
uint16_t pna_sw_lna(uint16_t value, uint8_t port);
uint16_t pna_sw_config_port(uint16_t value, uint8_t port, uint16_t sw);

void pna_enb_dis_tx_channel(struct ad9361_rf_phy *phy, uint8_t channel, uint8_t enable);

void pna_get_command(char* command);
void initAttenuation();
void setAttenuation();

#endif
