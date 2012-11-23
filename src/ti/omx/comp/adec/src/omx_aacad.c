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
/*
 * ****************************************************************************
 *  @file  omx_aacad.c
 *  @brief This file contains methods that provides the functionality for a
 *         AAC Audio Decoder OpenMax 1.1.2 Component.
 *  @rev   1.0
 * ****************************************************************************
 */

/*******************************************************************************
*                             Compilation Control Switches
*******************************************************************************/
/* None */

/*******************************************************************************
*                             INCLUDE FILES
*******************************************************************************/
/*--------------------- system and platform files ----------------------------*/
#include <xdc/std.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Diags.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include <ti/sdo/ce/Engine.h>
#include <ti/sdo/ce/CERuntime.h>
#include <ti/sdo/ce/audio1/auddec1.h>
#include "timm_osal_interfaces.h"

/*-------------------------program files -------------------------------------*/
#include "ti/omx/interfaces/openMaxv11/OMX_Core.h"
#include <omx_base.h>
#include "omx_base_utils.h"
#include "omx_aacad_internal.h"
#include "omx_adec.h"
#include "omx_adec_util.h"
#include "OMX_TI_Audio.h"

/*******************************************************************************
 * EXTERNAL REFERENCES NOTE : only use if not found in header file
*******************************************************************************/

/*------------------------ data declarations ---------------------------------*/
/* None */

/*----------------------- function prototypes --------------------------------*/
/* None */

/*******************************************************************************
 * PUBLIC DECLARATIONS Defined here, used elsewhere
 ******************************************************************************/

/*------------------------ data declarations ---------------------------------*/

/* None */
//extern int FRAME_DECODED[];

/*----------------------- function prototypes --------------------------------*/

/*****************************************************************************
 * PRIVATE DECLARATIONS Defined here, used only here
 ******************************************************************************/

/** Codec name. This should be same as the one created in the cfg file */
static String decoderName = "aaclcdec";

/**
 *  min - The macro finds the minimum of two values
*/
#define min(a,b) ((a) < (b) ? (a) : (b))

/* Function to map OMX Audio PCM source Sampling Rate to Sample Rate Index 
 * required by TI AAC decoder */
int mapOmxSampleRateToTiAacDecSrIdx(int sampleRate) {
	int sampleRateIndex = 0;
	switch(sampleRate) {
	  case 96000: 
		  sampleRateIndex = 0;
		  break;
	  case 88200: 
		  sampleRateIndex = 1;
		  break;
	  case 64000: 
		  sampleRateIndex = 2;
		  break;
	  case 48000: 
		  sampleRateIndex = 3;
		  break;
      case 44100:
		  sampleRateIndex = 4;
		  break;
	  case 32000: 
		  sampleRateIndex = 5;
		  break;
	  case 24000: 
		  sampleRateIndex = 6;
		  break;
	  case 22050: 
		  sampleRateIndex = 7;
		  break;
      case 16000:
		  sampleRateIndex = 8;
		  break;
	  case 12000: 
		  sampleRateIndex = 9;
		  break;
	  case 11025: 
		  sampleRateIndex = 10;
		  break;
	  case 8000: 
		  sampleRateIndex = 11;
		  break;
	  default:
          sampleRateIndex = 4;/* Set as 44.1KHz*/
		  break;
	}
	return (sampleRateIndex);
}
/**
********************************************************************************
 *  @func  OMX_TI_AACAD_Set_StaticParams()
 *  @brief  This method is called for setting the create time parameters for
 *          AAC decoder. 
 *
 * @param [in] hComponent      : handle of the component private
 * @param [in] staticparams    :    Codec create time parameter
 *
 *
*/
/* ========================================================================== */
void OMX_TI_AACAD_Set_StaticParams(OMX_AUDDEC_PVTTYPE *pAudDecComPvt,
                                    void *staticparams)
{
    int i;
    IAUDDEC1_Params *pDecParams;
    IAACDEC_Params *params = (IAACDEC_Params *) staticparams;        
	
	/* Set Base params */
    pDecParams = &(params->auddec_params);
    pDecParams->size = sizeof(IAACDEC_Params);
    pDecParams->outputPCMWidth = AAC_OUTPUT_PCMWIDTH_16;
    pDecParams->pcmFormat = IAUDIO_INTERLEAVED;
    pDecParams->dataEndianness = XDM_BYTE;

	/* Set Extended params */
	/* sixChannelMode is not applicable to MPEG4 AAC-LC decoder 
	   and this field is ignored.
	 */
	params->sixChannelMode = 0;

    for (i = 0; i < OMX_AUDDEC_MAX_OUTPUT_BUFFER_COUNT; i++) {
        pAudDecComPvt->decOutArgsPtr = &(pAudDecComPvt->decOutArgs[i]);
        pAudDecComPvt->decOutArgsPtr->auddec1OutArgs.size = sizeof(pAudDecComPvt->decOutArgs[i]);
        pAudDecComPvt->decOutArgsPtr->auddec1OutArgs.extendedError = 0;
        pAudDecComPvt->decOutArgsPtr->auddec1OutArgs.bytesConsumed = 0;
    }

    return;
}
/**
********************************************************************************
 *  @func  OMX_TI_AACAD_Set_DynamicParams()
 *  @brief  This method is called for setting the default run time parameters for
 *          AAC decoder.
 *
 * @param [in] dynParams      : poinetr to dynamic parameters
 *
 *  @return
 *
*/
/* ========================================================================== */

