/***************************************************************************//**
 * (c) Copyright 2016 Microsemi SoC Products Group.  All rights reserved.
 *
 *
 * Main function for Video/Imaging demo
 *
 *
 * To know more details about this example project please refer ./README.TXT in
 * project directory.
 *
 *
 */

#include <stdio.h>
#include <math.h>
#include "mouse_app.h"
#include "mss_i2c.h"
#include "mss_spi.h"
#include "mss_gpio.h"
#include "mss_usb_device.h"
#include "mss_usb_device_hid.h"
#include "audio/audio.h"

#define DDR_BASE_ADDRESS 0xA0000000
#define MDDR_REG_RANGE 0x40020800

/* Fabric Addresses */
#define FABRIC_PROJECT_ID_LOCATION 0x30000000
//#define FABRIC_DDR2_BASE_ADDRESS 0x30000100
#define Brightness_Gain_Address  0x30000200
#define Contrast_Gain_Address    0x30000204
#define Saturation_Gain_Address  0x30000208
#define Hue_Gain_Address         0x3000020C
#define Sharpness_K_factor       0x30000210
#define AB_Image_Height          0x30000214
#define AB_Image_Width           0x30000218
#define AB_Image_PosX            0x3000021C
#define AB_Image_PosY            0x30000220
#define AB_Image_GB_Alpha        0x30000224
#define AB_Image_Address         0x30000228
#define Output_scalar_resolution  0x3000022C

#define SIN_MEM_BASE_ADDR        0x30001000
#define COS_MEM_BASE_ADDR        0x30002000

#define SENSOR_CLK_OVRD_PIN MSS_GPIO_5 // GPIO 5 is used to trigger the sensor clock override.
#define SENSOR_CLK_OVRD_TIME_MS 10000

extern void TW_Audio_SF2I2S_HW_Loopback(uint8_t enable);

/* USB specific function declaration */
void USB_rx_handler(void);
void USB_PCKHandler();

/* MSS to Fabric */
void MSS_M2FabricWrite(void);

volatile uint32_t *sin_mem_base_addr = (volatile uint32_t *) SIN_MEM_BASE_ADDR;
volatile uint32_t *cos_mem_base_addr = (volatile uint32_t *) COS_MEM_BASE_ADDR;

mss_usbd_hid_report_t report = { 0 };
unsigned char data_received = 0;
unsigned char data_sent = 0;

uint32_t *ddr_mem_ptr;
uint32_t *usb_array_ptr;
uint32_t *DMA_Address_ptr;
int callcount = 0;
uint32_t temp = 0;

uint32_t *debug_ptr;

uint32_t mode_change;
uint32_t mode_val;

tw_status_t     tw_status;
uint32_t audio_config_done = 0;


extern void HDMI_tx_init();

extern void sincos_mem_init ();

extern void parallel_ar0330_1080_720_30fps(uint32_t mode);
extern void color_mode_select_none();
extern void color_mode_select_red();
extern void color_mode_select_green();
extern void color_mode_select_blue();
extern void color_mode_bars();
extern void color_mode_walking1s();

// Dynamic sensor configuration functions from ar0330.c
extern void set_frame_length_lines(uint16_t frame_length);
extern void set_line_length_pck(uint16_t line_length);
extern void set_coarse_integration_time(uint16_t coarse_integration_time);
extern void set_coarse_integration_time(uint16_t coarse_integration_time);
extern void set_vt_pix_clk_div(uint16_t clk_div);
extern void set_vt_sys_clk_div(uint16_t clk_div);
extern void set_pre_pll_clk_div(uint16_t clk_div);
extern void set_pll_multiplier(uint16_t pll_mult);

/* This function does audio loop-back from TW->SF2 I2S->TW. The audio is transferred outside TW - to I2S of SF2.
 * This function shall be called after calling TW_Audio_Init() function.
 */
extern tw_status_t TW_Audio_SF2I2S_Loopback(tw_command_t tw_command);

/*  This function initialises TW platform. This function flashes firmware and configuration record
 * hence should ideally be called once. TW supports loading of multiple firmware however for simplicity
 * the function will return error if it is already initialized after calling this function. This function
 * should be called first when flash is empty and also immediately after calling TW_Audio_DeInit() function.
 */
extern tw_status_t TW_Audio_Init();

