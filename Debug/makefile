################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include freertos/src/subdir.mk
-include example/src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: Master_v7.axf

# Tool invocations
Master_v7.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -nostdlib -L"/home/juan/LPCXpresso/workspace/lpc_chip_175x_6x/Debug" -L"/home/juan/LPCXpresso/workspace/lpc_board_nxp_lpcxpresso_1769/Debug" -L"/home/juan/LPCXpresso/workspace/lpc_chip_175x_6x/libs" -Xlinker -Map="Master_v7.map" -Xlinker --gc-sections -mcpu=cortex-m3 -mthumb -T "Master_v7_Debug.ld" -o "Master_v7.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) Master_v7.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "Master_v7.axf"; # arm-none-eabi-objcopy -v -O binary "Master_v7.axf" "Master_v7.bin" ; # checksum -p LPC1769 -d "Master_v7.bin";
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
