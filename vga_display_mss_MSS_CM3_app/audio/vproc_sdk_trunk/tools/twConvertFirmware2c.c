/* This utility converts the Timberwolf *.s3 and *.cr2 files into
 * c declaration files (*.c, *.h)
 * Example, to convert a file ZLS38040_v1.0.10.s3 to *.c and related *.h, run
 * twConvertRomfile2C <*.s3 or *.cr2 file to convert> <outputfilename.c>  -- without the <>
 * Make sure the *.img or the *.cr to convert is at the same path where the
 * the executable of this utility is located
 * 
 * Ex:
 * twConvertRomfile2C ZLS38040_v1.0.10.s3 zl38040_firmware.c [blockSize] 
 * firmware [blockSize] is optional, but if specified, its range is 16 to 128
 * or
 * twConvertRomfile2C ZLS38040_v1.0.10.cr2 zl38040_config.c  [blockSize]
 * config [blockSize] is optional, but if specified, its range is 1 to 128
 */


#include <stdlib.h> /* malloc, free, rand */
#include <stdio.h>
#include <stdarg.h>
#include <time.h>       /* time_t, struct tm, time, localtime */
#include <string.h>
#include <unistd.h>

/* length in bytes */
#define HBI_MAX_PAGE_LEN   256

#define BUF_LEN         1024

#define VER_LEN_WIDTH      1
#define FORMAT_LEN_WIDTH   1
#define OPN_LEN_WIDTH      2
#define CHUNK_LEN_WIDTH    2
#define TOTAL_LEN_WIDTH    4
#define RESERVE_LEN_WIDTH  2

#define FWR_CHKSUM_LEN     1

#define IMG_HDR_LEN    \
   (VER_LEN_WIDTH +FORMAT_LEN_WIDTH +  \
   OPN_LEN_WIDTH + CHUNK_LEN_WIDTH + TOTAL_LEN_WIDTH + RESERVE_LEN_WIDTH)

/* Image Version Info */
#define IMG_VERSION_MAJOR_SHIFT 6
#define IMG_VERSION_MINOR_SHIFT 4
#define IMG_VERSION_MAJOR       0
#define IMG_VERSION_MINOR       0
#define IMG_HDR_VERSION \
         ((IMG_VERSION_MAJOR << IMG_VERSION_MAJOR_SHIFT) | \
          (IMG_VERSION_MINOR << IMG_VERSION_MINOR_SHIFT))
    
/* image type */
#define IMG_HDR_TYPE_SHIFT    6
#define IMG_HDR_TYPE          (0<<IMG_HDR_TYPE_SHIFT) // 0 -fw, 1-cfg
#define IMG_HDR_ENDIAN_SHIFT  5
#define IMG_HDR_ENDIAN        (0<<IMG_HDR_ENDIAN_SHIFT) //0-big 1-little
#define IMG_HDR_FORMAT        (IMG_HDR_TYPE | IMG_HDR_ENDIAN)

/* TW registers */
#define PAGE255_REG                 0x000C
#define HOST_FWR_EXEC_REG           0x012C /*Fwr EXEC register*/


typedef unsigned short u16;

/* HBI Commands */
#define HBI_CONFIGURE(pinConfig) \
            ((u16)(0xFD00 | (pinConfig)))

#define HBI_DIRECT_PAGE_ACCESS_CMD_LEN 2
#define HBI_DIRECT_PAGE_ACCESS_CMD 0x80

#define HBI_SELECT_PAGE_CMD_LEN   2
#define HBI_SELECT_PAGE_CMD      0xFE
#define HBI_SELECT_PAGE(page)    ((u16)((HBI_SELECT_PAGE_CMD << 8) | page)))

#define HBI_PAGE_OFFSET_CMD_LEN        2
#define HBI_PAGED_OFFSET_MIN_DATA_LEN  2

