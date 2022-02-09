################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/GitHub/Splat2OS/DevBoard755/Common/Src/Interpreter/editor.c \
C:/GitHub/Splat2OS/DevBoard755/Common/Src/Interpreter/exectoks.c \
C:/GitHub/Splat2OS/DevBoard755/Common/Src/Interpreter/expr.c \
C:/GitHub/Splat2OS/DevBoard755/Common/Src/Interpreter/tokens.c \
C:/GitHub/Splat2OS/DevBoard755/Common/Src/Interpreter/utils.c 

C_DEPS += \
./Common/Src/Interpreter/editor.d \
./Common/Src/Interpreter/exectoks.d \
./Common/Src/Interpreter/expr.d \
./Common/Src/Interpreter/tokens.d \
./Common/Src/Interpreter/utils.d 

OBJS += \
./Common/Src/Interpreter/editor.o \
./Common/Src/Interpreter/exectoks.o \
./Common/Src/Interpreter/expr.o \
./Common/Src/Interpreter/tokens.o \
./Common/Src/Interpreter/utils.o 


# Each subdirectory must supply rules for building sources it contributes
Common/Src/Interpreter/editor.o: C:/GitHub/Splat2OS/DevBoard755/Common/Src/Interpreter/editor.c Common/Src/Interpreter/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32H755xx -c -I../Core/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/GitHub/Splat2OS/DevBoard755/Common/Src/Interpreter" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Common/Src/Interpreter/exectoks.o: C:/GitHub/Splat2OS/DevBoard755/Common/Src/Interpreter/exectoks.c Common/Src/Interpreter/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32H755xx -c -I../Core/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/GitHub/Splat2OS/DevBoard755/Common/Src/Interpreter" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Common/Src/Interpreter/expr.o: C:/GitHub/Splat2OS/DevBoard755/Common/Src/Interpreter/expr.c Common/Src/Interpreter/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32H755xx -c -I../Core/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/GitHub/Splat2OS/DevBoard755/Common/Src/Interpreter" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Common/Src/Interpreter/tokens.o: C:/GitHub/Splat2OS/DevBoard755/Common/Src/Interpreter/tokens.c Common/Src/Interpreter/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32H755xx -c -I../Core/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/GitHub/Splat2OS/DevBoard755/Common/Src/Interpreter" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Common/Src/Interpreter/utils.o: C:/GitHub/Splat2OS/DevBoard755/Common/Src/Interpreter/utils.c Common/Src/Interpreter/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32H755xx -c -I../Core/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/GitHub/Splat2OS/DevBoard755/Common/Src/Interpreter" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Common-2f-Src-2f-Interpreter

clean-Common-2f-Src-2f-Interpreter:
	-$(RM) ./Common/Src/Interpreter/editor.d ./Common/Src/Interpreter/editor.o ./Common/Src/Interpreter/exectoks.d ./Common/Src/Interpreter/exectoks.o ./Common/Src/Interpreter/expr.d ./Common/Src/Interpreter/expr.o ./Common/Src/Interpreter/tokens.d ./Common/Src/Interpreter/tokens.o ./Common/Src/Interpreter/utils.d ./Common/Src/Interpreter/utils.o

.PHONY: clean-Common-2f-Src-2f-Interpreter

