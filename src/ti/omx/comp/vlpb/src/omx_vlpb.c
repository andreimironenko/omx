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
 *   @file  omx_vlpb.c
 *          This file contains methods that provides the functionality for a
 *          VLPB openmax1.1 component. This can be used as reference to
 *          develop Khronos-OpenMAX1.1 components with the help of a "Base
 *          component" which covers most of the omx protocol implementation.
 *          This component copies a buffer on its input port to a buffer on the
 *          output port.
 *
 *
 *   @path \ti\omx\comp\vlpb\src
 *
 *  @rev  1.0
 */
/* -------------------------------------------------------------------------- */
/* ======================================================================== !
 * Revision History !
 * ======================================================================== !
 * 02-September-2009 Badri Narayanan S Initial Version
 * ========================================================================= */

/******************************************************************************
* Includes
******************************************************************************/
/* ----- system and platform files ---------------------------- */
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
/* Debug switches that can be defined for debug purpose only */
/* Prints messages to console */
//#define VLPB_DEBUG
/* Writes the data being copied from the input to the output port to a file */
//#define VLPB_RECORD_BITSREAM
#ifdef VLPB_RECORD_BITSREAM
/* Defines the number of frames to be written to file for debug */
#define VLPB_NUM_FRAMES_TO_RECORD (60)
#endif

/*-------program files ----------------------------------------*/
#include <OMX_Core.h>
#include <OMX_Component.h>
#include <omx_base.h>
#include <omx_base_utils.h>
#include <timm_osal_interfaces.h>

#include "omx_vlpb.h"
#include "omx_vlpb_priv.h"
#include "omx_vlpb_utils.h"
#include <ti/omx/comp/omxbase/omx_base_comp/inc/omx_base_internal.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Error.h>
#ifdef _LOCAL_CORE_c6xdsp_
#include <ti/sdo/fc/memutils/memutils.h>
#endif

/*****************************************************************************
 * EXTERNAL REFERENCES NOTE : only use if not found in header file
*****************************************************************************/

/*-----------------------data declarations ----------------------------------*/
OMX_BOOL g_pVLPBDataProcessPrdTskStartSemPosted = OMX_FALSE;

/*--------------------- function prototypes ---------------------------------*/
/** Callback from Base to Derived to Notify Command */
static OMX_ERRORTYPE OMX_TI_VLPB_CommandNotify ( OMX_HANDLETYPE hComponent,
                       OMX_COMMANDTYPE Cmd,
                                                OMX_U32 nParam,
                                                OMX_PTR pCmdData);

OMX_ERRORTYPE OMX_VLPB_FlushChannelBuf ( OMX_HANDLETYPE hComponent );
/*****************************************************************************
 * PUBLIC DECLARATIONS Defined here, used elsewhere
 *****************************************************************************/

/*---------------------data declarations -----------------------------------*/


/*---------------------function prototypes ---------------------------------*/

#ifdef DYN_PORT_RECONFIG_TEST

static OMX_ERRORTYPE _OMX_TI_VLPB_ParseBufferHeader ( OMX_HANDLETYPE hComponent,
                                                      OMX_BUFFERHEADERTYPE *
                                                      pBufHdr );
#endif

/*****************************************************************************
 * PRIVATE DECLARATIONS Defined here, used only here
 *****************************************************************************/

/*--------------------------- macros  ---------------------------------------*/

/*-------------------------data declarations --------------------------------*/
OMX_U32 frameNumber[OMX_VLPB_NUM_INPUT_PORTS] ={0};      

/*-------------------------function prototypes -------------------------------*/
/* ========================================================================== */

/**
* @fn OMX_TI_VLPB_ComponentInit():
*     This is the first call into a component and is used to perform one time
*     Initialization specific to a component.Component may assume that the
*     Application has allocated the handle and filled in nSize and nVersion
*     fields.This method should also allocate and fill in the component
*     Private data structure.
*
* @param [in] hComponent   :   Handle of the component to be accessed.
*/
/* ========================================================================== */
OMX_ERRORTYPE
OMX_TI_VLPB_ComponentInit (OMX_HANDLETYPE hComponent)
{
  OMX_VLPB_PVTTYPE *pVLPBComPvt;

  OMX_COMPONENTTYPE *pHandle;

  OMX_ERRORTYPE eError = OMX_ErrorNone;

  TIMM_OSAL_ERRORTYPE tStatus = TIMM_OSAL_ERR_NONE;

  /* check input parameters,this should be TRUE else report an Error */
  OMX_BASE_REQUIRE ( hComponent != NULL, OMX_ErrorBadParameter );

  pHandle = ( OMX_COMPONENTTYPE * ) hComponent;

  OMX_BASE_CHK_VERSION ( pHandle, OMX_COMPONENTTYPE, eError );

  /* Allocate memory required for component private area. If derived component
     creates a default heap then use that instead of NULL */
  pHandle->pComponentPrivate =
      ( OMX_VLPB_PVTTYPE * ) TIMM_OSAL_MallocExtn ( sizeof ( OMX_VLPB_PVTTYPE ),
                                                    TIMM_OSAL_TRUE, 0,
                                               TIMMOSAL_MEM_SEGMENT_EXT, NULL);
  OMX_BASE_ASSERT ( pHandle->pComponentPrivate != NULL,
                    OMX_ErrorInsufficientResources );

  tStatus =
      TIMM_OSAL_Memset ( pHandle->pComponentPrivate, 0x0,
                         sizeof ( OMX_VLPB_PVTTYPE ) );
  OMX_BASE_ASSERT ( TIMM_OSAL_ERR_NONE == tStatus, OMX_ErrorUndefined );

  /*--------------------------------------------------------------------------*/
  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /* Following are the initializations:                                       */
  /* 1. Initialize the Component Private handle                               */
  /* 2. Derived to Base params with Actual values                             */
  /* 3. initialize the Hooks to Notify Command and Data from Base Comp to     */
  /*    Derived Comp                                                          */
  /* 4. Initialize the Base Component                                         */
  /* 5. Override the functions provided by the Base component                 */
  /* 6. Initialize the Other Component Private fields                         */
  /* 7. Incase of any error, roll back the initializations                    */
  /*--------------------------------------------------------------------------*/
  /*--------------------------------------------------------------------------*/
  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /* Step1: Initialize the Component Private handle                           */
  /*                                                                          */
  /*--------------------------------------------------------------------------*/
  pVLPBComPvt = ( OMX_VLPB_PVTTYPE * ) pHandle->pComponentPrivate;

  /*--------------------------------------------------------------------------*/
  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /* Step2: Initialize Derived to Base params with Actual values              */
  /*                                                                          */
  /*--------------------------------------------------------------------------*/
  eError = _OMX_VLPB_SetDerToBaseParameters ( hComponent );

  OMX_BASE_ASSERT ( OMX_ErrorNone == eError, eError );
  /*--------------------------------------------------------------------------*/
  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /* Step3: Initialize the Data and commandnotify hooks                       */
  /*                                                                          */
  /*--------------------------------------------------------------------------*/
  pVLPBComPvt->tDerToBase.fpCommandNotify = OMX_TI_VLPB_CommandNotify;
  pVLPBComPvt->tDerToBase.fpDataNotify = OMX_TI_VLPB_DataNotify;

  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /* Step4: Initialize the Base component                                     */
  /*                                                                          */
  /*--------------------------------------------------------------------------*/
  eError = OMX_BASE_ComponentInit ( hComponent );
  OMX_BASE_ASSERT ( OMX_ErrorNone == eError, eError );
  /*--------------------------------------------------------------------------*/
  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /* Step5: Override the functions provided by the Base component             */
  /*        as these are specific to the VLPB                                 */
  /*                                                                          */
  /*--------------------------------------------------------------------------*/
  pHandle->GetParameter = OMX_TI_VLPB_GetParameter;
  pHandle->SetParameter = OMX_TI_VLPB_SetParameter;
  pHandle->SetConfig = OMX_TI_VLPB_SetConfig;
  pHandle->GetConfig = OMX_TI_VLPB_GetConfig;
  pHandle->ComponentDeInit = OMX_TI_VLPB_ComponentDeinit;

  /*--------------------------------------------------------------------------*/
  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /* Step6: Initialize the private fields                                     */
  /*                                                                          */
  /*--------------------------------------------------------------------------*/
  eError = _OMX_VLPB_InitialzeComponentPrivateParams ( pHandle );
  OMX_BASE_ASSERT ( OMX_ErrorNone == eError, eError );

  /* Initialize the Other Component Private fields */
  eError = _OMX_VLPB_InitialzeComponentExtraDataInfo ( pHandle );
  OMX_BASE_ASSERT ( OMX_ErrorNone == eError, eError );
  
  g_pVLPBDataProcessPrdTskStartSemPosted = OMX_FALSE;

EXIT:
  /*--------------------------------------------------------------------------*/
  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /* Step7: Incase of error, deinitialize the component                       */
  /*                                                                          */
  /*--------------------------------------------------------------------------*/
  if (eError == OMX_ErrorInsufficientResources)
  {
    OMX_BASE_Error ( " Component Initialization Failed...! " );
    pHandle->ComponentDeInit ( hComponent );
  }
  return eError;
}


