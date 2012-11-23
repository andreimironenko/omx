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
/*
 * ****************************************************************************
 *  @file  omx_aenc.c
 *  @brief This file contains methods that provides the functionality for a
 *         Audio Encoder OpenMax 1.1 Component.
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
#include <ti/sdo/ce/audio1/audenc1.h>
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
#include "omx_aenc_priv.h"
#include "omx_aenc_util.h"

#include "omx_aenc.h"

#include "omx_aacae_internal.h"

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
#define STREAM_FLUSH_DONE               (0x200)

//extern int FRAME_ENCODED[];
/*----------------------- function prototypes --------------------------------*/

/*****************************************************************************
 * PRIVATE DECLARATIONS Defined here, used only here
 ******************************************************************************/

/** Codec engine name. This should be same as the one created in the cfg file */

static String engineName = "aenc"; //SP: same as in vpsAppMain.cfg

int ENCODER_CHANNEL = 0;
OMX_U32 g_AudEncComponentCreated = 0;

OMX_AudEncCallbacks OMX_AudEncCallbacksTable[OMX_AudEncCallbacksTableEntryCnt] = {

#ifdef CODEC_AACENC
    {
     OMX_AUDIO_CodingAAC,
     OMX_TI_AACAE_Init,
     OMX_TI_AACAE_Set_StaticParams,
     OMX_TI_AACAE_Set_DynamicParams,
     NULL,
     OMX_TI_AACAE_Outbuff_Details,
     }
#endif
};

/**
********************************************************************************
 *  @func   OMX_AUDENC_ComponentInit()
 *  @brief  Entry point for OMX Audio Encoder Component.
 *          This function is called whenever the IL Client invokes a GetHandle
 *          function with the name "OMX.TI.DSP.AUDENC"
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

OMX_ERRORTYPE OMX_TI_AUDENC_ComponentInit(OMX_HANDLETYPE hComponent)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    eError =
        OMX_TI_AUDENC_ComponentInit_Common(hComponent,
                                          OMX_AUDENC_DEFAULT_COMPRESSION_FORMAT);
    return(eError);
}

/**
********************************************************************************
 *  @func   OMX_TI_AUDENC_ComponentInit_Common()
 *  @brief   This function is called whenever the IL Client invokes a GetHandle
 *          function with the name "OMX.TI.DSP.AUDENC".
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
OMX_ERRORTYPE OMX_TI_AUDENC_ComponentInit_Common(OMX_HANDLETYPE hComponent,
                                                 OMX_AUDIO_CODINGTYPE
                                                 compressionFormat)
{
	OMX_ERRORTYPE eError = OMX_ErrorNone;

    OMX_AUDENC_PVTTYPE *pAudEncComPvt = NULL;

    OMX_COMPONENTTYPE *pHandle = TIMM_OSAL_NULL;

    OMX_U32 i = 0;

    /* Check the input parameters, this should be TRUE else report an Error */

    OMX_BASE_REQUIRE(hComponent != NULL, OMX_ErrorBadParameter);

    pHandle = (OMX_COMPONENTTYPE *) hComponent;

    OMX_BASE_CHK_VERSION(pHandle, OMX_COMPONENTTYPE, eError);

    /* Allocate memory for AUDENC Component's private data area */

    pHandle->pComponentPrivate =
        (OMX_AUDENC_PVTTYPE *) TIMM_OSAL_Malloc(sizeof(OMX_AUDENC_PVTTYPE),
                                                TIMM_OSAL_TRUE, 0,
                                                TIMMOSAL_MEM_SEGMENT_EXT);

    OMX_BASE_ASSERT(pHandle->pComponentPrivate != NULL,
                    OMX_ErrorInsufficientResources);

    TIMM_OSAL_Memset(pHandle->pComponentPrivate, 0x0,
                     sizeof(OMX_AUDENC_PVTTYPE));

    pAudEncComPvt = (OMX_AUDENC_PVTTYPE *) pHandle->pComponentPrivate;

    /* Initialize Derived to Base params with Actual values; Fill the structure
       "OMX_BASE_PARAMSFROMDERIVED" which needs to be given as an input to the
       Base Component before calling Base Component Init method */

    pAudEncComPvt->tDerToBase.cComponentName =
        (OMX_STRING) TIMM_OSAL_Malloc(sizeof(OMX_U8) *
                                      OMX_MAX_STRINGNAME_SIZE, TIMM_OSAL_TRUE,
                                      0, TIMMOSAL_MEM_SEGMENT_EXT);
    OMX_BASE_ASSERT(pAudEncComPvt->tDerToBase.cComponentName != NULL,
                    OMX_ErrorInsufficientResources);

    TIMM_OSAL_Memset (pAudEncComPvt->tDerToBase.cComponentName, 0,
                      (sizeof ( OMX_U8 ) * OMX_MAX_STRINGNAME_SIZE));

    TIMM_OSAL_Memcpy(pAudEncComPvt->tDerToBase.cComponentName,
                     OMX_AUDENC_COMPONENT_NAME,
                     strlen(OMX_AUDENC_COMPONENT_NAME)+1);

    /* The component will use the default heap */

    pAudEncComPvt->tDerToBase.hDefaultHeap = NULL;

    /* Initialize Audio Port parameters */

    OMX_BASE_MALLOC_STRUCT(pAudEncComPvt->tDerToBase.pAudioPortParams,
                           OMX_PORT_PARAM_TYPE,
                           pAudEncComPvt->tDerToBase.hDefaultHeap);
    OMX_BASE_INIT_STRUCT_PTR(pAudEncComPvt->tDerToBase.pAudioPortParams,
                             OMX_PORT_PARAM_TYPE);
    pAudEncComPvt->tDerToBase.pAudioPortParams->nPorts =
        OMX_AUDENC_NUM_OF_PORTS;
    pAudEncComPvt->tDerToBase.pAudioPortParams->nStartPortNumber =
        OMX_AUDENC_DEFAULT_START_PORT_NUM;

    /* Setting number of ports - this has to be set before calling SetDefault
       Properties */

    pAudEncComPvt->tDerToBase.nNumPorts = OMX_AUDENC_NUM_OF_PORTS;
    pAudEncComPvt->tDerToBase.nMinStartPortIndex =
        OMX_AUDENC_DEFAULT_START_PORT_NUM;

    /* Allocating memory for port properties before calling SetDefaultProperties */

    pAudEncComPvt->tDerToBase.pPortProperties = (OMX_BASE_PORT_PROPERTIES **)
        TIMM_OSAL_Malloc(sizeof(OMX_BASE_PORT_PROPERTIES *) *
                         pAudEncComPvt->tDerToBase.nNumPorts, TIMM_OSAL_TRUE, 0,
                         TIMMOSAL_MEM_SEGMENT_EXT);

    OMX_BASE_ASSERT(pAudEncComPvt->tDerToBase.pPortProperties != NULL,
                    OMX_ErrorInsufficientResources);

    for (i = 0; i < (pAudEncComPvt->tDerToBase.nNumPorts); i++) {
        pAudEncComPvt->tDerToBase.pPortProperties[i] =
            (OMX_BASE_PORT_PROPERTIES *) TIMM_OSAL_Malloc
            (sizeof(OMX_BASE_PORT_PROPERTIES), TIMM_OSAL_TRUE, 0,
             TIMMOSAL_MEM_SEGMENT_EXT);
        TIMM_OSAL_Memset(pAudEncComPvt->tDerToBase.pPortProperties[i], 0x0,
                         sizeof(OMX_BASE_PORT_PROPERTIES));


        OMX_BASE_ASSERT(pAudEncComPvt->tDerToBase.pPortProperties[i] != NULL,
                        OMX_ErrorInsufficientResources);
    }
    /* Setting default properties. PreCondition: NumOfPorts is filled and all
       pointers allocated */

    OMX_BASE_SetDefaultProperties(hComponent);

    /* encoder task is based on notification for every event */

    pAudEncComPvt->tDerToBase.pPortProperties[OMX_AUDENC_INPUT_PORT]->
        eNotifyType = OMX_NOTIFY_TYPE_ALWAYS;
    pAudEncComPvt->tDerToBase.pPortProperties[OMX_AUDENC_OUTPUT_PORT]->
        eNotifyType = OMX_NOTIFY_TYPE_ALWAYS;


    /* default memory type, it can be modified through set param in the IL client */
    pAudEncComPvt->tDerToBase.pPortProperties[OMX_AUDENC_INPUT_PORT]->
            eBufMemoryType = OMX_BUFFER_MEMORY_DEFAULT;
  	pAudEncComPvt->tDerToBase.pPortProperties[OMX_AUDENC_INPUT_PORT]->nNumComponentBuffers = OMX_AUDENC_MAX_INPUT_BUFFER_COUNT;
    pAudEncComPvt->tDerToBase.pPortProperties[OMX_AUDENC_INPUT_PORT]->pBufParams = NULL;

    pAudEncComPvt->tDerToBase.pPortProperties[OMX_AUDENC_OUTPUT_PORT]->
        eBufMemoryType = OMX_BUFFER_MEMORY_DEFAULT;
	pAudEncComPvt->tDerToBase.pPortProperties[OMX_AUDENC_OUTPUT_PORT]->nNumComponentBuffers = OMX_AUDENC_MAX_INPUT_BUFFER_COUNT;
     pAudEncComPvt->tDerToBase.pPortProperties[OMX_AUDENC_OUTPUT_PORT]->pBufParams = NULL;

    /* Overriding this value. Notify derived component only when data is
       available on all ports, however audenc component is implemented as signle
       port */

    pAudEncComPvt->tDerToBase.bNotifyForAnyPort = OMX_TRUE;

    /* Set the ContextType whether passive/Active depending on requirement,
       for audenc default is active */

    pAudEncComPvt->tDerToBase.ctxType = OMX_AUDENC_DEFAULT_CONTEXT_TYPE;

    /* Incase of Active Context, set the Stacksize and GroupPriority */

    if (pAudEncComPvt->tDerToBase.ctxType == OMX_BASE_CONTEXT_ACTIVE) {
        snprintf(pAudEncComPvt->tDerToBase.cTaskName,
                 sizeof(pAudEncComPvt->tDerToBase.cTaskName),
                 "AENC_%p", hComponent);
        pAudEncComPvt->tDerToBase.nStackSize = OMX_AUDENC_TASK_STACKSIZE;
        pAudEncComPvt->tDerToBase.nPrioirty = OMX_AUDENC_TASK_PRIORITY;
    }

    pAudEncComPvt->tDerToBase.nGroupID = OMX_AUDENC_GROUPID;
    pAudEncComPvt->tDerToBase.nGroupPriority = OMX_AUDENC_GROUPPRIORITY;

    /* Fill component's version, this may not be same as the OMX Specification
       version */
    pAudEncComPvt->tDerToBase.nComponentVersion.s.nVersionMajor =
        OMX_AUDENC_COMP_VERSION_MAJOR;
    pAudEncComPvt->tDerToBase.nComponentVersion.s.nVersionMinor =
        OMX_AUDENC_COMP_VERSION_MINOR;
    pAudEncComPvt->tDerToBase.nComponentVersion.s.nRevision =
        OMX_AUDENC_COMP_VERSION_REVISION;
    pAudEncComPvt->tDerToBase.nComponentVersion.s.nStep =
        OMX_AUDENC_COMP_VERSION_STEP;

    /* Hooks to Notify Command and Data from Base Comp to Derived Comp */

    pAudEncComPvt->tDerToBase.fpCommandNotify = OMX_TI_AUDENC_CommandNotify;
    pAudEncComPvt->tDerToBase.fpDataNotify = OMX_TI_AUDENC_DataNotify;

    /* Initializing the Base Component */
    eError = OMX_BASE_ComponentInit(hComponent);
    OMX_BASE_ASSERT(OMX_ErrorNone == eError, eError);

    /* Here we can override whatever default the BaseComponent had set, we can
       override the function pointers in the private structure  */

    pAudEncComPvt = (OMX_AUDENC_PVTTYPE *) pHandle->pComponentPrivate;
    /* Override the function pointers of pHandle "OMX_COMPONENTTYPE" which Base
       cannot handle and needs to be taken care by the Dervied comp */

    pHandle->GetParameter = OMX_TI_AUDENC_GetParameter;
    pHandle->SetParameter = OMX_TI_AUDENC_SetParameter;
    pHandle->SetConfig = OMX_TI_AUDENC_SetConfig;
    pHandle->GetConfig = OMX_TI_AUDENC_GetConfig;
    pHandle->ComponentDeInit = OMX_TI_AUDENC_ComponentDeinit;

    /* overriding these for allocating struct to platform private */

    pHandle->UseBuffer = OMX_TI_AUDENC_UseBuffer;
    pHandle->AllocateBuffer = OMX_TI_AUDENC_AllocateBuffer;
    pHandle->FreeBuffer = OMX_TI_AUDENC_FreeBuffer;

    /* Initialize the call back function pointers specific to this encoder */

    OMX_TI_AUDENC_InitCallbacks(hComponent, compressionFormat);

    /* Set the default port properties, codec engine init */

    eError = OMX_TI_AUDENC_SetDefault((OMX_AUDENC_PVTTYPE *) pAudEncComPvt,
                                      compressionFormat);
    OMX_BASE_ASSERT(OMX_ErrorNone == eError, eError);   

    pAudEncComPvt->channel = ENCODER_CHANNEL++;

  EXIT:
    if (eError != OMX_ErrorNone) {
        OMX_BASE_Error(" AACEnc Comp Initialization Failed ");
        if (pHandle) {
            pHandle->ComponentDeInit(hComponent);
        }
    }    
    g_AudEncComponentCreated++;

    return eError;
}

