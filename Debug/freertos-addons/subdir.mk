################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
C:/git/breeze/firmware/freertos-addons/ccondition_variable.cpp \
C:/git/breeze/firmware/freertos-addons/cevent_groups.cpp \
C:/git/breeze/firmware/freertos-addons/cmem_pool.cpp \
C:/git/breeze/firmware/freertos-addons/cmutex.cpp \
C:/git/breeze/firmware/freertos-addons/cqueue.cpp \
C:/git/breeze/firmware/freertos-addons/cread_write_lock.cpp \
C:/git/breeze/firmware/freertos-addons/csemaphore.cpp \
C:/git/breeze/firmware/freertos-addons/ctasklet.cpp \
C:/git/breeze/firmware/freertos-addons/cthread.cpp \
C:/git/breeze/firmware/freertos-addons/ctickhook.cpp \
C:/git/breeze/firmware/freertos-addons/ctimer.cpp \
C:/git/breeze/firmware/freertos-addons/cworkqueue.cpp 

OBJS += \
./freertos-addons/ccondition_variable.o \
./freertos-addons/cevent_groups.o \
./freertos-addons/cmem_pool.o \
./freertos-addons/cmutex.o \
./freertos-addons/cqueue.o \
./freertos-addons/cread_write_lock.o \
./freertos-addons/csemaphore.o \
./freertos-addons/ctasklet.o \
./freertos-addons/cthread.o \
./freertos-addons/ctickhook.o \
./freertos-addons/ctimer.o \
./freertos-addons/cworkqueue.o 

CPP_DEPS += \
./freertos-addons/ccondition_variable.d \
./freertos-addons/cevent_groups.d \
./freertos-addons/cmem_pool.d \
./freertos-addons/cmutex.d \
./freertos-addons/cqueue.d \
./freertos-addons/cread_write_lock.d \
./freertos-addons/csemaphore.d \
./freertos-addons/ctasklet.d \
./freertos-addons/cthread.d \
./freertos-addons/ctickhook.d \
./freertos-addons/ctimer.d \
./freertos-addons/cworkqueue.d 


# Each subdirectory must supply rules for building sources it contributes
freertos-addons/ccondition_variable.o: C:/git/breeze/firmware/freertos-addons/ccondition_variable.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -I"c:\git\breeze\firmware\freertos-addons\include" -I../OSExtensions -I"../Library/CMSIS/Include" -I"../../ThirdParty/FreeRTOS/Source/portable/GCC/ARM_CM4F" -I"../../ThirdParty/FreeRTOS/Source/include" -I"../../ThirdParty/FreeRTOS/Demo/Common/include" -I"../../ThirdParty/FreeRTOS/Demo/Common/Minimal" -I"../Library/Device/Nuvoton/M480/Include" -I"../Library/StdDriver/inc" -I../source/hardware -std=gnu++11 -fabi-version=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

freertos-addons/cevent_groups.o: C:/git/breeze/firmware/freertos-addons/cevent_groups.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -I"c:\git\breeze\firmware\freertos-addons\include" -I../OSExtensions -I"../Library/CMSIS/Include" -I"../../ThirdParty/FreeRTOS/Source/portable/GCC/ARM_CM4F" -I"../../ThirdParty/FreeRTOS/Source/include" -I"../../ThirdParty/FreeRTOS/Demo/Common/include" -I"../../ThirdParty/FreeRTOS/Demo/Common/Minimal" -I"../Library/Device/Nuvoton/M480/Include" -I"../Library/StdDriver/inc" -I../source/hardware -std=gnu++11 -fabi-version=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

freertos-addons/cmem_pool.o: C:/git/breeze/firmware/freertos-addons/cmem_pool.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -I"c:\git\breeze\firmware\freertos-addons\include" -I../OSExtensions -I"../Library/CMSIS/Include" -I"../../ThirdParty/FreeRTOS/Source/portable/GCC/ARM_CM4F" -I"../../ThirdParty/FreeRTOS/Source/include" -I"../../ThirdParty/FreeRTOS/Demo/Common/include" -I"../../ThirdParty/FreeRTOS/Demo/Common/Minimal" -I"../Library/Device/Nuvoton/M480/Include" -I"../Library/StdDriver/inc" -I../source/hardware -std=gnu++11 -fabi-version=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

freertos-addons/cmutex.o: C:/git/breeze/firmware/freertos-addons/cmutex.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -I"c:\git\breeze\firmware\freertos-addons\include" -I../OSExtensions -I"../Library/CMSIS/Include" -I"../../ThirdParty/FreeRTOS/Source/portable/GCC/ARM_CM4F" -I"../../ThirdParty/FreeRTOS/Source/include" -I"../../ThirdParty/FreeRTOS/Demo/Common/include" -I"../../ThirdParty/FreeRTOS/Demo/Common/Minimal" -I"../Library/Device/Nuvoton/M480/Include" -I"../Library/StdDriver/inc" -I../source/hardware -std=gnu++11 -fabi-version=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