/* ========================================================================== 
* @fn OMX_TI_VLPB_ComponentDeinit():
*     This method will deinitalize the component which
*
* @param [in] hComponent : Handle of the component to be accessed.
*
 *  
 *  @return  OMX_ErrorNone 
 *           OMX_ErrorBadParameter
 *           OMX_ErrorVersionMismatch
 *           OMX_ErrorUndefined
********************************************************************************
*/
OMX_ERRORTYPE
OMX_TI_VLPB_ComponentDeinit (OMX_HANDLETYPE hComponent)
{
  OMX_COMPONENTTYPE *pComp;

  OMX_VLPB_PVTTYPE *pVLPBComPvt;

  OMX_ERRORTYPE eError = OMX_ErrorNone;

  OMX_U32 nIndex = 0;

  OMX_U32 i = 0;

  TIMM_OSAL_ERRORTYPE osalError = TIMM_OSAL_ERR_NONE;
  /*--------------------------------------------------------------------------*/
  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /* Verify the input pointers                                                */
  /*--------------------------------------------------------------------------*/
  OMX_BASE_REQUIRE ( hComponent != NULL, OMX_ErrorBadParameter );

  /*--------------------------------------------------------------------------*/
  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /* Verify the version                                                       */
  /*                                                                          */
  /*--------------------------------------------------------------------------*/
  pComp = ( OMX_COMPONENTTYPE * ) hComponent;
  pVLPBComPvt = ( OMX_VLPB_PVTTYPE * ) pComp->pComponentPrivate;
  
  pVLPBComPvt->bTaskExit = OMX_TRUE;
  if (OMX_FALSE == pVLPBComPvt->bPrdTaskEnable)
  {
    TIMM_OSAL_SemaphoreRelease ( pVLPBComPvt->pVlpbDataProcessPrdTskStartSem );
  }  
  
  do
  {
  osalError = TIMM_OSAL_DeleteTask ( pVLPBComPvt->pVlpbDataProcessPrdTsk ); 
  } while(TIMM_OSAL_ERR_NONE != osalError);
  
  if (TIMM_OSAL_ERR_NONE != osalError)
  {
    eError = OMX_ErrorUndefined;
    OMX_BASE_ASSERT ( eError == OMX_ErrorNone, eError );
  }
  
  osalError = TIMM_OSAL_SemaphoreDelete ( pVLPBComPvt->
                                          pVlpbDataProcessPrdTskStartSem ); 
  
  if (TIMM_OSAL_ERR_NONE != osalError)
  {
    eError = OMX_ErrorUndefined;
    OMX_BASE_ASSERT ( eError == OMX_ErrorNone, eError );
  } 
  
  /*--------------------------------------------------------------------------*/
  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /* Un-initialize the Base Component                                         */
  /*                                                                          */
  /*--------------------------------------------------------------------------*/
  eError = OMX_BASE_ComponentDeinit ( hComponent );
  OMX_BASE_ASSERT ( eError == OMX_ErrorNone, eError );

  for (nIndex = 0; nIndex < pVLPBComPvt->tDerToBase.nNumPorts; nIndex++)
  {
    TIMM_OSAL_Free ( pVLPBComPvt->tDerToBase.pPortProperties[nIndex] );
  }

  TIMM_OSAL_Free ( pVLPBComPvt->tDerToBase.pOtherPortParams );
  TIMM_OSAL_Free ( pVLPBComPvt->tDerToBase.pImagePortParams );
  TIMM_OSAL_Free ( pVLPBComPvt->tDerToBase.pVideoPortParams );
  TIMM_OSAL_Free ( pVLPBComPvt->tDerToBase.pAudioPortParams );
  for (i = 0; i < OMX_VLPB_NUM_INPUT_PORTS; i++)
  {
  TIMM_OSAL_Free ( pVLPBComPvt->tDerToBase.
                     pPortProperties[OMX_VLPB_INPUT_PORT_START_INDEX + i]->
                   pMetaDataFieldTypesArr );
  }
  for (i = 0; i < OMX_VLPB_NUM_OUTPUT_PORTS; i++)
  {
  TIMM_OSAL_Free ( pVLPBComPvt->tDerToBase.
                     pPortProperties[OMX_VLPB_OUTPUT_PORT_START_INDEX + i]->
                   pMetaDataFieldTypesArr );          
  }
  osalError = TIMM_OSAL_MutexDelete ( pVLPBComPvt->pDataNotifyMutex );
  /* Check the input parameters, this should be TRUE else report an Error */
  OMX_BASE_REQUIRE ( ( osalError == TIMM_OSAL_ERR_NONE ),
                     OMX_ErrorInsufficientResources );


  TIMM_OSAL_Free ( pVLPBComPvt->tDerToBase.pPortProperties );
  TIMM_OSAL_Free ( pVLPBComPvt->tDerToBase.cComponentName );
  TIMM_OSAL_Free ( pVLPBComPvt );
  pVLPBComPvt = NULL;

EXIT:
  return eError;
}

/* ========================================================================== */

/**
* @fn OMX_TI_VLPB_GetParameter():
*     This method will get one of the parameter settings from the component.
*     nParamIndex specifies which structure is requested from the comp.Caller
*     shall provide memory for the structure and populate nSize and nVersion
*     fields before invoking this macro.
*     This method redirects the call to OMX_BASE_GetParameter for handling
*     standard component parameters and tunneling.
*
* @param [in]    hComponent     : Handle of the component to be accessed.
* @param [in]    nParamIndex    : Index of the structure to be filled
* @param [inout] pParamStrucut  : Ptr to the client allocated structure
*
* PREQUISITES: Invoked when the comp is in any state except the Invalid state.
*
* Note : Handle all domain specific indexes over here.
*/
/* ========================================================================== */
OMX_ERRORTYPE
OMX_TI_VLPB_GetParameter (OMX_HANDLETYPE hComponent,
                          OMX_INDEXTYPE nParamIndex, OMX_PTR pParamStruct)
{
  OMX_VLPB_PVTTYPE *pVLPBComPvt;

  OMX_COMPONENTTYPE *pHandle;

  OMX_ERRORTYPE eError = OMX_ErrorNone;

  /* Check all the input parametrs */
  OMX_BASE_REQUIRE ( ( hComponent != NULL )
                     && ( pParamStruct != NULL ), OMX_ErrorBadParameter );

  pHandle = ( OMX_COMPONENTTYPE * ) hComponent;
  pVLPBComPvt = ( OMX_VLPB_PVTTYPE * ) pHandle->pComponentPrivate;

  /* This method cannot be ivoked in OMX_StateInvalid state */
  OMX_BASE_ASSERT ( pVLPBComPvt->tCurState != OMX_StateInvalid,
                    OMX_ErrorIncorrectStateOperation );

  /* Only the component specific Getparameters are implemented here. Items
     which common across all the components are redirected to the OMX base via
     the default case and are implemented part of OMX base component. Add
     appropriate case statements here for any component specific GetParameters*/
  switch (nParamIndex)
  {
    /*-------------------------------------------------------------------------*
     *  Get the component NotifyType. The components could
     *  be configured as low latency components, where the data processing
     *  is based on DataNotify rather than the periodic task.
     *------------------------------------------------------------------------*/
    case OMX_TI_IndexParamCompPortNotifyType:
      {
      OMX_PARAM_COMPPORT_NOTIFYTYPE *pNotifyType =
          ( OMX_PARAM_COMPPORT_NOTIFYTYPE * ) pParamStruct;
      pNotifyType->eNotifyType = pVLPBComPvt->tDerToBase.
                   pPortProperties[pNotifyType->nPortIndex]->eNotifyType;
      eError = OMX_ErrorNone;
      break;
      }

    /* redirect to "OMX_BASE_GetParameter" to handle standard indexes */
    default:
      eError = OMX_BASE_GetParameter ( hComponent, nParamIndex, pParamStruct );
      break;
  }

EXIT:
  return eError;
}

