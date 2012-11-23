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

/**
*   @file  omx_Vlpb_priv.h
 *   This file contains API/Defines that provides the functionality for the
 *   video frame capture component.
 *   - A user would typically use functions and data structures defined in this
 *   module to access different VLPB functionalites.
 *
 *   @path \ti\omx\comp\Vlpb\src
*
*   @rev  1.0
 *   @Release version 04.00.00.02
 *   @{
 */

/** ===========================================================================
 *! Revision History
 *! ===========================================================================
 *!17-October-2009 : Initial Version
 * ============================================================================
 */

#ifndef _OMX_VLPB_PRIV_H
#define _OMX_VLPB_PRIV_H

#ifdef _cplusplus
extern "C"
{
#endif                          /* _cplusplus */

  /* -------------------compilation control switches ----------------------- */

  /* ----------------------------------------------------------------------- */

/*****************************************************************************
 * -------------------INCLUDE FILES-------------------------------------------
 *****************************************************************************/
  /* ------------------ system and platform files -------------------------- */

/*--------------------program files -----------------------------------------*/
#include "ti/omx/comp/omxbase/omx_base.h"
#include "timm_osal_interfaces.h"
#include "OMX_TI_Video.h"

/*****************************************************************************
 * PUBLIC DECLARATIONS Defined here, used elsewhere
 *****************************************************************************/

 /** @ingroup    HDVPSSVLPB */
  /* @{ */

 /** defines the major version of the VLPB Component */
#define OMX_VLPB_COMP_VERSION_MAJOR (1)

 /**  defines the minor version of the VLPB component  */
#define OMX_VLPB_COMP_VERSION_MINOR (1)

 /** defiens the revision of the VLPB component */
#define OMX_VLPB_COMP_VERSION_REVISION (0)

 /** defines the step version of the VLPB component */
#define OMX_VLPB_COMP_VERSION_STEP (0)

/* ========================================================================== */
/* Macros & Typedefs */
/* ========================================================================== */


 /** Maximum Number of ports for the VLPB Comp */
#define  OMX_VLPB_NUM_PORTS     (OMX_VLPB_NUM_INPUT_PORTS +                 \
                                                     OMX_VLPB_NUM_OUTPUT_PORTS)

 /** Default context Type as HOST CONTEXT(0) */
#define DEFAULT_CONTEXT_TYPE (OMX_BASE_CONTEXT_PASSIVE)

 /** Stack Size of the OMX VLPB Comp*/
#define OMX_VLPB_STACKSIZE (1024 * 50)

 /** Priority of the OMX VLPB Comp  */
#define OMX_VLPB_GROUPPRIORITY (0)

 /** Priority of the OMX VLPB Comp  */
#  define OMX_VLPB_DEFAULT_TASKPRIORITY (10)

 /** Group Id of the VLPB Comp */
#define OMX_VLPB_GROUPID (0)

#define VLPB_FRAME_QUE_LEN_PER_CH     (2)

 /** default input buffer count */
#define OMX_VLPB_DEFAULT_INPUT_BUFFER_COUNT (VLPB_FRAME_QUE_LEN_PER_CH*2)

 /** Minimum input buffer count */
#define OMX_VLPB_MIN_INPUT_BUFFER_COUNT (VLPB_FRAME_QUE_LEN_PER_CH*2)

 /** default output buffer count */
#define OMX_VLPB_DEFAULT_OUTPUT_BUFFER_COUNT (VLPB_FRAME_QUE_LEN_PER_CH*2)

 /** Minimum output buffer count */
#define OMX_VLPB_MIN_OUTPUT_BUFFER_COUNT (VLPB_FRAME_QUE_LEN_PER_CH*2)

 /** Defualt Frame Width*/
#define OMX_VLPB_DEFAULT_INPUT_FRAME_WIDTH (720)

 /** Defualt Frame Height*/
#define OMX_VLPB_DEFAULT_INPUT_FRAME_HEIGHT (480)

 /** Defualt Frame Width*/
#define OMX_VLPB_DEFAULT_OUTPUT_FRAME_WIDTH (720)

 /** Defualt Frame Height*/
#define OMX_VLPB_DEFAULT_OUTPUT_FRAME_HEIGHT (480)

/** Period of channel process thread */
#define OMX_VLPB_PROCESS_CHANNEL_TASK_PERIOD_MS  (32/2)

  /** Max Frame width*/
#define OMX_VLPB_MAX_INPUT_FRAME_WIDTH  (720)

  /** Max Frame Height*/
#define OMX_VLPB_MAX_INPUT_FRAME_HEIGHT (576)

  /** Max Frame width*/
#define OMX_VLPB_MAX_OUTPUT_FRAME_WIDTH  (720)

  /** Max Frame Height*/
#define OMX_VLPB_MAX_OUTPUT_FRAME_HEIGHT (576)

 /** Size of pixel in bytes */

#define OMX_VLPB_BYTES_PER_PIXEL        (2)

  /** deafult input buffer size */
#define OMX_VLPB_DEFAULT_INPUT_BUFFER_SIZE ((OMX_VLPB_DEFAULT_INPUT_FRAME_WIDTH *  \
                                               OMX_VLPB_DEFAULT_INPUT_FRAME_HEIGHT * \
                                               OMX_VLPB_BYTES_PER_PIXEL)/4)

  /** deafult input buffer size */
