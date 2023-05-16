/**************************************************************************//**
 * @file     BOOTLOADER.c
 * @version  V1.00
 * @brief    Runs the BOOTLOADER for CDDLive Projects in LDROM flash area.
 *           Only 4kB LDROM Flash is available, so code is minimised to the
 *           absolutely necessary.
 *           Looks for a valid firmware image in the SPI Flash area,
 *           if found, it is programmed into the APROM area, CRC checked
 *           and the APP is then launched.
 *           The APP clears the 'OLY_UPGRADE_THE_FIRMWARE' word in the
 *           OLY_REGION header, so if APP fails to launch the BOOTLOADER
 *           will retry the firmware upgrade once more.
 *
 *           The Nuvoton programming tools are the 'get out of jail' fallback
 *           if the APP somehow loses the ability to upgrade the SPI Flash firmware.
 *
*****************************************************************************/
// SPI Flash Layout is as follows :
//
//      - First 1 MiByte are allocated to CDDLive Parameters.
//      - Second 1 MiByte are allocated to the APP firmware image (see below)
//
//    +--------------------------+  0x0010 0000
//    |    OLY_REGION header     |
//    |       (0x40 bytes)       |
//    |                          |
//    |      'upgradeFwCode'     | (Signal to BOOTLOADER to upgrade if set to 'OLY_UPGRADE_THE_FIRMWARE')
//    |                          |    (Cleared by APP on successful launch - ensure Upgrade.h in line with APP)
//    |                          |
//    +--------------------------+  0x0010 0040
//    |                          |
//    |                          |
//    +--------------------------+  0x0010 4040 (Start of APP fw, comes after space for 16k bootloader in APROM)
//    |                          |
//    |    APP Firmware Area     |
//    |                          |
//    |                          |
//    |                          |
//    |                          |
//    |                          |
//    +--------------------------+
//

//#include <stdio.h>
#include "NuMicro.h"
#include "spi_flash_nu.h"
#include "flash_params.h"
#include "crc16.h"
#include "Upgrade.h"

// BOOTLOADER Version Information :
const char VERSION_MAJOR = '1';
const char VERSION_MINOR = '0';


static uint32_t	g_retryCount = 0;
const uint32_t FLASH_WRITE_RETRIES = 3;		// Retry 3 times before giving up on a sector write

static volatile uint8_t s_fwUpdateLedPattern = 0x01;	// Holds pattern for LEDs during programming


OLY_REGION  g_firmwareHeader;
uint32_t    g_flashSector[P_SECTOR_SIZE_U32];   // Can hold one 4kB sector data


#define GPIOB_PIN_BASE (GPIO_PIN_DATA_BASE+(0x40*(1)))
#define GPIOC_PIN_BASE (GPIO_PIN_DATA_BASE+(0x40*(2)))

#define CDDP_PANEL_LED1(value)      (*(volatile uint32_t *)(GPIOB_PIN_BASE + (3<<2)) = !value)
#define CDDP_PANEL_LED2(value)      (*(volatile uint32_t *)(GPIOB_PIN_BASE + (2<<2)) = !value)
#define CDDP_PANEL_LED3(value)      (*(volatile uint32_t *)(GPIOC_PIN_BASE + (12<<2)) = !value)
#define CDDP_PANEL_LED4(value)      (*(volatile uint32_t *)(GPIOC_PIN_BASE + (11<<2)) = !value)

#define DELAY_COUNT		10000000000


//---------------------------------------------------------------------------
// Set the specified LED to the specified output state
//---------------------------------------------------------------------------
static void setNextLed(void)
{
	if(s_fwUpdateLedPattern & 1) CDDP_PANEL_LED1(1); else CDDP_PANEL_LED1(0);
	if(s_fwUpdateLedPattern & 2) CDDP_PANEL_LED2(1); else CDDP_PANEL_LED2(0);
	if(s_fwUpdateLedPattern & 4) CDDP_PANEL_LED3(1); else CDDP_PANEL_LED3(0);
	if(s_fwUpdateLedPattern & 8) CDDP_PANEL_LED4(1); else CDDP_PANEL_LED4(0);

	s_fwUpdateLedPattern = s_fwUpdateLedPattern << 1;
	if (s_fwUpdateLedPattern == (1 << 4))
		s_fwUpdateLedPattern = 0x01;	// Back to LED1
}