/* ========================================================================== */

/**
* @fn OMX_TI_VLPB_SetParameter():
*     This method will send a parameter structure to a component. nParamIndex
*     indicates which structure is passed to the comp.Caller shall provide
*     memory for the correct structure and shall fill in the nSize and nVersion
*     fields before invoking this macro.
*     This method redirects the call to OMX_BASE_SetParameter for handling
*     standard component parameters and tunneling.
*
* @param [in]    hComponent    : Handle of the component to be accessed.
* @param [in]    nParamIndex   : Index of the structure that is to be sent
* @param [inout] pParamStrucut : Ptr to client allocated structure
*
* PREQUISITES: Invoked when the comp is in Loaded state or on disabled port.
*
* Note : Handle all domain specific indexes over here.
*/
/* ========================================================================== */
OMX_ERRORTYPE
OMX_TI_VLPB_SetParameter (OMX_HANDLETYPE hComponent,
                          OMX_INDEXTYPE nParamIndex, OMX_PTR pParamStruct)
{
  OMX_VLPB_PVTTYPE *pVLPBComPvt;

  OMX_COMPONENTTYPE *pHandle;

  OMX_ERRORTYPE eError = OMX_ErrorNone;

  OMX_BASE_REQUIRE ( ( hComponent != NULL )
                     && ( pParamStruct != NULL ), OMX_ErrorBadParameter );

  pHandle = ( OMX_COMPONENTTYPE * ) hComponent;
  pVLPBComPvt = ( OMX_VLPB_PVTTYPE * ) pHandle->pComponentPrivate;

  /* This method can be invoked only when the comp is in OMX_StateLoaded or on
     a port that is disabled */
  OMX_BASE_ASSERT ( ( pVLPBComPvt->tCurState == OMX_StateLoaded ) ||
                    ( nParamIndex == OMX_IndexParamCompBufferSupplier ),
                    OMX_ErrorIncorrectStateOperation );

  /* Only the component specific Setparameters are implemented here. Items
     which common across all the components are redirected to the OMX base via
     the default case and are implemented part of OMX base component. Add
     appropriate case statements here for any component specific SetParameters*/
  switch (nParamIndex)
  {
    /*-------------------------------------------------------------------------*
     *  Initialize the component NotifyType. The components could
     *  be configured as low latency components, where the data processing
     *  is based on DataNotify rather than the periodic task.
     *------------------------------------------------------------------------*/
    case OMX_TI_IndexParamCompPortNotifyType:
      {
      OMX_PARAM_COMPPORT_NOTIFYTYPE *pNotifyType =
          ( OMX_PARAM_COMPPORT_NOTIFYTYPE * ) pParamStruct;
      pVLPBComPvt->tDerToBase.pPortProperties[pNotifyType->nPortIndex]->
                                  eNotifyType = pNotifyType->eNotifyType;
      eError = OMX_ErrorNone;
      break;
      }

    /* Redirect to "OMX_BASE_SetParameter" to handle standard indexes */
    default:
      eError = OMX_BASE_SetParameter ( hComponent, nParamIndex, pParamStruct );
      break;
  }

EXIT:
  return eError;
}

/* ========================================================================== */

/**
* @fn OMX_TI_VLPB_SetConfig():
*     This method will send one of the configurations struct to a comp.Caller
*     shall provide the memory for the correct structure and fill in the size
*     and version fields before invoking this macro.
*
* @param [in] hComponent  : Handle of the component to be accessed.
* @param [in] nIndex      : Index of the strucutre to be filled.
* @param [in] pConfigData : ptr to the client allocated structure
*
* PREQUISITES: Can be invoked in any state except OMX_StateInvalid.
*
* Note: Handle all configuration indexes over here, As this call is
*       not redirected to Base component.
*/
/* ========================================================================== */
OMX_ERRORTYPE
OMX_TI_VLPB_SetConfig (OMX_HANDLETYPE hComponent,
                       OMX_INDEXTYPE nIndex, OMX_PTR pConfigData)
{
  OMX_VLPB_PVTTYPE *pVLPBComPvt;

  OMX_COMPONENTTYPE *pHandle;

  OMX_ERRORTYPE eError = OMX_ErrorNone;

  OMX_BASE_REQUIRE ( ( hComponent != NULL )
                     && ( pConfigData != NULL ), OMX_ErrorBadParameter );

  pHandle = ( OMX_COMPONENTTYPE * ) hComponent;
  pVLPBComPvt = ( OMX_VLPB_PVTTYPE * ) pHandle->pComponentPrivate;

  /* This method can't be invoked in OMX_StateInvalid state */
  OMX_BASE_ASSERT ( pVLPBComPvt->tCurState != OMX_StateInvalid,
                    OMX_ErrorIncorrectStateOperation );

  /* Take care of Supported Indexes over here */
  switch (nIndex)
  {
    case OMX_TI_IndexConfigChannelGroupInfo:
      {
        OMX_CONFIG_CHANNELGROUPINFO *chGrpInfo = pConfigData;

        eError = pVLPBComPvt->fpDioGroupCreate ( hComponent, ( String )
                                                 chGrpInfo->cChannelGroupName,
                                                 chGrpInfo->auPortGroupIndex,
                                                 chGrpInfo->nNumPortsInGroup,
                                                 &pVLPBComPvt->hDioGroupHandle );
        break;
      }
    /* Redirect to "OMX_BASE_SetConfig" to handle standard indexes */
    default:
      return OMX_BASE_SetConfig ( hComponent, nIndex, pConfigData );
  }

EXIT:
  return eError;
}

/* ========================================================================== */

/**
* @fn OMX_TI_VLPB_GetConfig():
*     This methos will get a configuration structure from a comp. The caller
*     shall provide the memory for the correct structure and populate the
*     nSize and nVersion fields before invoking this macro.
*
* @param [in]    hComponent  : Handle of the component to be accessed.
* @param [in]    nIndex      : Index of the structure to be filled.
* @param [inout] pConfigData : Pointer to the IL client structure
*                              that the component fills.
*
* PREQUISITES: Can be invoked in any state except OMX_StateInvalid.
*
* Note: Handle all configuration indexes over here, As this call is
*       not redirected to Base component.
*/
/* ========================================================================== */
OMX_ERRORTYPE
OMX_TI_VLPB_GetConfig (OMX_HANDLETYPE hComponent,
                       OMX_INDEXTYPE nIndex, OMX_PTR pConfigData)
{
  OMX_VLPB_PVTTYPE *pVLPBComPvt;

  OMX_COMPONENTTYPE *pHandle;

  OMX_ERRORTYPE eError = OMX_ErrorNone;

  OMX_BASE_REQUIRE ( ( hComponent != NULL )
                     && ( pConfigData != NULL ), OMX_ErrorBadParameter );

  pHandle = ( OMX_COMPONENTTYPE * ) hComponent;
  pVLPBComPvt = ( OMX_VLPB_PVTTYPE * ) pHandle->pComponentPrivate;

  /* This method can't be invoked when the comp is in OMX_StateInvalid */
  OMX_BASE_REQUIRE ( pVLPBComPvt->tCurState != OMX_StateInvalid,
                     OMX_ErrorIncorrectStateOperation );

  switch (nIndex)
  {
    /* Redirect to "OMX_BASE_GetConfig" to handle standard indexes */
  default:
    return OMX_BASE_GetConfig (hComponent, nIndex, pConfigData);
  }

EXIT:
  return eError;
}

