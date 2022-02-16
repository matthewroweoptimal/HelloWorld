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
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -I"../Library/CMSIS/Include" -I"../../ThirdParty/FreeRTOS/Source/portable/GCC/ARM_CM4F" -I"../../ThirdParty/FreeRTOS/Source/include" -I"../../ThirdParty/FreeRTOS/Demo/Common/include" -I"../../ThirdParty/FreeRTOS/Demo/Common/Minimal" -I"C:\git\nuvoton\helloworld\User" -I"../Library/Device/Nuvoton/M480/Include" -I"../Library/StdDriver/inc" -I"c:\git\breeze\firmware\freertos-addons\include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


