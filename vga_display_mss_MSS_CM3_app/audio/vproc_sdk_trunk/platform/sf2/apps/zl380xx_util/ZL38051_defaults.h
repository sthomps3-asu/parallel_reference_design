#ifndef __ZL38051_defaults_H_
#define __ZL38051_defaults_H_

#define ZL380XX_CFG_BLOCK_SIZE 1
typedef struct {
   unsigned short reg;   /*the register */
   unsigned short value[1]; /*the value to write into reg */
} dataArr;

extern const unsigned short configStreamLen;
extern const dataArr st_twConfig[];
extern const unsigned short zl_configBlockSize;
#endif
