################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CMSIS/startup_gcc/newlib_stubs.c 

S_UPPER_SRCS += \
../CMSIS/startup_gcc/startup_m2sxxx.S 

OBJS += \
./CMSIS/startup_gcc/newlib_stubs.o \
./CMSIS/startup_gcc/startup_m2sxxx.o 

C_DEPS += \
./CMSIS/startup_gcc/newlib_stubs.d 


# Each subdirectory must supply rules for building sources it contributes
CMSIS/startup_gcc/%.o: ../CMSIS/startup_gcc/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU C Compiler'
	arm-none-eabi-gcc -mthumb -mcpu=cortex-m3 -DMSS_USB_DEVICE_ENABLED -IC:\Projects\parallel_cam_video_ref_design_2\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform -IC:\Projects\parallel_cam_video_ref_design_2\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\CMSIS -IC:\Projects\parallel_cam_video_ref_design_2\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\CMSIS\startup_gcc -IC:\Projects\parallel_cam_video_ref_design_2\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\drivers -IC:\Projects\parallel_cam_video_ref_design_2\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\drivers\mss_gpio -IC:\Projects\parallel_cam_video_ref_design_2\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\drivers\mss_hpdma -IC:\Projects\parallel_cam_video_ref_design_2\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\drivers\mss_i2c -IC:\Projects\parallel_cam_video_ref_design_2\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\drivers\mss_nvm -IC:\Projects\parallel_cam_video_ref_design_2\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\drivers\mss_sys_services -IC:\Projects\parallel_cam_video_ref_design_2\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\drivers\mss_timer -IC:\Projects\parallel_cam_video_ref_design_2\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\drivers\mss_usb -IC:\Projects\parallel_cam_video_ref_design_2\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\drivers_config -IC:\Projects\parallel_cam_video_ref_design_2\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\drivers_config\sys_config -IC:\Projects\parallel_cam_video_ref_design_2\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\hal -IC:\Projects\parallel_cam_video_ref_design_2\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\hal\CortexM3 -IC:\Projects\parallel_cam_video_ref_design_2\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\hal\CortexM3\GNU -O0 -ffunction-sections -fdata-sections -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CMSIS/startup_gcc/%.o: ../CMSIS/startup_gcc/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Assembler'
	arm-none-eabi-gcc -c -mthumb -mcpu=cortex-m3 -specs=bare.specs  -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


