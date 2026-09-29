################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../driver/Mcu/Mcu.c \
../driver/Mcu/Mcu_Cfg.c 

OBJS += \
./driver/Mcu/Mcu.o \
./driver/Mcu/Mcu_Cfg.o 

C_DEPS += \
./driver/Mcu/Mcu.d \
./driver/Mcu/Mcu_Cfg.d 


# Each subdirectory must supply rules for building sources it contributes
driver/Mcu/%.o: ../driver/Mcu/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@driver/Mcu/Mcu.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