#define OMX_VLPB_DEFAULT_OUTPUT_BUFFER_SIZE ((OMX_VLPB_DEFAULT_OUTPUT_FRAME_WIDTH *  \
                                               OMX_VLPB_DEFAULT_OUTPUT_FRAME_HEIGHT * \
                                               OMX_VLPB_BYTES_PER_PIXEL)/4)


/** deafult Input data units */
#define OMX_VLPB_DEFAULT_INPUT_DATAUNITS (1)

/** deafult Output data units */
#define OMX_VLPB_DEFAULT_OUTPUT_DATAUNITS (1)

/** default frame rate */
#define OMX_VLPB_DEFAULT_FRAME_RATE       (30)

/** Maximum number of frame buffers */
#define OMX_VLPB_MAX_FRAME_BUF_COUNT      (VLPB_FRAME_QUE_LEN_PER_CH * \
                                             OMX_VLPB_NUM_INPUT_PORTS)

/** Maximum number of port group supported */
#define OMX_VLPB_MAX_NUM_PORT_GROUPS      (2)

/** Define used to indicate invalid port group index */
#define OMX_VLPB_PORT_GROUP_INDEX_INVALID ((OMX_U32)~(0u))

/*--------------------------Data declarations --------------------------------*/

/*******************************************************************************
* Enumerated Types
*******************************************************************************/

/* ========================================================================== */
/* Macros & Typedefs */
/* ========================================================================== */

/*******************************************************************************
* Strutures
*******************************************************************************/

  typedef struct omxVLPBInPort2WinMgr_t
  {
    OMX_U32 nPortIndex;
    OMX_BASE_VIDEO_CUSTOMHEADER
      sInBufHeader[OMX_VLPB_DEFAULT_INPUT_BUFFER_COUNT];
    OMX_U32 nInBufHdrFreeIdx;
  }
  omxVLPBInPort2WinMgr_t;

  typedef struct omxVLPBOutPort2WinMgr_t
  {
    OMX_U32 nPortIndex;
    OMX_BASE_VIDEO_CUSTOMHEADER
      sOutBufHeader[OMX_VLPB_DEFAULT_OUTPUT_BUFFER_COUNT];
    OMX_U32 nOutBufHdrFreeIdx;
  }
  omxVLPBOutPort2WinMgr_t;

/* ========================================================================== */

/** OMX_VLPB_COMP_PVTTYPE - Structure for the private elements of the component
 *      This structur includes all the private elements of the Base Component
 *
 *   Define all derived Component Specifc fields over here
 */
/* ========================================================================== */

    DERIVEDSTRUCT (OMX_VLPB_PVTTYPE, OMX_BASE_PRIVATETYPE)
#define OMX_VLPB_PVTTYPE_FIELDS OMX_BASE_PRIVATETYPE_FIELDS                   \
  OMX_BOOL              bBuferFlush;                                          \
  OMX_BOOL              bTaskExit;                                            \
  OMX_BOOL              bPrdTaskEnable;                                       \
  OMX_PTR               pDataNotifyMutex;                                     \
  TIMM_OSAL_PTR         pVlpbDataProcessPrdTsk;                               \
  Char                  cVlpbDataProcessPrdTskName[OMX_MAX_STRINGNAME_SIZE];  \
  TIMM_OSAL_PTR         pVlpbDataProcessPrdTskStartSem;                       \
  omxVLPBInPort2WinMgr_t   omxVLPBInPort2WinMgr[OMX_VLPB_NUM_INPUT_PORTS];    \
  omxVLPBOutPort2WinMgr_t  omxVLPBOutPort2WinMgr[OMX_VLPB_NUM_OUTPUT_PORTS];  \
  OMX_PTR               hDioGroupHandle;
    ENDSTRUCT (OMX_VLPB_PVTTYPE)
/*----------------------function prototypes ---------------------------------*/
/** Callback from Base to Derived to Notify Data */
  OMX_ERRORTYPE OMX_TI_VLPB_DataNotify (OMX_HANDLETYPE hComponent);

/** Function to set the parameters of the VLPB Component*/
  OMX_ERRORTYPE OMX_TI_VLPB_SetParameter (OMX_HANDLETYPE hComponent,
                                          OMX_INDEXTYPE nParamIndex,
                                          OMX_PTR pParamStruct);

/** Function to get the parameters from the component */
  OMX_ERRORTYPE OMX_TI_VLPB_GetParameter (OMX_HANDLETYPE hComponent,
                                          OMX_INDEXTYPE nParamIndex,
                                          OMX_PTR pParamStruct);

/** Function to set the Configurations of the Component */
  OMX_ERRORTYPE OMX_TI_VLPB_SetConfig (OMX_HANDLETYPE hComponent,
                                       OMX_INDEXTYPE nIndex,
                                       OMX_PTR pConfigData);

/** Function to get the Configurations of the component */
  OMX_ERRORTYPE OMX_TI_VLPB_GetConfig (OMX_HANDLETYPE hComponent,
                                       OMX_INDEXTYPE nIndex,
                                       OMX_PTR pConfigData);

/** Function to deinitalize the VLPB component */
  OMX_ERRORTYPE OMX_TI_VLPB_ComponentDeinit (OMX_HANDLETYPE hComponent);


#ifdef _cplusplus
}
#endif                          /* __cplusplus */

#endif                          /* _OMX_VLPB_PRIV_H */

/* Nothing beyond this point */
