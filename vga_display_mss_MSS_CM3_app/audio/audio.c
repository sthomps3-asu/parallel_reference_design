
#include "audio.h"
//#include "mss_uart.h"
#include "mss_spi.h"
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include "typedefs.h"
#include "chip.h"
#include "hbi.h"
#include "ZL38051_defaults.h" /* Load default configuration record */
//#include "ZL38051_P1.3.0.h"

#define CONFIG_ZL380XX_HBI_BOOT			1
#define CONFIG_ZL380XX_HBI_BOOT_STATIC  1
#define CONFIG_ZL380XX_HBI_LOAD_CFGREC  1

#define HEADER_STRING(s) #s
#define HEADER_I(name) HEADER_STRING(name)
#define HEADER(name) HEADER_I(name)

#if CONFIG_ZL380XX_HBI_BOOT
#if CONFIG_ZL380XX_HBI_BOOT_STATIC
/* compile in C-style firmware image */
#ifdef FWR_C_FILE
#include HEADER(FWR_C_FILE)
#else
# error "firmware c file not defined"
#endif
#endif
#endif

#if CONFIG_ZL380XX_HBI_LOAD_CFGREC
#if CONFIG_ZL380XX_HBI_LOAD_CFGREC_STATIC
#ifdef CFGREC_C_FILE
#include HEADER(CFGREC_C_FILE)
#else
//# error "config record file not defined"
#include "test.h"
#endif
#endif
#endif

#if CONFIG_ZL380XX_HBI_I2C
static int bus_num = 0;
static int dev_id = 0x45;
#else
static int bus_num = 1; //changed by DARSHAK
static int dev_id = 0;
#endif

static hbi_handle_t handle;
#define CFGREC_BLOCK_SIZE 512
#define CFGREC_SIZE       512//0x1000

#define ahb_config      0x70000000

struct {
    reg_addr_t reg;
    unsigned char buf[CFGREC_SIZE];
    size_t size;
}cfgrec;

#if CONFIG_ZL380XX_HBI_BOOT
static hbi_status_t hbi_wr_bin_fw(hbi_handle_t handle,
                                 unsigned char *buf,
                                 size_t size,
                                 size_t block_size)
{
   hbi_status_t         status = HBI_STATUS_SUCCESS;
   int                  i=0;
   size_t               chunk_len;
   hbi_data_t           fwrimg;


   /* convert to bytes */
   chunk_len = block_size << 1;

   printf("Retrieved Block size of %d\n",block_size);

   do
   {
     if(size  < chunk_len || !chunk_len)
     {
         printf("Incomplete data read.requested %d, read %d\n",chunk_len,size);
         return HBI_STATUS_RESOURCE_ERR;
     }

     fwrimg.pData = &buf[i];
     fwrimg.size = chunk_len;

     status = HBI_set_command(handle,HBI_CMD_LOAD_FWR_FROM_HOST, &fwrimg);
     if ((status != HBI_STATUS_SUCCESS &&  status != HBI_STATUS_OP_INCOMPLETE))
      {
         printf("HBI_write failed\n");
         return status;
      }
      i+=chunk_len;
   }while(i<size);
   printf("2- Conclude boot image loading....\n");

   status = HBI_set_command(handle,HBI_CMD_LOAD_FWR_COMPLETE,NULL);
   if (status != HBI_STATUS_SUCCESS)
   {
     printf("Error %d:HBI_CMD_BOOT_COMPLETE failed!\n", status);
   }

   return status;
}

/*
 * 	This function loads config records to RAM of Timberwolf. The function needs input file as C .h file and not as .cr2.
 * 	To convert .cr2 file to .h file, twConvertFirmware2c.exe utility is required.
 * 	command: twConvertFirmware2c.exe -i ZLS38051_defaults.cr2 -o  configRecord -b 1
 * 	Include configRecord.h file in the project and pass to this function as argument.
 */
