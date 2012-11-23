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
 *  @file  omx_adec.c
 *  @brief This file contains methods that provides the functionality for a
 *         Audio Decoder OpenMax 1.1 Component.
 *         This file can be used as a reference to develop OMX1.1 Compliant
 *         component which uses the OMX 1.1 compliant Base component for BIOS6
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
#include <xdc/runtime/Gate.h>
#include <string.h>
#include <stdio.h>
#include <ti/sdo/ce/Engine.h>
#include <ti/sdo/ce/audio1/auddec1.h>
#include <ti/sysbios/heaps/HeapMem.h>
#include <xdc/runtime/knl/Cache.h>

#include "timm_osal_interfaces.h"

/*-------------------------program files -------------------------------------*/
#include "ti/omx/interfaces/openMaxv11/OMX_Core.h"
#include "OMX_TI_Index.h"

#include <omx_base.h>
#include <ti/omx/comp/omxbase/omx_base_comp/inc/omx_base_internal.h>
#include <ti/sdo/fc/memutils/memutils.h>
#include "omx_base_utils.h"
#include "omx_adec_priv.h"
#include "omx_adec_util.h"

#include "omx_adec.h"

#ifdef CODEC_MP3DEC
#include "omx_mp3ad_internal.h"
#endif

#ifdef CODEC_AACDEC
#include "omx_aacad_internal.h"
#endif

#include "OMX_TI_Audio.h"

/*******************************************************************************
 * EXTERNAL REFERENCES NOTE : only use if not found in header file
*******************************************************************************/

/*------------------------ data declarations ---------------------------------*/


/*----------------------- function prototypes --------------------------------*/

/*******************************************************************************
 * PUBLIC DECLARATIONS Defined here, used elsewhere
 ******************************************************************************/

/*------------------------ data declarations ---------------------------------*/
/* None */

/*----------------------- function prototypes --------------------------------*/

/*****************************************************************************
 * PRIVATE DECLARATIONS Defined here, used only here
 ******************************************************************************/

/** Codec engine name. This should be same as the one created in the cfg file */
static String engineName = "adec";

OMX_U32 g_AudDecComponentCreated = 0;
OMX_AudDecCallbacks OMX_AudDecCallbacksTable[OMX_AudDecCallbacksTableEntryCnt] = {
#ifdef CODEC_MP3DEC
    {
     OMX_AUDIO_CodingMP3,
     OMX_TI_MP3AD_Init,
     OMX_TI_MP3AD_Set_StaticParams,
     OMX_TI_MP3AD_Set_DynamicParams,
     NULL,
     }
#endif
#if (OMX_AudDecCallbacksTableEntryCnt > 1)
     ,
#endif
#ifdef CODEC_AACDEC
    {
     OMX_AUDIO_CodingAAC,
     OMX_TI_AACAD_Init,
     OMX_TI_AACAD_Set_StaticParams,
     OMX_TI_AACAD_Set_DynamicParams,
     NULL,
     }
#endif
};


/**
********************************************************************************
 *  @func   OMX_AUDDEC_ComponentInit()
 *  @brief  Entry point for OMX Audio Decoder Component.
 *          This function is called whenever the IL Client invokes a GetHandle
 *          function with the name "OMX.TI.DSP.AUDDEC"
 *          This is the first call to component and is used to perform one time
 *          initialization specific to component. Component may assume that the
 *          Application has allocated the handle and filled in nSize and nVersion
 *          fields. This method should also allocate and fill in the component
 *           private data structure.
 *
 *  @param [in]  hComponent    Handle of the component to be accessed.
 *
 *  @return
 *  OMX_ErrorNone = Successful
 *  Other_value = Failed (Error code is returned)
********************************************************************************
*/

OMX_ERRORTYPE OMX_TI_AUDDEC_ComponentInit(OMX_HANDLETYPE hComponent)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;

    eError =
        OMX_TI_AUDDEC_ComponentInit_Common(hComponent,
                                          OMX_AUDDEC_DEFAULT_COMPRESSION_FORMAT);
    return(eError);
}

/**
********************************************************************************
 *  @func   OMX_TI_AUDDEC_ComponentInit_Common()
 *  @brief   This function is called whenever the IL Client invokes a GetHandle
 *          function with the name "OMX.TI.DSP.AUDDEC".
 *          This is the first call to component and is used to perform one time
 *          initialization specific to component. Component may assume that the
 *          Application has allocated the handle and filled in nSize and nVersion
 *          fields. This method should also allocate and fill in the component
            private data structure.
 *
 *  @param [in]  hComponent           Handle of the component to be accessed.
 *  @param [in]  compressionFormat    codec type.
  *
 *  @return
 *  OMX_ErrorNone = Successful
 *  Other_value = Failed (Error code is returned)
********************************************************************************
*/
OMX_ERRORTYPE OMX_TI_AUDDEC_ComponentInit_Common(OMX_HANDLETYPE hComponent,
                                                 OMX_AUDIO_CODINGTYPE
                                                 compressionFormat)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;

    OMX_AUDDEC_PVTTYPE *pAudDecComPvt = NULL;

    OMX_COMPONENTTYPE *pHandle = TIMM_OSAL_NULL;

    OMX_U32 i = 0;

    /* Check the input parameters, this should be TRUE else report an Error */
    OMX_BASE_REQUIRE(hComponent != NULL, OMX_ErrorBadParameter);

    pHandle = (OMX_COMPONENTTYPE *) hComponent;

    OMX_BASE_CHK_VERSION(pHandle, OMX_COMPONENTTYPE, eError);

    /* Allocate memory for AUDDEC Component's private data area */
    pHandle->pComponentPrivate =
        (OMX_AUDDEC_PVTTYPE *) TIMM_OSAL_Malloc(sizeof(OMX_AUDDEC_PVTTYPE),
                                                TIMM_OSAL_TRUE, 0,
                                                TIMMOSAL_MEM_SEGMENT_EXT);
    OMX_BASE_ASSERT(pHandle->pComponentPrivate != NULL,
                    OMX_ErrorInsufficientResources);

    TIMM_OSAL_Memset(pHandle->pComponentPrivate, 0x0,
                     sizeof(OMX_AUDDEC_PVTTYPE));

    pAudDecComPvt = (OMX_AUDDEC_PVTTYPE *) pHandle->pComponentPrivate;

    /* Initialize Derived to Base params with Actual values; Fill the structure
       "OMX_BASE_PARAMSFROMDERIVED" which needs to be given as an input to the
       Base Component before calling Base Component Init method */

    pAudDecComPvt->tDerToBase.cComponentName =
        (OMX_STRING) TIMM_OSAL_Malloc(sizeof(OMX_U8) *
                                      OMX_MAX_STRINGNAME_SIZE, TIMM_OSAL_TRUE,
                                      0, TIMMOSAL_MEM_SEGMENT_EXT);
    OMX_BASE_ASSERT(pAudDecComPvt->tDerToBase.cComponentName != NULL,
                    OMX_ErrorInsufficientResources);

    TIMM_OSAL_Memset (pAudDecComPvt->tDerToBase.cComponentName, 0,
                      (sizeof ( OMX_U8 ) * OMX_MAX_STRINGNAME_SIZE));

    TIMM_OSAL_Memcpy(pAudDecComPvt->tDerToBase.cComponentName,
                     OMX_AUDDEC_COMPONENT_NAME,
                     strlen(OMX_AUDDEC_COMPONENT_NAME)+1);

    /* The component will use the default heap */

    pAudDecComPvt->tDerToBase.hDefaultHeap = NULL;

    /* Initialize Audio Port parameters */

    OMX_BASE_MALLOC_STRUCT(pAudDecComPvt->tDerToBase.pAudioPortParams,
                           OMX_PORT_PARAM_TYPE,
                           pAudDecComPvt->tDerToBase.hDefaultHeap);
    OMX_BASE_INIT_STRUCT_PTR(pAudDecComPvt->tDerToBase.pAudioPortParams,
                             OMX_PORT_PARAM_TYPE);
    pAudDecComPvt->tDerToBase.pAudioPortParams->nPorts =
        OMX_AUDDEC_NUM_OF_PORTS;
    pAudDecComPvt->tDerToBase.pAudioPortParams->nStartPortNumber =
        OMX_AUDDEC_DEFAULT_START_PORT_NUM;

    /* Setting number of ports - this has to be set before calling SetDefault
       Properties */

    pAudDecComPvt->tDerToBase.nNumPorts = OMX_AUDDEC_NUM_OF_PORTS;
    pAudDecComPvt->tDerToBase.nMinStartPortIndex =
        OMX_AUDDEC_DEFAULT_START_PORT_NUM;

    /* Allocating memory for port properties before calling SetDefaultProperties */

    pAudDecComPvt->tDerToBase.pPortProperties = (OMX_BASE_PORT_PROPERTIES **)
        TIMM_OSAL_Malloc(sizeof(OMX_BASE_PORT_PROPERTIES *) *
                         pAudDecComPvt->tDerToBase.nNumPorts, TIMM_OSAL_TRUE, 0,
                         TIMMOSAL_MEM_SEGMENT_EXT);

    OMX_BASE_ASSERT(pAudDecComPvt->tDerToBase.pPortProperties != NULL,
                    OMX_ErrorInsufficientResources);

    for (i = 0; i < (pAudDecComPvt->tDerToBase.nNumPorts); i++) {
        pAudDecComPvt->tDerToBase.pPortProperties[i] =
            (OMX_BASE_PORT_PROPERTIES *) TIMM_OSAL_Malloc
            (sizeof(OMX_BASE_PORT_PROPERTIES), TIMM_OSAL_TRUE, 0,
             TIMMOSAL_MEM_SEGMENT_EXT);
        TIMM_OSAL_Memset(pAudDecComPvt->tDerToBase.pPortProperties[i], 0x0,
                         sizeof(OMX_BASE_PORT_PROPERTIES));


        OMX_BASE_ASSERT(pAudDecComPvt->tDerToBase.pPortProperties[i] != NULL,
                        OMX_ErrorInsufficientResources);
    }
    /* Setting default properties. PreCondition: NumOfPorts is filled and all
       pointers allocated */

    OMX_BASE_SetDefaultProperties(hComponent);

    /* decoder task is based on notofication for every event */
    pAudDecComPvt->tDerToBase.pPortProperties[OMX_AUDDEC_INPUT_PORT]->
        eNotifyType = OMX_NOTIFY_TYPE_ALWAYS;
    pAudDecComPvt->tDerToBase.pPortProperties[OMX_AUDDEC_OUTPUT_PORT]->
        eNotifyType = OMX_NOTIFY_TYPE_ALWAYS;

    /* default memory type, it can be modified through set param in the IL client */
    pAudDecComPvt->tDerToBase.pPortProperties[OMX_AUDDEC_INPUT_PORT]->
            eBufMemoryType = OMX_BUFFER_MEMORY_DEFAULT;
  	pAudDecComPvt->tDerToBase.pPortProperties[OMX_AUDDEC_INPUT_PORT]->nNumComponentBuffers = OMX_AUDDEC_MAX_INPUT_BUFFER_COUNT;
    pAudDecComPvt->tDerToBase.pPortProperties[OMX_AUDDEC_INPUT_PORT]->pBufParams = NULL;

    pAudDecComPvt->tDerToBase.pPortProperties[OMX_AUDDEC_OUTPUT_PORT]->
        eBufMemoryType = OMX_BUFFER_MEMORY_DEFAULT;
	pAudDecComPvt->tDerToBase.pPortProperties[OMX_AUDDEC_OUTPUT_PORT]->nNumComponentBuffers = OMX_AUDDEC_MAX_INPUT_BUFFER_COUNT;
     pAudDecComPvt->tDerToBase.pPortProperties[OMX_AUDDEC_OUTPUT_PORT]->pBufParams = NULL;

    /* Overriding this value. Notify derived component only when data is
       available on all ports, however auddec component is implemented as signle
       port */

    pAudDecComPvt->tDerToBase.bNotifyForAnyPort = OMX_TRUE;

    /* Set the ContextType whether passive/Active depending on requirement,
       for auddec default is active */

    pAudDecComPvt->tDerToBase.ctxType = OMX_AUDDEC_DEFAULT_CONTEXT_TYPE;

    /* Incase of Active Context, set the Stacksize and GroupPriority */

    if (pAudDecComPvt->tDerToBase.ctxType == OMX_BASE_CONTEXT_ACTIVE) {
        snprintf(pAudDecComPvt->tDerToBase.cTaskName,
                 sizeof(pAudDecComPvt->tDerToBase.cTaskName),
                 "ADEC_%p", hComponent);
        pAudDecComPvt->tDerToBase.nStackSize = OMX_AUDDEC_TASK_STACKSIZE;
        pAudDecComPvt->tDerToBase.nPrioirty = OMX_AUDDEC_TASK_PRIORITY;
    }

    pAudDecComPvt->tDerToBase.nGroupID = OMX_AUDDEC_GROUPID;
    pAudDecComPvt->tDerToBase.nGroupPriority = OMX_AUDDEC_GROUPPRIORITY;

    /* Fill component's version, this may not be same as the OMX Specification
       version */
    pAudDecComPvt->tDerToBase.nComponentVersion.s.nVersionMajor =
        OMX_AUDDEC_COMP_VERSION_MAJOR;
    pAudDecComPvt->tDerToBase.nComponentVersion.s.nVersionMinor =
        OMX_AUDDEC_COMP_VERSION_MINOR;
    pAudDecComPvt->tDerToBase.nComponentVersion.s.nRevision =
        OMX_AUDDEC_COMP_VERSION_REVISION;
    pAudDecComPvt->tDerToBase.nComponentVersion.s.nStep =
        OMX_AUDDEC_COMP_VERSION_STEP;

    /* Hooks to Notify Command and Data from Base Comp to Derived Comp */

    pAudDecComPvt->tDerToBase.fpCommandNotify = OMX_TI_AUDDEC_CommandNotify;
    pAudDecComPvt->tDerToBase.fpDataNotify = OMX_TI_AUDDEC_DataNotify;

    /* Initializing the Base Component */
    eError = OMX_BASE_ComponentInit(hComponent);
    OMX_BASE_ASSERT(OMX_ErrorNone == eError, eError);

    /* Here we can override whatever default the BaseComponent had set, we can
       override the function pointers in the private structure  */

    pAudDecComPvt = (OMX_AUDDEC_PVTTYPE *) pHandle->pComponentPrivate;

    /* Override the function pointers of pHandle "OMX_COMPONENTTYPE" which Base
       cannot handle and needs to be taken care by the Dervied comp */

    pHandle->GetParameter = OMX_TI_AUDDEC_GetParameter;
    pHandle->SetParameter = OMX_TI_AUDDEC_SetParameter;
    pHandle->SetConfig = OMX_TI_AUDDEC_SetConfig;
    pHandle->GetConfig = OMX_TI_AUDDEC_GetConfig;
    pHandle->ComponentDeInit = OMX_TI_AUDDEC_ComponentDeinit;

    /* Initialize the call back function pointers specific to this decoder */

    OMX_TI_AUDDEC_InitCallbacks(hComponent, compressionFormat);

    /* Set the default port properties, codec engine init */

    eError = OMX_TI_AUDDEC_SetDefault((OMX_AUDDEC_PVTTYPE *) pAudDecComPvt,
                                      compressionFormat);
    OMX_BASE_ASSERT(OMX_ErrorNone == eError, eError);   

  EXIT:
    if (eError != OMX_ErrorNone) {
        OMX_BASE_Error(" ADEC Comp Initialization Failed...! ");
        if (pHandle) {
            pHandle->ComponentDeInit(hComponent);
        }
    }    
    g_AudDecComponentCreated++;

    return eError;
}