/* ========================================================================== */

/**
********************************************************************************
 *  @func   OMX_TI_AUDENC_AllocateBuffer()
 *  @brief  Allocates the buffers of nSizeBytes, along with allocating the
 *          buffer headers.
 *
 * @param [in] hComponent   : Component handle
 * @param [in] ppBuffer     : Pointer to the buffer header pointer
 * @param [in] nPortIndex   : Port Index on which the allocation has to be made
 * @param [in] pAppPrivate  : pointer to the app private structure
 * @param [in] nSizeBytes   : Total size of the bytes requested
 *
 *  @return
 *  OMX_ErrorNone = Successful
 *
 *  Other_value = Failed (Error code is returned)
 *
********************************************************************************
*/
/* ===========================================================================*/
OMX_ERRORTYPE OMX_TI_AUDENC_AllocateBuffer(OMX_IN OMX_HANDLETYPE hComponent,
                                           OMX_INOUT OMX_BUFFERHEADERTYPE **
                                           ppBuffer,
                                           OMX_IN OMX_U32 nPortIndex,
                                           OMX_IN OMX_PTR pAppPrivate,
                                           OMX_IN OMX_U32 nSizeBytes)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;

    /* base function would do the buffer allocation  */

    OMX_BASE_REQUIRE((hComponent != NULL), OMX_ErrorBadParameter);

    eError =
        OMX_BASE_AllocateBuffer(hComponent, ppBuffer, nPortIndex, pAppPrivate,
                                nSizeBytes);
    if (eError != OMX_ErrorNone) {
        goto EXIT;
    }

  EXIT:
    return eError;
}

/* ========================================================================== */

/**
********************************************************************************
 *  @func   OMX_TI_AUDENC_UseBuffer()
 *  @brief  This function accepts the buffer of nSizeBytes, along
 *          with the buffer headers. The buffer is allocated by the IL Client
 *
 *  @param [in] hComponent   : Component handle
 *  @param [in] ppBufferHdr  : Pointer to the buffer header pointer
 *  @param [in] nPortIndex   : Port Index of the buffer
 *  @param [in] pAppPrivate  : pointer to the app private structure
 *  @param [in] nSizeBytes   : Total size of the bytes allocated
 *  @param [in] pBuffer      : Pointer to the buffer
 *
 *  @return
 *  OMX_ErrorNone = Successful
 *
 *  Other_value = Failed (Error code is returned)
 *
********************************************************************************
*/
/* ========================================================================== */

OMX_ERRORTYPE OMX_TI_AUDENC_UseBuffer(OMX_IN OMX_HANDLETYPE hComponent,
                                      OMX_INOUT OMX_BUFFERHEADERTYPE **
                                      ppBufferHdr, OMX_IN OMX_U32 nPortIndex,
                                      OMX_IN OMX_PTR pAppPrivate,
                                      OMX_IN OMX_U32 nSizeBytes,
                                      OMX_IN OMX_U8 * pBuffer)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;

    /* base implements the openMax basic calls */

    OMX_BASE_REQUIRE((hComponent != NULL), OMX_ErrorBadParameter);

    eError =
        OMX_BASE_UseBuffer(hComponent, ppBufferHdr, nPortIndex, pAppPrivate,
                           nSizeBytes, pBuffer);
    if (eError != OMX_ErrorNone) {
        goto EXIT;
    }
  EXIT:
    return eError;
}

/* ========================================================================== */

/**
********************************************************************************
 *  @func  OMX_TI_AUDENC_FreeBuffer()
 *  @brief This function frees the buffer of nSizeBytes
 *         and the buffer headers that was allocated using AllocateBuffer calls.
 *
 *  @param [in] hComponent   : Component handle
 *  @param [in] nPortIndex   : Port Index of the buffer
 *  @param [in] pBuffer      : Pointer to the buffer that has to be freed
 *
 *  @return
 *  OMX_ErrorNone = Successful
 *
 *  Other_value = Failed (Error code is returned)
********************************************************************************
*/
/* ========================================================================== */

OMX_ERRORTYPE OMX_TI_AUDENC_FreeBuffer(OMX_IN OMX_HANDLETYPE hComponent,
                                       OMX_IN OMX_U32 nPortIndex,
                                       OMX_IN OMX_BUFFERHEADERTYPE * pBuffer)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;

    /* base implements the openMax basic calls */

    OMX_BASE_REQUIRE((hComponent != NULL), OMX_ErrorBadParameter);

    eError = OMX_BASE_FreeBuffer(hComponent, nPortIndex, pBuffer);

    EXIT:
    return eError;
}

/* ========================================================================== */
/* ========================================================================== */

