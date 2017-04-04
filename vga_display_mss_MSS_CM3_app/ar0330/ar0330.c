/*
 * ar0330.c
 *
 *  Created on: Mar 09, 2016
 *      Author: badal.nilawar
 */
#include <stdio.h>
#include "ar0330.h"

extern void msdelay(uint32_t tms);

static mss_i2c_instance_t * sensor_i2c = &g_mss_i2c0;

static uint8_t tx_buffer[64];
//static uint8_t rx_buffer[64];
static uint32_t write_length;
static mss_i2c_status_t status;

static mss_i2c_status_t sensor_i2c_write(uint16_t data_reg, uint16_t data) {
	tx_buffer[0] = data_reg >> 8;
	tx_buffer[1] = data_reg & 0xff;
	tx_buffer[2] = data >> 8;
	tx_buffer[3] = data & 0xff;
	write_length = sizeof(data_reg) + sizeof(data);

	MSS_I2C_write(sensor_i2c, AR0331_DEV_REG, (const uint8_t *) tx_buffer,
			write_length, MSS_I2C_RELEASE_BUS );

	status = MSS_I2C_wait_complete(sensor_i2c, MSS_I2C_NO_TIMEOUT );
	return status;
}

static mss_i2c_status_t sensor_i2c_read(uint16_t data_reg, uint16_t *data) {
	tx_buffer[0] = data_reg >> 8;
	tx_buffer[1] = data_reg & 0xff;
	write_length = sizeof(data_reg) + sizeof(data);

	MSS_I2C_write_read(sensor_i2c, AR0331_DEV_REG, tx_buffer, 2,
			(uint8_t *) data, 2, MSS_I2C_RELEASE_BUS );

	status = MSS_I2C_wait_complete(sensor_i2c, MSS_I2C_NO_TIMEOUT );
	return status;
}

static mss_i2c_status_t sensor_i2c_write_bits(uint16_t data_reg, uint16_t mask,
		uint8_t bitval) {
	uint16_t readdata;
	uint16_t writedata;

	sensor_i2c_read(data_reg, (uint16_t *) &readdata);

	tx_buffer[0] = data_reg >> 8;
	tx_buffer[1] = data_reg & 0xff;

	if (bitval == 0) {
		writedata = readdata & ~mask;
	} else {
		writedata = readdata | mask;
	}
	tx_buffer[2] = writedata >> 8;
	tx_buffer[3] = writedata & 0xff;
	write_length = sizeof(data_reg) + sizeof(writedata);

	MSS_I2C_write(sensor_i2c, AR0331_DEV_REG, (const uint8_t *) tx_buffer,
			write_length, MSS_I2C_RELEASE_BUS );

	status = MSS_I2C_wait_complete(sensor_i2c, MSS_I2C_NO_TIMEOUT );
	return status;
}

static void pll_settings_parallel() {
	sensor_i2c_write(VT_PIX_CLK_DIV, 0x0008);
	sensor_i2c_write(VT_SYS_CLK_DIV, 0x0001);
	sensor_i2c_write(PRE_PLL_CLK_DIV, 0x0002);
	sensor_i2c_write(PLL_MULTIPLIER, 0x0020);
	sensor_i2c_write(OP_PIX_CLK_DIV, 0x000C);
	sensor_i2c_write(OP_SYS_CLK_DIV, 0x0001);
}