/* ========================================================================== */
/* ========================================================================== */

/**
********************************************************************************
 *  @func   OMX_TI_AUDDEC_GetParameter()
 *  @brief  This method will get one of the parameter settings from the
 *          component. nParamIndex specifies which strucutre is requested
 *          from the component.caller shall provide memory for the structure and
 *          populate the nSize and nVersion fileds before invoking this macro.
 *          This method redirects the call to OMX_BASE_GetParameter which
 *          supports standard component parameters.
 *          Note 1 :  Derived Comp owner's need to take care of the supported
 *          domain specific indexes which can't be taken care in the Base
 *          Component.
 *          Note 2:   This method can be ivnoked when the comp is in any state
 *          except the OMX_StateInvalid state.
 *
 *  @param [in] hComponent   : Component handle
 *  @param [in] nParamIndex  : Index of the structure to be filled
 *  @param [in] pParamStruct : Pointer to the IL client allocated structure that
                               the comp fills
 *
 *  @return
 *  OMX_ErrorNone = Successful
 *
 *  Other_value = Failed (Error code is returned)
 *
*/
/* ========================================================================== */

static OMX_ERRORTYPE OMX_TI_AUDDEC_GetParameter(OMX_HANDLETYPE hComponent,
                                                OMX_INDEXTYPE nParamIndex,
                                                OMX_PTR pParamStruct)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;

    OMX_AUDDEC_PVTTYPE *pAudDecComPvt;

    OMX_COMPONENTTYPE *pHandle;

    OMX_AUDIO_PARAM_PORTFORMATTYPE *pAudioPortParams;

    OMX_BASE_REQUIRE((hComponent != NULL)
                     && (pParamStruct != NULL), OMX_ErrorBadParameter);

    pHandle = (OMX_COMPONENTTYPE *) hComponent;
    pAudDecComPvt = (OMX_AUDDEC_PVTTYPE *) pHandle->pComponentPrivate;

    /* GetParameter can't be invoked incase the comp is in Invalid State */

    OMX_BASE_ASSERT(pAudDecComPvt->tCurState != OMX_StateInvalid,
                    OMX_ErrorIncorrectStateOperation);

    switch (nParamIndex) {

        /* Client uses this to retrieve the Info related to the params of audio
           ports */

    case OMX_IndexParamPortDefinition:
        {

            OMX_BASE_CHK_VERSION(pParamStruct, OMX_PARAM_PORTDEFINITIONTYPE,
                                 eError);

            if (((OMX_PARAM_PORTDEFINITIONTYPE *) (pParamStruct))->nPortIndex
                ==
                pAudDecComPvt->pPortdefs[OMX_AUDDEC_INPUT_PORT]->nPortIndex) {
                TIMM_OSAL_Memcpy(pParamStruct,
                                 pAudDecComPvt->
                                 pPortdefs[OMX_AUDDEC_INPUT_PORT],
                                 sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
            } else if (((OMX_PARAM_PORTDEFINITIONTYPE *) (pParamStruct))->
                       nPortIndex ==
                       pAudDecComPvt->
                       pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->nPortIndex) {
                TIMM_OSAL_Memcpy(pParamStruct,
                                 pAudDecComPvt->
                                 pPortdefs[OMX_AUDDEC_OUTPUT_PORT],
                                 sizeof(OMX_PARAM_PORTDEFINITIONTYPE));


            } else {
                eError = OMX_ErrorBadPortIndex;
            }
        }
        break;

        /* client uses this to query the format supported by the port   */

    case OMX_IndexParamAudioPortFormat:
        {
            OMX_BASE_CHK_VERSION(pParamStruct, OMX_AUDIO_PARAM_PORTFORMATTYPE,
                                 eError);

            pAudioPortParams = (OMX_AUDIO_PARAM_PORTFORMATTYPE *) pParamStruct;

            if (pAudioPortParams->nPortIndex ==
                pAudDecComPvt->pPortdefs[OMX_AUDDEC_INPUT_PORT]->nPortIndex) {
                if (pAudioPortParams->nIndex >
                    pAudDecComPvt->tAudioParams[OMX_AUDDEC_INPUT_PORT].nIndex) {
                    eError = OMX_ErrorNoMore;
                } else {
                    *(pAudioPortParams) =
                        pAudDecComPvt->tAudioParams[OMX_AUDDEC_INPUT_PORT];

                }
            } else if (pAudioPortParams->nPortIndex ==
                       pAudDecComPvt->
                       pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->nPortIndex) {
                if (pAudioPortParams->nIndex >
                    pAudDecComPvt->tAudioParams[OMX_AUDDEC_OUTPUT_PORT].
                    nIndex) {
                    eError = OMX_ErrorNoMore;
                } else {
                    *(pAudioPortParams) =
                        pAudDecComPvt->tAudioParams[OMX_AUDDEC_OUTPUT_PORT];

                }
            } else {
                eError = OMX_ErrorBadPortIndex;
            }
        }
        break;

    case OMX_IndexParamAudioMp3:
        {
#ifdef CODEC_MP3DEC
            OMX_BASE_CHK_VERSION(pParamStruct, OMX_AUDIO_PARAM_MP3TYPE, eError);

            /* this info is static, known from decoder release notes
               no placeholder  for this in * XDM1.0 */
			((OMX_AUDIO_PARAM_MP3TYPE *) (pParamStruct))->nChannels = 
			  2;

			((OMX_AUDIO_PARAM_MP3TYPE *) (pParamStruct))->nSampleRate = 
			 (IAUDDEC1_Status *)((IMP3DEC_Status*)pAudDecComPvt->pDecStatus)
			   ->auddec_status.sampleRate;

			((OMX_AUDIO_PARAM_MP3TYPE *) (pParamStruct))->nBitRate = 
			 (IAUDDEC1_Status *)((IMP3DEC_Status*)pAudDecComPvt->pDecStatus)
			   ->auddec_status.bitRate;

			((OMX_AUDIO_PARAM_MP3TYPE *) (pParamStruct))->nAudioBandWidth = 
			   0;

            ((OMX_AUDIO_PARAM_MP3TYPE *) (pParamStruct))->eFormat =
                (((IMP3DEC_Status*)pAudDecComPvt->pDecStatus)->bsi[0]==0)?
                OMX_AUDIO_MP3StreamFormatMP2Layer3:
			    (((IMP3DEC_Status*)pAudDecComPvt->pDecStatus)->bsi[0]==1)?
				OMX_AUDIO_MP3StreamFormatMP1Layer3:
				OMX_AUDIO_MP3StreamFormatMP2_5Layer3;			

			((OMX_AUDIO_PARAM_MP3TYPE *) (pParamStruct))->eChannelMode = 				
			 ((IMP3DEC_Status*)pAudDecComPvt->pDecStatus)->bsi[4];
#endif
        }
        break;

        /* redirects the call to "OMX_BASE_GetParameter" which supports standard
           comp indexes */

    case OMX_IndexParamAudioAac:
        {

            OMX_BASE_CHK_VERSION(pParamStruct, OMX_AUDIO_PARAM_AACPROFILETYPE, eError);

            /* this info is static, known from decoder release notes
               no placeholder  for this in * XDM1.0 */
            ((OMX_AUDIO_PARAM_AACPROFILETYPE *) (pParamStruct))->nChannels = 
              2;

            ((OMX_AUDIO_PARAM_AACPROFILETYPE *) (pParamStruct))->nSampleRate = 
             (OMX_U32)((AUDDEC1_Status *)((IAACDEC_Status*)pAudDecComPvt->pDecStatus)
               ->auddec_status.sampleRate);

            ((OMX_AUDIO_PARAM_AACPROFILETYPE *) (pParamStruct))->nBitRate = 
             (OMX_U32)((AUDDEC1_Status *)((IAACDEC_Status*)pAudDecComPvt->pDecStatus)
               ->auddec_status.bitRate);

            ((OMX_AUDIO_PARAM_AACPROFILETYPE *) (pParamStruct))->nAudioBandWidth = 
               0;

            ((OMX_AUDIO_PARAM_AACPROFILETYPE *) (pParamStruct))->nFrameLength = 
             (OMX_U32)((AUDDEC1_Status *)((IAACDEC_Status*)pAudDecComPvt->pDecStatus)
               ->auddec_status.numSamples);

            ((OMX_AUDIO_PARAM_AACPROFILETYPE *) (pParamStruct))->nAACtools = 
             ((IAACDEC_Status*)pAudDecComPvt->pDecStatus)->toolsUsed;

            /* As per OMX, nAACERtools is optional as decoder info output */
            //((OMX_AUDIO_PARAM_AACPROFILETYPE *) (pParamStruct))->nAACERtools = 
            // 0; 

            ((OMX_AUDIO_PARAM_AACPROFILETYPE *) (pParamStruct))->eAACProfile = 
             (OMX_AUDIO_AACPROFILETYPE)((AUDDEC1_Status *)((IAACDEC_Status*)pAudDecComPvt->pDecStatus)
               ->auddec_status.channelMode);

            ((OMX_AUDIO_PARAM_AACPROFILETYPE *) (pParamStruct))->eAACStreamFormat =
             (((IAACDEC_DynamicParams*)pAudDecComPvt->pDecDynParams)->bRawFormat)?
                OMX_AUDIO_AACStreamFormatRAW:
                OMX_AUDIO_AACStreamFormatMP2ADTS;

            ((OMX_AUDIO_PARAM_AACPROFILETYPE *) (pParamStruct))->eChannelMode = 
                mapTiChannelModeToOmxChannelMode(
             (IAUDIO_ChannelMode)((AUDDEC1_Status *)((IAACDEC_Status*)pAudDecComPvt->pDecStatus)
               ->auddec_status.channelMode));
        }
        break;
    default:
        eError = OMX_BASE_GetParameter(hComponent, nParamIndex, pParamStruct);
        OMX_BASE_ASSERT(OMX_ErrorNone == eError, eError);
        break;
    }

  EXIT:
    return eError;
}

/* ========================================================================== */

/**
********************************************************************************
 *  @func   OMX_TI_AUDDEC_InitCallbacks()
 *  @brief  This method initialize the auddec components callback, which are
 *          specific to a Audio coded type.
 *
 *  @param [in] hComponent   : Component handle
 *  @param [in] compressionFormat  : compression format
 *
 *  @return
 *  OMX_ErrorNone = Successful
 *
 *  Other_value = Failed (Error code is returned)
 *
*/
/* ========================================================================== */

OMX_ERRORTYPE OMX_TI_AUDDEC_InitCallbacks(OMX_HANDLETYPE hComponent,
                                         OMX_U32 compressionFormat)
{
    int i;
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_COMPONENTTYPE *pHandle;
    OMX_AUDDEC_PVTTYPE *pAudDecComPvt = NULL;

    pHandle = (OMX_COMPONENTTYPE *) hComponent;
    pAudDecComPvt = (OMX_AUDDEC_PVTTYPE *) pHandle->pComponentPrivate;

    for (i = 0; i < OMX_AudDecCallbacksTableEntryCnt; i++) {
        if (OMX_AudDecCallbacksTable[i].compressionFormat == compressionFormat)
            break;
    }
    if (i == OMX_AudDecCallbacksTableEntryCnt) {
        return eError;
    }
    pAudDecComPvt->fpSet_StaticParams =
        OMX_AudDecCallbacksTable[i].fpSet_StaticParams;
    pAudDecComPvt->fpSet_DynamicParams =
        OMX_AudDecCallbacksTable[i].fpSet_DynamicParams;
    pAudDecComPvt->fpSet_Status = OMX_AudDecCallbacksTable[i].fpSet_Status;
    OMX_AudDecCallbacksTable[i].fpInit_Auddec(hComponent);

    return eError;
}

