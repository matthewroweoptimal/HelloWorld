################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lwip/src/api/api_lib.c \
../lwip/src/api/api_msg.c \
../lwip/src/api/err.c \
../lwip/src/api/if_api.c \
../lwip/src/api/netbuf.c \
../lwip/src/api/netdb.c \
../lwip/src/api/netifapi.c \
../lwip/src/api/sockets.c \
../lwip/src/api/tcpip.c 

OBJS += \
./lwip/src/api/api_lib.o \
./lwip/src/api/api_msg.o \
./lwip/src/api/err.o \
./lwip/src/api/if_api.o \
./lwip/src/api/netbuf.o \
./lwip/src/api/netdb.o \
./lwip/src/api/netifapi.o \
./lwip/src/api/sockets.o \
./lwip/src/api/tcpip.o 

C_DEPS += \
./lwip/src/api/api_lib.d \
./lwip/src/api/api_msg.d \
./lwip/src/api/err.d \
./lwip/src/api/if_api.d \
./lwip/src/api/netbuf.d \
./lwip/src/api/netdb.d \
./lwip/src/api/netifapi.d \
./lwip/src/api/sockets.d \
./lwip/src/api/tcpip.d 


# Each subdirectory must supply rules for building sources it contributes
lwip/src/api/%.o: ../lwip/src/api/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -DUSE_RTOS=1 -DLWIP_DEBUG=1 -DLWIP_SOCKET_POLL=0 -IC:/git/nuvoton/helloworld/source -IC:/git/breeze/firmware/freertos/freertos_kernel/include -IC:/git/breeze/firmware/freertos/freertos_kernel/portable -IC:/git/breeze/firmware/freertos/freertos_kernel/portable/GCC/ARM_CM4F -IC:/git/breeze/firmware/freertos/freertos_kernel/portable/MemMang -IC:/git/nuvoton/helloworld/Library/CMSIS/Include -IC:/git/nuvoton/helloworld/Library/Device/Nuvoton/M480/Include -IC:/git/nuvoton/helloworld/Library/StdDriver/inc -IC:/git/nuvoton/helloworld/OSExtensions -IC:/git/breeze/firmware/freertos-addons/include -IC:/git/nuvoton/helloworld/source/hardware -IC:/git/breeze/firmware/freertos/freertos_kernel -Ic:/git/nuvoton/helloworld/lwip/port/include -Ic:/git/nuvoton/helloworld/lwip/port -Ic:/git/breeze/firmware/lwip/src -Ic:/git/breeze/firmware/lwip/src/include -Ic:/git/nuvoton/helloworld/lwip/port/netif -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


