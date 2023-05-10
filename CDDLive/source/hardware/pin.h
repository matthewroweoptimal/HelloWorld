/*
 * pin.h
 *
 *  Created on: 16 Feb 2022
 *      Author: Matthew.Rowe
 */

#ifndef HARDWARE_PIN_H_
#define HARDWARE_PIN_H_

// pin helper class
#include "M480.h"
#include "gpio.h"
#include "board.h"

#define GPIOA_PIN_BASE (GPIO_PIN_DATA_BASE+(0x40*(0)))
#define GPIOB_PIN_BASE (GPIO_PIN_DATA_BASE+(0x40*(1)))
#define GPIOC_PIN_BASE (GPIO_PIN_DATA_BASE+(0x40*(2)))
#define GPIOD_PIN_BASE (GPIO_PIN_DATA_BASE+(0x40*(3)))
#define GPIOE_PIN_BASE (GPIO_PIN_DATA_BASE+(0x40*(4)))
#define GPIOF_PIN_BASE (GPIO_PIN_DATA_BASE+(0x40*(5)))
#define GPIOG_PIN_BASE (GPIO_PIN_DATA_BASE+(0x40*(6)))
#define GPIOH_PIN_BASE (GPIO_PIN_DATA_BASE+(0x40*(7)))

//Temp monitor pins
#define AMP1_TEMP_MON_PIN  			PB0
#define AMP2_TEMP_MON_PIN  			PB1

class Pin
{
public:
	Pin(uint32_t port, uint32_t pin)
	  : m_port(port),
		m_pin(pin << 2) {}

	uint32_t Read() {
		uint32_t uiRead = *(volatile uint32_t *)(m_port + m_pin);
		return uiRead;
	}

	void Write(uint8_t output) {
		*(volatile uint32_t *)(m_port + m_pin) = output;
	}

//protected:
	uint32_t 	m_port;
	uint32_t 	m_pin;
};

enum LED_t {
	GREEN_LED1,
	GREEN_LED2,
	NUM_LEDS
};

static Pin ledPins[NUM_LEDS] = {
	{GPIOH_PIN_BASE, 0},	// led1 on H0
	{GPIOH_PIN_BASE, 1},	// led2 on H1
};


enum GPIO_t {
	SHARC_RESET,
	ULTIMO_RESET,
	ADC_RESET,
	DAC_RESET,
	AMP1_DISABLE_MUTE_CNTRL,
	AMP2_DISABLE_MUTE_CNTRL,
	AMP1_2_STANDBY_CNTRL,
	BTL4R_CONTROL,
	AMP1_TEMP_VAC_SEL,
	AMP2_TEMP_VAC_SEL,
	FLASH_SPI_SS,
	SW2,
	AMP1_CH1_CLIP,
	AMP1_CH2_CLIP,
	AMP2_CH1_CLIP,
	AMP2_CH2_CLIP,
	AMP1_PROTECT,
	AMP2_PROTECT,
	DANTE_MUTE,
	SHARC_SPI_READY,
	DISPLAY_D2,
	DISPLAY_D3,
	DISPLAY_D4,
	DISPLAY_D5,
	DISPLAY_D6,
	LOGO_LED_RED,
	LOGO_LED_AMBER,
	LOGO_LED_WHITE,		
	FAN_CONTROL,
	NUM_GPIO
};

