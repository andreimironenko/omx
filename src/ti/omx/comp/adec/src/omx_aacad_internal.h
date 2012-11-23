/*
 *  Copyright (c) 2010-2011, Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 *  *  Neither the name of Texas Instruments Incorporated nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  Contact information for paper mail:
 *  Texas Instruments
 *  Post Office Box 655303
 *  Dallas, Texas 75265
 *  Contact information:
 *  http://www-k.ext.ti.com/sc/technical-support/product-information-centers.htm?
 *  DCMP=TIHomeTracking&HQS=Other+OT+home_d_contact
 *  ============================================================================
 *
 */
/*!
 *****************************************************************************
 * @file
 *	omx_aacad_internal.h
 *
 * @brief
 *  This file contains methods that provides the functionality for the  AAC
 *  Audio Decoder component. This is not to be included by any file other
 *  than the OMX AAC Component itself
 *
 * @rev 1.0
 *
 *****************************************************************************
 */

#ifndef _OMX_AACDEC_INTERNAL_H
#define _OMX_AACDEC_INTERNAL_H

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
#include <ti/sdo/ce/audio1/auddec1.h>
#include <ti/sdo/codecs/aaclcdec/iaacdec.h>


#include "ti/omx/interfaces/openMaxv11/OMX_Core.h"
#include "ti/omx/interfaces/openMaxv11/OMX_Component.h"
#include "omx_base.h"
#include "omx_adec_priv.h"

/* ----- system and platform files ----------------------------*/


#define AAC_OUTPUT_PCMWIDTH_16  16

#define AAC_OUTPUT_PCMWIDTH_24  24

/*--------data declarations -----------------------------------*/

/****************************************************************
 * PRIVATE DECLARATIONS Defined here, used only here
 ****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/
OMX_ERRORTYPE OMX_TI_AACAD_ComponentInit ( OMX_HANDLETYPE hComponent );

void OMX_TI_AACAD_Set_StaticParams(OMX_AUDDEC_PVTTYPE * pAudDecComPvt, void* staticparams );

void OMX_TI_AACAD_Set_DynamicParams(void* dynParams);

OMX_ERRORTYPE OMX_TI_AACAD_Init(OMX_HANDLETYPE hComponent);

OMX_ERRORTYPE OMX_TI_AACAD_CB_ProcessDone ();

int mapOmxSampleRateToTiAacDecSrIdx(int sampleRate);

OMX_AUDIO_CHANNELMODETYPE mapTiChannelModeToOmxChannelMode(IAUDIO_ChannelMode channelMode);

#ifdef _cplusplus
}
#endif /* __cplusplus */

#endif /* _OMX_AACDEC_INTERNAL_H */


