################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../driver/Gpt/Gpt.c \
../driver/Gpt/Gpt_Irq.c \
../driver/Gpt/Gpt_PBCfg.c 

OBJS += \
./driver/Gpt/Gpt.o \
./driver/Gpt/Gpt_Irq.o \
./driver/Gpt/Gpt_PBCfg.o 

C_DEPS += \
./driver/Gpt/Gpt.d \
./driver/Gpt/Gpt_Irq.d \
./driver/Gpt/Gpt_PBCfg.d 


# Each subdirectory must supply rules for building sources it contributes
driver/Gpt/%.o: ../driver/Gpt/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@driver/Gpt/Gpt.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