/* ========================================================================== */

/**
********************************************************************************
 *  @func   OMX_TI_AUDDEC_SetParameter()
 *   @brief  This method will send a parameter structure
 *          to a component. nParamIndex indicates which strucutre is passed to
 *          the component. Caller shall provide memory for the correct structure
 *          and shall fill in the nSize and nVersion fields before invoking this
 *          macro This method redirects the call to OMX_BASE_SetParameter which
 *          supports standard component indexes.
 *
 *          Note 1 :  Derived Comp owner's need to take care of the supported
 *                    domain specific indexes which can't be taken care in the
 *                    Base Component.
 *
 *          Note 2 :  This method can be invoked when the comp is in Loaded
 *                    State or on a port that is disabled.
 *
 *  @param [in] hComponent   : Component handle
 *  @param [in] nParamIndex  : Index of the structure to be filled
 *  @param [in] pParamStruct : Pointer to the IL client allocated structure that
 *                            the comp fills
 *
 *  @return
 *  OMX_ErrorNone = Successful
 *
 *  Other_value = Failed (Error code is returned)
 *
*/
/* ========================================================================== */
static OMX_ERRORTYPE OMX_TI_AUDDEC_SetParameter(OMX_HANDLETYPE hComponent,
                                                OMX_INDEXTYPE nParamIndex,
                                                OMX_PTR pParamStruct)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;

    OMX_U32 nPortIx;

    OMX_AUDDEC_PVTTYPE *pAudDecComPvt = NULL;

    OMX_COMPONENTTYPE *pHandle;

    OMX_AUDIO_PARAM_PORTFORMATTYPE *pAudioPortParams;

    OMX_PARAM_PORTDEFINITIONTYPE *pPortDefs = NULL;

    OMX_PARAM_COMPPORT_NOTIFYTYPE *pNotifyType = NULL;

    OMX_BASE_REQUIRE((hComponent != NULL)
                     && (pParamStruct != NULL), OMX_ErrorBadParameter);

    pHandle = (OMX_COMPONENTTYPE *) hComponent;
    pAudDecComPvt = (OMX_AUDDEC_PVTTYPE *) pHandle->pComponentPrivate;

    /* SetParameter can be invoked only when the comp is loaded or on disabled
       port */
    nPortIx = (( OMX_CONFIG_COMSTRUCT *)pParamStruct)->nPortIndex;

    OMX_BASE_ASSERT((pAudDecComPvt->tCurState == OMX_StateLoaded) ||
                    (pAudDecComPvt->pPortdefs[nPortIx]->
                     bEnabled == OMX_FALSE), OMX_ErrorIncorrectStateOperation);

    switch (nParamIndex) {

        /* Client uses this to modify the num of Audio ports  */

    case OMX_IndexParamPortDefinition:
        {

            pPortDefs = (OMX_PARAM_PORTDEFINITIONTYPE *) pParamStruct;

            OMX_BASE_CHK_VERSION(pParamStruct, OMX_PARAM_PORTDEFINITIONTYPE,
                                 eError);
            if (pPortDefs->nPortIndex ==
                pAudDecComPvt->pPortdefs[OMX_AUDDEC_INPUT_PORT]->nPortIndex) {

                if (pAudDecComPvt->pPortdefs[OMX_AUDDEC_INPUT_PORT]->
                    nBufferCountActual != pPortDefs->nBufferCountActual) {

                    pAudDecComPvt->bInBufferCountChange = OMX_TRUE;
                }

                pAudDecComPvt->pPortdefs[OMX_AUDDEC_INPUT_PORT]->
                    nBufferCountActual = pPortDefs->nBufferCountActual;

                pAudDecComPvt->pPortdefs[OMX_AUDDEC_INPUT_PORT]->nBufferSize = pPortDefs->nBufferSize;
                if(pPortDefs->format.audio.eEncoding !=
                    pAudDecComPvt->pPortdefs[OMX_AUDDEC_INPUT_PORT]->format.audio.eEncoding) {
                	memcpy(&pAudDecComPvt->pPortdefs[OMX_AUDDEC_INPUT_PORT]->format,
                			&pPortDefs->format, sizeof(OMX_AUDIO_PARAM_PORTFORMATTYPE));
                OMX_TI_AUDDEC_InitCallbacks(pHandle,
                                           pPortDefs->format.audio.
                                           eEncoding);
                }

            } else if (pPortDefs->nPortIndex ==
                       pAudDecComPvt->pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->
                       nPortIndex) {

                if (pAudDecComPvt->pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->
                    nBufferCountActual != pPortDefs->nBufferCountActual) {

                    pAudDecComPvt->bOutBufferCountChange = OMX_TRUE;
                }

                pAudDecComPvt->pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->
                    nBufferCountActual = pPortDefs->nBufferCountActual;

                pAudDecComPvt->pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->format =
                    pPortDefs->format;

                /* original values are overwritten, so store them back ; if need to
                   change, it is tested for change */

                pAudDecComPvt->pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->nBufferSize = pPortDefs->nBufferSize;

                pAudDecComPvt->pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->
                    nBufferCountMin = pPortDefs->nBufferCountMin;

                pAudDecComPvt->fpSet_StaticParams(hComponent,
                                                  pAudDecComPvt->pDecStaticParams);
            } else {
                eError = OMX_ErrorBadPortIndex;
            }
        }
        break;

        /* client uses this to modify the format type of an port */
    case OMX_IndexParamAudioInit:
        {
            OMX_BASE_CHK_VERSION(pParamStruct, OMX_PORT_PARAM_TYPE, eError);
            pAudDecComPvt->tDerToBase.pAudioPortParams->nPorts =
                ((OMX_PORT_PARAM_TYPE *) pParamStruct)->nPorts;
        }
        break;

    case OMX_IndexParamAudioPortFormat:
        {
            OMX_BASE_CHK_VERSION(pParamStruct, OMX_AUDIO_PARAM_PORTFORMATTYPE,
                                 eError);

            pAudioPortParams = (OMX_AUDIO_PARAM_PORTFORMATTYPE *) pParamStruct;
            /* todo set function pointers based on compression format */
            /* todo set static parameters */
            if (pAudioPortParams->nPortIndex ==
                pAudDecComPvt->tAudioParams[OMX_AUDDEC_INPUT_PORT].nPortIndex) {
                pAudDecComPvt->tAudioParams[OMX_AUDDEC_INPUT_PORT].
                    eEncoding = pAudioPortParams->eEncoding;

            } else if (pAudioPortParams->nPortIndex ==
                       pAudDecComPvt->tAudioParams[OMX_AUDDEC_OUTPUT_PORT].
                       nPortIndex) {
                pAudDecComPvt->tAudioParams[OMX_AUDDEC_OUTPUT_PORT].
                    eEncoding = pAudioPortParams->eEncoding;
            } else {
                eError = OMX_ErrorBadPortIndex;
            }
        }
        break;

    case OMX_TI_IndexParamCompPortNotifyType:
        {
            pNotifyType =
                (OMX_PARAM_COMPPORT_NOTIFYTYPE *) pParamStruct;
            pAudDecComPvt->tDerToBase.pPortProperties[pNotifyType->
                                                      nPortIndex]->eNotifyType =
                pNotifyType->eNotifyType;
            eError = OMX_ErrorNone;
            break;
        }

    case OMX_IndexParamAudioAac:
    {

        //OMX_BASE_CHK_VERSION(pParamStruct, OMX_AUDIO_PARAM_AACPROFILETYPE, eError);

        /* this info is static, known from decoder release notes
           no placeholder  for this in * XDM1.0 */
        if(((OMX_AUDIO_PARAM_AACPROFILETYPE *)pParamStruct)->eAACStreamFormat == 
            OMX_AUDIO_AACStreamFormatRAW){
             ((IAACDEC_DynamicParams*)pAudDecComPvt->pDecDynParams)->bRawFormat = 1;
             ((IAACDEC_DynamicParams*)pAudDecComPvt->pDecDynParams)->ulSamplingRateIdx = 
                 mapOmxSampleRateToTiAacDecSrIdx(((OMX_AUDIO_PARAM_AACPROFILETYPE *)pParamStruct)->nSampleRate);
        }
    }
    break;

        /* redirects the call to "OMX_BASE_SetParameter" which supports standard
           comp indexes */
    default:
        return OMX_BASE_SetParameter(hComponent, nParamIndex, pParamStruct);
    }

  EXIT:
    return eError;
}

/* ========================================================================== */

/**
********************************************************************************
 *  @func  OMX_TI_AUDDEC_SetConfig()
 *  @brief  This method will send one of the configurationsstrucutre to a
 *          component. The caller shall provide the memory for the correct
 *          structure and fill in the size and version to all other fields
 *          before invoking this macro.
 *          Note 1 :  Derived Comp owner's need to take care of the supported
 *          domain specific indexes which can't be taken care in the Base
 *          Component.
 *          Note 2 :  This method can be invoked when the comp is in any state
 *          except Invalid state.
 *
 * @param [in] hComponent  : Component handle
 * @param [in] nIndex      : Index of the strucutre to be filled.
 * @param [in] pConfigData : Pointer to the IL Client allocated structure that
 *                           the component uses for initialization.
 *
 *  @return
 *  OMX_ErrorNone = Successful
 *
 *  Other_value = Failed (Error code is returned)
 *
*/
/* ========================================================================== */
static OMX_ERRORTYPE OMX_TI_AUDDEC_SetConfig(OMX_HANDLETYPE hComponent,
                                             OMX_INDEXTYPE nIndex,
                                             OMX_PTR pConfigData)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;

    OMX_U32 nPortIx;

    OMX_AUDDEC_PVTTYPE *pAudDecComPvt = NULL;

    OMX_COMPONENTTYPE *pHandle;

    OMX_AUDIO_PARAM_PORTFORMATTYPE *pAudioPortParams;

    OMX_PARAM_PORTDEFINITIONTYPE *pPortDefs = NULL;

    OMX_PARAM_COMPPORT_NOTIFYTYPE *pNotifyType = NULL;

    OMX_BASE_REQUIRE((hComponent != NULL)
                     && (pConfigData != NULL), OMX_ErrorBadParameter);

    pHandle = (OMX_COMPONENTTYPE *) hComponent;
    pAudDecComPvt = (OMX_AUDDEC_PVTTYPE *) pHandle->pComponentPrivate;

    /* SetParameter can be invoked only when the comp is loaded or on disabled
       port */
    nPortIx = (( OMX_CONFIG_COMSTRUCT *)pConfigData)->nPortIndex;

    OMX_BASE_ASSERT((pAudDecComPvt->tCurState == OMX_StateLoaded) ||
                    (pAudDecComPvt->pPortdefs[nPortIx]->
                     bEnabled == OMX_FALSE), OMX_ErrorIncorrectStateOperation);

    switch (nIndex) {

        /* Client uses this to modify the num of Audio ports  */

    case OMX_IndexParamPortDefinition:
        {

            pPortDefs = (OMX_PARAM_PORTDEFINITIONTYPE *) pConfigData;

            OMX_BASE_CHK_VERSION(pConfigData, OMX_PARAM_PORTDEFINITIONTYPE,
                                 eError);
            if (pPortDefs->nPortIndex ==
                pAudDecComPvt->pPortdefs[OMX_AUDDEC_INPUT_PORT]->nPortIndex) {

                if (pAudDecComPvt->pPortdefs[OMX_AUDDEC_INPUT_PORT]->
                    nBufferCountActual != pPortDefs->nBufferCountActual) {

                    pAudDecComPvt->bInBufferCountChange = OMX_TRUE;
                }

                pAudDecComPvt->pPortdefs[OMX_AUDDEC_INPUT_PORT]->
                    nBufferCountActual = pPortDefs->nBufferCountActual;

                pAudDecComPvt->pPortdefs[OMX_AUDDEC_INPUT_PORT]->nBufferSize = pPortDefs->nBufferSize;
                if(pPortDefs->format.audio.eEncoding !=
                    pAudDecComPvt->pPortdefs[OMX_AUDDEC_INPUT_PORT]->format.audio.eEncoding) {
                	memcpy(&pAudDecComPvt->pPortdefs[OMX_AUDDEC_INPUT_PORT]->format,
                			&pPortDefs->format, sizeof(OMX_AUDIO_PARAM_PORTFORMATTYPE));
                OMX_TI_AUDDEC_InitCallbacks(pHandle,
                                           pPortDefs->format.audio.
                                           eEncoding);
                }

            } else if (pPortDefs->nPortIndex ==
                       pAudDecComPvt->pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->
                       nPortIndex) {

                if (pAudDecComPvt->pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->
                    nBufferCountActual != pPortDefs->nBufferCountActual) {

                    pAudDecComPvt->bOutBufferCountChange = OMX_TRUE;
                }

                pAudDecComPvt->pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->
                    nBufferCountActual = pPortDefs->nBufferCountActual;

                pAudDecComPvt->pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->format =
                    pPortDefs->format;

                /* original values are overwritten, so store them back ; if need to
                   change, it is tested for change */

                pAudDecComPvt->pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->nBufferSize = pPortDefs->nBufferSize;

                pAudDecComPvt->pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->
                    nBufferCountMin = pPortDefs->nBufferCountMin;

                pAudDecComPvt->fpSet_StaticParams(hComponent,
                                                  pAudDecComPvt->pDecStaticParams);
            } else {
                eError = OMX_ErrorBadPortIndex;
            }
        }
        break;

        /* client uses this to modify the format type of an port */
    case OMX_IndexParamAudioInit:
        {
            OMX_BASE_CHK_VERSION(pConfigData, OMX_PORT_PARAM_TYPE, eError);
            pAudDecComPvt->tDerToBase.pAudioPortParams->nPorts =
                ((OMX_PORT_PARAM_TYPE *) pConfigData)->nPorts;
        }
        break;

    case OMX_IndexParamAudioPortFormat:
        {
            OMX_BASE_CHK_VERSION(pConfigData, OMX_AUDIO_PARAM_PORTFORMATTYPE,
                                 eError);

            pAudioPortParams = (OMX_AUDIO_PARAM_PORTFORMATTYPE *) pConfigData;
            /* todo set function pointers based on compression format */
            /* todo set static parameters */
            if (pAudioPortParams->nPortIndex ==
                pAudDecComPvt->tAudioParams[OMX_AUDDEC_INPUT_PORT].nPortIndex) {
                pAudDecComPvt->tAudioParams[OMX_AUDDEC_INPUT_PORT].
                    eEncoding = pAudioPortParams->eEncoding;

            } else if (pAudioPortParams->nPortIndex ==
                       pAudDecComPvt->tAudioParams[OMX_AUDDEC_OUTPUT_PORT].
                       nPortIndex) {
                pAudDecComPvt->tAudioParams[OMX_AUDDEC_OUTPUT_PORT].
                    eEncoding = pAudioPortParams->eEncoding;
            } else {
                eError = OMX_ErrorBadPortIndex;
            }
        }
        break;

    case OMX_TI_IndexParamCompPortNotifyType:
        {
            pNotifyType =
                (OMX_PARAM_COMPPORT_NOTIFYTYPE *) pConfigData;
            pAudDecComPvt->tDerToBase.pPortProperties[pNotifyType->
                                                      nPortIndex]->eNotifyType =
                pNotifyType->eNotifyType;
            eError = OMX_ErrorNone;
            break;
        }

    case OMX_IndexParamAudioAac:
    {

        //OMX_BASE_CHK_VERSION(pConfigData, OMX_AUDIO_PARAM_AACPROFILETYPE, eError);

        /* this info is static, known from decoder release notes
           no placeholder  for this in * XDM1.0 */
        if(((OMX_AUDIO_PARAM_AACPROFILETYPE *)pConfigData)->eAACStreamFormat == 
            OMX_AUDIO_AACStreamFormatRAW){
             ((IAACDEC_DynamicParams*)pAudDecComPvt->pDecDynParams)->bRawFormat = 1;
             ((IAACDEC_DynamicParams*)pAudDecComPvt->pDecDynParams)->ulSamplingRateIdx = 
                 mapOmxSampleRateToTiAacDecSrIdx(((OMX_AUDIO_PARAM_AACPROFILETYPE *)pConfigData)->nSampleRate);
        }
    }
    break;

        /* redirects the call to "OMX_BASE_SetConfig" which supports standard
           comp indexes */
    default:
        return OMX_BASE_SetConfig(hComponent, nIndex, pConfigData);
    }

  EXIT:
    return eError;
}

