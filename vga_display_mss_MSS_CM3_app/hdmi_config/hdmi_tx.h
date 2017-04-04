/*
 * hdmi_config.h
 *
 *  Created on: Jan 20, 2016
 *      Author: badal.nilawar
 */

#ifndef HDMI_TX_H_
#define HDMI_TX_H_

#include "mss_i2c/mss_i2c.h"

#define I2C_MUX_ADDR (0xE0 >> 1)
#define IO_EXP_ADDR (0x40 >> 1)
#define I2C_HDMI_OUT_ADDR (0x72 >> 1)
#define I2C_HDMI_IN_ADDR (0x98 >> 1)
#define I2C_VCS_ADDR (0xCA >> 1)

#define IIC_HDMI_OUT_CONFIG_LEN  (41)


#endif /* HDMI_TX_H_ */