static void ar0330_optm_v5_sequencer()
{

	//OTPM V5 Sequencer

	msdelay(100);


	sensor_i2c_write(0x3064, 0x1802);
	sensor_i2c_write(COLOR_MODE_GR2, 0x0001);
	sensor_i2c_write(0x31E0, 0x0703);
	sensor_i2c_write(0x306E, 0xFC10);

    sensor_i2c_write(0x30f0, 0x1200);
    sensor_i2c_write(COLOR_MODE_RED, 0x0000);
    sensor_i2c_write(0x30BA, 0x002C);
    sensor_i2c_write(0x30FE, 0x0080);
    sensor_i2c_write(0x31E0, 0x0000);

    sensor_i2c_write(0x3ED0, 0xE4F6);
    sensor_i2c_write(0x3ED2, 0x0146);
    sensor_i2c_write(0x3ED4, 0x8F6C);
    sensor_i2c_write(0x3ED6, 0x66CC);
    sensor_i2c_write(0x3ED8, 0x8C42);
    sensor_i2c_write(0x3EDA, 0x8822);
    sensor_i2c_write(0x3EDC, 0x2222);
    sensor_i2c_write(0x3EDE, 0x22C0);
    sensor_i2c_write(0x3EE0, 0x1500);
    sensor_i2c_write(0x3EE6, 0x0080);
    sensor_i2c_write(0x3EE8, 0x2027);
    sensor_i2c_write(0x3EEA, 0x001D);
    sensor_i2c_write(0x3F06, 0x046A);

    sensor_i2c_write(0x301a, 0x10c8);

	sensor_i2c_write(0x3088, 0x8000);
	sensor_i2c_write(0x3086, 0x4A03);
	sensor_i2c_write(0x3086, 0x4316);
	sensor_i2c_write(0x3086, 0x0443);
	sensor_i2c_write(0x3086, 0x1645);
	sensor_i2c_write(0x3086, 0x4045);
	sensor_i2c_write(0x3086, 0x6017);
	sensor_i2c_write(0x3086, 0x2045);
	sensor_i2c_write(0x3086, 0x404B);
	sensor_i2c_write(0x3086, 0x1244);
	sensor_i2c_write(0x3086, 0x6134);
	sensor_i2c_write(0x3086, 0x4A31);
	sensor_i2c_write(0x3086, 0x4342);
	sensor_i2c_write(0x3086, 0x4560);
	sensor_i2c_write(0x3086, 0x2714);
	sensor_i2c_write(0x3086, 0x3DFF);
	sensor_i2c_write(0x3086, 0x3DFF);
	sensor_i2c_write(0x3086, 0x3DEA);
	sensor_i2c_write(0x3086, 0x2704);
	sensor_i2c_write(0x3086, 0x3D10);
	sensor_i2c_write(0x3086, 0x2705);
	sensor_i2c_write(0x3086, 0x3D10);
	sensor_i2c_write(0x3086, 0x2715);
	sensor_i2c_write(0x3086, 0x3527);
	sensor_i2c_write(0x3086, 0x053D);
	sensor_i2c_write(0x3086, 0x1045);
	sensor_i2c_write(0x3086, 0x4027);
	sensor_i2c_write(0x3086, 0x0427);
	sensor_i2c_write(0x3086, 0x143D);
	sensor_i2c_write(0x3086, 0xFF3D);
	sensor_i2c_write(0x3086, 0xFF3D);
	sensor_i2c_write(0x3086, 0xEA62);
	sensor_i2c_write(0x3086, 0x2728);
	sensor_i2c_write(0x3086, 0x3627);
	sensor_i2c_write(0x3086, 0x083D);
	sensor_i2c_write(0x3086, 0x6444);
	sensor_i2c_write(0x3086, 0x2C2C);
	sensor_i2c_write(0x3086, 0x2C2C);
	sensor_i2c_write(0x3086, 0x4B01);
	sensor_i2c_write(0x3086, 0x432D);
	sensor_i2c_write(0x3086, 0x4643);
	sensor_i2c_write(0x3086, 0x1647);
	sensor_i2c_write(0x3086, 0x435F);
	sensor_i2c_write(0x3086, 0x4F50);
	sensor_i2c_write(0x3086, 0x2604);
	sensor_i2c_write(0x3086, 0x2684);
	sensor_i2c_write(0x3086, 0x2027);
	sensor_i2c_write(0x3086, 0xFC53);
	sensor_i2c_write(0x3086, 0x0D5C);
	sensor_i2c_write(0x3086, 0x0D57);
	sensor_i2c_write(0x3086, 0x5417);
	sensor_i2c_write(0x3086, 0x0955);
	sensor_i2c_write(0x3086, 0x5649);
	sensor_i2c_write(0x3086, 0x5307);
	sensor_i2c_write(0x3086, 0x5302);
	sensor_i2c_write(0x3086, 0x4D28);
	sensor_i2c_write(0x3086, 0x6C4C);
	sensor_i2c_write(0x3086, 0x0928);
	sensor_i2c_write(0x3086, 0x2C28);
	sensor_i2c_write(0x3086, 0x294E);
	sensor_i2c_write(0x3086, 0x5C09);
	sensor_i2c_write(0x3086, 0x6045);
	sensor_i2c_write(0x3086, 0x0045);
	sensor_i2c_write(0x3086, 0x8026);
	sensor_i2c_write(0x3086, 0xA627);
	sensor_i2c_write(0x3086, 0xF817);
	sensor_i2c_write(0x3086, 0x0227);
	sensor_i2c_write(0x3086, 0xFA5C);
	sensor_i2c_write(0x3086, 0x0B17);
	sensor_i2c_write(0x3086, 0x1826);
	sensor_i2c_write(0x3086, 0xA25C);
	sensor_i2c_write(0x3086, 0x0317);
	sensor_i2c_write(0x3086, 0x4427);
	sensor_i2c_write(0x3086, 0xF25F);
	sensor_i2c_write(0x3086, 0x2809);
	sensor_i2c_write(0x3086, 0x1714);
	sensor_i2c_write(0x3086, 0x2808);
	sensor_i2c_write(0x3086, 0x1616);
	sensor_i2c_write(0x3086, 0x4D1A);
	sensor_i2c_write(0x3086, 0x2683);
	sensor_i2c_write(0x3086, 0x1616);
	sensor_i2c_write(0x3086, 0x27FA);
	sensor_i2c_write(0x3086, 0x45A0);
	sensor_i2c_write(0x3086, 0x1707);
	sensor_i2c_write(0x3086, 0x27FB);
	sensor_i2c_write(0x3086, 0x1729);
	sensor_i2c_write(0x3086, 0x4580);
	sensor_i2c_write(0x3086, 0x1708);
	sensor_i2c_write(0x3086, 0x27FA);
	sensor_i2c_write(0x3086, 0x1728);
	sensor_i2c_write(0x3086, 0x5D17);
	sensor_i2c_write(0x3086, 0x0E26);
	sensor_i2c_write(0x3086, 0x8153);
	sensor_i2c_write(0x3086, 0x0117);
	sensor_i2c_write(0x3086, 0xE653);
	sensor_i2c_write(0x3086, 0x0217);
	sensor_i2c_write(0x3086, 0x1026);
	sensor_i2c_write(0x3086, 0x8326);
	sensor_i2c_write(0x3086, 0x8248);
	sensor_i2c_write(0x3086, 0x4D4E);
	sensor_i2c_write(0x3086, 0x2809);
	sensor_i2c_write(0x3086, 0x4C0B);
	sensor_i2c_write(0x3086, 0x6017);
	sensor_i2c_write(0x3086, 0x2027);
	sensor_i2c_write(0x3086, 0xF217);
	sensor_i2c_write(0x3086, 0x535F);
	sensor_i2c_write(0x3086, 0x2808);
	sensor_i2c_write(0x3086, 0x164D);
	sensor_i2c_write(0x3086, 0x1A16);
	sensor_i2c_write(0x3086, 0x1627);
	sensor_i2c_write(0x3086, 0xFA26);
	sensor_i2c_write(0x3086, 0x035C);
	sensor_i2c_write(0x3086, 0x0145);
	sensor_i2c_write(0x3086, 0x4027);
	sensor_i2c_write(0x3086, 0x9817);
	sensor_i2c_write(0x3086, 0x2A4A);
	sensor_i2c_write(0x3086, 0x0A43);
	sensor_i2c_write(0x3086, 0x160B);
	sensor_i2c_write(0x3086, 0x4327);
	sensor_i2c_write(0x3086, 0x9C45);
	sensor_i2c_write(0x3086, 0x6017);
	sensor_i2c_write(0x3086, 0x0727);
	sensor_i2c_write(0x3086, 0x9D17);
	sensor_i2c_write(0x3086, 0x2545);
	sensor_i2c_write(0x3086, 0x4017);
	sensor_i2c_write(0x3086, 0x0827);
	sensor_i2c_write(0x3086, 0x985D);
	sensor_i2c_write(0x3086, 0x2645);
	sensor_i2c_write(0x3086, 0x5C01);
	sensor_i2c_write(0x3086, 0x4B17);
	sensor_i2c_write(0x3086, 0x0A28);
	sensor_i2c_write(0x3086, 0x0853);
	sensor_i2c_write(0x3086, 0x0D52);
	sensor_i2c_write(0x3086, 0x5112);
	sensor_i2c_write(0x3086, 0x4460);
	sensor_i2c_write(0x3086, 0x184A);
	sensor_i2c_write(0x3086, 0x0343);
	sensor_i2c_write(0x3086, 0x1604);
	sensor_i2c_write(0x3086, 0x4316);
	sensor_i2c_write(0x3086, 0x5843);
	sensor_i2c_write(0x3086, 0x1659);
	sensor_i2c_write(0x3086, 0x4316);
	sensor_i2c_write(0x3086, 0x5A43);
	sensor_i2c_write(0x3086, 0x165B);
	sensor_i2c_write(0x3086, 0x4345);
	sensor_i2c_write(0x3086, 0x4027);
	sensor_i2c_write(0x3086, 0x9C45);
	sensor_i2c_write(0x3086, 0x6017);
	sensor_i2c_write(0x3086, 0x0727);
	sensor_i2c_write(0x3086, 0x9D17);
	sensor_i2c_write(0x3086, 0x2545);
	sensor_i2c_write(0x3086, 0x4017);
	sensor_i2c_write(0x3086, 0x1027);
	sensor_i2c_write(0x3086, 0x9817);
	sensor_i2c_write(0x3086, 0x2022);
	sensor_i2c_write(0x3086, 0x4B12);
	sensor_i2c_write(0x3086, 0x442C);
	sensor_i2c_write(0x3086, 0x2C2C);
	sensor_i2c_write(0x3086, 0x2C00);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);
	sensor_i2c_write(0x3086, 0x0000);

}