extern void msdelay_init();
extern void msdelay(uint32_t tms);
uint8_t run_flag = 0;
uint32_t x, y;


void sensor_clock_override(int hold_time_ms)
{
    /* Description:
     * This function is used to override the image sensor clock for time in milliseconds given by parameter hold_time_ms
     *
     * Argument:
     * int hold_time_ms - time in milliseconds to delay between driving SENSOR_CLK_OVRD_PIN high and low.
     *
     * Author:
     * Stephen Thompson - sthomps3@asu.edu
    */
    MSS_GPIO_drive_inout(SENSOR_CLK_OVRD_PIN, MSS_GPIO_DRIVE_HIGH); // Drive HIGH to SENSOR_CLK_OVRD_PIN to freeze sensor clock
    msdelay(hold_time_ms);
    MSS_GPIO_drive_inout(SENSOR_CLK_OVRD_PIN, MSS_GPIO_DRIVE_LOW); // Drive LOW to SENSOR_CLK_OVRD_PIN to un-freeze sensor clock
    msdelay(hold_time_ms);
}

void movelogo(uint32_t x, uint32_t y)
{
    *(volatile int*) AB_Image_PosX = x;
    *(volatile int*) AB_Image_PosY = y;
}
int main() {

    msdelay_init();

    MSS_GPIO_init();
    MSS_GPIO_config(MSS_GPIO_8, MSS_GPIO_OUTPUT_MODE); // CAM reset
    MSS_GPIO_config(SENSOR_CLK_OVRD_PIN, MSS_GPIO_OUTPUT_MODE); // Pin used to enable sensor clock holding.

    parallel_ar0330_1080_720_30fps(0);
    msdelay(10);


    HDMI_tx_init();
    msdelay(100);


    sincos_mem_init();

    MOUSE_init();


    MSS_GPIO_config(MSS_GPIO_4, MSS_GPIO_OUTPUT_MODE); // Mux Selection

    DMA_Address_ptr = (uint32_t*) DDR_BASE_ADDRESS;

    *(volatile int*) AB_Image_Address  = 0xA0000000;
    *(volatile int*) AB_Image_Height = 0x34;
    *(volatile int*) AB_Image_Width = 0x110;
    *(volatile int*) AB_Image_PosX = 0x1;
    *(volatile int*) AB_Image_PosY = 0x1;
    *(volatile int*) AB_Image_GB_Alpha = 0xFF;

    *(volatile int*) Brightness_Gain_Address = 0x00;
    *(volatile int*) Contrast_Gain_Address = 0x01;
    *(volatile int*) Saturation_Gain_Address = 0x01;
    *(volatile int*) Hue_Gain_Address = 0x000;
    *(volatile int*) Sharpness_K_factor = 0x000;


    while (1) {
        if(mode_change)
        {
            mode_change = 0;
            parallel_ar0330_1080_720_30fps(mode_val);
        }

        sensor_clock_override(SENSOR_CLK_OVRD_TIME_MS);
    }

    MSS_USBD_rx_ep_disable_irq(MSS_USB_RX_EP_1);
}

