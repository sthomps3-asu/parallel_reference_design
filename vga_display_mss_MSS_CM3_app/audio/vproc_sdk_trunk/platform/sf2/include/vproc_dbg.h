#ifndef __VPROC_DBG_H__
#define __VPROC_DBG_H__
//#include "mss_uart.h"
/* Bitmask for selecting debug level information */
typedef enum
{
    VPROC_DBG_LVL_NONE=0x0,
    VPROC_DBG_LVL_FUNC=0x1,
    VPROC_DBG_LVL_INFO=0x2,
    VPROC_DBG_LVL_WARN=0x4,
    VPROC_DBG_LVL_ERR=0x8,
    VPROC_DBG_LVL_ALL=(VPROC_DBG_LVL_FUNC|VPROC_DBG_LVL_INFO|VPROC_DBG_LVL_WARN|VPROC_DBG_LVL_ERR)
}VPROC_DBG_LVL;

#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL VPROC_DBG_LVL_ERR
#endif

extern VPROC_DBG_LVL vproc_dbg_lvl;
#define VPROC_DBG_SET_LVL(dbg_lvl) (vproc_dbg_lvl = dbg_lvl)
/* Macros defining SSL_print */
#ifdef DEBUG 
#if 0
#define VPROC_DBG_PRINT(level,msg,args...) \
	if(level & vproc_dbg_lvl) { \
	MSS_UART_polled_tx(&g_mss_uart0,(const uint8_t*) msg, sizeof(msg));\
	}
#else
#define VPROC_DBG_PRINT(level,msg,args...) if(level & vproc_dbg_lvl) printf("%s:%d \n"msg,__func__,__LINE__,##args)
#endif
#else
#define VPROC_DBG_PRINT(level,msg,args...) 
#endif



#endif /*__VPROC_DBG_H__ */ 