/**
********************************************************************************
 *  @func   OMX_TI_AUDENC_GetParameter()
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

static OMX_ERRORTYPE OMX_TI_AUDENC_GetParameter(OMX_HANDLETYPE hComponent,
                                                OMX_INDEXTYPE nParamIndex,
                                                OMX_PTR pParamStruct)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;

    OMX_AUDENC_PVTTYPE *pAudEncComPvt;

    OMX_COMPONENTTYPE *pHandle;

    OMX_BASE_REQUIRE((hComponent != NULL)
                     && (pParamStruct != NULL), OMX_ErrorBadParameter);

    pHandle = (OMX_COMPONENTTYPE *) hComponent;
    pAudEncComPvt = (OMX_AUDENC_PVTTYPE *) pHandle->pComponentPrivate;

    /* GetParameter can't be invoked incase the comp is in Invalid State */

    OMX_BASE_ASSERT(pAudEncComPvt->tCurState != OMX_StateInvalid,
                    OMX_ErrorIncorrectStateOperation);

    switch (nParamIndex) {

        /* Client uses this to retrieve the Info related to the params of audio
           ports */
       
       /*OMX_IndexParamPortDefinition implemented in BASE*/

        /* client uses this to query the format supported by the port   */
    case OMX_IndexParamAudioPortFormat:
        {
		    OMX_AUDIO_PARAM_PORTFORMATTYPE *pAudioPortFormats;
            OMX_BASE_CHK_VERSION(pParamStruct, OMX_AUDIO_PARAM_PORTFORMATTYPE,
                                 eError);

            pAudioPortFormats = (OMX_AUDIO_PARAM_PORTFORMATTYPE *) pParamStruct;

            if (pAudioPortFormats->nPortIndex ==
                pAudEncComPvt->pPortdefs[OMX_AUDENC_INPUT_PORT]->nPortIndex) {
                if (pAudioPortFormats->nIndex > 0 ) {
                    eError = OMX_ErrorNoMore;
                } 
                else {
		     pAudioPortFormats->eEncoding =
                            pAudEncComPvt->pPortdefs[OMX_AUDENC_INPUT_PORT]->format.audio.eEncoding;
                }
            } 
            else if (pAudioPortFormats->nPortIndex ==   pAudEncComPvt->
                       pPortdefs[OMX_AUDENC_OUTPUT_PORT]->nPortIndex) {
                if (pAudioPortFormats->nIndex > 0 ) {
                    eError = OMX_ErrorNoMore;
                } 
                else {
    			pAudioPortFormats->eEncoding =
                           pAudEncComPvt->pPortdefs[OMX_AUDENC_OUTPUT_PORT]->format.audio.eEncoding;
                }
            } 
            else {
                eError = OMX_ErrorBadPortIndex;
            }
        }
        break;

    case OMX_IndexParamAudioAac:
        {
            OMX_AUDIO_PARAM_AACPROFILETYPE * pAACProfile;
            OMX_BASE_CHK_VERSION(pParamStruct, OMX_AUDIO_PARAM_AACPROFILETYPE, eError);
            pAACProfile =( OMX_AUDIO_PARAM_AACPROFILETYPE *)pParamStruct;
			if(pAACProfile->nPortIndex == pAudEncComPvt->
                pPortdefs[OMX_AUDENC_OUTPUT_PORT]->nPortIndex){
                   pAACProfile->nChannels = 
                     OMX_TI_CECHANNELS_TO_NCHANNELS((IAUDENC1_Params *)pAudEncComPvt->pEncStaticParams); 
                   pAACProfile->nSampleRate = ((IAUDENC1_Params *)pAudEncComPvt->pEncStaticParams)->sampleRate;
                   pAACProfile->nBitRate = ((IAUDENC1_Params *)pAudEncComPvt->pEncStaticParams)->bitRate;
                   pAACProfile->nAudioBandWidth = 0;
				   /*1024 samples per channel*/
                   pAACProfile->nFrameLength = AAC_DEFAULT_SAMPLES_IN_A_FRAME ;
                   pAACProfile->nAACtools = OMX_AUDIO_AACToolNone ;
                   pAACProfile->nAACERtools = OMX_AUDIO_AACERNone ;
                   pAACProfile->eAACProfile = OMX_AUDIO_AACObjectLC ;
                   pAACProfile->eAACStreamFormat = 
                       (OMX_AUDIO_AACSTREAMFORMATTYPE) OMX_TI_AE_SF_TO_OMX_SF(
                          (IAUDENC1_Params *)pAudEncComPvt->pEncStaticParams);
            }
	    else {
                eError = OMX_ErrorBadPortIndex;
            }

        }
        break;
   case OMX_IndexParamAudioPcm:
        {
            OMX_AUDIO_PARAM_PCMMODETYPE *pPcmMode;
            OMX_BASE_CHK_VERSION(pParamStruct, OMX_AUDIO_PARAM_PCMMODETYPE, eError);
            pPcmMode = (OMX_AUDIO_PARAM_PCMMODETYPE *)pParamStruct;
            if(pPcmMode->nPortIndex == pAudEncComPvt->
                pPortdefs[OMX_AUDENC_INPUT_PORT]->nPortIndex){
                pPcmMode->nChannels = 
                        OMX_TI_CECHANNELS_TO_NCHANNELS(
                             (IAUDENC1_Params *)pAudEncComPvt->pEncStaticParams);
		pPcmMode->eNumData = OMX_NumericalDataUnsigned;
		pPcmMode->eEndian = OMX_EndianLittle;
                pPcmMode->bInterleaved = OMX_TRUE;
                pPcmMode->nBitPerSample = 16;
		pPcmMode->nSamplingRate = ((IAUDENC1_Params *)pAudEncComPvt->pEncStaticParams)->sampleRate;
		pPcmMode->ePCMMode = OMX_AUDIO_PCMModeLinear;



	    }
            else {
                eError = OMX_ErrorBadPortIndex;
            }


	}
	break;

        /* redirects the call to "OMX_BASE_GetParameter" which supports standard
           comp indexes */

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
 *  @func   OMX_TI_AUDENC_InitCallbacks()
 *  @brief  This method initialize the audenc components callback, which are
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

OMX_ERRORTYPE OMX_TI_AUDENC_InitCallbacks(OMX_HANDLETYPE hComponent,
                                         OMX_U32 compressionFormat)
{
    int i;
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_COMPONENTTYPE *pHandle;
    OMX_AUDENC_PVTTYPE *pAudEncComPvt = NULL;

    pHandle = (OMX_COMPONENTTYPE *) hComponent;
    pAudEncComPvt = (OMX_AUDENC_PVTTYPE *) pHandle->pComponentPrivate;

    for (i = 0; i < OMX_AudEncCallbacksTableEntryCnt; i++) {
        if (OMX_AudEncCallbacksTable[i].compressionFormat == compressionFormat)
            break;
    }
    if (i == OMX_AudEncCallbacksTableEntryCnt) {
        return eError;
    }
    pAudEncComPvt->fpSet_StaticParams =
        OMX_AudEncCallbacksTable[i].fpSet_StaticParams;
    pAudEncComPvt->fpSet_DynamicParams =
        OMX_AudEncCallbacksTable[i].fpSet_DynamicParams;
    pAudEncComPvt->fpSet_Status = OMX_AudEncCallbacksTable[i].fpSet_Status;
    pAudEncComPvt->fpCalculate_oubuffdetails =
        OMX_AudEncCallbacksTable[i].fpCalculate_oubuffdetails;

    OMX_AudEncCallbacksTable[i].fpInit_Audenc(hComponent);

    return eError;
}

/* ========================================================================== */

/**
********************************************************************************
 *  @func   OMX_TI_AUDENC_SetParameter()
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
static OMX_ERRORTYPE OMX_TI_AUDENC_SetParameter(OMX_HANDLETYPE hComponent,
                                                OMX_INDEXTYPE nParamIndex,
                                                OMX_PTR pParamStruct)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;

    OMX_U32 nPortIx;

    OMX_AUDENC_PVTTYPE *pAudEncComPvt = NULL;

    OMX_COMPONENTTYPE *pHandle;

    OMX_PARAM_PORTDEFINITIONTYPE *pPortDefs = NULL;

    OMX_PARAM_COMPPORT_NOTIFYTYPE *pNotifyType = NULL;

    OMX_BASE_REQUIRE((hComponent != NULL)
                     && (pParamStruct != NULL), OMX_ErrorBadParameter);

    pHandle = (OMX_COMPONENTTYPE *) hComponent;
    pAudEncComPvt = (OMX_AUDENC_PVTTYPE *) pHandle->pComponentPrivate;

    /* SetParameter can be invoked only when the comp is loaded or on disabled
       port */
    nPortIx = (( OMX_CONFIG_COMSTRUCT *)pParamStruct)->nPortIndex;

    OMX_BASE_ASSERT((pAudEncComPvt->tCurState == OMX_StateLoaded) ||
                    (pAudEncComPvt->pPortdefs[nPortIx]->
                     bEnabled == OMX_FALSE), OMX_ErrorIncorrectStateOperation);

    switch (nParamIndex) {

        /* Client uses this to modify the num of Audio ports  */

    case OMX_IndexParamPortDefinition:
        {

            pPortDefs = (OMX_PARAM_PORTDEFINITIONTYPE *) pParamStruct;

            OMX_BASE_CHK_VERSION(pParamStruct, OMX_PARAM_PORTDEFINITIONTYPE,
                                 eError);
            if (pPortDefs->nPortIndex ==
                pAudEncComPvt->pPortdefs[OMX_AUDENC_OUTPUT_PORT]->nPortIndex) {

                if (pAudEncComPvt->pPortdefs[OMX_AUDENC_OUTPUT_PORT]->
                    nBufferCountActual != pPortDefs->nBufferCountActual) {

                    pAudEncComPvt->bOutBufferCountChange = OMX_TRUE;
                }

                pAudEncComPvt->pPortdefs[OMX_AUDENC_OUTPUT_PORT]->
                    nBufferCountActual = pPortDefs->nBufferCountActual;

                if(pPortDefs->format.audio.eEncoding !=
                    pAudEncComPvt->pPortdefs[OMX_AUDENC_OUTPUT_PORT]->format.audio.eEncoding) {
                	memcpy(&pAudEncComPvt->pPortdefs[OMX_AUDENC_OUTPUT_PORT]->format,
                			&pPortDefs->format, sizeof(OMX_AUDIO_PARAM_PORTFORMATTYPE));
                OMX_TI_AUDENC_InitCallbacks(pHandle,
                                           pPortDefs->format.audio.
                                           eEncoding);
                }
				/*calculate the buffersize based on codec structure values*/
				/*default value for AAC LC encoder*/
                  
               pAudEncComPvt->pPortdefs[OMX_AUDENC_OUTPUT_PORT]->nBufferSize = AAC_DEFAULT_OUTBUF_SIZE;


            } else if (pPortDefs->nPortIndex ==
                       pAudEncComPvt->pPortdefs[OMX_AUDENC_INPUT_PORT]->
                       nPortIndex) {
			    OMX_U32 nChannels = 0;

                if (pAudEncComPvt->pPortdefs[OMX_AUDENC_INPUT_PORT]->
                    nBufferCountActual != pPortDefs->nBufferCountActual) {

                    pAudEncComPvt->bOutBufferCountChange = OMX_TRUE;
                }

                    pAudEncComPvt->pPortdefs[OMX_AUDENC_INPUT_PORT]->
                    nBufferCountActual = pPortDefs->nBufferCountActual;


                pAudEncComPvt->pPortdefs[OMX_AUDENC_INPUT_PORT]->format =
                    pPortDefs->format;

                pAudEncComPvt->fpSet_StaticParams(hComponent,
                                                  ((IAUDENC1_Params *)pAudEncComPvt->pEncStaticParams));
                /*Set the buffersize after the default static params have been set*/
                nChannels = OMX_TI_CECHANNELS_TO_NCHANNELS((IAUDENC1_Params *)pAudEncComPvt->pEncStaticParams);

		pAudEncComPvt->pPortdefs[OMX_AUDENC_OUTPUT_PORT]->nBufferSize = nChannels * AAC_DEFAULT_SAMPLES_IN_A_FRAME *
		          ( ((IAUDENC1_Params *)pAudEncComPvt->pEncStaticParams)->inputBitsPerSample>>3);
            } else {
                eError = OMX_ErrorBadPortIndex;
            }
        }
        break;

        /* client uses this to modify the format type of an port */
    case OMX_IndexParamAudioInit:
        {
            OMX_BASE_CHK_VERSION(pParamStruct, OMX_PORT_PARAM_TYPE, eError);
            pAudEncComPvt->tDerToBase.pAudioPortParams->nPorts =
                ((OMX_PORT_PARAM_TYPE *) pParamStruct)->nPorts;
        }
        break;

    case OMX_IndexParamAudioPortFormat:
        {
		    OMX_AUDIO_PARAM_PORTFORMATTYPE *pAudioPortFormat;
            OMX_BASE_CHK_VERSION(pParamStruct, OMX_AUDIO_PARAM_PORTFORMATTYPE,
                                 eError);

            pAudioPortFormat = (OMX_AUDIO_PARAM_PORTFORMATTYPE *) pParamStruct;
                 
            if (pAudioPortFormat->nPortIndex ==
                pAudEncComPvt->pPortdefs[OMX_AUDENC_INPUT_PORT]->nPortIndex) {
                  if(pAudioPortFormat->eEncoding != OMX_AUDIO_CodingUnused){
                      eError = OMX_ErrorBadParameter;
                      break;
                  }              				

            } 
            else if (pAudioPortFormat->nPortIndex ==
                       pAudEncComPvt->pPortdefs[OMX_AUDENC_OUTPUT_PORT]->
                       nPortIndex) {
			    OMX_U32 nChannels;
                pAudEncComPvt->pPortdefs[OMX_AUDENC_OUTPUT_PORT]->format.audio.
                    eEncoding = pAudioPortFormat->eEncoding;
                OMX_TI_AUDENC_InitCallbacks(hComponent, pAudioPortFormat->eEncoding);
				/*Set the static parameters*/
                pAudEncComPvt->fpSet_StaticParams(hComponent,
                                                  ((IAUDENC1_Params *)pAudEncComPvt->pEncStaticParams));
		        /*Set the port definition buffersizes*/
               pAudEncComPvt->pPortdefs[OMX_AUDENC_OUTPUT_PORT]->nBufferSize = AAC_DEFAULT_OUTBUF_SIZE;
               nChannels = OMX_TI_CECHANNELS_TO_NCHANNELS( (IAUDENC1_Params *)pAudEncComPvt->pEncStaticParams);
               pAudEncComPvt->pPortdefs[OMX_AUDENC_INPUT_PORT]->nBufferSize = nChannels * AAC_DEFAULT_SAMPLES_IN_A_FRAME *
	            ( ((IAUDENC1_Params *)pAudEncComPvt->pEncStaticParams)->inputBitsPerSample>>3);

            } 
            else {
                eError = OMX_ErrorBadPortIndex;
            }
        }
        break;
    case OMX_IndexParamAudioPcm:
        {
		    OMX_AUDIO_PARAM_PCMMODETYPE *pPcmMode;
            OMX_BASE_CHK_VERSION(pParamStruct, OMX_AUDIO_PARAM_PCMMODETYPE, eError);
            pPcmMode = (OMX_AUDIO_PARAM_PCMMODETYPE *)pParamStruct;
            /*only configure input port*/
            if(pPcmMode->nPortIndex == pAudEncComPvt->
                pPortdefs[OMX_AUDENC_INPUT_PORT]->nPortIndex){
                     ((IAUDENC1_Params *)pAudEncComPvt->pEncStaticParams)->lfeFlag = 0;
                     OMX_TI_NCHANNELS_TO_CECHANNELS(((IAUDENC1_Params *)pAudEncComPvt->pEncStaticParams),pPcmMode->nChannels);

                     ((IAUDENC1_Params *)pAudEncComPvt->pEncStaticParams)->sampleRate = pPcmMode->nSamplingRate ;

                     pAudEncComPvt->pPortdefs[OMX_AUDENC_INPUT_PORT]->nBufferSize = pPcmMode->nChannels * AAC_DEFAULT_SAMPLES_IN_A_FRAME *
		         ( ((IAUDENC1_Params *)pAudEncComPvt->pEncStaticParams)->inputBitsPerSample>>3);


	    }
            else {
                eError = OMX_ErrorBadPortIndex;
            }


	}
	break;
    case OMX_IndexParamAudioAac:
        {
            /*only configure output port*/
 
            OMX_AUDIO_PARAM_AACPROFILETYPE * pAACProfile;
            OMX_BASE_CHK_VERSION(pParamStruct, OMX_AUDIO_PARAM_AACPROFILETYPE, eError);
            pAACProfile =( OMX_AUDIO_PARAM_AACPROFILETYPE *)pParamStruct;
            /*we use the bitrate and transmode*/
            if(pAACProfile->nPortIndex == pAudEncComPvt->
                pPortdefs[OMX_AUDENC_OUTPUT_PORT]->nPortIndex){
                ((IAUDENC1_Params *)pAudEncComPvt->pEncStaticParams)->bitRate =
                       pAACProfile->nBitRate;
#ifdef CODEC_AACENC
                ((IMP4AACENC_Params *)pAudEncComPvt->pEncStaticParams)->transMode =
                       (IMP4AACENC_TRANSPORT_TYPE) OMX_TI_OMX_SF_TO_AE_SF(pAACProfile->eAACStreamFormat);
#endif
            }
            else {
                eError = OMX_ErrorBadPortIndex;
            }

        }
        break;

    case OMX_TI_IndexParamFileModeType:
        {
            OMX_PARAM_FILEMODE_TYPE * pFileModeCfg;

            OMX_BASE_CHK_VERSION(pParamStruct, OMX_PARAM_FILEMODE_TYPE, eError);

            pFileModeCfg = (OMX_PARAM_FILEMODE_TYPE *) pParamStruct;

            /* reset the numBytesConsumed counter on transition to file mode operation */
            if (pFileModeCfg->bEnableFileMode) {
                if (OMX_FALSE == pAudEncComPvt->bFileMode) {
                    pAudEncComPvt->numBytesConsumed = 0;
                }
            }
            pAudEncComPvt->bFileMode = pFileModeCfg->bEnableFileMode;
        }
        break;

    case OMX_TI_IndexParamCompPortNotifyType:
        {
            pNotifyType =
                (OMX_PARAM_COMPPORT_NOTIFYTYPE *) pParamStruct;
            pAudEncComPvt->tDerToBase.pPortProperties[pNotifyType->
                                                      nPortIndex]->eNotifyType =
                pNotifyType->eNotifyType;
            eError = OMX_ErrorNone;
            break;
        }

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
 *  @func  OMX_TI_AUDENC_SetConfig()
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
static OMX_ERRORTYPE OMX_TI_AUDENC_SetConfig(OMX_HANDLETYPE hComponent,
                                             OMX_INDEXTYPE nIndex,
                                             OMX_PTR pConfigData)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;

    OMX_AUDENC_PVTTYPE *pAudEncComPvt = NULL;

    OMX_COMPONENTTYPE *pHandle;

    OMX_BASE_REQUIRE((hComponent != NULL)
                     && (pConfigData != NULL), OMX_ErrorBadParameter);

    pHandle = (OMX_COMPONENTTYPE *) hComponent;
    pAudEncComPvt = (OMX_AUDENC_PVTTYPE *) pHandle->pComponentPrivate;

    /* SetConfig can't be invoked when the comp is in Invalid state */

    OMX_BASE_ASSERT(pAudEncComPvt->tCurState != OMX_StateInvalid,
                    OMX_ErrorIncorrectStateOperation);

    /* Take care of Supported Indexes over here  */
    switch (nIndex) {
    // SP: add support for Audio
    default:
        eError = OMX_BASE_SetConfig(hComponent, nIndex, pConfigData);
    }

  EXIT:
    return eError;
}

/* ========================================================================== */

/**
********************************************************************************
 *  @func  OMX_TI_AUDENC_GetConfig()
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

static OMX_ERRORTYPE OMX_TI_AUDENC_GetConfig(OMX_HANDLETYPE hComponent,
                                             OMX_INDEXTYPE nIndex,
                                             OMX_PTR pConfigData)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;

    OMX_AUDENC_PVTTYPE *pAudEncComPvt = NULL;

    OMX_COMPONENTTYPE *pHandle;

    OMX_BASE_REQUIRE((hComponent != NULL) &&
                     (pConfigData != NULL), OMX_ErrorBadParameter);

    pHandle = (OMX_COMPONENTTYPE *) hComponent;
    pAudEncComPvt = (OMX_AUDENC_PVTTYPE *) pHandle->pComponentPrivate;

    /* GetConfig can't be invoked when the comp is in Invalid state */

    OMX_BASE_REQUIRE(pAudEncComPvt->tCurState != OMX_StateInvalid,
                     OMX_ErrorIncorrectStateOperation);

    switch (nIndex) {
//SP: TBD

    default:
        /* Process if supported by base component */
        eError = OMX_BASE_SetConfig(hComponent, nIndex, pConfigData);
    }

  EXIT:
    return eError;
}

