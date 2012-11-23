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

/* ! @file omx_vlpb_utils.c @brief This file contains methods that Support the
 * functionality for the Openmax Video Capture component. @author @version 0.1
 * - April 03,2009 */

/* -------------------------------------------------------------------------- */
/* ========================================================================= !
 * Revision History !
 * ========================================================================
 * !3-Apr-2009 Radha Purnima Dasari: Initial Version !25-Aug-2009 Radha Purnima
 * * * * * * * * * radhapurnima@ti.com
 * ========================================================================= */

/******************************************************************************
* Includes
******************************************************************************/
/* ----- system and platform files ---------------------------- */
#include <string.h>
#include <xdc/std.h>
#include <ti/timmosal/timm_osal_interfaces.h>

/*-------program files ----------------------------------------*/
#include <ti/omx/interfaces/openMaxv11/OMX_Core.h>
#include <ti/omx/interfaces/openMaxv11/OMX_Component.h>

#include <ti/omx/comp/omxbase/omx_base.h>
#include <ti/omx/comp/omxbase/omx_base_utils.h>


#include "OMX_TI_Index.h"
#include "OMX_TI_Video.h"
#include "omx_vlpb.h"
#include "omx_vlpb_priv.h"
#include "omx_vlpb_utils.h"

/*******************************************************************************
 * EXTERNAL REFERENCES NOTE : only use if not found in header file
 ******************************************************************************/

/*------------------------ data declarations ---------------------------------*/
/* None */

/*----------------------- function prototypes --------------------------------*/
extern OMX_ERRORTYPE OMX_VLPBFlushChannelBuf ( OMX_HANDLETYPE hComponent );

/*******************************************************************************
 * PUBLIC DECLARATIONS Defined here, used elsewhere
 ******************************************************************************/

/*------------------------ data declarations ---------------------------------*/
/* None */

/*----------------------- function prototypes --------------------------------*/
/* None */

/*******************************************************************************
 * PRIVATE DECLARATIONS Defined here, used only here
 ******************************************************************************/

/*--------------------------- macros  ----------------------------------------*/
/* None */

/*------------------------- data declarations --------------------------------*/
/* None */

/*------------------------- function prototypes ------------------------------*/
/* None */
/* ========================================================================== */

/**
 * _OMX_VLPB_SetDerToBaseParameters() :
 * Fill the structure "OMX_BASE_PARAMSFROMDERIVED" which needs to
 *                     be given as an input to the Base Component before calling
 *                     OMX_BASE_ComponentInit()
 *
 * @param hComponent  : Handle to the component data structure
 * @return
 * OMX_ErrorNone = Successful
 *
 * Other_value = Failed (Error code is returned)
 *
 */
