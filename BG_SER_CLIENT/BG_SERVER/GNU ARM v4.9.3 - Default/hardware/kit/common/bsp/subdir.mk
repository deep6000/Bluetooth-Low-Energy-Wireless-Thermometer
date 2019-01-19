################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../hardware/kit/common/bsp/bsp_stk.c 

OBJS += \
./hardware/kit/common/bsp/bsp_stk.o 

C_DEPS += \
./hardware/kit/common/bsp/bsp_stk.d 


# Each subdirectory must supply rules for building sources it contributes
hardware/kit/common/bsp/bsp_stk.o: ../hardware/kit/common/bsp/bsp_stk.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-D__STACK_SIZE=0x800' '-DHAL_CONFIG=1' '-D__HEAP_SIZE=0xD00' '-D__StackLimit=0x20000000' '-DEFR32BG13P632F512GM48=1' -I"F:\MS Embedded Systems\IOT\Practicals\BG_SER_CLIENT\BG_SERVER" -I"F:\MS Embedded Systems\IOT\Practicals\BG_SER_CLIENT\BG_SERVER\platform\Device\SiliconLabs\EFR32BG13P\Include" -I"F:\MS Embedded Systems\IOT\Practicals\BG_SER_CLIENT\BG_SERVER\platform\CMSIS\Include" -I"F:\MS Embedded Systems\IOT\Practicals\BG_SER_CLIENT\BG_SERVER\platform\Device\SiliconLabs\EFR32BG13P\Source" -I"F:\MS Embedded Systems\IOT\Practicals\BG_SER_CLIENT\BG_SERVER\platform\radio\rail_lib\common" -I"F:\MS Embedded Systems\IOT\Practicals\BG_SER_CLIENT\BG_SERVER\platform\emlib\src" -I"F:\MS Embedded Systems\IOT\Practicals\BG_SER_CLIENT\BG_SERVER\platform\emlib\inc" -I"F:\MS Embedded Systems\IOT\Practicals\BG_SER_CLIENT\BG_SERVER\hardware\kit\common\bsp" -I"F:\MS Embedded Systems\IOT\Practicals\BG_SER_CLIENT\BG_SERVER\platform\Device\SiliconLabs\EFR32BG13P\Source\GCC" -I"F:\MS Embedded Systems\IOT\Practicals\BG_SER_CLIENT\BG_SERVER\hardware\kit\common\drivers" -I"F:\MS Embedded Systems\IOT\Practicals\BG_SER_CLIENT\BG_SERVER\platform\bootloader\api" -I"F:\MS Embedded Systems\IOT\Practicals\BG_SER_CLIENT\BG_SERVER\platform\emdrv\sleep\inc" -I"F:\MS Embedded Systems\IOT\Practicals\BG_SER_CLIENT\BG_SERVER\protocol\bluetooth\ble_stack\inc\common" -I"F:\MS Embedded Systems\IOT\Practicals\BG_SER_CLIENT\BG_SERVER\hardware\kit\common\halconfig" -I"F:\MS Embedded Systems\IOT\Practicals\BG_SER_CLIENT\BG_SERVER\hardware\kit\EFR32BG13_BRD4104A\config" -I"F:\MS Embedded Systems\IOT\Practicals\BG_SER_CLIENT\BG_SERVER\platform\emdrv\uartdrv\inc" -I"F:\MS Embedded Systems\IOT\Practicals\BG_SER_CLIENT\BG_SERVER\platform\emdrv\common\inc" -I"F:\MS Embedded Systems\IOT\Practicals\BG_SER_CLIENT\BG_SERVER\protocol\bluetooth\ble_stack\inc\soc" -I"F:\MS Embedded Systems\IOT\Practicals\BG_SER_CLIENT\BG_SERVER\platform\halconfig\inc\hal-config" -I"F:\MS Embedded Systems\IOT\Practicals\BG_SER_CLIENT\BG_SERVER\platform\emdrv\gpiointerrupt\inc" -I"F:\MS Embedded Systems\IOT\Practicals\BG_SER_CLIENT\BG_SERVER\platform\emdrv\sleep\src" -I"F:\MS Embedded Systems\IOT\Practicals\BG_SER_CLIENT\BG_SERVER\platform\radio\rail_lib\chip\efr32" -I"F:\MS Embedded Systems\IOT\Practicals\BG_SER_CLIENT\BG_SERVER\app\bluetooth\common\stack_bridge" -I"F:\MS Embedded Systems\IOT\Practicals\BG_SER_CLIENT\BG_SERVER\platform\bootloader" -I"F:\MS Embedded Systems\IOT\Practicals\BG_SER_CLIENT\BG_SERVER\lcdGraphics" -I"H:/Simplicity/developer/sdks/gecko_sdk_suite/v2.3//platform/middleware/glib/glib" -I"H:/Simplicity/developer/sdks/gecko_sdk_suite/v2.3//platform/middleware/glib/dmd" -I"H:/Simplicity/developer/sdks/gecko_sdk_suite/v2.3//util/silicon_labs/silabs_core/graphics" -I"H:/Simplicity/developer/sdks/gecko_sdk_suite/v2.3//hardware/kit/common/drivers" -I"H:/Simplicity/developer/sdks/gecko_sdk_suite/v2.3//platform/middleware/glib" -O0 -Wall -c -fmessage-length=0 -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"hardware/kit/common/bsp/bsp_stk.d" -MT"hardware/kit/common/bsp/bsp_stk.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