void USB_PCKHandler() {
    uint16_t hue_value;
    uint32_t array_size = 0;
    uint32_t i = 0;

    callcount++;
    if (g_rx_data[0] == 0)//Bulk Write Operation
    {
        DMA_Address_ptr = (uint32_t *) ((g_rx_data[1] << 24) + (g_rx_data[2]
                << 16) + (g_rx_data[3] << 8) + (g_rx_data[4]));
        //uint8_t No_of_Bytes = g_rx_data[5];

        usb_array_ptr = (uint32_t *) &g_rx_data[7];
        array_size = sizeof(g_rx_data) - 7; //Initial 6 Bytes will have the command identifier, DMA Address and No of Bytes to Read or Write
        g_rx_complete_status = 0;
        for (i = 0; i < (array_size / 4); i++) {
            *DMA_Address_ptr = *usb_array_ptr;
            DMA_Address_ptr++;
            usb_array_ptr++;
        }
        g_rx_complete_status = 1;
    }
    if (g_rx_data[0] == 1)//Bulk Read Operation
    {
        g_rx_complete_status = 0;
        uint16_t No_of_Bytes = 0;
        DMA_Address_ptr = (uint32_t *) ((g_rx_data[1] << 24) + (g_rx_data[2]
                << 16) + (g_rx_data[3] << 8) + (g_rx_data[4]));
        No_of_Bytes = (uint16_t)((g_rx_data[5] << 8) + (g_rx_data[6]));

        for (i = 0; i <= No_of_Bytes; i = i + 4) {
            temp = *DMA_Address_ptr;
            g_tx_data[i + 0] = (uint8_t)(temp);
            g_tx_data[i + 1] = (uint8_t)(temp >> 8);
            g_tx_data[i + 2] = (uint8_t)(temp >> 16);
            g_tx_data[i + 3] = (uint8_t)(temp >> 24);
            DMA_Address_ptr++;
        }
        g_rx_complete_status = 1;
        //g_tx_complete_status=1;
        MSS_USBD_HID_tx_report((uint8_t*) &g_tx_data, sizeof(g_tx_data));
    }
    if (g_rx_data[0] == 2)//Brightness and Contrast Adjustment
    {
        g_rx_complete_status = 0;

        *(volatile int*) Brightness_Gain_Address = g_rx_data[2];
        *(volatile int*) Contrast_Gain_Address = g_rx_data[4];

        g_tx_data[i + 0] = 0;
        g_tx_data[i + 1] = 0;
        g_tx_data[i + 2] = 0;
        g_tx_data[i + 3] = 0;

        g_rx_complete_status = 1;

        MSS_USBD_HID_tx_report((uint8_t*) &g_tx_data, sizeof(g_tx_data));

    }
    if (g_rx_data[0] == 3)//Saturation and Hue Adjustment
    {
        g_rx_complete_status = 0;

        *(volatile int*) Saturation_Gain_Address = g_rx_data[2];
        hue_value = (g_rx_data[3] << 8) + g_rx_data[4];
        *(volatile int*) Hue_Gain_Address = hue_value;

        g_tx_data[i + 0] = 0;
        g_tx_data[i + 1] = 0;
        g_tx_data[i + 2] = 0;
        g_tx_data[i + 3] = 0;

        g_rx_complete_status = 1;

        MSS_USBD_HID_tx_report((uint8_t*) &g_tx_data, sizeof(g_tx_data));

    }
    if (g_rx_data[0] == 4)//Sharpness Adjustment
    {
        g_rx_complete_status = 0;

        *(volatile int*) Sharpness_K_factor = (g_rx_data[1] << 8)
                + g_rx_data[2];

        g_tx_data[i + 0] = 0;
        g_tx_data[i + 1] = 0;
        g_tx_data[i + 2] = 0;
        g_tx_data[i + 3] = 0;

        g_rx_complete_status = 1;

        MSS_USBD_HID_tx_report((uint8_t*) &g_tx_data, sizeof(g_tx_data));

    }
    if (g_rx_data[0] == 5)// Demo Selection
    {
        g_rx_complete_status = 0;

        if (g_rx_data[1] == 1u) {
            MSS_GPIO_set_output(MSS_GPIO_4, 1u);
        } else if (g_rx_data[1] == 2u) {
            MSS_GPIO_set_output(MSS_GPIO_4, 0u);
        } else {
            MSS_GPIO_set_output(MSS_GPIO_4, 0u);
        }

        g_rx_complete_status = 1;

        MSS_USBD_HID_tx_report((uint8_t*) &g_tx_data, sizeof(g_tx_data));

    }
    if (g_rx_data[0] == 6)// Camera Test Mode Selection
    {
        mode_change = 1;
        mode_val = g_rx_data[1];

        g_rx_complete_status = 1;

        MSS_USBD_HID_tx_report((uint8_t*) &g_tx_data, sizeof(g_tx_data));
    }
    if (g_rx_data[0] == 7)// Audio Start / Stop
    {
        switch (g_rx_data[1])
        {
        case 0:
            // No Action
            break;
        case 1:
            // Start Audio Loopback
            TW_Audio_SF2I2S_HW_Loopback(1);
            break;
        case 2:
            // Stop Audio Loopback
            TW_Audio_SF2I2S_HW_Loopback(0);
            break;
        }

        g_rx_complete_status = 1;

        MSS_USBD_HID_tx_report((uint8_t*) &g_tx_data, sizeof(g_tx_data));
    }

    if (g_rx_data[0] == 8)// Configure Audio - Time consuming process
    {
        tw_status = TW_Audio_Init();
        if((tw_status == TW_STATUS_SUCCESS) || (tw_status == TW_STATUS_ALREADY_INIT))
        {
            /* Start I2S Audio Loop-back */
            tw_status = TW_Audio_SF2I2S_Loopback(TW_COMMAND_START);
            audio_config_done = 1;
        }
        g_rx_complete_status = 1;

        MSS_USBD_HID_tx_report((uint8_t*) &g_tx_data, sizeof(g_tx_data));
    }

    if (g_rx_data[0] == 9)// Connect
    {
        g_tx_data[0] = 9;
        g_tx_data[1] = 10;

        g_rx_complete_status = 1;

        MSS_USBD_HID_tx_report((uint8_t*) &g_tx_data, sizeof(g_tx_data));
    }

    if (g_rx_data[0] == 10)// Connect
    {
        g_tx_data[0] = 10;
        g_tx_data[1] = audio_config_done;

        g_rx_complete_status = 1;

        MSS_USBD_HID_tx_report((uint8_t*) &g_tx_data, sizeof(g_tx_data));
    }

    if (g_rx_data[0] == 11)//fabric project version
    {
        uint32_t *fabric_id = (uint32_t *)FABRIC_PROJECT_ID_LOCATION;

        //*fabric_id
        g_rx_complete_status = 0;

        *(volatile int*) Sharpness_K_factor = (g_rx_data[1] << 8)
                + g_rx_data[2];

        g_tx_data[0] = 0xB;
        g_tx_data[1] = *fabric_id & 0xFF;
        g_tx_data[2] = (*fabric_id >> 8) & 0xFF;
        g_tx_data[3] = (*fabric_id >> 16) & 0xFF;

        g_rx_complete_status = 1;

        MSS_USBD_HID_tx_report((uint8_t*) &g_tx_data, sizeof(g_tx_data));

    }

    if (g_rx_data[0] == 12)//Image width, height, X position and Y position
    {
        g_rx_complete_status = 0;

        *(volatile uint32_t *) AB_Image_Width = (g_rx_data[1] << 8) + g_rx_data[2];
        *(volatile uint32_t *) AB_Image_Height = (g_rx_data[3] << 8) + g_rx_data[4];
        *(volatile uint32_t *) AB_Image_PosX = (g_rx_data[5] << 8) + g_rx_data[6];
        *(volatile uint32_t *) AB_Image_PosY = (g_rx_data[7] << 8) + g_rx_data[8];
        *(volatile uint32_t *) AB_Image_Address = ((g_rx_data[9] << 24) | (g_rx_data[10] << 16) | (g_rx_data[11] << 8) | (g_rx_data[12]));
        g_tx_data[i + 0] = 0;
        g_tx_data[i + 1] = 0;
        g_tx_data[i + 2] = 0;
        g_tx_data[i + 3] = 0;

        g_rx_complete_status = 1;

        MSS_USBD_HID_tx_report((uint8_t*) &g_tx_data, sizeof(g_tx_data));

    }

    if (g_rx_data[0] == 13)//Global Alpha
    {
        g_rx_complete_status = 0;

        *(volatile int*) AB_Image_GB_Alpha = (g_rx_data[1] << 8) + g_rx_data[2];

        g_tx_data[i + 0] = 0;
        g_tx_data[i + 1] = 0;
        g_tx_data[i + 2] = 0;
        g_tx_data[i + 3] = 0;

        g_rx_complete_status = 1;

        MSS_USBD_HID_tx_report((uint8_t*) &g_tx_data, sizeof(g_tx_data));

    }

    if (g_rx_data[0] == 14)//Move Object
    {
        g_rx_complete_status = 0;

        run_flag = g_rx_data[1];

        g_tx_data[i + 0] = 0;
        g_tx_data[i + 1] = 0;
        g_tx_data[i + 2] = 0;
        g_tx_data[i + 3] = 0;

        g_rx_complete_status = 1;

        MSS_USBD_HID_tx_report((uint8_t*) &g_tx_data, sizeof(g_tx_data));

    }

}

void USB_rx_handler() {

    if ((1 == g_rx_complete_status) && (data_received == 1)) {
        data_received = 0;
    }
    if (data_received == 0) {
        data_received = 0;
    }
}