/* ========================================================================== */
OMX_ERRORTYPE
_OMX_VLPB_SetDerToBaseParameters (OMX_HANDLETYPE hComponent)
{
  OMX_ERRORTYPE eError = OMX_ErrorUndefined;

  OMX_COMPONENTTYPE *pHandle = NULL;

  OMX_VLPB_PVTTYPE *pVLPBComPvt = NULL;

  OMX_U8 i;

  /*--------------------------------------------------------------------------*/
  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
  /* Verify the input pointers                                                */
  /*                                                                          */
  /*--------------------------------------------------------------------------*/
  OMX_BASE_REQUIRE ( hComponent != NULL, OMX_ErrorBadParameter );

  pHandle = ( OMX_COMPONENTTYPE * ) hComponent;

  /*--------------------------------------------------------------------------*/
  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
  /* Verify the version                                                       */
  /*                                                                          */
  /*--------------------------------------------------------------------------*/
  OMX_BASE_CHK_VERSION ( pHandle, OMX_COMPONENTTYPE, eError );

  pVLPBComPvt = ( OMX_VLPB_PVTTYPE * ) pHandle->pComponentPrivate;

  /* VLPB component is not creating any heap so DefaultHeap is set to NULL.
     Thus it'll pick up the static default heap */
  pVLPBComPvt->tDerToBase.hDefaultHeap = NULL;
  pVLPBComPvt->tDerToBase.cComponentName =
      ( OMX_STRING ) OMX_BASE_Malloc ( sizeof ( OMX_U8 ) *
                                       OMX_MAX_STRINGNAME_SIZE,
                                       pVLPBComPvt->tDerToBase.hDefaultHeap );
  OMX_BASE_ASSERT ( pVLPBComPvt->tDerToBase.cComponentName != NULL,
                    OMX_ErrorInsufficientResources );


  /*--------------------------------------------------------------------------*/
  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
  /* Initialize the component name, version                                   */
  /*                                                                          */
  /*--------------------------------------------------------------------------*/
#ifdef _LOCAL_CORE_a8host_
  strcpy ( pVLPBComPvt->tDerToBase.cComponentName, OMX_VLPB_HOSTCOMP_NAME );
#endif
#ifdef _LOCAL_CORE_c6xdsp_
  strcpy ( pVLPBComPvt->tDerToBase.cComponentName, OMX_VLPB_DSPCOMP_NAME );
#endif
  pVLPBComPvt->tDerToBase.nComponentVersion.s.nVersionMajor =
      OMX_VLPB_COMP_VERSION_MAJOR;
  pVLPBComPvt->tDerToBase.nComponentVersion.s.nVersionMinor =
      OMX_VLPB_COMP_VERSION_MINOR;
  pVLPBComPvt->tDerToBase.nComponentVersion.s.nRevision =
      OMX_VLPB_COMP_VERSION_REVISION;
  pVLPBComPvt->tDerToBase.nComponentVersion.s.nStep =
      OMX_VLPB_COMP_VERSION_STEP;

  /*--------------------------------------------------------------------------*/
  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
  /* Alloc the space for Audio, Video, Image and Other ports                  */
  /*                                                                          */
  /*--------------------------------------------------------------------------*/
  OMX_BASE_MALLOC_STRUCT ( pVLPBComPvt->tDerToBase.pAudioPortParams,
                           OMX_PORT_PARAM_TYPE,
                           pVLPBComPvt->tDerToBase.hDefaultHeap );
  OMX_BASE_INIT_STRUCT_PTR ( pVLPBComPvt->tDerToBase.pAudioPortParams,
                             OMX_PORT_PARAM_TYPE );
  pVLPBComPvt->tDerToBase.pAudioPortParams->nPorts = 0;
  pVLPBComPvt->tDerToBase.pAudioPortParams->nStartPortNumber = 0;

  /* Initialize Video Port parameters */
  OMX_BASE_MALLOC_STRUCT ( pVLPBComPvt->tDerToBase.pVideoPortParams,
                           OMX_PORT_PARAM_TYPE,
                           pVLPBComPvt->tDerToBase.hDefaultHeap );
  OMX_BASE_INIT_STRUCT_PTR ( pVLPBComPvt->tDerToBase.pVideoPortParams,
                             OMX_PORT_PARAM_TYPE );
  pVLPBComPvt->tDerToBase.pVideoPortParams->nPorts = OMX_VLPB_NUM_PORTS;
  pVLPBComPvt->tDerToBase.pVideoPortParams->nStartPortNumber =
      OMX_VLPB_DEFAULT_START_PORT_NUM;

  /* Initialize Image Port parameters */
  OMX_BASE_MALLOC_STRUCT ( pVLPBComPvt->tDerToBase.pImagePortParams,
                           OMX_PORT_PARAM_TYPE,
                           pVLPBComPvt->tDerToBase.hDefaultHeap );
  OMX_BASE_INIT_STRUCT_PTR ( pVLPBComPvt->tDerToBase.pImagePortParams,
                             OMX_PORT_PARAM_TYPE );
  pVLPBComPvt->tDerToBase.pImagePortParams->nPorts = 0;
  pVLPBComPvt->tDerToBase.pImagePortParams->nStartPortNumber = 0;

  /* Initialize Other Port parameters */
  OMX_BASE_MALLOC_STRUCT ( pVLPBComPvt->tDerToBase.pOtherPortParams,
                           OMX_PORT_PARAM_TYPE,
                           pVLPBComPvt->tDerToBase.hDefaultHeap );
  OMX_BASE_INIT_STRUCT_PTR ( pVLPBComPvt->tDerToBase.pOtherPortParams,
                             OMX_PORT_PARAM_TYPE );
  pVLPBComPvt->tDerToBase.pOtherPortParams->nPorts = 0;
  pVLPBComPvt->tDerToBase.pOtherPortParams->nStartPortNumber = 0;

  /* Initialize the Total Number of Ports and Start Port Number */
  pVLPBComPvt->tDerToBase.nNumPorts = OMX_VLPB_NUM_PORTS;
  pVLPBComPvt->tDerToBase.nMinStartPortIndex = OMX_VLPB_DEFAULT_START_PORT_NUM;

  /*--------------------------------------------------------------------------*/
  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
  /* Initiailize the comp.context type, task priorities                       */
  /* Note: By having context type as ACTIVE, the BASE becomes an active       */
  /* component, meaning a thread is created for servicing of command and data */
  /* notifications.Otherwise all the commands/data notifications are serviced */
  /* in the application context.                                              */
  /* Caution: Do not use the ctxType as PASSIVE since this mode is not Tested */
  /*                                                                          */
  /*--------------------------------------------------------------------------*/
  pVLPBComPvt->tDerToBase.ctxType = OMX_BASE_CONTEXT_ACTIVE;

  /* Incase of Active Context, set the Stacksize and GroupPriority */
  if (pVLPBComPvt->tDerToBase.ctxType == OMX_BASE_CONTEXT_ACTIVE)
  {
    snprintf ( pVLPBComPvt->tDerToBase.cTaskName,
               sizeof ( pVLPBComPvt->tDerToBase.cTaskName ),
               "VLPB_%p", hComponent );
    pVLPBComPvt->tDerToBase.nStackSize = OMX_VLPB_STACKSIZE;
    pVLPBComPvt->tDerToBase.nPrioirty = OMX_VLPB_DEFAULT_TASKPRIORITY;
  }
  pVLPBComPvt->tDerToBase.nGroupID = OMX_VLPB_GROUPID;
  pVLPBComPvt->tDerToBase.nGroupPriority = OMX_VLPB_GROUPPRIORITY;

  /*--------------------------------------------------------------------------*/
  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
  /* Allocate the memory and set the default properties                       */
  /*                                                                          */
  /*--------------------------------------------------------------------------*/

  /* Allocating memory for port properties before calling SetDefaultProperties */
  pVLPBComPvt->tDerToBase.pPortProperties = ( OMX_BASE_PORT_PROPERTIES ** )
      TIMM_OSAL_Malloc ( sizeof ( OMX_BASE_PORT_PROPERTIES * ) *
                         pVLPBComPvt->tDerToBase.nNumPorts, TIMM_OSAL_TRUE, 0,
                         TIMMOSAL_MEM_SEGMENT_EXT );
  OMX_BASE_ASSERT ( pVLPBComPvt->tDerToBase.pPortProperties != NULL,
                    OMX_ErrorInsufficientResources );

  for (i = 0; i < (pVLPBComPvt->tDerToBase.nNumPorts); i++)
  {
    pVLPBComPvt->tDerToBase.pPortProperties[i] =
        ( OMX_BASE_PORT_PROPERTIES * ) TIMM_OSAL_Malloc
        ( sizeof ( OMX_BASE_PORT_PROPERTIES ), TIMM_OSAL_TRUE, 0,
          TIMMOSAL_MEM_SEGMENT_EXT );

    OMX_BASE_ASSERT ( pVLPBComPvt->tDerToBase.pPortProperties[i] != NULL,
                      OMX_ErrorInsufficientResources );
  }

  /* Setting default properties. PreCondition: NumOfPorts is filled and all
     pointers allocated */
  OMX_BASE_SetDefaultProperties ( hComponent );

  /*--------------------------------------------------------------------------*/
  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /* Configure the notification:                                              */
  /*                                                                          */
  /*--------------------------------------------------------------------------*/
  pVLPBComPvt->tDerToBase.bNotifyForAnyPort = OMX_TRUE;
    /* Change the buffer type to Tiler Memory & set nNumComponentBuffers to 1 */
  for (i = 0; i < (pVLPBComPvt->tDerToBase.nNumPorts); i++)
  {
    pVLPBComPvt->tDerToBase.pPortProperties[i]->eBufMemoryType =
                                        OMX_BUFFER_MEMORY_DEFAULT;
    pVLPBComPvt->tDerToBase.pPortProperties[i]->nNumComponentBuffers = 1;
    pVLPBComPvt->tDerToBase.pPortProperties[i]->pBufParams = NULL;
  }


  eError = OMX_ErrorNone;

EXIT:
  return eError;

/* _OMX_VLPB_SetDerToBaseParameters */
}

