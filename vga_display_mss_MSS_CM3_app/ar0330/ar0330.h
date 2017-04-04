/*
 * ar0331.h
 *
 *  Created on: Jan 13, 2016
 *      Author: badal.nilawar
 */

#ifndef AR0331_H_
#define AR0331_H_

#include "mss_i2c/mss_i2c.h"
#include "mss_gpio/mss_gpio.h"

/*-------------------------------------------------------------------------*//**
  AR00331 I2C device register definitions.
 */
#define AR0331_DEV_REG    (0x20 >> 1)  // To be defined

/*-------------------------------------------------------------------------*//**
  AR00331 I2C functional register definitions.
 */
#define RESET_REGISTER          0x301Au
#define DIGITAL_TEST            0x30B0u
#define DIGITAL_CTRL            0x30BAu
#define DATA_FORMAT_BITS        0x31ACu
#define Y_ADDR_START            0x3002u
#define Y_ADDR_END              0x3006u
#define X_ADDR_START            0x3004u
#define X_ADDR_END              0x3008u
#define FRAME_LENGTH_LINES      0x300Au
#define LINE_LENGTH_PCK         0x300Cu
#define COARSE_INTEGRATION_TIME 0x3012u
#define FINE_INTEGRATION_TIME   0x3014u
#define X_ODD_INC               0x30A2u
#define Y_ODD_INC               0x30A6u
#define READ_MODE               0x3040u
#define SERIAL_FORMAT           0x31AEu
#define DAC_LD_6_7              0x3ED2u

/* Mode setting regs */
#define OPERATION_MODE_CTRL 0x3082
#define COMPANDING          0x31D0
#define ADACD_PEDESTAL      0x320A
#define ALTM_OUT_PEDESTAL   0x2450
#define DATA_PEDESTAL       0x301E

/* PLL setting regs */
#define VT_PIX_CLK_DIV      0x302A
#define VT_SYS_CLK_DIV      0x302C
#define PRE_PLL_CLK_DIV     0x302E
#define PLL_MULTIPLIER      0x3030
#define OP_PIX_CLK_DIV      0x3036
#define OP_SYS_CLK_DIV      0x3038

/* Feature Control Regs */
#define ALTM_CONTROL                0x2400
#define ALTM_POWER_GAIN             0x2410
#define ALTM_POWER_OFFSET           0x2412
#define ALTM_CONTROL_KEY_K0         0x2442
#define ALTM_CONTROL_KEY_K01_LO     0x2444
#define ALTM_CONTROL_KEY_K01_HI     0x2446
#define ALTM_CONTROL_DAMPER         0x2440
#define ALTM_CONTROL_MIN_FACTOR     0x2438
#define ALTM_CONTROL_MAX_FACTOR     0x243A
#define ALTM_CONTROL_DARK_FLOOR     0x243C
#define ALTM_CONTROL_BRIGHT_FLOOR   0x243E
#define COMPANDING                  0x31D0
#define DATA_PEDESTAL               0x301E

#define DELTA_DK_CONTROL            0x3180

#define COLOR_MODE_SELECT           0x3070
#define COLOR_MODE_RED              0x3072
#define COLOR_MODE_GR1              0x3074
#define COLOR_MODE_GR2              0x3078
#define COLOR_MODE_BLUE             0x3076

#define I2C_DEVICE_ADDR  (0x20 >> 1)
#define I2C_DUMMY_ADDR   0x10u
#define TX_LENGTH        16u
#define RX_LENGTH        8u




#endif /* AR0331_H_ */