hbi_status_t LoadCfgRecFromHeader()
{
    hbi_status_t   status = HBI_STATUS_SUCCESS;
    dataArr		   config_record = {0};
    int			   size = configStreamLen;
    int 		   i = 0;
    unsigned char value[2];
    printf("LoadCfgRecFromHeader() ..\n");

    do
    {
      config_record.reg = st_twConfig[i].reg;
      value[0] = st_twConfig[i].value[0]  >> 8;
      value[1] = st_twConfig[i].value[0] & 0xFF;

      status = HBI_write(handle,config_record.reg,(user_buffer_t *)&value[0],2);

      if ((status != HBI_STATUS_SUCCESS &&  status != HBI_STATUS_OP_INCOMPLETE))
       {
          printf("HBI_write failed\n");
          return status;
       }
       i++;
    }while(i < size);

    return status;
}

static tw_status_t LoadFirmware()
{
#if CONFIG_ZL380XX_HBI_BOOT_STATIC
    hbi_status_t   status = HBI_STATUS_SUCCESS;
    tw_status_t    tw_status = TW_STATUS_SUCCESS;
    hbi_data_t 	   fwrimg;
    int			   total_img_len=1;
    int 		   block_size;
    int 		   hdr_len;
    hbi_img_hdr_t  hdr;

	/* If this is the first chunk received, parse header and
       get total image length */
	fwrimg.pData = buffer;
	fwrimg.size = sizeof(buffer);
	status = HBI_get_header(&fwrimg,&hdr);
	if(status != HBI_STATUS_SUCCESS) {
		printf("Error ! Invalid Image Header Found\n");
		tw_status = TW_STATUS_ERROR_INIT;
		return tw_status;
	}

	total_img_len = hdr.img_len;
	block_size = hdr.block_size;
	hdr_len = hdr.hdr_len;

	status = hbi_wr_bin_fw(handle,&(buffer[hdr_len]),total_img_len,block_size);

	if (status != HBI_STATUS_SUCCESS) {
		printf("Error %d:HBI_CMD_LOAD_FWR_FROM_HOST failed\n", status);
		tw_status = TW_STATUS_FW_LOAD_ERROR;
		return tw_status;
	}
	else {
		printf("-- Firmware data transfer - done....\n");
	}
    return tw_status;
#else
    printf("Dynamic boot loading not supported \n");
    tw_status = TW_STATUS_ERROR;
    return tw_status;
#endif
}
#endif /* CONFIG_ZL380XX_HBI_BOOT*/

/*	This function initialises TW platform. This function flashes firmware and configuration record
 * hence should ideally be called once. TW supports loading of multiple firmware however for simplicity
 * the function will return error if it is already initialized after calling this function. This function
 * should be called first when flash is empty and also immediately after calling TW_Audio_DeInit() function.
 */
