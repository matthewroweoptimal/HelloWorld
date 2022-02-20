################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lwip/src/core/ipv4/autoip.c \
../lwip/src/core/ipv4/dhcp.c \
../lwip/src/core/ipv4/etharp.c \
../lwip/src/core/ipv4/icmp.c \
../lwip/src/core/ipv4/igmp.c \
../lwip/src/core/ipv4/ip4.c \
../lwip/src/core/ipv4/ip4_addr.c \
../lwip/src/core/ipv4/ip4_frag.c 

OBJS += \
./lwip/src/core/ipv4/autoip.o \
./lwip/src/core/ipv4/dhcp.o \
./lwip/src/core/ipv4/etharp.o \
./lwip/src/core/ipv4/icmp.o \
./lwip/src/core/ipv4/igmp.o \
./lwip/src/core/ipv4/ip4.o \
./lwip/src/core/ipv4/ip4_addr.o \
./lwip/src/core/ipv4/ip4_frag.o 

C_DEPS += \
./lwip/src/core/ipv4/autoip.d \
./lwip/src/core/ipv4/dhcp.d \
./lwip/src/core/ipv4/etharp.d \
./lwip/src/core/ipv4/icmp.d \
./lwip/src/core/ipv4/igmp.d \
./lwip/src/core/ipv4/ip4.d \
./lwip/src/core/ipv4/ip4_addr.d \
./lwip/src/core/ipv4/ip4_frag.d 


# Each subdirectory must supply rules for building sources it contributes
lwip/src/core/ipv4/%.o: ../lwip/src/core/ipv4/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -DUSE_RTOS=1 -DLWIP_SOCKET_POLL=0 -IC:/git/breeze/firmware/freertos/freertos_kernel/include -IC:/git/breeze/firmware/freertos/freertos_kernel/portable -IC:/git/breeze/firmware/freertos/freertos_kernel/portable/GCC/ARM_CM4F -IC:/git/breeze/firmware/freertos/freertos_kernel/portable/MemMang -IC:/git/nuvoton/helloworld/Library/CMSIS/Include -IC:/git/nuvoton/helloworld/Library/Device/Nuvoton/M480/Include -IC:/git/nuvoton/helloworld/Library/StdDriver/inc -IC:/git/nuvoton/helloworld/OSExtensions -IC:/git/breeze/firmware/freertos-addons/include -IC:/git/nuvoton/helloworld/source/hardware -IC:/git/nuvoton/helloworld/source -IC:/git/breeze/firmware/freertos/freertos_kernel -Ic:/git/nuvoton/helloworld/lwip/port/include -Ic:/git/nuvoton/helloworld/lwip/port -Ic:/git/breeze/firmware/lwip/src -Ic:/git/breeze/firmware/lwip/src/include -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