/***** Implementation of Command and Data Notify Hooks *****/

/* ========================================================================= */

/**
* @name  OMX_TI_VLPB_CommandNotify()
 *         This method is a Hook from the Base component to
*        Notify commands which needs to be executed by the
*        component. This method should complete all  the
 *         typical allocation of resources or  operations like
*        Alg instance create/delete depending on the cmd requested.
*        once all the operations have been done notify the Base
*        component via an ReturnEventNotify call.
*
* Loaded --> Idle: Buffers should be allocated
 *  Idle --> Executing: Enables the periodic task
* Executing --> Idle:  Flush the buffers from the driver & Stop the Driver
* Idle --> Loaded: Delete the driver; De-allocate the buffers 
* Invalid ---> Any other state: Delete the driver; De-allocate the buffers
* Executing --> Pause: Not supported
* Pause      --> Executing: Not supported.
* Pause --> Idle: Not supported
* For Flush Command: Flush the Driver Channel
* For Port Disable Command: Disable the Driver Channel
* For Port Enable Command: Enable the Driver Channel
*
* @param [in]    hComponent:   Handle of the component to be accessed.
* @param [in]    Cmd:   Command for the component to execute
* @param [in]    nParam : Integer parameter for the
*                         command that is to be executed
* @param [in]    pCmdData:Pointer that contains implementation
*                specific data that cannot be represented with
*                the numeric parameter nParam
*
 *           OMX_ErrorIncorrectStateOperation
 *           OMX_ErrorBadParameter
 *           OMX_ErrorVersionMismatch
 *           OMX_ErrorInsufficientResources
*/
/* ========================================================================= */
static OMX_ERRORTYPE
OMX_TI_VLPB_CommandNotify (OMX_HANDLETYPE hComponent,
                           OMX_COMMANDTYPE Cmd,
                           OMX_U32 nParam, OMX_PTR pCmdData)
{
  OMX_ERRORTYPE eError = OMX_ErrorUndefined;

  OMX_VLPB_PVTTYPE *pVLPBComPvt;

  OMX_COMPONENTTYPE *pHandle = NULL;

  OMX_BASE_INTERNALTYPE* pBaseComInt = NULL;

  OMX_BASE_PRIVATETYPE* pBaseComPvt = NULL;

  OMX_STATETYPE tCurState, tNewState;
  
  OMX_U32 nInMsgCount = 0;
 
  OMX_BASE_VIDEO_CUSTOMHEADER *pInBufHeader;

  OMX_BASE_VIDEO_CUSTOMHEADER InBuf;
 
  pInBufHeader = &InBuf;
  /*--------------------------------------------------------------------------*/
  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /* Verify the input pointers                                                */
  /*                                                                          */
  /*--------------------------------------------------------------------------*/
  OMX_BASE_REQUIRE ( hComponent != NULL, OMX_ErrorBadParameter );

  /*--------------------------------------------------------------------------*/
  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /* Verify the version                                                       */
  /*                                                                          */
  /*--------------------------------------------------------------------------*/
  pHandle = ( OMX_COMPONENTTYPE * ) hComponent;
  pVLPBComPvt = ( OMX_VLPB_PVTTYPE * ) pHandle->pComponentPrivate;
  pBaseComPvt = (OMX_BASE_PRIVATETYPE *)pHandle->pComponentPrivate;
  pBaseComInt = (OMX_BASE_INTERNALTYPE *)pBaseComPvt->hOMXBaseInt;
  OMX_BASE_CHK_VERSION ( pHandle, OMX_COMPONENTTYPE, eError );

  /*--------------------------------------------------------------------------*/
  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /* Get the current state and the next state. Accordingly,process the command*/
  /*                                                                          */
  /*--------------------------------------------------------------------------*/
  TIMM_OSAL_MutexObtain ( pVLPBComPvt->pMutex, TIMM_OSAL_SUSPEND );
  tCurState = pVLPBComPvt->tCurState;
  tNewState = pVLPBComPvt->tNewState;
  TIMM_OSAL_MutexRelease ( pVLPBComPvt->pMutex );

  /*--------------------------------------------------------------------------*/
  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /* Following commands are handled here                                      */
  /*    1. OMX_CommandStateSet                                                */
  /*    2. OMX_CommandFlush                                                   */
  /*    3. OMX_CommandPortDisable                                             */
  /*    4. OMX_CommandPortEnable                                              */
  /*                                                                          */
  /*--------------------------------------------------------------------------*/
  switch (Cmd)
  {

    case OMX_CommandStateSet:
      /*----------------------------------------------------------------------*/
      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
      /* (loaded -> idle), Nothing is done                                    */
      /*----------------------------------------------------------------------*/
    if ((tCurState == OMX_StateLoaded) && (tNewState == OMX_StateIdle))
    {
      }
      /*----------------------------------------------------------------------*/
      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
      /* (idle -> executing), Enable the periodic process                     */
      /*                                                                      */
      /*----------------------------------------------------------------------*/
    else if ((tCurState == OMX_StateIdle) && (tNewState == OMX_StateExecuting))
    {
        _OMX_VLPBEnablePeriodicDataProcess (hComponent);
      }
      /*----------------------------------------------------------------------*/
      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
      /* executing -> idle, return all the buffers back to the IL client      */
      /*                                                                      */
      /*----------------------------------------------------------------------*/
      else if ( ( ( tCurState == OMX_StateExecuting )
              && (tNewState == OMX_StateIdle)))
    {
        eError = OMX_VLPBFlushChannelBuf ( hComponent );
        OMX_BASE_ASSERT ( eError == OMX_ErrorNone, eError );
     }
      /*----------------------------------------------------------------------*/
      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
      /* Pause -> Executing, Not supported                                    */
      /*                                                                      */
      /*----------------------------------------------------------------------*/
    else if ((tCurState == OMX_StatePause) && (tNewState == OMX_StateExecuting))
    {
                /*===================================
                need to update if needed
                =================================*/
      }
      /*----------------------------------------------------------------------*/
      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
      /* Executing -> pause, Not supported                                   */
      /*                                                                      */
      /*----------------------------------------------------------------------*/
    else if ((tCurState == OMX_StateExecuting) && (tNewState == OMX_StatePause))
    {
                /*===================================
                need to update if needed
                =================================*/
      }
      /*----------------------------------------------------------------------*/
      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
      /* Idle -> Loaded, Return any OMX port buffers held by component        */
      /*                                                                      */
      /*----------------------------------------------------------------------*/
    else if ((tCurState == OMX_StateIdle) && (tNewState == OMX_StateLoaded))
    {
      }
    else if (tNewState == OMX_StateInvalid)
    {
      /*----------------------------------------------------------------------*/
      /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
      /* Invalid state transition.  */
      /* */
      /*----------------------------------------------------------------------*/
    }
    else
    {
       /*--------------------------------------------------------------------*/
       /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
       /*  Invalid state transition.                                         */
       /*                                                                    */
       /*--------------------------------------------------------------------*/
      }
      break;

    case OMX_CommandFlush:

    OMX_BASE_ASSERT (((nParam <
                            OMX_VLPB_INPUT_PORT_START_INDEX +
                            OMX_VLPB_NUM_INPUT_PORTS + OMX_VLPB_NUM_OUTPUT_PORTS ) )
                        || ( nParam == OMX_ALL ), OMX_ErrorBadParameter );

      /*--------------------------------------------------------------------*/
      /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
      /*  Do the flush operation.                                           */
      /*                                                                    */
      /*--------------------------------------------------------------------*/
      eError = OMX_VLPBFlushChannelBuf ( hComponent );
      OMX_BASE_ASSERT ( eError == OMX_ErrorNone, eError );
      break;

    case OMX_CommandPortDisable:
 
    if(tCurState != OMX_StateLoaded) 
    {
     /* Make sure nParam corresponds to a valid port */
      OMX_BASE_ASSERT (((nParam < OMX_VLPB_INPUT_PORT_START_INDEX +
               OMX_VLPB_NUM_INPUT_PORTS + OMX_VLPB_NUM_OUTPUT_PORTS ) ), 
               OMX_ErrorBadParameter );

     /* Free all the input buffer */
      if (((nParam <=
               OMX_VLPB_INPUT_PORT_START_INDEX + OMX_VLPB_NUM_INPUT_PORTS)))
     {
      if ( ( pVLPBComPvt->pPortdefs[nParam]->bEnabled ) ||
            (OMX_TRUE == pBaseComInt->ports[nParam]->bIsTransitionToDisabled))
        {
            nInMsgCount = 0;
            pVLPBComPvt->fpDioGetCount(hComponent, nParam,
                          (OMX_PTR) & nInMsgCount);
            while (nInMsgCount > 0) 
            {
              pVLPBComPvt->fpDioDequeue(hComponent, nParam,
                            (OMX_PTR) (pInBufHeader));
            pVLPBComPvt->fpDioSend (hComponent, nParam, pInBufHeader);
              nInMsgCount--;
            }
       }
     }
     }    
     break;

    case OMX_CommandPortEnable:

     /* Make sure nParam corresponds to a valid port */
    OMX_BASE_ASSERT (((nParam < OMX_VLPB_INPUT_PORT_START_INDEX +
               OMX_VLPB_NUM_INPUT_PORTS + OMX_VLPB_NUM_OUTPUT_PORTS ) ), 
               OMX_ErrorBadParameter );
     
     _OMX_VLPBEnablePeriodicDataProcess (hComponent);

      break;
    default:
      break;
  }

  /* Note: Notify this completion to the Base comp via ReturnEventNotify call */
  eError =
      pVLPBComPvt->fpReturnEventNotify ( hComponent, OMX_EventCmdComplete, Cmd,
                                         nParam, NULL );
  OMX_BASE_ASSERT ( eError == OMX_ErrorNone, eError );

EXIT:

  return eError;
}


