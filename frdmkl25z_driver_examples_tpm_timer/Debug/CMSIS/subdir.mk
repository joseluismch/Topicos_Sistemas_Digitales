################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CMSIS/system_MKL25Z4.c 

OBJS += \
./CMSIS/system_MKL25Z4.o 

C_DEPS += \
./CMSIS/system_MKL25Z4.d 


# Each subdirectory must supply rules for building sources it contributes
CMSIS/%.o: ../CMSIS/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DPRINTF_FLOAT_ENABLE=0 -DCR_INTEGER_PRINTF -DFREEDOM -DFRDM_KL25Z -DDEBUG -DSDK_DEBUGCONSOLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DCPU_MKL25Z128VLK4 -DCPU_MKL25Z128VLK4_cm0plus -I"C:\Users\PEDRO\Desktop\proyectos\REPOGIT\frdmkl25z_driver_examples_tpm_timer\board" -I"C:\Users\PEDRO\Desktop\proyectos\REPOGIT\frdmkl25z_driver_examples_tpm_timer\source" -I"C:\Users\PEDRO\Desktop\proyectos\REPOGIT\frdmkl25z_driver_examples_tpm_timer" -I"C:\Users\PEDRO\Desktop\proyectos\REPOGIT\frdmkl25z_driver_examples_tpm_timer\drivers" -I"C:\Users\PEDRO\Desktop\proyectos\REPOGIT\frdmkl25z_driver_examples_tpm_timer\utilities" -I"C:\Users\PEDRO\Desktop\proyectos\REPOGIT\frdmkl25z_driver_examples_tpm_timer\CMSIS" -I"C:\Users\PEDRO\Desktop\proyectos\REPOGIT\frdmkl25z_driver_examples_tpm_timer\startup" -O0 -fno-common -g -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


