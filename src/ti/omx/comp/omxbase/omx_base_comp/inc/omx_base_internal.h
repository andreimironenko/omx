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
 *  @file  omx_base_internal.h
 *
 *  @path \ti\omx\comp\omxbase\omx_base_comp\inc
 *
 *  @rev 1.0
 */

/*==============================================================
 *! Revision History
 *! ============================
 *! 17-April-2008 Rabbani Patan rabbani@ti.com: Initial Version
 *================================================================*/
#undef OMX_UTL_TRACE_MODULE_NAME
#define OMX_UTL_TRACE_MODULE_NAME ((OMX_BASE_PRIVATETYPE *)((OMX_COMPONENTTYPE *)hComponent)->pComponentPrivate)->tDerToBase.cComponentName

#ifndef _OMX_BASE_INTERNAL_H_
#define _OMX_BASE_INTERNAL_H_

#ifdef __cplusplus
extern "C"
{
#endif                          /* __cplusplus */

/******************************************************************
 *   INCLUDE FILES
 ******************************************************************/
#include <ti/omx/interfaces/openMaxv11/OMX_Core.h>
#include <ti/omx/interfaces/openMaxv11/OMX_Component.h>
#include <omx_base.h>
#include "ti/omx/omxutils/omx_utils.h"

/*******************************************************************
 * EXTERNAL REFERENCE NOTE: only use if not found in header file
 *******************************************************************/
/*----------         function prototypes      ------------------- */
/*----------         data declarations        ------------------- */
/*******************************************************************
 * PUBLIC DECLARATIONS: defined here, used elsewhere
 *******************************************************************/
/*----------         data declarations        ------------------- */
#define OMX_BASE_IsPortEnabled OMX_BASE_BufsFromPortDequable

/*----------         function prototypes      ------------------- */
/*******************************************************************
 * PRIVATE DECLARATIONS: defined here, used only here
 *******************************************************************/
/*----------          data declarations        ------------------- */
/* Extra 1 is added for the dup-main port */
#define OMX_BASE_COMMON_MAX_DUPD_PORTS  (OMX_TI_COMMON_MAX_DUPD_PORTS+1)

/*----------          function prototypes      ------------------- */

#define BUF_ALLOC_EVENT (0x00000001)

#define BUF_FREE_EVENT  (0x00000002)

#define BUF_FAIL_EVENT  (0x00000004)

#define BUF_EVENT_RESERVED1 (0x00000008)

#define OMX_BASE_CMD_EVENT  (0x00000010)

#define OMX_BASE_DATA_EVENT (0x00000020)

#define OMX_BASE_END_EVENT  (0x00000040)

#define OMX_BASE_CMD_COMPLETED_EVENT  (0x00000080)

#define OMX_BASE_CmdStateSet (OMX_BASE_CMD_COMPLETED_EVENT)

#define OMX_BASE_CmdPortEnable (OMX_BASE_CMD_COMPLETED_EVENT)

#define OMX_BASE_CmdPortDisable (OMX_BASE_CMD_COMPLETED_EVENT)

#define OMX_BASE_CmdFlush (OMX_BASE_CMD_COMPLETED_EVENT)

#define OMX_BASE_CmdMarkBuffer  (OMX_BASE_CMD_COMPLETED_EVENT)

#define OMX_BASE_USER1_EVENT  (0x00000100)

#define OMX_BASE_USER2_EVENT  (0x00000200)


#define OMX_BASE_MODNAME  "ti.omx.comp.omxbase"

#define PORT_IS_SUPPLIER(pPort)   \
        (pPort->nTunnelFlags & OMX_BASE_DIO_SUPPLIER )

#define PORT_IS_TUNNELED(pPort) \
        (pPort->nTunnelFlags & OMX_BASE_DIO_TUNNELED)

#define PORT_IS_NONSUPPLIER(pPort) \
        (pPort->nTunnelFlags & OMX_BASE_DIO_NONSUPPLIER)

/*Size of OMX_OTHER_EXTRADATATYPE struct. Has to be updated if the structure 
changes. The actual size is 21 bytes of which 20 bytes consists of the header.
If there is data then that is appended to this 20 bytes. If there is no data
then there is 1 byte empty placeholder which has to be 4 byte aligned to some
padding will be added at the end*/
#define EXTRADATATYPE_SIZE 20

/** The OMX_BASE_TUNNEL_CONFIGTYPE enumeration is used to
 *  sepcify the port type i.e Supplier, NonSupplier..
 */
  typedef enum OMX_BASE_TUNNEL_CONFIGTYPE
  {

    OMX_BASE_DIO_NONTUNNELED = 0x1,
    OMX_BASE_DIO_NONSUPPLIER = 0x2,
    OMX_BASE_DIO_TUNNELED = 0x4,
    OMX_BASE_DIO_SUPPLIER = 0x8
  }
  OMX_BASE_TUNNEL_CONFIGTYPE;


/** OMX_BASE_CMDPARAMS
 *  This structure contains the params required to execute command
 *  @param cmd       :  command to execute
 *  @param nParam    :  parameter for the command to be executed
 *  @param pCmdData  :  pointer to the command data
 */
  typedef struct OMX_BASE_CMDPARAMS
  {

    OMX_COMMANDTYPE eCmd;
    OMX_U32 unParam;
    OMX_PTR pCmdData;

  }
  OMX_BASE_CMDPARAMS;

/** OMX_BASE_DUPEPORT_TYPE
 *  This enumerations contains the values required to set the port type
 */
  typedef enum OMX_BASE_DUPEPORT_TYPE
  {

    OMX_BASE_NORMAL_PORT = 0x0,
    OMX_BASE_DUP_MAIN_PORT = 0x1,
    OMX_BASE_DUP_PORT = 0x2
  }
  OMX_BASE_DUPEPORT_TYPE;

/** OMX_BASE_DUPMAINPORT
 *  This structure contains the parameters of a Dup Main port
 *  @param hDIODupMain:  Handle to the intenal DIO
 *  @param nDupPortsEnabledBitMask     :  Used to identify the ports enabled.
 *  @param nNumDupedPorts  : No.of duped ports
 *  @param anDupedPorts    : Array of Duped Port Indexes
 *  @param ahDIODupedPorts : Array of the hDIOs
 *  @param ahDIODupedPortsWriteIx: Used to identify the place holder for hDIO
 */
  typedef struct OMX_BASE_DUPMAINPORT
  {
    OMX_PTR hDIODupMain;
    OMX_U32 nDupPortsEnabledBitMask;
    OMX_U32 nNumDupedPorts;
    OMX_U32 anDupedPorts[OMX_BASE_COMMON_MAX_DUPD_PORTS];
    OMX_PTR ahDIODupedPorts[OMX_BASE_COMMON_MAX_DUPD_PORTS];
    OMX_U32 ahDIODupedPortsWriteIx;
  }
  OMX_BASE_DUPMAINPORT;

/** OMX_BASE_DUPPORT
 *  This structure contains the parameters of a Dup port
 *  @param nDupPortBitMask : Used to set or reset a particular bit 
 *  @param nDupOfPortIdx   : Used to identify the main Dup Port.
 */
  typedef struct OMX_BASE_DUPPORT
  {
    OMX_U32 nDupPortBitMask;
    OMX_U32 nDupOfPortIdx;
  }
  OMX_BASE_DUPPORT;

/** OMX_BASE_DUPE_INFO
 *  This structure contains the parameters of a Dup Main port
 *  @param eDupPortType:  Port type
 *  @param u : Union of elements of different port types.
 */

  typedef struct OMX_BASE_DUPE_INFO
  {

    OMX_BASE_DUPEPORT_TYPE eDupPortType;
    union
    {
      OMX_BASE_DUPMAINPORT sDupMainPort;
      OMX_BASE_DUPPORT sDupPort;
    }
    u;
  }
  OMX_BASE_DUPE_INFO;

/** OMX_BASE_APP_FQBUFMGR_INFO
 *  This structure contains the parameters of a Dup Main port
 *  @param strFqManagerName: String of the FQ buf manager
 *  @param uPoolId         : Used to identify the pool Id
 */

  typedef struct OMX_BASE_APP_FQBUFMGR_INFO
  {
    OMX_S8 strFqManagerName[OMX_UTL_MAX_STRINGNAME_SIZE];
    OMX_U8 uPoolId;
  }
  OMX_BASE_APP_FQBUFMGR_INFO;

/** OMX_BASE_PORTTYPE :
 *  This structure contains the elements of a port
 *  @param hTunnelComp             :  handle of tunneled comp
 *  @param nTunnelFlags            :  tunnel flags 
 *  @param nTunnelPort             :  tunneled port index
 *  @param bIsTransitionToDisabled :  is in transition to disabled
 *  @param bIsTransitionToEnabled  :  is in transition to enabled
 *  @param bIsFlushingBuffers      :  is in process of flushing
 *  @param sPortDefParams          :  port specific params
 *  @param eSupplierSetting        :  supplier setting of a port
 *  @param pBufferlist             :  list of buffers
 *  @param nBufferCnt              :  Buffer count index
 *  @param pBufAllocSem            :  Event for synchronising both buffer allocation and freeing up
 *  @param pDioOpenCloseSem        :  Semaphore to synchronise DIO open/close 
 *  @param hDIO                    :  handle to access DIO object
 *  @param cChannelName            :  Channel name 
 *  @param bTiledBuffers           :  Flag indicating if port uses tiled memory
 *  @param nMetaDataSize           :  Size of extra meta data in bytes.
 */
  typedef struct OMX_BASE_PORTTYPE
  {

    OMX_HANDLETYPE hTunnelComp;
    OMX_U32 nTunnelFlags;
    OMX_U32 nTunnelPort;
    OMX_BOOL bIsTransitionToDisabled;
    OMX_BOOL bIsTransitionToEnabled;
    OMX_BOOL bIsFlushingBuffers;
    OMX_BOOL bEosRecd;
    OMX_PARAM_PORTDEFINITIONTYPE sPortDef;
    OMX_BUFFERSUPPLIERTYPE eSupplierSetting;
    OMX_BUFFERHEADERTYPE **pBufferlist;
    OMX_U32 nBufferCnt;
    OMX_PTR pBufAllocFreeEvent;
    OMX_PTR pDioOpenCloseSem;
    OMX_PTR hDIO;
    OMX_PTR pMemPluginHandle;
    char cChannelName[128];
    OMX_BOOL bTiledBuffers;
    OMX_U32 nMetaDataSize;
    OMX_BASE_DUPE_INFO sDupeInfo;
    OMX_BASE_APP_FQBUFMGR_INFO sFqMgrInfo;
    OMX_TI_CONFIG_BUFFERREFCOUNTNOTIFYTYPE sBufRefNotify;
  }
  OMX_BASE_PORTTYPE;

/** OMX_BASE_INTERNALTYPE
 *  This structure contains the internals of a Base component
 *  @param ports                   : info related to ports
 *  @param sAppCallbacks           : Application callbacks
 *  @param pCmdPipe                : pointer to command pipe
 *  @param pThreadId               : pointer to thread
 *  @param pTriggerEvent           : pointer to trigger an Event
 *  @param pCmdCompleteEvent : Command comletion Event
 *  @param fpInvokeProcessFunction : hook to process command and data
 */
  typedef struct OMX_BASE_INTERNALTYPE
  {

    OMX_BASE_PORTTYPE **ports;
    OMX_CALLBACKTYPE sAppCallbacks;
    OMX_BOOL bForceNotifyOnce;
    OMX_BOOL bDataProcessingPending;
    OMX_U32 eventMask;
    OMX_U32 eventsFromOS;
    OMX_U32 currentEvents;
    OMX_U32 savedEvents;
    OMX_PTR pCmdPipe;
    OMX_BASE_CMDPARAMS sCurCmdParams;
    OMX_PTR pThreadId;
    OMX_PTR pTriggerEvent;
    OMX_PTR pNewStateMutex;
      OMX_ERRORTYPE (*fpInvokeProcessFunction) (OMX_HANDLETYPE hComponent,
                                                OMX_U32 retEvent);
  }
  OMX_BASE_INTERNALTYPE;


/*===============================================================*/
/** @fn OMX_BASE_BufsFromPortDequable :
 *      This function is called to identify if a port is enabled or
 *  disabled or a port is dequable.
 *
 * @param hComponent  : handle of the component
 * @param retEvent    : Event to be processed
 */
/*===============================================================*/

  static inline OMX_BOOL OMX_BASE_BufsFromPortDequable (OMX_IN OMX_HANDLETYPE
                                                        hComponent,
                                                        OMX_IN OMX_U32
                                                        nPortIndex)
  {
    OMX_COMPONENTTYPE *pComp = NULL;
    OMX_BASE_PRIVATETYPE *pBaseComPvt = NULL;
    OMX_BASE_INTERNALTYPE *pBaseComInt = NULL;
    OMX_BASE_PORTTYPE *pPort = NULL;
    OMX_BOOL bPortEnabled = OMX_FALSE;
    OMX_U32 nStartPortNumber;

      pComp = (OMX_COMPONENTTYPE *) hComponent;
      pBaseComPvt = (OMX_BASE_PRIVATETYPE *) pComp->pComponentPrivate;
      pBaseComInt = (OMX_BASE_INTERNALTYPE *) pBaseComPvt->hOMXBaseInt;
      nStartPortNumber = pBaseComPvt->tDerToBase.nMinStartPortIndex;
      pPort = pBaseComInt->ports[nPortIndex - nStartPortNumber];

    if (OMX_BASE_NORMAL_PORT == pPort->sDupeInfo.eDupPortType)
    {
      if ((OMX_FALSE == pPort->sPortDef.bEnabled) ||
          (OMX_TRUE == pPort->bIsTransitionToEnabled) ||
          (OMX_TRUE == pPort->bIsTransitionToDisabled))
      {
        bPortEnabled = OMX_FALSE;
      }
      else
      {
        bPortEnabled = OMX_TRUE;
      }
    }
    else
    {
      if (OMX_BASE_DUP_MAIN_PORT == pPort->sDupeInfo.eDupPortType)
      {
        if ((pPort->sPortDef.bEnabled) ||
            (pPort->sDupeInfo.u.sDupMainPort.nDupPortsEnabledBitMask))
        {
          bPortEnabled = OMX_TRUE;
        }
      }
    }

    return (bPortEnabled);
  }


/*===============================================================*/
/** @fn OMX_BASE_PROCESS_Events :
 *      This function is called in both Active/passive context of
 *      a component which basically does process the command
 *      and data events
 *
 * @param hComponent  : handle of the component
 * @param retEvent    : Event to be processed
 */
/*===============================================================*/
  OMX_ERRORTYPE OMX_BASE_PROCESS_Events (OMX_HANDLETYPE hComponent,
                                         OMX_U32 retEvent);

/*===============================================================*/
/** @fn OMX_BASE_PROCESS_TriggerEvent :
 *      This is executed only in Active context to trigger
 *      component thread
 *
 *  @param hComponent  : handle of component
 *  @param EventToSet  : event to be set
 */
/*===============================================================*/
  OMX_ERRORTYPE OMX_BASE_PROCESS_TriggerEvent (OMX_HANDLETYPE hComponent,
                                               OMX_U32 EventToSet);

/*===============================================================*/
/** @fn OMX_BASE_CompThreadEntry :
 *      Component Thread function entry to process
 */
/*===============================================================*/
  void OMX_BASE_CompThreadEntry (OMX_U32 nArgs, void *arg);


/*===============================================================*/
/** @fn OMX_BASE_PROCESS_Events :
 *  see omx_base_internal.h
 */
/*===============================================================*/
  OMX_ERRORTYPE OMX_BASE_CB_ReturnEventNotify (OMX_HANDLETYPE hComponent,
                                               OMX_EVENTTYPE eEvent,
                                               OMX_U32 nData1, OMX_U32 nData2,
                                               OMX_PTR pEventData);

/*===============================================================*/
/** @fn _OMX_BASE_HandleStateTransition :
 *      To handle state transitions
 */
/*===============================================================*/
  OMX_ERRORTYPE _OMX_BASE_HandleStateTransition (OMX_HANDLETYPE hComponent,
                                                 OMX_U32 nParam);

  OMX_ERRORTYPE _OMX_BASE_HandleStateTransitionComplete (OMX_HANDLETYPE
                                                         hComponent,
                                                         OMX_U32 nParam);


/*===============================================================*/
/** @fn OMX_BASE_PROCESS_Events :
 *  see omx_base_internal.h
 */
/*===============================================================*/
  OMX_ERRORTYPE _OMX_BASE_InitializePorts (OMX_HANDLETYPE hComponent);

/*===============================================================*/
/** @fn OMX_BASE_PROCESS_Events :
 *  see omx_base_internal.h
 */
/*===============================================================*/
  OMX_ERRORTYPE _OMX_BASE_DeinitializePorts (OMX_HANDLETYPE hComponent);

/*===============================================================*/
/** @fn OMX_BASE_PROCESS_Events :
 *  see omx_base_internal.h
 */
/*===============================================================*/
  OMX_ERRORTYPE _OMX_BASE_TunnelEstablish (OMX_HANDLETYPE hComponent,
                                           OMX_U32 nPort,
                                           OMX_HANDLETYPE hTunnelComp,
                                           OMX_U32 nTunnPort);

/*===============================================================*/
/** @fn OMX_BASE_PROCESS_Events :
 *  see omx_base_internal.h
 */
/*===============================================================*/
  OMX_ERRORTYPE _OMX_BASE_DisablePort (OMX_HANDLETYPE hComponent,
                                       OMX_U32 nParam);

/*===============================================================*/
/** @fn OMX_BASE_PROCESS_Events :
 *  see omx_base_internal.h
 */
/*===============================================================*/
  OMX_ERRORTYPE _OMX_BASE_EnablePort (OMX_HANDLETYPE hComponent,
                                      OMX_U32 nParam);

/*===============================================================*/
/** @fn OMX_BASE_PROCESS_Events :
 *  see omx_base_internal.h
 */
/*===============================================================*/
  OMX_ERRORTYPE _OMX_BASE_FlushBuffers (OMX_HANDLETYPE hComponent,
                                        OMX_U32 nParam);

/*===============================================================*/
/** @fn _OMX_BASE_GetChannelInfo :
 *  see omx_base_internal.h
 */
/*===============================================================*/
  OMX_ERRORTYPE _OMX_BASE_GetChannelInfo (OMX_BASE_PORTTYPE * pPort,
                                          OMX_STRING cChannelName);

/*===============================================================*/
/** @fn _OMX_BASE_NotifyClient :
 *  see omx_base_internal.h
 */
/*===============================================================*/
  OMX_ERRORTYPE _OMX_BASE_EventNotifyToClient (OMX_HANDLETYPE hComponent,
                                               OMX_COMMANDTYPE Cmd,
                                               OMX_U32 nParam,
                                               OMX_PTR pCmdData);

/*===============================================================*/
/** @fn _OMX_BASE_VerifyTunnelConnection :
 *  see omx_base_internal.h
 */
/*===============================================================*/
  OMX_ERRORTYPE _OMX_BASE_VerifyTunnelConnection (OMX_BASE_PORTTYPE * pPort,
                                                  OMX_HANDLETYPE hTunneledComp);



  OMX_S32 _OMX_BASE_FindInTable (OMX_EXTRADATATYPE eExtraDataType);

/*----------        DIO function prototypes      ------------------ */
/*===============================================================*/
/** @fn OMX_BASE_DIO_Init      : Initialize DIO
 *
 * @ param hComponent [in]     : Handle of the component.
 * @ param nPortIndex [in]     : Index of the port.
 * @ param cChannelType [in]   : Channel type.
 * @ param pCreateParams [in]  : Create time parameters. 
 *                               Also see omx_base_dio_plugin.h
 */
/*===============================================================*/
  OMX_ERRORTYPE OMX_BASE_DIO_Init (OMX_HANDLETYPE hComponent,
                                   OMX_U32 nPortIndex,
                                   OMX_STRING cChannelType,
                                   OMX_PTR pCreateParams);

/*===============================================================*/
/** @fn OMX_BASE_DIO_GetPort      : Initialize DIO
 *
 * @ param hComponent [in]     : Handle of the component.
 * @ param nPortIndex [in]     : Index of the port.
 */
/*===============================================================*/
  OMX_PTR OMX_BASE_DIO_GetPort (OMX_HANDLETYPE hComponent, OMX_U32 nPortIndex);


/*===============================================================*/
/** @fn OMX_BASE_DIO_Open      : Open DIO. Allocates buffers and buffer headers
 *                               depending on supplier/non-supplier.
 *
 * @ param hComponent [in]     : Handle of the component.
 * @ param nPortIndex [in]     : Index of the port.
 * @ param pOprnParams [in]    : Open parameters. 
 *                               Also see omx_base_dio_plugin.h
 */
/*===============================================================*/
  OMX_ERRORTYPE OMX_BASE_DIO_Open (OMX_HANDLETYPE hComponent,
                                   OMX_U32 nPortIndex, OMX_PTR pOpenParams);


/*===============================================================*/
/** @fn OMX_BASE_DIO_Close     : Close DIO. Frees up buffers if supplier.
 *
 * @ param hComponent [in]     : Handle of the component.
 * @ param nPortIndex [in]     : Index of the port.
 */
/*===============================================================*/
  OMX_ERRORTYPE OMX_BASE_DIO_Close (OMX_HANDLETYPE hComponent,
                                    OMX_U32 nPortIndex);


/*===============================================================*/
/** @fn OMX_BASE_DIO_Queue     : Queue up the buffer internally.
 *
 * @ param hComponent [in]     : Handle of the component.
 * @ param nPortIndex [in]     : Index of the port.
 * @ param pBuffHeader [in]    : Buffer header to be queued up.
 */
/*===============================================================*/
  OMX_ERRORTYPE OMX_BASE_DIO_Queue (OMX_HANDLETYPE hComponent,
                                    OMX_U32 nPortIndex, OMX_PTR pBuffHeader);


/*===============================================================*/
/** @fn OMX_BASE_DIO_Dequeue    : Dequeue a buffer header from the internal 
 *                                queue for processing. The caller provides
 *                                a local buffer header which the DIO layer
 *                                can read for certain parameters such as 
 *                                buffer size required. Rest of the parameters
 *                                in the available header are then copied to 
 *                                the local header to be used by the caller.
 *                        
 *
 * @ param hComponent [in]      : Handle of the component.
 * @ param nPortIndex [in]      : Index of the port.
 * @ param pBuffHeader [in out] : Buffer header.
 */
/*===============================================================*/
  OMX_ERRORTYPE OMX_BASE_DIO_Dequeue (OMX_HANDLETYPE hComponent,
                                      OMX_U32 nPortIndex, OMX_PTR pBuffHeader);


/*===============================================================*/
/** @fn OMX_BASE_DIO_Send    : Send the buffer. The caller will provide  a 
 *                             local buffer header which will be copied and the
 *                             copy will be sent across.
 *
 * @ param hComponent [in]   : Handle of the component.
 * @ param nPortIndex [in]   : Index of the port.
 * @ param pBuffHeader [in]  : Buffer header to be sent.
 */
/*===============================================================*/
  OMX_ERRORTYPE OMX_BASE_DIO_Send (OMX_HANDLETYPE hComponent,
                                   OMX_U32 nPortIndex, OMX_PTR pBuffHeader);


/*===============================================================*/
/** @fn OMX_BASE_DIO_Cancel  : Cancel the buffer. The next dequeue call will
 *                             return the same buffer.
 *
 * @ param hComponent [in]   : Handle of the component.
 * @ param nPortIndex [in]   : Index of the port.
 * @ param pBuffHeader [in]  : Buffer header to be canceled.
 */
/*===============================================================*/
  OMX_ERRORTYPE OMX_BASE_DIO_Cancel (OMX_HANDLETYPE hComponent,
                                     OMX_U32 nPortIndex, OMX_PTR pBuffHeader);


/*===============================================================*/
/** @fn OMX_BASE_DIO_Dup           : Duplicate a buffer header. Used when the
 *                                   caller wants to hold on to a buffer and
 *                                   share with others. The duplicate buffer 
 *                                   header will be a local header provided by
 *                                   the caller. After returning, the caller 
 *                                   can send the original buffer and retain 
 *                                   the duplicate, to be canceled later.
 *
 * @ param hComponent [in]         : Handle of the component.
 * @ param nPortIndex [in]         : Index of the port.
 * @ param pBuffHeader [in]        : Buffer header to be duplicated.
 * @ param pDupBuffHeader [in out] : Duplicate buffer header.
 */
/*===============================================================*/
  OMX_ERRORTYPE OMX_BASE_DIO_Dup (OMX_HANDLETYPE hComponent,
                                  OMX_U32 nPortIndex,
                                  OMX_PTR pBuffHeader, OMX_PTR pDupBuffHeader);


/*===============================================================*/
/** @fn OMX_BASE_DIO_Control : Control call.
 *
 * @ param hComponent [in]   : Handle of the component.
 * @ param nPortIndex [in]   : Index of the port.
 * @ param nCmdType [in]     : Control command. See omx_base.h
 * @ param pParams [in]      : Any control parameters.
 */
/*===============================================================*/
  OMX_ERRORTYPE OMX_BASE_DIO_Control (OMX_HANDLETYPE hComponent,
                                      OMX_U32 nPortIndex,
                                      OMX_BASE_DIO_CTRLCMD_TYPE nCmdType,
                                      OMX_PTR pParams);

/*===============================================================*/
/** @fn OMX_BASE_DIO_DupControl : Control call.
 *
 * @ param hComponent [in]   : Handle of the component.
 * @ param nPortIndex [in]   : Index of the port.
 * @ param nCmdType [in]     : Control command. See omx_base.h
 * @ param pParams [in]      : Any control parameters.
 */
/*===============================================================*/
  OMX_ERRORTYPE OMX_BASE_DIO_DupControl (OMX_HANDLETYPE hComponent,
                                         OMX_U32 nPortIndex,
                                         OMX_BASE_DIO_CTRLCMD_TYPE nCmdType,
                                         OMX_PTR pParams);


/*===============================================================*/
/** @fn OMX_BASE_DIO_Util    : Utility functions.
 *
 * @ param hComponent [in]   : Handle of the component.
 * @ param nPortIndex [in]   : Index of the port.
 * @ param nUtilType [in]    : Utility type. See omx_base.h
 * @ param pParams [in]      : Any parameters.
 */
/*===============================================================*/
  OMX_ERRORTYPE OMX_BASE_DIO_Util (OMX_HANDLETYPE hComponent,
                                   OMX_U32 nPortIndex,
                                   OMX_BASE_DIO_UTIL_TYPE nUtilType,
                                   OMX_PTR pParams);



/*===============================================================*/
/** @fn OMX_BASE_DIO_GetCount : Get no. of buffers available.
 *
 * @ param hComponent [in]    : Handle of the component.
 * @ param nPortIndex [in]    : Index of the port.
 * @ param pCount [out]       : Count of the buffers available.
 */
/*===============================================================*/
  OMX_ERRORTYPE OMX_BASE_DIO_GetCount (OMX_HANDLETYPE hComponent,
                                       OMX_U32 nPortIndex, OMX_U32 * pCount);


/*===============================================================*/
/** @fn OMX_BASE_DIO_Deinit   : Deinitialize DIO.
 *
 * @ param hComponent [in]    : Handle of the component.
 * @ param nPortIndex [in]    : Index of the port.
 */
/*===============================================================*/
  OMX_ERRORTYPE OMX_BASE_DIO_Deinit (OMX_HANDLETYPE hComponent,
                                     OMX_U32 nPortIndex);

/*===============================================================*/
/** @fn OMX_BASE_DIO_DupMainDeinit   : Deinitialize DIO.
 *
 * @ param hComponent [in]    : Handle of the component.
 * @ param nPortIndex [in]    : Index of the port.
 */
/*===============================================================*/
  OMX_ERRORTYPE OMX_BASE_DIO_DupMainDeinit (OMX_HANDLETYPE hComponent,
                                            OMX_U32 nPortIndex);

/*===============================================================*/
/** @fn OMX_BASE_DIO_GroupCreate   : Deinitialize DIO.
 *
 * @ param hComponent [in]    : Handle of the component.
 * @ param szGroupName [in]   : Name used to identify the port Group
 * @ param pPortIndexGroup [in]  : Array of port indexes that will part of group
 * @ param nNumPortsInGroup [in] : Number of ports in this group
 * @ param hDioGroupHandle [out] : Pointer to Group Handle - Populated in this function
 */
/*===============================================================*/
  OMX_ERRORTYPE OMX_BASE_DIO_GroupCreate (OMX_HANDLETYPE hComponent,
                                          char *szGroupName,
                                          OMX_U32 pPortIndexGroup[],
                                          OMX_U32 nNumPortsInGroup,
                                          OMX_PTR * hDioGroupHandle);

/*============================================================================*/
/** @fn OMX_BASE_DIO_GroupGetCount   : Get no. of buffers available in multi 
 *                                      port group
 *
 * @ param hComponent [in]        : Handle of the component.
 * @ param hDIOGroupHandle [in]   : Handle to the multi port group
 * @ param pPortIndex      [in]   : Array of port indexes for which count required
 * @ param pCount          [out]  : Number of buffers available for each port in pPortIndex
 * @ param nNumPorts       [in]   : Number of ports in pPortIndex
 */
/*===========================================================================*/
  OMX_ERRORTYPE OMX_BASE_DIO_GroupGetCount (OMX_HANDLETYPE hComponent,
                                            OMX_HANDLETYPE hDIOGroupHandle,
                                            OMX_U32 pPortIndex[],
                                            OMX_U32 pCount[],
                                            OMX_U32 nNumPorts);

/*============================================================================*/
/** @fn OMX_BASE_DIO_GroupSend   : Send the header - multi  port group version
 *
 * @ param hComponent [in]        : Handle of the component.
 * @ param hDIOGroupHandle [in]   : Handle to the multi port group
 * @ param pPortIndex      [in]   : Array of port indexes for which send required
 * @ param pBuffHeader     [in]   : Array of OmxBufferHeaders to be sent
 * @ param nNumPorts       [in]   : Number of ports in pPortIndex
 */
/*============================================================================*/
  OMX_ERRORTYPE OMX_BASE_DIO_GroupSend (OMX_HANDLETYPE hComponent,
                                        OMX_HANDLETYPE hDIOGroupHandle,
                                        OMX_U32 pPortIndex[],
                                        OMX_PTR pBuffHeader[],
                                        OMX_U32 nNumPorts);

/*============================================================================*/
/** @fn OMX_BASE_DIO_GroupCancel  : Send the header - multi  port group version
 *
 * @ param hComponent [in]        : Handle of the component.
 * @ param hDIOGroupHandle [in]   : Handle to the multi port group
 * @ param pPortIndex      [in]   : Array of port indexes for which send required
 * @ param pBuffHeader     [in]   : Array of OmxBufferHeaders to be sent
 * @ param nNumPorts       [in]   : Number of ports in pPortIndex
 */
/*============================================================================*/
  OMX_ERRORTYPE OMX_BASE_DIO_GroupCancel (OMX_HANDLETYPE hComponent,
                                          OMX_HANDLETYPE hDIOGroupHandle,
                                          OMX_U32 pPortIndex[],
                                          OMX_PTR pBuffHeader[],
                                          OMX_U32 nNumPorts);

/*============================================================================*/
/** @fn OMX_BASE_DIO_GroupDequeue   : Dequeue the header - multi  port group version
 *
 * @ param hComponent [in]        : Handle of the component.
 * @ param hDIOGroupHandle [in]   : Handle to the multi port group
 * @ param pPortIndex      [in]   : Array of port indexes for which dequeue required
 * @ param pBuffHeader     [in]   : Array of OmxBufferHeaders to be sent
 * @ param nNumPorts       [in]   : Number of ports in pPortIndex
 */
/*============================================================================*/
  OMX_ERRORTYPE OMX_BASE_DIO_GroupDequeue (OMX_HANDLETYPE hComponent,
                                           OMX_HANDLETYPE hDIOGroupHandle,
                                           OMX_U32 pPortIndex[],
                                           OMX_PTR pBuffHeader[],
                                           OMX_U32 nNumPorts);


/*============================================================================*/
/** @fn OMX_BASE_RegisterLog   : Registration of OMX base with Log.
 *
 */
/*============================================================================*/
  void OMX_BASE_RegisterLog (void);

/*===============================================================*/
/** @fn OMX_BASE_GetUVBuffer    : In case of packed format for NV12 buffers 
 *                                with 2D buffers, the
 *                                pBuffer field in the buffer header points to 
 *                                only the Y buffer. Calling this function
 *                                returns the corresponding UV buffer.
 *
 *  @ params [IN] hComponent    : Component handle.
 *  @ params [IN] nPortIndex    : Port index.
 *  @ params [IN] pBufHdr       : Pointer to the buffer header.
 *  @ params [OUT] pUVBuffer    : Pointer to the UV buffer is filled here and 
 *                                returned to the caller.
 */
/*===============================================================*/
  OMX_ERRORTYPE OMX_BASE_GetUVBuffer (OMX_HANDLETYPE hComponent,
                                      OMX_U32 nPortIndex,
                                      OMX_PTR pBufHdr, OMX_PTR * pUVBuffer);
  OMX_ERRORTYPE OMX_BASE_SaveDioGroupInfo (OMX_HANDLETYPE hComponent,
                                           OMX_PTR hDioGroupHandle);

  OMX_ERRORTYPE OMX_BASE_RestoreDioGroupInfo (OMX_HANDLETYPE hComponent);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _OMX_BASE_INTERNAL_H_ */
