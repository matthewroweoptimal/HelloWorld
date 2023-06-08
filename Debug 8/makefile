################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include source/Drivers/subdir.mk
-include source/Dante/src/subdir.mk
-include source/Dante/subdir.mk
-include source/Comm/subdir.mk
-include source/subdir.mk
-include lwip/src/netif/ppp/polarssl/subdir.mk
-include lwip/src/netif/ppp/subdir.mk
-include lwip/src/netif/subdir.mk
-include lwip/src/core/ipv6/subdir.mk
-include lwip/src/core/ipv4/subdir.mk
-include lwip/src/core/subdir.mk
-include lwip/src/apps/mdns/subdir.mk
-include lwip/src/api/subdir.mk
-include lwip/port/netif/subdir.mk
-include lwip/port/subdir.mk
-include freertos_kernel/portable/MemMang/subdir.mk
-include freertos_kernel/portable/GCC/ARM_CM4F/subdir.mk
-include freertos_kernel/portable/Common/subdir.mk
-include freertos_kernel/subdir.mk
-include freertos-addons/subdir.mk
-include Library/StdDriver/src/subdir.mk
-include Library/Device/Nuvoton/GCC/subdir.mk
-include Library/Device/Nuvoton/subdir.mk
-include Common/Mandolin/subdir.mk
-include Common/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := CDDLive
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
CDDLive.hex \

SECONDARY_SIZE += \
CDDLive.siz \


# All Target
all: CDDLive.elf secondary-outputs

# Tool invocations
CDDLive.elf: $(OBJS) $(USER_OBJS) makefile objects.mk $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross ARM GNU C++ Linker'
	arm-none-eabi-g++ -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -T "C:\Users\Iain.Quarmby\OneDrive - Focusrite Group\Documents\GitHub\HelloWorld\CMSIS\GCC\gcc_arm.ld" -Xlinker --gc-sections -Wl,-Map,"CDDLive.map" -o "CDDLive.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

CDDLive.hex: CDDLive.elf makefile objects.mk $(OPTIONAL_TOOL_DEPS)
	@echo 'Invoking: Cross ARM GNU Create Flash Image'
	arm-none-eabi-objcopy -O ihex "CDDLive.elf"  "CDDLive.hex"
	@echo 'Finished building: $@'
	@echo ' '

CDDLive.siz: CDDLive.elf makefile objects.mk $(OPTIONAL_TOOL_DEPS)
	@echo 'Invoking: Cross ARM GNU Print Size'
	arm-none-eabi-size --format=berkeley "CDDLive.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(CC_DEPS)$(C++_DEPS)$(OBJS)$(C_UPPER_DEPS)$(CXX_DEPS)$(SECONDARY_FLASH)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_UPPER_DEPS)$(C_DEPS)$(CPP_DEPS) CDDLive.elf
	-@echo ' '

post-build:
	-@echo 'Pack Binary into Firmware file with attached REGION header'
	-arm-none-eabi-objcopy -O binary "CDDLive.elf" "CDDLive.bin" & sleep 10 & "C:/Users/Iain.Quarmby/OneDrive - Focusrite Group/Documents/GitHub/HelloWorld/TOOLS/OlyPack/Release/OlyPack.exe" -L CDDLive.log -I CDDLive.bin -O CDDLive.Fw.bin -V 1.0.0 -S 0x0 -T 0x0 -P 0x28000
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents post-build

-include ../makefile.targets
