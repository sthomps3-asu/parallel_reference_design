#define HBI_LNX_DRV_MAGIC 'q' 

typedef struct hbi_lnx_drv_rw_arg
{
    hbi_handle_t handle;
    hbi_status_t status;
    reg_addr_t   reg;
    user_buffer_t  *pData;
    size_t      len;
}hbi_lnx_drv_rw_arg_t;

typedef struct hbi_lnx_send_data_arg{
    hbi_handle_t handle;
    hbi_status_t status;
    hbi_data_t   data;
}hbi_lnx_send_data_arg_t;

typedef struct hbi_lnx_start_fw_arg{
    hbi_handle_t handle;
    hbi_status_t status;
}hbi_lnx_start_fw_arg_t;

typedef struct{
    hbi_handle_t handle;
    hbi_status_t status;
    int32_t    image_num;
}hbi_lnx_flash_save_fwrcfg_arg_t;

typedef struct{
    hbi_handle_t handle;
    hbi_status_t status;
    int32_t    image_num;
}hbi_lnx_flash_load_fwrcfg_arg_t;

typedef struct {
    hbi_handle_t handle;
    hbi_status_t status;
    int32_t   image_num;
}hbi_lnx_flash_erase_fwcfg_arg_t;


#define HBI_OPEN        _IOWR(HBI_LNX_DRV_MAGIC,1,hbi_dev_cfg_t)
#define HBI_CLOSE       _IOWR(HBI_LNX_DRV_MAGIC,2,__u32)
#define HBI_WRITE       _IOWR(HBI_LNX_DRV_MAGIC,3,hbi_lnx_drv_rw_arg_t)
#define HBI_READ        _IOWR(HBI_LNX_DRV_MAGIC,4,hbi_lnx_drv_rw_arg_t)

/* TODO: We have alternative approach to combine few IOCTL and make them one Ex.
    HBI_FW_LOAD_SAVE_TO_FLASH -> this would contain loading firmware to device memory and save to flash
    HBI_FW_LOAD_RUN - > Loads and start execution from RAM
    HBI_FW_LOAD_FROM_FLASH_RUN -> Read from flash and run
    HBI_FW_LOAD_SAVE_TO_FLASH_RUN -> loads firmware, save  to flash and run (if possible)
    */

/* ioctl called when loading firmware from host.*/
#define HBI_LOAD_FW     _IOWR(HBI_LNX_DRV_MAGIC,5,hbi_lnx_send_data_arg_t) 

/* start firmware execution i.e. loaded into VPROC device memory 
   (should be called after HBI_LOAD_FW or HBI_FLASH_LOAD_FWR_CFGREC) */
#define HBI_START_FW    _IOWR(HBI_LNX_DRV_MAGIC,6,hbi_lnx_start_fw_arg_t) 

/* Following ioctl are defined based in FLASH_PRESENT macro */

/* ioctl to save loaded firmware into RAM to flash. 
   should be called after HBI_LOAD_FW*/
#define HBI_FLASH_SAVE_FWR_CFGREC  _IOWR(HBI_LNX_DRV_MAGIC,7,hbi_lnx_flash_save_fwrcfg_arg_t) 

/* ioctl to read specified firmware image from flash. 
   can be called anytime */
#define HBI_FLASH_LOAD_FWR_CFGREC  _IOWR(HBI_LNX_DRV_MAGIC,8,hbi_lnx_flash_load_fwrcfg_arg_t) 

/* ioctl to erase specific firmware image from flash. */
#define HBI_FLASH_ERASE_FWRCFGREC  _IOWR(HBI_LNX_DRV_MAGIC,9,hbi_lnx_flash_erase_fwcfg_arg_t) 

/* ioctl to erase complete firmware image from flash. */
#define HBI_FLASH_ERASE_WHOLE      _IOWR(HBI_LNX_DRV_MAGIC,10,hbi_lnx_flash_erase_fwcfg_arg_t)





