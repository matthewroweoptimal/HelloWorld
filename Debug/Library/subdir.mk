################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Library/clk.c \
../Library/retarget.c \
../Library/sys.c \
../Library/uart.c 

OBJS += \
./Library/clk.o \
./Library/retarget.o \
./Library/sys.o \
./Library/uart.o 

C_DEPS += \
./Library/clk.d \
./Library/retarget.d \
./Library/sys.d \
./Library/uart.d 


# Each subdirectory must supply rules for building sources it contributes
Library/%.o: ../Library/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -IC:/git/nuvoton/helloworld/Library/CMSIS/Include -IC:/git/nuvoton/helloworld/Library/Device/Nuvoton/M480/Include -IC:/git/nuvoton/helloworld/Library/StdDriver/inc -IC:/git/nuvoton/helloworld/hardware -IC:/git/nuvoton/helloworld/OSExtensions -IC:/git/nuvoton/ThirdParty/FreeRTOS/Source/portable/GCC/ARM_CM4F -IC:/git/nuvoton/ThirdParty/FreeRTOS/Source/include -IC:/git/breeze/firmware/freertos-addons/include -IC:/git/nuvoton/helloworld/source/hardware -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


