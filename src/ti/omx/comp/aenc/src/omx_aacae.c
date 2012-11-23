/*******************************************************************************
 * Texas Instruments OpenMax Multimedia Sub-system                             *
 *                                                                             *
 * "OpenMax Multimedia Sub-system" is a software module developed on TI's DM   *
 * class of SOCs. These modules are based on Khronos open max 1.1.2            *
 * specification.                                6                             *
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 ******************************************************************************/
/*
 * ****************************************************************************
 *  @file  omx_aacae.c
 *  @brief This file contains methods that provides the functionality for a
 *         AAC Audio Encoder OpenMax 1.1.2 Component.
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
#include <ti/sdo/ce/audio1/audenc1.h>
#include "timm_osal_interfaces.h"

/*-------------------------program files -------------------------------------*/
#include "ti/omx/interfaces/openMaxv11/OMX_Core.h"
#include <omx_base.h>
#include "omx_base_utils.h"
#include "omx_aacae_internal.h"
#include "omx_aenc.h"
#include "omx_aenc_util.h"
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
//extern int FRAME_ENCODED[];

/*----------------------- function prototypes --------------------------------*/

/*****************************************************************************
 * PRIVATE DECLARATIONS Defined here, used only here
 ******************************************************************************/

/** Codec name. This should be same as the one created in the cfg file */
static String encoderName = "aaclcenc";
/**
 *  min - The macro finds the minimum of two values
*/
#define min(a,b) ((a) < (b) ? (a) : (b))

/**
**
********************************************************************************
 *  @func  OMX_TI_AACAE_Set_StaticParams()
 *  @brief  This method is called for setting the create time parameters for
 *          AAC encoder. height and width are not used currently, and port
 *          parameters are used instead.
 *
 * @param [in] hComponent      : handle of the component
 *
 *
*/
/* ========================================================================== */
extern int globdj_aenc_cnt[];

void OMX_TI_AACAE_Set_StaticParams(OMX_HANDLETYPE hComponent,
                                    void *staticparams)
{
    int i;
    IAUDENC1_Params *pEncParams;
    IMP4AACENC_Params *params = (IMP4AACENC_Params *) staticparams;
    OMX_COMPONENTTYPE *pHandle = NULL;
    OMX_AUDENC_PVTTYPE *pAudEncComPvt = NULL;

    pHandle = (OMX_COMPONENTTYPE *) hComponent;
    pAudEncComPvt = (OMX_AUDENC_PVTTYPE *) pHandle->pComponentPrivate;
    pEncParams = &(params->audenc_params);

    pEncParams->size = sizeof(IMP4AACENC_Params);

	pEncParams->bitRate = 128000;
	pEncParams->ancFlag = 0;
	pEncParams->channelMode = IAUDIO_2_0;
	pEncParams->dataEndianness = (XDAS_Int32)(XDM_LE_16);
	pEncParams->crcFlag        = 0;
	pEncParams->dualMonoMode = 0;
	pEncParams->encMode = IAUDIO_CBR;
	pEncParams->inputBitsPerSample = 16;
	pEncParams->inputFormat = IAUDIO_INTERLEAVED;
	pEncParams->lfeFlag = 0;
	pEncParams->maxBitRate = 192000;// 800000 is the maximum bitrate for CBR but
									// as we are not supporting VBR we need not
									// assign its value.

	pEncParams->sampleRate = 44100;

	params->aot = IMP4AACENC_AOT_LC;
	//params->transMode = IMP4AACENC_TT_RAW;
	params->transMode = IMP4AACENC_TT_ADTS;
	//params->transMode = IMP4AACENC_TT_ADIF;



    for (i = 0; i < OMX_AUDENC_MAX_OUTPUT_BUFFER_COUNT; i++) {
        pAudEncComPvt->encOutArgsPtr = &(pAudEncComPvt->encOutArgs[i]);
        pAudEncComPvt->encOutArgsPtr->size = sizeof(pAudEncComPvt->encOutArgs[i]);
        pAudEncComPvt->encOutArgsPtr->extendedError = 0;
        pAudEncComPvt->encOutArgsPtr->bytesGenerated = 0;
    }

    return;
}
/**
********************************************************************************
 *  @func  OMX_TI_AACAE_Set_DynamicParams()
 *  @brief  This method is called for setting the default run time parameters for
 *          AAC encoder.
 *
 * @param [in] dynParams      : poinetr to dynamic parameters
 *
 *  @return
 *
*/
/* ========================================================================== */

