/*
 * board.h
 *
 *  Created on: 16 Feb 2022
 *      Author: Matthew.Rowe
 */

#ifndef HARDWARE_BOARD_H_
#define HARDWARE_BOARD_H_
#include "NuMicro.h"
#include "Pin.h"

#define HIGH 0
#define LOW 1


static void initEthernetHardware(void)
{
	  /* Set GPB multi-function pins for UART0 RXD and TXD */
	    SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk);
	    SYS->GPB_MFPH |= (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);

		    // Configure RMII pins
		    SYS->GPA_MFPL |= SYS_GPA_MFPL_PA6MFP_EMAC_RMII_RXERR | SYS_GPA_MFPL_PA7MFP_EMAC_RMII_CRSDV;
		    SYS->GPC_MFPL |= SYS_GPC_MFPL_PC6MFP_EMAC_RMII_RXD1 | SYS_GPC_MFPL_PC7MFP_EMAC_RMII_RXD0;
		    SYS->GPC_MFPH |= SYS_GPC_MFPH_PC8MFP_EMAC_RMII_REFCLK;
		    SYS->GPE_MFPH |= SYS_GPE_MFPH_PE8MFP_EMAC_RMII_MDC |
		                     SYS_GPE_MFPH_PE9MFP_EMAC_RMII_MDIO |
		                     SYS_GPE_MFPH_PE10MFP_EMAC_RMII_TXD0 |
		                     SYS_GPE_MFPH_PE11MFP_EMAC_RMII_TXD1 |
		                     SYS_GPE_MFPH_PE12MFP_EMAC_RMII_TXEN;

		    // Enable high slew rate on all RMII TX output pins
		    PE->SLEWCTL = (GPIO_SLEWCTL_HIGH << GPIO_SLEWCTL_HSREN10_Pos) |
		                  (GPIO_SLEWCTL_HIGH << GPIO_SLEWCTL_HSREN11_Pos) |
		                  (GPIO_SLEWCTL_HIGH << GPIO_SLEWCTL_HSREN12_Pos);


}


enum LED_t {
	GREEN_LED1,
	GREEN_LED2,
	NUM_LEDS
};

static Pin ledPins[NUM_LEDS] = {
	{GPIOH_PIN_BASE, 0},	// led1 on H0
	{GPIOH_PIN_BASE, 1},	// led2 on H1
};

enum BUTTON_t {
	SW2,
	SW3,
	NUM_SWITCHES
};

static Pin buttonPins[NUM_SWITCHES] = {
	{GPIOG_PIN_BASE, 15},	// sw2 on G15
	{GPIOF_PIN_BASE, 11}	// sw3 on F11
};



enum GPIO_t {
	SHARC_RESET,
	ULTIMO_RESET,
	ADC_RESET,
	DAC_RESET,
	AMP1_MUTE,
	AMP2_MUTE,
	AMP_STANDBY,
	AMP_BTL4R_CTL,
	AMP1_TEMP_SEL,
	AMP2_TEMP_SEL,
	NUM_GPIO
};

static Pin gpioPins[NUM_GPIO] = {
	{GPIOH_PIN_BASE, 2},	// sharc reset on H2
	{GPIOB_PIN_BASE, 15},	// ultimo reset on B15;
	{GPIOD_PIN_BASE, 3},	// sharc reset on D3
	{GPIOE_PIN_BASE, 7},	// DAC reset on E7
	{GPIOG_PIN_BASE, 0},	// AMP1_MUTE on G0
	{GPIOG_PIN_BASE, 2},	// AMP1_MUTE on G2
	{GPIOG_PIN_BASE, 1},	// AMP_STANDBY on G1
	{GPIOG_PIN_BASE, 3},	// AMP_BTL4R_CTL on G3
	{GPIOF_PIN_BASE, 5},	// AMP1_TEMP_SEL on F5
	{GPIOF_PIN_BASE, 6},	// AMP2_TEMP_SEL on F6
};

class Leds
{
	Leds();

public:
	static void setLed(LED_t led, bool state) {
		ledPins[led].Write(state ? 0:1);
	};

	static void toggleLed(LED_t led) {
		ledPins[led].Write( ledPins[led].Read() ? 0:1 );
	}
};

class Gpio
{
	Gpio();

public:
	static void setGpio(GPIO_t device, bool state) {
		gpioPins[device].Write(state ? 0:1);
	};

};

class Buttons
{
	Buttons();

public:
	static bool readButton(BUTTON_t button) {
		return !buttonPins[button].Read();
	}
};


#endif //HARDWARE_BOARD_H_
