#ifndef __SSL_H__
#define __SSL_H__

#include "sys_config_mss_clocks.h"

/* SSL Driver Function Call Return Code Enumeration */
typedef enum
{
    SSL_STATUS_NOT_INIT = -1, /* SSL Driver not initialised */
    SSL_STATUS_INVALID_ARG = -2, /* Invalid parameter passed to SSL driver function */
    SSL_STATUS_INTERNAL_ERR = -3,  /* An error is reported from platform specific function/layer*/
    SSL_STATUS_BAD_HANDLE = -4,    /* Invalid port handle passed */
    SSL_STATUS_RESOURCE_ERR = -5,  /* Requested resource unavailable */
    SSL_STATUS_TIMEOUT = -6,       /* Timeout waiting on a certain operation */
    SSL_STATUS_FAILED = -7,        /* SSL Driver function called failed */
    SSL_STATUS_OP_INCOMPLETE = -8, /* port read/write incomplete. */
    SSL_STATUS_OK=0            /* SSL Driver function called successful */
}ssl_status_t;


/* Enum to list out wait type on lock. if none of these given them timeout
   assumed to be in msec */
typedef enum
{
    SSL_WAIT_NONE,
    SSL_WAIT_FOREVER
}ssl_wait_t;

/* Enum indicating read/write function on port. Can be 'Read Only' , 
   'Write Only' or 'Read/Write' both
*/
typedef enum
{
    SSL_OP_PORT_RD=0x01,
    SSL_OP_PORT_WR=0x02,
    SSL_OP_PORT_RW = (SSL_OP_PORT_RD | SSL_OP_PORT_WR)
}ssl_op_t;

/* structure used during port read/write operation.*/
typedef struct
{
    void    *pSrc;    /* pointer to source buffer, 
                         must be set to a valid value for PORT WRITE Operation
                      */
    void    *pDst;    /* pointer to destination buffer, 
                         must be set to a valid value for PORT READ Operation 
                      */
    size_t   nread;   /* number of bytes to read valid in case of PORT 
                         Read operation 
                      */
    size_t   nwrite;  /* number of write. valid in case of PORT Write operation 
                      */
    ssl_op_t op_type; /* Enum indicating port operation: 
                        'read','write' or 'read/write' 
                      */
}ssl_port_access_t;

ssl_status_t SSL_init(ssl_drv_cfg_t *);
ssl_status_t SSL_lock_create(ssl_lock_handle_t *pLock, 
                           const char *pName, void *pOption);
ssl_status_t SSL_lock(ssl_lock_handle_t lock_id,ssl_wait_t wait_type);
ssl_status_t SSL_unlock(ssl_lock_handle_t lock_id);
ssl_status_t SSL_lock_delete(ssl_lock_handle_t lock_id);
ssl_status_t SSL_term(void);
ssl_status_t SSL_port_open(ssl_port_handle_t *pHandle, ssl_dev_cfg_t *pDevCfg);
ssl_status_t SSL_port_close(ssl_port_handle_t handle);
ssl_status_t SSL_port_read(ssl_port_handle_t handle, void *pDst, size_t *pNread);
ssl_status_t SSL_port_write(ssl_port_handle_t handle,void *pSrc, size_t *pNwrite);
ssl_status_t SSL_port_rw(ssl_port_handle_t handle, ssl_port_access_t *pPort);
ssl_status_t SSL_memset(void *pDst, int32_t val,size_t size);
ssl_status_t SSL_memcpy(void *pDst,const void *pSrc, size_t size);
ssl_status_t SSL_delay(uint32_t tmsec);
#endif