/* ========================================================================== */

/**
********************************************************************************
 *  @func  OMX_TI_AUDENC_CommandNotify()
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
static OMX_ERRORTYPE OMX_TI_AUDENC_CommandNotify(OMX_HANDLETYPE hComponent,
                                                 OMX_COMMANDTYPE Cmd,
                                                 OMX_U32 nParam,
                                                 OMX_PTR pCmdData)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;

    OMX_AUDENC_PVTTYPE *pAudEncComPvt = NULL;

    OMX_COMPONENTTYPE *pHandle;

    OMX_BUFFERHEADERTYPE *pInBufHeader = NULL;

    XDAS_Int32 status = 0, i;

    OMX_BOOL bCodecCreate = OMX_FALSE;

    OMX_U32 nInMsgCount = 0, nBufferCountActual;

//    OMX_PTR codecPtr;
	OMX_U32 nIndex;

    OMX_BASE_REQUIRE((hComponent != NULL), OMX_ErrorBadParameter);

    pHandle = (OMX_COMPONENTTYPE *) hComponent;
    pAudEncComPvt = (OMX_AUDENC_PVTTYPE *) pHandle->pComponentPrivate;


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
        if ((pAudEncComPvt->tCurState == OMX_StateLoaded) &&
            (pAudEncComPvt->tNewState == OMX_StateIdle)) {

            /* allocate and initialize the encoder on the codec Engine,
               set the dynamic parameters  */

            if (pAudEncComPvt->encHandle == NULL) {
                pAudEncComPvt->encHandle = AUDENC1_create(pAudEncComPvt->ce, pAudEncComPvt->encoderName,
                                     ((IAUDENC1_Params *)pAudEncComPvt->pEncStaticParams));
	   // SP: Codec engine API

	        if (pAudEncComPvt->encHandle == NULL) {
                    OMX_BASE_Error("AUDENC1_create failed\n");
                    eError = OMX_ErrorInsufficientResources;
                    goto EXIT;
                }
                pAudEncComPvt->bCodecCreate = OMX_FALSE;

                pAudEncComPvt->fpSet_DynamicParams(pAudEncComPvt->pEncDynParams);

                /* call control to get buffer info */

                status = AUDENC1_control(pAudEncComPvt->encHandle,                 // SP: Codec engine API
                                         XDM_GETBUFINFO,
                                         (AUDENC1_DynamicParams *)pAudEncComPvt->pEncDynParams,
                                         (AUDENC1_Status *)
                                         pAudEncComPvt->pEncStatus);

                if (status != AUDENC1_EOK) {
                    OMX_BASE_Error("AUDENC1_control XDM_GETBUFINFO failed\n");
                    eError = OMX_ErrorInsufficientResources;
                    goto EXIT;
                }

                for (i = 0; i < OMX_AUDENC_MAX_INPUT_BUFFER_COUNT; i++) {
                    pAudEncComPvt->inputBufDesc[i].numBufs =
                        ((IAUDENC1_Status *) (pAudEncComPvt->pEncStatus))->bufInfo.
                        minNumInBufs;
                }
                for (i = 0; i < OMX_AUDENC_MAX_OUTPUT_BUFFER_COUNT; i++) {
                    pAudEncComPvt->outputBufDesc[i].numBufs =
                        ((IAUDENC1_Status *) (pAudEncComPvt->pEncStatus))->bufInfo.
                        minNumOutBufs;
                }
            }

            if (pAudEncComPvt->pPortdefs[OMX_AUDENC_INPUT_PORT]->bEnabled) {
                nBufferCountActual =
                pAudEncComPvt->pPortdefs[OMX_AUDENC_INPUT_PORT]->nBufferCountActual;

                if(pAudEncComPvt->inBufHeaderArray == NULL) {
                    pAudEncComPvt->inBufHeaderArray = (OMX_AUDENC_buf_header *)
                        TIMM_OSAL_Malloc((sizeof(OMX_AUDENC_buf_header)
                              * nBufferCountActual), TIMM_OSAL_TRUE, 0,
                              TIMMOSAL_MEM_SEGMENT_EXT);
                }

                OMX_BASE_ASSERT(pAudEncComPvt->inBufHeaderArray != NULL,
                            OMX_ErrorInsufficientResources);

                TIMM_OSAL_Memset(pAudEncComPvt->inBufHeaderArray, 0x0,
                                 ((sizeof(OMX_AUDENC_buf_header) *
                                   (nBufferCountActual))));

                for (i = 0; i < nBufferCountActual; i++) {
                   pAudEncComPvt->inBufHeaderArray[i].pBufHeader = NULL;
                   pAudEncComPvt->inBufHeaderArray[i].bufStatus =  OMX_BUFFER_FREE;
                }
            }

            if (pAudEncComPvt->pPortdefs[OMX_AUDENC_OUTPUT_PORT]->bEnabled) {
                nBufferCountActual =
                    pAudEncComPvt->pPortdefs[OMX_AUDENC_OUTPUT_PORT]->nBufferCountActual;

                if(pAudEncComPvt->outBufHeaderArray == NULL) {
                   pAudEncComPvt->outBufHeaderArray = (OMX_AUDENC_buf_header *)
                   TIMM_OSAL_Malloc((sizeof(OMX_AUDENC_buf_header)
                              * nBufferCountActual), TIMM_OSAL_TRUE, 0,
                              TIMMOSAL_MEM_SEGMENT_EXT);
                }

                OMX_BASE_ASSERT(pAudEncComPvt->outBufHeaderArray != NULL,
                            OMX_ErrorInsufficientResources);

                TIMM_OSAL_Memset(pAudEncComPvt->outBufHeaderArray, 0x0,
                                 ((sizeof(OMX_AUDENC_buf_header) *
                                   (nBufferCountActual))));

                for (i = 0; i < nBufferCountActual; i++) {
                    pAudEncComPvt->outBufHeaderArray[i].pBufHeader = NULL;
                    pAudEncComPvt->outBufHeaderArray[i].bufStatus =  OMX_BUFFER_FREE;
                }
            }


        }
        /* Incase if the comp is moving from idle to executing, process buffers
           if an supplier port; base would change state */

        else if ((pAudEncComPvt->tCurState == OMX_StateIdle) &&
                 (pAudEncComPvt->tNewState == OMX_StateExecuting)) {
          /* When ComponentInit is done, this gets initialized to 0. However
             on doing a Port Disable/Exec-Idle transition, this changes to RESET_DONE,
             and remains so when doing a Port Enable/Idle-Exec transition.
             This causes ProcessDone to return error.
             So re-initializing to 0 to avoid false alarm in ProcessDone */
          pAudEncComPvt->cmdStatus = 0;

        }

        /* Incase If the comp is moving to Idle from executing, return all the
           buffers back to the allocator */

        else if (((pAudEncComPvt->tCurState == OMX_StateExecuting)
                  && (pAudEncComPvt->tNewState == OMX_StateIdle))
                 || ((pAudEncComPvt->tCurState == OMX_StatePause)
                     && (pAudEncComPvt->tNewState == OMX_StateIdle))) {

            /* Free all the input buffer */
            pAudEncComPvt->fpDioGetCount(hComponent, OMX_AUDENC_INPUT_PORT,
                                         (OMX_PTR) & nInMsgCount);
            while (nInMsgCount > 0) {
                pAudEncComPvt->fpDioDequeue(hComponent, OMX_AUDENC_INPUT_PORT,
                                            (OMX_PTR) (pInBufHeader));
                pAudEncComPvt->fpDioSend(hComponent, OMX_AUDENC_INPUT_PORT,
                                         pInBufHeader);
                nInMsgCount--;
            }
            /* command complete will be posted to base after reset is executed */
        }

        /* State transition from pause to executing state; pauss is not
           supported */

        else if ((pAudEncComPvt->tCurState == OMX_StatePause) &&
                 (pAudEncComPvt->tNewState == OMX_StateExecuting)) {

        }

        else if ((pAudEncComPvt->tCurState == OMX_StateExecuting) &&
                 (pAudEncComPvt->tNewState == OMX_StatePause)) {
        }

        else if ((pAudEncComPvt->tCurState == OMX_StateIdle) &&
                 (pAudEncComPvt->tNewState == OMX_StateLoaded)) {

            /* Delete all the Resources allocated in Create Stage tear down the
               codec and Engine */

            AUDENC1_delete(pAudEncComPvt->encHandle);
            pAudEncComPvt->encHandle = NULL;

            pAudEncComPvt->numBytesConsumed = 0;
            for (i = 0; i < OMX_AUDENC_MAX_INPUT_BUFFER_COUNT; i++) {
                pAudEncComPvt->encInArgsPtr = &(pAudEncComPvt->encInArgs[i]);
                //pAudEncComPvt->encInArgsPtr->audenc1InArgs.numBytes = 0;
            }
            for (i = 0; i < OMX_AUDENC_MAX_OUTPUT_BUFFER_COUNT; i++) {
                pAudEncComPvt->encOutArgsPtr = &(pAudEncComPvt->encOutArgs[i]);
                //pAudEncComPvt->encOutArgsPtr->audenc1OutArgs.bytesConsumed = 0;
            }

            /* free up memory allocated for buffer headers */
            for (nIndex = 0; nIndex <
                 pAudEncComPvt->pPortdefs[OMX_AUDENC_INPUT_PORT]->nBufferCountActual;
                 nIndex++) {
                if(pAudEncComPvt->inBufHeaderArray[nIndex].pBufHeader != NULL)
                TIMM_OSAL_Free(pAudEncComPvt->inBufHeaderArray[nIndex].pBufHeader);
            }

            for (nIndex = 0; nIndex <
                 pAudEncComPvt->pPortdefs[OMX_AUDENC_OUTPUT_PORT]->nBufferCountActual;
                 nIndex++) {
                if(pAudEncComPvt->outBufHeaderArray[nIndex].pBufHeader != NULL)
                TIMM_OSAL_Free(pAudEncComPvt->outBufHeaderArray[nIndex].pBufHeader);
            }
            
            if(pAudEncComPvt->inBufHeaderArray != NULL) {
               TIMM_OSAL_Free(pAudEncComPvt->inBufHeaderArray);
               pAudEncComPvt->inBufHeaderArray = NULL;
            }

            if(pAudEncComPvt->outBufHeaderArray != NULL) {
              TIMM_OSAL_Free(pAudEncComPvt->outBufHeaderArray);
              pAudEncComPvt->outBufHeaderArray = NULL;
            }
        }
        /* If comp moves from states(Idle,Executing,Pause) to invalid state
           Delete all the Resources allocated in Create Stage, tear down the
           codec and Engine */

        else if ((pAudEncComPvt->tCurState == OMX_StateIdle
                  || pAudEncComPvt->tCurState == OMX_StateExecuting
                  || pAudEncComPvt->tCurState == OMX_StatePause)
                 && (pAudEncComPvt->tNewState == OMX_StateInvalid)) {
            pAudEncComPvt->tCurState = pAudEncComPvt->tNewState;
            AUDENC1_delete(pAudEncComPvt->encHandle);
        }
        /* if same state movement, retun error */
        else if ( pAudEncComPvt->tCurState == pAudEncComPvt->tNewState) {
            eError = OMX_ErrorSameState;
            }
        break;

    case OMX_CommandPortDisable:

        if(pAudEncComPvt->tCurState != OMX_StateLoaded) {
        /* Make sure nParam corresponds to a valid port */
        OMX_BASE_ASSERT(((nParam == OMX_AUDENC_OUTPUT_PORT)
                         || (nParam == OMX_AUDENC_INPUT_PORT)
                         || (nParam == OMX_ALL)), OMX_ErrorBadParameter);

             /* for o/p port disable, only flag is set as false in base */
             if ((nParam == OMX_AUDENC_INPUT_PORT) || (nParam == OMX_ALL)) {
                 /* Free all the input buffer */
                 pAudEncComPvt->fpDioGetCount(hComponent, OMX_AUDENC_INPUT_PORT,
                                     (OMX_PTR) & nInMsgCount);
                 while (nInMsgCount > 0) {
                    pAudEncComPvt->fpDioDequeue(hComponent, OMX_AUDENC_INPUT_PORT,
                                        (OMX_PTR) (pInBufHeader));
                    pAudEncComPvt->fpDioSend(hComponent, OMX_AUDENC_INPUT_PORT,
                                     pInBufHeader);
                    nInMsgCount--;
                 }
             }

             if(pAudEncComPvt->tCurState == OMX_StateExecuting) {
             /* Set the command id as XDM_RESET, as this would be done in process
                thread context */
                pAudEncComPvt->cmdXdm = XDM_RESET;

                goto EXIT;
             }
        }

        break;

    case OMX_CommandPortEnable:
        /* allocate and initialize the encoder on the Engine; If After port
           disable, format is changed it should have been deleted; and this
           command would result in codec creation, setting dynamic parameters,
           and getting the buffer information from codec  */
        /* base is taking care of allocating all the resources */
        /* nothing needs to be done here actually              */
        OMX_BASE_ASSERT(((nParam == OMX_AUDENC_OUTPUT_PORT)
                         || (nParam == OMX_AUDENC_INPUT_PORT)
                         || (nParam == OMX_ALL)), OMX_ErrorBadParameter);

        /* When ComponentInit is done, this gets initialized to 0. However
           on doing a Port Disable/Exec-Idle transition, this changes to RESET_DONE,
           and remains so when doing a Port Enable/Idle-Exec transition.
           This causes ProcessDone to return error.
           So re-initializing to 0 to avoid false alarm in ProcessDone */
        pAudEncComPvt->cmdStatus = 0;
        /* OMX_SetParameter sets bCodecCreate to signal change in Create time
           parameters. Therefore, the codec needs to be deleted and re-created
           here as part of PortEnable (also done in Loaded to Idle)
        */
        TIMM_OSAL_MutexObtain(pAudEncComPvt->pMutex, TIMM_OSAL_SUSPEND);
        bCodecCreate = pAudEncComPvt->bCodecCreate;
        TIMM_OSAL_MutexRelease(pAudEncComPvt->pMutex);

        if ((bCodecCreate == OMX_TRUE) && (pAudEncComPvt->tCurState != OMX_StateLoaded)) {
           /* free up memory allocated for buffer headers */
           if (pAudEncComPvt->encHandle) {
               AUDENC1_delete(pAudEncComPvt->encHandle);
               pAudEncComPvt->encHandle = NULL;
           }
                /* create new codec instance */
		pAudEncComPvt->encHandle =
                    AUDENC1_create(pAudEncComPvt->ce, pAudEncComPvt->encoderName,
                                   ((IAUDENC1_Params *)pAudEncComPvt->pEncStaticParams));
                if (pAudEncComPvt->encHandle == NULL) {
                    OMX_BASE_Error("AUDENC1_create failed\n");
                    eError = OMX_ErrorInsufficientResources;
                    goto EXIT;
                }
                pAudEncComPvt->fpSet_DynamicParams(pAudEncComPvt->pEncDynParams);

                status = AUDENC1_control(pAudEncComPvt->encHandle,
                                         XDM_GETBUFINFO,
                                         pAudEncComPvt->pEncDynParams,
                                         (AUDENC1_Status *)
                                         pAudEncComPvt->pEncStatus);

                if (status != AUDENC1_EOK) {
                    OMX_BASE_Error
                        ("AUDENC1_control XDM_GETBUFINFO failed ....! \n");
                    eError = OMX_ErrorInsufficientResources;
                    goto EXIT;
                }

                for (i = 0; i < OMX_AUDENC_MAX_INPUT_BUFFER_COUNT; i++) {
                    pAudEncComPvt->inputBufDesc[i].numBufs =
                        ((IAUDENC1_Status *) (pAudEncComPvt->pEncStatus))->bufInfo.
                        minNumInBufs;
                }
                for (i = 0; i < OMX_AUDENC_MAX_OUTPUT_BUFFER_COUNT; i++) {
                    pAudEncComPvt->outputBufDesc[i].numBufs =
                        ((IAUDENC1_Status *) (pAudEncComPvt->pEncStatus))->bufInfo.
                        minNumOutBufs;
                }

                /* Reset the Codeccreate flag & variable */
                pAudEncComPvt->bCodecCreate = OMX_FALSE;
        } /* end if bCodecCreate & not Loaded */

           /* If ports were enabled, it would have been created in loaded to idle
              transition. If not nBufferCountactual can change while port disabled */

        if(pAudEncComPvt->bInBufferCountChange == OMX_TRUE) {
            if (pAudEncComPvt->inBufHeaderArray != NULL) {
               TIMM_OSAL_Free(pAudEncComPvt->inBufHeaderArray);
               pAudEncComPvt->inBufHeaderArray = NULL;
            }
        }
        if(pAudEncComPvt->bOutBufferCountChange == OMX_TRUE) {
            if (pAudEncComPvt->outBufHeaderArray != NULL) {
               TIMM_OSAL_Free(pAudEncComPvt->outBufHeaderArray);
               pAudEncComPvt->outBufHeaderArray = NULL;
            }
        }

        if (pAudEncComPvt->inBufHeaderArray == NULL) {
             nBufferCountActual =
                pAudEncComPvt->pPortdefs[OMX_AUDENC_INPUT_PORT]->nBufferCountActual;

             pAudEncComPvt->inBufHeaderArray = (OMX_AUDENC_buf_header *)
                 TIMM_OSAL_Malloc((sizeof(OMX_AUDENC_buf_header)
                                  * nBufferCountActual), TIMM_OSAL_TRUE, 0,
                                  TIMMOSAL_MEM_SEGMENT_EXT);

             OMX_BASE_ASSERT(pAudEncComPvt->inBufHeaderArray != NULL,
                                OMX_ErrorInsufficientResources);

             TIMM_OSAL_Memset(pAudEncComPvt->inBufHeaderArray, 0x0,
                                   ((sizeof(OMX_AUDENC_buf_header) *
                                     (nBufferCountActual))));

             for (i = 0; i < nBufferCountActual; i++) {
                 pAudEncComPvt->inBufHeaderArray[i].pBufHeader = NULL;
                 pAudEncComPvt->inBufHeaderArray[i].bufStatus =  OMX_BUFFER_FREE;
              }
         }
         if (pAudEncComPvt->outBufHeaderArray == NULL) {
             nBufferCountActual =
                pAudEncComPvt->pPortdefs[OMX_AUDENC_OUTPUT_PORT]->nBufferCountActual;

             pAudEncComPvt->outBufHeaderArray = (OMX_AUDENC_buf_header *)
             TIMM_OSAL_Malloc((sizeof(OMX_AUDENC_buf_header)
                                  * nBufferCountActual), TIMM_OSAL_TRUE, 0,
                                  TIMMOSAL_MEM_SEGMENT_EXT);

             OMX_BASE_ASSERT(pAudEncComPvt->outBufHeaderArray != NULL,
                              OMX_ErrorInsufficientResources);

             TIMM_OSAL_Memset(pAudEncComPvt->outBufHeaderArray, 0x0,
                                   ((sizeof(OMX_AUDENC_buf_header) *
                                       (nBufferCountActual))));

             for (i = 0; i < nBufferCountActual; i++) {
                 pAudEncComPvt->outBufHeaderArray[i].pBufHeader = NULL;
                 pAudEncComPvt->outBufHeaderArray[i].bufStatus =  OMX_BUFFER_FREE;
             }
         }
         /* clear inargs and outArgs arrays */
         for (i = 0; i < OMX_AUDENC_MAX_INPUT_BUFFER_COUNT; i++) {
             pAudEncComPvt->encInArgsPtr = &(pAudEncComPvt->encInArgs[i]);
             TIMM_OSAL_Memset(pAudEncComPvt->encInArgsPtr, 0x0,
                                  ((sizeof(AUDENC1_InArgs)
                                       )));
             pAudEncComPvt->encInArgsPtr->size =
                    sizeof(IAUDENC1_InArgs);

         }
         for (i = 0; i < OMX_AUDENC_MAX_OUTPUT_BUFFER_COUNT; i++) {
             pAudEncComPvt->encOutArgsPtr = &(pAudEncComPvt->encOutArgs[i]);
             TIMM_OSAL_Memset(pAudEncComPvt->encOutArgsPtr, 0x0,
                                     ((sizeof(AUDENC1_OutArgs)
                                       )));
             pAudEncComPvt->encOutArgsPtr->size =
                    sizeof(IAUDENC1_OutArgs);
         }
         pAudEncComPvt->numBytesConsumed = 0;

        break;

    case OMX_CommandFlush:
             /* omx flush (not codec flush), is similar to codec reset case;
                Free all the input buffer, set status as reset in server */

            pAudEncComPvt->fpDioGetCount(hComponent, OMX_AUDENC_INPUT_PORT,
                                         (OMX_PTR) & nInMsgCount);
            while (nInMsgCount > 0) {
                pAudEncComPvt->fpDioDequeue(hComponent, OMX_AUDENC_INPUT_PORT,
                                            (OMX_PTR) (pInBufHeader));
                pAudEncComPvt->fpDioSend(hComponent, OMX_AUDENC_INPUT_PORT,
                                         pInBufHeader);
                nInMsgCount--;
            }
            /* Set the command id as XDM_RESET, as this would be done in server
               thread context */

            pAudEncComPvt->cmdXdm = XDM_RESET;

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
        pAudEncComPvt->fpReturnEventNotify(hComponent, OMX_EventCmdComplete,
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

/* ========================================================================== */


void OMX_TI_AUDENC_UTL_CancelBuffer(void *hComponent, void * pOutBufHeaderPtr)
{
    OMX_COMPONENTTYPE *pHandle;
    OMX_AUDENC_PVTTYPE *pAudEncComPvt;
    OMX_BUFFERHEADERTYPE *pOutBufHeader = NULL;

    pHandle = (OMX_COMPONENTTYPE *) hComponent;

    pAudEncComPvt = (OMX_AUDENC_PVTTYPE *) pHandle->pComponentPrivate;

    pOutBufHeader = (OMX_BUFFERHEADERTYPE *) pOutBufHeaderPtr;

    pAudEncComPvt->fpDioCancel(hComponent,
                              OMX_AUDENC_OUTPUT_PORT,
                              pOutBufHeader);

}

void OMX_TI_AUDENC_UTL_GetNewBuffer(void *hComponent, void * pOutBufHeaderPtr,
                                    void * pOutBufDescPtr)
{
    OMX_COMPONENTTYPE *pHandle;
    OMX_AUDENC_PVTTYPE *pAudEncComPvt;
    OMX_BUFFERHEADERTYPE *pOutBufHeader = NULL;
    XDM1_BufDesc            *pOutputBufDescPtr;
    int i;

    pHandle = (OMX_COMPONENTTYPE *) hComponent;

    pAudEncComPvt = (OMX_AUDENC_PVTTYPE *) pHandle->pComponentPrivate;

    pOutBufHeader = (OMX_BUFFERHEADERTYPE *) pOutBufHeaderPtr;

    pOutputBufDescPtr = (XDM1_BufDesc *)pOutBufDescPtr;

    pAudEncComPvt->fpDioDequeue(hComponent, OMX_AUDENC_OUTPUT_PORT,
                                (OMX_PTR) (pOutBufHeader));
    for (i = 0; i < pOutputBufDescPtr->numBufs; i++) {
        pOutputBufDescPtr->descs[i].buf = (XDAS_Int8 *) pOutBufHeader->pBuffer ;
        Assert_isTrue((pOutputBufDescPtr->numBufs >= 1),
                      Assert_E_assertFailed);
    }
}


/**
********************************************************************************
 *  @func   OMX_TI_AUDENC_DataNotify()
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
static OMX_ERRORTYPE OMX_TI_AUDENC_DataNotify(OMX_HANDLETYPE hComponent)
{

    OMX_ERRORTYPE eError = OMX_ErrorNone;

    OMX_AUDENC_PVTTYPE *pAudEncComPvt;

    OMX_COMPONENTTYPE *pComp;

    OMX_BUFFERHEADERTYPE *pInBufHeader = NULL;

    OMX_BUFFERHEADERTYPE *pOutBufHeader = NULL;


    OMX_U16 inEOSReceived = 0;

    Int32 status;

    OMX_U32 nInMsgCount = 0, nBufferCountActual;
    
    OMX_U32 nOutMsgCount = 0, nCurBufEncodeOffset = 0;

    OMX_U32 bufStatus;

    OMX_STATETYPE tCurState;

    OMX_BASE_REQUIRE((hComponent != NULL), OMX_ErrorBadParameter);

    pComp = (OMX_COMPONENTTYPE *) hComponent;
    pAudEncComPvt = (OMX_AUDENC_PVTTYPE *) pComp->pComponentPrivate;

    /* Start buffer processing only when the comp is in Executing state and the
       Port are Enabled */
    TIMM_OSAL_MutexObtain(pAudEncComPvt->pDataNotifyMutex, TIMM_OSAL_SUSPEND);
    tCurState = pAudEncComPvt->tCurState;
    TIMM_OSAL_MutexRelease(pAudEncComPvt->pDataNotifyMutex);


    /* if there is reset command pending/done, do not take new data to process;
       there could be data event pending, which should be ignored */

    if (pAudEncComPvt->cmdStatus == RESET_DONE)
        goto EXIT;

    /* Start buffer processing only when the comp is in Executing state */

    if ((tCurState == OMX_StateExecuting)
        && (pAudEncComPvt->pPortdefs[OMX_AUDENC_INPUT_PORT]->bEnabled)
        && (pAudEncComPvt->pPortdefs[OMX_AUDENC_OUTPUT_PORT]->bEnabled)) {

        /* check for both input and output */
        pAudEncComPvt->fpDioGetCount(hComponent, OMX_AUDENC_INPUT_PORT,
                                     (OMX_PTR) & nInMsgCount);
        nOutMsgCount = 0;
        /* Check the output buffers only if input buffers are available */
        if ( nInMsgCount ) {
          pAudEncComPvt->fpDioGetCount(hComponent, OMX_AUDENC_OUTPUT_PORT,
                                       (OMX_PTR) & nOutMsgCount);
        }
        /* if both are ready then only  process data */
        if ((nInMsgCount > 0) && (nOutMsgCount > 0)) {
            /* allocate space for buffer headers if it is not allocated already, It will
               be done only once and no run time allocation would be done post this  */

            pInBufHeader =
                pAudEncComPvt->inBufHeaderArray[pAudEncComPvt->inBufArrayIndx].pBufHeader;

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
                pAudEncComPvt->inBufHeaderArray[pAudEncComPvt->inBufArrayIndx].
                    pBufHeader = pInBufHeader;
            } //if (pInBufHeader == NULL)

            nBufferCountActual =
            pAudEncComPvt->pPortdefs[OMX_AUDENC_INPUT_PORT]->nBufferCountActual;

            pAudEncComPvt->inBufArrayIndx++;

            if (pAudEncComPvt->inBufArrayIndx >= nBufferCountActual)
             pAudEncComPvt->inBufArrayIndx = 0;

            pOutBufHeader =
                pAudEncComPvt->outBufHeaderArray[pAudEncComPvt->outBufArrayIndx].
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
                pAudEncComPvt->outBufHeaderArray[pAudEncComPvt->outBufArrayIndx].
                    pBufHeader = pOutBufHeader;
            } //if (pOutBufHeader == NULL)

            nBufferCountActual =
            pAudEncComPvt->pPortdefs[OMX_AUDENC_OUTPUT_PORT]->nBufferCountActual;

            pAudEncComPvt->outBufArrayIndx++;

            if (pAudEncComPvt->outBufArrayIndx >= nBufferCountActual)
             pAudEncComPvt->outBufArrayIndx = 0;
            pAudEncComPvt->fpDioDequeue(hComponent, OMX_AUDENC_INPUT_PORT,
                                        (OMX_PTR) (pInBufHeader));

            pAudEncComPvt->fpDioDequeue(hComponent, OMX_AUDENC_OUTPUT_PORT,
                                        (OMX_PTR) (pOutBufHeader));

            if ((pInBufHeader->nFlags & OMX_BUFFERFLAG_EOS) != OMX_BUFFERFLAG_EOS){
               if(pAudEncComPvt->bEOSProcessOn == OMX_TRUE)
                 pAudEncComPvt->bEOSProcessOn = OMX_FALSE;
             }

            if(pAudEncComPvt->bEOSProcessOn == OMX_TRUE) {
               pAudEncComPvt->fpDioSend(hComponent, OMX_AUDENC_INPUT_PORT,
                                               pInBufHeader);
               pAudEncComPvt->fpDioCancel(hComponent,
                                               OMX_AUDENC_OUTPUT_PORT,
                                               pOutBufHeader);
               OMX_TI_AUDENC_UTL_SetBufStatus(&(pAudEncComPvt->outBufHeaderArray[0]),
                                   pAudEncComPvt->pPortdefs[OMX_AUDENC_OUTPUT_PORT]->nBufferCountActual,
                                   pOutBufHeader, OMX_BUFFER_FREE);

               OMX_TI_AUDENC_UTL_SetBufStatus(&(pAudEncComPvt->inBufHeaderArray[0]),
                                   pAudEncComPvt->pPortdefs[OMX_AUDENC_INPUT_PORT]->nBufferCountActual,
                                   pInBufHeader, OMX_BUFFER_FREE);
               goto EXIT;
            } //if(pAudEncComPvt->bEOSProcessOn == OMX_TRUE)

            if ((pInBufHeader->nFlags & OMX_BUFFERFLAG_EOS) == OMX_BUFFERFLAG_EOS) {
                /* this flag will be used if filled length is non zero */
                inEOSReceived = 1;

                if(pAudEncComPvt->bEOSProcessOn == OMX_FALSE)
                pAudEncComPvt->cmdStatus = FLUSH_START;

                pOutBufHeader->nFlags = OMX_BUFFERFLAG_EOS;

                if (pInBufHeader->nFilledLen == 0) {
                    /* as there is no input data, Give back the last input and
                       output dequeued buffer; In case there is data it will be
                       treated as regular buffer submission to scheduler, and
                       flag in frame set to flush */
                    pAudEncComPvt->fpDioSend(hComponent, OMX_AUDENC_INPUT_PORT,
                                             pInBufHeader);
                    pOutBufHeader->nFilledLen = 0;
                    pAudEncComPvt->fpDioCancel(hComponent,
                                               OMX_AUDENC_OUTPUT_PORT,
                                               pOutBufHeader);
                    /* component keeps track of all the buffes, whether it is
                       locked by component or dequeued but not used */
                    OMX_TI_AUDENC_UTL_SetBufStatus(&(pAudEncComPvt->outBufHeaderArray[0]),
                                        pAudEncComPvt->pPortdefs[OMX_AUDENC_OUTPUT_PORT]->nBufferCountActual,
                                        pOutBufHeader, OMX_BUFFER_FREE);

                    OMX_TI_AUDENC_UTL_SetBufStatus(&(pAudEncComPvt->inBufHeaderArray[0]),
                                        pAudEncComPvt->pPortdefs[OMX_AUDENC_INPUT_PORT]->nBufferCountActual,
                                        pInBufHeader, OMX_BUFFER_FREE);
                    if(pAudEncComPvt->bEOSProcessOn == OMX_FALSE) {
                       /* set the element status in scheduler as flush, as there is
                       no data available no need to submit any frames to
                       scheduler, just set the status */
                       pAudEncComPvt->cmdXdm = XDM_FLUSH;
                    }
                    pAudEncComPvt->bEOSProcessOn = OMX_TRUE;
                    goto EXIT;
                } //if (pInBufHeader->nFilledLen == 0)
            } //if ((pInBufHeader->nFlags & OMX_BUFFERFLAG_EOS) == OMX_BUFFERFLAG_EOS)

            bufStatus = OMX_TI_AUDENC_UTL_GetBufStatus(&(pAudEncComPvt->outBufHeaderArray[0]),
                                pAudEncComPvt->pPortdefs[OMX_AUDENC_OUTPUT_PORT]->nBufferCountActual,
                                pOutBufHeader);

            if(bufStatus != OMX_BUFFER_FREE) {
              //Handle this: TBD
			}
            OMX_TI_AUDENC_UTL_SetBufStatus(&(pAudEncComPvt->outBufHeaderArray[0]),
                                pAudEncComPvt->pPortdefs[OMX_AUDENC_OUTPUT_PORT]->nBufferCountActual,
                                pOutBufHeader, OMX_BUFFER_DQ);

            OMX_TI_AUDENC_UTL_SetBufStatus(&(pAudEncComPvt->inBufHeaderArray[0]),
                                pAudEncComPvt->pPortdefs[OMX_AUDENC_INPUT_PORT]->nBufferCountActual,
                                pInBufHeader, OMX_BUFFER_DQ);

            pAudEncComPvt->encInArgsPtr =
                &(pAudEncComPvt->encInArgs[pAudEncComPvt->encInArgsIdx]);

            pAudEncComPvt->encOutArgsPtr =
                &(pAudEncComPvt->encOutArgs[pAudEncComPvt->encOutArgsIdx]);

            pAudEncComPvt->inputBufDescPtr =
                &(pAudEncComPvt->inputBufDesc[pAudEncComPvt->encInArgsIdx]);

            pAudEncComPvt->outputBufDescPtr =
                &(pAudEncComPvt->outputBufDesc[pAudEncComPvt->encOutArgsIdx]);

           pAudEncComPvt->encInArgsPtr->numInSamples = AAC_DEFAULT_SAMPLES_IN_A_FRAME;
          
           /* in case of file mode, component keeps track of pointer of the data
              in a sigle large buffer; this mode is set by IL client */

            if (pAudEncComPvt->bFileMode) {
                nCurBufEncodeOffset = pAudEncComPvt->numBytesConsumed;
            } else {
                nCurBufEncodeOffset = 0;
            }
            /* to be used if supplier changes the offset */
            pInBufHeader->nOffset = 0;
            pAudEncComPvt->inputBufDescPtr->descs[0].buf =
                (XDAS_Int8 *) ((XDAS_Int32) pInBufHeader->pBuffer +
                               pInBufHeader->nOffset + nCurBufEncodeOffset);             
            pAudEncComPvt->inputBufDescPtr->descs[0].bufSize = pInBufHeader->nAllocLen;
			pAudEncComPvt->outputBufDescPtr->descs[0].buf =
                      (XDAS_Int8 *) pOutBufHeader->pBuffer;
            pAudEncComPvt->outputBufDescPtr->descs[0].bufSize = pOutBufHeader->nAllocLen;
            /* control can be called directly, if it is not in the processing state */
            pAudEncComPvt->encInArgsIdx++;
            if (pAudEncComPvt->encInArgsIdx >=
                OMX_AUDENC_MAX_INPUT_BUFFER_COUNT)
                pAudEncComPvt->encInArgsIdx = 0;

            pAudEncComPvt->encOutArgsIdx++;
            if (pAudEncComPvt->encOutArgsIdx >=
                OMX_AUDENC_MAX_OUTPUT_BUFFER_COUNT)
                pAudEncComPvt->encOutArgsIdx = 0;
        		
       
            /*  Propagate the Time Stamps from input to Corresponding Output */
            TIMM_OSAL_TraceExt(pAudEncComPvt->nTraceGroup,
                               "Propagate the timestamp: %d", pInBufHeader->nTimeStamp);
            pOutBufHeader->nTimeStamp = pInBufHeader->nTimeStamp;

            status = AUDENC1_process(pAudEncComPvt->encHandle, (XDM1_BufDesc *)
                                     pAudEncComPvt->inputBufDescPtr,
                                     (XDM1_BufDesc *)
                                     pAudEncComPvt->outputBufDescPtr,
                                     (AUDENC1_InArgs *)
                                     pAudEncComPvt->encInArgsPtr,
                                     (AUDENC1_OutArgs *)
                                     pAudEncComPvt->encOutArgsPtr);
            /* Increment and keep track of the total frames encoded */
            pAudEncComPvt->frameCounter++;
            //MEMUTILS_cacheWb(pOutBufHeader->pBuffer,pOutBufHeader->nAllocLen);
            if (status == XDM_EFAIL) {
                eError = OMX_ErrorStreamCorrupt;
#ifdef CODEC_AACENC
                if (pAudEncComPvt->pPortdefs[OMX_AUDENC_OUTPUT_PORT]->format.audio.
                  eEncoding == OMX_AUDIO_CodingAAC){
                    OMX_TI_AACAE_CB_ProcessDone(hComponent);
                }
#endif
               OMX_BASE_Error("Process function returned an Error...");
                // break; /* Error Condition: Application may want to break off */
            }
            else {            	
#ifdef CODEC_AACENC
                if (pAudEncComPvt->pPortdefs[OMX_AUDENC_OUTPUT_PORT]->format.audio.
                  eEncoding == OMX_AUDIO_CodingAAC){
                    OMX_TI_AACAE_CB_ProcessDone(hComponent);
                }
#endif
                pAudEncComPvt->numBytesConsumed = ((AUDENC1_OutArgs *)pAudEncComPvt->encOutArgsPtr)->bytesGenerated;
	    }        
        } //if ((nInMsgCount > 0) && (nOutMsgCount > 0))
    } //(tCurState == OMX_StateExecuting)