#define HBI_CONT_PAGED_WR_CMD   0xFB 
#define HBI_CONT_PAGED_WRITE(length) ((u16)((HBI_CONT_PAGED_WR_CMD <<8) | length))

#define HBI_NO_OP_CMD 0xFF

#define OUTBUF_NAME "buffer"

unsigned char outbuf[BUF_LEN];
unsigned short fw_opn_code;

/*config record structures*/
typedef struct {
   unsigned short reg;   /*the register */
   unsigned short value[128]; /*the value to write into reg */
} dataArr;

char *outpath,*inpath;
unsigned short len;
unsigned int total_len=0;

FILE *saveFhande;
FILE *BOOT_FD;
unsigned short numElements;
int bOutputTypeC = 0;
dataArr *pCr2Buf;

unsigned short  zl_firmwareBlockSize = 16;
unsigned short  zl_configBlockSize = 1;

#undef DISPLAY_TO_TERMINAL  /*to see the data while being converted*/
#define strupr(func) func

#ifdef DEBUG
#define DBG printf
#else
#define DBG
#endif
void outLog(const char *str, ...);
void HbiPagedBootImage();


/* fseekNunlines() -- The firmware file is an ascii text file.
 * the information from fseek will not be usefull
 * this is our own fseek equivalent
 */
static unsigned long fseekNunlines(FILE *fptr) {
    unsigned short line_count = 0;
    int c;

    while ( (c=fgetc(fptr)) != EOF ) {
        if ( c == '\n' )
            line_count++;
    }
    len  = line_count-1;  
    return len;
}


/* readCfgFile() use this function to
 * Read the Voice processing cr2 config file into a C code
 * filepath -- pointer to the location where to find the file
 * pCr2Buf -- the actual firmware data array will be pointed to this buffer
 */
static int readCfgFile(char *filepath) {
    unsigned int reg[2], val[2];
    unsigned int done = 0;
    unsigned int index = 0, j = 1;
    char *s;
    char line[512] = "";
    int i =0, n = 0;

#if 0
    BOOT_FD = fopen(filepath, "rb");
#endif
    if (BOOT_FD != NULL) {
#if 0
        len = fseekNunlines(BOOT_FD);
        if (len <= 0) {
            printf("Error: file is not of the correct format...\n");
            return -1;
        }
        //printf("fileLength = %u\n", len);
        /*start at the beginning of the file*/
        //fseek(BOOT_FD, 0, SEEK_SET);
        rewind(BOOT_FD);
#endif
        /* allocate memory to contain the reg and val:*/
        pCr2Buf = (dataArr *) malloc(len*sizeof(dataArr));
        if (pCr2Buf == NULL) {
            printf ("not enough memory to allocate %u bytes.. ", len*sizeof(dataArr));
            return -1;
        }


        outLog("const dataArr st_twConfig[] ={\n");

        /*read and format the data accordingly*/
        numElements  = 0;
        do 
        { 
            //index = TWOLF_CONFIG_FIRST_REG;
            s = fgets(line, 512, BOOT_FD);
            if (line[0] == ';') {
             continue;
            }
            else  if (s != NULL) 
            {
                 numElements++;
                 sscanf(line, "%x %c %x", reg, s, val);
                 if (i <= (zl_configBlockSize -1)) 
                 {
                    if (index != j) 
                    { 
                        pCr2Buf[index].reg = reg[0];
                        j = index;
                    }
                    pCr2Buf[index].value[i] = val[0];
                    //printf("index =%d: reg = 0x%04x : val = 0x%04x\n", index, pCr2Buf[index].reg, pCr2Buf[index].value[i]);	
                    if (i == (zl_configBlockSize -1)) 
                    { 
                        i = 0;
                        index++;
                     } 
                     else
                        i++;
                    } 
            } 
            else 
            { 
                done = 1;
            }
        } while (done == 0);

        for (j =0; j < index; j++) {
            outLog("\t{0x%04X, {", pCr2Buf[j].reg);
            for (i =0; i < zl_configBlockSize; i++) {
            if (n++ < 8) {
            if (i < (zl_configBlockSize -1)) 
                outLog("0x%04X, ", pCr2Buf[j].value[i]);
            else 
                outLog("0x%04X", pCr2Buf[j].value[i]);
            } else {
             if (i < (zl_configBlockSize -1))
                 outLog("0x%04X, \n\t          ", pCr2Buf[j].value[i]);
             else
                 outLog("0x%04X", pCr2Buf[j].value[i]);
                 n = 0;
                }
            }
            outLog("}},\n");                
            n = 0;
        } 

        outLog("};\n");
        outLog("const unsigned short zl_configBlockSize = %u;\n", zl_configBlockSize);
#if 0
        fclose(BOOT_FD);
#endif
        free(pCr2Buf);
        numElements = index;
        //printf ("size of pCr2Buf = %u bytes.. \n", sizeof(pCr2Buf));
    } 
    else 
    {
        printf("Error: can't open file\n");
    }
    return 0;
}

