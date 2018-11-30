################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../system/src/ADuCM350/adi_nvic.c \
../system/src/ADuCM350/afe.c \
../system/src/ADuCM350/afe_lib.c \
../system/src/ADuCM350/beep.c \
../system/src/ADuCM350/captouch.c \
../system/src/ADuCM350/captouch_lib.c \
../system/src/ADuCM350/crc.c \
../system/src/ADuCM350/dma.c \
../system/src/ADuCM350/flash.c \
../system/src/ADuCM350/gpio.c \
../system/src/ADuCM350/gpt.c \
../system/src/ADuCM350/metrics.c \
../system/src/ADuCM350/pdi.c \
../system/src/ADuCM350/rng.c \
../system/src/ADuCM350/rtc.c \
../system/src/ADuCM350/spi.c \
../system/src/ADuCM350/uart.c \
../system/src/ADuCM350/wdt.c \
../system/src/ADuCM350/wut.c 

OBJS += \
./system/src/ADuCM350/adi_nvic.o \
./system/src/ADuCM350/afe.o \
./system/src/ADuCM350/afe_lib.o \
./system/src/ADuCM350/beep.o \
./system/src/ADuCM350/captouch.o \
./system/src/ADuCM350/captouch_lib.o \
./system/src/ADuCM350/crc.o \
./system/src/ADuCM350/dma.o \
./system/src/ADuCM350/flash.o \
./system/src/ADuCM350/gpio.o \
./system/src/ADuCM350/gpt.o \
./system/src/ADuCM350/metrics.o \
./system/src/ADuCM350/pdi.o \
./system/src/ADuCM350/rng.o \
./system/src/ADuCM350/rtc.o \
./system/src/ADuCM350/spi.o \
./system/src/ADuCM350/uart.o \
./system/src/ADuCM350/wdt.o \
./system/src/ADuCM350/wut.o 

C_DEPS += \
./system/src/ADuCM350/adi_nvic.d \
./system/src/ADuCM350/afe.d \
./system/src/ADuCM350/afe_lib.d \
./system/src/ADuCM350/beep.d \
./system/src/ADuCM350/captouch.d \
./system/src/ADuCM350/captouch_lib.d \
./system/src/ADuCM350/crc.d \
./system/src/ADuCM350/dma.d \
./system/src/ADuCM350/flash.d \
./system/src/ADuCM350/gpio.d \
./system/src/ADuCM350/gpt.d \
./system/src/ADuCM350/metrics.d \
./system/src/ADuCM350/pdi.d \
./system/src/ADuCM350/rng.d \
./system/src/ADuCM350/rtc.d \
./system/src/ADuCM350/spi.d \
./system/src/ADuCM350/uart.d \
./system/src/ADuCM350/wdt.d \
./system/src/ADuCM350/wut.d 


# Each subdirectory must supply rules for building sources it contributes
system/src/ADuCM350/%.o: ../system/src/ADuCM350/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DTRACE -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/ADuCM350" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