/* ============================================================================
 */

/**
* @fn OMX_TI_VLPB_DataNotify():
*     This method is invoked in the Base comp context to notify data either
*     input or output buffers to be processed.
*
* @param [in] hComponent  :  Handle of the component to be accessed.
* @param [in] nPortIndex    :  Index of the port
* @param [in] pBufHeader   :  Pointer to an Bufferheader which need to be processed
*
* Note: The Advanced function demonstrates usage of advanced features such as
*       extra data, dup etc.
*/
/* ============================================================================
 */
OMX_ERRORTYPE
OMX_TI_VLPB_DataNotify (OMX_HANDLETYPE hComponent)
{
 OMX_COMPONENTTYPE *pComp = NULL;

 OMX_VLPB_PVTTYPE *pVLPBComPvt = NULL;

 OMX_ERRORTYPE eError = OMX_ErrorNone;

 OMX_U32 i = 0;
 OMX_U32 nInMsgCount = 0;
 OMX_U32 nOutMsgCount = 0;

 OMX_BASE_VIDEO_CUSTOMHEADER *pInBufHeaderPtr;
 OMX_BASE_VIDEO_CUSTOMHEADER *pOutBufHeaderPtr;

 omxVLPBInPort2WinMgr_t *Inport_entry;

 omxVLPBOutPort2WinMgr_t *Outport_entry;

 TIMM_OSAL_ERRORTYPE osalError;
 
 OMX_BASE_INTERNALTYPE* pBaseComInt = NULL;

 OMX_BASE_PRIVATETYPE* pBaseComPvt = NULL;

 OMX_BASE_PORTTYPE* pPort = NULL;

 
#ifdef VLPB_DEBUG
  printf ("Entered: %s (0x%x)", __FUNCTION__, hComponent);
#endif
  /*--------------------------------------------------------------------------*/
  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /* Verify the input pointers                                                */
  /*                                                                          */
  /*--------------------------------------------------------------------------*/
  OMX_BASE_REQUIRE ( hComponent != NULL, OMX_ErrorBadParameter );
  pComp = ( OMX_COMPONENTTYPE * ) hComponent;
  pVLPBComPvt = ( OMX_VLPB_PVTTYPE * ) pComp->pComponentPrivate;
  OMX_BASE_REQUIRE ( hComponent != NULL, OMX_ErrorBadParameter );

  pBaseComPvt = (OMX_BASE_PRIVATETYPE *)pComp->pComponentPrivate;

  pBaseComInt = (OMX_BASE_INTERNALTYPE *)pBaseComPvt->hOMXBaseInt;

  /*--------------------------------------------------------------------------*/
  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /* Verify the version                                                       */
  /*--------------------------------------------------------------------------*/
  OMX_BASE_CHK_VERSION ( pComp, OMX_COMPONENTTYPE, eError );

  TIMM_OSAL_MutexObtain ( pVLPBComPvt->pDataNotifyMutex, TIMM_OSAL_SUSPEND );

  /*--------------------------------------------------------------------------*/
  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
  /* Process the data if the state is Executiong                              */
  /* Steps are as follows:                                                    */
  /*                                                                          */
  /*    A. For all enabled ports,                                             */
  /*        a. Get no. of free buffers                                        */
  /*        b. For each of the free Buffers                                   */
  /*               i.  Deque a buffer                                         */
  /*               ii. Release the buffer                                     */
  /*                                                                          */
  /*--------------------------------------------------------------------------*/
  if (pVLPBComPvt->tCurState == OMX_StateExecuting)
  {
    if (NULL == pVLPBComPvt->hDioGroupHandle)
    {
     for ( i = 0; i < OMX_VLPB_NUM_INPUT_PORTS; i++ ) 
     {
       Inport_entry = &pVLPBComPvt->omxVLPBInPort2WinMgr[i];
       pPort = pBaseComInt->ports[OMX_VLPB_INPUT_PORT_START_INDEX+i];
        if ((pVLPBComPvt->pPortdefs[(OMX_VLPB_INPUT_PORT_START_INDEX + i)]->
             bEnabled) && (OMX_FALSE == pPort->bIsTransitionToEnabled) ) 
        {
          eError =
            pVLPBComPvt->fpDioGetCount (hComponent,
                                        (OMX_VLPB_INPUT_PORT_START_INDEX + i),
                                        (OMX_PTR) & nInMsgCount);
          OMX_BASE_ASSERT ((OMX_ErrorNone == eError), eError);
          if (nInMsgCount == 0)
          {
            continue;
          }
          }
       else 
          {
            continue;
          }
       pPort = pBaseComInt->ports[OMX_VLPB_OUTPUT_PORT_START_INDEX+i];
       Outport_entry = &pVLPBComPvt->omxVLPBOutPort2WinMgr[i];

       if ( pVLPBComPvt->pPortdefs[(OMX_VLPB_OUTPUT_PORT_START_INDEX + i)]->
                bEnabled && (OMX_FALSE == pPort->bIsTransitionToEnabled) )
       {
         eError =  pVLPBComPvt->fpDioGetCount (hComponent,
                                        (OMX_VLPB_OUTPUT_PORT_START_INDEX + i),
                                        (OMX_PTR) & nOutMsgCount);       
         OMX_BASE_ASSERT ((OMX_ErrorNone == eError), eError);
          if (nOutMsgCount == 0)
          {
           continue;
         }         
       }
       else 
       {
          continue;
       }
       /* Queue data bufs available at this port to the driver */
       if ( nInMsgCount > 1 ) {
        OMX_BASE_SetDataProcessingPending ( hComponent );
        nInMsgCount = 1;
       }
         /* On input port, the component is expecting all 3 meta data fields
             to be available so all 3 bits are set. If any of these fields is
             unavailable then the bit field will be updated when the buffer is
             received */

          pInBufHeaderPtr =
            &(Inport_entry->sInBufHeader[Inport_entry->nInBufHdrFreeIdx]);
          pOutBufHeaderPtr =
            &(Outport_entry->sOutBufHeader[Outport_entry->nOutBufHdrFreeIdx]);

          pInBufHeaderPtr->nMetaDataUsageBitField =
            OMX_BASE_VIDEO_EXTRADATA_ALL_BIT;
          pOutBufHeaderPtr->nMetaDataUsageBitField =
            OMX_BASE_VIDEO_EXTRADATA_ALL_BIT;

          eError =
            pVLPBComPvt->fpDioDequeue (hComponent,
                                       (OMX_VLPB_INPUT_PORT_START_INDEX + i),
                           ( OMX_PTR ) ( pInBufHeaderPtr ) );
          OMX_BASE_ASSERT ((OMX_ErrorNone == eError), eError);
         
          if (eError == OMX_TI_WarningAttributePending)
          {
         /* Call GetAttribute to get the attribute and perform the necesary
               operations. To be used if you expect your component to receive
               in band control information */
         }
          eError =
            pVLPBComPvt->fpDioDequeue (hComponent,
                                       (OMX_VLPB_OUTPUT_PORT_START_INDEX + i),
                           ( OMX_PTR ) ( pOutBufHeaderPtr ) );
          OMX_BASE_ASSERT ((OMX_ErrorNone == eError), eError);

         Inport_entry->nInBufHdrFreeIdx++;
         Outport_entry->nOutBufHdrFreeIdx++;
            if (Inport_entry->nInBufHdrFreeIdx >=
                OMX_VLPB_DEFAULT_INPUT_BUFFER_COUNT)
            {
           Inport_entry->nInBufHdrFreeIdx = 0;
         }
            if (Outport_entry->nOutBufHdrFreeIdx >=
                OMX_VLPB_DEFAULT_OUTPUT_BUFFER_COUNT)
            {
           Outport_entry->nOutBufHdrFreeIdx = 0;
         }
#ifdef VLPB_RECORD_BITSREAM
         if(frameNumber[i] < VLPB_NUM_FRAMES_TO_RECORD)
         {
          FILE *f_vlpb_out;

          if(frameNumber[i] == 0)
           printf("Opening bitstream file\n");
          
          if(frameNumber[i] == 0) /* first frame */
          {
           f_vlpb_out = fopen("bitstream.264","wb");
          }
          else
          {
           f_vlpb_out = fopen("bitstream.264","ab");
          }
          
          /* Record bitstream */
          printf("Writing frame %ld ... ", frameNumber[i]+1);
              fwrite (pInBufHeaderPtr->pBuffer, 1, pInBufHeaderPtr->nFilledLen,
                      f_vlpb_out);
          printf("done!\n");
          
          if(frameNumber[i] == (VLPB_NUM_FRAMES_TO_RECORD-1))
           printf("Closing bitstream file\n");
          
          fclose(f_vlpb_out);
         }
#endif
            osalError =
              TIMM_OSAL_Memcpy ((TIMM_OSAL_PTR) (pOutBufHeaderPtr->pBuffer),
                                (TIMM_OSAL_PTR) (pInBufHeaderPtr->pBuffer),
                                (TIMM_OSAL_U32) pInBufHeaderPtr->nFilledLen);
         if( osalError !=  TIMM_OSAL_ERR_NONE)
         {
           printf("Error while Copying frame \n");
           eError = OMX_ErrorUndefined;
           goto EXIT;
         }
#ifdef _LOCAL_CORE_c6xdsp_
         MEMUTILS_cacheInv(pInBufHeaderPtr->pBuffer, 
                           pInBufHeaderPtr->nFilledLen);
         MEMUTILS_cacheWbInv(pOutBufHeaderPtr->pBuffer, 
                             pInBufHeaderPtr->nFilledLen);
#endif
         pOutBufHeaderPtr->nFilledLen = pInBufHeaderPtr->nFilledLen;
         
         frameNumber[i]++;
         if((frameNumber[i]%1800) == 0)
         {
           printf("frameNumber[%d]: %ld\n", (int) i, frameNumber[i]);
         }
         
         /* Buffer Done Logic */
            pInBufHeaderPtr->nMetaDataUsageBitField =
              OMX_BASE_VIDEO_EXTRADATA_ALL_BIT;
            /* Buffer Done Logic */
            pOutBufHeaderPtr->nMetaDataUsageBitField =
              OMX_BASE_VIDEO_EXTRADATA_ALL_BIT;

         /* Since the input buffer is completely consumed , return input
               buffer */
         eError = pVLPBComPvt->fpDioSend ( hComponent,
                            pInBufHeaderPtr->nInputPortIndex,
                            pInBufHeaderPtr );
        OMX_BASE_ASSERT ((OMX_ErrorNone == eError), eError);

         /* Since the Output buffer is completely filled , return Output
               buffer */
         eError = pVLPBComPvt->fpDioSend ( hComponent,
                            pOutBufHeaderPtr->nOutputPortIndex,
                            pOutBufHeaderPtr );                             
#ifdef VLPB_DEBUG
         printf("Sending frame ptr : 0x%x Status: %d\n",
               pInBufHeaderPtr, osalError);
         printf("Sending frame ptr : 0x%x Status: %d\n",
               pOutBufHeaderPtr, osalError);               
#endif

        OMX_BASE_ASSERT ((OMX_ErrorNone == eError), eError);

      }/* For OMX_VLPB_NUM_INPUT_PORTS */
   }
    else
    {
      OMX_U32 auPortIndex[OMX_VLPB_NUM_INPUT_PORTS];

      OMX_PTR pInBufHeaderPtrList[OMX_VLPB_NUM_INPUT_PORTS];

      OMX_U32 auQueuedBufCnt[OMX_VLPB_NUM_INPUT_PORTS];

      OMX_BASE_VIDEO_CUSTOMHEADER sInBufHeaderList[OMX_VLPB_NUM_INPUT_PORTS];

      OMX_U32 numPortsToGetCount;

      OMX_U32 numPortsToAlloc;

      Bool bAdditionalBufExists;

      // omxVLPBPort2WinMgr_t *port_entry = NULL;

      OMX_PTR pOMxBufHdrList[OMX_VLPB_NUM_INPUT_PORTS];

      do
      {
        /*
         * Check if any command is pending before processing a buffer.
         * This ensures that commands are processed without any latency.
         * This check should be made before every process so that if multiple
         * buffers are available at a port then all those buffers are not
         * processed before the command.
         */
        numPortsToGetCount = 0;
        bAdditionalBufExists = FALSE;
        for (i = 0; i < OMX_VLPB_NUM_INPUT_PORTS; i++)
        {
          auQueuedBufCnt[i] = 0;
          /* If the port is enabled, allocate a buffer */
          if (OMX_TRUE == pVLPBComPvt->pPortdefs[i]->bEnabled)
          {
            auPortIndex[numPortsToGetCount] =
                ( OMX_VLPB_INPUT_PORT_START_INDEX + i );
            numPortsToGetCount++;
          }
        }
        if (numPortsToGetCount > 0)
        {
          eError = pVLPBComPvt->fpDioGroupGetCount ( hComponent,
                                                    pVLPBComPvt->
                                                    hDioGroupHandle,
                                                    auPortIndex, auQueuedBufCnt,
                                                     numPortsToGetCount );
          OMX_BASE_ASSERT ( ( OMX_ErrorNone == eError ), eError );
          numPortsToAlloc = 0;
          for (i = 0; i < numPortsToGetCount; i++)
          {
            if (auQueuedBufCnt[i] != 0)
            {
              auPortIndex[numPortsToAlloc] = auPortIndex[i];
              if (auQueuedBufCnt[i] > 1)
              {
                bAdditionalBufExists = TRUE;
              }
              pInBufHeaderPtrList[numPortsToAlloc] =
                  &( sInBufHeaderList[numPortsToAlloc] );
              sInBufHeaderList[numPortsToAlloc].nMetaDataUsageBitField =
                  OMX_BASE_VIDEO_EXTRADATA_ALL_BIT;
              numPortsToAlloc++;
            }
          }
          if (numPortsToAlloc > 0)
          {
            eError = pVLPBComPvt->fpDioGroupDequeue ( hComponent,
                                                     pVLPBComPvt->
                                                     hDioGroupHandle,
                                                      auPortIndex,
                                                      pInBufHeaderPtrList,
                                                      numPortsToAlloc );
            OMX_BASE_ASSERT ( ( OMX_ErrorNone == eError ), eError );
            for (i = 0; i < numPortsToAlloc; i++)
            {

              // port_entry = &pVLPBComPvt->omxVLPBPort2WinMgr[auPortIndex[i]];
              pInBufHeaderPtr = pInBufHeaderPtrList[i];
              /* Handling a Mark Buffer */
              if (pVLPBComPvt->pMark != NULL)
              {
                pInBufHeaderPtr->hMarkTargetComponent =
                    pVLPBComPvt->pMark->hMarkTargetComponent;
                pInBufHeaderPtr->pMarkData = pVLPBComPvt->pMark->pMarkData;
                pVLPBComPvt->pMark = NULL;
              }

              /* send an Mark Event incase if the target comp is same as this
                 comp */
              if (pInBufHeaderPtr->hMarkTargetComponent == pComp)
              {
                osalError = pVLPBComPvt->fpReturnEventNotify ( hComponent,
                                                               OMX_EventMark, 0,
                                                               0,
                                                              pInBufHeaderPtr->
                                                              pMarkData);
                OMX_BASE_ASSERT ( ( osalError == TIMM_OSAL_ERR_NONE ),
                                  OMX_ErrorInsufficientResources );
                pInBufHeaderPtr->pMarkData = NULL;
                pInBufHeaderPtr->hMarkTargetComponent = NULL;
              }

              /* Propogating the mark from an input buffer to the output buffer */
              if (pInBufHeaderPtr->hMarkTargetComponent != NULL)
              {
                osalError = pVLPBComPvt->fpReturnEventNotify ( hComponent,
                                                               OMX_EventError,
                                                               ( OMX_U32 )
                                                               OMX_ErrorBadParameter,
                                                               0, NULL );
                OMX_BASE_ASSERT ( ( osalError == TIMM_OSAL_ERR_NONE ),
                                  OMX_ErrorInsufficientResources );
              }

              auPortIndex[i] = pInBufHeaderPtr->nInputPortIndex;
              pOMxBufHdrList[i] = pInBufHeaderPtr;
            }
            eError = pVLPBComPvt->fpDioGroupSend ( hComponent,
                                                   pVLPBComPvt->hDioGroupHandle,
                                                   auPortIndex, pOMxBufHdrList,
                                                   numPortsToAlloc );
            OMX_BASE_ASSERT ( ( OMX_ErrorNone == eError ), eError );
          }                     /* end of if (numPortsToAlloc > 0) - Dequeue >
                                   0 */
        }                       /* end of if (numPortsToGetCount * * * * * * *
                                   > 0) - GetCount > 0 */
      }
      while (TRUE == bAdditionalBufExists);
    }
  }/*if curState = EXECUTING */
  TIMM_OSAL_MutexRelease ( pVLPBComPvt->pDataNotifyMutex );

EXIT:

  return eError;

}

