################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include User/Drivers/subdir.mk
-include User/Boot/subdir.mk
-include User/subdir.mk
-include Library/Library/subdir.mk
-include Library/subdir.mk
-include CMSIS/CMSIS/GCC/subdir.mk
-include CMSIS/CMSIS/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := BOOTLOADER
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
BOOTLOADER.hex \

SECONDARY_SIZE += \
BOOTLOADER.siz \


# All Target
all: BOOTLOADER.elf secondary-outputs

# Tool invocations
BOOTLOADER.elf: $(OBJS) $(USER_OBJS) makefile objects.mk $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross ARM GNU C Linker'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -T "C:\DEVELOPMENT\MatthewRoweOptimal\BOOTLOADER\GCC\BOOTLOADER.ld" -Xlinker --gc-sections -Wl,-Map,"BOOTLOADER.map" --specs=nano.specs -o "BOOTLOADER.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

BOOTLOADER.hex: BOOTLOADER.elf makefile objects.mk $(OPTIONAL_TOOL_DEPS)
	@echo 'Invoking: Cross ARM GNU Create Flash Image'
	arm-none-eabi-objcopy -O ihex "BOOTLOADER.elf"  "BOOTLOADER.hex"
	@echo 'Finished building: $@'
	@echo ' '

BOOTLOADER.siz: BOOTLOADER.elf makefile objects.mk $(OPTIONAL_TOOL_DEPS)
	@echo 'Invoking: Cross ARM GNU Print Size'
	arm-none-eabi-size --format=berkeley "BOOTLOADER.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(SECONDARY_FLASH)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_UPPER_DEPS)$(C_DEPS) BOOTLOADER.elf
	-@echo ' '

post-build:
	-@echo 'Creating .hex and .bin files under BOOTLOADER directory'
	-arm-none-eabi-objcopy -O binary BOOTLOADER.elf ../../../BOOTLOADER.bin  &  arm-none-eabi-objcopy -O ihex BOOTLOADER.elf ../../../BOOTLOADER.hex
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents post-build

-include ../makefile.targets