/* ========================================================================== */

/**
********************************************************************************
 *  @func  OMX_TI_AUDDEC_GetConfig()
 *  @brief  This method will get one of the configurationsstrucutre to a
 *          component. The caller shall provide the memory for the correct
 *          structure and fill in the size and version to all other fields
 *          before invoking this macro.
 *          Note 1 :  Derived Comp owner's need to take care of the supported
 *          domain specific indexes which can't be taken care in the Base
 *          Component.
 *          Note 2 :  This method can be invoked when the comp is in any state
 *          except Invalid state.
 *
 * @param [in] hComponent  : Component handle
 * @param [in] nIndex      : Index of the strucutre to be filled.
 * @param [in] pConfigData : Pointer to the IL Client allocated structure that
 *                           the component uses for initialization.
 *
 *  @return
 *  OMX_ErrorNone = Successful
 *
 *  Other_value = Failed (Error code is returned)
 *
*/
/* ========================================================================== */

static OMX_ERRORTYPE OMX_TI_AUDDEC_GetConfig(OMX_HANDLETYPE hComponent,
                                             OMX_INDEXTYPE nIndex,
                                             OMX_PTR pConfigData)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;

    OMX_AUDDEC_PVTTYPE *pAudDecComPvt;

    OMX_COMPONENTTYPE *pHandle;

    OMX_AUDIO_PARAM_PORTFORMATTYPE *pAudioPortParams;

    OMX_BASE_REQUIRE((hComponent != NULL)
                     && (pConfigData != NULL), OMX_ErrorBadParameter);

    pHandle = (OMX_COMPONENTTYPE *) hComponent;
    pAudDecComPvt = (OMX_AUDDEC_PVTTYPE *) pHandle->pComponentPrivate;

    /* GetConfig can't be invoked incase the comp is in Invalid State */

    OMX_BASE_ASSERT(pAudDecComPvt->tCurState != OMX_StateInvalid,
                    OMX_ErrorIncorrectStateOperation);

    switch (nIndex) {

        /* Client uses this to retrieve the Info related to the params of audio
           ports */

    case OMX_IndexParamPortDefinition:
        {

            OMX_BASE_CHK_VERSION(pConfigData, OMX_PARAM_PORTDEFINITIONTYPE,
                                 eError);

            if (((OMX_PARAM_PORTDEFINITIONTYPE *) (pConfigData))->nPortIndex
                ==
                pAudDecComPvt->pPortdefs[OMX_AUDDEC_INPUT_PORT]->nPortIndex) {
                TIMM_OSAL_Memcpy(pConfigData,
                                 pAudDecComPvt->
                                 pPortdefs[OMX_AUDDEC_INPUT_PORT],
                                 sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
            } else if (((OMX_PARAM_PORTDEFINITIONTYPE *) (pConfigData))->
                       nPortIndex ==
                       pAudDecComPvt->
                       pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->nPortIndex) {
                TIMM_OSAL_Memcpy(pConfigData,
                                 pAudDecComPvt->
                                 pPortdefs[OMX_AUDDEC_OUTPUT_PORT],
                                 sizeof(OMX_PARAM_PORTDEFINITIONTYPE));


            } else {
                eError = OMX_ErrorBadPortIndex;
            }
        }
        break;

        /* client uses this to query the format supported by the port   */

    case OMX_IndexParamAudioPortFormat:
        {
            OMX_BASE_CHK_VERSION(pConfigData, OMX_AUDIO_PARAM_PORTFORMATTYPE,
                                 eError);

            pAudioPortParams = (OMX_AUDIO_PARAM_PORTFORMATTYPE *) pConfigData;

            if (pAudioPortParams->nPortIndex ==
                pAudDecComPvt->pPortdefs[OMX_AUDDEC_INPUT_PORT]->nPortIndex) {
                if (pAudioPortParams->nIndex >
                    pAudDecComPvt->tAudioParams[OMX_AUDDEC_INPUT_PORT].nIndex) {
                    eError = OMX_ErrorNoMore;
                } else {
                    *(pAudioPortParams) =
                        pAudDecComPvt->tAudioParams[OMX_AUDDEC_INPUT_PORT];

                }
            } else if (pAudioPortParams->nPortIndex ==
                       pAudDecComPvt->
                       pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->nPortIndex) {
                if (pAudioPortParams->nIndex >
                    pAudDecComPvt->tAudioParams[OMX_AUDDEC_OUTPUT_PORT].
                    nIndex) {
                    eError = OMX_ErrorNoMore;
                } else {
                    *(pAudioPortParams) =
                        pAudDecComPvt->tAudioParams[OMX_AUDDEC_OUTPUT_PORT];

                }
            } else {
                eError = OMX_ErrorBadPortIndex;
            }
        }
        break;

    case OMX_IndexParamAudioMp3:
        {
#ifdef CODEC_MP3DEC
            OMX_BASE_CHK_VERSION(pConfigData, OMX_AUDIO_PARAM_MP3TYPE, eError);

            /* this info is static, known from decoder release notes
               no placeholder  for this in * XDM1.0 */
			((OMX_AUDIO_PARAM_MP3TYPE *) (pConfigData))->nChannels = 
			  2;

			((OMX_AUDIO_PARAM_MP3TYPE *) (pConfigData))->nSampleRate = 
			 (IAUDDEC1_Status *)((IMP3DEC_Status*)pAudDecComPvt->pDecStatus)
			   ->auddec_status.sampleRate;

			((OMX_AUDIO_PARAM_MP3TYPE *) (pConfigData))->nBitRate = 
			 (IAUDDEC1_Status *)((IMP3DEC_Status*)pAudDecComPvt->pDecStatus)
			   ->auddec_status.bitRate;

			((OMX_AUDIO_PARAM_MP3TYPE *) (pConfigData))->nAudioBandWidth = 
			   0;
			
            ((OMX_AUDIO_PARAM_MP3TYPE *) (pConfigData))->eFormat =
                (((IMP3DEC_Status*)pAudDecComPvt->pDecStatus)->bsi[0]==0)?
                OMX_AUDIO_MP3StreamFormatMP2Layer3:
			    (((IMP3DEC_Status*)pAudDecComPvt->pDecStatus)->bsi[0]==1)?
				OMX_AUDIO_MP3StreamFormatMP1Layer3:
				OMX_AUDIO_MP3StreamFormatMP2_5Layer3;			

			((OMX_AUDIO_PARAM_MP3TYPE *) (pConfigData))->eChannelMode = 				
			 ((IMP3DEC_Status*)pAudDecComPvt->pDecStatus)->bsi[4];
#endif
        }
        break;

        /* redirects the call to "OMX_BASE_GetConfig" which supports standard
           comp indexes */

    case OMX_IndexParamAudioAac:
        {

            OMX_BASE_CHK_VERSION(pConfigData, OMX_AUDIO_PARAM_AACPROFILETYPE, eError);

            /* this info is static, known from decoder release notes
               no placeholder  for this in * XDM1.0 */
            ((OMX_AUDIO_PARAM_AACPROFILETYPE *) (pConfigData))->nChannels = 
              2;

            ((OMX_AUDIO_PARAM_AACPROFILETYPE *) (pConfigData))->nSampleRate = 
             (OMX_U32)(AUDDEC1_Status *)((IAACDEC_Status*)pAudDecComPvt->pDecStatus)
               ->auddec_status.sampleRate;

            ((OMX_AUDIO_PARAM_AACPROFILETYPE *) (pConfigData))->nBitRate = 
             (OMX_U32)(AUDDEC1_Status *)((IAACDEC_Status*)pAudDecComPvt->pDecStatus)
               ->auddec_status.bitRate;

            ((OMX_AUDIO_PARAM_AACPROFILETYPE *) (pConfigData))->nAudioBandWidth = 
               0;

            ((OMX_AUDIO_PARAM_AACPROFILETYPE *) (pConfigData))->nFrameLength = 
             (OMX_U32)(AUDDEC1_Status *)((IAACDEC_Status*)pAudDecComPvt->pDecStatus)
               ->auddec_status.numSamples;

            ((OMX_AUDIO_PARAM_AACPROFILETYPE *) (pConfigData))->nAACtools = 
             ((IAACDEC_Status*)pAudDecComPvt->pDecStatus)->toolsUsed;

            /* As per OMX, nAACERtools is optional as decoder info output */
            //((OMX_AUDIO_PARAM_AACPROFILETYPE *) (pConfigData))->nAACERtools = 
            // 0; 

            ((OMX_AUDIO_PARAM_AACPROFILETYPE *) (pConfigData))->eAACProfile = 
             (OMX_AUDIO_AACPROFILETYPE)(AUDDEC1_Status *)((IAACDEC_Status*)pAudDecComPvt->pDecStatus)
               ->auddec_status.channelMode;

            ((OMX_AUDIO_PARAM_AACPROFILETYPE *) (pConfigData))->eAACStreamFormat =
             (((IAACDEC_DynamicParams*)pAudDecComPvt->pDecDynParams)->bRawFormat)?
                OMX_AUDIO_AACStreamFormatRAW:
                OMX_AUDIO_AACStreamFormatMP2ADTS;

            ((OMX_AUDIO_PARAM_AACPROFILETYPE *) (pConfigData))->eChannelMode = 
                mapTiChannelModeToOmxChannelMode((IAUDIO_ChannelMode)
             (AUDDEC1_Status *)((IAACDEC_Status*)pAudDecComPvt->pDecStatus)
               ->auddec_status.channelMode);
        }
        break;
    default:
        OMX_BASE_GetConfig(hComponent, nIndex, pConfigData);
        OMX_BASE_ASSERT(OMX_ErrorNone == eError, eError);
        break;
    }

  EXIT:
    return eError;
}

/* ========================================================================== */