freertos-addons/cqueue.o: C:/git/breeze/firmware/freertos-addons/cqueue.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -I"c:\git\breeze\firmware\freertos-addons\include" -I../OSExtensions -I"../Library/CMSIS/Include" -I"../../ThirdParty/FreeRTOS/Source/portable/GCC/ARM_CM4F" -I"../../ThirdParty/FreeRTOS/Source/include" -I"../../ThirdParty/FreeRTOS/Demo/Common/include" -I"../../ThirdParty/FreeRTOS/Demo/Common/Minimal" -I"../Library/Device/Nuvoton/M480/Include" -I"../Library/StdDriver/inc" -I../source/hardware -std=gnu++11 -fabi-version=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

freertos-addons/cread_write_lock.o: C:/git/breeze/firmware/freertos-addons/cread_write_lock.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -I"c:\git\breeze\firmware\freertos-addons\include" -I../OSExtensions -I"../Library/CMSIS/Include" -I"../../ThirdParty/FreeRTOS/Source/portable/GCC/ARM_CM4F" -I"../../ThirdParty/FreeRTOS/Source/include" -I"../../ThirdParty/FreeRTOS/Demo/Common/include" -I"../../ThirdParty/FreeRTOS/Demo/Common/Minimal" -I"../Library/Device/Nuvoton/M480/Include" -I"../Library/StdDriver/inc" -I../source/hardware -std=gnu++11 -fabi-version=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

freertos-addons/csemaphore.o: C:/git/breeze/firmware/freertos-addons/csemaphore.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -I"c:\git\breeze\firmware\freertos-addons\include" -I../OSExtensions -I"../Library/CMSIS/Include" -I"../../ThirdParty/FreeRTOS/Source/portable/GCC/ARM_CM4F" -I"../../ThirdParty/FreeRTOS/Source/include" -I"../../ThirdParty/FreeRTOS/Demo/Common/include" -I"../../ThirdParty/FreeRTOS/Demo/Common/Minimal" -I"../Library/Device/Nuvoton/M480/Include" -I"../Library/StdDriver/inc" -I../source/hardware -std=gnu++11 -fabi-version=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

freertos-addons/ctasklet.o: C:/git/breeze/firmware/freertos-addons/ctasklet.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -I"c:\git\breeze\firmware\freertos-addons\include" -I../OSExtensions -I"../Library/CMSIS/Include" -I"../../ThirdParty/FreeRTOS/Source/portable/GCC/ARM_CM4F" -I"../../ThirdParty/FreeRTOS/Source/include" -I"../../ThirdParty/FreeRTOS/Demo/Common/include" -I"../../ThirdParty/FreeRTOS/Demo/Common/Minimal" -I"../Library/Device/Nuvoton/M480/Include" -I"../Library/StdDriver/inc" -I../source/hardware -std=gnu++11 -fabi-version=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

freertos-addons/cthread.o: C:/git/breeze/firmware/freertos-addons/cthread.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -I"c:\git\breeze\firmware\freertos-addons\include" -I../OSExtensions -I"../Library/CMSIS/Include" -I"../../ThirdParty/FreeRTOS/Source/portable/GCC/ARM_CM4F" -I"../../ThirdParty/FreeRTOS/Source/include" -I"../../ThirdParty/FreeRTOS/Demo/Common/include" -I"../../ThirdParty/FreeRTOS/Demo/Common/Minimal" -I"../Library/Device/Nuvoton/M480/Include" -I"../Library/StdDriver/inc" -I../source/hardware -std=gnu++11 -fabi-version=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

freertos-addons/ctickhook.o: C:/git/breeze/firmware/freertos-addons/ctickhook.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -I"c:\git\breeze\firmware\freertos-addons\include" -I../OSExtensions -I"../Library/CMSIS/Include" -I"../../ThirdParty/FreeRTOS/Source/portable/GCC/ARM_CM4F" -I"../../ThirdParty/FreeRTOS/Source/include" -I"../../ThirdParty/FreeRTOS/Demo/Common/include" -I"../../ThirdParty/FreeRTOS/Demo/Common/Minimal" -I"../Library/Device/Nuvoton/M480/Include" -I"../Library/StdDriver/inc" -I../source/hardware -std=gnu++11 -fabi-version=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

freertos-addons/ctimer.o: C:/git/breeze/firmware/freertos-addons/ctimer.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -I"c:\git\breeze\firmware\freertos-addons\include" -I../OSExtensions -I"../Library/CMSIS/Include" -I"../../ThirdParty/FreeRTOS/Source/portable/GCC/ARM_CM4F" -I"../../ThirdParty/FreeRTOS/Source/include" -I"../../ThirdParty/FreeRTOS/Demo/Common/include" -I"../../ThirdParty/FreeRTOS/Demo/Common/Minimal" -I"../Library/Device/Nuvoton/M480/Include" -I"../Library/StdDriver/inc" -I../source/hardware -std=gnu++11 -fabi-version=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

freertos-addons/cworkqueue.o: C:/git/breeze/firmware/freertos-addons/cworkqueue.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -I"c:\git\breeze\firmware\freertos-addons\include" -I../OSExtensions -I"../Library/CMSIS/Include" -I"../../ThirdParty/FreeRTOS/Source/portable/GCC/ARM_CM4F" -I"../../ThirdParty/FreeRTOS/Source/include" -I"../../ThirdParty/FreeRTOS/Demo/Common/include" -I"../../ThirdParty/FreeRTOS/Demo/Common/Minimal" -I"../Library/Device/Nuvoton/M480/Include" -I"../Library/StdDriver/inc" -I../source/hardware -std=gnu++11 -fabi-version=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


