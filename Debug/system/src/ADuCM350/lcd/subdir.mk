################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../system/src/ADuCM350/lcd/lcd_VIM828.c 

OBJS += \
./system/src/ADuCM350/lcd/lcd_VIM828.o 

C_DEPS += \
./system/src/ADuCM350/lcd/lcd_VIM828.d 


# Each subdirectory must supply rules for building sources it contributes
system/src/ADuCM350/lcd/%.o: ../system/src/ADuCM350/lcd/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DTRACE -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/ADuCM350" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