/**
********************************************************************************
 *  @func  OMX_TI_AUDDEC_CommandNotify()
 *  @brief This method is an Hook from the Base component to Notify commands
 *         which needs to be executed by the component. This method should
 *         complete all the typical allocation of resrouces or operations like
 *         SocketNode init/deinitdepending on the cmd requested. Once all the
 *         operations have been done notify the Base component via an
 *         ReturnEventNotify call.
 *
 *  @param [in] hComponent  : Component handle
 *  @param [in] Cmd         : Command for the component to execute
 *  @param [in] nParam      : Integer parameter for the command that is to be
 *                            executed
 *  @param [in] pCmdData    : Pointer that contains implementation specific data
 *                            that cannot be represented with the numeric
 *                            parameter nParam
 *
 *  @return
 *  OMX_ErrorNone = Successful
 *
 *  Other_value = Failed (Error code is returned)
 *
*/
/* ========================================================================== */
static OMX_ERRORTYPE OMX_TI_AUDDEC_CommandNotify(OMX_HANDLETYPE hComponent,
                                                 OMX_COMMANDTYPE Cmd,
                                                 OMX_U32 nParam,
                                                 OMX_PTR pCmdData)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;

    OMX_AUDDEC_PVTTYPE *pAudDecComPvt = NULL;

    OMX_COMPONENTTYPE *pHandle;

    OMX_BUFFERHEADERTYPE *pInBufHeader = NULL;

    XDAS_Int32 status = 0, i;

    OMX_BOOL bCodecCreate = OMX_FALSE;

    OMX_U32 nInMsgCount = 0, nBufferCountActual;

    //OMX_PTR codecPtr;
    OMX_U32 nIndex;

    OMX_BASE_REQUIRE((hComponent != NULL), OMX_ErrorBadParameter);

    pHandle = (OMX_COMPONENTTYPE *) hComponent;
    pAudDecComPvt = (OMX_AUDDEC_PVTTYPE *) pHandle->pComponentPrivate;

    pInBufHeader = (OMX_BUFFERHEADERTYPE *)
        TIMM_OSAL_Malloc(sizeof(OMX_BUFFERHEADERTYPE), TIMM_OSAL_TRUE, 0,
                         TIMMOSAL_MEM_SEGMENT_EXT);
        TIMM_OSAL_Memset(pInBufHeader, 0x0,
                         sizeof(OMX_BUFFERHEADERTYPE));

    if (pInBufHeader == TIMM_OSAL_NULL) {
        eError = OMX_ErrorInsufficientResources;
        goto EXIT;
    }

    /* allocation of any resources which are specific to the Component, Notify
       this  Asynchronous event completion to the Base Comp via
       ReturnEventNotify call */

    switch (Cmd) {

    case OMX_CommandStateSet:

        if ((pAudDecComPvt->tCurState == OMX_StateLoaded) &&
            (pAudDecComPvt->tNewState == OMX_StateIdle)) {

            /* allocate and initialize the decoder on the codec Engine,
               set the dynamic parameters  */

            if (pAudDecComPvt->decHandle == NULL) {
                pAudDecComPvt->decHandle =
                    AUDDEC1_create(pAudDecComPvt->ce, pAudDecComPvt->decoderName,
                                   NULL);
                if (pAudDecComPvt->decHandle == NULL) {
                    OMX_BASE_Error("AUDDEC1_create failed ....! \n");
                    eError = OMX_ErrorInsufficientResources;
                    goto EXIT;
                }
                pAudDecComPvt->bCodecCreate = OMX_FALSE;

                pAudDecComPvt->fpSet_DynamicParams(pAudDecComPvt->pDecDynParams);

                /* call control to get buffer info */
                status = AUDDEC1_control(pAudDecComPvt->decHandle,
                                         XDM_GETBUFINFO,
                                         (AUDDEC1_DynamicParams *)pAudDecComPvt->pDecDynParams,
                                         (AUDDEC1_Status *)
                                         pAudDecComPvt->pDecStatus);

                if (status != AUDDEC1_EOK) {
                    OMX_BASE_Error
                        ("AUDDEC1_control XDM_GETBUFINFO failed ....! \n");
                    eError = OMX_ErrorInsufficientResources;
                    goto EXIT;
                }

                for (i = 0; i < OMX_AUDDEC_MAX_INPUT_BUFFER_COUNT; i++) {
                    pAudDecComPvt->inputBufDesc[i].numBufs =
                        ((IAUDDEC1_Status *) (pAudDecComPvt->pDecStatus))->bufInfo.
                        minNumInBufs;
                }
                for (i = 0; i < OMX_AUDDEC_MAX_OUTPUT_BUFFER_COUNT; i++) {
                    pAudDecComPvt->outputBufDesc[i].numBufs =
                        ((IAUDDEC1_Status *) (pAudDecComPvt->pDecStatus))->bufInfo.
                        minNumOutBufs;
                }
            }

            if (pAudDecComPvt->pPortdefs[OMX_AUDDEC_INPUT_PORT]->bEnabled) {
            nBufferCountActual =
            pAudDecComPvt->pPortdefs[OMX_AUDDEC_INPUT_PORT]->nBufferCountActual;

            if(pAudDecComPvt->inBufHeaderArray == NULL)
             {
            pAudDecComPvt->inBufHeaderArray = (OMX_AUDDEC_buf_header *)
            TIMM_OSAL_Malloc((sizeof(OMX_AUDDEC_buf_header)
                              * nBufferCountActual), TIMM_OSAL_TRUE, 0,
                              TIMMOSAL_MEM_SEGMENT_EXT);
             }

            OMX_BASE_ASSERT(pAudDecComPvt->inBufHeaderArray != NULL,
                            OMX_ErrorInsufficientResources);

            TIMM_OSAL_Memset(pAudDecComPvt->inBufHeaderArray, 0x0,
                                 ((sizeof(OMX_AUDDEC_buf_header) *
                                   (nBufferCountActual))));

            for (i = 0; i < nBufferCountActual; i++) {
                pAudDecComPvt->inBufHeaderArray[i].pBufHeader = NULL;
                pAudDecComPvt->inBufHeaderArray[i].bufStatus =  OMX_BUFFER_FREE;
            }
            }

            if (pAudDecComPvt->pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->bEnabled) {
            nBufferCountActual =
            pAudDecComPvt->pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->nBufferCountActual;

            if(pAudDecComPvt->outBufHeaderArray == NULL)
             {
               pAudDecComPvt->outBufHeaderArray = (OMX_AUDDEC_buf_header *)
               TIMM_OSAL_Malloc((sizeof(OMX_AUDDEC_buf_header)
                              * nBufferCountActual), TIMM_OSAL_TRUE, 0,
                              TIMMOSAL_MEM_SEGMENT_EXT);
             }

            OMX_BASE_ASSERT(pAudDecComPvt->outBufHeaderArray != NULL,
                            OMX_ErrorInsufficientResources);

            TIMM_OSAL_Memset(pAudDecComPvt->outBufHeaderArray, 0x0,
                                 ((sizeof(OMX_AUDDEC_buf_header) *
                                   (nBufferCountActual))));

            for (i = 0; i < nBufferCountActual; i++) {
                pAudDecComPvt->outBufHeaderArray[i].pBufHeader = NULL;
                pAudDecComPvt->outBufHeaderArray[i].bufStatus =  OMX_BUFFER_FREE;
            }
            }


        }
        /* Incase if the comp is moving from idle to executing, process buffers
           if an supplier port; base would change state */

        else if ((pAudDecComPvt->tCurState == OMX_StateIdle) &&
                 (pAudDecComPvt->tNewState == OMX_StateExecuting)) {          
        }

        /* Incase If the comp is moving to Idle from executing, return all the
           buffers back to the allocator */

        else if (((pAudDecComPvt->tCurState == OMX_StateExecuting)
                  && (pAudDecComPvt->tNewState == OMX_StateIdle))
                 || ((pAudDecComPvt->tCurState == OMX_StatePause)
                     && (pAudDecComPvt->tNewState == OMX_StateIdle))) {

            /* Free all the input buffer */
            pAudDecComPvt->fpDioGetCount(hComponent, OMX_AUDDEC_INPUT_PORT,
                                         (OMX_PTR) & nInMsgCount);
            while (nInMsgCount > 0) {
                pAudDecComPvt->fpDioDequeue(hComponent, OMX_AUDDEC_INPUT_PORT,
                                            (OMX_PTR) (pInBufHeader));
                pAudDecComPvt->fpDioSend(hComponent, OMX_AUDDEC_INPUT_PORT,
                                         pInBufHeader);
                nInMsgCount--;
            }
        }

        /* State transition from pause to executing state; pauss is not
           supported */

        else if ((pAudDecComPvt->tCurState == OMX_StatePause) &&
                 (pAudDecComPvt->tNewState == OMX_StateExecuting)) {

        }

        else if ((pAudDecComPvt->tCurState == OMX_StateExecuting) &&
                 (pAudDecComPvt->tNewState == OMX_StatePause)) {
        }

        else if ((pAudDecComPvt->tCurState == OMX_StateIdle) &&
                 (pAudDecComPvt->tNewState == OMX_StateLoaded)) {

            /* Delete all the Resources allocated in Create Stage tear down the
               codec and Engine */

            AUDDEC1_delete(pAudDecComPvt->decHandle);
            pAudDecComPvt->decHandle = NULL;

            for (i = 0; i < OMX_AUDDEC_MAX_INPUT_BUFFER_COUNT; i++) {
                pAudDecComPvt->decInArgsPtr = &(pAudDecComPvt->decInArgs[i]);
                pAudDecComPvt->decInArgsPtr->auddec1InArgs.numBytes = 0;
            }
            for (i = 0; i < OMX_AUDDEC_MAX_OUTPUT_BUFFER_COUNT; i++) {
                pAudDecComPvt->decOutArgsPtr = &(pAudDecComPvt->decOutArgs[i]);
                pAudDecComPvt->decOutArgsPtr->auddec1OutArgs.bytesConsumed = 0;
            }

            /* free up memory allocated for buffer headers */
            for (nIndex = 0; nIndex <
                 pAudDecComPvt->pPortdefs[OMX_AUDDEC_INPUT_PORT]->nBufferCountActual;
                 nIndex++) {
                if(pAudDecComPvt->inBufHeaderArray[nIndex].pBufHeader != NULL)
                TIMM_OSAL_Free(pAudDecComPvt->inBufHeaderArray[nIndex].pBufHeader);
                pAudDecComPvt->inBufHeaderArray[nIndex].pBufHeader = NULL;
            }

            for (nIndex = 0; nIndex <
                 pAudDecComPvt->pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->nBufferCountActual;
                 nIndex++) {
                if(pAudDecComPvt->outBufHeaderArray[nIndex].pBufHeader != NULL)
                TIMM_OSAL_Free(pAudDecComPvt->outBufHeaderArray[nIndex].pBufHeader);
                pAudDecComPvt->outBufHeaderArray[nIndex].pBufHeader = NULL;
            }

            if(pAudDecComPvt->inBufHeaderArray != NULL) {
               TIMM_OSAL_Free(pAudDecComPvt->inBufHeaderArray);
               pAudDecComPvt->inBufHeaderArray = NULL;
            }

            if(pAudDecComPvt->outBufHeaderArray != NULL) {
              TIMM_OSAL_Free(pAudDecComPvt->outBufHeaderArray);
              pAudDecComPvt->outBufHeaderArray = NULL;
            }
        }
        /* If comp moves from states(Idle,Executing,Pause) to invalid state
           Delete all the Resources allocated in Create Stage, tear down the
           codec and Engine */

        else if ((pAudDecComPvt->tCurState == OMX_StateIdle
                  || pAudDecComPvt->tCurState == OMX_StateExecuting
                  || pAudDecComPvt->tCurState == OMX_StatePause)
                 && (pAudDecComPvt->tNewState == OMX_StateInvalid)) {
            pAudDecComPvt->tCurState = pAudDecComPvt->tNewState;
            AUDDEC1_delete(pAudDecComPvt->decHandle);
        }
        /* if same state movement, retun error */
        else if ( pAudDecComPvt->tCurState == pAudDecComPvt->tNewState) {
            eError = OMX_ErrorSameState;
            }
        break;

    case OMX_CommandPortDisable:

        if(pAudDecComPvt->tCurState != OMX_StateLoaded) {
        /* Make sure nParam corresponds to a valid port */
        OMX_BASE_ASSERT(((nParam == OMX_AUDDEC_OUTPUT_PORT)
                         || (nParam == OMX_AUDDEC_INPUT_PORT)
                         || (nParam == OMX_ALL)), OMX_ErrorBadParameter);

        /* for o/p port disable, only flag is set as false in base */
        if ((nParam == OMX_AUDDEC_INPUT_PORT) || (nParam == OMX_ALL)) {
        /* Free all the input buffer */
        pAudDecComPvt->fpDioGetCount(hComponent, OMX_AUDDEC_INPUT_PORT,
                                     (OMX_PTR) & nInMsgCount);
        while (nInMsgCount > 0) {
            pAudDecComPvt->fpDioDequeue(hComponent, OMX_AUDDEC_INPUT_PORT,
                                        (OMX_PTR) (pInBufHeader));
            pAudDecComPvt->fpDioSend(hComponent, OMX_AUDDEC_INPUT_PORT,
                                     pInBufHeader);
            nInMsgCount--;
        }
        }

        if(pAudDecComPvt->tCurState == OMX_StateExecuting) {
        /* Set the command id as XDM_RESET, as this would be done in process
           thread context */
            pAudDecComPvt->cmdXdm = XDM_RESET;

            goto EXIT;
        }
        }

        break;

    case OMX_CommandPortEnable:
        /* allocate and initialize the decoder on the Engine; If After port
           disable, format is changed it should have been deleted; and this
           command would result in codec creation, setting dynamic parameters,
           and getting the buffer information from codec  */
        /* base is taking care of allocating all the resources */
        /* nothing needs to be done here actually              */
        OMX_BASE_ASSERT(((nParam == OMX_AUDDEC_OUTPUT_PORT)
                         || (nParam == OMX_AUDDEC_INPUT_PORT)
                         || (nParam == OMX_ALL)), OMX_ErrorBadParameter);
        /* OMX_SetParameter sets bCodecCreate to signal change in Create time
           parameters. Therefore, the codec needs to be deleted and re-created
           here as part of PortEnable (also done in Loaded to Idle)
        */
        TIMM_OSAL_MutexObtain(pAudDecComPvt->pMutex, TIMM_OSAL_SUSPEND);
        bCodecCreate = pAudDecComPvt->bCodecCreate;
        TIMM_OSAL_MutexRelease(pAudDecComPvt->pMutex);

        if ((bCodecCreate == OMX_TRUE) && (pAudDecComPvt->tCurState != OMX_StateLoaded)) {
           /* free up memory allocated for buffer headers */
           if (pAudDecComPvt->decHandle) {
               AUDDEC1_delete(pAudDecComPvt->decHandle);
               pAudDecComPvt->decHandle = NULL;
           }
                /* create new codec instance */
                pAudDecComPvt->decHandle =
                    AUDDEC1_create(pAudDecComPvt->ce, pAudDecComPvt->decoderName,
                                   pAudDecComPvt->pDecStaticParams);
                if (pAudDecComPvt->decHandle == NULL) {
                    OMX_BASE_Error("AUDDEC1_create failed ....! \n");
                    eError = OMX_ErrorInsufficientResources;
                    goto EXIT;
                }
                pAudDecComPvt->fpSet_DynamicParams(pAudDecComPvt->pDecDynParams);

                status = AUDDEC1_control(pAudDecComPvt->decHandle,
                                         XDM_GETBUFINFO,
                                         pAudDecComPvt->pDecDynParams,
                                         (AUDDEC1_Status *)
                                         pAudDecComPvt->pDecStatus);

                if (status != AUDDEC1_EOK) {
                    OMX_BASE_Error
                        ("AUDDEC1_control XDM_GETBUFINFO failed ....! \n");
                    eError = OMX_ErrorInsufficientResources;
                    goto EXIT;
                }

                for (i = 0; i < OMX_AUDDEC_MAX_INPUT_BUFFER_COUNT; i++) {
                    pAudDecComPvt->inputBufDesc[i].numBufs =
                        ((IAUDDEC1_Status *) (pAudDecComPvt->pDecStatus))->bufInfo.
                        minNumInBufs;
                }
                for (i = 0; i < OMX_AUDDEC_MAX_OUTPUT_BUFFER_COUNT; i++) {
                    pAudDecComPvt->outputBufDesc[i].numBufs =
                        ((IAUDDEC1_Status *) (pAudDecComPvt->pDecStatus))->bufInfo.
                        minNumOutBufs;
                }

                /* Reset the Codeccreate flag & variable */
           pAudDecComPvt->bCodecCreate = OMX_FALSE;
        } /* end if bCodecCreate & not Loaded */

           /* If ports were enabled, it would have been created in loaded to idle
              transition. If not nBufferCountactual can change while port disabled */

            if(pAudDecComPvt->bInBufferCountChange == OMX_TRUE) {
                if (pAudDecComPvt->inBufHeaderArray != NULL) {
                    TIMM_OSAL_Free(pAudDecComPvt->inBufHeaderArray);
                    pAudDecComPvt->inBufHeaderArray = NULL;
                }
             }
            if(pAudDecComPvt->bOutBufferCountChange == OMX_TRUE) {
                if (pAudDecComPvt->outBufHeaderArray != NULL) {
                    TIMM_OSAL_Free(pAudDecComPvt->outBufHeaderArray);
                    pAudDecComPvt->outBufHeaderArray = NULL;
                }
             }

            if (pAudDecComPvt->inBufHeaderArray == NULL) {
                nBufferCountActual =
                pAudDecComPvt->pPortdefs[OMX_AUDDEC_INPUT_PORT]->nBufferCountActual;

                pAudDecComPvt->inBufHeaderArray = (OMX_AUDDEC_buf_header *)
                TIMM_OSAL_Malloc((sizeof(OMX_AUDDEC_buf_header)
                                  * nBufferCountActual), TIMM_OSAL_TRUE, 0,
                                  TIMMOSAL_MEM_SEGMENT_EXT);

                OMX_BASE_ASSERT(pAudDecComPvt->inBufHeaderArray != NULL,
                                OMX_ErrorInsufficientResources);

                TIMM_OSAL_Memset(pAudDecComPvt->inBufHeaderArray, 0x0,
                                     ((sizeof(OMX_AUDDEC_buf_header) *
                                       (nBufferCountActual))));

                for (i = 0; i < nBufferCountActual; i++) {
                    pAudDecComPvt->inBufHeaderArray[i].pBufHeader = NULL;
                    pAudDecComPvt->inBufHeaderArray[i].bufStatus =  OMX_BUFFER_FREE;
                }
            }
            if (pAudDecComPvt->outBufHeaderArray == NULL) {
                nBufferCountActual =
                pAudDecComPvt->pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->nBufferCountActual;

                pAudDecComPvt->outBufHeaderArray = (OMX_AUDDEC_buf_header *)
                TIMM_OSAL_Malloc((sizeof(OMX_AUDDEC_buf_header)
                                  * nBufferCountActual), TIMM_OSAL_TRUE, 0,
                                  TIMMOSAL_MEM_SEGMENT_EXT);

                OMX_BASE_ASSERT(pAudDecComPvt->outBufHeaderArray != NULL,
                                OMX_ErrorInsufficientResources);

                TIMM_OSAL_Memset(pAudDecComPvt->outBufHeaderArray, 0x0,
                                     ((sizeof(OMX_AUDDEC_buf_header) *
                                       (nBufferCountActual))));

                for (i = 0; i < nBufferCountActual; i++) {
                    pAudDecComPvt->outBufHeaderArray[i].pBufHeader = NULL;
                    pAudDecComPvt->outBufHeaderArray[i].bufStatus =  OMX_BUFFER_FREE;
                }
            }
            /* clear inargs and outArgs arrays */
            for (i = 0; i < OMX_AUDDEC_MAX_INPUT_BUFFER_COUNT; i++) {
                pAudDecComPvt->decInArgsPtr = &(pAudDecComPvt->decInArgs[i]);
                TIMM_OSAL_Memset(pAudDecComPvt->decInArgsPtr, 0x0,
                                     ((sizeof(AUDDEC1_InArgs)
                                       )));
                pAudDecComPvt->decInArgsPtr->auddec1InArgs.size =
                    sizeof(IAUDDEC1_InArgs);

            }
            for (i = 0; i < OMX_AUDDEC_MAX_OUTPUT_BUFFER_COUNT; i++) {
                pAudDecComPvt->decOutArgsPtr = &(pAudDecComPvt->decOutArgs[i]);
                TIMM_OSAL_Memset(pAudDecComPvt->decOutArgsPtr, 0x0,
                                     ((sizeof(AUDDEC1_OutArgs)
                                       )));
                pAudDecComPvt->decOutArgsPtr->auddec1OutArgs.size =
                    sizeof(IAUDDEC1_OutArgs);
            }
        break;

    case OMX_CommandFlush:
             /* omx flush (not codec flush), is similar to codec reset case;
                Free all the input buffer, set status as reset in server */

            pAudDecComPvt->fpDioGetCount(hComponent, OMX_AUDDEC_INPUT_PORT,
                                         (OMX_PTR) & nInMsgCount);
            while (nInMsgCount > 0) {
                pAudDecComPvt->fpDioDequeue(hComponent, OMX_AUDDEC_INPUT_PORT,
                                            (OMX_PTR) (pInBufHeader));
                pAudDecComPvt->fpDioSend(hComponent, OMX_AUDDEC_INPUT_PORT,
                                         pInBufHeader);
                nInMsgCount--;
            }
            /* Set the command id as XDM_RESET, as this would be done in server
               thread context */

            pAudDecComPvt->cmdXdm = XDM_RESET;

            /* command complete will be posted to base after reset is executed */
            goto EXIT;

           /* EXIT above will break it, avoiding warning */
           /* break; */

    default :
        break;
    }

    /* Notify this completion to the Base comp via ReturnEventNotify call */
    if ( eError == OMX_ErrorNone) {
    eError =
        pAudDecComPvt->fpReturnEventNotify(hComponent, OMX_EventCmdComplete,
                                           Cmd, nParam, NULL);
    OMX_BASE_ASSERT(eError == OMX_ErrorNone, eError);
    }

  EXIT:
    if (pInBufHeader) {
        TIMM_OSAL_Free(pInBufHeader);
        pInBufHeader = TIMM_OSAL_NULL;
    }
    return eError;
}