/* ========================================================================== */

/**
 *_OMX_VLPB_InitialzeComponentPrivateParams :
 *                      Initializa the ComponentPrivate Fields
 *
 * @param hComponent  : Handle to the component data structure
 * @return
 * OMX_ErrorNone = Successful
 *
 * Other_value = Failed (Error code is returned)
 *
 */
/* ========================================================================== */

OMX_ERRORTYPE
_OMX_VLPB_InitialzeComponentPrivateParams (OMX_HANDLETYPE hComponent)
{

  OMX_ERRORTYPE eError = OMX_ErrorUndefined;

  OMX_COMPONENTTYPE *pHandle = NULL;

  OMX_VLPB_PVTTYPE *pVLPBComPvt = NULL;

  OMX_U32 i;

  TIMM_OSAL_ERRORTYPE osalError;

  omxVLPBInPort2WinMgr_t *Inport_entry = NULL;
  omxVLPBOutPort2WinMgr_t *Outport_entry = NULL;
  /* Check the input parameters, this should be TRUE else report an Error */
  OMX_BASE_REQUIRE ( hComponent != NULL, OMX_ErrorBadParameter );

  pHandle = ( OMX_COMPONENTTYPE * ) hComponent;

  OMX_BASE_CHK_VERSION ( pHandle, OMX_COMPONENTTYPE, eError );

  pVLPBComPvt = ( OMX_VLPB_PVTTYPE * ) pHandle->pComponentPrivate;

  /*==========================================================================
   Set the Port Definition (OMX_PARAM_PORTDEFINITIONTYPE)  Values : INPUT PORT
   ==========================================================================*/
  for (i = 0; i < OMX_VLPB_NUM_INPUT_PORTS; i++)
  {
    pVLPBComPvt->pPortdefs[OMX_VLPB_INPUT_PORT_START_INDEX + i]->nPortIndex =
        ( OMX_VLPB_INPUT_PORT_START_INDEX + i );
    pVLPBComPvt->pPortdefs[OMX_VLPB_INPUT_PORT_START_INDEX + i]->eDir =
        OMX_DirInput;
    pVLPBComPvt->pPortdefs[OMX_VLPB_INPUT_PORT_START_INDEX +
                           i]->nBufferCountActual =
        OMX_VLPB_DEFAULT_INPUT_BUFFER_COUNT;
    pVLPBComPvt->pPortdefs[OMX_VLPB_INPUT_PORT_START_INDEX +
                           i]->nBufferCountMin =
        OMX_VLPB_DEFAULT_INPUT_BUFFER_COUNT;
    pVLPBComPvt->pPortdefs[OMX_VLPB_INPUT_PORT_START_INDEX + i]->nBufferSize =
        OMX_VLPB_DEFAULT_INPUT_BUFFER_SIZE;
    pVLPBComPvt->pPortdefs[OMX_VLPB_INPUT_PORT_START_INDEX +
                           i]->nBufferAlignment = 0x2;
    pVLPBComPvt->pPortdefs[OMX_VLPB_INPUT_PORT_START_INDEX + i]->bEnabled =
        OMX_FALSE;
    pVLPBComPvt->pPortdefs[OMX_VLPB_INPUT_PORT_START_INDEX + i]->bPopulated =
        OMX_FALSE;
    pVLPBComPvt->pPortdefs[OMX_VLPB_INPUT_PORT_START_INDEX + i]->eDomain =
        OMX_PortDomainVideo;
    /* Update the Domain (Video) Specific values */
    pVLPBComPvt->pPortdefs[OMX_VLPB_INPUT_PORT_START_INDEX +
                           i]->format.video.cMIMEType = NULL;
    pVLPBComPvt->pPortdefs[OMX_VLPB_INPUT_PORT_START_INDEX +
                           i]->format.video.pNativeRender = NULL;
    pVLPBComPvt->pPortdefs[OMX_VLPB_INPUT_PORT_START_INDEX +
                           i]->format.video.nFrameWidth =
        OMX_VLPB_DEFAULT_INPUT_FRAME_WIDTH;
    pVLPBComPvt->pPortdefs[OMX_VLPB_INPUT_PORT_START_INDEX +
                           i]->format.video.nFrameHeight =
        OMX_VLPB_DEFAULT_INPUT_FRAME_HEIGHT;
    pVLPBComPvt->pPortdefs[OMX_VLPB_INPUT_PORT_START_INDEX +
                           i]->format.video.nStride =
        OMX_VLPB_DEFAULT_INPUT_FRAME_WIDTH;
    pVLPBComPvt->pPortdefs[OMX_VLPB_INPUT_PORT_START_INDEX +
                           i]->format.video.nSliceHeight = 0;
    pVLPBComPvt->pPortdefs[OMX_VLPB_INPUT_PORT_START_INDEX +
                           i]->format.video.xFramerate =
        OMX_VLPB_DEFAULT_FRAME_RATE;
    pVLPBComPvt->pPortdefs[OMX_VLPB_INPUT_PORT_START_INDEX +
                           i]->format.video.eCompressionFormat =
        OMX_VIDEO_CodingUnused;
    pVLPBComPvt->pPortdefs[OMX_VLPB_INPUT_PORT_START_INDEX +
                           i]->format.video.eColorFormat =
        OMX_COLOR_FormatYUV420SemiPlanar;
    pVLPBComPvt->pPortdefs[OMX_VLPB_INPUT_PORT_START_INDEX +
                           i]->format.video.pNativeWindow = NULL;

    /*-------------------------------------------------------------------------*
     *  Initialize the eNotifyType of the component with NONE as the data
     *  processing is based on the periodic driver call-back rather than usual
     *  DataNotify event
     *------------------------------------------------------------------------*/
    pVLPBComPvt->tDerToBase.pPortProperties[OMX_VLPB_INPUT_PORT_START_INDEX
                                            + i]->eNotifyType =
      OMX_NOTIFY_TYPE_ALWAYS;
  }

  
   /*==========================================================================
   Set the Port Definition (OMX_PARAM_PORTDEFINITIONTYPE)  Values : OUTPUT PORT
   ==========================================================================*/
  for (i = 0; i < OMX_VLPB_NUM_OUTPUT_PORTS; i++)
  {
    pVLPBComPvt->pPortdefs[OMX_VLPB_OUTPUT_PORT_START_INDEX + i]->nPortIndex =
      (OMX_VLPB_OUTPUT_PORT_START_INDEX + i);
    pVLPBComPvt->pPortdefs[OMX_VLPB_OUTPUT_PORT_START_INDEX + i]->eDir =
      OMX_DirOutput;
    pVLPBComPvt->pPortdefs[OMX_VLPB_OUTPUT_PORT_START_INDEX +
                           i]->nBufferCountActual =
      OMX_VLPB_DEFAULT_OUTPUT_BUFFER_COUNT;
    pVLPBComPvt->pPortdefs[OMX_VLPB_OUTPUT_PORT_START_INDEX +
                           i]->nBufferCountMin =
      OMX_VLPB_DEFAULT_OUTPUT_BUFFER_COUNT;
    pVLPBComPvt->pPortdefs[OMX_VLPB_OUTPUT_PORT_START_INDEX + i]->nBufferSize =
      OMX_VLPB_DEFAULT_OUTPUT_BUFFER_SIZE;
    pVLPBComPvt->pPortdefs[OMX_VLPB_OUTPUT_PORT_START_INDEX +
                           i]->nBufferAlignment = 0x2;
    pVLPBComPvt->pPortdefs[OMX_VLPB_OUTPUT_PORT_START_INDEX + i]->bEnabled =
      OMX_FALSE;
    pVLPBComPvt->pPortdefs[OMX_VLPB_OUTPUT_PORT_START_INDEX + i]->bPopulated =
      OMX_FALSE;
    pVLPBComPvt->pPortdefs[OMX_VLPB_OUTPUT_PORT_START_INDEX + i]->eDomain =
      OMX_PortDomainVideo;
    /* Update the Domain (Video) Specific values */
    pVLPBComPvt->pPortdefs[OMX_VLPB_OUTPUT_PORT_START_INDEX +
                           i]->format.video.cMIMEType = NULL;
    pVLPBComPvt->pPortdefs[OMX_VLPB_OUTPUT_PORT_START_INDEX +
                           i]->format.video.pNativeRender = NULL;
    pVLPBComPvt->pPortdefs[OMX_VLPB_OUTPUT_PORT_START_INDEX +
                           i]->format.video.nFrameWidth =
      OMX_VLPB_DEFAULT_OUTPUT_FRAME_WIDTH;
    pVLPBComPvt->pPortdefs[OMX_VLPB_OUTPUT_PORT_START_INDEX +
                           i]->format.video.nFrameHeight =
      OMX_VLPB_DEFAULT_OUTPUT_FRAME_HEIGHT;
    pVLPBComPvt->pPortdefs[OMX_VLPB_OUTPUT_PORT_START_INDEX +
                           i]->format.video.nStride =
      OMX_VLPB_DEFAULT_OUTPUT_FRAME_WIDTH;
    pVLPBComPvt->pPortdefs[OMX_VLPB_OUTPUT_PORT_START_INDEX +
                           i]->format.video.nSliceHeight = 0;
    pVLPBComPvt->pPortdefs[OMX_VLPB_OUTPUT_PORT_START_INDEX +
                           i]->format.video.xFramerate =
      OMX_VLPB_DEFAULT_FRAME_RATE;
    pVLPBComPvt->pPortdefs[OMX_VLPB_OUTPUT_PORT_START_INDEX +
                           i]->format.video.eCompressionFormat =
      OMX_VIDEO_CodingUnused;
    pVLPBComPvt->pPortdefs[OMX_VLPB_OUTPUT_PORT_START_INDEX +
                           i]->format.video.eColorFormat =
      OMX_COLOR_FormatYUV420SemiPlanar;
    pVLPBComPvt->pPortdefs[OMX_VLPB_OUTPUT_PORT_START_INDEX +
                           i]->format.video.pNativeWindow = NULL;

    /*-------------------------------------------------------------------------*
     *  Initialize the eNotifyType of the component with NONE as the data
     *  processing is based on the periodic driver call-back rather than usual
     *  DataNotify event
     *------------------------------------------------------------------------*/
    pVLPBComPvt->tDerToBase.pPortProperties[OMX_VLPB_OUTPUT_PORT_START_INDEX +
                                            i]->eNotifyType =
      OMX_NOTIFY_TYPE_ALWAYS;
  }
  
  
  pVLPBComPvt->bBuferFlush = OMX_FALSE;
  pVLPBComPvt->hDioGroupHandle = NULL;
  osalError = TIMM_OSAL_MutexCreate ( &pVLPBComPvt->pDataNotifyMutex );
  /* Check the input parameters, this should be TRUE else report an Error */
  OMX_BASE_REQUIRE ( ( osalError == TIMM_OSAL_ERR_NONE ),
                     OMX_ErrorInsufficientResources );

  for (i = 0; i < OMX_VLPB_NUM_INPUT_PORTS; i++)
  {
		Inport_entry = &pVLPBComPvt->omxVLPBInPort2WinMgr[i];
		Inport_entry->nPortIndex = ( OMX_VLPB_INPUT_PORT_START_INDEX + i );
		Inport_entry->nInBufHdrFreeIdx = 0;
    TIMM_OSAL_Memset (Inport_entry->sInBufHeader, 0,
                      sizeof (OMX_BASE_VIDEO_CUSTOMHEADER) *
                      OMX_VLPB_DEFAULT_INPUT_BUFFER_COUNT);
  }
  for (i = 0; i < OMX_VLPB_NUM_OUTPUT_PORTS; i++)
  {
		Outport_entry = &pVLPBComPvt->omxVLPBOutPort2WinMgr[i];
		Outport_entry->nPortIndex = ( OMX_VLPB_OUTPUT_PORT_START_INDEX + i );
		Outport_entry->nOutBufHdrFreeIdx = 0;
    TIMM_OSAL_Memset (Outport_entry->sOutBufHeader, 0,
                      sizeof (OMX_BASE_VIDEO_CUSTOMHEADER) *
                      OMX_VLPB_DEFAULT_OUTPUT_BUFFER_COUNT);
	}  
	pVLPBComPvt->bPrdTaskEnable = OMX_FALSE;
	pVLPBComPvt->bTaskExit = OMX_FALSE;
  osalError =
    TIMM_OSAL_SemaphoreCreate (&(pVLPBComPvt->pVlpbDataProcessPrdTskStartSem),
                               0);
  OMX_BASE_REQUIRE (osalError == TIMM_OSAL_ERR_NONE,
                    OMX_ErrorInsufficientResources);

	snprintf ( pVLPBComPvt->cVlpbDataProcessPrdTskName,
             ( sizeof ( pVLPBComPvt->cVlpbDataProcessPrdTskName ) - 1 ),
             "VlpbDataProcPrdTsk_%p", hComponent );
	osalError =
      TIMM_OSAL_CreateTask ( &pVLPBComPvt->pVlpbDataProcessPrdTsk,
                             &_OMX_VLPBDataProcPrdTaskFcn, 1, hComponent,
                             OMX_VLPB_STACKSIZE, OMX_VLPB_DEFAULT_TASKPRIORITY,
                             ( TIMM_OSAL_S8 * )
                             pVLPBComPvt->cVlpbDataProcessPrdTskName );
	OMX_BASE_REQUIRE ( osalError == TIMM_OSAL_ERR_NONE,
                     OMX_ErrorInsufficientResources );

	eError = OMX_ErrorNone;

EXIT:
  return eError;

}

