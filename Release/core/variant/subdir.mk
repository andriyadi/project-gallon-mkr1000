################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
/Users/andri/eclipse/cpp-mars/eclipseArduino.app/Contents/Eclipse/arduinoPlugin/packages/arduino-mkr1000/hardware/samd/1.6.4-mkr02/variants/mkr1000/variant.cpp 

AR_OBJ += \
./core/variant.cpp.o 

CPP_DEPS += \
./core/variant.cpp.d 


# Each subdirectory must supply rules for building sources it contributes
core/variant.cpp.o: /Users/andri/eclipse/cpp-mars/eclipseArduino.app/Contents/Eclipse/arduinoPlugin/packages/arduino-mkr1000/hardware/samd/1.6.4-mkr02/variants/mkr1000/variant.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"/Users/andri/eclipse/cpp-mars/eclipseArduino.app/Contents/Eclipse/arduinoPlugin/tools/arduino/arm-none-eabi-gcc/4.8.3-2014q1/bin/arm-none-eabi-g++" -mcpu=cortex-m0plus -mthumb -c -g -Os -std=gnu++11 -ffunction-sections -fdata-sections -fno-threadsafe-statics -nostdlib --param max-inline-insns-single=500 -fno-rtti -fno-exceptions -MMD -DF_CPU=48000000L -DARDUINO=10606 -DARDUINO_SAMD_MKR1000 -DARDUINO_ARCH_SAMD  -D__SAMD21G18A__ -DUSB_VID=0x2341 -DUSB_PID=0x804e -DUSBCON '-DUSB_MANUFACTURER="Arduino LLC"' '-DUSB_PRODUCT="Arduino MKR1000"' "-I/Users/andri/eclipse/cpp-mars/eclipseArduino.app/Contents/Eclipse/arduinoPlugin/tools/arduino/CMSIS/4.0.0-atmel/CMSIS/Include/" "-I/Users/andri/eclipse/cpp-mars/eclipseArduino.app/Contents/Eclipse/arduinoPlugin/tools/arduino/CMSIS/4.0.0-atmel/Device/ATMEL/"   -I"/Users/andri/eclipse/cpp-mars/eclipseArduino.app/Contents/Eclipse/arduinoPlugin/packages/arduino-mkr1000/hardware/samd/1.6.4-mkr02/cores/arduino" -I"/Users/andri/eclipse/cpp-mars/eclipseArduino.app/Contents/Eclipse/arduinoPlugin/packages/arduino-mkr1000/hardware/samd/1.6.4-mkr02/variants/mkr1000" -I"/Users/andri/eclipse/cpp-mars/eclipseArduino.app/Contents/Eclipse/arduinoPlugin/packages/arduino-mkr1000/hardware/samd/1.6.4-mkr02/libraries/Wire" -I"/Users/andri/eclipse/cpp-mars/eclipseArduino.app/Contents/Eclipse/arduinoPlugin/libraries/WiFi101/0.9.0" -I"/Users/andri/eclipse/cpp-mars/eclipseArduino.app/Contents/Eclipse/arduinoPlugin/libraries/WiFi101/0.9.0/src" -I"/Users/andri/eclipse/cpp-mars/eclipseArduino.app/Contents/Eclipse/arduinoPlugin/packages/arduino-mkr1000/hardware/samd/1.6.4-mkr02/libraries/SPI" -I"/Users/andri/eclipse/cpp-mars/eclipseArduino.app/Contents/Eclipse/arduinoPlugin/libraries/PubSubClient/2.6.0" -I"/Users/andri/eclipse/cpp-mars/eclipseArduino.app/Contents/Eclipse/arduinoPlugin/libraries/PubSubClient/2.6.0/src" -I"/Users/andri/eclipse/cpp-mars/eclipseArduino.app/Contents/Eclipse/arduinoPlugin/libraries/RTCZero/1.4.3" -I"/Users/andri/eclipse/cpp-mars/eclipseArduino.app/Contents/Eclipse/arduinoPlugin/libraries/RTCZero/1.4.3/src" -I"/Users/andri/eclipse/cpp-mars/eclipseArduino.app/Contents/Eclipse/arduinoPlugin/libraries/ArduinoJson/5.1.1" -I"/Users/andri/eclipse/cpp-mars/eclipseArduino.app/Contents/Eclipse/arduinoPlugin/libraries/ArduinoJson/5.1.1/src" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"   -Wall
	@echo 'Finished building: $<'
	@echo ' '