tw_status_t TW_Audio_Init() {

    hbi_status_t 	status = HBI_STATUS_SUCCESS;
    tw_status_t  	tw_status = TW_STATUS_SUCCESS;
    uint8_t 		val[32];
    hbi_dev_cfg_t 	devcfg;
    uint32_t		i;

    /* Update configuration record to put into TW-I2S loopback
     * 0x0210, 0x0005, CpDac1Src,          CP01 DAC1 Source
     * 0x0212, 0x0006, CpDac2Src,          CP02 DAC2 Source
     * 0x0214, 0x0001, CpTdma1Src,         CP03 I2S-1L/TDMA-1 Source
     * 0x0216, 0x0001, CpTdma2Src,         CP04 I2S-1R/TDMA-2 Source
     * 0x02B0, 0x000F, DigMicCfg,          Digital MIC Configuration
     */
    dataArr		   i2s_loopback_record[6] = {
											 {0x0210, 0x0005} ,
											 {0x0212, 0x0006} ,
											 {0x0214, 0x0001} ,
											 {0x0216, 0x0001} ,
											 {0x02B0, 0x000F} ,
											 {0x0262, 0x81F2}
											};
    uint32_t		loopCount = sizeof(i2s_loopback_record)/sizeof(dataArr);

    status = HBI_init(NULL);
    if (status != HBI_STATUS_SUCCESS) {
        printf("HBI_init Failed!\n");
        tw_status = TW_STATUS_ERROR_INIT;
        return tw_status;
    }

    devcfg.dev_addr = dev_id;
    devcfg.bus_num = bus_num;

    status = HBI_open(&handle,&devcfg);

    if (status != HBI_STATUS_SUCCESS) {
        printf("Error %d:HBI_open()\n", status);
        HBI_term();
        tw_status = TW_STATUS_ERROR_INIT;
        return tw_status;
    }

#if 0
    /* take firmware product code information before updating */
    printf("Firmware Product code before\n");
    status = HBI_read(handle,0x0020,val,12);
    if(status == HBI_STATUS_SUCCESS) {
        for(i=0;i<sizeof(val);i++)
            printf("0x%x\t",val[i]);
       printf("\n");
    } else {
		HBI_close(handle);
		HBI_term();
    	tw_status = TW_STATUS_SPI_ERROR;
    	return tw_status;
    }
#endif

    /* Get number of images in flash */
    status = HBI_read(handle,0x0026,val,sizeof(val));

    if(status == HBI_STATUS_SUCCESS) {
		if(val[1] == 1) {
			/* Don't close & terminate HBI instance */
			tw_status = TW_STATUS_ALREADY_INIT;
			return tw_status;
		}
    }

#if 1
    printf("Load firmware...\n");
    tw_status = LoadFirmware();
    if (tw_status != TW_STATUS_SUCCESS) {
        printf("Error %d:HBI_open()\n", tw_status);
		HBI_close(handle);
		HBI_term();
        tw_status = TW_STATUS_ERROR_INIT;
        return tw_status;
    }

    printf("Load config record from ..\n");
    status = LoadCfgRecFromHeader();
    if (tw_status != TW_STATUS_SUCCESS) {
        printf("Error %d:HBI_open()\n", tw_status);
		HBI_close(handle);
		HBI_term();
        tw_status = TW_STATUS_CFG_LOAD_ERROR;
        return tw_status;
    }

    for (i=0; i<loopCount; i++) {

    	val[0] = i2s_loopback_record[i].value[0]  >> 8;
    	val[1] = i2s_loopback_record[i].value[0] & 0xFF;

		status = HBI_write(handle,i2s_loopback_record[i].reg,(user_buffer_t *)&val[0],2);

		if ((status != HBI_STATUS_SUCCESS) &&  (status != HBI_STATUS_OP_INCOMPLETE)) {
			printf("HBI_write failed\n");
			return status;
		}
    }
#if 0
    printf("-- Saving firmware / config to flash....\n");
    status = HBI_set_command(handle,HBI_CMD_SAVE_FWRCFG_TO_FLASH,NULL);
    if (status != HBI_STATUS_SUCCESS) {
         printf("Error %d:HBI_CMD_SAVE_FWRCFG_TO_FLASH failed\n", status);
         HBI_close(handle);
         HBI_term();
         tw_status = TW_STATUS_FLASH_WRITE_ERROR;
         return status;
    }
#endif
#endif
    //printf("-- Saving to flash....done\n");

    return tw_status;
}

/* This function de-initialises TW platform. This function removes firmware and configuration record
 * from flash. After calling this function, Tw won't able to run the audio application hence higher level
 * application shall call TW_Audio_Init.
 */
tw_status_t TW_Audio_DeInit() {
    hbi_status_t 	status = HBI_STATUS_SUCCESS;
    tw_status_t  	tw_status = TW_STATUS_SUCCESS;
    uint8_t 		val[2];
    hbi_dev_cfg_t 	devcfg;
    uint32_t		i;

    /* take firmware product code information before updating */
    printf("Firmware Product code before\n");
    status = HBI_read(handle,0x0020,val,sizeof(val));
    if(status == HBI_STATUS_SUCCESS) {
        for(i=0;i<sizeof(val);i++)
            printf("0x%x\t",val[i]);
       printf("\n");
    } else {
    	tw_status = TW_STATUS_SPI_ERROR;
    	return tw_status;
    }

	/* Erase the full content of the ZL380x0 controlled slave flash */
	status  = HBI_set_command(handle, HBI_CMD_ERASE_WHOLE_FLASH, NULL);
	if (status != HBI_STATUS_SUCCESS) {
		printf("Error %d:HBI_set_command(HBI_CMD_ERASE_WHOLE_FLASH)\n", status);
		HBI_close(handle);
		HBI_term();
		tw_status = TW_STATUS_FLASH_ERASE_ERROR;
		return tw_status;
	}
	printf("flash erasing completed successfully...\n");

	HBI_close(handle);
	HBI_term();
    return tw_status;
}

