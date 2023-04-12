// config.h

#ifndef __CONFIG_H__
#define __CONFIG_H__

// Compile Time Switches - Not all switch combinations are valid!

// 'OS' Specific
#define NO_BOOT_LOADER 0    // 0 -> assume boot run at startup, 1 -> assume no boot loader
#define NO_BIST        0    // 0 -> assume BIST run at startup, 1 -> assume no BIST

// NOTE:  This project requires SDRAM to be configured at link time!
//        This means that either the boot loader and BIST must be run
//        or init code must be prepended to the loader file using the Project Options -> Load -> Options tab.

// General
#define SPEAK_ETHERNET 1    // 0 -> don't enable ethernet,  1 -> build using lwIP (and VDK)
#define VERBOSE        0    // 0 -> suppress debugging I/O, 1 -> activate debugging I/O

#if SPEAK_ETHERNET
#define USE_VDK 1
#else
#define USE_VDK 0   // 0 -> don't use the VDK RTOS, 1 -> use the VDK RTOS
#endif

#if USE_VDK
#define ISR_MACRO ADI_INT_HANDLER
#else
#define ISR_MACRO EX_INTERRUPT_HANDLER
#endif

#if VERBOSE
#define SUCCINCT 1
#else
#define SUCCINCT 0  // 0 -> suppress message handling I/O, 1 -> activate message handling I/O
#endif

// Clock
#define FAST_CORE_300_100       0                   // Maximize the core clock frequency at the expense of lower system bus frequency
#define FAST_SYSTEM_250_125     1                   // Maximize the system bus frequency at the expense of lower core clock frequency
#define CORE_SYSTEM_CLOCK_COMBO FAST_SYSTEM_250_125

#if (FAST_SYSTEM_250_125==CORE_SYSTEM_CLOCK_COMBO)

// CCLK = 250 MHz, SCLK = 125 MHz (assuming 25 MHz oscillator)
#define PLL_MSEL 10
#define PLL_SSEL 2

// CCLK dependent
#define CORE_TIMER_PERIOD_1MS 250000

// SCLK dependent
#define SDRAM_Micron_MT48LC32M16A2_SDRRC 0x03C8     // RDIV =  968 = ((125 MHz * 64 ms)/8192 rows) - ((44 ns + 20 ns) / 8 ns)
#define SDRAM_Micron_MT48LC8M16A2_SDRRC  0x0799     // RDIV = 1945 = ((125 MHz * 64 ms)/4096 rows) - ((44 ns + 20 ns) / 8 ns)
#define SDRAM_SDGCTL                     0x0491999D // CL = 3
#define TIMER_WIDTH_1US 125
#define TIMER_WIDTH_1MS 125000
#define UART_DL_115200  68      // 114889.706 (-0.3%)

#else   // default to fast core

// CCLK = 300 MHz, SCLK = 100 MHz (assuming 25 MHz oscillator)
#define PLL_MSEL 12
#define PLL_SSEL 3

// CCLK dependent
#define CORE_TIMER_PERIOD_1MS 300000

// SCLK dependent
#define SDRAM_Micron_MT48LC32M16A2_SDRRC 0x0306     // RDIV =  774 = ((100 MHz * 64 ms)/8192 rows) - ((44 ns + 20 ns) / 10 ns)
#define SDRAM_Micron_MT48LC8M16A2_SDRRC  0x0614     // RDIV = 1556 = ((100 MHz * 64 ms)/4096 rows) - ((44 ns + 20 ns) / 10 ns)
#define SDRAM_SDGCTL                     0x04911149 // CL = 2
#define TIMER_WIDTH_1US 100
#define TIMER_WIDTH_1MS 100000
#define UART_DL_115200  54      // 115740.740 (+0.5%)

#endif

#endif // __CONFIG_H__