EXIT:

    return eError;
}


/*============================================================================*/

/**
********************************************************************************
 *  @func   OMX_TI_AUDENC_setDefault()
 *  @brief  This method is used to initalize the port definitions structure and
 *          set them to default values for AAC Encoder
 *
 *  @param pAudEncComPvt  : Pointer to the component private data structure
 *
 *  @return
 *  OMX_ErrorNone = Successful
 *
 *  Other_value = Failed (Error code is returned)
 *
 *
 * ============================================================================
 */

static OMX_ERRORTYPE OMX_TI_AUDENC_SetDefault(OMX_AUDENC_PVTTYPE *
                                              pAudEncComPvt,
                                              OMX_AUDIO_CODINGTYPE
                                              compressionFormat)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_U32 i;
    Engine_Error errorCode;

    /* Override the default values set to an Input Port with Actual/Default
       values */

    pAudEncComPvt->pPortdefs[OMX_AUDENC_INPUT_PORT]->bEnabled = OMX_FALSE;
    pAudEncComPvt->pPortdefs[OMX_AUDENC_INPUT_PORT]->eDir = OMX_DirInput;
    pAudEncComPvt->pPortdefs[OMX_AUDENC_INPUT_PORT]->nPortIndex =
        OMX_AUDENC_INPUT_PORT;
    pAudEncComPvt->pPortdefs[OMX_AUDENC_INPUT_PORT]->nBufferCountMin =
        OMX_AUDENC_DEFAULT_INPUT_BUFFER_COUNT;
    pAudEncComPvt->pPortdefs[OMX_AUDENC_INPUT_PORT]->nBufferCountActual =
        OMX_AUDENC_DEFAULT_INPUT_BUFFER_COUNT;
    pAudEncComPvt->pPortdefs[OMX_AUDENC_INPUT_PORT]->nBufferSize =
        OMX_AUDENC_DEFAULT_INPUT_BUFFER_SIZE;
    pAudEncComPvt->pPortdefs[OMX_AUDENC_INPUT_PORT]->eDomain =
        OMX_PortDomainAudio;
    pAudEncComPvt->pPortdefs[OMX_AUDENC_INPUT_PORT]->format.audio.cMIMEType = "PCM";
    
	pAudEncComPvt->pPortdefs[OMX_AUDENC_INPUT_PORT]->format.
        audio.pNativeRender = OMX_FALSE;
    pAudEncComPvt->pPortdefs[OMX_AUDENC_INPUT_PORT]->format.
        audio.bFlagErrorConcealment = OMX_FALSE;
    pAudEncComPvt->pPortdefs[OMX_AUDENC_INPUT_PORT]->format.audio.
        eEncoding = OMX_AUDIO_CodingUnused;
    pAudEncComPvt->pPortdefs[OMX_AUDENC_INPUT_PORT]->format.
        audio.pNativeRender = OMX_FALSE;
    pAudEncComPvt->pPortdefs[OMX_AUDENC_INPUT_PORT]->bBuffersContiguous =
        OMX_FALSE;
    pAudEncComPvt->pPortdefs[OMX_AUDENC_INPUT_PORT]->nBufferAlignment = OMX_AUDENC_DEFAULT_BUFFER_ALIGNMENT;

    /*-------------------------------------------------------------------------*
     *  Initialize the eNotifyType of the component with NONE as the data
     *  processing is based on the periodic driver call-back rather than usual
     *  DataNotify event
     *------------------------------------------------------------------------*/
    pAudEncComPvt->tDerToBase.pPortProperties[OMX_AUDENC_INPUT_PORT]->eNotifyType =
                                            OMX_NOTIFY_TYPE_ALWAYS;

    /* Override the default values set to an Output Port with Actual/Default
       values */

    pAudEncComPvt->pPortdefs[OMX_AUDENC_OUTPUT_PORT]->bEnabled = OMX_FALSE;
    pAudEncComPvt->pPortdefs[OMX_AUDENC_OUTPUT_PORT]->eDir = OMX_DirOutput;
    pAudEncComPvt->pPortdefs[OMX_AUDENC_OUTPUT_PORT]->nPortIndex =
        OMX_AUDENC_OUTPUT_PORT;
    pAudEncComPvt->pPortdefs[OMX_AUDENC_OUTPUT_PORT]->nBufferCountMin =
        OMX_AUDENC_DEFAULT_OUTPUT_BUFFER_COUNT;
    pAudEncComPvt->pPortdefs[OMX_AUDENC_OUTPUT_PORT]->nBufferCountActual =
        OMX_AUDENC_DEFAULT_OUTPUT_BUFFER_COUNT;
    pAudEncComPvt->pPortdefs[OMX_AUDENC_OUTPUT_PORT]->nBufferSize =
        OMX_AUDENC_DEFAULT_OUTPUT_BUFFER_SIZE;
    pAudEncComPvt->pPortdefs[OMX_AUDENC_OUTPUT_PORT]->eDomain =
        OMX_PortDomainAudio;
    pAudEncComPvt->pPortdefs[OMX_AUDENC_OUTPUT_PORT]->format.audio.cMIMEType = "AAC";

    pAudEncComPvt->pPortdefs[OMX_AUDENC_OUTPUT_PORT]->format.
        audio.pNativeRender = OMX_FALSE;

    pAudEncComPvt->pPortdefs[OMX_AUDENC_OUTPUT_PORT]->format.
        audio.bFlagErrorConcealment = OMX_FALSE;
    pAudEncComPvt->pPortdefs[OMX_AUDENC_OUTPUT_PORT]->format.
        audio.eEncoding = compressionFormat;
    pAudEncComPvt->pPortdefs[OMX_AUDENC_OUTPUT_PORT]->format.
        audio.pNativeRender = OMX_FALSE;
    pAudEncComPvt->pPortdefs[OMX_AUDENC_OUTPUT_PORT]->bBuffersContiguous =
        OMX_FALSE;

    /* todo FrameQ would provide aligned buffers */
    pAudEncComPvt->pPortdefs[OMX_AUDENC_OUTPUT_PORT]->nBufferAlignment = OMX_AUDENC_DEFAULT_BUFFER_ALIGNMENT;

    /*-------------------------------------------------------------------------*
     *  Initialize the eNotifyType of the component with NONE as the data
     *  processing is based on the periodic driver call-back rather than usual
     *  DataNotify event
     *------------------------------------------------------------------------*/

    pAudEncComPvt->tDerToBase.pPortProperties[OMX_AUDENC_OUTPUT_PORT]->eNotifyType =
                                              OMX_NOTIFY_TYPE_ALWAYS;

    /* Set the current State to Loaded  */

    pAudEncComPvt->tCurState = OMX_StateLoaded;

    pAudEncComPvt->pEncStatus->size = sizeof(AUDENC1_Status);
    ((IAUDENC1_DynamicParams *) (pAudEncComPvt->pEncDynParams))->size =
        sizeof(AUDENC1_DynamicParams);
    for (i = 0; i < OMX_AUDENC_MAX_INPUT_BUFFER_COUNT; i++) {
        pAudEncComPvt->encInArgsPtr = &(pAudEncComPvt->encInArgs[i]);
        pAudEncComPvt->encInArgsPtr->size =
            sizeof(IAUDENC1_InArgs);
        pAudEncComPvt->encInArgsPtr->numInSamples = 0;
    }

    for (i = 0; i < OMX_AUDENC_MAX_OUTPUT_BUFFER_COUNT; i++) {
        pAudEncComPvt->encOutArgsPtr = &(pAudEncComPvt->encOutArgs[i]);
        pAudEncComPvt->encOutArgsPtr->size =
            sizeof(IAUDENC1_OutArgs);
        pAudEncComPvt->encOutArgsPtr->bytesGenerated = 0;
    }

    pAudEncComPvt->numBytesConsumed = 0;

    pAudEncComPvt->bFileMode = OMX_FALSE;

    pAudEncComPvt->isfirstFrame = 1;

    pAudEncComPvt->cmdStatus = 0;

    /* Based on this flag codec would be created */
    pAudEncComPvt->bCodecCreate = OMX_TRUE;

    /* using default buffers */
    pAudEncComPvt->bInBufferCountChange = OMX_FALSE;
    pAudEncComPvt->bOutBufferCountChange = OMX_FALSE;
    pAudEncComPvt->inBufHeaderArray = NULL;
    pAudEncComPvt->outBufHeaderArray = NULL;

    /* initialize the code engine; todo for multichannel might require once */
    pAudEncComPvt->ce = Engine_open(engineName, NULL, &errorCode);
    if (pAudEncComPvt->ce == NULL) {
        OMX_BASE_Error("Engine creation: Could not open engine \n");
        eError = OMX_ErrorInsufficientResources;
        goto EXIT;
    }

  /*--------------------------------------------------------------------------*/
  /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /*  Set Task exit flag as False                                             */
  /*                                                                          */
  /*--------------------------------------------------------------------------*/
    pAudEncComPvt->bTaskExit = OMX_FALSE;
    pAudEncComPvt->bCmdFlush = OMX_FALSE;
    pAudEncComPvt->bEOSProcessOn = OMX_FALSE;

  EXIT:
    return eError;
}