/* ========================================================================== */

/**
********************************************************************************
 *  @func   _OMX_VLPB_InitialzeComponentExtraDataInfo ()
 *  @brief  This function performs one time initialization of the component
 *          Initialize the ComponentPrivate Fields, mainly the extra-data info
 *          of port propertiy structure required for VFDC derived component
 *
 * For setting the extra data
 * Other than the usual OMX buff hedaer, we required a couple of extra info
 * that required for the algorithm/driver to process.  These info shall be
 * exchange between the components and are allocated at the end of data
 * buffers as per OMX specificatins.  The extra data list as below
 *  OMX_U32 nChId         : Video Channel Identifier
 *  OMX_U32 nFrameWidth   : Frame Width
 *  OMX_U32 nFrameHeight  : Frame Height
 *  OMX_U32 nFramePitch   : Frame Pitch
 *  OMX_U32 nFrameType    : Frame Type (interlaced/progressive)
 *  OMX_U32 nChromaFormat : Picture chroma format
 *  OMX_U32 nHorOffset    : Active image x offset
 *  OMX_U32 nVerOffset    : Active image y offset
 *  OMX_U32 nCropWidth    : Active image width
 *  OMX_U32 nCropHeight   : Active image height
 *  OMX_PTR nCBufPtr      : Second buffer pointer
 *
 * @param hComponent  : Handle to the component data structure
 * @return
 * OMX_ErrorNone = Successful
 *
 * Other_value = Failed (Error code is returned)
 *
 */