//---------------------------------------------------------------------------
// Hardware initialisation
//---------------------------------------------------------------------------
void SYS_Init(void)
{
    /* Protected registers should be unlocked already */
    /* Set XT1_OUT(PF.2) and XT1_IN(PF.3) to input mode */
    PF->MODE &= ~(GPIO_MODE_MODE2_Msk | GPIO_MODE_MODE3_Msk);

    /* Enable External XTAL (4~24 MHz) */
    CLK->PWRCTL |= CLK_PWRCTL_HXTEN_Msk;

    /* Waiting for 12MHz clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);

    /* Switch HCLK clock source to HXT */
    CLK->CLKSEL0 = (CLK->CLKSEL0 & ~CLK_CLKSEL0_HCLKSEL_Msk) | CLK_CLKSEL0_HCLKSEL_HXT;

    /* Set core clock as PLL_CLOCK from PLL */
    CLK->PLLCTL = CLK_PLLCTL_192MHz_HIRC;

    /* Waiting for PLL clock ready */
    CLK_WaitClockReady(CLK_STATUS_PLLSTB_Msk);

    /* Switch HCLK clock source to HXT */
    CLK->CLKSEL0 = (CLK->CLKSEL0 & ~CLK_CLKSEL0_HCLKSEL_Msk) | CLK_CLKSEL0_HCLKSEL_PLL;

    /* Set both PCLK0 and PCLK1 as HCLK/2 */
    CLK->PCLKDIV = CLK_PCLKDIV_APB0DIV_DIV2 | CLK_PCLKDIV_APB1DIV_DIV2;


    //---- UART0 SETUP ----
    CLK->APBCLK0 |= CLK_APBCLK0_UART0CKEN_Msk;	/* Enable UART clock */

    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk);
    SYS->GPB_MFPH |= (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);


    //---- SPI Flash Hardware Initialisation ----
    CLK_EnableModuleClock(SPIM_MODULE);

    /* Init SPIM multi-function pins, MOSI(PC.0), MISO(PC.1), CLK(PC.2), SS(PC.3), D3(PC.4), and D2(PC.5) */
    SYS->GPC_MFPL &= ~(SYS_GPC_MFPL_PC0MFP_Msk | SYS_GPC_MFPL_PC1MFP_Msk | SYS_GPC_MFPL_PC2MFP_Msk |
                       SYS_GPC_MFPL_PC3MFP_Msk | SYS_GPC_MFPL_PC4MFP_Msk | SYS_GPC_MFPL_PC5MFP_Msk);
    SYS->GPC_MFPL |= SYS_GPC_MFPL_PC0MFP_SPIM_MOSI | SYS_GPC_MFPL_PC1MFP_SPIM_MISO |
                     SYS_GPC_MFPL_PC2MFP_SPIM_CLK | SYS_GPC_MFPL_PC3MFP_SPIM_SS |
                     SYS_GPC_MFPL_PC4MFP_SPIM_D3 | SYS_GPC_MFPL_PC5MFP_SPIM_D2;
    PC->SMTEN |= GPIO_SMTEN_SMTEN2_Msk;

    /* Set SPIM I/O pins as high slew rate up to 80 MHz. */
    PC->SLEWCTL = (PC->SLEWCTL & 0xFFFFF000) |
                  (0x1<<GPIO_SLEWCTL_HSREN0_Pos) | (0x1<<GPIO_SLEWCTL_HSREN1_Pos) |
                  (0x1<<GPIO_SLEWCTL_HSREN2_Pos) | (0x1<<GPIO_SLEWCTL_HSREN3_Pos) |
                  (0x1<<GPIO_SLEWCTL_HSREN4_Pos) | (0x1<<GPIO_SLEWCTL_HSREN5_Pos);

    /* some SPIM init */
    SPIM_SET_CLOCK_DIVIDER(1);        /* Set SPIM clock as HCLK divided by 2 */

    SPIM_SET_RXCLKDLY_RDDLYSEL(0);    /* Insert 0 delay cycle. Adjust the sampling clock of received data to latch the correct data. */
    SPIM_SET_RXCLKDLY_RDEDGE();       /* Use SPI input clock rising edge to sample received data. */

    SPIM_SET_DCNUM(8);                /* Set 8 dummy cycle. */


    //---- Enable LED output GPIO pins ----
    /* LEDs (PB.3, PB.2, PC.12, PC.11) */
    PB->MODE = (PB->MODE & ~(GPIO_MODE_MODE3_Msk | GPIO_MODE_MODE2_Msk)) |
               (GPIO_MODE_OUTPUT << GPIO_MODE_MODE3_Pos) |
               (GPIO_MODE_OUTPUT << GPIO_MODE_MODE2_Pos);  // Set to output mode
