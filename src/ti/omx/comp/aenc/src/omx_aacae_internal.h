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
 *	omx_aacae_internal.h
 *
 * @brief
 *  This file contains methods that provides the functionality for the  AAC
 *  Audio Encoder component. This is not to be included by any file other
 *  than the OMX AAC Component itself
 *
 * @rev 1.0
 *
 *****************************************************************************
 */

#ifndef _OMX_AACENC_INTERNAL_H
#define _OMX_AACENC_INTERNAL_H

#ifdef _cplusplus
extern "C"   {
#endif /* _cplusplus */

/****************************************************************
 * INCLUDE FILES
 ****************************************************************/

/*-------program files ----------------------------------------*/
#include <xdc/std.h>
#include <ti/sdo/ce/Engine.h>
#include <ti/sdo/ce/CERuntime.h>
#include <ti/xdais/xdas.h>
#include <ti/sdo/ce/audio1/audenc1.h>
#ifdef CODEC_AACENC
#include <ti/sdo/codecs/aaclcenc/imp4aacenc.h>
#endif

#include "ti/omx/interfaces/openMaxv11/OMX_Core.h"
#include "ti/omx/interfaces/openMaxv11/OMX_Component.h"
#include "omx_base.h"
#include "omx_aenc_priv.h"

/* ----- system and platform files ----------------------------*/


#define AAC_OUTPUT_PCMWIDTH_16  16

#define AAC_OUTPUT_PCMWIDTH_24  24

#define AAC_DEFAULT_SAMPLES_IN_A_FRAME 1024

#define AAC_DEFAULT_OUTBUF_SIZE 4608

/*--------data declarations -----------------------------------*/

/****************************************************************
 * PRIVATE DECLARATIONS Defined here, used only here
 ****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/



OMX_ERRORTYPE OMX_TI_AACAE_ComponentInit ( OMX_HANDLETYPE hComponent );

void OMX_TI_AACAE_Set_StaticParams(OMX_HANDLETYPE hComponent, void* staticparams ); /* No of args changed for audio*/

void OMX_TI_AACAE_Set_DynamicParams(void* dynParams);

OMX_ERRORTYPE OMX_TI_AACAE_Init(OMX_HANDLETYPE hComponent);

AUDENC_BufParams OMX_TI_AACAE_Outbuff_Details(void *outArgs, OMX_U32 portIndex );

OMX_ERRORTYPE OMX_TI_AACAE_CB_ProcessDone ();


#ifdef _cplusplus
}
#endif /* __cplusplus */

#endif /* _OMX_AACENC_INTERNAL_H */