/**
********************************************************************************
 *  @func   OMX_TI_AUDDEC_DataNotify()
 *  @brief  This method is an hook from the Base Component to notify the Data
 *          either input or output buffers which need processed by the Derived
 *          Component. once the buffer is processed notify to the Base Component
 *          via ReturnDataNotify call to communicate with the IL client or
 *          tunneled component.
 *
 *  @param [in] hComponent  : Component handle
 *
 *  @return
 *  OMX_ErrorNone = Successful
 *
 *  Other_value = Failed (Error code is returned)
 *
*/
/* ========================================================================== */
static OMX_ERRORTYPE OMX_TI_AUDDEC_DataNotify(OMX_HANDLETYPE hComponent)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;

    OMX_AUDDEC_PVTTYPE *pAudDecComPvt;

    OMX_COMPONENTTYPE *pComp;

    OMX_BUFFERHEADERTYPE *pInBufHeader = NULL;

    OMX_BUFFERHEADERTYPE *pOutBufHeader = NULL;

    OMX_U16 inEOSReceived = 0;

    Int32 status;

    OMX_U32 nInMsgCount = 0, nBufferCountActual;

    OMX_U32 nOutMsgCount = 0;

    OMX_U32 bufStatus;

    OMX_STATETYPE tCurState;

    OMX_BASE_REQUIRE((hComponent != NULL), OMX_ErrorBadParameter);

    pComp = (OMX_COMPONENTTYPE *) hComponent;
    pAudDecComPvt = (OMX_AUDDEC_PVTTYPE *) pComp->pComponentPrivate;

    /* Start buffer processing only when the comp is in Executing state and the
       Port are Enabled */
    tCurState = pAudDecComPvt->tCurState;

    /* Start buffer processing only when the comp is in Executing state */
    if ((tCurState == OMX_StateExecuting)
        && (pAudDecComPvt->pPortdefs[OMX_AUDDEC_INPUT_PORT]->bEnabled)
        && (pAudDecComPvt->pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->bEnabled)) {

        /* check for both input and output */
        pAudDecComPvt->fpDioGetCount(hComponent, OMX_AUDDEC_INPUT_PORT,
                                     (OMX_PTR) & nInMsgCount);
        nOutMsgCount = 0;
        /* Check the output buffers only if input buffers are available */
        if ( nInMsgCount ) {
          pAudDecComPvt->fpDioGetCount(hComponent, OMX_AUDDEC_OUTPUT_PORT,
                                       (OMX_PTR) & nOutMsgCount);
        }
        /* if both are ready then only  process data */
        if ((nInMsgCount > 0) && (nOutMsgCount > 0)) {
            /* allocate space for buffer headers if it is not allocated already, It will
               be done only once and no run time allocation would be done post this  */

            pInBufHeader =
                pAudDecComPvt->inBufHeaderArray[pAudDecComPvt->inBufArrayIndx].pBufHeader;

            if (pInBufHeader == NULL) {
                pInBufHeader = (OMX_BUFFERHEADERTYPE *)
                    TIMM_OSAL_Malloc(sizeof(OMX_BUFFERHEADERTYPE), TIMM_OSAL_TRUE, 0,
                                     TIMMOSAL_MEM_SEGMENT_EXT);
                    TIMM_OSAL_Memset(pInBufHeader, 0x0,
                                     sizeof(OMX_BUFFERHEADERTYPE));

                if (pInBufHeader == TIMM_OSAL_NULL) {
                    eError = OMX_ErrorInsufficientResources;
                    goto EXIT;
                }
                pAudDecComPvt->inBufHeaderArray[pAudDecComPvt->inBufArrayIndx].
                    pBufHeader = pInBufHeader;
            } //if (pInBufHeader == NULL)

            nBufferCountActual =
            pAudDecComPvt->pPortdefs[OMX_AUDDEC_INPUT_PORT]->nBufferCountActual;

            pAudDecComPvt->inBufArrayIndx++;

            if (pAudDecComPvt->inBufArrayIndx >= nBufferCountActual)
             pAudDecComPvt->inBufArrayIndx = 0;

            pOutBufHeader =
                pAudDecComPvt->outBufHeaderArray[pAudDecComPvt->outBufArrayIndx].
                pBufHeader;

            if (pOutBufHeader == NULL) {
                pOutBufHeader = (OMX_BUFFERHEADERTYPE *)
                    TIMM_OSAL_Malloc(sizeof(OMX_BUFFERHEADERTYPE),
                                     TIMM_OSAL_TRUE, 0, TIMMOSAL_MEM_SEGMENT_EXT);
                    TIMM_OSAL_Memset(pOutBufHeader, 0x0,
                                     sizeof(OMX_BUFFERHEADERTYPE));

                if (pOutBufHeader == TIMM_OSAL_NULL) {
                    eError = OMX_ErrorInsufficientResources;
                    goto EXIT;
                }
                pAudDecComPvt->outBufHeaderArray[pAudDecComPvt->outBufArrayIndx].
                    pBufHeader = pOutBufHeader;
            } //if (pOutBufHeader == NULL)

            nBufferCountActual =
            pAudDecComPvt->pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->nBufferCountActual;

            pAudDecComPvt->outBufArrayIndx++;

            if (pAudDecComPvt->outBufArrayIndx >= nBufferCountActual)
             pAudDecComPvt->outBufArrayIndx = 0;
            pAudDecComPvt->fpDioDequeue(hComponent, OMX_AUDDEC_INPUT_PORT,
                                        (OMX_PTR) (pInBufHeader));

            pAudDecComPvt->fpDioDequeue(hComponent, OMX_AUDDEC_OUTPUT_PORT,
                                        (OMX_PTR) (pOutBufHeader));

            if ((pInBufHeader->nFlags & OMX_BUFFERFLAG_EOS) != OMX_BUFFERFLAG_EOS){
               if(pAudDecComPvt->bEOSProcessOn == OMX_TRUE)
                 pAudDecComPvt->bEOSProcessOn = OMX_FALSE;
             }

            if(pAudDecComPvt->bEOSProcessOn == OMX_TRUE) {
               pAudDecComPvt->fpDioSend(hComponent, OMX_AUDDEC_INPUT_PORT,
                                               pInBufHeader);
               pAudDecComPvt->fpDioCancel(hComponent,
                                               OMX_AUDDEC_OUTPUT_PORT,
                                               pOutBufHeader);
               OMX_TI_AUDDEC_UTL_SetBufStatus(&(pAudDecComPvt->outBufHeaderArray[0]),
                                   pAudDecComPvt->pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->nBufferCountActual,
                                   pOutBufHeader, OMX_BUFFER_FREE);

               OMX_TI_AUDDEC_UTL_SetBufStatus(&(pAudDecComPvt->inBufHeaderArray[0]),
                                   pAudDecComPvt->pPortdefs[OMX_AUDDEC_INPUT_PORT]->nBufferCountActual,
                                   pInBufHeader, OMX_BUFFER_FREE);
               goto EXIT;
            } //if(pAudDecComPvt->bEOSProcessOn == OMX_TRUE)

            if ((pInBufHeader->nFlags & OMX_BUFFERFLAG_EOS) == OMX_BUFFERFLAG_EOS) {
                /* this flag will be used if filled length is non zero */
                inEOSReceived = 1;

                pOutBufHeader->nFlags = OMX_BUFFERFLAG_EOS;

                if (pInBufHeader->nFilledLen == 0) {
                    /* as there is no input data, Give back the last input and
                       output dequeued buffer; In case there is data it will be
                       treated as regular buffer submission to scheduler, and
                       flag in frame set to flush */
                    pAudDecComPvt->fpDioSend(hComponent, OMX_AUDDEC_INPUT_PORT,
                                             pInBufHeader);
                    pOutBufHeader->nFilledLen = 0;
                    pAudDecComPvt->fpDioCancel(hComponent,
                                               OMX_AUDDEC_OUTPUT_PORT,
                                               pOutBufHeader);
                    /* component keeps track of all the buffes, whether it is
                       locked by component or dequeued but not used */
                    OMX_TI_AUDDEC_UTL_SetBufStatus(&(pAudDecComPvt->outBufHeaderArray[0]),
                                        pAudDecComPvt->pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->nBufferCountActual,
                                        pOutBufHeader, OMX_BUFFER_FREE);

                    OMX_TI_AUDDEC_UTL_SetBufStatus(&(pAudDecComPvt->inBufHeaderArray[0]),
                                        pAudDecComPvt->pPortdefs[OMX_AUDDEC_INPUT_PORT]->nBufferCountActual,
                                        pInBufHeader, OMX_BUFFER_FREE);
                    if(pAudDecComPvt->bEOSProcessOn == OMX_FALSE) {
                       /* set the element status in scheduler as flush, as there is
                       no data available no need to submit any frames to
                       scheduler, just set the status */
                       pAudDecComPvt->cmdXdm = XDM_FLUSH;
                    }
                    pAudDecComPvt->bEOSProcessOn = OMX_TRUE;
                    goto EXIT;
                } //if (pInBufHeader->nFilledLen == 0)
            } //if ((pInBufHeader->nFlags & OMX_BUFFERFLAG_EOS) == OMX_BUFFERFLAG_EOS)

            bufStatus = OMX_TI_AUDDEC_UTL_GetBufStatus(&(pAudDecComPvt->outBufHeaderArray[0]),
                                pAudDecComPvt->pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->nBufferCountActual,
                                pOutBufHeader);

            if(bufStatus != OMX_BUFFER_FREE) {
              printf(" Decoder o/p buffer was not free ");
              //Handle this: TBD
			}
            OMX_TI_AUDDEC_UTL_SetBufStatus(&(pAudDecComPvt->outBufHeaderArray[0]),
                                pAudDecComPvt->pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->nBufferCountActual,
                                pOutBufHeader, OMX_BUFFER_DQ);

            OMX_TI_AUDDEC_UTL_SetBufStatus(&(pAudDecComPvt->inBufHeaderArray[0]),
                                pAudDecComPvt->pPortdefs[OMX_AUDDEC_INPUT_PORT]->nBufferCountActual,
                                pInBufHeader, OMX_BUFFER_DQ);

            pAudDecComPvt->decInArgsPtr =
                &(pAudDecComPvt->decInArgs[pAudDecComPvt->decInArgsIdx]);

            pAudDecComPvt->decOutArgsPtr =
                &(pAudDecComPvt->decOutArgs[pAudDecComPvt->decOutArgsIdx]);

            pAudDecComPvt->inputBufDescPtr =
                &(pAudDecComPvt->inputBufDesc[pAudDecComPvt->decInArgsIdx]);

            pAudDecComPvt->outputBufDescPtr =
                &(pAudDecComPvt->outputBufDesc[pAudDecComPvt->decOutArgsIdx]);

            pAudDecComPvt->decInArgsPtr->auddec1InArgs.numBytes =
                pInBufHeader->nFilledLen;
            pAudDecComPvt->decInArgsPtr->auddec1InArgs.desiredChannelMode = 1;

            /* to be used if supplier changes the offset */
            pAudDecComPvt->inputBufDescPtr->descs[0].buf =
                (XDAS_Int8 *) ((XDAS_Int32) pInBufHeader->pBuffer +
                               pInBufHeader->nOffset);             
            pAudDecComPvt->inputBufDescPtr->descs[0].bufSize = pInBufHeader->nAllocLen;
			pAudDecComPvt->outputBufDescPtr->descs[0].buf =
                 (XDAS_Int8 *) ((XDAS_Int32) pOutBufHeader->pBuffer + 
                               pOutBufHeader->nOffset);
            pAudDecComPvt->outputBufDescPtr->descs[0].bufSize = pOutBufHeader->nAllocLen;
            /* control can be called directly, if it is not in the processing state */

            if (pAudDecComPvt->isfirstFrame) {
               status = AUDDEC1_control(
                     pAudDecComPvt->decHandle,
                     XDM_SETPARAMS,
                     (AUDDEC1_DynamicParams *) pAudDecComPvt->pDecDynParams,
                     (AUDDEC1_Status *) pAudDecComPvt->pDecStatus);
                pAudDecComPvt->isfirstFrame = 0;
                if (status != AUDDEC1_EOK) {
                    OMX_BASE_Error
                        ("AUDDEC1_control XDM_SETPARAMS failed ....! \n");
                    eError = OMX_ErrorInsufficientResources;
                    goto EXIT;
                }
            }
            pAudDecComPvt->decInArgsIdx++;
            if (pAudDecComPvt->decInArgsIdx >=
                OMX_AUDDEC_MAX_INPUT_BUFFER_COUNT)
                pAudDecComPvt->decInArgsIdx = 0;

            pAudDecComPvt->decOutArgsIdx++;
            if (pAudDecComPvt->decOutArgsIdx >=
                OMX_AUDDEC_MAX_OUTPUT_BUFFER_COUNT)
                pAudDecComPvt->decOutArgsIdx = 0;
        		
       
            /*  Propagate the Time Stamps from input to Corresponding Output */
            TIMM_OSAL_TraceExt(pAudDecComPvt->nTraceGroup,
                               "Propagate the timestamp: %d", pInBufHeader->nTimeStamp);
            pOutBufHeader->nTimeStamp = pInBufHeader->nTimeStamp;
            status = AUDDEC1_process(pAudDecComPvt->decHandle, (XDM1_BufDesc *)
                                     pAudDecComPvt->inputBufDescPtr,
                                     (XDM1_BufDesc *)
                                     pAudDecComPvt->outputBufDescPtr,
                                     (AUDDEC1_InArgs *)
                                     pAudDecComPvt->decInArgsPtr,
                                     (AUDDEC1_OutArgs *)
                                     pAudDecComPvt->decOutArgsPtr);

            /* Increment and keep track of the total frames decoded */
            pAudDecComPvt->frameCounter++;
			/* Call GetStatus for frame procesed */
			status = AUDDEC1_control(
                     pAudDecComPvt->decHandle,
                     XDM_GETSTATUS,
                     (AUDDEC1_DynamicParams *) pAudDecComPvt->pDecDynParams,
                     (AUDDEC1_Status *) pAudDecComPvt->pDecStatus);
                
                if (status != AUDDEC1_EOK) {
                    OMX_BASE_Error
                        ("AUDDEC1_control XDM_SETPARAMS failed ....! \n");
                    eError = OMX_ErrorInsufficientResources;
                    goto EXIT;
                }
            //MEMUTILS_cacheWb(pOutBufHeader->pBuffer,pOutBufHeader->nAllocLen);
            if (status == XDM_EFAIL) {
				eError = OMX_ErrorStreamCorrupt;
#ifdef CODEC_MP3DEC
                if (pAudDecComPvt->tAudioParams[OMX_AUDDEC_INPUT_PORT].
                  eEncoding == OMX_AUDIO_CodingMP3){
				OMX_TI_MP3AD_CB_ProcessDone(hComponent);
                }
#endif
                if (pAudDecComPvt->tAudioParams[OMX_AUDDEC_INPUT_PORT].
                  eEncoding == OMX_AUDIO_CodingAAC){
			        OMX_TI_AACAD_CB_ProcessDone(hComponent);
                }
                OMX_BASE_Error("\n Process function returned an Error...  ");
                // break; /* Error Condition: Application may want to break off */
            }else {            	
#ifdef CODEC_MP3DEC
              if (pAudDecComPvt->tAudioParams[OMX_AUDDEC_INPUT_PORT].
                 eEncoding == OMX_AUDIO_CodingMP3){
            	OMX_TI_MP3AD_CB_ProcessDone(hComponent);
              }
#endif
              if (pAudDecComPvt->tAudioParams[OMX_AUDDEC_INPUT_PORT].
                eEncoding == OMX_AUDIO_CodingAAC){
			    OMX_TI_AACAD_CB_ProcessDone(hComponent);
              }
			}        
        } //if ((nInMsgCount > 0) && (nOutMsgCount > 0))
	} //(tCurState == OMX_StateExecuting)

