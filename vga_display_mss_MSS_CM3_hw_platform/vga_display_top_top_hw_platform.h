#ifndef vga_display_top_top_HW_PLATFORM_H_
#define vga_display_top_top_HW_PLATFORM_H_
/*****************************************************************************
*
*Created by Microsemi SmartDesign  Tue Mar 08 16:03:43 2016
*
*Memory map specification for peripherals in vga_display_top_top
*/

/*-----------------------------------------------------------------------------
* CM3 subsystem memory map
* Master(s) for this subsystem: CM3 FABRIC2MSSFIC2 
*---------------------------------------------------------------------------*/
#define APB_WRAPPER_0                   0x50000000U
#define VGA_DISPLAY_MSS                 0x50001000U
#define VGA_DISPLAY_MSS_MSS_0           0x40020800U


/*-----------------------------------------------------------------------------
* video_dma_0 subsystem memory map
* Master(s) for this subsystem: video_dma_0 video_dma_0 
*---------------------------------------------------------------------------*/
#define VGA_DISPLAY_MSS_MSS_0_MDDR_DDR_AXI_SLAVE 0x00000000U


#endif /* vga_display_top_top_HW_PLATFORM_H_*/