static void ar0330_sequencer_shortened()
{
	//Sequencer length is 1014clocks/row.
	//sensor_i2c_write_bits(0x301A, 0x0004, 0);   //Disable Streaming
	msdelay(100);
	sensor_i2c_write(0x3088, 0x80BA); 			//0x80AA
	sensor_i2c_write(0x3086, 0x0253);
}

void sensor_reset(void) {
	sensor_i2c_write(RESET_REGISTER, 0x01);
	sensor_i2c_write(RESET_REGISTER, 0x10D8);
}


void color_mode_select_none() {

	sensor_i2c_write(COLOR_MODE_RED, 0xFFFF);
	sensor_i2c_write(COLOR_MODE_GR1, 0xFFFF);
	sensor_i2c_write(COLOR_MODE_GR2, 0xFFFF);
	sensor_i2c_write(COLOR_MODE_BLUE, 0xFFFF);
	sensor_i2c_write(COLOR_MODE_SELECT, 0x00);
}

void color_mode_select_red() {

	sensor_i2c_write(COLOR_MODE_SELECT, 0x00);
	sensor_i2c_write(COLOR_MODE_RED, 0xFFFF);
	sensor_i2c_write(COLOR_MODE_GR1, 0x0);
	sensor_i2c_write(COLOR_MODE_BLUE, 0x0);
	sensor_i2c_write(COLOR_MODE_GR2, 0x0);
	sensor_i2c_write(COLOR_MODE_SELECT, 0x01);
}