EXIT:
    return eError;
}


/*============================================================================*/

/**
********************************************************************************
 *  @func   OMX_TI_AUDDEC_setDefault()
 *  @brief  This method is used to initalize the port definitions structure and
 *          set them to default values for MP3 Decoder
 *
 *  @param pAudDecComPvt  : Pointer to the component private data structure
 *
 *  @return
 *  OMX_ErrorNone = Successful
 *
 *  Other_value = Failed (Error code is returned)
 *
 *
 * ============================================================================
 */

static OMX_ERRORTYPE OMX_TI_AUDDEC_SetDefault(OMX_AUDDEC_PVTTYPE *
                                              pAudDecComPvt,
                                              OMX_AUDIO_CODINGTYPE
                                              compressionFormat)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_U32 i;
    Engine_Error errorCode;

    /* Override the default values set to an Input Port with Actual/Default
       values */

    pAudDecComPvt->pPortdefs[OMX_AUDDEC_INPUT_PORT]->bEnabled = OMX_FALSE;
    pAudDecComPvt->pPortdefs[OMX_AUDDEC_INPUT_PORT]->eDir = OMX_DirInput;
    pAudDecComPvt->pPortdefs[OMX_AUDDEC_INPUT_PORT]->nPortIndex =
        OMX_AUDDEC_INPUT_PORT;
    pAudDecComPvt->pPortdefs[OMX_AUDDEC_INPUT_PORT]->nBufferCountMin =
        OMX_AUDDEC_DEFAULT_INPUT_BUFFER_COUNT;
    pAudDecComPvt->pPortdefs[OMX_AUDDEC_INPUT_PORT]->nBufferCountActual =
        OMX_AUDDEC_DEFAULT_INPUT_BUFFER_COUNT;
    pAudDecComPvt->pPortdefs[OMX_AUDDEC_INPUT_PORT]->nBufferSize =
        OMX_AUDDEC_DEFAULT_INPUT_BUFFER_SIZE;
    pAudDecComPvt->pPortdefs[OMX_AUDDEC_INPUT_PORT]->eDomain =
        OMX_PortDomainAudio;
    pAudDecComPvt->pPortdefs[OMX_AUDDEC_INPUT_PORT]->format.audio.cMIMEType =
        "ADEC";
    pAudDecComPvt->pPortdefs[OMX_AUDDEC_INPUT_PORT]->format.
        audio.pNativeRender = OMX_FALSE;
    pAudDecComPvt->pPortdefs[OMX_AUDDEC_INPUT_PORT]->format.
        audio.bFlagErrorConcealment = OMX_FALSE;
    pAudDecComPvt->pPortdefs[OMX_AUDDEC_INPUT_PORT]->format.audio.
        eEncoding = compressionFormat;
    pAudDecComPvt->pPortdefs[OMX_AUDDEC_INPUT_PORT]->format.
        audio.pNativeRender = OMX_FALSE;
    pAudDecComPvt->pPortdefs[OMX_AUDDEC_INPUT_PORT]->bBuffersContiguous =
        OMX_FALSE;
    pAudDecComPvt->pPortdefs[OMX_AUDDEC_INPUT_PORT]->nBufferAlignment = OMX_AUDDEC_DEFAULT_BUFFER_ALIGNMENT;

    /*-------------------------------------------------------------------------*
     *  Initialize the eNotifyType of the component with NONE as the data
     *  processing is based on the periodic driver call-back rather than usual
     *  DataNotify event
     *------------------------------------------------------------------------*/
    pAudDecComPvt->tDerToBase.pPortProperties[OMX_AUDDEC_INPUT_PORT]->eNotifyType =
                                            OMX_NOTIFY_TYPE_ALWAYS;
    /* Override the default values set to an Output Port with Actual/Default
       values */
    pAudDecComPvt->pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->bEnabled = OMX_FALSE;
    pAudDecComPvt->pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->eDir = OMX_DirOutput;
    pAudDecComPvt->pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->nPortIndex =
        OMX_AUDDEC_OUTPUT_PORT;
    pAudDecComPvt->pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->nBufferCountMin =
        OMX_AUDDEC_DEFAULT_OUTPUT_BUFFER_COUNT;
    pAudDecComPvt->pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->nBufferCountActual =
        OMX_AUDDEC_DEFAULT_OUTPUT_BUFFER_COUNT;
    pAudDecComPvt->pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->nBufferSize =
        OMX_AUDDEC_DEFAULT_OUTPUT_BUFFER_SIZE;
    pAudDecComPvt->pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->eDomain =
        OMX_PortDomainAudio;
    pAudDecComPvt->pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->format.audio.cMIMEType =
        "PCM";
    pAudDecComPvt->pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->format.
        audio.pNativeRender = OMX_FALSE;

    pAudDecComPvt->pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->format.
        audio.bFlagErrorConcealment = OMX_FALSE;
    pAudDecComPvt->pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->format.
        audio.eEncoding = OMX_AUDIO_CodingUnused;
    pAudDecComPvt->pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->format.
        audio.pNativeRender = OMX_FALSE;
    pAudDecComPvt->pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->bBuffersContiguous =
        OMX_FALSE;

    /* todo FrameQ would provide aligned buffers */
    pAudDecComPvt->pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->nBufferAlignment = OMX_AUDDEC_DEFAULT_BUFFER_ALIGNMENT;

    /*-------------------------------------------------------------------------*
     *  Initialize the eNotifyType of the component with NONE as the data
     *  processing is based on the periodic driver call-back rather than usual
     *  DataNotify event
     *------------------------------------------------------------------------*/
    pAudDecComPvt->tDerToBase.pPortProperties[OMX_AUDDEC_OUTPUT_PORT]->eNotifyType =
                                              OMX_NOTIFY_TYPE_ALWAYS;

    /* Initialize Audio Format type parameters of an Input port; default
       compression format would be supplied throgh parameter */

    OMX_BASE_INIT_STRUCT_PTR(&pAudDecComPvt->tAudioParams
                             [OMX_AUDDEC_INPUT_PORT],
                             OMX_AUDIO_PARAM_PORTFORMATTYPE);
    pAudDecComPvt->tAudioParams[OMX_AUDDEC_INPUT_PORT].nPortIndex =
        OMX_AUDDEC_INPUT_PORT;
    pAudDecComPvt->tAudioParams[OMX_AUDDEC_INPUT_PORT].nIndex = 0;
    pAudDecComPvt->tAudioParams[OMX_AUDDEC_INPUT_PORT].eEncoding=
        compressionFormat;

    OMX_BASE_INIT_STRUCT_PTR(&pAudDecComPvt->tAudioParams
                             [OMX_AUDDEC_OUTPUT_PORT],
                             OMX_AUDIO_PARAM_PORTFORMATTYPE);
    pAudDecComPvt->tAudioParams[OMX_AUDDEC_OUTPUT_PORT].nPortIndex =
        OMX_AUDDEC_OUTPUT_PORT;
    pAudDecComPvt->tAudioParams[OMX_AUDDEC_OUTPUT_PORT].nIndex = 0;
    pAudDecComPvt->tAudioParams[OMX_AUDDEC_OUTPUT_PORT].eEncoding =
        OMX_AUDIO_CodingUnused;

    /* Set the current State to Loaded  */

    pAudDecComPvt->tCurState = OMX_StateLoaded;

    pAudDecComPvt->pDecStatus->size = sizeof(AUDDEC1_Status);
    ((IAUDDEC1_DynamicParams *) (pAudDecComPvt->pDecDynParams))->size =
        sizeof(AUDDEC1_DynamicParams);

    /* Set Default Static & Dynamic params */
    pAudDecComPvt->fpSet_DynamicParams(pAudDecComPvt->pDecDynParams);
    pAudDecComPvt->fpSet_StaticParams(pAudDecComPvt, pAudDecComPvt->pDecStaticParams);

    for (i = 0; i < OMX_AUDDEC_MAX_INPUT_BUFFER_COUNT; i++) {
        pAudDecComPvt->decInArgsPtr = &(pAudDecComPvt->decInArgs[i]);
        pAudDecComPvt->decInArgsPtr->auddec1InArgs.size =
            sizeof(IAUDDEC1_InArgs);
        pAudDecComPvt->decInArgsPtr->auddec1InArgs.numBytes = 0;
    }

    for (i = 0; i < OMX_AUDDEC_MAX_OUTPUT_BUFFER_COUNT; i++) {
        pAudDecComPvt->decOutArgsPtr = &(pAudDecComPvt->decOutArgs[i]);
        pAudDecComPvt->decOutArgsPtr->auddec1OutArgs.size =
            sizeof(IAUDDEC1_OutArgs);
        pAudDecComPvt->decOutArgsPtr->auddec1OutArgs.bytesConsumed = 0;
    }

    pAudDecComPvt->isfirstFrame = 1;    

    /* Based on this flag codec would be created */
    pAudDecComPvt->bCodecCreate = OMX_TRUE;

    /* using default buffers */
    pAudDecComPvt->bInBufferCountChange = OMX_FALSE;
    pAudDecComPvt->bOutBufferCountChange = OMX_FALSE;
    pAudDecComPvt->inBufHeaderArray = NULL;
    pAudDecComPvt->outBufHeaderArray = NULL;

    /* initialize the code engine; todo for multichannel might require once */
    pAudDecComPvt->ce = Engine_open(engineName, NULL, &errorCode);
    if (pAudDecComPvt->ce == NULL) {
        OMX_BASE_Error("Engine creation: Could not open engine \n");
        eError = OMX_ErrorInsufficientResources;
        goto EXIT;
    }

  /*--------------------------------------------------------------------------*/
  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /*  Set Task exit flag as False                                             */
  /*                                                                          */
  /*--------------------------------------------------------------------------*/
    pAudDecComPvt->bTaskExit = OMX_FALSE;
    pAudDecComPvt->bCmdFlush = OMX_FALSE;
    pAudDecComPvt->bEOSProcessOn = OMX_FALSE;

  EXIT:
    return eError;
}

