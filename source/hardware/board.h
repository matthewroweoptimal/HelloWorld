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
	RED_LED,
	YELLOW_LED,
	GREEN_LED,
	NUM_LEDS
};

static Pin ledPins[NUM_LEDS] = {
	{GPIOH_PIN_BASE, 0},	// red led on H0
	{GPIOH_PIN_BASE, 1},	// yellow led on H1
	{GPIOH_PIN_BASE, 2}		// green led on H2
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

class Buttons
{
	Buttons();

public:
	static bool readButton(BUTTON_t button) {
		return !buttonPins[button].Read();
	}
};

#endif /* HARDWARE_BOARD_H_ */