void OMX_TI_AACAD_Set_DynamicParams(void *dynParams)
{
    IAUDDEC1_DynamicParams *pDecDynParams;
    IAACDEC_DynamicParams *params = (IAACDEC_DynamicParams *) dynParams;
    pDecDynParams = &(params->auddec_dynamicparams);

    pDecDynParams->size = sizeof(IAACDEC_DynamicParams);
  
	/* For decoding ADIF and ADTS streams, set the field bRawFormat to 0 and 
	 * ignore the ulSamplingRateIdx and nProfile fields.
	 * For decoding raw streams, set the field bRawFormat to 1 and the fields 
	 * ulSamplingRateIdx and nProfile to one of the values given above.
     * nProfile will be 1 as the decoder supports only LC profile
     */
	
	/* bRawFormat: 0-ADIF/ADTS, 1-RAW */
    params->bRawFormat = 0;

	/* nProfile: 0-Main, 1-LC, 2-SSR, 3-LTP */
    params->nProfile = 1;

    /*ulSamplingRateIdx: SR in KHz	
		0-96, 1-88.2 , 2-64
		3-48, 4-44.1 , 5-32
		6-24, 7-22.05, 8-16
		9-12, 10-11.025,11-8
    */							
	params->ulSamplingRateIdx = 4;

	/* enablePS, pseudoSurroundEnableFlag and enableARIBDownmix are not 
	 * applicable MPEG4-AAC.LC Decoder and these fields will be ignored.
	 */
	params->enablePS = 0;
	params->pseudoSurroundEnableFlag = 0;
	params->enableARIBDownmix = 0;    
    
    return;
}
/**
********************************************************************************
 *  @func   OMX_AACAD_Init()
 *  @brief  This method is called for allocating  AAC specific parameters
 *          Based on codec requirement could be diffreent.
 * @param [in] hComponent      : handle of the component
 *
 *  @return
 *  OMX_ErrorNone = Successful
 *
 *  Other_value = Failed (Error code is returned)
 *
*/
/* ========================================================================== */

OMX_ERRORTYPE OMX_TI_AACAD_Init(OMX_HANDLETYPE hComponent)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_COMPONENTTYPE *pHandle = NULL;
    OMX_AUDDEC_PVTTYPE *pAudDecComPvt = NULL;

    pHandle = (OMX_COMPONENTTYPE *) hComponent;
    pAudDecComPvt = (OMX_AUDDEC_PVTTYPE *) pHandle->pComponentPrivate;
    /*printf ("\nDJDBG_AACAD init\n");*/
    snprintf ( pAudDecComPvt->decoderName,
             ( sizeof ( pAudDecComPvt->decoderName ) - 1 ),
             decoderName);

    if (pAudDecComPvt->pDecStaticParams != NULL)
        TIMM_OSAL_Free(pAudDecComPvt->pDecStaticParams);

    pAudDecComPvt->pDecStaticParams =
        (IAACDEC_Params *) TIMM_OSAL_Malloc(sizeof(IAACDEC_Params),
                                              TIMM_OSAL_TRUE, 0,
                                              TIMMOSAL_MEM_SEGMENT_EXT);
    OMX_BASE_ASSERT(pAudDecComPvt->pDecStaticParams != NULL,
                    OMX_ErrorInsufficientResources);
    TIMM_OSAL_Memset(pAudDecComPvt->pDecStaticParams, 0x0,
                     sizeof(IAACDEC_Params));

    if (pAudDecComPvt->pDecDynParams != NULL)
        TIMM_OSAL_Free(pAudDecComPvt->pDecDynParams);

    pAudDecComPvt->pDecDynParams =
        (IAACDEC_DynamicParams *)
        TIMM_OSAL_Malloc(sizeof(IAACDEC_DynamicParams), TIMM_OSAL_TRUE, 0,
                         TIMMOSAL_MEM_SEGMENT_EXT);
    OMX_BASE_ASSERT(pAudDecComPvt->pDecDynParams != NULL,
                    OMX_ErrorInsufficientResources);
    TIMM_OSAL_Memset(pAudDecComPvt->pDecDynParams, 0x0,
                     sizeof(IAACDEC_DynamicParams));

    if (pAudDecComPvt->pDecStatus != NULL)
        TIMM_OSAL_Free(pAudDecComPvt->pDecStatus);

    pAudDecComPvt->pDecStatus =
        (AUDDEC1_Status *) TIMM_OSAL_Malloc(sizeof(IAACDEC_Status),
                                            TIMM_OSAL_TRUE, 0,
                                            TIMMOSAL_MEM_SEGMENT_EXT);
    OMX_BASE_ASSERT(pAudDecComPvt->pDecStatus != NULL,
                    OMX_ErrorInsufficientResources);
    TIMM_OSAL_Memset(pAudDecComPvt->pDecStatus, 0x0, sizeof(IAACDEC_Status));

    pAudDecComPvt->fpSet_StaticParams(hComponent,
                                      pAudDecComPvt->pDecStaticParams);

    pAudDecComPvt->pDecStatus->size = sizeof(IAACDEC_Status);

  EXIT:
    return eError;
}