/* ===========================================================================*/
/**
********************************************************************************
 *  @func   OMX_TI_AUDDEC_ComponentDeinit()
 *  @brief  This method is used to deinitalize the component which means to free
 *          any resources allocated at component initialization.
 *
 *  @param hComponent  : Component handle
 *
 *  @return
 *  OMX_ErrorNone = Successful
 *
 *  Other_value = Failed (Error code is returned)
 *
 */
  /* ==========================================================================
   */
static OMX_ERRORTYPE OMX_TI_AUDDEC_ComponentDeinit(OMX_HANDLETYPE hComponent)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;

    OMX_COMPONENTTYPE *pComp;

    OMX_AUDDEC_PVTTYPE *pAudDecComPvt;

    OMX_U32 nIndex = 0;

    OMX_BASE_REQUIRE(hComponent != NULL, OMX_ErrorBadParameter);

    pComp = (OMX_COMPONENTTYPE *) hComponent;
    pAudDecComPvt = (OMX_AUDDEC_PVTTYPE *) pComp->pComponentPrivate;

    g_AudDecComponentCreated--;

    if(g_AudDecComponentCreated == 0) pAudDecComPvt->bTaskExit = OMX_TRUE;

    /* Add CE deinit related stuff here
       Deinitialize the decoder Instance/s and free all the resources     */
    if (pAudDecComPvt->ce) {
        Engine_close(pAudDecComPvt->ce);
    }

	if(pAudDecComPvt->tDerToBase.
        pPortProperties[OMX_AUDDEC_OUTPUT_PORT]->pMetaDataFieldTypesArr) {
    TIMM_OSAL_Free(pAudDecComPvt->tDerToBase.
        pPortProperties[OMX_AUDDEC_OUTPUT_PORT]->pMetaDataFieldTypesArr);
        pAudDecComPvt->tDerToBase.
            pPortProperties[OMX_AUDDEC_OUTPUT_PORT]->pMetaDataFieldTypesArr = NULL;
    }   

    if (pAudDecComPvt->tDerToBase.
        pPortProperties[OMX_AUDDEC_OUTPUT_PORT]->pBufParams) {
        TIMM_OSAL_Free(pAudDecComPvt->
                       tDerToBase.pPortProperties[OMX_AUDDEC_OUTPUT_PORT]->
                       pBufParams);
        pAudDecComPvt->tDerToBase.
            pPortProperties[OMX_AUDDEC_OUTPUT_PORT]->pBufParams = NULL;
    }

    for (nIndex = 0;
         nIndex < pAudDecComPvt->tDerToBase.pAudioPortParams->nPorts;
         nIndex++) {
        TIMM_OSAL_Free(pAudDecComPvt->tDerToBase.pPortProperties[nIndex]);
    }

    /* free up memory allocated for buffer headers */
	if(pAudDecComPvt->inBufHeaderArray != NULL) {
    for (nIndex = 0; nIndex <
         pAudDecComPvt->pPortdefs[OMX_AUDDEC_INPUT_PORT]->nBufferCountActual;
         nIndex++) {
				 if(pAudDecComPvt->inBufHeaderArray[nIndex].pBufHeader != NULL) {
					TIMM_OSAL_Free(pAudDecComPvt->inBufHeaderArray[nIndex].pBufHeader);
					pAudDecComPvt->inBufHeaderArray[nIndex].pBufHeader = NULL;
				 }
		}
	}

	if(pAudDecComPvt->outBufHeaderArray != NULL) {
		for (nIndex = 0; nIndex <
			 pAudDecComPvt->pPortdefs[OMX_AUDDEC_OUTPUT_PORT]->nBufferCountActual;
			 nIndex++) {
				 if(pAudDecComPvt->outBufHeaderArray[nIndex].pBufHeader != NULL) {
					TIMM_OSAL_Free(pAudDecComPvt->outBufHeaderArray[nIndex].pBufHeader);
					pAudDecComPvt->outBufHeaderArray[nIndex].pBufHeader = NULL;
				 }
		}
	}

    if(pAudDecComPvt->inBufHeaderArray != NULL) {
       TIMM_OSAL_Free(pAudDecComPvt->inBufHeaderArray);
       pAudDecComPvt->inBufHeaderArray = NULL;
    }

    if(pAudDecComPvt->outBufHeaderArray != NULL) {
      TIMM_OSAL_Free(pAudDecComPvt->outBufHeaderArray);
      pAudDecComPvt->outBufHeaderArray = NULL;
    }

    /* Calling OMX Base Component Deinit */
    eError = OMX_BASE_ComponentDeinit(hComponent);
    OMX_BASE_ASSERT(eError == OMX_ErrorNone, eError);

	if(pAudDecComPvt->pDecStaticParams) {
    TIMM_OSAL_Free(pAudDecComPvt->pDecStaticParams);
	   pAudDecComPvt->pDecStaticParams = NULL;
	}

    if(pAudDecComPvt->pDecDynParams) {
	   TIMM_OSAL_Free(pAudDecComPvt->pDecDynParams);
	   pAudDecComPvt->pDecDynParams = NULL;
	}

	if(pAudDecComPvt->pDecStatus) {
    TIMM_OSAL_Free(pAudDecComPvt->pDecStatus);
	   pAudDecComPvt->pDecStatus = NULL;
	}

	if(pAudDecComPvt->tDerToBase.pAudioPortParams) {
       TIMM_OSAL_Free(pAudDecComPvt->tDerToBase.pAudioPortParams);
	   pAudDecComPvt->tDerToBase.pAudioPortParams = NULL;
	}

	if(pAudDecComPvt->tDerToBase.pPortProperties) {
    TIMM_OSAL_Free(pAudDecComPvt->tDerToBase.pPortProperties);
	   pAudDecComPvt->tDerToBase.pPortProperties = NULL;
	}

	if(pAudDecComPvt->tDerToBase.cComponentName) {
       TIMM_OSAL_Free(pAudDecComPvt->tDerToBase.cComponentName);
	   pAudDecComPvt->tDerToBase.cComponentName = NULL;
	}

	if(pAudDecComPvt) {
    TIMM_OSAL_Free(pAudDecComPvt);
       pComp->pComponentPrivate = NULL;
	}

  EXIT:
    return eError;
}

/* ==========================================================================
 */
