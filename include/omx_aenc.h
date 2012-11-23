/*******************************************************************************
 * Texas Instruments OpenMax Multimedia Sub-system                             *
 *                                                                             *
 * "OpenMax Multimedia Sub-system" is a software module developed on TI's DM   *
 * class of SOCs. These modules are based on Khronos open max 1.1.2            *
 * specification.                                                              *
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 ******************************************************************************/
/*!  
 *****************************************************************************
 * @file
 *	omx_adec.h														 
 *
 * @brief  
 *    This file contains methods that provides the functionality for the 
 *    Audio Decoder component
 *	
 * @rev 1.0
 *
 *****************************************************************************
 */

#ifndef _OMX_AUDENC_H
#define _OMX_AUDENC_H

#ifdef _cplusplus
extern "C"   {
#endif /* _cplusplus */

/****************************************************************
 * INCLUDE FILES
 ****************************************************************/

#include "ti/omx/interfaces/openMaxv11/OMX_Core.h"
#include "OMX_Types.h"

/** Input port Index for the OMX_AUDDEC Comp */
#define OMX_AUDENC_INPUT_PORT 0

/** Output port Index for the OMX_AUDDEC Comp */
#define OMX_AUDENC_OUTPUT_PORT 1

/** OMX Audio Decoder component's name */
#define OMX_AUDENC_COMPONENT_NAME              "OMX.TI.DSP.AUDENC"


/* Control/command status */
#define AUDIO_CTRL_PENDING                    (1)
#define AUDIO_COMMAND                         (2)
#define AUDIO_DATA                            (4)

/* reset frames */
#define RESET_PEND                      (0x1)
#define RESET_DONE                      (0x2)
#define FLUSH_PEND                      (0x4)
#define FLUSH_START                     (0x8)
#define FLUSH_DONE                      (0x10)
#define FLUSH_IDLE                      (0x20)
#define SETPARAM_PEND                   (0x40)
#define SETPARAM_DONE                   (0x80)
#define STREAM_FLUSH_ON                 (0x100)


/* ========================================================================== */
/**
* OMX_TI_AUDENC_ComponentInit() : Entry point for OMX Audio Encoder Component.
* This function is called whenever the IL Client invokes a GetHandle function 
* with the name "OMX.TI.DSP.AUDENC"
*
* @param hComponent     OMX_HANDLETYPE hComponent 
*
*  @return      
*  OMX_ErrorNone = Successful 
*
*  Other_value = Failed (Error code is returned)
*
*/
/* ========================================================================== */

OMX_ERRORTYPE OMX_TI_AUDENC_ComponentInit(OMX_HANDLETYPE hComponent);

void Test_Fun1();

#ifdef _cplusplus
}
#endif /* __cplusplus */

#endif /* _OMX_AUDENC_H */


