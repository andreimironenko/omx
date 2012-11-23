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

/**
 *******************************************************************************
 *  @file   omx_aenc_utils.h
 *  @brief  This header file contains structure definitions, function 
 *          Headers for utilities of Audio Encoder Component.
 *
 *  @rev 1.0
 *******************************************************************************
 */

#ifndef _OMX_AUDENC_UTIL_H
#define _OMX_AUDENC_UTIL_H

#include "OMX_Types.h"
#include "ti/omx/interfaces/openMaxv11/OMX_Core.h"
#include "ti/omx/interfaces/openMaxv11/OMX_Component.h"

#include <xdc/std.h>
#include <ti/sdo/ce/CERuntime.h>
#include <ti/sdo/ce/audio1/audenc1.h>
#ifdef CODEC_AACENC
#include <ti/sdo/codecs/aaclcenc/imp4aacenc.h>
#endif

/* Buffer is in FQ Pool or sent to next tunneled component */
#define OMX_BUFFER_FREE 0

/* buffer has been dequed by component */
#define OMX_BUFFER_DQ   1

/* buffer has been duped by component */
#define OMX_BUFFER_DUP  2

typedef struct _OMX_AUDENC_buf_header {
    OMX_PTR                   pBufHeader;
    OMX_U32                   bufStatus;
} OMX_AUDENC_buf_header;

OMX_U32 OMX_TI_AUDENC_UTL_GetBufStatus(OMX_AUDENC_buf_header *bufHeader, OMX_U32 cnt, OMX_PTR pBuffheader );

void OMX_TI_AUDENC_UTL_SetBufStatus(OMX_AUDENC_buf_header *bufHeader, OMX_U32 cnt, OMX_PTR pBuffheader, OMX_U32 status );

/*inline functions*/
static inline void OMX_TI_NCHANNELS_TO_CECHANNELS(IAUDENC1_Params * params, OMX_U32 channels)
{
     switch(channels){
         case 1: 
             params->channelMode = IAUDIO_1_0;
             break;
         case 2: 
	    params->channelMode = IAUDIO_2_0;
             break;
         case 5: 
            params->channelMode = IAUDIO_3_2;
             break;
         case 6: 
            params->channelMode = IAUDIO_3_2;
            params->lfeFlag = 1;
             break;
         default:
            /*dont modify existing values in case an unsupported parameter is passed. */
             break;

     }
}
static inline OMX_U32 OMX_TI_CECHANNELS_TO_NCHANNELS(IAUDENC1_Params * params)
{
    switch(params->channelMode){
        case IAUDIO_1_0: 
             return 1;
	case IAUDIO_2_0:
             return 2;
        case IAUDIO_3_2:{
             if(params->lfeFlag)
		return 6;
             else
                return 5;
	}
        default:
               return (OMX_U32)-1;

    }
}
static inline OMX_U32 OMX_TI_OMX_SF_TO_AE_SF(OMX_AUDIO_AACSTREAMFORMATTYPE streamFormatType)
{

    	switch(streamFormatType){
#ifdef CODEC_AACENC
	   case OMX_AUDIO_AACStreamFormatRAW:
               return (OMX_U32) IMP4AACENC_TT_RAW;
           case OMX_AUDIO_AACStreamFormatADIF:
               return (OMX_U32) IMP4AACENC_TT_ADIF;
           case OMX_AUDIO_AACStreamFormatMP4ADTS:
               return (OMX_U32) IMP4AACENC_TT_ADTS;
#endif
           default:
               return (OMX_U32) -1;

        }
 
}
static inline OMX_U32 OMX_TI_AE_SF_TO_OMX_SF(IAUDENC1_Params * params)
{

#ifdef CODEC_AACENC
       IMP4AACENC_Params *aacParams = (IMP4AACENC_Params *)params;

    	switch(aacParams->transMode){
	   case IMP4AACENC_TT_RAW:
               return (OMX_U32) OMX_AUDIO_AACStreamFormatRAW;
           case IMP4AACENC_TT_ADIF:
               return (OMX_U32) OMX_AUDIO_AACStreamFormatADIF;
           case IMP4AACENC_TT_ADTS:
               return (OMX_U32) OMX_AUDIO_AACStreamFormatMP4ADTS;
           default:
               return (OMX_U32) -1;

        }
#else
               return (OMX_U32) -1;
#endif 
}
#endif
