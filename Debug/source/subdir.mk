################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../source/MonitoringTimer.cpp \
../source/Threads.cpp \
../source/main.cpp 

OBJS += \
./source/MonitoringTimer.o \
./source/Threads.o \
./source/main.o 

CPP_DEPS += \
./source/MonitoringTimer.d \
./source/Threads.d \
./source/main.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -DUSE_RTOS=1 -DLWIP_SOCKET_POLL=0 -Ic:/git/breeze/firmware/lwip/src/include -IC:/git/breeze/firmware/freertos/freertos_kernel/portable -IC:/git/breeze/firmware/freertos/freertos_kernel/portable/GCC/ARM_CM4F -IC:/git/breeze/firmware/freertos/freertos_kernel/portable/MemMang -IC:/git/nuvoton/helloworld/Library/CMSIS/Include -IC:/git/nuvoton/helloworld/Library/Device/Nuvoton/M480/Include -IC:/git/nuvoton/helloworld/Library/StdDriver/inc -IC:/git/nuvoton/helloworld/OSExtensions -IC:/git/breeze/firmware/freertos-addons/include -IC:/git/nuvoton/helloworld/source/hardware -IC:/git/nuvoton/helloworld/source -IC:/git/breeze/firmware/freertos/freertos_kernel -IC:/git/breeze/firmware/freertos/freertos_kernel/include -Ic:/git/breeze/firmware/lwip/ -Ic:/git/nuvoton/helloworld/lwip/port -std=gnu++11 -fabi-version=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