#if ROTATING_LED_INDICATOR
    // Also initialise LED3 and LED4 if rotating LED pattern configured
    PC->MODE = (PC->MODE & ~(GPIO_MODE_MODE12_Msk | GPIO_MODE_MODE11_Msk)) |
               (GPIO_MODE_OUTPUT << GPIO_MODE_MODE12_Pos) |
               (GPIO_MODE_OUTPUT << GPIO_MODE_MODE11_Pos);  // Set to output mode
#endif
}

//---------------------------------------------------------------------------
// Serial UART0 initialisation
//---------------------------------------------------------------------------
void UART0_Init(void)
{
    /* Configure UART0 and set UART0 baud rate */
    UART0->LINE = 0x3;
    UART0->BAUD = 0x30000066;
}

//---------------------------------------------------------------------------
// Send a single character to UART0
// @param ch    Character to output to serial
//---------------------------------------------------------------------------
static void PutChar(char ch)
{
	while (UART0->FIFOSTS & UART_FIFOSTS_TXFULL_Msk);

	UART0->DAT = (int)ch;
	if(ch == '\n')
	{
		while (UART0->FIFOSTS & UART_FIFOSTS_TXFULL_Msk);
		UART0->DAT = '\r';
	}
}

// Define 'NO_SERIAL_OUTPUT' in 'flash_params.h' to control serial information messages (code size reduction)
#if NO_DEBUG_MSG_OUTPUT
	#define PutString(x)	// To save space, take away all serial information messages
#else
	void PutString(char *str)
	{
		while (*str != '\0')
		{
			PutChar(*str++);
		}
	}
#endif

//---------------------------------------------------------------------------
// Minimal HardFault Handler (Space limitaions)
//---------------------------------------------------------------------------
void Hard_Fault_Handler(uint32_t stack[])
{
    PutString("Hard Fault\n");
    while(1);
}