void OMX_TI_AACAE_Set_DynamicParams(void *dynParams)
{
    IAUDENC1_DynamicParams *pEncDynParams;
    IMP4AACENC_DynamicParams *params = (IMP4AACENC_DynamicParams *) dynParams;
    pEncDynParams = &(params->audenc_dynamicparams);

    pEncDynParams->size = sizeof(IMP4AACENC_DynamicParams);
    
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

OMX_ERRORTYPE OMX_TI_AACAE_Init(OMX_HANDLETYPE hComponent)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_COMPONENTTYPE *pHandle = NULL;
    OMX_AUDENC_PVTTYPE *pAudEncComPvt = NULL;

    pHandle = (OMX_COMPONENTTYPE *) hComponent;
    pAudEncComPvt = (OMX_AUDENC_PVTTYPE *) pHandle->pComponentPrivate;
    snprintf ( pAudEncComPvt->encoderName,
             ( sizeof ( pAudEncComPvt->encoderName ) - 1 ),
             encoderName);

    if (pAudEncComPvt->pEncStaticParams != NULL)
        TIMM_OSAL_Free(pAudEncComPvt->pEncStaticParams);

    pAudEncComPvt->pEncStaticParams =
        (IMP4AACENC_Params *) TIMM_OSAL_Malloc(sizeof(IMP4AACENC_Params),
                                              TIMM_OSAL_TRUE, 0,
                                              TIMMOSAL_MEM_SEGMENT_EXT);
    OMX_BASE_ASSERT(pAudEncComPvt->pEncStaticParams != NULL,
                    OMX_ErrorInsufficientResources);
    TIMM_OSAL_Memset(pAudEncComPvt->pEncStaticParams, 0x0,
                     sizeof(IMP4AACENC_Params));

    if (pAudEncComPvt->pEncDynParams != NULL)
        TIMM_OSAL_Free(pAudEncComPvt->pEncDynParams);

    pAudEncComPvt->pEncDynParams =
        (IMP4AACENC_DynamicParams *)
        TIMM_OSAL_Malloc(sizeof(IMP4AACENC_DynamicParams), TIMM_OSAL_TRUE, 0,
                         TIMMOSAL_MEM_SEGMENT_EXT);
    OMX_BASE_ASSERT(pAudEncComPvt->pEncDynParams != NULL,
                    OMX_ErrorInsufficientResources);
    TIMM_OSAL_Memset(pAudEncComPvt->pEncDynParams, 0x0,
                     sizeof(IMP4AACENC_DynamicParams));

    if (pAudEncComPvt->pEncStatus != NULL)
        TIMM_OSAL_Free(pAudEncComPvt->pEncStatus);

    pAudEncComPvt->pEncStatus =
        (AUDENC1_Status *) TIMM_OSAL_Malloc(sizeof(IMP4AACENC_Status),
                                            TIMM_OSAL_TRUE, 0,
                                            TIMMOSAL_MEM_SEGMENT_EXT);
    OMX_BASE_ASSERT(pAudEncComPvt->pEncStatus != NULL,
                    OMX_ErrorInsufficientResources);
    TIMM_OSAL_Memset(pAudEncComPvt->pEncStatus, 0x0, sizeof(IMP4AACENC_Status));

    pAudEncComPvt->fpSet_StaticParams(hComponent,
                                      pAudEncComPvt->pEncStaticParams);

    pAudEncComPvt->pEncStatus->size = sizeof(IMP4AACENC_Status);

  EXIT:
    return eError;
}