tw_status_t TW_Audio_Erase() {

    hbi_status_t 	status = HBI_STATUS_SUCCESS;
    tw_status_t  	tw_status = TW_STATUS_SUCCESS;
    uint8_t 		val[32];
    hbi_dev_cfg_t 	devcfg;
    uint32_t		i;

    status = HBI_init(NULL);
    if (status != HBI_STATUS_SUCCESS) {
        printf("HBI_init Failed!\n");
        tw_status = TW_STATUS_ERROR_INIT;
        return tw_status;
    }

    devcfg.dev_addr = dev_id;
    devcfg.bus_num = bus_num;

    status = HBI_open(&handle,&devcfg);

    if (status != HBI_STATUS_SUCCESS) {
        printf("Error %d:HBI_open()\n", status);
        HBI_term();
        tw_status = TW_STATUS_ERROR_INIT;
        return tw_status;
    }

    tw_status = TW_Audio_DeInit();

    if (tw_status != TW_STATUS_SUCCESS) {
        printf("Error %d:HBI_open()\n", tw_status);
		HBI_close(handle);
		HBI_term();
        return tw_status;
    }

    HBI_close(handle);
    HBI_term();

    return tw_status;


}
/* This function does audio loop-back at TW level. The audio won't be transferred outside TW - not to I2S
 * or FPGA. This function shall be called after calling TW_Audio_Init().
 */
tw_status_t TW_Audio_Loopback(tw_command_t tw_command) {

    hbi_status_t 	status = HBI_STATUS_SUCCESS;
    tw_status_t  	tw_status = TW_STATUS_SUCCESS;
    uint8_t 		val[2];
    hbi_dev_cfg_t 	devcfg;
    uint32_t		i;
    int16_t 		image_number;

    /* take firmware product code information before updating */
    printf("Firmware Product code before\n");
    status = HBI_read(handle,0x0020,val,sizeof(val));
    if(status == HBI_STATUS_SUCCESS) {
        for(i=0;i<sizeof(val);i++)
            printf("0x%x\t",val[i]);
       printf("\n");
    } else {
    	tw_status = TW_STATUS_SPI_ERROR;
    	return tw_status;
    }

    if (tw_command == TW_COMMAND_START) {
		/* Load firmware from flash */
		printf("-- Loading firmware from flash....\n");
		image_number = 1;
		val[0] = image_number >> 8;
		val[1] = image_number & 0xFF;
		status = HBI_set_command(handle,HBI_CMD_LOAD_FWRCFG_FROM_FLASH,val);
		if (status != HBI_STATUS_SUCCESS) {
			printf("Error %d:HBI_CMD_LOAD_FWRCFG_FROM_FLASH failed\n", status);
			HBI_close(handle);
			HBI_term();
			tw_status = TW_STATUS_FW_LOAD_ERROR;
			return tw_status;
		}
		printf("-- Loading firmware....done\n");

		printf("-- Starting firmware....\n");
		status = HBI_set_command(handle,HBI_CMD_START_FWR,NULL);
		if (status != HBI_STATUS_SUCCESS) {
			printf("Error %d:HBI_CMD_START_FWR failed\n", status);
			HBI_close(handle);
			HBI_term();
			tw_status = TW_STATUS_START_FW_ERROR;
			return tw_status;
		}
		printf("-- Starting firmware....done\n");

    } else if (tw_command == TW_COMMAND_STOP) {
		printf("-- Stop firmware....\n");

		tw_status = TW_STATUS_NOT_SUPPORTED;
		return tw_status;
    }

    printf("-- Stopping firmware....done\n");

    return tw_status;
}