/* ========================================================================== */

OMX_ERRORTYPE
_OMX_VLPB_InitialzeComponentExtraDataInfo (OMX_HANDLETYPE hComponent)
{
  OMX_ERRORTYPE eError = OMX_ErrorNone;

  OMX_COMPONENTTYPE *pHandle = NULL;

  OMX_VLPB_PVTTYPE *pVLPBComPvt = NULL;

  OMX_BASE_VIDEO_CUSTOMHEADER sCustomHeader;

  OMX_U32 i, j=0;

  /* Check the input parameters, this should be TRUE else report an Error */
  OMX_BASE_REQUIRE ( hComponent != NULL, OMX_ErrorBadParameter );

  pHandle = ( OMX_COMPONENTTYPE * ) hComponent;

  OMX_BASE_CHK_VERSION ( pHandle, OMX_COMPONENTTYPE, eError );

  pVLPBComPvt = ( OMX_VLPB_PVTTYPE * ) pHandle->pComponentPrivate;

  for (i = 0; i < OMX_VLPB_NUM_INPUT_PORTS; i++)
  {
    /*------------------------------------------------------------------------*/
    /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
    /* These values cannot be changed - they are based on the buffer header
       defined by the derived component. The number of meta data fields and
       their types should correspond to the header. The order of the field
       types should be exactly the same order as in the header */
    /*------------------------------------------------------------------------*/
    pVLPBComPvt->tDerToBase.pPortProperties[OMX_VLPB_INPUT_PORT_START_INDEX +
                                            i]->nNumMetaDataFields =
        OMX_BASE_VIDEO_EXTRADATA_NUM_FIELDS;
    pVLPBComPvt->tDerToBase.pPortProperties[OMX_VLPB_INPUT_PORT_START_INDEX +
                                            i]->pMetaDataFieldTypesArr =
        ( OMX_EXTRADATATYPE * )
        OMX_BASE_Malloc ( OMX_BASE_VIDEO_EXTRADATA_NUM_FIELDS *
                          sizeof ( OMX_EXTRADATATYPE ),
                          pVLPBComPvt->tDerToBase.hDefaultHeap );
    for (j = 0; j < (OMX_BASE_VIDEO_EXTRADATA_NUM_FIELDS - 1); j++)
    {
    pVLPBComPvt->tDerToBase.pPortProperties[OMX_VLPB_INPUT_PORT_START_INDEX +
                                            i]->pMetaDataFieldTypesArr[j] =
        OMX_ExtraDataVideo;
    }
    pVLPBComPvt->tDerToBase.pPortProperties[OMX_VLPB_INPUT_PORT_START_INDEX +
                                            i]->
      pMetaDataFieldTypesArr[(OMX_BASE_VIDEO_EXTRADATA_NUM_FIELDS - 1)] =
        (OMX_EXTRADATATYPE)OMX_ExtraDataSecondaryBufPtr;

    /* Update input port buffer size */
    /*------------------------------------------------------------------------*/
    /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /*  Using the DIO utility function, determine the buffersize that needs   */
    /*  to be allocated.                                                      */
    /*                                                                        */
    /*------------------------------------------------------------------------*/
    sCustomHeader.nAllocLen = OMX_VLPB_DEFAULT_INPUT_BUFFER_SIZE;
    /* The bit field indicated which of the meta data fields will be used by
       the component. The 1st field in the header should correspond to the LSB
       and so on. The bit field value can be changed at runtime. However here,
       only those bits should be turned off for which there will never be any
       data. Here maximum possible bits should be turned on so that size
       allocated is never less than required */
    sCustomHeader.nMetaDataUsageBitField =
        ( OMX_BASE_VIDEO_EXTRADATA_CHID_BIT |
          OMX_BASE_VIDEO_EXTRADATA_FRAME_WIDTH_BIT |
          OMX_BASE_VIDEO_EXTRADATA_FRAME_HEIGHT_BIT |
          OMX_BASE_VIDEO_EXTRADATA_FRAME_PITCH_BIT |
          OMX_BASE_VIDEO_EXTRADATA_FRAME_TYPE_BIT |
          OMX_BASE_VIDEO_EXTRADATA_CHROMA_FMT_BIT |
          OMX_BASE_VIDEO_EXTRADATA_HOR_OFFSET_BIT |
          OMX_BASE_VIDEO_EXTRADATA_VER_OFFSET_BIT |
          OMX_BASE_VIDEO_EXTRADATA_CROP_WIDTH_BIT |
          OMX_BASE_VIDEO_EXTRADATA_CROP_HEIGHT_BIT |
          OMX_BASE_VIDEO_EXTRADATA_CHROMA_BUF_BIT );
    eError =
        pVLPBComPvt->fpDioUtil ( hComponent,
                                 ( OMX_VLPB_INPUT_PORT_START_INDEX + i ),
                                 OMX_BASE_DIO_UTIL_GetTotalBufferSize,
                                 &sCustomHeader );
    OMX_BASE_ASSERT ( OMX_ErrorNone == eError, eError );
    /* AllocLen field now contains the size of the entire buffer that needs to*/
    /*------------------------------------------------------------------------*/
    /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /* Initialize the port def with buffer size obtained in the above step    */
    /*------------------------------------------------------------------------*/
    pVLPBComPvt->pPortdefs[( OMX_VLPB_INPUT_PORT_START_INDEX +
                             i )]->nBufferSize = sCustomHeader.nAllocLen;
  }
  
  for (i = 0; i < OMX_VLPB_NUM_OUTPUT_PORTS; i++)
  {
    /*------------------------------------------------------------------------*/
    /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /* These values cannot be changed - they are based on the buffer header
       defined by the derived component. The number of meta data fields and
       their types should correspond to the header. The order of the field
       types should be exactly the same order as in the header */
    /*------------------------------------------------------------------------*/
    pVLPBComPvt->tDerToBase.pPortProperties[OMX_VLPB_OUTPUT_PORT_START_INDEX +
                                            i]->nNumMetaDataFields =
      OMX_BASE_VIDEO_EXTRADATA_NUM_FIELDS;
    pVLPBComPvt->tDerToBase.pPortProperties[OMX_VLPB_OUTPUT_PORT_START_INDEX +
                                            i]->pMetaDataFieldTypesArr =
      (OMX_EXTRADATATYPE *) OMX_BASE_Malloc (OMX_BASE_VIDEO_EXTRADATA_NUM_FIELDS
                                             * sizeof (OMX_EXTRADATATYPE),
                                             pVLPBComPvt->tDerToBase.
                                             hDefaultHeap);
    for (j = 0; j < (OMX_BASE_VIDEO_EXTRADATA_NUM_FIELDS - 1); j++)
    {
      pVLPBComPvt->tDerToBase.pPortProperties[OMX_VLPB_OUTPUT_PORT_START_INDEX +
                                              i]->pMetaDataFieldTypesArr[j] =
        OMX_ExtraDataVideo;
    }
    pVLPBComPvt->tDerToBase.pPortProperties[OMX_VLPB_OUTPUT_PORT_START_INDEX +
                                            i]->
      pMetaDataFieldTypesArr[(OMX_BASE_VIDEO_EXTRADATA_NUM_FIELDS - 1)] =
        (OMX_EXTRADATATYPE)OMX_ExtraDataSecondaryBufPtr;

		/* Update Output port buffer size */
		/*------------------------------------------------------------------------*/
    /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		/*  Using the DIO utility function, determine the buffersize that needs   */
		/*  to be allocated.                                                      */
		/*                                                                        */
		/*------------------------------------------------------------------------*/
		sCustomHeader.nAllocLen = OMX_VLPB_DEFAULT_OUTPUT_BUFFER_SIZE;
		/* The bit field indicated which of the meta data fields will be used by
       the component. The 1st field in the header should correspond to the LSB
       and so on. The bit field value can be changed at runtime. However here,
       only those bits should be turned off for which there will never be any
       data. Here maximum possible bits should be turned on so that size
       allocated is never less than required */
		sCustomHeader.nMetaDataUsageBitField =
			 ( OMX_BASE_VIDEO_EXTRADATA_CHID_BIT 		|
			  OMX_BASE_VIDEO_EXTRADATA_FRAME_WIDTH_BIT  |
			  OMX_BASE_VIDEO_EXTRADATA_FRAME_HEIGHT_BIT |
			  OMX_BASE_VIDEO_EXTRADATA_FRAME_PITCH_BIT  |
			  OMX_BASE_VIDEO_EXTRADATA_FRAME_TYPE_BIT |
			  OMX_BASE_VIDEO_EXTRADATA_CHROMA_FMT_BIT |
			  OMX_BASE_VIDEO_EXTRADATA_HOR_OFFSET_BIT |
			  OMX_BASE_VIDEO_EXTRADATA_VER_OFFSET_BIT |
			  OMX_BASE_VIDEO_EXTRADATA_CROP_WIDTH_BIT |
			  OMX_BASE_VIDEO_EXTRADATA_CROP_HEIGHT_BIT |
			  OMX_BASE_VIDEO_EXTRADATA_CHROMA_BUF_BIT );
		eError =  pVLPBComPvt->fpDioUtil ( hComponent,
                                 ( OMX_VLPB_OUTPUT_PORT_START_INDEX + i ),
                                 OMX_BASE_DIO_UTIL_GetTotalBufferSize,
                                 &sCustomHeader );
		OMX_BASE_ASSERT ( OMX_ErrorNone == eError, eError );
    /* AllocLen field now contains the size of the entire buffer that needs to*/
		/*------------------------------------------------------------------------*/
    /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    /* Initialize the port def with buffer size obtained in the above step    */
    /*------------------------------------------------------------------------*/
    pVLPBComPvt->pPortdefs[(OMX_VLPB_OUTPUT_PORT_START_INDEX + i)]->
      nBufferSize = sCustomHeader.nAllocLen;
  }  

EXIT:
  /* NOTE: If the data size changes due to some change in settings by the
     client then this entire procedure has to be followed again. The alloc len
     field will contain only the size of the data buffer. Size of any packet
     data will be specified separately as above. The alloc len field will then
     return the total buffer size that needs to be allocated */
  return eError;
}

