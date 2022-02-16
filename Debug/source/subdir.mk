################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../source/Threads.cpp \
../source/main.cpp 

OBJS += \
./source/Threads.o \
./source/main.o 

CPP_DEPS += \
./source/Threads.d \
./source/main.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -I"c:\git\breeze\firmware\freertos-addons\include" -I"../Library/CMSIS/Include" -I"../../ThirdParty/FreeRTOS/Source/portable/GCC/ARM_CM4F" -I"../../ThirdParty/FreeRTOS/Source/include" -I"../../ThirdParty/FreeRTOS/Demo/Common/include" -I"../../ThirdParty/FreeRTOS/Demo/Common/Minimal" -I"../Library/Device/Nuvoton/M480/Include" -I"../Library/StdDriver/inc" -I../source/hardware -std=gnu++11 -fabi-version=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


