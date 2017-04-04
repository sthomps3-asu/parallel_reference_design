#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include "typedefs.h"
#include "chip.h"
#include "hbi.h"
#include "ZL38051_defaults.h" /* Load default configuration record */
//#include "ZL38051_Loopback.h"   /* Load loopback (Mic1->Dec1)configuration record */

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

#undef SAVE_IMAGE_TO_FLASH /*define this macro to save the firmware from RAM to flash*/
#define TEST_ERASE_FLASH

#define SAVE_FWRCFG_TO_FLASH
#define DBG_BOOT 1
#define DBG_CFGREC 0

static hbi_handle_t handle;
#define CFGREC_BLOCK_SIZE 512
#define CFGREC_SIZE       512//0x1000

struct {
    reg_addr_t reg;
    unsigned char buf[CFGREC_SIZE];
    size_t size;
}cfgrec;

#if CONFIG_ZL380XX_HBI_LOAD_CFGREC
#if CONFIG_ZL380XX_HBI_LOAD_CFGREC_STATIC
hbi_status_t VprocLoadConfig(const dataArr  *cfgrec,uint16_t len)
{
    int i;
    hbi_status_t status;
    uint16_t val;
    
    printf("- Loading the config file into the device RAM....\n");

    if(cfgrec == NULL)
    {
        return HBI_STATUS_INVALID_ARG;
    }
    for(i=0;i<len;i++)
    {
        status = HBI_write(handle,
                           (cfgrec[i].reg),
                           (user_buffer_t *)(cfgrec[i].value),
                           ZL380XX_CFG_BLOCK_SIZE*2);
        if(status != HBI_STATUS_SUCCESS)
        {
            printf("HBI write failed(reg 0x%x .\n",(cfgrec[i].reg));
            return status;
        }
    }
    val = ZL380xx_HOST_SW_FLAGS_APP_REBOOT;
    status = HBI_write(handle,ZL380xx_HOST_SW_FLAGS_REG,(user_buffer_t *)&val,2);
    if(status != HBI_STATUS_SUCCESS)
    {
        printf("[%s:%d] failed\n",__FUNCTION__,__LINE__);
    }
    return status;
}
#else

static uint32_t AsciiHexToHex(const char * str, unsigned char len)
{
    uint32_t val = 0;
    char c;
    unsigned char i = 0;

    for (i = 0; i< len; i++)    
    {
        c = *str++; 


        if (c >= '0' && c <= '9')
        {
            val <<= 4;
            val += c & 0x0F;
            continue;
        }

        c &= 0xDF;
        if (c >= 'A' && c <= 'F')
        {
            val <<= 4;
            val += (c & 0x07) + 9;
            continue;
        }
    }
    return val;
}

#if 1
//Added by DARSHAK
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
#endif