static volatile int FRAME_SENT = 0;
static volatile int FRAME_CANCELED = 0;
OMX_ERRORTYPE OMX_TI_AACAD_CB_ProcessDone(void *hComponent)
{
    OMX_AUDDEC_PVTTYPE *pAudDecComPvt;

    OMX_COMPONENTTYPE *pComp;

    OMX_ERRORTYPE eError = OMX_ErrorNone;

    OMX_BUFFERHEADERTYPE *pInBufHeader = NULL;

    OMX_BUFFERHEADERTYPE *pOutBufHeader = NULL;

    //XDAS_Int32 status;

    OMX_U32 nBufferCountActual;

    //XDAS_UInt32  i, ii;

    //OMX_U32 cancelRequired;

    IAACDEC_OutArgs  *pArgsPtr;

    pComp = (OMX_COMPONENTTYPE *) hComponent;

    pAudDecComPvt = (OMX_AUDDEC_PVTTYPE *) pComp->pComponentPrivate;

    pInBufHeader = pAudDecComPvt->inBufHeaderArray[pAudDecComPvt->inBufArrayIndx].pBufHeader;

    pOutBufHeader = pAudDecComPvt->outBufHeaderArray[pAudDecComPvt->outBufArrayIndx].pBufHeader;

    pArgsPtr = (IAACDEC_OutArgs *)&pAudDecComPvt->decOutArgs[pAudDecComPvt->decOutArgsIdx];

    nBufferCountActual =
    pAudDecComPvt->pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->nBufferCountActual;

    pOutBufHeader->nFilledLen = pArgsPtr->auddec_outArgs.numSamples*4;

    pAudDecComPvt->fpDioSend(pComp, OMX_AUDDEC_OUTPUT_PORT,
                                     pOutBufHeader);
            OMX_TI_AUDDEC_UTL_SetBufStatus(&(pAudDecComPvt->outBufHeaderArray[0]),
                                nBufferCountActual,
                                pOutBufHeader, OMX_BUFFER_FREE);
    FRAME_SENT++;
    
    if (pInBufHeader != NULL) {
        pInBufHeader->nFilledLen =
         (pInBufHeader->nFilledLen-pArgsPtr->auddec_outArgs.bytesConsumed);
        pAudDecComPvt->fpDioSend(pComp, OMX_AUDDEC_INPUT_PORT,
                                 pInBufHeader);
        OMX_TI_AUDDEC_UTL_SetBufStatus(&(pAudDecComPvt->inBufHeaderArray[0]),
                            pAudDecComPvt->pPortdefs[OMX_AUDDEC_INPUT_PORT]->nBufferCountActual,
                            pInBufHeader, OMX_BUFFER_FREE);
    }
    
//  EXIT:
    return(eError);
}

/* Function to map TI channel mode to OMX channel mode */
OMX_AUDIO_CHANNELMODETYPE mapTiChannelModeToOmxChannelMode(IAUDIO_ChannelMode channelMode)
{
	OMX_AUDIO_CHANNELMODETYPE omxChanelModeType = 0;
	switch(channelMode) {
	  case IAUDIO_1_0: 
		  omxChanelModeType = OMX_AUDIO_ChannelModeMono;
		  break;
	  case IAUDIO_2_0: 
		  omxChanelModeType = OMX_AUDIO_ChannelModeStereo;
		  break;
	  case IAUDIO_11_0: 
		  omxChanelModeType = OMX_AUDIO_ChannelModeDual;
		  break;	  
	  default:
          omxChanelModeType = OMX_AUDIO_ChannelModeStereo;
		  break;
	}
	return (omxChanelModeType);
}
