################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/git/nuvoton/SampleCode/FreeRTOS/ParTest.c 

CPP_SRCS += \
../User/Threads.cpp \
../User/main.cpp 

OBJS += \
./User/ParTest.o \
./User/Threads.o \
./User/main.o 

C_DEPS += \
./User/ParTest.d 

CPP_DEPS += \
./User/Threads.d \
./User/main.d 


# Each subdirectory must supply rules for building sources it contributes
User/ParTest.o: C:/git/nuvoton/SampleCode/FreeRTOS/ParTest.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -I"../Library/CMSIS/Include" -I"../../ThirdParty/FreeRTOS/Source/portable/GCC/ARM_CM4F" -I"../../ThirdParty/FreeRTOS/Source/include" -I"../../ThirdParty/FreeRTOS/Demo/Common/include" -I"../../ThirdParty/FreeRTOS/Demo/Common/Minimal" -I"C:\git\nuvoton\helloworld\User" -I"../Library/Device/Nuvoton/M480/Include" -I"../Library/StdDriver/inc" -I"c:\git\breeze\firmware\freertos-addons\include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

User/%.o: ../User/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -I"c:\git\breeze\firmware\freertos-addons\include" -I"../Library/CMSIS/Include" -I"../../ThirdParty/FreeRTOS/Source/portable/GCC/ARM_CM4F" -I"../../ThirdParty/FreeRTOS/Source/include" -I"../../ThirdParty/FreeRTOS/Demo/Common/include" -I"../../ThirdParty/FreeRTOS/Demo/Common/Minimal" -I"C:\git\nuvoton\helloworld\User" -I"../Library/Device/Nuvoton/M480/Include" -I"../Library/StdDriver/inc" -std=gnu++11 -fabi-version=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