hbi_status_t LoadCfgRec()
{
    hbi_status_t status;
    unsigned char line[CFGREC_BLOCK_SIZE];
    FILE *fp=NULL;
    int bytestoread = CFGREC_BLOCK_SIZE;
    int numbytesread = 0;
    int eof=0;
    int index=0,i;

    uint16_t value;
    reg_addr_t reg;

    printf("LoadCfgRec() ..\n");

    /*make sure sd-card has  image with the name given in file_fopen call */
    if((fp=fopen("ZLS38051_defaults.cr2", "rb"))==NULL)
    {
        printf("file open failed\n");
        return HBI_STATUS_INTERNAL_ERR;
    }

    printf("1- Opening file - done....\n");

    /*read and format the data accordingly*/
    int unprocessed=0;
    
    uint16_t prev=0;
    
    memset(&cfgrec,0,sizeof(cfgrec));
    do {
//        printf("bytestoread %d\n",bytestoread);
        if(!bytestoread)
            break;
      
         numbytesread = fread(&line[unprocessed],1,bytestoread,fp);

         if(numbytesread < bytestoread)
         {
            printf("EOF !!\n");
            eof=1;
         }
         i=0;
         bytestoread=CFGREC_BLOCK_SIZE;
         numbytesread+=unprocessed;
         unprocessed=0;
         printf("line %s, len %d\n",line,numbytesread);
         while(i < numbytesread)
         {
            int j;

             printf(" current indx %d\n",i);

             /* get 1 line */
             index = i;
             while(i < numbytesread && line[i++]!='\n');
             if(line[i-1]!='\n'/*i>=numbytesread*/) 
             {
                 unprocessed = i-index;
                 printf("unprocessed %d\n",unprocessed);
                /*copy unprocessed data to beginning of buffer */
                for(j=0;j<unprocessed;j++)
                    line[j]=line[index+j];
                bytestoread -= unprocessed;
                break;
            }

            if(line[index]==';')
            {
                continue;
            }
            
            if(line[index]!='\n')
            {
                /* read one line */

                while(memcmp(&line[index],"0x",2))
                    index++;

                index+=2;
                j=index;
                while(line[j++] != ',');

                reg = AsciiHexToHex(&line[index],j-index);

                printf("value at loc %d\n",j);
                index = j;
                
                while(memcmp(&line[index],"0x",2))
                    index++;

                index+=2;
                j=index;
                while(line[j++] != ',');

                value = AsciiHexToHex(&line[index],j-index);

                if(!prev)
                {
                    cfgrec.reg = reg;
                }
                else if(cfgrec.size >= 256 || (reg != prev+2))
                {
                    /* non-contigous*/
                    printf("Writing @ 0x%x, len %d to device\n",cfgrec.reg,cfgrec.size);
           //        status = HBI_write(handle,cfgrec.reg,cfgrec.buf,cfgrec.size);
                    cfgrec.reg = reg;
                    cfgrec.size=0;
                }
                *((uint16_t *)&(cfgrec.buf[cfgrec.size]))=value;
                printf("prev = 0x%x, reg = 0x%x, value = 0x%x, cfgrec.buf[%d]:0x%x\n", prev, reg, value,cfgrec.size,*((uint16_t*)&(cfgrec.buf[cfgrec.size])));
                cfgrec.size+=2;
                prev=reg;
            }

            /* continue to read rest of lines */
        }
    } while (eof == 0);
    
    printf("Writing @ 0x%x, val 0x%x to device\n",cfgrec.reg,*(cfgrec.buf));

    /* write last entry */
    //status = HBI_write(handle,cfgrec.reg,cfgrec.buf,cfgrec.size);


    fclose(fp);
    return status;
} 
#endif
#endif

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
int LoadFwr()
{
#if CONFIG_ZL380XX_HBI_BOOT_STATIC
    hbi_status_t   status = HBI_STATUS_SUCCESS;
    hbi_data_t 	   fwrimg;
    int			   total_img_len=1;
    int 		   block_size;
    int 		   hdr_len;
    hbi_img_hdr_t  hdr;

	/* if this the first chunk received, parse header and
            get total image len */
	fwrimg.pData = buffer;
	fwrimg.size = sizeof(buffer);
	 status = HBI_get_header(&fwrimg,&hdr);
	 if(status != HBI_STATUS_SUCCESS)
	 {
		printf("Error ! Invalid Image Header Found\n");
		return -1;
	 }

	total_img_len = hdr.img_len;
	block_size = hdr.block_size;
	hdr_len = hdr.hdr_len;

#if 1//DARSHAK
	status = hbi_wr_bin_fw(handle,&(buffer[hdr_len]),total_img_len,block_size);

	if (status != HBI_STATUS_SUCCESS) {
		printf("Error %d:HBI_CMD_LOAD_FWR_FROM_HOST failed\n", status);
	}
	else
		printf("-- Firmware data transfer - done....\n");
#endif
    return 0;
#else
    printf("Dynamic boot loading not supported \n");
    return -1;
#endif
}
#endif /* CONFIG_ZL380XX_HBI_BOOT*/

/*This example host app load the *.s3 firmware to the device RAM. Optionally save it to flash
 * Then start the firmware from the execution address in RAM
 */
