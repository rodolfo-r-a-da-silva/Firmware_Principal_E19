################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/ft_can.c \
../Core/Src/main.c \
../Core/Src/pdm_can.c \
../Core/Src/principal_can.c \
../Core/Src/principal_config.c \
../Core/Src/principal_datalogger.c \
../Core/Src/principal_interrupts.c \
../Core/Src/principal_variables.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c 

OBJS += \
./Core/Src/ft_can.o \
./Core/Src/main.o \
./Core/Src/pdm_can.o \
./Core/Src/principal_can.o \
./Core/Src/principal_config.o \
./Core/Src/principal_datalogger.o \
./Core/Src/principal_interrupts.o \
./Core/Src/principal_variables.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o 

C_DEPS += \
./Core/Src/ft_can.d \
./Core/Src/main.d \
./Core/Src/pdm_can.d \
./Core/Src/principal_can.d \
./Core/Src/principal_config.d \
./Core/Src/principal_datalogger.d \
./Core/Src/principal_interrupts.d \
./Core/Src/principal_variables.d \
./Core/Src/stm32f4xx_hal_msp.d \
./Core/Src/stm32f4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../USB_DEVICE/App -I../USB_DEVICE/Target -I../Middlewares/Third_Party/FatFs/src -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/ft_can.d ./Core/Src/ft_can.o ./Core/Src/ft_can.su ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/pdm_can.d ./Core/Src/pdm_can.o ./Core/Src/pdm_can.su ./Core/Src/principal_can.d ./Core/Src/principal_can.o ./Core/Src/principal_can.su ./Core/Src/principal_config.d ./Core/Src/principal_config.o ./Core/Src/principal_config.su ./Core/Src/principal_datalogger.d ./Core/Src/principal_datalogger.o ./Core/Src/principal_datalogger.su ./Core/Src/principal_interrupts.d ./Core/Src/principal_interrupts.o ./Core/Src/principal_interrupts.su ./Core/Src/principal_variables.d ./Core/Src/principal_variables.o ./Core/Src/principal_variables.su ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_hal_msp.su ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/stm32f4xx_it.su ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/system_stm32f4xx.su

.PHONY: clean-Core-2f-Src