/* ==========================================================================
 */

/**
********************************************************************************
 *  @func  _OMX_AUDENC_InitialzeComponentExtraDataInfo
 *  @brief Initializa the ComponentPrivate Fields only for output port
 *
 *  @param hComponent  : Handle to the component data structure
 *  @return
 *  OMX_ErrorNone = Successful
 *
 *  Other_value = Failed (Error code is returned)
 *
 * ==========================================================================
 */
#if 0
static
OMX_ERRORTYPE _OMX_TI_AUDENC_InitialzeComponentExtraDataInfo(OMX_HANDLETYPE
                                                             hComponent)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;

    OMX_COMPONENTTYPE *pHandle = NULL;

    OMX_AUDENC_PVTTYPE *pAudEncComPvt;

    OMX_BUFFERHEADERTYPE sCustomHeader;

    OMX_BASE_REQUIRE(hComponent != NULL, OMX_ErrorBadParameter);

    pHandle = (OMX_COMPONENTTYPE *) hComponent;

    OMX_BASE_CHK_VERSION(pHandle, OMX_COMPONENTTYPE, eError);

    pAudEncComPvt = (OMX_AUDENC_PVTTYPE *) pHandle->pComponentPrivate;


   /*--------------------------------------------------------------------------*
    * The bit field indicated which of the meta data fields will be used by    *
    * the component. The 1st field in the header should correspond to the LSB  *
    * and so on. The bit field value can be changed at runtime. However here,  *
    * only those bits should be turned off for which there will never be any   *
    * data. Here maximum possible bits should be turned on so that size        *
    * allocated is never less than required                                    *
    *--------------------------------------------------------------------------*/

    eError =
        pAudEncComPvt->fpDioUtil(hComponent, OMX_AUDENC_OUTPUT_PORT,
                                 OMX_BASE_DIO_UTIL_GetTotalBufferSize,
                                 &sCustomHeader);

  EXIT:
  /*--------------------------------------------------------------------------*
   * NOTE: If the data size changes due to some change in settings by the     *
   * client then this entire procedure has to be followed again. The alloc    *
   * len field will contain only the size of the data buffer. Size of any     *
   * packet data will be specified separately as above. The alloc len field   *
   * will then return the total buffer size that needs to be allocated        *
   *--------------------------------------------------------------------------*/
    return eError;
}
#endif
/* ===========================================================================*/
/**
********************************************************************************
 *  @func   OMX_TI_AUDENC_ComponentDeinit()
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
static OMX_ERRORTYPE OMX_TI_AUDENC_ComponentDeinit(OMX_HANDLETYPE hComponent)
{
    OMX_ERRORTYPE eError = OMX_ErrorNone;

    OMX_COMPONENTTYPE *pComp;

    OMX_AUDENC_PVTTYPE *pAudEncComPvt;

    OMX_U32 nIndex = 0;

    OMX_BASE_REQUIRE(hComponent != NULL, OMX_ErrorBadParameter);

    pComp = (OMX_COMPONENTTYPE *) hComponent;
    pAudEncComPvt = (OMX_AUDENC_PVTTYPE *) pComp->pComponentPrivate;

    g_AudEncComponentCreated--;

    if(g_AudEncComponentCreated == 0) pAudEncComPvt->bTaskExit = OMX_TRUE;

    /* Add CE deinit related stuff here
       Deinitialize the encoder Instance/s and free all the resources     */

    if (pAudEncComPvt->ce) {
        Engine_close(pAudEncComPvt->ce);
    }
    TIMM_OSAL_Free(pAudEncComPvt->tDerToBase.
        pPortProperties[OMX_AUDENC_OUTPUT_PORT]->pMetaDataFieldTypesArr);

    if (pAudEncComPvt->tDerToBase.
        pPortProperties[OMX_AUDENC_OUTPUT_PORT]->pBufParams) {
        TIMM_OSAL_Free(pAudEncComPvt->
                       tDerToBase.pPortProperties[OMX_AUDENC_OUTPUT_PORT]->
                       pBufParams);
        pAudEncComPvt->tDerToBase.
            pPortProperties[OMX_AUDENC_OUTPUT_PORT]->pBufParams = NULL;
    }

    for (nIndex = 0;
         nIndex < pAudEncComPvt->tDerToBase.pAudioPortParams->nPorts;
         nIndex++) {
        TIMM_OSAL_Free(pAudEncComPvt->tDerToBase.pPortProperties[nIndex]);
    }

    /* free up memory allocated for buffer headers */
    for (nIndex = 0; nIndex <
         pAudEncComPvt->pPortdefs[OMX_AUDENC_INPUT_PORT]->nBufferCountActual;
         nIndex++) {
        if(pAudEncComPvt->inBufHeaderArray[nIndex].pBufHeader != NULL)
        TIMM_OSAL_Free(pAudEncComPvt->inBufHeaderArray[nIndex].pBufHeader);
    }

    for (nIndex = 0; nIndex <
         pAudEncComPvt->pPortdefs[OMX_AUDENC_OUTPUT_PORT]->nBufferCountActual;
         nIndex++) {
        if(pAudEncComPvt->outBufHeaderArray[nIndex].pBufHeader != NULL)
        TIMM_OSAL_Free(pAudEncComPvt->outBufHeaderArray[nIndex].pBufHeader);
    }

    if(pAudEncComPvt->inBufHeaderArray != NULL) {
       TIMM_OSAL_Free(pAudEncComPvt->inBufHeaderArray);
       pAudEncComPvt->inBufHeaderArray = NULL;
    }

    if(pAudEncComPvt->outBufHeaderArray != NULL) {
      TIMM_OSAL_Free(pAudEncComPvt->outBufHeaderArray);
      pAudEncComPvt->outBufHeaderArray = NULL;
    }

    /* Calling OMX Base Component Deinit */
    eError = OMX_BASE_ComponentDeinit(hComponent);
    OMX_BASE_ASSERT(eError == OMX_ErrorNone, eError);

    TIMM_OSAL_Free(pAudEncComPvt->pEncStaticParams);

    TIMM_OSAL_Free(pAudEncComPvt->pEncDynParams);

    TIMM_OSAL_Free(pAudEncComPvt->pEncStatus);

    TIMM_OSAL_Free(pAudEncComPvt->tDerToBase.pAudioPortParams);

    TIMM_OSAL_Free(pAudEncComPvt->tDerToBase.pPortProperties);

    TIMM_OSAL_Free(pAudEncComPvt->tDerToBase.cComponentName);

    TIMM_OSAL_Free(pAudEncComPvt);

    pAudEncComPvt = NULL;

  EXIT:
    return eError;
}

/* ==========================================================================
 */