int hbi_ldfwrcfg(int argc, char** argv) {
    hbi_status_t status = HBI_STATUS_SUCCESS;
    hbi_dev_cfg_t devcfg;
    int ret;
    uint8_t val[32];
    int16_t image_number;
#if (DBG_BOOT || DBG_CFGREC)
    int j,i;
    size_t size=12;
#endif

    status = HBI_init(NULL);
    if (status != HBI_STATUS_SUCCESS)
    {
        printf("HBI_init Failed!\n");
        
        return -1;
    }

    devcfg.dev_addr = dev_id; //changed by DARSHAK
    devcfg.bus_num = bus_num;	//changed by DARSHAK

    status = HBI_open(&handle,&devcfg);

    if (status != HBI_STATUS_SUCCESS) {
        printf("Error %d:HBI_open()\n", status);
        HBI_term();
        return -1;
    }
    
#if CONFIG_ZL380XX_HBI_BOOT
#if DBG_BOOT
        /* take firmware product code information before updating */
        printf("Firmware Product code before\n");
        status = HBI_read(handle,0x0020,val,size);
        if(status == HBI_STATUS_SUCCESS)
        {
            for(i=0;i<size;i++)
                printf("0x%x\t",val[i]);
           printf("\n");
        }
#endif


#ifdef TEST_ERASE_FLASH //Added here to check whether erasing flash is required before flashing firmware+image
{
    /*Erase the full content of the ZL380x0 controlled slave flash*/
    status  = HBI_set_command(handle, HBI_CMD_ERASE_WHOLE_FLASH, NULL);
    if (status != HBI_STATUS_SUCCESS) {
        printf("Error %d:HBI_set_command(HBI_CMD_ERASE_WHOLE_FLASH)\n", status);
        HBI_close(handle);
        HBI_term();
        return -1;
    }
    printf("flash erasing completed successfully...\n");
}
#endif

    ret = LoadFwr();

#if DBG_BOOT
        status = HBI_read(handle,0x0020,val,size);
        if(status == HBI_STATUS_SUCCESS)
        {
            for(i=0;i<size;i++)
                printf("0x%x\t",val[i]);
           printf("\n");
        }
#endif

#endif

#if CONFIG_ZL380XX_HBI_LOAD_CFGREC
        printf(" %s %d \n",__FUNCTION__,__LINE__);

#if CONFIG_ZL380XX_HBI_LOAD_CFGREC_STATIC
        status  = VprocLoadConfig((dataArr *)st_twConfig,
                                        (uint16_t)configStreamLen);
#else
        printf("Calling LoadCfgRec() ..\n");
        //status = LoadCfgRec(); //DARSHAK: file read is replaced by array read
        status = LoadCfgRecFromHeader();
#endif
    /*Firmware reset - in order for the configuration to take effect*/
    val[0] = ZL380xx_HOST_SW_FLAGS_APP_REBOOT >> 8;
    val[1] = ZL380xx_HOST_SW_FLAGS_APP_REBOOT & 0xFF;
    status  = HBI_write(handle,0x0006,val,2);
    if (status != HBI_STATUS_SUCCESS) 
    {
        printf("Error %d:HBI_reset()\n", status);
        return status;
    }
#if DBG_CFGREC
    /* Read back cfg record*/
    printf("Read back config record\n");
    char cfgrecval[256];
    size_t len=256;
    for(i=0x200;i<0x1000;i+=len)
    {
        status = HBI_read(handle,(reg_addr_t)i,(user_buffer_t *)cfgrecval,len);
        if(status == HBI_STATUS_SUCCESS)
        {
            for(j=0;j<len;j+=2)
                printf("0x%x,0x%x\n",i+j,*((u16*)&cfgrecval[j]));
        }
    }
#endif
#endif




#ifdef SAVE_FWRCFG_TO_FLASH
    if(status == HBI_STATUS_SUCCESS)
    {
       printf("-- Saving firmware / config to flash....\n");
       status = HBI_set_command(handle,HBI_CMD_SAVE_FWRCFG_TO_FLASH,NULL);
       if (status != HBI_STATUS_SUCCESS) {
            printf("Error %d:HBI_CMD_SAVE_FWRCFG_TO_FLASH failed\n", status);
            return status;
       }
        printf("-- Saving to flash....done\n");
    }
#endif

    if(status == HBI_STATUS_SUCCESS)
    {

        printf("-- Loading firmware from flash....\n");
        image_number = 1;
        val[0] = image_number >> 8;
        val[1] = image_number & 0xFF;
        status = HBI_set_command(handle,HBI_CMD_LOAD_FWRCFG_FROM_FLASH,val);
        if (status != HBI_STATUS_SUCCESS) {
             printf("Error %d:HBI_CMD_LOAD_FWRCFG_FROM_FLASH failed\n", status);
             return status;
        }
        printf("-- Loading firmware....done\n");

       printf("-- Starting firmware....\n");
       status = HBI_set_command(handle,HBI_CMD_START_FWR,NULL);
       if (status != HBI_STATUS_SUCCESS) {
            printf("Error %d:HBI_CMD_START_FWR failed\n", status);
            return status;
       }
        printf("-- Starting firmware....done\n");
    }

    HBI_close(handle);
    HBI_term();

    return 0;
}