OMX_ERRORTYPE
_OMX_VLPBEnablePeriodicDataProcess (OMX_HANDLETYPE hComponent)
{

  OMX_ERRORTYPE eError = OMX_ErrorUndefined;

  OMX_COMPONENTTYPE *pHandle = NULL;

  OMX_VLPB_PVTTYPE *pVLPBComPvt = NULL;

  OMX_U32 i;

  OMX_BOOL bPrdTaskEnable = OMX_FALSE;

#ifdef VLPB_DEBUG
  printf("Entering: %s \n", __FUNCTION__ );
#endif
  /* Check the input parameters, this should be TRUE else report an Error */
  OMX_BASE_REQUIRE ( hComponent != NULL, OMX_ErrorBadParameter );

  pHandle = ( OMX_COMPONENTTYPE * ) hComponent;

  OMX_BASE_CHK_VERSION ( pHandle, OMX_COMPONENTTYPE, eError );

  pVLPBComPvt = ( OMX_VLPB_PVTTYPE * ) pHandle->pComponentPrivate;

  for (i = 0; i < OMX_VLPB_NUM_INPUT_PORTS; i++)
  {
   if((OMX_NOTIFY_TYPE_NONE == pVLPBComPvt->tDerToBase.
       pPortProperties[OMX_VLPB_INPUT_PORT_START_INDEX + i]->eNotifyType) &&
      (OMX_TRUE == pVLPBComPvt->pPortdefs[( i +
                                             OMX_VLPB_INPUT_PORT_START_INDEX)]->
         bEnabled))
    {
      bPrdTaskEnable = OMX_TRUE;
    }
  }
  for (i = 0; i < OMX_VLPB_NUM_OUTPUT_PORTS; i++)
  {
   if((OMX_NOTIFY_TYPE_NONE == pVLPBComPvt->tDerToBase.
       pPortProperties[OMX_VLPB_OUTPUT_PORT_START_INDEX + i]->eNotifyType) &&
      (OMX_TRUE == pVLPBComPvt->pPortdefs[( i +
                                             OMX_VLPB_OUTPUT_PORT_START_INDEX)]->
         bEnabled))
    {
      bPrdTaskEnable = OMX_TRUE;
    }
  }
  if (OMX_TRUE == bPrdTaskEnable && !g_pVLPBDataProcessPrdTskStartSemPosted)
  {
    g_pVLPBDataProcessPrdTskStartSemPosted = OMX_TRUE;
    TIMM_OSAL_SemaphoreRelease (pVLPBComPvt->pVlpbDataProcessPrdTskStartSem);
  }
  
  pVLPBComPvt->bPrdTaskEnable = bPrdTaskEnable;

#ifdef VLPB_DEBUG
  printf("Leaving:%s bPrdTaskEnable:%d\n", __FUNCTION__, bPrdTaskEnable);
#endif

  eError = OMX_ErrorNone;

EXIT:
  return eError;
}

