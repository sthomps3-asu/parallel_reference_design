/*
Header files defining a "Hardware Abstraction Layer for Voice processor devices
over linux kernel 3.18.x i2c core driver. Every successful call would return 0 or 
a linux error code as defined in linux errno.h
*/
#include "mss_spi.h"
#include "typedefs.h"
#include "ssl.h"
#include "hal.h"
#include "vproc_dbg.h"

#define HAL_DEBUG 1

/* this struct hold required information for each opened device*/
struct spi_dev_info{
	int			  		bBusInitialised;
	mss_spi_instance_t  *master;
};

/* array of maximum allowed device. index number maps to device chip select*/
struct spi_dev_info dev[MSS_SPI_MAX_NB_OF_SLAVES];

int hal_init(void)
{
	SSL_memset(dev,0,sizeof(dev));
    VPROC_DBG_PRINT(VPROC_DBG_LVL_FUNC,"Enter...\n");
    return 0;
}

int hal_term()
{
    VPROC_DBG_PRINT(VPROC_DBG_LVL_FUNC,"Enter\n");
    VPROC_DBG_PRINT(VPROC_DBG_LVL_FUNC,"Exit\n");
    return 0;
}
/*
    This function opens an client to a device
    identifies via "devcfg" parameter.
    Returns 0 and update client into a reference handle, if successful
    or an error code for an invalid parameter or client instantiation 
    fails.
*/
int hal_open(void **ppHandle,void *pDevCfg)
{
    ssl_dev_cfg_t 		*pDev = (ssl_dev_cfg_t *)pDevCfg;
    mss_spi_instance_t  *this_spi;

    VPROC_DBG_PRINT(VPROC_DBG_LVL_FUNC,"Enter\n");
    if(pDev == NULL)
    {
        VPROC_DBG_PRINT(VPROC_DBG_LVL_ERR,"Invalid Device Cfg Reference\n");
        return -1;
    }
   	if(pDev->dev_addr >= MSS_SPI_MAX_NB_OF_SLAVES)
   	{
        VPROC_DBG_PRINT(VPROC_DBG_LVL_ERR,"Invalid Slave select\n");
        return -1;
	}

    if(pDev->bus_num)
    	this_spi = &g_mss_spi1;
    else
    	this_spi = &g_mss_spi0;

    if(!(dev[pDev->dev_addr].bBusInitialised))
    {
    	MSS_SPI_init( this_spi );
    	dev[pDev->dev_addr].bBusInitialised = 1;
    }
    if(dev[pDev->dev_addr].master != NULL)
    {
    	VPROC_DBG_PRINT(VPROC_DBG_LVL_ERR,"Device is already in use\n");
    	return -1;
    }
    MSS_SPI_configure_master_mode(this_spi,
								  (mss_spi_slave_t)(pDev->dev_addr),
								  (mss_spi_protocol_mode_t)MSS_SPI_MODE0,
								  4u,/* APB_1_CLK is 50MHz, divisor is 2 so PCLK = 25*/
								  MSS_SPI_BLOCK_TRANSFER_FRAME_SIZE);//8-bits

    *((mss_spi_slave_t *)ppHandle) = (pDev->dev_addr);

    dev[pDev->dev_addr].master = this_spi;

    VPROC_DBG_PRINT(VPROC_DBG_LVL_INFO,
                     "Updating handle 0x%x back to user\n",(uint32_t)pDev->dev_addr);

    VPROC_DBG_PRINT(VPROC_DBG_LVL_FUNC,"Exit..\n");

    return 0;
}

int hal_close(void *pHandle)
{
	mss_spi_slave_t slave = (mss_spi_slave_t)pHandle;

    VPROC_DBG_PRINT(VPROC_DBG_LVL_FUNC,"Enter..\n");

    VPROC_DBG_PRINT(VPROC_DBG_LVL_INFO,
                     "Unregistering client 0x%x\n", pHandle);

    dev[slave].master = NULL;

    VPROC_DBG_PRINT(VPROC_DBG_LVL_FUNC,"Exit..\n");
    return 0;
}

int hal_port_rw(void *pHandle,void *pPortAccess)
{
	mss_spi_slave_t slave = (mss_spi_slave_t)pHandle;
    int                 ret=0;
    ssl_port_access_t   *pPort = (ssl_port_access_t *)pPortAccess;

    ssl_op_t          op_type;
#if HAL_DEBUG
    int                  i;
#endif
    VPROC_DBG_PRINT(VPROC_DBG_LVL_FUNC,
                     "Enter (handle 0x%x)...\n",(uint32_t)pHandle);
    if(pPort == NULL)
    {
        VPROC_DBG_PRINT(VPROC_DBG_LVL_ERR,"Invalid Parameters\n");
        return -1;
    }
    
    op_type = pPort->op_type;


    if(op_type & SSL_OP_PORT_WR)
    {
        if(pPort->pSrc == NULL)
        {
            VPROC_DBG_PRINT(VPROC_DBG_LVL_ERR,"NULL src buffer passed\n");
            return -1;
        }
        
        
#if HAL_DEBUG
        VPROC_DBG_PRINT(VPROC_DBG_LVL_INFO,"writing %d bytes..\n",pPort->nwrite);

        for(i=0;i<pPort->nwrite;i++)
        {
        	printf("0x%x\t",((uint8_t *)(pPort->pSrc))[i]);
        }
        VPROC_DBG_PRINT(VPROC_DBG_LVL_INFO,"\n");
#endif
    }

    if(op_type & SSL_OP_PORT_RD)
    {
        if(pPort->pDst == NULL)
        {
        	printf(VPROC_DBG_LVL_ERR,"NULL destination buffer passed\n");
            return -1;
        }

        printf(VPROC_DBG_LVL_INFO,"read %d bytes..\n",pPort->nread);
    }

    MSS_SPI_set_slave_select( dev[slave].master, slave);
    /* Currently not using DMA based transfer.
     * We can interchangeably use DMA and non-DMA based transaction
     * based on transfer size.
     * TODO task going further*/
    MSS_SPI_transfer_block(dev[slave].master,
							(const uint8_t *) pPort->pSrc,
							(uint16_t) pPort->nwrite,
							(uint8_t *) pPort->pDst,
							(uint16_t) pPort->nread
						);
    MSS_SPI_clear_slave_select( dev[slave].master, slave );
    #if HAL_DEBUG
    if(pPort->nread >0)
    {
        printf(VPROC_DBG_LVL_INFO,"Received...\n");
        for(i=0;i<pPort->nread;i++)
        {
        	printf(VPROC_DBG_LVL_INFO,"0x%x\t",((uint8_t *)(pPort->pDst))[i]);
        }
        printf(VPROC_DBG_LVL_INFO,"\n");
    }
    #endif
    return ret;
}