void color_mode_select_green() {

	sensor_i2c_write(COLOR_MODE_SELECT, 0x00);
	sensor_i2c_write(COLOR_MODE_RED, 0x0);
	sensor_i2c_write(COLOR_MODE_GR1, 0xFFFF);
	sensor_i2c_write(COLOR_MODE_BLUE, 0x0);
	sensor_i2c_write(COLOR_MODE_GR2, 0xFFFF);
	sensor_i2c_write(COLOR_MODE_SELECT, 0x01);
}

void color_mode_select_blue() {

	sensor_i2c_write(COLOR_MODE_SELECT, 0x00);
	sensor_i2c_write(COLOR_MODE_RED, 0x0);
	sensor_i2c_write(COLOR_MODE_GR1, 0x0);
	sensor_i2c_write(COLOR_MODE_BLUE, 0xFFFF);
	sensor_i2c_write(COLOR_MODE_GR2, 0x0);
	sensor_i2c_write(COLOR_MODE_SELECT, 0x01);
}

void color_mode_bars() {

	sensor_i2c_write(COLOR_MODE_SELECT, 0x00);
	sensor_i2c_write(COLOR_MODE_RED, 0xFFFF);
	sensor_i2c_write(COLOR_MODE_GR1, 0xFFFF);
	sensor_i2c_write(COLOR_MODE_GR2, 0xFFFF);
	sensor_i2c_write(COLOR_MODE_BLUE, 0xFFFF);
	sensor_i2c_write(COLOR_MODE_SELECT, 0x02);
}