/* This function does audio loop-back from TW->SF2 I2S->TW. The audio is transferred outside TW - to I2S of SF2.
 * This function shall be called after calling TW_Audio_Init() function.
 */
tw_status_t TW_Audio_SF2I2S_Loopback(tw_command_t tw_command) {

    hbi_status_t 	status = HBI_STATUS_SUCCESS;
    tw_status_t  	tw_status = TW_STATUS_SUCCESS;
    uint8_t 		val[2];
    hbi_dev_cfg_t 	devcfg;
    int16_t 		image_number;
    unsigned int Read0, Read1, Read2, Read3, Read4, Read5, Read6 , Read7;
#if 0
    /* Update configuration record to put into TW-I2S loopback
     * 0x0210, 0x0005, CpDac1Src,          CP01 DAC1 Source
     * 0x0212, 0x0006, CpDac2Src,          CP02 DAC2 Source
     * 0x0214, 0x0001, CpTdma1Src,         CP03 I2S-1L/TDMA-1 Source
     * 0x0216, 0x0001, CpTdma2Src,         CP04 I2S-1R/TDMA-2 Source
     * 0x02B0, 0x000F, DigMicCfg,          Digital MIC Configuration
     */
    dataArr		   i2s_loopback_record[6] = {
											 {0x0210, 0x0005} ,
											 {0x0212, 0x0006} ,
											 {0x0214, 0x0001} ,
											 {0x0216, 0x0001} ,
											 {0x02B0, 0x000F} ,
											 {0x0262, 0x81F2}
											};
    uint32_t		i, loopCount = sizeof(i2s_loopback_record)/sizeof(dataArr);

    /* I2S Control Registers */
    *(volatile unsigned int *)(ahb_config + 0x4)      = 0x00000006;
    Read0 =  *(volatile unsigned int *)(ahb_config + 0x4);
    //I2S Wake Up Registers
    *(volatile unsigned int *)(ahb_config + 0x8)      = 0x00000001;
    //I2S sample width Registers
    *(volatile unsigned int *)(ahb_config + 0xc)      = 0x00000010;
    //Enabling I2S controller on RX side
    *(volatile unsigned int *)(ahb_config + 0x000)   = 0x0000003f;
#endif

#if 0
    /* Take firmware product code information before updating */
    printf("Firmware Product code before\n");
    status = HBI_read(handle,0x0020,val,sizeof(val));
    if(status == HBI_STATUS_SUCCESS) {
        for(i=0;i<sizeof(val);i++)
            printf("0x%x\t",val[i]);
       printf("\n");
    } else {
    	tw_status = TW_STATUS_SPI_ERROR;
    	return tw_status;
    }
#endif

#if 0
    printf("Load firmware...\n");
    tw_status = LoadFirmware();
    if (tw_status != TW_STATUS_SUCCESS) {
        printf("Error %d:HBI_open()\n", tw_status);
		HBI_close(handle);
		HBI_term();
        tw_status = TW_STATUS_ERROR_INIT;
        return tw_status;
    }

    printf("Load config record from ..\n");
    status = LoadCfgRecFromHeader();
    if (tw_status != TW_STATUS_SUCCESS) {
        printf("Error %d:HBI_open()\n", tw_status);
		HBI_close(handle);
		HBI_term();
        tw_status = TW_STATUS_CFG_LOAD_ERROR;
        return tw_status;
    }

    for (i=0; i<loopCount; i++) {

    	val[0] = i2s_loopback_record[i].value[0]  >> 8;
    	val[1] = i2s_loopback_record[i].value[0] & 0xFF;

		status = HBI_write(handle,i2s_loopback_record[i].reg,(user_buffer_t *)&val[0],2);

		if ((status != HBI_STATUS_SUCCESS) &&  (status != HBI_STATUS_OP_INCOMPLETE)) {
			printf("HBI_write failed\n");
			return status;
		}
    }
#endif

#if 0
    printf("-- Saving firmware / config to flash....\n");
    status = HBI_set_command(handle,HBI_CMD_SAVE_FWRCFG_TO_FLASH,NULL);
    if (status != HBI_STATUS_SUCCESS) {
         printf("Error %d:HBI_CMD_SAVE_FWRCFG_TO_FLASH failed\n", status);
         HBI_close(handle);
         HBI_term();
         tw_status = TW_STATUS_FLASH_WRITE_ERROR;
         return status;
    }
    printf("-- Saving to flash....done\n");
#endif
    if (tw_command == TW_COMMAND_START) {

#if 0
		/* Load firmware from flash */
		printf("-- Loading firmware from flash....\n");
		image_number = 1;
		val[0] = image_number >> 8;
		val[1] = image_number & 0xFF;
		status = HBI_set_command(handle,HBI_CMD_LOAD_FWRCFG_FROM_FLASH,val);
		if (status != HBI_STATUS_SUCCESS) {
			printf("Error %d:HBI_CMD_LOAD_FWRCFG_FROM_FLASH failed\n", status);
			HBI_close(handle);
			HBI_term();
			tw_status = TW_STATUS_FW_LOAD_ERROR;
			return tw_status;
		}
		printf("-- Loading firmware....done\n");
#endif

		printf("-- Starting firmware....\n");
		status = HBI_set_command(handle,HBI_CMD_START_FWR,NULL);
		if (status != HBI_STATUS_SUCCESS) {
			printf("Error %d:HBI_CMD_START_FWR failed\n", status);
			HBI_close(handle);
			HBI_term();
			tw_status = TW_STATUS_START_FW_ERROR;
			return tw_status;
		}
		printf("-- Starting firmware....done\n");

    } else if (tw_command == TW_COMMAND_STOP) {
		printf("-- Stop firmware....\n");

		tw_status = TW_STATUS_NOT_SUPPORTED;
		return tw_status;
    }

    printf("-- Starting firmware....done\n");

    return tw_status;

}

