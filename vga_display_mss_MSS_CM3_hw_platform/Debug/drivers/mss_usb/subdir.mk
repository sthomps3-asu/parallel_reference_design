################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/mss_usb/mss_usb_common_cif.c \
../drivers/mss_usb/mss_usb_device.c \
../drivers/mss_usb/mss_usb_device_cdc.c \
../drivers/mss_usb/mss_usb_device_cif.c \
../drivers/mss_usb/mss_usb_device_hid.c \
../drivers/mss_usb/mss_usb_device_msd.c \
../drivers/mss_usb/mss_usb_device_printer.c \
../drivers/mss_usb/mss_usb_device_vendor.c \
../drivers/mss_usb/mss_usb_host.c \
../drivers/mss_usb/mss_usb_host_cif.c \
../drivers/mss_usb/mss_usb_host_msc.c 

OBJS += \
./drivers/mss_usb/mss_usb_common_cif.o \
./drivers/mss_usb/mss_usb_device.o \
./drivers/mss_usb/mss_usb_device_cdc.o \
./drivers/mss_usb/mss_usb_device_cif.o \
./drivers/mss_usb/mss_usb_device_hid.o \
./drivers/mss_usb/mss_usb_device_msd.o \
./drivers/mss_usb/mss_usb_device_printer.o \
./drivers/mss_usb/mss_usb_device_vendor.o \
./drivers/mss_usb/mss_usb_host.o \
./drivers/mss_usb/mss_usb_host_cif.o \
./drivers/mss_usb/mss_usb_host_msc.o 

C_DEPS += \
./drivers/mss_usb/mss_usb_common_cif.d \
./drivers/mss_usb/mss_usb_device.d \
./drivers/mss_usb/mss_usb_device_cdc.d \
./drivers/mss_usb/mss_usb_device_cif.d \
./drivers/mss_usb/mss_usb_device_hid.d \
./drivers/mss_usb/mss_usb_device_msd.d \
./drivers/mss_usb/mss_usb_device_printer.d \
./drivers/mss_usb/mss_usb_device_vendor.d \
./drivers/mss_usb/mss_usb_host.d \
./drivers/mss_usb/mss_usb_host_cif.d \
./drivers/mss_usb/mss_usb_host_msc.d 


# Each subdirectory must supply rules for building sources it contributes
drivers/mss_usb/%.o: ../drivers/mss_usb/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU C Compiler'
	arm-none-eabi-gcc -mthumb -mcpu=cortex-m3 -DMSS_USB_DEVICE_ENABLED -IC:\Projects\parallel_cam_video_ref_design_2\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform -IC:\Projects\parallel_cam_video_ref_design_2\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\CMSIS -IC:\Projects\parallel_cam_video_ref_design_2\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\CMSIS\startup_gcc -IC:\Projects\parallel_cam_video_ref_design_2\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\drivers -IC:\Projects\parallel_cam_video_ref_design_2\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\drivers\mss_gpio -IC:\Projects\parallel_cam_video_ref_design_2\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\drivers\mss_hpdma -IC:\Projects\parallel_cam_video_ref_design_2\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\drivers\mss_i2c -IC:\Projects\parallel_cam_video_ref_design_2\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\drivers\mss_nvm -IC:\Projects\parallel_cam_video_ref_design_2\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\drivers\mss_sys_services -IC:\Projects\parallel_cam_video_ref_design_2\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\drivers\mss_timer -IC:\Projects\parallel_cam_video_ref_design_2\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\drivers\mss_usb -IC:\Projects\parallel_cam_video_ref_design_2\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\drivers_config -IC:\Projects\parallel_cam_video_ref_design_2\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\drivers_config\sys_config -IC:\Projects\parallel_cam_video_ref_design_2\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\hal -IC:\Projects\parallel_cam_video_ref_design_2\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\hal\CortexM3 -IC:\Projects\parallel_cam_video_ref_design_2\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\hal\CortexM3\GNU -O0 -ffunction-sections -fdata-sections -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