void dumpFile(unsigned char *buf, int len)
{
   int j;

   if(bOutputTypeC)
   {
#if DBG_COUT
   for(j=0;j<len;j+=2)
      outLog("0x%04X, ",*((unsigned short *)&(buf[j])));
#else
   for(j=0;j<len;j++)
      outLog("0x%02X, ",buf[j]);
#endif
      outLog("\n");
   }
   else
   {
      fwrite(buf,1,len,saveFhande);
#ifdef DISPLAY_TO_TERMINAL
      for(j=0;j<len;j++)
         printf("0x%x ",buf[j]);
#endif
   }
   total_len+=len;
   return;
}

/* HbiPagedBootImage() - This function reads and process
 * the Voice processing s3 file into a HBI PAGED write command   
 * based image
 * FILE -- pointer to the location where to find the file
 */
void HbiPagedBootImage()
{
   char          line[1024] = "";
   int           rec_type;
   char          addrStr[5] = "";
   int           addrLen = 0;
   unsigned int  nextAddrToRead=0xFFFFFFFF;
   unsigned int  byteCount = 0;
   int           addrLenNumByteMap[10] = {4, 4, 6, 8, 0, 4, 0, 8, 6, 4};
   int           hbi_cmd_indx=-1;
   unsigned int   outDataLen=0;
   unsigned int  address;
   unsigned int  BaseAddr = 0xFFFFFFFF;
   unsigned char offset;
   int           j = 0,i;
   int           bCont=0;
   int           total_len_indx;
   time_t        rawtime;
   struct tm     *timeinfo;

   /* required for C-output, a single byte entry is encoded as 0x<val>, 
      totals upto 6
      */
   int chars_per_entry = 6;
   int offset_to_shift = 0;
   int current_fpos = 0;

   /*start at the beginning of the file*/
   fseek(BOOT_FD, 0, SEEK_SET);

   if (bOutputTypeC)
   {
      time (&rawtime);
      timeinfo = localtime (&rawtime);

      outLog ("/*Source file %s, modified: %s */ \n", inpath, asctime(timeinfo));  
      outLog("const unsigned char %s[] ={\n",OUTBUF_NAME);

      /* save current file position to be used later while writing
         header
      */
      current_fpos = ftell(saveFhande);
      /* Add 1 for newline char which is added in C- output */
      offset_to_shift = (IMG_HDR_LEN * chars_per_entry)+1; 
   }
   else
      offset_to_shift = IMG_HDR_LEN;

   /* 
      Leave space for header to be filled at the end of 
      function.
      Push the file pointer by the required offset
   */
   fseek(saveFhande,offset_to_shift,SEEK_CUR);

   memset(outbuf,0,sizeof(outbuf));


   /* HBI command to write page offset */
   outbuf[byteCount++] = HBI_SELECT_PAGE_CMD;
   outbuf[byteCount++] = 0xFF;

    while (fgets(line, 1024, BOOT_FD) != NULL) {
        int inPtr=0;
        int inDataLen = 0;

        /* if this line is not an srecord skip it */
        if (line[inPtr++] != 'S') {
            continue;
        }

        /* get the srecord type */
        rec_type = line[inPtr++] - '0';

        /* skip non-existent srecord types and block header */
        if ((rec_type == 4) || (rec_type == 6) || (rec_type == 0)) {
            continue;
        }

        /* get number of bytes */
        sscanf(&line[inPtr], "%02x", &inDataLen);

        inPtr += 2;

        /* get the info based on srecord type (skip checksum) */
        addrLen = addrLenNumByteMap[rec_type];

        sprintf(addrStr, "%%%ix", addrLen);
        
        sscanf(&line[inPtr], addrStr, &address);

        inDataLen -= (addrLen/2);
        inDataLen -= FWR_CHKSUM_LEN;

        inPtr += addrLen;

        DBG("address 0x%x, InDataLen %d nextAddrToRead 0x%x\n",
             address,inDataLen,nextAddrToRead);

        addrLen = addrLen >> 1;

        if((rec_type == 7) || (rec_type == 8) || (rec_type == 9))
        {
            if(bCont)
            {
               outbuf[hbi_cmd_indx] = ((outDataLen >> 1) - 1);
               bCont = 0;
            }
            else
            {
               outbuf[hbi_cmd_indx] = (0x80 | ((outDataLen>>1)-1));
            }
            outDataLen = 0;
            if((byteCount >= zl_firmwareBlockSize) || 
               (byteCount > (zl_firmwareBlockSize - (HBI_SELECT_PAGE_CMD_LEN+
                                                    HBI_PAGE_OFFSET_CMD_LEN+
                                                    addrLen))))
            {
               dumpFile(outbuf,byteCount);
               byteCount = 0;
               outDataLen=0;
            }
            /* write the address into Firmware Execution Address reg */
            outbuf[byteCount++] = HBI_SELECT_PAGE_CMD;
            outbuf[byteCount++] = (((HOST_FWR_EXEC_REG >> 8) & 0xFF)-1);


            outbuf[byteCount++] = (HOST_FWR_EXEC_REG & 0xFF) >> 1;
            outbuf[byteCount++] = ((addrLen >> 1) - 1)|0x80;

            for(i=(addrLen-1);i>=0;i--)
                outbuf[byteCount++] = ((address >> (8*i))& 0xFF);

            /* fill with HBI_NOOP CMD */
            while(byteCount < zl_firmwareBlockSize)
            {
               outbuf[byteCount++]= HBI_NO_OP_CMD;
               outbuf[byteCount++]= HBI_NO_OP_CMD;
            }

            /* write to file */
            dumpFile(outbuf, byteCount);

            printf("Firmware Read Complete\n");
            break;
        }

        if(address != nextAddrToRead)
        {

            DBG("Discontinuous Address !!!!!\n");
            if(hbi_cmd_indx >=0)
            {
               if(bCont)
               {
                  outbuf[hbi_cmd_indx] = ((outDataLen >> 1) - 1);
                  bCont = 0;
               }
               else
               {
                  outbuf[hbi_cmd_indx] = (0x80 | ((outDataLen>>1)-1));
               }
               outDataLen=0;
            }

            DBG(" %d : byteCount %d, nextAddrToRead 0x%x\n",
                     __LINE__,byteCount,nextAddrToRead);

            /* every HBI chunk need one full complete command
               thus check if length of remaining buffer is enough to
               hold one complete HBI command*/
            if ( byteCount >= ( zl_firmwareBlockSize - 
                              ( HBI_DIRECT_PAGE_ACCESS_CMD_LEN + addrLen +
                                HBI_PAGE_OFFSET_CMD_LEN + 
                                HBI_PAGED_OFFSET_MIN_DATA_LEN)))
            {
               /* dump data into output file */
               DBG("%d Insufficient space\n",__LINE__);

               while(byteCount<zl_firmwareBlockSize)
               {
                  outbuf[byteCount++] = 0xFF;
                  outbuf[byteCount++] = 0xFF;
               }

               dumpFile(outbuf, byteCount);

               byteCount =0;
               outDataLen=0;
            }

            /* 1. write page 255 base address register 0x000C */            
            outbuf[byteCount++] = HBI_DIRECT_PAGE_ACCESS_CMD | 
                                  ((PAGE255_REG & 0xFF) >> 1);
            outbuf[byteCount++] = ((addrLen>>1)-1) | 0x80;

            BaseAddr = address & 0xFFFFFF00;

            offset = (address & 0xFF)>>1;

            /* write data MSB */
            outbuf[byteCount++] = (BaseAddr >> 24) & 0xFF;
            outbuf[byteCount++] = (BaseAddr >> 16) & 0xFF;
            outbuf[byteCount++] = (BaseAddr >> 8) & 0xFF;
            outbuf[byteCount++] = BaseAddr & 0xff;

            outbuf[byteCount++] = offset;
            hbi_cmd_indx = byteCount++;  /* save for later use */

            nextAddrToRead = address;
        }

        /* copy block data into a buffer */
        for(i=0;i<inDataLen;i++,inPtr+=2)
        {
           DBG("i %d byteCount %d, outDataLen %d " \
               "nextAddrToRead 0x%x\n",i,byteCount,outDataLen,nextAddrToRead);

            if(outDataLen > 255)
            {
               printf("Error ! HBI Command payload exceeded max allowed "\
                        "for single write \n");
               return;
            }

            if((byteCount >= zl_firmwareBlockSize) || 
               (nextAddrToRead >= BaseAddr+HBI_MAX_PAGE_LEN))
            {
               DBG("dump data to file\n");

               /* data that have been written 
                  into outbuffer */

               if(bCont)
               {
                  outbuf[hbi_cmd_indx] = ((outDataLen >> 1) - 1);
                  bCont = 0;
               }
               else
               {
                  outbuf[hbi_cmd_indx] = (0x80 | ((outDataLen>>1)-1));
               }
               outDataLen=0;

               if(nextAddrToRead >= (BaseAddr+HBI_MAX_PAGE_LEN))
               {
                  DBG("Continuous Wr exceeded maximum supported \n");

                  BaseAddr = nextAddrToRead & 0xFFFFFF00;
                  offset = (nextAddrToRead & 0xFF) >> 1;

                  addrLen = sizeof(BaseAddr);

                  if(byteCount >= (zl_firmwareBlockSize - 
                                  (HBI_DIRECT_PAGE_ACCESS_CMD_LEN + 
                                  addrLen+2)))
                  {
                     
                     while(byteCount < zl_firmwareBlockSize)
                     {
                        DBG("Line %d fill with NOOP\n",__LINE__);
                        outbuf[byteCount++]=0xFF;
                        outbuf[byteCount++]=0xFF;
                     }

                     dumpFile(outbuf,byteCount);

                     byteCount =0;

                  }

                  
                  /* 1. write page 255 base address register 0x000C */            
                  outbuf[byteCount++] = HBI_DIRECT_PAGE_ACCESS_CMD | 
                                        ((PAGE255_REG & 0xFF) >> 1);
                  outbuf[byteCount++] = ((addrLen>>1)-1) | 0x80;

                   /* write data MSB */
                  outbuf[byteCount++] = (BaseAddr >> 24) & 0xFF;
                  outbuf[byteCount++] = (BaseAddr >> 16) & 0xFF;
                  outbuf[byteCount++] = (BaseAddr >> 8) & 0xFF;
                  outbuf[byteCount++] = BaseAddr & 0xFF;

                  outbuf[byteCount++] = offset;
               }
               else
               {
                  /* dump data into output file */
                  dumpFile(outbuf, byteCount);

                  byteCount = 0;
                  if(i < inDataLen-1)
                  {
                     /* continue writing remaining data into buffer
                        using continue paged access command*/
                     DBG("continue page wr\n");
                     outbuf[byteCount++] = HBI_CONT_PAGED_WR_CMD;
                     bCont=1;
                  }
               }
               hbi_cmd_indx = byteCount++;  /* save for later use */
            }

            sscanf(&line[inPtr],"%2x",&outbuf[byteCount++]);
            outDataLen++;
            nextAddrToRead++;
        }
    }  /*while end*/

    if(bOutputTypeC)
    {
      outLog("};\n");
    }

   /* write HBI header to file */   
   i = 0;

   outbuf[i++] = IMG_HDR_VERSION;
   outbuf[i++] = IMG_HDR_FORMAT;
   outbuf[i++] = (fw_opn_code >> 8) & 0xFF;
   outbuf[i++] = fw_opn_code & 0xFF;
   outbuf[i++] = (zl_firmwareBlockSize >>1) >> 8;
   outbuf[i++] = (zl_firmwareBlockSize >>1) & 0xFF;
   outbuf[i++] = total_len >> 24; 
   outbuf[i++] = total_len >> 16;
   outbuf[i++] = total_len >> 8; 
   outbuf[i++] = total_len & 0xFF; 
   outbuf[i++] = 0; //reserved
   outbuf[i++] = 0; //reserved

   if(i > IMG_HDR_LEN)
   {
      printf("Something wrong with header\n");
      return;
   }

   /* set to the header position */
   fseek(saveFhande,current_fpos,SEEK_SET);

   dumpFile(outbuf,i);

   DBG("total length of data written %d, block size %d\n",total_len,zl_firmwareBlockSize);
   return;
}


