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
	arm-none-eabi-gcc -mthumb -mcpu=cortex-m3 -DNDEBUG -DMSS_USB_DEVICE_ENABLED -ID:\ProjectSpecific\Projects\VideoSolution\parallelCleanup\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform -ID:\ProjectSpecific\Projects\VideoSolution\parallelCleanup\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\CMSIS -ID:\ProjectSpecific\Projects\VideoSolution\parallelCleanup\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\CMSIS\startup_gcc -ID:\ProjectSpecific\Projects\VideoSolution\parallelCleanup\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\drivers -ID:\ProjectSpecific\Projects\VideoSolution\parallelCleanup\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\drivers\mss_gpio -ID:\ProjectSpecific\Projects\VideoSolution\parallelCleanup\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\drivers\mss_hpdma -ID:\ProjectSpecific\Projects\VideoSolution\parallelCleanup\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\drivers\mss_i2c -ID:\ProjectSpecific\Projects\VideoSolution\parallelCleanup\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\drivers\mss_nvm -ID:\ProjectSpecific\Projects\VideoSolution\parallelCleanup\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\drivers\mss_sys_services -ID:\ProjectSpecific\Projects\VideoSolution\parallelCleanup\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\drivers\mss_timer -ID:\ProjectSpecific\Projects\VideoSolution\parallelCleanup\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\drivers\mss_usb -ID:\ProjectSpecific\Projects\VideoSolution\parallelCleanup\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\drivers_config -ID:\ProjectSpecific\Projects\VideoSolution\parallelCleanup\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\drivers_config\sys_config -ID:\ProjectSpecific\Projects\VideoSolution\parallelCleanup\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\hal -ID:\ProjectSpecific\Projects\VideoSolution\parallelCleanup\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\hal\CortexM3 -ID:\ProjectSpecific\Projects\VideoSolution\parallelCleanup\parallel_cam_video_ref_design\SoftConsole\vga_display_mss_MSS_CM3\vga_display_mss_MSS_CM3_hw_platform\hal\CortexM3\GNU -O0 -ffunction-sections -fdata-sections -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

CMSIS/startup_gcc/%.o: ../CMSIS/startup_gcc/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Assembler'
	arm-none-eabi-gcc -c -mthumb -mcpu=cortex-m3 -specs=bare.specs  -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