void color_mode_walking1s() {

	sensor_i2c_write(COLOR_MODE_SELECT, 0x00);
	sensor_i2c_write(COLOR_MODE_RED, 0xFFFF);
	sensor_i2c_write(COLOR_MODE_GR1, 0xFFFF);
	sensor_i2c_write(COLOR_MODE_GR2, 0xFFFF);
	sensor_i2c_write(COLOR_MODE_BLUE, 0xFFFF);
	sensor_i2c_write(COLOR_MODE_SELECT, 0x100);
}

void parallel_ar0330_1080_720_30fps(uint32_t mode)
{

	MSS_GPIO_set_output(MSS_GPIO_8, 0u);
	msdelay(5);
	MSS_GPIO_set_output(MSS_GPIO_8, 1u); // Bring camera out of reset

	MSS_I2C_init( sensor_i2c, I2C_DEVICE_ADDR, MSS_I2C_PCLK_DIV_256 );

	msdelay(100);
	pll_settings_parallel();
	msdelay(500);

    ar0330_optm_v5_sequencer();

	switch(mode)
	{
	case 1:
		color_mode_select_red();
		break;
	case 2:
		color_mode_select_green();
		break;
	case 3:
		color_mode_select_blue();
		break;
	case 4:
		color_mode_bars();
	case 5:
		color_mode_walking1s();
		break;
	case 0:
	default:
		color_mode_select_none();
		break;
	}

    sensor_i2c_write(0x301a,0x10d8);


    sensor_i2c_write(Y_ADDR_START, 0x00EA);
    sensor_i2c_write(X_ADDR_START, 0x00C6);
    sensor_i2c_write(Y_ADDR_END, 0x0521);
    sensor_i2c_write(X_ADDR_END, 0x0845);
    sensor_i2c_write(FRAME_LENGTH_LINES, 0x0523);
    sensor_i2c_write(LINE_LENGTH_PCK, 0x04DA);
    sensor_i2c_write(COARSE_INTEGRATION_TIME, 0x0521);
    sensor_i2c_write(X_ODD_INC, 0x0001);
    sensor_i2c_write(Y_ODD_INC, 0x0001);
    sensor_i2c_write(READ_MODE, 0x0000);
    sensor_i2c_write(SERIAL_FORMAT, 0x0301);


	sensor_i2c_write(0x3056, 0x90);
	sensor_i2c_write(0x3058, 0x90);
	sensor_i2c_write(0x305A, 0x90);
	sensor_i2c_write(0x305C, 0x90);
	sensor_i2c_write(0x305E, 0x90);

    sensor_i2c_write(0x3088,0x80BA);
    sensor_i2c_write(0x3086,0x0253);
	sensor_i2c_write_bits(0x3040, 0xC000, 1);  // Vertical and Horizontal Flip


	sensor_i2c_write(0x3060, 0);
	ar0330_sequencer_shortened();
	msdelay(500);
    sensor_i2c_write(0x301A,0x10dc);
}

void set_frame_length_lines(uint16_t frame_length)
{
	// frame_length_lines: the number of row periods per frame.
	sensor_i2c_write(FRAME_LENGTH_LINES,frame_length);
}

void set_line_length_pck(uint16_t line_length)
{
	// line_length_pck: The number of clocks required for each sensor row.
	sensor_i2c_write(LINE_LENGTH_PCK,line_length);
}

void set_coarse_integration_time(uint16_t coarse_integration_time)
{
	// coarse_integration_time: number of row periods between a row's reset and a row read.
	sensor_i2c_write(COARSE_INTEGRATION_TIME,coarse_integration_time);
}

void set_fine_integration_time(uint16_t fine_integration_time)
{
	// fine_integration_time: number of pixel clock periods between the row reset and row read.
	// NOTE: ON Semi recommends fine_integration_time left at 0.
	sensor_i2c_write(FINE_INTEGRATION_TIME,fine_integration_time);
}

// PLL Configuration functions
void set_vt_pix_clk_div(uint16_t clk_div)
{
	sensor_i2c_write(VT_PIX_CLK_DIV, clk_div);
}

void set_vt_sys_clk_div(uint16_t clk_div)
{
	sensor_i2c_write(VT_SYS_CLK_DIV, clk_div);
}

void set_pre_pll_clk_div(uint16_t clk_div)
{
	sensor_i2c_write(PRE_PLL_CLK_DIV, clk_div);
}

void set_pll_multiplier(uint16_t pll_mult)
{
	sensor_i2c_write(PLL_MULTIPLIER, pll_mult);
}
