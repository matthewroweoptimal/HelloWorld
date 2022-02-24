################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lwip/port/sys_arch.c \
../lwip/port/time_stamp.c 

OBJS += \
./lwip/port/sys_arch.o \
./lwip/port/time_stamp.o 

C_DEPS += \
./lwip/port/sys_arch.d \
./lwip/port/time_stamp.d 


# Each subdirectory must supply rules for building sources it contributes
lwip/port/%.o: ../lwip/port/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -DUSE_RTOS=1 -DLWIP_DEBUG=1 -DLWIP_SOCKET_POLL=0 -IC:/git/nuvoton/helloworld/source -IC:/git/breeze/firmware/freertos/freertos_kernel/include -IC:/git/breeze/firmware/freertos/freertos_kernel/portable -IC:/git/breeze/firmware/freertos/freertos_kernel/portable/GCC/ARM_CM4F -IC:/git/breeze/firmware/freertos/freertos_kernel/portable/MemMang -IC:/git/nuvoton/helloworld/Library/CMSIS/Include -IC:/git/nuvoton/helloworld/Library/Device/Nuvoton/M480/Include -IC:/git/nuvoton/helloworld/Library/StdDriver/inc -IC:/git/nuvoton/helloworld/OSExtensions -IC:/git/breeze/firmware/freertos-addons/include -IC:/git/nuvoton/helloworld/source/hardware -IC:/git/breeze/firmware/freertos/freertos_kernel -Ic:/git/nuvoton/helloworld/lwip/port/include -Ic:/git/nuvoton/helloworld/lwip/port -Ic:/git/breeze/firmware/lwip/src -Ic:/git/breeze/firmware/lwip/src/include -Ic:/git/nuvoton/helloworld/lwip/port/netif -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


