///----------------------------------------------------------------------------------------
/// This module implements the Fatal Error handling for error conditions such as :
///    - Hard Faults,
///    - FreeRTOS task Stack Overflow detection
///    - FreeRTOS memory allocation failures
///
/// For HardFults, the failure location and register information is logged to SD card
/// with the LOG_FATAL() routine. The file 'fatal.txt' is used for storage.
///
/// The PC (Program Counter) and LR (Link Register - return from subroutine address)
/// values are the most important here for determining which bit of code was being executed
/// when the fault occurred.
///
/// Please check the PC and LR addresses against the 'Debug\Breeze.axf' file :
///     In IDE right click 'Binaries\Breeze.axf', select 'Binary Utilities'-->'Disassemble' and
///     examine the assembly code for the PC and LR addresses.
///     The other register values (r0,r1,r2,r3,r12 etc) will provide further context around the
///     failure condition (see assembly code).
///----------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include "MQX_To_FreeRTOS.h"

extern "C"
{
	// These are volatile to try and prevent the compiler/linker optimising them
	// away as the variables never actually get used.
	static volatile uint32_t r0;
	static volatile uint32_t r1;
	static volatile uint32_t r2;
	static volatile uint32_t r3;
	static volatile uint32_t r12;
	static volatile uint32_t lr; // Link register.
	static volatile uint32_t pc; // Program counter.
	static volatile uint32_t psr;// Program status register.
	static volatile __attribute__((unused)) uint32_t _CFSR ;
	static volatile __attribute__((unused)) uint32_t _HFSR ;
	static volatile __attribute__((unused)) uint32_t _DFSR ;
	static volatile __attribute__((unused)) uint32_t _AFSR ;
	static volatile __attribute__((unused)) uint32_t _BFAR ;
	static volatile __attribute__((unused)) uint32_t _MMAR ;
	static TaskStatus_t xTaskDetails;

	// Helper routine to print the FreeRTOS heap statistics.
	//-------------------------------------------------------
	void printFreeRTOSHeapStats()
	{
 		static xHeapStats stats = {0};
		vPortGetHeapStats( &stats );
		printf("\tFree = %zu, biggest = %zu, smallest = %zu\n",
			   stats.xAvailableHeapSpaceInBytes, stats.xSizeOfLargestFreeBlockInBytes, stats.xSizeOfSmallestFreeBlockInBytes );
	}

	// Called from FreeRTOS Stack overflow detection, runs in context of task switching, so can't dump to SD card here unfortunalely.
	//-------------------------------------------------------------------------------------------------------------------------------
	void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName)
	{
		taskDISABLE_INTERRUPTS();
		printf("StackOverflowHook : TaskHandle %p (%s)\n", xTask, pcTaskName );
#if defined(DEBUG)
		__BKPT(3);	// Stack will be available for examination under debugger
#endif
		while (1) { /* Loop forever */ }
	}

	// Called from FreeRTOS Heap management (heap_4.c) when memory request cannot be satisfied.
	// We may still have plenty of memory to satisfy smaller requests though, so possible to recover.
	//------------------------------------------------------------------------------------------------
	void vApplicationMallocFailedHook(size_t requestedSize)
	{
		printf("MallocFailedHook( %d bytes )\n", requestedSize);
		printFreeRTOSHeapStats();
	#if 0	// SC : Let the routine return and let LwIP (most likely culprit) try to recover from the memory allocation error.
		taskDISABLE_INTERRUPTS();
		while (1) { /* Loop forever */ }
	#endif
	}

	// Called from HardFault handler to dump fatal error information to SD card outside of interrupt contect. NEVER returns !
	//-----------------------------------------------------------------------------------------------------------------------
	void dumpFatalError( void )
	{
		printf("HardFault: (Examine PC & LR against '*.axf' disassembly)" );
		printf("Registers: PC=%p, LR=%p, psr=%p, r0=%p, r1=%p, r2=%p, r3=%p, r12=%p", pc, lr, psr, r0, r1, r2, r3, r12 );
		printf("Status Registers: CFSR=%p, HFSR=%p, DFSR=%p, AFSR=%p, MMAR=%p, BFAR=%p", _CFSR, _HFSR, _DFSR, _AFSR, _MMAR, _BFAR );
		printf("Task '%s': Stack base %p, highWaterMark = %d",
					xTaskDetails.pcTaskName, xTaskDetails.pxStackBase , xTaskDetails.usStackHighWaterMark );

		// Most useful thing is to check the PC and LR (return) addresses against 'Debug\Breeze.axf' file.
		// In IDE right click 'Binaries\Breeze.axf', select 'Binary Utilities'-->'Disassemble' and
		// examine assembly code for the PC and LR addresses, with r0,r1,r2,r3,r12 registers providing further context.
	#if defined(DEBUG)
		__BKPT(3);	// Registers will be available for examination under debugger
	#endif

		// TODO : Maybe call NVIC_SystemReset() here after a delay to allow system recovery ?
		while (1) { /* Loop forever */ }
	}

	// Called from HardFault_Handler() (Interrupt context) to access the saved register values on stack.
	//---------------------------------------------------------------------------------------------------
	void prvGetRegistersFromStack( uint32_t *pulFaultStackAddress )
	{
		r0 = pulFaultStackAddress[ 0 ];
		r1 = pulFaultStackAddress[ 1 ];
		r2 = pulFaultStackAddress[ 2 ];
		r3 = pulFaultStackAddress[ 3 ];
		r12 = pulFaultStackAddress[ 4 ];
		lr = pulFaultStackAddress[ 5 ];
		pc = pulFaultStackAddress[ 6 ];
		psr = pulFaultStackAddress[ 7 ];

		// Configurable Fault Status Register
		// Consists of MMSR, BFSR and UFSR
		_CFSR = (*((volatile unsigned long *)(0xE000ED28))) ;
		// Hard Fault Status Register
		_HFSR = (*((volatile unsigned long *)(0xE000ED2C))) ;
		// Debug Fault Status Register
		_DFSR = (*((volatile unsigned long *)(0xE000ED30))) ;
		// Auxiliary Fault Status Register
		_AFSR = (*((volatile unsigned long *)(0xE000ED3C))) ;
		// Read the Fault Address Registers. These may not contain valid values.
		// Check BFARVALID/MMARVALID to see if they are valid values
		// MemManage Fault Address Register
		_MMAR = (*((volatile unsigned long *)(0xE000ED34))) ;
		// Bus Fault Address Register
		_BFAR = (*((volatile unsigned long *)(0xE000ED38))) ;

		// Use the handle to obtain further information about the task.
		vTaskGetInfo( NULL, &xTaskDetails, pdTRUE, eInvalid );

		// NOTE : Dumping of Fatal Error information to the SD card does not work from an interrupt context (no SDHC interrupts active)
		//        Therefore, we have to trick the code to come out of interrupt handler to dump the Fatal Error informtion to SD card.
		//        We modify the PC and LR register values stored on stack to jump to dumpFatalError() routine, which will then
		//        perform the write to SD card from outwith the interrupt context.
		pulFaultStackAddress[5] = (uint32_t)dumpFatalError;	// Override LR register on stack
		pulFaultStackAddress[6] = (uint32_t)dumpFatalError;	// Override PC register on stack

		// >>>>>>>>>> EXECUTION PASSSES TO dumpFatalError() <<<<<<<<<<
	}

	// The prototype shows it is a naked function - in effect this is just an
	// assembly function.
	void HardFault_Handler( void ) __attribute__( ( naked ) );

	// The HardFault handler implementation : calls prvGetRegistersFromStack() function.
	//----------------------------------------------------------------------------------
	void HardFault_Handler(void)
	{
		__asm volatile
		(
			" tst lr, #4                                                \n"
			" ite eq                                                    \n"
			" mrseq r0, msp                                             \n"
			" mrsne r0, psp                                             \n"
			" ldr r1, [r0, #24]                                         \n"
			" ldr r2, handler2_address_const                            \n"
			" bx r2                                                     \n"
			" handler2_address_const: .word prvGetRegistersFromStack    \n"
		);
	}
} // extern "C"