/* This function stores audio in RAM of SF2. Audio travels from TW->SF2 I2S->RAM.
 * This function shall be called after calling TW_Audio_Init() function.
 */
void TW_Audio_SF2RAM_CAPTURE(tw_command_t tw_command) {

}

/* This function retrieves audio from RAM of SF2. Audio travels from RAM->SF2 I2S->TW.
 * This function shall be called after calling TW_Audio_SF2I2S_Loopback() function.
 */
void TW_Audio_SF2RAM_PLAYBACK(tw_command_t tw_command) {

}
/* Workaround to create and delete audio loop back path */
#if 1
void TW_Audio_SF2I2S_HW_Loopback(uint8_t enable)
{
	unsigned int Read0;

	if(enable)
	{
		/* I2S Control Registers */
		*(volatile unsigned int *)(ahb_config + 0x4)	= 0x00000006;
		Read0 =  *(volatile unsigned int *)(ahb_config + 0x4);
		//I2S Wake Up Registers
		*(volatile unsigned int *)(ahb_config + 0x8)    = 0x00000001;
		//I2S sample width Registers
		*(volatile unsigned int *)(ahb_config + 0xc)    = 0x00000010;
		//Enabling I2S controller on RX side
		*(volatile unsigned int *)(ahb_config + 0x000)  = 0x0000003f;
	}
	else
	{
		/* I2S Control Registers */
		*(volatile unsigned int *)(ahb_config + 0x4)	= 0;
		Read0 =  *(volatile unsigned int *)(ahb_config + 0x4);
		//I2S Wake Up Registers
		*(volatile unsigned int *)(ahb_config + 0x8)    = 0;
		//I2S sample width Registers
		*(volatile unsigned int *)(ahb_config + 0xc)    = 0;
		//Enabling I2S controller on RX side
		*(volatile unsigned int *)(ahb_config + 0x000)  = 0;

	}
}
#endif