//---------------------------------------------------------------------------
// BOOTLOADER main entry point
//---------------------------------------------------------------------------
int main()
{
	uint64_t delayCounter = 0;

	while(delayCounter<DELAY_COUNT) {
		delayCounter++;
	}


    SYS_UnlockReg();                   /* Unlock protected registers */
    SYS_Init();                        /* Init System, IP clock and multi-function I/O */

    UART0_Init();                      /* Initialize UART0 */

    PutString("\nBOOTLOADER Version ");
    PutChar( VERSION_MAJOR );
    PutChar( '.' );
    PutChar( VERSION_MINOR );
    PutChar( '\n' );

    // Check for Firmware image in SPI Flash
    // (Note : Validation has been carried out by the APP, so just basic sanity checking here (as we have limited 4kB LDROM))
    if ( spi_flash_init() == SPI_FLASH_LOAD_OK )
    {
		spi_flash_readFwHeader( &g_firmwareHeader );

		if ( (g_firmwareHeader.upgradeFwCode == OLY_UPGRADE_THE_FIRMWARE) &&
			 (g_firmwareHeader.length > 0) && (g_firmwareHeader.length != 0xFFFFFFFF) )
		{	// Check the validity of the firmware image in SPI flash by Verifying CRC
			unsigned char *pCrcPtr = (unsigned char *)g_firmwareHeader.address;
			unsigned short crcCalc;
			REGION_CRC rgnCrc;

			Crc16Init( &rgnCrc );
			Crc16Update( &rgnCrc, pCrcPtr, g_firmwareHeader.length, CRC_SPI_FLASH );
			Crc16Finalize( &rgnCrc, &crcCalc );
			if ( g_firmwareHeader.crc == crcCalc )
			{
				PutString("CRC MATCH\n");

				// Program APROM flash with new Firmware
				FMC_Open();                        /*--- Enable FMC ISP function ---*/
				FMC_ENABLE_AP_UPDATE();            /*--- Enable APROM update. ---*/

				uint32_t flashAddress = g_firmwareHeader.address;
				for ( uint32_t sector = 0; sector <= g_firmwareHeader.length/P_SECTOR_SIZE; )
				{
					uint32_t spiFlashOffset = flashAddress;
					spi_flash_readFwSector( spiFlashOffset, g_flashSector );

					if ( !flash_writeChunk( flashAddress, g_flashSector, P_SECTOR_SIZE_U32 ))
	#if INCLUDE_FLASH_WRITE_RETRIES
					{	// Failed
						// Can take this out to save space. If flashing fails a reboot will attempt the update again.
						if ( g_retryCount == 0 )
						{
							PutChar('X');
						}

						g_retryCount++;
						if ( g_retryCount > FLASH_WRITE_RETRIES )
						{	// Move on to next sector
							g_retryCount = 0;
							flashAddress += P_SECTOR_SIZE;
							sector++;
						}
					}
					else
					{	// Success
						flashAddress += P_SECTOR_SIZE;
						sector++;
						PutChar('.');
					}
	#else // No Retries
					{	// Failed
						PutChar('X');
					}
					else
					{	// Success
						PutChar('.');
					}
					flashAddress += P_SECTOR_SIZE;
					sector++;
	#endif
					setNextLed();	// Animate LED indication
				}

				Crc16Init( &rgnCrc );
				Crc16Update( &rgnCrc, pCrcPtr, g_firmwareHeader.length, CRC_APROM_FLASH );
				Crc16Finalize( &rgnCrc, &crcCalc );
				if ( g_firmwareHeader.crc != crcCalc )
				{
					PutString("APROM CRC MIS-MATCH\n");
					while (1);
				}

				FMC_DISABLE_AP_UPDATE();           /*--- Disable APROM update. ---*/
				FMC_Close();                       /*--- Disable FMC ISP function ---*/
			}
			else
				PutString("Firmware CRC ERROR\n");
		}
		else
		{
			PutString("No new firmware detected\n");
		}
    }

    PutString("\n\nLaunching APP...\n");
    while (!(UART0->FIFOSTS & UART_FIFOSTS_TXEMPTY_Msk));	// wait until UART3 TX FIFO is empty

    // NOTE :
    // Before changing VECMAP, user MUST disable all interrupts.
    //
    __disable_irq();							// Just incase
    FMC_Open();                        			// Enable FMC ISP function
    FMC_SetVectorPageAddr(APROM_APP_LOCATION);	// Vector remap APROM page 0 to start of APP at 0x4000.
    if (g_FMC_i32ErrCode != 0)
    {
        PutString("Vector Re-map Fail\n");
        while (1);	// Not good, avoid resetting into incorrect vector table.
    }
    FMC_Close();

    //---- Reset CPU, which should now launch directly into APP ----
    SYS_ResetCPU();

    while (1);	// Should never get here
}

