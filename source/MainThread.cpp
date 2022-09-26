/*
 * MainThread.cpp
 *
 *  Created on: 17 Aug 2022
 *      Author: Stuart Campbell
 */
#include "MainThread.h"
#include "MQX_To_FreeRTOS.h"
#include "spi_sharc.h"
#include "oly_sharc_boot.h"
#include "board.h"
#include "i2c.h"

#define DAC_I2C_ADDR				0x30

#define ID1 						0x00010000
#define ID2 						0x00010001
#define ID3 						0x00010002

#define DAC_SYS_CLK__REG			0x00010006
#define ASP_FS_REG					0x0001000B
#define POW_DOWN_REG				0x00020000
#define VOL_B_REG					0x00090001
#define VOL_A_REG					0x00090002
#define AN_OUT_CTL_REG				0x00080000
#define ASP_CLK_CNFG_REG			0x00040018

#define DAC_SYS_CLK 				0x00
#define ASP_FS						0x02
#define POWER_DOWN_CTL				0xAE
#define VOL_B						0x00
#define VOL_A						0x00
#define AN_OUT_CTL					0x31
//#define ASP_CLK_CNFG				0x0D	//flips left and right channels
#define ASP_CLK_CNFG				0x0C	//normal operation


MainThread::MainThread(uint16_t usStackDepth, UBaseType_t uxPriority, SemaphoreHandle_t& semMainThreadComplete)
     : Thread("MainThread", usStackDepth, uxPriority),
       _semMainThreadComplete(semMainThreadComplete)
{
    printf("MainThread created\n");  
}

void I2C2_Init(void)
{
    /* Open I2C module and set bus clock */
    I2C_Open(I2C2, 100000);

    /* Get I2C0 Bus Clock */
    printf("I2C2 clock %d Hz\n", I2C_GetBusClockFreq(I2C2));
}