static Pin gpioPins[NUM_GPIO] = {
	{GPIOH_PIN_BASE, 2},	// sharc reset on H2
	{GPIOB_PIN_BASE, 15},	// ultimo reset on B15;
	{GPIOD_PIN_BASE, 3},	// sharc reset on D3
	{GPIOE_PIN_BASE, 7},	// DAC reset on E7
	{GPIOG_PIN_BASE, 0},	// AMP1_MUTE on G0
	{GPIOG_PIN_BASE, 2},	// AMP2_MUTE on G2
	{GPIOG_PIN_BASE, 1},	// AMP_STANDBY on G1
	{GPIOG_PIN_BASE, 3},	// AMP_BTL4R_CTL on G3
	{GPIOF_PIN_BASE, 5},	// AMP1_TEMP_SEL on F5
	{GPIOF_PIN_BASE, 6},	// AMP2_TEMP_SEL on F6
	{GPIOC_PIN_BASE, 3},	// FLASH_SPI_SS on C3
	{GPIOB_PIN_BASE, 5},	// Membrane Switch B5
#if REV004_PIN_CHANGES
	{GPIOA_PIN_BASE, 9},	// AMP1_CH1_CLIP on A9
	{GPIOF_PIN_BASE, 10},	// AMP1_CH2_CLIP on F10
	{GPIOF_PIN_BASE, 11},	// AMP2_CH1_CLIP on F11
	{GPIOG_PIN_BASE, 4},	// AMP2_CH2_CLIP on G4
	{GPIOA_PIN_BASE, 11},	// AMP1_PROTECT on A11
	{GPIOA_PIN_BASE, 10},	// AMP2_PROTECT on A10
#else
	{GPIOF_PIN_BASE, 9},	// AMP1_CH1_CLIP on F9
	{GPIOF_PIN_BASE, 10},	// AMP1_CH2_CLIP on F10
	{GPIOF_PIN_BASE, 11},	// AMP2_CH1_CLIP on F11
	{GPIOG_PIN_BASE, 4},	// AMP2_CH2_CLIP on G4
	{GPIOF_PIN_BASE, 7},	// AMP1_PROTECT on F7
	{GPIOF_PIN_BASE, 8},	// AMP2_PROTECT on F8
#endif
	{GPIOC_PIN_BASE, 14},	// DANTE_MUTE on C14 - ULTIMO_PDP_UNLOCKED
	{GPIOH_PIN_BASE, 3},	// SHARC_SPI_READY on H3
	{GPIOB_PIN_BASE, 3},	// DISPLAY_D2 on B3
	{GPIOB_PIN_BASE, 2},	// DISPLAY_D3 on B2
	{GPIOC_PIN_BASE, 12},	// DISPLAY_D4 on C12
	{GPIOC_PIN_BASE, 11},	// DISPLAY_D5 on C11
	{GPIOC_PIN_BASE, 10},	// DISPLAY_D6 on C10
	{GPIOA_PIN_BASE, 1},	// LOGO_LED_RED on A1
	{GPIOA_PIN_BASE, 2},	// LOGO_LED_AMBER on A2 - Dummy Mapping for LOGO_LED_AMBER (currently spare on Nuvoton hardware)
	{GPIOA_PIN_BASE, 3},	// LOGO_LED_WHITE on A3 - Dummy Mapping for LOGO_LED_WHITE (currently spare on Nuvoton hardware)
	{GPIOA_PIN_BASE, 0},	// FAN_CONTROL on A0
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
	static void setGpio(GPIO_t device, uint8_t state) {
		gpioPins[device].Write(state);
	};

};




/* Adding driver functions used in the original CDD/OLY code
**         GPIO_DRV_Init            - void GPIO_DRV_Init(const gpio_input_pin_user_config_t * inputPins,const...
**         GPIO_DRV_InputPinInit    - void GPIO_DRV_InputPinInit(const gpio_input_pin_user_config_t * inputPin);
**         GPIO_DRV_OutputPinInit   - void GPIO_DRV_OutputPinInit(const gpio_output_pin_user_config_t * outputPin);
**         GPIO_DRV_GetPinDir       - gpio_pin_direction_t GPIO_DRV_GetPinDir(uint32_t pinName);
**         GPIO_DRV_SetPinDir       - void GPIO_DRV_SetPinDir(uint32_t pinName,gpio_pin_direction_t direction);
**         GPIO_DRV_WritePinOutput  - void GPIO_DRV_WritePinOutput(uint32_t pinName,uint32_t output);
**         GPIO_DRV_SetPinOutput    - void GPIO_DRV_SetPinOutput(uint32_t pinName);
**         GPIO_DRV_ClearPinOutput  - void GPIO_DRV_ClearPinOutput(uint32_t pinName);
**         GPIO_DRV_TogglePinOutput - void GPIO_DRV_TogglePinOutput(uint32_t pinName);
**         GPIO_DRV_ReadPinInput    - uint32_t GPIO_DRV_ReadPinInput(uint32_t pinName);
**         GPIO_DRV_ClearPinIntFlag - void GPIO_DRV_ClearPinIntFlag(uint32_t pinName); */

#define GPIO_DRV_WritePinOutput(pinName,output)		gpioPins[pinName].Write(output)
#define GPIO_DRV_SetPinOutput(pinName)				gpioPins[pinName].Write(HIGH)
#define GPIO_DRV_ClearPinOutput(pinName)			gpioPins[pinName].Write(LOW)
#define GPIO_DRV_ReadPinInput(pinName)				*(volatile uint32_t *)(gpioPins[pinName].m_port + gpioPins[pinName].m_pin)

#endif /* HARDWARE_PIN_H_ */
