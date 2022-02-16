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

#define GPIOA_PIN_BASE (GPIO_PIN_DATA_BASE+(0x40*(0)))
#define GPIOB_PIN_BASE (GPIO_PIN_DATA_BASE+(0x40*(1)))
#define GPIOC_PIN_BASE (GPIO_PIN_DATA_BASE+(0x40*(2)))
#define GPIOD_PIN_BASE (GPIO_PIN_DATA_BASE+(0x40*(3)))
#define GPIOE_PIN_BASE (GPIO_PIN_DATA_BASE+(0x40*(4)))
#define GPIOF_PIN_BASE (GPIO_PIN_DATA_BASE+(0x40*(5)))
#define GPIOG_PIN_BASE (GPIO_PIN_DATA_BASE+(0x40*(6)))
#define GPIOH_PIN_BASE (GPIO_PIN_DATA_BASE+(0x40*(7)))

class Pin
{
public:
	Pin(uint32_t port, uint32_t pin)
	  : m_port(port), m_pin(pin << 2) {}

	uint32_t Read() {
		uint32_t uiRead = *(volatile uint32_t *)(m_port + m_pin);
		return uiRead;
	}

	void Write(uint8_t output) {
		*(volatile uint32_t *)(m_port + m_pin) = output;
	}

	uint32_t 	m_port;
	uint32_t 	m_pin;
};

#endif /* HARDWARE_PIN_H_ */
