################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../hal/CortexM3/cortex_nvic.c 

OBJS += \
./hal/CortexM3/cortex_nvic.o 

C_DEPS += \
./hal/CortexM3/cortex_nvic.d 


# Each subdirectory must supply rules for building sources it contributes
hal/CortexM3/%.o: ../hal/CortexM3/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU C Compiler'
	arm-none-eabi-gcc -mthumb -mcpu=cortex-m3 -DNDEBUG -DMSS_USB_DEVICE_ENABLED -ID:\ProjectSpecific\Projects\VideoSolution\parallelCleanup\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform -ID:\ProjectSpecific\Projects\VideoSolution\parallelCleanup\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\CMSIS -ID:\ProjectSpecific\Projects\VideoSolution\parallelCleanup\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\CMSIS\startup_gcc -ID:\ProjectSpecific\Projects\VideoSolution\parallelCleanup\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\drivers -ID:\ProjectSpecific\Projects\VideoSolution\parallelCleanup\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\drivers\mss_gpio -ID:\ProjectSpecific\Projects\VideoSolution\parallelCleanup\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\drivers\mss_hpdma -ID:\ProjectSpecific\Projects\VideoSolution\parallelCleanup\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\drivers\mss_i2c -ID:\ProjectSpecific\Projects\VideoSolution\parallelCleanup\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\drivers\mss_nvm -ID:\ProjectSpecific\Projects\VideoSolution\parallelCleanup\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\drivers\mss_sys_services -ID:\ProjectSpecific\Projects\VideoSolution\parallelCleanup\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\drivers\mss_timer -ID:\ProjectSpecific\Projects\VideoSolution\parallelCleanup\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\drivers\mss_usb -ID:\ProjectSpecific\Projects\VideoSolution\parallelCleanup\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\drivers_config -ID:\ProjectSpecific\Projects\VideoSolution\parallelCleanup\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\drivers_config\sys_config -ID:\ProjectSpecific\Projects\VideoSolution\parallelCleanup\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\hal -ID:\ProjectSpecific\Projects\VideoSolution\parallelCleanup\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\hal\CortexM3 -ID:\ProjectSpecific\Projects\VideoSolution\parallelCleanup\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\hal\CortexM3\GNU -O0 -ffunction-sections -fdata-sections -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