void MainThread::Run()
{
    printf("MainThread HW setup\n");
		
    // Below code needs to happen after the Scheduler is started as it relies on FreeRTOS functionality (delays, semaphores etc)
    // TODO : Possibly can move to MainThread and stop other threads running until completed ?
    
    /* TODO : Bring ultimo & DSP out of reset */
    //GPIO_DRV_SetPinOutput(ULTIMO_FACTORY);
    _time_delay(50);
    //GPIO_DRV_SetPinOutput(SHARC_RESET);
    Gpio::setGpio(SHARC_RESET,HIGH);
    
    /* Load DSP boot code */
    _time_delay(50);
    DSPI_DRV_MasterTransferBlocking(FSL_SPI_SHARC, OLY_sharc_boot, NULL, OLY_sharc_boot_size, 5000);
    
    /* Set DSP SPI driver to control mode */
    _time_delay(250);
    dsp_sharc_SetMode_Host();
    
    /* Bring the ADC & DAC out of reset */
    Gpio::setGpio(ADC_RESET,HIGH);
    Gpio::setGpio(DAC_RESET,HIGH);

    I2C2_Init();
    /* Setup the DAC */
    /* TODO : tidy this up! */
    uint8_t l=0;
    printf("TXing 1st byte\n");
    l=I2C_WriteByteThreeRegsPlus(I2C2, DAC_I2C_ADDR, POW_DOWN_REG, POWER_DOWN_CTL);
    printf("POWER_DOWN_CTL Reg Mesg sent... %d was returned\n", l);
	_time_delay(10);
    l=I2C_WriteByteThreeRegsPlus(I2C2, DAC_I2C_ADDR, DAC_SYS_CLK__REG, DAC_SYS_CLK);
    printf("DAC_SYS_CLK Reg Mesg sent... %d was returned\n", l);
	_time_delay(10);
    printf("TXing 3rd byte... %d bytes transmitted\n", l);;
    l=I2C_WriteByteThreeRegsPlus(I2C2, DAC_I2C_ADDR, ASP_FS_REG, ASP_FS);
    printf("ASP_FS Reg Mesg sent... %d was returned\n", l);
	_time_delay(10);
    printf("TXing 4th byte... %d bytes transmitted\n", l);
    l=I2C_WriteByteThreeRegsPlus(I2C2, DAC_I2C_ADDR, VOL_B_REG, VOL_B);
    printf("VOL_B Reg Mesg sent... %d was returned\n", l);
	_time_delay(10);
    printf("TXing 5th byte... %d bytes transmitted\n", l);
    l=I2C_WriteByteThreeRegsPlus(I2C2, DAC_I2C_ADDR, VOL_A_REG, VOL_A);
    printf("VOL_A Reg Mesg sent... %d was returned\n", l);
	_time_delay(10);
    printf("TXing 6th byte... %d bytes transmitted\n", l);
    l=I2C_WriteByteThreeRegsPlus(I2C2, DAC_I2C_ADDR, AN_OUT_CTL_REG, AN_OUT_CTL);
    printf("Out Cnt Reg Mesg sent... %d was returned\n", l);
	_time_delay(10);
    printf("TXing 7th byte... %d bytes transmitted\n", l);
    l=I2C_WriteByteThreeRegsPlus(I2C2, DAC_I2C_ADDR, ASP_CLK_CNFG_REG, ASP_CLK_CNFG);
    printf("ASP_CNFG Reg Mesg sent... %d was returned\n", l);

//let's read back some registers to check we are setting the thing up correctly!
    uint8_t readByte = I2C_ReadByteThreeRegs(I2C2, DAC_I2C_ADDR, ID1);
	printf("1st ID byte read... 0x%02x\n", readByte);
	_time_delay(10);
    readByte = I2C_ReadByteThreeRegs(I2C2, DAC_I2C_ADDR, ID2);
	printf("2nd ID byte read... 0x%02x\n", readByte);
	_time_delay(10);
    readByte = I2C_ReadByteThreeRegs(I2C2, DAC_I2C_ADDR, ID3);
	printf("3rd ID byte read... 0x%02x\n", readByte);
	_time_delay(10);
    readByte = I2C_ReadByteThreeRegs(I2C2, DAC_I2C_ADDR, DAC_SYS_CLK__REG);
	printf("sys clk reg... 0x%02x\n", readByte);
	_time_delay(10);
    readByte = I2C_ReadByteThreeRegs(I2C2, DAC_I2C_ADDR, ASP_FS_REG);
	printf("asp fs reg... 0x%02x\n", readByte);
	_time_delay(10);
    readByte = I2C_ReadByteThreeRegs(I2C2, DAC_I2C_ADDR, POW_DOWN_REG);
	printf("power down reg... 0x%02x\n", readByte);
	_time_delay(10);
    readByte = I2C_ReadByteThreeRegs(I2C2, DAC_I2C_ADDR, VOL_B_REG);
	printf("vol b reg... 0x%02x\n", readByte);
	_time_delay(10);
    readByte = I2C_ReadByteThreeRegs(I2C2, DAC_I2C_ADDR, VOL_A_REG);
	printf("vol a reg... 0x%02x\n", readByte);
	_time_delay(10);
    readByte = I2C_ReadByteThreeRegs(I2C2, DAC_I2C_ADDR, AN_OUT_CTL_REG);
	printf("an ctl reg... 0x%02x\n", readByte);
	_time_delay(10);
    readByte = I2C_ReadByteThreeRegs(I2C2, DAC_I2C_ADDR, ASP_CLK_CNFG_REG);
	printf("asp cnfg reg... 0x%02x\n", readByte);

	/* unmnute the amps! */
    Gpio::setGpio(AMP1_MUTE,LOW);
    Gpio::setGpio(AMP2_MUTE,LOW);
    Gpio::setGpio(AMP_STANDBY,LOW);

    // Signal that other Threads can now start
    xSemaphoreGive(_semMainThreadComplete);
    printf("MainThread Complete\n");
    
    // go to sleep
    Suspend();
}
