################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/ST/STM32_USB_Device_Library/Class/AUDIO/Src/usbd_audio.c \
../Middlewares/ST/STM32_USB_Device_Library/Class/AUDIO/Src/usbd_audio_if_template.c 

OBJS += \
./Middlewares/ST/STM32_USB_Device_Library/Class/AUDIO/Src/usbd_audio.o \
./Middlewares/ST/STM32_USB_Device_Library/Class/AUDIO/Src/usbd_audio_if_template.o 

C_DEPS += \
./Middlewares/ST/STM32_USB_Device_Library/Class/AUDIO/Src/usbd_audio.d \
./Middlewares/ST/STM32_USB_Device_Library/Class/AUDIO/Src/usbd_audio_if_template.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/ST/STM32_USB_Device_Library/Class/AUDIO/Src/usbd_audio.o: ../Middlewares/ST/STM32_USB_Device_Library/Class/AUDIO/Src/usbd_audio.c Middlewares/ST/STM32_USB_Device_Library/Class/AUDIO/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F412Rx -c -I../Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/AUDIO/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/Users/mischastuder/Büro/Projekt/Midi_Audio-Interface/DSP-Board-mdk/Drivers/CMSIS/DSP/Include" -I"/Users/mischastuder/Büro/Projekt/Midi_Audio-Interface/DSP-Board-mdk/Drivers/CMSIS/NN/Include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/ST/STM32_USB_Device_Library/Class/AUDIO/Src/usbd_audio.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Middlewares/ST/STM32_USB_Device_Library/Class/AUDIO/Src/usbd_audio_if_template.o: ../Middlewares/ST/STM32_USB_Device_Library/Class/AUDIO/Src/usbd_audio_if_template.c Middlewares/ST/STM32_USB_Device_Library/Class/AUDIO/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DDEBUG -DSTM32F412Rx -c -I../Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/ST/STM32_USB_Device_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Device_Library/Class/AUDIO/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/Users/mischastuder/Büro/Projekt/Midi_Audio-Interface/DSP-Board-mdk/Drivers/CMSIS/DSP/Include" -I"/Users/mischastuder/Büro/Projekt/Midi_Audio-Interface/DSP-Board-mdk/Drivers/CMSIS/NN/Include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/ST/STM32_USB_Device_Library/Class/AUDIO/Src/usbd_audio_if_template.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

