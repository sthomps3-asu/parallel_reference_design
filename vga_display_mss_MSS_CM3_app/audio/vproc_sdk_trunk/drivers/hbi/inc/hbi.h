#ifndef __HBI_H__
#define __HBI_H__

/*  hbi_cmd_t
    Enumerates various HOST commands that can be issued to device.
    Each command may or may not be followed by input and/or output arguments.
    Details of argument types w.r.t each command is commented on top of each 
    command below. Arguments can be mandatory or optional. By default, arguments 
    assumed to be mandatory unless explicitly mentioned as optional
*/
typedef enum
{
    /* Loads firmware image to device.
       input args: pointer to hbi_data_buf_t 
                  (populated with pointer to image buffer and length of buffer)
       output args: none 
    */
    HBI_CMD_LOAD_FWR_FROM_HOST,

    /* Loads configuration record from host to device.
       input args: pointer to hbi_data_buf_t 
                  (populated with pointer to config record buffer and length of 
                   buffer) 
       output args: none 
    */
    HBI_CMD_LOAD_CFGREC_FROM_HOST,

    /* Signal device that host data transfer is complete.Should be called after
       HBI_CMD_LOAD_FWR_FROM_HOST or HBI_CMD_LOAD_CFGREC_FROM_HOST
       input args : none
       output args : none
    */
    HBI_CMD_LOAD_FWR_COMPLETE, 

    /* Loads requested firmware and associated configuration record from flash 
       connected to voice processor device. Need image number from user.
       input args: an unsigend int image_num 
       output args: none
    */
    HBI_CMD_LOAD_FWRCFG_FROM_FLASH,  

    /* Saves current firmware and configuration record in device memory to flash.
       connected to voice processor device.Should be called after 
       HBI_CMD_LOAD_FWR_FROM_HOST and HBI_CMD_HOST_LOAD_COMPLETE.
       input args : none
       output args: pointer to an unsigned int to pass back uploaded image number
                    (optional)
    */
    HBI_CMD_SAVE_FWRCFG_TO_FLASH,    
#if 0
    /* Saves current configuration record in device memory to flash 
       connected to voice processor device.
       input args: an unsigned int firmware image number.please note 
                   configuration record should be stored against compatible
                   firmware image. To be taken care if there are multiple images
                   on flash.
       output args: none
    */
    HBI_CMD_SAVE_CFG_TO_FLASH    
#endif
    /* Erases whole flash connected to voice processor device
       input args: none 
       output args: none
    */
    HBI_CMD_ERASE_WHOLE_FLASH,       

    /* Erases specific firmware and associated configuration record from flash 
       connected to voice processor device. Need an image number from user 
       Input args: an unsigned int firmware image number 
       output args: none
    */
    HBI_CMD_ERASE_FWRCFG_FROM_FLASH, 

    /* Starts executing current firmware in RAM after it is being loaded either 
       from flash or host. Please note this should be called ONLY and IMMEDIATLY 
       after HBI_CMD_LOAD_FROM_HOST and HBI_CMD_HOST_LOAD_COMPLETE OR 
       HBI_CMD_LOAD_FWRCFG_FROM_FLASH.
       input args: none
       output args: none
    */
    HBI_CMD_START_FWR,

    /* End marker for Host Command List */
    HBI_CMD_END
}hbi_cmd_t;

/*hbi_status_t
  Enumerates various status codes of HBI Driver
*/
typedef enum 
{
    HBI_STATUS_NOT_INIT,     /*  driver not initilised */
    HBI_STATUS_INTERNAL_ERR, /* platform specific layer reported an error */
    HBI_STATUS_RESOURCE_ERR, /* request resource unavailable */
    HBI_STATUS_INVALID_ARG,  /* invalid argument passed to a function call */
    HBI_STATUS_BAD_HANDLE,   /* a bad reference handle passed */
    HBI_STATUS_BAD_IMAGE,    /* requested firmware image not present on flash */
    HBI_STATUS_FLASH_FULL,   /* no more space left on flash */
    HBI_STATUS_NO_FLASH_PRESENT, /* no flash connected to device */
    HBI_STATUS_COMMAND_ERR,    /* HBI Command failed */
    HBI_STATUS_INCOMPAT_APP,   /* firmware image is incompatible */
    HBI_STATUS_INVALID_STATE,  /* driver is in invalid state for current action */
    HBI_STATUS_OP_INCOMPLETE, /* operation incomplete */
    HBI_STATUS_SUCCESS         /* driver call successful */
}hbi_status_t;

typedef enum
{
   HBI_IMG_TYPE_FWR=0,
   HBI_IMG_TYPE_CR=1,
   HBI_IMG_TYPE_LAST
}hbi_img_type_t;