OMX_ERRORTYPE
_OMX_VLPBFlushBuffer (OMX_HANDLETYPE hComponent)
{

  OMX_ERRORTYPE eError = OMX_ErrorUndefined;

  OMX_COMPONENTTYPE *pHandle = NULL;

  OMX_VLPB_PVTTYPE *pVLPBComPvt = NULL;

  /*--------------------------------------------------------------------------*/
  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
  /* Verify the input pointers                                                */
  /*                                                                          */
  /*--------------------------------------------------------------------------*/
  OMX_BASE_REQUIRE ( hComponent != NULL, OMX_ErrorBadParameter );

  pHandle = ( OMX_COMPONENTTYPE * ) hComponent;

  /*--------------------------------------------------------------------------*/
  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
  /* Verify the version                                                       */
  /*                                                                          */
  /*--------------------------------------------------------------------------*/
  OMX_BASE_CHK_VERSION ( pHandle, OMX_COMPONENTTYPE, eError );

  pVLPBComPvt = ( OMX_VLPB_PVTTYPE * ) pHandle->pComponentPrivate;

  ( Void ) pVLPBComPvt;

  eError = OMX_ErrorNone;

EXIT:
  return eError;
}

OMX_ERRORTYPE
_OMX_VLPBDisableChannel (OMX_HANDLETYPE hComponent, OMX_U32 nParam)
{
  OMX_ERRORTYPE eError = OMX_ErrorUndefined;

  OMX_VLPB_PVTTYPE *pVLPBComPvt = NULL;

  OMX_COMPONENTTYPE *pHandle = NULL;

  /*--------------------------------------------------------------------------*/
  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
  /* Verify the input pointers                                                */
  /*                                                                          */
  /*--------------------------------------------------------------------------*/
  OMX_BASE_REQUIRE ( ( hComponent != NULL ), OMX_ErrorBadParameter );

  pHandle = ( OMX_COMPONENTTYPE * ) hComponent;
  pVLPBComPvt = ( OMX_VLPB_PVTTYPE * ) pHandle->pComponentPrivate;

  ( Void ) pVLPBComPvt;
  /*Send back any buffers in the OMX ports*/
  eError = OMX_VLPBFlushChannelBuf ( hComponent );
  OMX_BASE_ASSERT ( OMX_ErrorNone == eError, eError );

EXIT:
  return eError;
}

/* Nothing beyond this point */
