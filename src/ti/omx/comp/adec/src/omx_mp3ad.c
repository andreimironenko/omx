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
 *  @file  omx_mp3ad.c 
 *  @brief This file contains methods that provides the functionality for a 
 *         MP3 Audio Decoder OpenMax 1.1.2 Component.
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
#include "omx_mp3ad_internal.h"
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
static String decoderName = "mp3dec";

/** 
 *  min - The macro finds the minimum of two values
*/
#define min(a,b) ((a) < (b) ? (a) : (b))
/**
/**
********************************************************************************
 *  @func  OMX_TI_MP3AD_Set_StaticParams() 
 *  @brief  This method is called for setting the create time parameters for 
 *          MP3 decoder.
 * @param [in] pAudDecComPvt   :    handle of the component private
 * @param [in] staticparams    :    Codec create time parameter
 *
*/
/* ========================================================================== */
void OMX_TI_MP3AD_Set_StaticParams(OMX_AUDDEC_PVTTYPE * pAudDecComPvt,
                                    void *staticparams)
{
    int i;
    IAUDDEC1_Params *pDecParams;
    IMP3DEC_Params *params = (IMP3DEC_Params *) staticparams;
    
    pDecParams = &(params->auddec_params);

    pDecParams->size = sizeof(IMP3DEC_Params);
    pDecParams->outputPCMWidth = MP3_OUTPUT_PCMWIDTH_16;
    pDecParams->pcmFormat = IAUDIO_INTERLEAVED;
    pDecParams->dataEndianness = XDM_BYTE;

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
 *  @func  OMX_TI_MP3AD_Set_DynamicParams() 
 *  @brief  This method is called for setting the default run time parameters for 
 *          MP3 decoder. 
 *
 * @param [in] dynParams      : poinetr to dynamic parameters
 *
 *  @return      
 *
*/
/* ========================================================================== */

void OMX_TI_MP3AD_Set_DynamicParams(void *dynParams)
{
    IAUDDEC1_DynamicParams *pDecDynParams;
    IMP3DEC_DynamicParams *params = (IMP3DEC_DynamicParams *) dynParams;
    pDecDynParams = &(params->auddec_dynamicparams);

    pDecDynParams->size = sizeof(IMP3DEC_DynamicParams);
	/* For MP3, SBR flag is ignored within Codec since it is not supported*/
    pDecDynParams->downSampleSbrFlag = 0;   
    return;
}
/**
********************************************************************************
 *  @func   OMX_MP3AD_Init() 
 *  @brief  This method is called for allocating  MP3 specific parameters
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

OMX_ERRORTYPE OMX_TI_MP3AD_Init(OMX_HANDLETYPE hComponent)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_COMPONENTTYPE *pHandle = NULL;
    OMX_AUDDEC_PVTTYPE *pAudDecComPvt = NULL;

    pHandle = (OMX_COMPONENTTYPE *) hComponent;
    pAudDecComPvt = (OMX_AUDDEC_PVTTYPE *) pHandle->pComponentPrivate;
    //printf ("\nDJDBG_MPEG2VD init\n");
    snprintf ( pAudDecComPvt->decoderName,
             ( sizeof ( pAudDecComPvt->decoderName ) - 1 ),
             decoderName);

    if (pAudDecComPvt->pDecStaticParams != NULL)
        TIMM_OSAL_Free(pAudDecComPvt->pDecStaticParams);

    pAudDecComPvt->pDecStaticParams =
        (IMP3DEC_Params *) TIMM_OSAL_Malloc(sizeof(IMP3DEC_Params),
                                              TIMM_OSAL_TRUE, 0,
                                              TIMMOSAL_MEM_SEGMENT_EXT);
    OMX_BASE_ASSERT(pAudDecComPvt->pDecStaticParams != NULL,
                    OMX_ErrorInsufficientResources);
    TIMM_OSAL_Memset(pAudDecComPvt->pDecStaticParams, 0x0,
                     sizeof(IMP3DEC_Params));

    if (pAudDecComPvt->pDecDynParams != NULL)
        TIMM_OSAL_Free(pAudDecComPvt->pDecDynParams);

    pAudDecComPvt->pDecDynParams =
        (IMP3DEC_DynamicParams *)
        TIMM_OSAL_Malloc(sizeof(IMP3DEC_DynamicParams), TIMM_OSAL_TRUE, 0,
                         TIMMOSAL_MEM_SEGMENT_EXT);
    OMX_BASE_ASSERT(pAudDecComPvt->pDecDynParams != NULL,
                    OMX_ErrorInsufficientResources);
    TIMM_OSAL_Memset(pAudDecComPvt->pDecDynParams, 0x0,
                     sizeof(IMP3DEC_DynamicParams));

    if (pAudDecComPvt->pDecStatus != NULL)
        TIMM_OSAL_Free(pAudDecComPvt->pDecStatus);

    pAudDecComPvt->pDecStatus =
        (AUDDEC1_Status *) TIMM_OSAL_Malloc(sizeof(IMP3DEC_Status),
                                            TIMM_OSAL_TRUE, 0,
                                            TIMMOSAL_MEM_SEGMENT_EXT);
    OMX_BASE_ASSERT(pAudDecComPvt->pDecStatus != NULL,
                    OMX_ErrorInsufficientResources);
    TIMM_OSAL_Memset(pAudDecComPvt->pDecStatus, 0x0, sizeof(IMP3DEC_Status));

    pAudDecComPvt->fpSet_StaticParams(hComponent,
                                      pAudDecComPvt->pDecStaticParams);

    pAudDecComPvt->pDecStatus->size = sizeof(IMP3DEC_Status);

  EXIT:
    return eError;
}

volatile int FRAME_SENT = 0;
volatile int FRAME_CANCELED = 0;
OMX_ERRORTYPE OMX_TI_MP3AD_CB_ProcessDone(void *hComponent)
{
    OMX_AUDDEC_PVTTYPE *pAudDecComPvt;

    OMX_COMPONENTTYPE *pComp;

    OMX_ERRORTYPE eError = OMX_ErrorNone;

    OMX_BUFFERHEADERTYPE *pInBufHeader = NULL;

    OMX_BUFFERHEADERTYPE *pOutBufHeader = NULL;

    XDAS_Int32 status;    

    OMX_U32 nBufferCountActual;

    XDAS_UInt32  i, ii;

    OMX_U32 cancelRequired;

    IMP3DEC_OutArgs  *pArgsPtr;

    pComp = (OMX_COMPONENTTYPE *) hComponent;
  
    pAudDecComPvt = (OMX_AUDDEC_PVTTYPE *) pComp->pComponentPrivate;
  
    pInBufHeader = pAudDecComPvt->inBufHeaderArray[pAudDecComPvt->inBufArrayIndx].pBufHeader;

    pOutBufHeader = pAudDecComPvt->outBufHeaderArray[pAudDecComPvt->outBufArrayIndx].pBufHeader;

    pArgsPtr = (IMP3DEC_OutArgs *)&pAudDecComPvt->decOutArgs[pAudDecComPvt->decOutArgsIdx];

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
  EXIT:
    return(eError);
}
