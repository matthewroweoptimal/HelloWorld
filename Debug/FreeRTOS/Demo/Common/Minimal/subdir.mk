################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/git/nuvoton/ThirdParty/FreeRTOS/Demo/Common/Minimal/PollQ.c \
C:/git/nuvoton/ThirdParty/FreeRTOS/Demo/Common/Minimal/QueueSet.c \
C:/git/nuvoton/ThirdParty/FreeRTOS/Demo/Common/Minimal/flash.c 

OBJS += \
./FreeRTOS/Demo/Common/Minimal/PollQ.o \
./FreeRTOS/Demo/Common/Minimal/QueueSet.o \
./FreeRTOS/Demo/Common/Minimal/flash.o 

C_DEPS += \
./FreeRTOS/Demo/Common/Minimal/PollQ.d \
./FreeRTOS/Demo/Common/Minimal/QueueSet.d \
./FreeRTOS/Demo/Common/Minimal/flash.d 


# Each subdirectory must supply rules for building sources it contributes
FreeRTOS/Demo/Common/Minimal/PollQ.o: C:/git/nuvoton/ThirdParty/FreeRTOS/Demo/Common/Minimal/PollQ.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -IC:/git/NUworkspace/HelloWorld/Library/CMSIS/Include -IC:/git/NUworkspace/HelloWorld/Library/Device/Nuvoton/M480/Include -IC:/git/NUworkspace/HelloWorld/Library/StdDriver/inc -IC:/git/nuvoton/ThirdParty/FreeRTOS/Source/include -IC:/git/nuvoton/ThirdParty/FreeRTOS/Demo/Common/include -IC:/git/NUworkspace/HelloWorld -IC:/git/nuvoton/ThirdParty/lwIP/src/include -IC:/git/NUworkspace/HelloWorld/User -IC:/git/nuvoton/ThirdParty/FreeRTOS/Source/portable/GCC/ARM_CM4F -IC:/git/breeze/firmware/freertos-addons/include -IC:/git/nuvoton/SampleCode/FreeRTOS -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

FreeRTOS/Demo/Common/Minimal/QueueSet.o: C:/git/nuvoton/ThirdParty/FreeRTOS/Demo/Common/Minimal/QueueSet.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -IC:/git/NUworkspace/HelloWorld/Library/CMSIS/Include -IC:/git/NUworkspace/HelloWorld/Library/Device/Nuvoton/M480/Include -IC:/git/NUworkspace/HelloWorld/Library/StdDriver/inc -IC:/git/nuvoton/ThirdParty/FreeRTOS/Source/include -IC:/git/nuvoton/ThirdParty/FreeRTOS/Demo/Common/include -IC:/git/NUworkspace/HelloWorld -IC:/git/nuvoton/ThirdParty/lwIP/src/include -IC:/git/NUworkspace/HelloWorld/User -IC:/git/nuvoton/ThirdParty/FreeRTOS/Source/portable/GCC/ARM_CM4F -IC:/git/breeze/firmware/freertos-addons/include -IC:/git/nuvoton/SampleCode/FreeRTOS -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

FreeRTOS/Demo/Common/Minimal/flash.o: C:/git/nuvoton/ThirdParty/FreeRTOS/Demo/Common/Minimal/flash.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -IC:/git/NUworkspace/HelloWorld/Library/CMSIS/Include -IC:/git/NUworkspace/HelloWorld/Library/Device/Nuvoton/M480/Include -IC:/git/NUworkspace/HelloWorld/Library/StdDriver/inc -IC:/git/nuvoton/ThirdParty/FreeRTOS/Source/include -IC:/git/nuvoton/ThirdParty/FreeRTOS/Demo/Common/include -IC:/git/NUworkspace/HelloWorld -IC:/git/nuvoton/ThirdParty/lwIP/src/include -IC:/git/NUworkspace/HelloWorld/User -IC:/git/nuvoton/ThirdParty/FreeRTOS/Source/portable/GCC/ARM_CM4F -IC:/git/breeze/firmware/freertos-addons/include -IC:/git/nuvoton/SampleCode/FreeRTOS -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


