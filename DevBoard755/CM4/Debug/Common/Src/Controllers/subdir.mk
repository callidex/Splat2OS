################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
C:/GitHub/Splat2OS/DevBoard755/Common/Src/Controllers/lcd.cpp 

OBJS += \
./Common/Src/Controllers/lcd.o 

CPP_DEPS += \
./Common/Src/Controllers/lcd.d 


# Each subdirectory must supply rules for building sources it contributes
Common/Src/Controllers/lcd.o: C:/GitHub/Splat2OS/DevBoard755/Common/Src/Controllers/lcd.cpp Common/Src/Controllers/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32H755xx -c -I../Core/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Common-2f-Src-2f-Controllers

clean-Common-2f-Src-2f-Controllers:
	-$(RM) ./Common/Src/Controllers/lcd.d ./Common/Src/Controllers/lcd.o

.PHONY: clean-Common-2f-Src-2f-Controllers