/** 
********************************************************************************
 *  @func     OMX_VLPBFlushChannelBuf
 *  @brief  This function
 *          1. Check for any buffers allocated to reciever
               port but not consumed 
 *          2. Return the same the to the supplier port
 *          
 *
 *  @param[in ]  hComponent  : Handle to the component data structure
 * 
 *  @returns OMX_ErrorNone = Successful 
 *           OMX_ErrorBadParameter
 *           OMX_ErrorVersionMismatch
 *           OMX_ErrorInsufficientResources
 *           OMX_ErrorUndefined
*******************************************************************************/
OMX_ERRORTYPE
OMX_VLPBFlushChannelBuf (OMX_HANDLETYPE hComponent)
{
   
  OMX_COMPONENTTYPE *pHandle = NULL;
  
  OMX_VLPB_PVTTYPE *pVLPBComPvt = NULL;
  
  OMX_BASE_VIDEO_CUSTOMHEADER sInBufHeader = {NULL};
  
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  
  OMX_U32 i, nInMsgCount = 0u;
  
  pHandle = ( OMX_COMPONENTTYPE * ) hComponent;
  
  OMX_BASE_PORTTYPE* pPort = NULL;
  
  OMX_BASE_INTERNALTYPE* pBaseComInt = NULL;
  
  OMX_BASE_PRIVATETYPE* pBaseComPvt = NULL;
  
  pVLPBComPvt = ( OMX_VLPB_PVTTYPE * ) pHandle->pComponentPrivate;
  
  OMX_BASE_REQUIRE ( NULL != pVLPBComPvt, OMX_ErrorBadParameter );
  
  OMX_BASE_INIT_STRUCT_PTR ( &sInBufHeader, OMX_BASE_VIDEO_CUSTOMHEADER );
  
  pBaseComPvt = (OMX_BASE_PRIVATETYPE *)pHandle->pComponentPrivate;
  pBaseComInt = (OMX_BASE_INTERNALTYPE *)pBaseComPvt->hOMXBaseInt;
  
  /* If active DIO groups are not enabled */
  if (NULL == pVLPBComPvt->hDioGroupHandle)
  {
    for (i = 0u; i < OMX_VLPB_NUM_INPUT_PORTS; i++)
    {
      pPort = pBaseComInt->ports[i];
      nInMsgCount = 0;
      if ( ( pVLPBComPvt->
          pPortdefs[( OMX_VLPB_INPUT_PORT_START_INDEX + i )]->bEnabled ) ||
          (OMX_TRUE == pPort->bIsTransitionToDisabled))
      {
       /* Get the number of buffers queued up at the port */ 
        eError = pVLPBComPvt->fpDioGetCount ( hComponent,
                                              ( OMX_VLPB_INPUT_PORT_START_INDEX 
                                              + i ), ( OMX_PTR ) &nInMsgCount );
        OMX_BASE_ASSERT ( OMX_ErrorNone == eError, eError );
      }
     
   /*-------------------------------------------------------------------*
    * Dequeue and Send buffers back to supplier till number of buffers  
    * queued reduces to 0.                                              
    *-------------------------------------------------------------------*/

      while (nInMsgCount > 0)
      {

       /*-------------------------------------------------------------------*
        * Dequeue & Get the input buffers
        *-------------------------------------------------------------------*/
       eError = pVLPBComPvt->fpDioDequeue ( hComponent,
                                            OMX_VLPB_INPUT_PORT_START_INDEX + i,
                                            ( OMX_PTR ) ( &sInBufHeader ) );
       OMX_BASE_ASSERT ( OMX_ErrorNone == eError, eError );
       
       nInMsgCount--;
        
    /* Send the dequeued buffers back to supplier */ 
       eError = pVLPBComPvt->fpDioSend ( hComponent,
                                         sInBufHeader.nInputPortIndex,
                                         &sInBufHeader ); 
       OMX_BASE_ASSERT ( OMX_ErrorNone == eError, eError );

     /* end of while ( nInMsgCount > 0 ) */
     }
     /* end of for (i = 0; i < OMX_VLPB_NUM_INPUT_PORTS; i++) */
   } 
  
  }
  else
  {
    /* No support for flushing channels in case of grouped ports as of now Will 
       be implemented later. */
  }

  EXIT:
  return eError; 
  
}