/*hbi_rst_mode_t
  Enumerates reset modes of device
*/
typedef enum
{
    HBI_RST_POR, /* This simulates Power On Reset where it will clear everything 
                    including DSP memory and restart device with invocation of 
                    internal boot ROM code 
                    followed by loading of firmware from flash, if present. 
                    If flash not present in system then device will stop at 
                    Boot ROM prompt waiting on host 
                 */
}hbi_rst_mode_t;

/* hbi_dev_cfg_t
  typedef to a struct describing one-time device configuration.
*/
typedef struct
{
    uint8_t  dev_addr; /* device address. can be i2c complaint or spi chip 
                          select id*/
    uint8_t *pDevName; /* an optional pointer to device name */
    uint8_t  bus_num;  /* bus number device physically present on */
    ssl_lock_handle_t dev_lock; /* lock to serialise device access */
}hbi_dev_cfg_t;

/* hbi_data_t
  typedef struct declaring a data type that can be used to 
  send any host data over HBI
*/
typedef struct 
{
   unsigned char *pData; /* pointer to user data buffer */
   size_t        size; /* length of the buffer in bytes */
}hbi_data_t;

/* hbi_init_cfg_t
  typedef struct declaring a data type to pass one-time driver configuration
  as passed by user.
*/
typedef struct
{
    ssl_lock_handle_t lock; /* driver serialising lock. supposed to be created 
                               and passed by user. if passed, then driver will 
                               serialize all calls using this lock.
                            */
}hbi_init_cfg_t;

typedef struct
{
   int   major_ver;  /* header version major num */
   int   minor_ver;  /* header version minor num */
   hbi_img_type_t image_type; /* firmware or configuration record */
   int    endianness; /* endianness of the image excluding header */
   int    fwr_code;   /* if image_type == firmware, tells the firmware opn code */
   size_t block_size; /* block length in words image is divided into */
   size_t img_len;  /* total length of the image excluding header  */
   int    hdr_len;    /* length of header */
}hbi_img_hdr_t;

typedef uint32_t      hbi_handle_t;
typedef unsigned char user_buffer_t;

/*  HBI_init()

    Description: 
        Driver initialization function. Allocates and initialize 
        resources as required by driver. User may pass one time 
        initialization configuration. This function may be called multiple times.
        however if already initialized with one configuration that will be used
        for all users until HBI_term() is called.

    Input :      
        pCfg - Pointer to const driver init configuration. This is optional 
               parameter.

    Output:      
        None

    Return:
    
    Note:
        Initialization configuration passed in here supposed be user passed and
        not modified by driver. This is not an interrupt safe function and 
        should be called from process context.
*/
hbi_status_t HBI_init(const hbi_init_cfg_t *pCfg);

/* HBI_term()

   Description: 
        Driver termination function. Deallocates and deinitializes all resources 
        as acquired in HBI_init() function. 

   Input:
        None

   Output:
        None

   Return:
   
   Note:
        Make sure to close all of the devices and user before terminating the 
        driver else it will return an error. function is not interrupt safe and 
        should be called from process context.
*/
hbi_status_t HBI_term(void);

/* HBI_open()

   Description:
        function to open a device. inputs one-time device configuration as 
        passed by user. User can call this function multiple time for different
        or same device. if called on already open device, driver will pass 
        back already opened device reference with existing configuration and 
        that user passed configuration will be ignored. 

   Input:
       pDevCfg - Pointer to device configuration.

   Output:
      pHandle - Updated by driver after successful open call. Reference handle
                of device to be used by user in subsequent HBI driver device 
                access call.

   Return:
      HBI_STATUS_SUCCESS

   Note:
      This function is not interrupt safe.
      
*/
hbi_status_t HBI_open(hbi_handle_t *pHandle, hbi_dev_cfg_t *pDevCfg);

/* HBI_close()

   Description:
      function to close a device opened using HBI_open(). 
      if there are multiple user on the device, device physically doesn't 
      close until user count reaches to 0 otherwise it just free up user 
      reference and access to device.

   Input:
      handle  - Device handle as passed by HBI_open()

   Output:
      None

   Return:
      HBI_STATUS_SUCCESS
      HBI_STATUS_BAD_HANDLE
      HBI_STATUS_NOT_INIT
      
   Note:
      This function is not interrupt safe.
      
*/
hbi_status_t HBI_close(hbi_handle_t handle);

/* HBI_reset()

   Description:
      function to reset a device. this function equivalent to simulate Power On
      Reset

   Input:
      handle  - Device handle as returned by HBI_open()
      mode    - Reset mode of device

   Output:
      None

   Return:
      HBI_STATUS_SUCCESS
      HBI_STATUS_BAD_HANDLE
      HBI_STATUS_NOT_INIT
      HBI_STATUS_INTERNAL_ERR
      
   Note:
      This function is not interrupt safe.
      
*/
hbi_status_t HBI_reset(hbi_handle_t handle, hbi_rst_mode_t mode);

