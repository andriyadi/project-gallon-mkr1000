################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -f

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include libraries/Wire/subdir.mk
-include libraries/WiFi101/src/spi_flash/source/subdir.mk
-include libraries/WiFi101/src/socket/source/subdir.mk
-include libraries/WiFi101/src/driver/source/subdir.mk
-include libraries/WiFi101/src/common/source/subdir.mk
-include libraries/WiFi101/src/bus_wrapper/source/subdir.mk
-include libraries/WiFi101/src/bsp/source/subdir.mk
-include libraries/WiFi101/src/subdir.mk
-include libraries/SPI/subdir.mk
-include libraries/RTCZero/src/subdir.mk
-include libraries/PubSubClient/src/subdir.mk
-include libraries/ArduinoJson/src/Internals/subdir.mk
-include libraries/ArduinoJson/src/subdir.mk
-include core/variant/subdir.mk
-include core/core/avr/subdir.mk
-include core/core/USB/subdir.mk
-include core/core/subdir.mk
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
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
AR += \
arduino.ar \

ELF += \
SmartGallon_MKR1000.elf \

EEP += \
SmartGallon_MKR1000.eep \

FLASH_IMAGE += \
SmartGallon_MKR1000.bin \


# All Target
all: SmartGallon_MKR1000

# Tool invocations
SmartGallon_MKR1000: $(FLASH_IMAGE)
	@echo 'Building target: $@'
	@echo 'Printing size:'
	"/Users/andri/eclipse/cpp-mars/eclipseArduino.app/Contents/Eclipse/arduinoPlugin/tools/arduino/arm-none-eabi-gcc/4.8.3-2014q1/bin/arm-none-eabi-size" -A "/Users/andri/Projects/Arduino/EclipseArduino/SmartGallon_MKR1000/Release/SmartGallon_MKR1000.elf"
	@echo 'Finished building target: $@'
	@echo ' '

arduino.ar: $(AR_OBJ)
	@echo 'Starting archiver'
	"/Users/andri/eclipse/cpp-mars/eclipseArduino.app/Contents/Eclipse/arduinoPlugin/tools/arduino/arm-none-eabi-gcc/4.8.3-2014q1/bin/arm-none-eabi-ar" rcs  "/Users/andri/Projects/Arduino/EclipseArduino/SmartGallon_MKR1000/Release/arduino.ar"    arduino.ar $(AR_OBJ)
	@echo 'Finished building: $@'
	@echo ' '

SmartGallon_MKR1000.elf: $(LINK_OBJ) $(AR)
	@echo 'Starting combiner'
	"/Users/andri/eclipse/cpp-mars/eclipseArduino.app/Contents/Eclipse/arduinoPlugin/tools/arduino/arm-none-eabi-gcc/4.8.3-2014q1/bin/arm-none-eabi-gcc"  "-L/Users/andri/Projects/Arduino/EclipseArduino/SmartGallon_MKR1000/Release" -Os -Wl,--gc-sections -save-temps  "-T/Users/andri/eclipse/cpp-mars/eclipseArduino.app/Contents/Eclipse/arduinoPlugin/packages/arduino-mkr1000/hardware/samd/1.6.4-mkr02/variants/mkr1000/linker_scripts/gcc/flash_with_bootloader.ld" "-Wl,-Map,/Users/andri/Projects/Arduino/EclipseArduino/SmartGallon_MKR1000/Release/SmartGallon_MKR1000.map" --specs=nano.specs --specs=nosys.specs -mcpu=cortex-m0plus -mthumb -Wl,--cref -Wl,--check-sections -Wl,--gc-sections -Wl,--unresolved-symbols=report-all -Wl,--warn-common -Wl,--warn-section-align -o "/Users/andri/Projects/Arduino/EclipseArduino/SmartGallon_MKR1000/Release/SmartGallon_MKR1000.elf"   $(LINK_OBJ) $(AR)  -Wl,--start-group -lm   -Wl,--end-group
	@echo 'Finished building: $@'
	@echo ' '

SmartGallon_MKR1000.eep: $(ELF)
	@echo 'Create eeprom image'
	${A.RECIPE.OBJCOPY.EEP.PATTERN}
	@echo 'Finished building: $@'
	@echo ' '

SmartGallon_MKR1000.bin: $(EEP)
	@echo 'Create Flash image (ihex format)'
	"/Users/andri/eclipse/cpp-mars/eclipseArduino.app/Contents/Eclipse/arduinoPlugin/tools/arduino/arm-none-eabi-gcc/4.8.3-2014q1/bin/arm-none-eabi-objcopy" -O binary  "/Users/andri/Projects/Arduino/EclipseArduino/SmartGallon_MKR1000/Release/SmartGallon_MKR1000.elf" "/Users/andri/Projects/Arduino/EclipseArduino/SmartGallon_MKR1000/Release/SmartGallon_MKR1000.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C_UPPER_DEPS)$(PDE_DEPS)$(C_DEPS)$(AR)$(EEP)$(FLASH_IMAGE)$(CC_DEPS)$(AR_OBJ)$(C++_DEPS)$(LINK_OBJ)$(CXX_DEPS)$(ASM_DEPS)$(INO_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS)$(ELF)$(CPP_DEPS) SmartGallon_MKR1000
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
