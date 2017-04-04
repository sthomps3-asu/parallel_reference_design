#ifndef __AUDIO_H__
#define __AUDIO_H__

typedef enum {
	TW_COMMAND_START,
	TW_COMMAND_STOP,
	TW_COMMAND_DONOT_FLASH
} tw_command_t;

typedef enum {
	TW_STATUS_SUCCESS,			/* Previous COMMAND was executed successfully 	*/
	TW_STATUS_NOT_SUPPORTED,	/* The functionality is not supported 			*/
	TW_STATUS_SPI_ERROR,		/* Error during read write 						*/
	TW_STATUS_ALREADY_INIT,		/* TW is already initialised 					*/
	TW_STATUS_ERROR_INIT,		/* Error during Tw initialisation 				*/
	TW_STATUS_INVALID_IMG_HDR,	/* Invalid image header of the firmware			*/
	TW_STATUS_FW_LOAD_ERROR,	/* Error in loading firmware from host to Tw 	*/
	TW_STATUS_CFG_LOAD_ERROR,	/* Error in loading config record from host to Tw */
	TW_STATUS_START_FW_ERROR,	/* Error in starting firmware */
	TW_STATUS_FLASH_WRITE_ERROR,/* Error in flashing firmware and config record */
	TW_STATUS_FLASH_ERASE_ERROR,/* Error while erasing TW controlled flash 		*/
	TW_STATUS_ERROR				/* Unknown Error */
} tw_status_t;



#if 0
/*	This function initialises TW platform. This function flashes firmware and configuration record
 * hence should ideally be called once. TW supports loading of multiple firmware however for simplicity
 * the function will return error if it is already initialized after calling this function. This function
 * should be called first when flash is empty and also immediately after calling TW_Audio_DeInit() function.
 */
tw_status_t TW_Audio_Init();

/* This function de-initialises TW platform. This function removes firmware and configuration record
 * from flash. After calling this function, Tw won't able to run the audio application hence higher level
 * application shall call TW_Audio_Init.
 */
tw_status_t TW_Audio_DeInit();

/* This function does audio loop-back at TW level. The audio won't be transferred outside TW - not to I2S
 * or FPGA. This function shall be called after calling TW_Audio_Init().
 */
tw_status_t TW_Audio_Loopback(tw_command_t tw_command);

/* This function does audio loop-back from TW->SF2 I2S->TW. The audio is transferred outside TW - to I2S of SF2.
 * This function shall be called after calling TW_Audio_Init() function.
 */
void TW_Audio_SF2I2S_Loopback(tw_command_t tw_command);

/* This function stores audio in RAM of SF2. Audio travels from TW->SF2 I2S->RAM.
 * This function shall be called after calling TW_Audio_Init() function.
 */
void TW_Audio_SF2RAM_CAPTURE(tw_command_t tw_command);

/* This function retrieves audio from RAM of SF2. Audio travels from RAM->SF2 I2S->TW.
 * This function shall be called after calling TW_Audio_SF2I2S_Loopback() function.
 */
void TW_Audio_SF2RAM_PLAYBACK(tw_command_t tw_command);
#endif

#endif //__AUDIO_H__