/* HBI_read()

   Description:
      Reads the data from device memory starting from register address up to 
      specified length

   Input:
      handle  - Device handle as returned by HBI_open()
      reg     - Device register address to read from
      pData   - Pointer to user buffer to read data into
      length  - length of the data to be read in bytes. should be passed based
               on number_of_elements_to_be_read*sizeof(user_buffer_t)
      
   Output:
      pData - updated by driver on success

   Return:
      HBI_STATUS_SUCCESS
      HBI_STATUS_BAD_HANDLE
      HBI_STATUS_NOT_INIT
      HBI_STATUS_INTERNAL_ERR
      
   Note:
      It is caller responsibility to ensure that user buffer is sufficiently
      large enough to hold requested length of data else it may result in 
      system crash. driver may expect length in multiple of 16-bit depending 
      upon device family it is compiled for. This function is not interrupt safe.
      
*/
hbi_status_t HBI_read(hbi_handle_t handle,
                     reg_addr_t reg, 
                     user_buffer_t *pData, 
                     size_t length);

/* HBI_write()

   Description:
      Writes the data from user buffer to device memory starting from register  
      address up to specified length

   Input:
      handle  - Device handle as returned by HBI_open()
      reg     - Device register address to write to
      pData   - Pointer to user buffer to read data from
      length  - length of the data to be written in bytes. should be calculated
                based on sizeof(user_buffer_t)*number_of_elements_to_fill in 
                buffer. lets say, user allocate a user_buffer_t reg[2] and what
                to read 2 entries of the buffer then 
                length = 2*sizeof(user_buffer_t)
      
   Output:
      None

   Return:
      HBI_STATUS_SUCCESS
      HBI_STATUS_BAD_HANDLE
      HBI_STATUS_NOT_INIT
      HBI_STATUS_INTERNAL_ERR
      
   Note:
      It is caller responsibility to ensure that user buffer is sufficiently
      allocated and filled to number of elements to be written. 
      Else it may result in system crash. driver may expect 
      length in multiple of 16-bit depending upon device family it is compiled 
      for. This function is not interrupt safe.
      
*/
hbi_status_t HBI_write(hbi_handle_t handle, 
                     reg_addr_t reg, 
                     user_buffer_t *pData, 
                     size_t length);
 
 /* HBI_set_command()
 
    Description:
       Set the Host Commands as listed in hbi_cmd_t enum  
 
    Input:
       handle  - Device handle as returned by HBI_open()
       cmd     - Host Command as listed in hbi_cmd_t 
       cmdargs - Pointer to respective command arguments
       
    Output:
       cmdargs - May be updated by driver depending upon command issued.
 
    Return:
       HBI_STATUS_SUCCESS
       HBI_STATUS_BAD_HANDLE
       HBI_STATUS_NOT_INIT
       HBI_STATUS_INTERNAL_ERR
       HBI_STATUS_INVALID_ARG

    Note:
       This function is not interrupt safe.
 */
hbi_status_t HBI_set_command(hbi_handle_t handle, hbi_cmd_t cmd, void *pCmdArgs);

 /* HBI_wake()
 
    Description:
       wakes up the device from sleep mode.
 
    Input:
       handle  - Device handle as returned by HBI_open()
       
    Output:
      None
 
    Return:
       HBI_STATUS_SUCCESS
       HBI_STATUS_BAD_HANDLE
       HBI_STATUS_NOT_INIT
       HBI_STATUS_INTERNAL_ERR

    Note:
       This function is not interrupt safe.
 */
hbi_status_t HBI_wake(hbi_handle_t handle);

/* HBI_sleep()

   Description:
      this function puts device to sleep mode

   Input:
      handle  - Device handle as returned by HBI_open()
      
   Output:
     None

   Return:
      HBI_STATUS_SUCCESS
      HBI_STATUS_BAD_HANDLE
      HBI_STATUS_NOT_INIT
      HBI_STATUS_INTERNAL_ERR

   Note:
      This function is not interrupt safe.
*/
hbi_status_t HBI_sleep(hbi_handle_t handle);

/* HBI_get_header()

   Description:
      this function parses header of the image passed by user

   Input:
      pImg  - pointer to a structure containing image buffer and its length

   Output:
     pHeaderInfo - pointer updated by function with information extracted from
                  header

   Return:
      HBI_STATUS_SUCCESS

   Note:
      This function is not interrupt safe.
*/

hbi_status_t HBI_get_header(hbi_data_t *, hbi_img_hdr_t *);
#endif /* __HBI_H__*/