/* FUNCTION NAME :  outLog()
 * DESCRIPTION   :  This function is alternative to printf. It can output a buffer to a terminal or save it to disk
 */
void outLog(const char *str, ...)
{
    va_list ap;
    char buffer[600];
    va_start (ap, str);
    vsprintf (buffer, str, ap);
    va_end (ap);

    fputs(buffer,saveFhande); /*send to file*/  	

#ifdef DISPLAY_TO_TERMINAL
    printf (buffer); /*send to display terminal*/  
#endif /*DISPLAY_TO_TERMINAL*/ 
}

int main (int argc, char** argv) 
{
   long index=0, i=0;
   char *p, *p1;
   int flag = 0;
   unsigned long block_Size = 0;
   time_t rawtime;
   char c;
   struct tm * timeinfo;

   while((c = getopt(argc,argv,"i:o:b:f:h")) != -1)
   {
      switch(c){

      case 'i':
      inpath = optarg;
      DBG("inpath %s\n",inpath);
      break;

      case 'o':
      outpath = optarg;
      DBG("outpath %s\n",outpath);
      break;

      case 'b':
      block_Size = strtoul(optarg, NULL, 0);
      DBG("block size %u\n",block_Size);
      break;

      case'f':
      fw_opn_code = strtoul(optarg, NULL, 0);
      DBG("block size %u\n",fw_opn_code);
      break;

      case 'h':

      printf("Usage: %s -i [input filename] -o [output filename.bin/.c] " \
                "-b [block size] -f [firmware code] \n", argv[0]);

      printf (" -i: input image file (.s3 or .cr2) \n "\
              " -b: block size in unit of words with 16-bit word length \n" \
              " -o : output file name (please use .c as file extension " \
                    "for C output \n" \
              " -f : firmware code (applicable if regenerating firmware image \n" );

      printf("Image identification whether firmware or configuration " \
              "record is done dynamic based on file extension\n");
              
      printf("Example: to generate C file for zl38040 in blocks of 128 WORDS " \
             "with input file name ZLS38040_v1.0.10.s3 and output " \
             "zl38040_firmware.c " \
             "%s -i ZLS38040_v1.0.10.s3 -o zl38040_firmware.c -b 128 " \
             "-f 38040\n",argv[0]);
      return;
      }
   }

   if(inpath==NULL || outpath == NULL)
   {
      printf("Argument not been given appropraitly. please run %s -h\n",argv[0]);
      return;
   }

   /*check whether to convert a *.s3 or a*.cr2 file*/
   p1 = strstr (inpath, ".s3");

   if (p1 != NULL) 
   {
      flag = 1;
      if(!fw_opn_code)
      {
         printf("Need firmware code as input. for usage please run %s -h\n",
         argv[0]);
      }
      if ((block_Size <16) || (block_Size > 128)|| (block_Size % 16)) {
      printf("firmware block size must be a multiple of 16, and " \
              "should be a value from 16 to 128\n");
      printf("converting the firmware in blocks of 16 words...\n");
      block_Size  = 16;
      }
   } 
   else 
   {
      flag = 0;
      if ((block_Size < 1) || (block_Size > 128)) {
         printf("config block size must be a value from 1 to 128\n");	
         printf("converting the config in blocks of 1 words...\n");
         block_Size  = 1;
      }
   }

   BOOT_FD = fopen(inpath, "rb");
   if (BOOT_FD == NULL)
   {
      printf("Couldn't open %s file\n",inpath);
      return -1;
   }

   if(strstr (outpath, ".c") != NULL)
   {
      DBG("Output type is 'C' file\n");
      bOutputTypeC=1;
   }

   /* open file */
   saveFhande=fopen(outpath,"wb");
   if( saveFhande == NULL ) { 
      printf("Cannot open debug file %s for writing.\n", outpath); 
      return -1;
   }

   printf("%s convertion in progress...Please wait\n",inpath); 

   len = fseekNunlines(BOOT_FD);
   if (len <= 0) 
   {
      printf("Error: file is not of the correct format...\n");
      return -1;
   }
   rewind(BOOT_FD);
   DBG("total number of lines %d\n",len);

   if(flag)
   {
      zl_firmwareBlockSize = block_Size*2;
      HbiPagedBootImage();
      fclose(saveFhande);
   }
   else
   {
      p = strtok (outpath, ".");
      p = strcat (p, ".h");

      time (&rawtime);
      timeinfo = localtime (&rawtime);

      outLog ("/*Source file %s, modified: %s */ \n",inpath, asctime(timeinfo));  
      outLog ("#include \"%s\"\n\n", p);

      zl_configBlockSize = block_Size;

      if (readCfgFile(inpath) < 0)
      {
         printf("Error:read %s file\n", inpath);
         return;
      }

      outLog("const unsigned short configStreamLen = %lu;\n", numElements);

      fclose(saveFhande);

      strcpy(outpath, p); 
      saveFhande=fopen(outpath,"wb");
      if( saveFhande == NULL ) { 
          printf("Cannot open debug file %s for writing.\n", outpath); 
          return;
      }

      outLog("#ifndef __%s_H_\n",strupr(strtok (outpath, ".")));  
      outLog("#define __%s_H_\n\n",strupr(strtok (outpath, ".")));  
      outLog("#define ZL380XX_CFG_BLOCK_SIZE %d\n", zl_configBlockSize);  

      outLog(
      "typedef struct {\n"
      "   unsigned short reg;   /*the register */\n"
      "   unsigned short value[%u]; /*the value to write into reg */\n"
      "} dataArr;\n\n", zl_configBlockSize ); 
      outLog("extern const unsigned short configStreamLen;\n");
      outLog("extern const dataArr st_twConfig[];\n");        
      outLog("extern const unsigned short zl_configBlockSize;\n");
      outLog("#endif\n");  

      fclose(saveFhande);

   }
   fclose(BOOT_FD);

   printf("%s conversion completed successfully...\n", inpath);

   return 0;    
   }