AUDENC_BufParams OMX_TI_AACAE_Outbuff_Details(void *OutArgs, OMX_U32 portIndex)
{
    AUDENC_BufParams OutBuffDetails;
    /*IMP4AACENC_OutArgs *outargs = (IMP4AACENC_OutArgs *) OutArgs;*/

    if (portIndex == OMX_AUDENC_OUTPUT_PORT) {
        OutBuffDetails.nBufferCountMin = 1;
        OutBuffDetails.nBufferCountActual = 1;
    } else if (portIndex == OMX_AUDENC_INPUT_PORT) {
        OutBuffDetails.nBufferCountMin = 1;
        OutBuffDetails.nBufferCountActual = 1;
    }

    return OutBuffDetails;
}


static volatile int FRAME_SENT = 0;
static volatile int FRAME_CANCELED = 0;
OMX_ERRORTYPE OMX_TI_AACAE_CB_ProcessDone(void *hComponent)
{
    OMX_AUDENC_PVTTYPE *pAudEncComPvt;

    OMX_COMPONENTTYPE *pComp;

    OMX_ERRORTYPE eError = OMX_ErrorNone;

    OMX_BUFFERHEADERTYPE *pInBufHeader = NULL;

    OMX_BUFFERHEADERTYPE *pOutBufHeader = NULL;

    OMX_U32 nBufferCountActual;

    IMP4AACENC_OutArgs  *pArgsPtr;

    pComp = (OMX_COMPONENTTYPE *) hComponent;

    pAudEncComPvt = (OMX_AUDENC_PVTTYPE *) pComp->pComponentPrivate;

    pInBufHeader = pAudEncComPvt->inBufHeaderArray[pAudEncComPvt->inBufArrayIndx].pBufHeader;

    pOutBufHeader = pAudEncComPvt->outBufHeaderArray[pAudEncComPvt->outBufArrayIndx].pBufHeader;

    pArgsPtr = (IMP4AACENC_OutArgs *)&pAudEncComPvt->encOutArgs[pAudEncComPvt->encOutArgsIdx];

    /* if reset completed just now, there is no valid buffers now */

    if (pAudEncComPvt->cmdStatus == RESET_DONE)
        return (eError);


    nBufferCountActual =
    			pAudEncComPvt->pPortdefs[OMX_AUDENC_OUTPUT_PORT]->nBufferCountActual;

    pAudEncComPvt->numBytesConsumed += pArgsPtr->audenc_outArgs.bytesGenerated;

    pOutBufHeader->nOffset = 0;
    pOutBufHeader->nFilledLen = pArgsPtr->audenc_outArgs.bytesGenerated;

    pAudEncComPvt->fpDioSend(pComp, OMX_AUDENC_OUTPUT_PORT,
                                     pOutBufHeader);

    OMX_TI_AUDENC_UTL_SetBufStatus(&(pAudEncComPvt->outBufHeaderArray[0]),
                                nBufferCountActual,
                                pOutBufHeader, OMX_BUFFER_FREE);
    FRAME_SENT++;

    /* if some data still left in the input buff but have runout of o/p buffs
     * in the pipe then store the current input buff info */
    /* in case flush done, no valid input left */
    if (pAudEncComPvt->cmdStatus != STREAM_FLUSH_ON) {
        if (pInBufHeader != NULL) {

            pAudEncComPvt->fpDioSend(pComp, OMX_AUDENC_INPUT_PORT,
                                     pInBufHeader);

            OMX_TI_AUDENC_UTL_SetBufStatus(&(pAudEncComPvt->inBufHeaderArray[0]),
                                pAudEncComPvt->pPortdefs[OMX_AUDENC_INPUT_PORT]->nBufferCountActual,
                                pInBufHeader, OMX_BUFFER_FREE);

        }
    }
/*To suppress a warning*/
/*  EXIT:*/
    return(eError);
}