/* ========================================================================== */

/**
* @fn _OMX_TI_VLPB_ParseBufferHeader():
*     Internal function, parses the first buffer header to get the
*     new configurations(Frame width and Frame height).
*
* @param [in] pVLPBComPvt :  poiter to component private data
* @param [in] pBufHeader    :  Pointer to an Bufferheader, to be processed
*/
/* ========================================================================== */
#ifdef DYN_PORT_RECONFIG_TEST
OMX_ERRORTYPE
_OMX_TI_VLPB_ParseBufferHeader (OMX_HANDLETYPE hComponent,
                                               OMX_BUFFERHEADERTYPE * pBufHdr )
{
  OMX_ERRORTYPE eError = OMX_ErrorNone;

  OMX_VLPB_PVTTYPE *pVLPBComPvt;

  OMX_COMPONENTTYPE *pComp;

  OMX_BASE_Entering (  );

  pComp = ( OMX_COMPONENTTYPE * ) hComponent;
  pVLPBComPvt = ( OMX_VLPB_PVTTYPE * ) pComp->pComponentPrivate;

  /* Incase of video and Image components, parse the buffer header and get the
     correct framewidth and frameheight values */
  /* Incase of Audio component, parse the buffer header and get the correct
     output frame size that needs to be required */

  /* Not Implemented full functionality, since this is a VLPB component and
     objective is to validate the DynamicPortReCofniguration functionality. in
     order do that just modifying the output buffer size without actual buffer
     parsing and Notifying the IL client by generating
     OMX_EventPortSettingsChange event */

  pVLPBComPvt->portdefs[OMX_VLPB_INPUT_PORT]->nBufferSize +=
      pVLPBComPvt->portdefs[OMX_VLPB_INPUT_PORT]->nBufferSize;

  pVLPBComPvt->fpReturnEventNotify ( hComponent, OMX_EventPortSettingsChanged,
                                     OMX_VLPB_INPUT_PORT, 0, NULL );

  eError = OMX_ErrorBadParameter;

EXIT:
  return eError;
}
#endif

Void
_OMX_VLPBDataProcPrdTaskFcn (TIMM_OSAL_U32 argc, TIMM_OSAL_PTR argv)
{
  OMX_HANDLETYPE hComponent = NULL;

  OMX_VLPB_PVTTYPE *pVLPBComPvt = NULL;

  OMX_COMPONENTTYPE *pHandle = NULL;

  OMX_ERRORTYPE eError = OMX_ErrorNone;

  TIMM_OSAL_ERRORTYPE osalError;

  OMX_BOOL process_loop = OMX_TRUE;

  Error_Block ebObj;

  Error_Block *eb = &ebObj;

  hComponent = ( OMX_HANDLETYPE ) argv;
  Error_init ( eb );

#ifdef VLPB_DEBUG
  printf("Entering: %s \n", __FUNCTION__);
#endif
  /* Check the input parameters */
  OMX_BASE_REQUIRE ( ( hComponent != NULL ), OMX_ErrorBadParameter );
  pHandle = ( OMX_COMPONENTTYPE * ) hComponent;
  pVLPBComPvt = ( OMX_VLPB_PVTTYPE * ) pHandle->pComponentPrivate;

  /* Wait till signal to start task processing */
  osalError =
      TIMM_OSAL_SemaphoreObtain ( pVLPBComPvt->pVlpbDataProcessPrdTskStartSem,
                                  TIMM_OSAL_SUSPEND );
  OMX_BASE_ASSERT ( ( TIMM_OSAL_ERR_NONE == osalError ), OMX_ErrorUndefined );

  while (process_loop)
  {
    if (OMX_TRUE == pVLPBComPvt->bTaskExit)
    {
   break;
 }
    osalError = TIMM_OSAL_SleepTask ( OMX_VLPB_PROCESS_CHANNEL_TASK_PERIOD_MS );
    OMX_BASE_ASSERT ( ( TIMM_OSAL_ERR_NONE == osalError ), OMX_ErrorUndefined );
    if (pVLPBComPvt->tCurState == OMX_StateExecuting)
    {
      eError = OMX_TI_VLPB_DataNotify ( hComponent );
      OMX_BASE_ASSERT ( ( OMX_ErrorNone == eError ), eError );
    }
  }
EXIT:
  if (OMX_ErrorNone != eError)
  {
    Error_raise ( eb, Error_E_generic, "Error in processing loop:%d", eError );
  }
  TIMM_OSAL_ExitTask(pVLPBComPvt->pVlpbDataProcessPrdTsk,NULL);
}

/* Nothing beyond this point */
