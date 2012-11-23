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
 *  @file  OmxRpcStub.c
 *         DOMX RPC stub functionality 
 *
 *  @path \avmecomp\ti\omx\domx\OmxRpcStub.c
 *
 *  @rev 1.0
 */

 /******************************************************************
 *   INCLUDE FILES
 ******************************************************************/

#include <xdc/std.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <xdc/runtime/IHeap.h>
#ifndef _LOCAL_CORE_a8host_
  #include <xdc/runtime/Log.h>
  #include <xdc/runtime/Assert.h>
  #include <xdc/runtime/Diags.h>
#else
  /* Use Slog at A8 side */
  #if (USE_SLOG_PRINT == 1)
    #include <Log.h>
    #include <LoggerSys.h>

    /* Every module needs to define unique module id */
    #undef Module__MID
    #define Module__MID SLOG_DOMX_Module__MID
  #else
    #include <xdc/runtime/Log.h>
    #include <xdc/runtime/Assert.h>
    #include <xdc/runtime/Diags.h>
  #endif /* End Of #if (USE_SLOG_PRINT == 1) */
#endif /* End of #else */
#include <ti/omx/interfaces/openMaxv11/OMX_Types.h>
#include <ti/omx/interfaces/openMaxv11/OMX_Index.h>
#include <ti/omx/interfaces/openMaxv11/OMX_Core.h>
#include <ti/omx/interfaces/openMaxv11/OMX_Component.h>
#include <ti/omx/interfaces/openMaxv11/OMX_Audio.h>
#include <ti/omx/interfaces/openMaxv11/OMX_IVCommon.h>
#include <ti/omx/interfaces/openMaxv11/OMX_Image.h>
#include <ti/omx/interfaces/openMaxv11/OMX_Video.h>
#include <ti/omx/interfaces/openMaxv11/OMX_Other.h>
#include <ti/omx/interfaces/openMaxv11/OMX_ContentPipe.h>
#include <ti/omx/interfaces/openMaxv11/interfaces_openmaxv11_internal.h>
#include <ti/omx/domx/interfaces/domx_interfaces_internal.h>
#include <ti/omx/domx/delegates/shmem/domx_delegates_shmem_internal.h>
#include "domx_internal.h"
#include "domx_internalcfg.h"
#include "OmxRpc__epilogue.h"
#include "domx_util.h"



/**
 * @def    OMXRPC_RSRC_GETHANDLE_NONE
 * @def    OMXRPC_RSRC_GETHANDLE_MSGALLOC
 * @def    OMXRPC_RSRC_GETHANDLE_COMPONENT_RCM_CLIENT_CREATE
 * @def    OMXRPC_RSRC_GETHANDLE_COMPONENT_RCM_ETB_CLIENT_CREATE
 * @def    OMXRPC_RSRC_GETHANDLE_COMPONENT_RCM_FTB_CLIENT_CREATE
 * @def    OMXRPC_RSRC_GETHANDLE_ALL
 * @brief  Defines of resources allocated at OmxRpc GetHandle time 
 */

/* Bit values for each of the getHandle types */

/* Value 0x0 - binary value: 0000 0000 */
#define OMXRPC_RSRC_GETHANDLE_NONE                           0

/* Value 0x1 - binary value: 0000 0001 */
#define OMXRPC_RSRC_GETHANDLE_MSGALLOC                       (1 << 0)

/* Value 0x2 - binary value: 0000 0010 */
#define OMXRPC_RSRC_GETHANDLE_COMPONENT_RCM_CLIENT_CREATE    (1 << 1)

/* Value 0x4 - binary value: 0000 0100 */
#define OMXRPC_RSRC_GETHANDLE_COMPONENT_RCM_ETB_CLIENT_CREATE    (1 << 2)

/* Value 0x8 - binary value: 0000 1000 */
#define OMXRPC_RSRC_GETHANDLE_COMPONENT_RCM_FTB_CLIENT_CREATE    (1 << 3)

/* Value 0xF - binary value: 0000 1111 */
#define OMXRPC_RSRC_GETHANDLE_ALL        (                                     \
                       OMXRPC_RSRC_GETHANDLE_MSGALLOC                        | \
                       OMXRPC_RSRC_GETHANDLE_COMPONENT_RCM_CLIENT_CREATE     | \
                       OMXRPC_RSRC_GETHANDLE_COMPONENT_RCM_ETB_CLIENT_CREATE | \
                       OMXRPC_RSRC_GETHANDLE_COMPONENT_RCM_FTB_CLIENT_CREATE)

/**
 * @name omxrpc_gethandle_freersrc
 * @brief OmxRpc function to free resources allocated at OmxRpcStub_getHandle time
 * @param  rsrcMask         : Resource allocated bitfield 
 * @param  rcmHandleMsgAlloc: RcmClient Handle
 * @param  rcmMsg           : Pointer to rcmMsg
 * @param  pComponentRcmClientHandlePtr : Pointer to component RcmClient Handle
 * @param  pComponentRcmClientETBHandlePtr: Pointer to component RcmClient for ETB
 * @param  pComponentRcmClientFTBHandlePtr: Pointer to component RcmClient for FTB
 * @return NONE
 */
static Void omxrpc_gethandle_freersrc (Int rsrcMask,
                                       RcmClient_Handle rcmHandleMsgAlloc,
                                       RcmClient_Message *rcmMsg,
                                       RcmClient_Handle
                                         *pComponentRcmClientHandlePtr,
                                       RcmClient_Handle
                                         *pComponentRcmClientETBHandlePtr,
                                       RcmClient_Handle 
                                         *pComponentRcmClientFTBHandlePtr)
{
  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();
  if (rsrcMask & OMXRPC_RSRC_GETHANDLE_MSGALLOC)
  {
    /* return message to the heap */
    DOMX_UTL_TRACE_FUNCTION_MSG ("Before RcmClient_free");
    RcmClient_free (rcmHandleMsgAlloc, rcmMsg);
    DOMX_UTL_TRACE_FUNCTION_MSG ("After RcmClient_free");
  }
  if (rsrcMask & OMXRPC_RSRC_GETHANDLE_COMPONENT_RCM_ETB_CLIENT_CREATE)
  {
    /* delete the Rcm client */
    RcmClient_delete (pComponentRcmClientETBHandlePtr);
  }

  if (rsrcMask & OMXRPC_RSRC_GETHANDLE_COMPONENT_RCM_FTB_CLIENT_CREATE)
  {
    /* delete the Rcm client */
    RcmClient_delete (pComponentRcmClientFTBHandlePtr);
  }
  if (rsrcMask & OMXRPC_RSRC_GETHANDLE_COMPONENT_RCM_CLIENT_CREATE)
  {
    /* delete the Rcm client */
    DOMX_UTL_TRACE_FUNCTION_MSG ("Before RcmClient_delete");
    RcmClient_delete (pComponentRcmClientHandlePtr);
    DOMX_UTL_TRACE_FUNCTION_MSG ("After RcmClient_delete");
  }

  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (OMX_ErrorNone);
}

/**
 * @def    OMXRPC_RSRC_GETHANDLELITE_NONE
 * @def    OMXRPC_RSRC_GETHANDLELITE_COMPONENT_RCM_CLIENT_CREATE
 * @def    OMXRPC_RSRC_GETHANDLELITE_ALL
 * @brief  Defines of resources allocated at OmxRpc GetHandle Lite time 
 */

/* Bit values for each of the getHandle types */

/* Value 0x0 - binary value: 0000 0000 */
#define OMXRPC_RSRC_GETHANDLELITE_NONE                                       (0)

/* Value 0x0 - binary value: 0000 0001 */
#define OMXRPC_RSRC_GETHANDLELITE_COMPONENT_RCM_CLIENT_CREATE           (1 << 0)

#define OMXRPC_RSRC_GETHANDLELITE_ALL        (                                 \
	                          OMXRPC_RSRC_GETHANDLELITE_COMPONENT_RCM_CLIENT_CREATE)

/**
 * @name omxrpc_gethandlelite_freersrc
 * @brief OmxRpc function to free resources allocated at OmxRpcStub_getHandleLite time
 * @param  rsrcMask         : Resource allocated bitfield 
 * @param  pComponentRcmClientHandlePtr : Pointer to component RcmClient Handle
 * @return NONE
 */
static Void omxrpc_gethandlelite_freersrc (Int rsrcMask,
                                           RcmClient_Handle 
                                             *pComponentRcmClientHandlePtr)
{
  if (rsrcMask & OMXRPC_RSRC_GETHANDLELITE_COMPONENT_RCM_CLIENT_CREATE)
  {
    /* delete the Rcm client */
    RcmClient_delete (pComponentRcmClientHandlePtr);
  }
}

/**
 * @name omxrpc_msg_marshall_gethandle
 * @brief OmxRpc function to marshall params of OmxRpc GetHandle msg
 * @param  pGetHandleMsg     : Pointer to msg
 * @param  cComponentName    : Name of the OMX component
 * @param  cEventCbServername: Name of CallBack RcmServer 
 * @param  nOmxComponentRcmServerPriority : Priority of ComponentRcmServer
 * @param  stubCoreId       : OmxRpc stub coreID       
 * @param  pRpcStubHndl     : RpcStub Handle
 * @return None
 */
static Void omxrpc_msg_marshall_gethandle (OmxRpc_msgGetHandle *pGetHandleMsg,
                                           String cComponentName,
                                           String cEventCbServername,
                                           xdc_Int
                                             nOmxComponentRcmServerPriority,
                                           DomxTypes_coreType stubCoreId,
                                           OmxRpc_Handle pRpcStubHndl)
{
  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();
  DOMX_UTL_Strncpy ((OMX_STRING) pGetHandleMsg->componentName,
                    (OMX_STRING) cComponentName,
                    sizeof (pGetHandleMsg->componentName));
  DOMX_UTL_Strncpy ((OMX_STRING) pGetHandleMsg->componentEventCbServerName,
                    (OMX_STRING) cEventCbServername,
                    sizeof (pGetHandleMsg->componentEventCbServerName));
  pGetHandleMsg->rcmOmxComponentServerPriority = nOmxComponentRcmServerPriority;
  pGetHandleMsg->stubCoreId = stubCoreId;
  pGetHandleMsg->stubHandle = pRpcStubHndl;
  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (OMX_ErrorNone);
}

/**
 * @name omxrpc_msg_unmarshall_gethandle_response
 * @brief OmxRpc function to unmarshall params of OmxRpc GetHandle response msg
 * @param  pGetHandleMsg     : Pointer to msg
 * @param  ppSkelHandlePtr   : Pointer to RpcSkel Handle
 * @param  ppOmxComponentHandlePtr: Pointer to OMX COmponent Handle pointer
 * @param  nOmxComponentRcmServerPriority : Priority of ComponentRcmServer
 * @param  cComponentCtrlRcmServerName    : ComponentRcmServer name
 * @param  nServerNameBufSize             : Max size of buffer holding RcmServer name
 * @return None
 */
static Void
  omxrpc_msg_unmarshall_gethandle_response (OmxRpc_msgGetHandleResponse
                                              *pGetHandleResMsg,
                                            OmxRpc_Handle *ppSkelHandlePtr,
                                            OMX_HANDLETYPE
                                              *ppOmxComponentHandlePtr,
                                            OmxTypes_OMX_S8
                                              *cComponentCtrlRcmServerName,
                                            size_t nServerNameBufSize)
{
  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();
  *ppSkelHandlePtr = pGetHandleResMsg->skelHandle;
  *ppOmxComponentHandlePtr = pGetHandleResMsg->pOmxComponentHandle;
  DOMX_UTL_Strncpy ((OMX_STRING) cComponentCtrlRcmServerName,
                    (OMX_STRING) pGetHandleResMsg->componentCtrlServerName,
                    nServerNameBufSize);
  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (OMX_ErrorNone);
}

/**
 * @name omxrpc_msg_marshall_setparamconfig
 * @brief OmxRpc function to marshall params of OmxRpc setParam/Config msg
 * @param  pCfgParamMsg      : Pointer to msg
 * @param  nParamConfigIndex : Param/Config index
 * @param  pCompParamConfig  : Pointer to Param/Config structure
 * @param  nSizeofCompParamStruct : size of param/config structure
 * @return None
 */
static Void
  omxrpc_msg_marshall_setparamconfig (OmxRpc_msgConfigParam
                                        *pCfgParamMsg,
                                      OmxIndex_OMX_INDEXTYPE
                                        nParamConfigIndex,
                                      OmxTypes_OMX_PTR
                                        pCompParamConfig,
                                      SizeT nSizeofCompParamStruct)
{
  pCfgParamMsg->nParamConfigIndex = nParamConfigIndex;
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((nSizeofCompParamStruct <=
                                   sizeof (pCfgParamMsg->cCompParamStructBuf)),
                                  "paramStruct size incorrect");

    /*************************************************************************
    * TODO: Doing shallow copy .
    * Important!! need to look for spl case where we need to more specific
    *************************************************************************/
  memcpy (pCfgParamMsg->cCompParamStructBuf, pCompParamConfig,
          nSizeofCompParamStruct);
}

/**
 * @name omxrpc_msg_marshall_getparamconfig
 * @brief OmxRpc function to marshall params of OmxRpc getParam/Config msg
 * @param  pCfgParamQueryMsg: Pointer to msg
 * @param  nParamConfigIndex : Param/Config index
 * @param  pCompParamConfig  : Pointer to Param/Config structure
 * @param  nSizeofCompParamStruct : size of param/config structure
 * @return None
 */
static Void
  omxrpc_msg_marshall_getparamconfig (OmxRpc_msgConfigParamQuery
                                        *pCfgParamQueryMsg,
                                      OmxIndex_OMX_INDEXTYPE
                                        nParamConfigIndex,
                                      OmxTypes_OMX_PTR
                                        pCompParamConfig,
                                      SizeT nSizeofCompParamStruct)
{
  pCfgParamQueryMsg->nParamConfigIndex = nParamConfigIndex;
  pCfgParamQueryMsg->nSizeofCompParamStruct = nSizeofCompParamStruct;
  /* even for a GetParameter we are filling some fields in the pCompParamConfig
     structure hence we are copying the entire stucture as is. */
  memcpy (pCfgParamQueryMsg->cCompParamStructBuf, pCompParamConfig,
          nSizeofCompParamStruct);
}

/**
 * @name omxrpc_msg_unmarshall_getparamconfig_response
 * @brief OmxRpc function to unmarshall params of OmxRpc getParam/Config msg response
 * @param  pCfgParamResMsg   : Pointer to msg
 * @param  pCompParamConfig  : Pointer to Param/Config structure
 * @param  nSizeofCompParamStruct : size of param/config structure
 * @return None
 */
static Void
  omxrpc_msg_unmarshall_getparamconfig_response (OmxRpc_msgConfigParamQuery
                                                   *pCfgParamResMsg,
                                                 OmxTypes_OMX_PTR
                                                   pCompParamConfig,
                                                 SizeT nSizeofCompParamStruct)
{

    /*************************************************************************
    * TODO: Doing shallow copy .
    * Important!! need to look for spl case where we need to more specific
    *************************************************************************/
  memcpy (pCompParamConfig,
          pCfgParamResMsg->cCompParamStructBuf, nSizeofCompParamStruct);
}

/**
 * @name omxrpc_msg_marshall_sendcommand
 * @brief OmxRpc function to marshall params of OmxRpc sendCommand msg
 * @param  pSendCmdMsg       : Pointer to msg
 * @param  eCmd              : SendCommand enum
 * @param  nParam            : Command Param
 * @param  pCmdData          : Pointer to command data
 * @return None
 */
static Void omxrpc_msg_marshall_sendcommand (OmxRpc_msgSendCommand
                                               *pSendCmdMsg,
                                             OmxCore_OMX_COMMANDTYPE eCmd,
                                             OmxTypes_OMX_U32 nParam,
                                             OmxTypes_OMX_PTR pCmdData)
{
  pSendCmdMsg->eCmd = eCmd;
  pSendCmdMsg->nParam = nParam;
  if (eCmd == OMX_CommandMarkBuffer)
  {
    pSendCmdMsg->markData = *((OmxTypes_OMX_MARKTYPE *) pCmdData);
  }
}

/**
 * @name omxrpc_msg_unmarshall_sendcommand_response
 * @brief OmxRpc function to ubmarshall params of OmxRpc sendCommand msg response
 * @return None
 */
static Void omxrpc_msg_unmarshall_sendcommand_response ()
{
  /* Dummy function. Send commmand response has no api specific params */
}

/**
 * @name omxrpc_msg_marshall_getstate
 * @brief OmxRpc function to marshall params of OmxRpc getState msg 
 * @return None
 */
static Void omxrpc_msg_marshall_getstate ()
{
  /* Dummy function. GetState commmand response has no api specific params */
}

/**
 * @name omxrpc_msg_unmarshall_getstate_response
 * @brief OmxRpc function to unmarshall params of OmxRpc getState msg  response
 * @param  pGetStateResponseMsg : Pointer to msg
 * @param  pState               : Pointer to OMXState enum to be set
 * @return None
 */
static Void
  omxrpc_msg_unmarshall_getstate_response (OmxRpc_msgGetStateResponse
                                             *pGetStateResponseMsg,
                                           OmxCore_OMX_STATETYPE *pState)
{
  *pState = pGetStateResponseMsg->eState;
}

/**
 * @name omxrpc_msg_marshall_getheapmemstats
 * @brief OmxRpc function to unmarshall params of OmxRpc getState msg  response
 * @param  pGetStateResponseMsg : Pointer to msg
 * @param  pState               : Pointer to OMXState enum to be set
 * @return None
 */
static Void omxrpc_msg_marshall_getheapmemstats ()
{
  /* Dummy function. GetHeapMemStats commmand has no api specific params */
}

/**
 * @name omxrpc_msg_unmarshall_getheapmemstats_response
 * @brief OmxRpc function to unmarshall params of OmxRpc getState msg  response
 * @param  pGetStateResponseMsg : Pointer to msg
 * @param  pState               : Pointer to OMXState enum to be set
 * @return None
 */
static Void
  omxrpc_msg_unmarshall_getheapmemstats_response
    (OmxRpc_msgGetHeapMemStatsResponse *pGetHeapMemStatsResponseMsg,
     OMX_U32 *totalFreeSize)
{
  OMX_U32 i;
  for (i = 0; i < DOMX_INT_RPC_MSGHEAPMEMSTATRESP; i++)
  {
    totalFreeSize[i] = pGetHeapMemStatsResponseMsg->totalFreeSize[i];
  }
}

/**
 * @name omxrpc_msg_marshall_get_component_version
 * @brief OmxRpc function to marshall params of OmxRpc GetComponentVersion msg 
 * @return None
 */
/**
 * @name omxrpc_msg_marshall_get_component_version
 * @brief OmxRpc function to marshall params of OmxRpc GetComponentVersion msg 
 * @return None
 */
static Void omxrpc_msg_marshall_get_component_version ()
{
  /* Dummy function. GetComponentVersion commmand has no api specific params */
}

/**
 * @name omxrpc_msg_marshall_get_component_version
 * @brief OmxRpc function to unmarshall params of OmxRpc GetComponentVersion msg response 
 * @param pGetComponentVersionResponseMsg : Pointer to msg
 * @param pComponentName                  : Pointer to component name
 * @param pComponentVersion               : Pointer to component version
 * @param pSpecVersion                    : Pointer to spec version 
 * @param pComponentUUID                  : Pointer to component UUID
 * @return None
 */
static Void
  omxrpc_msg_unmarshall_get_component_version_response
    (OmxRpc_msgGetComponentVersionResponse *pGetComponentVersionResponseMsg,
     OmxTypes_OMX_STRING pComponentName,
     OmxTypes_OMX_VERSIONTYPE *pComponentVersion,
     OmxTypes_OMX_VERSIONTYPE *pSpecVersion,
     OmxTypes_OMX_UUIDTYPE *pComponentUUID)
{
  DOMX_UTL_Strncpy ((OMX_STRING) pComponentName,
                    (OMX_STRING) pGetComponentVersionResponseMsg->
                    componentName, OMX_MAX_STRINGNAME_SIZE);
  *pComponentVersion = pGetComponentVersionResponseMsg->componentVersion;
  *pSpecVersion = pGetComponentVersionResponseMsg->specVersion;
  memcpy (pComponentUUID, pGetComponentVersionResponseMsg->componentUUID,
          sizeof (pGetComponentVersionResponseMsg->componentUUID));
}

/**
 * @name omxrpc_msg_marshall_get_extension_index
 * @brief OmxRpc function to marshall params of OmxRpc GetExtensionIndex msg
 * @param pGetExtensionIndexMsg           : Pointer to msg
 * @param cParameterName                  : Param name
 * @return None
 */
static Void
  omxrpc_msg_marshall_get_extension_index (OmxRpc_msgGetExtensionIndex
                                             *pGetExtensionIndexMsg,
                                           OmxTypes_OMX_STRING cParameterName)
{
  DOMX_UTL_Strncpy ((OMX_STRING) pGetExtensionIndexMsg->cParameterName,
                    (OMX_STRING) cParameterName,
                    sizeof (pGetExtensionIndexMsg->cParameterName));
}

/**
 * @name omxrpc_msg_unmarshall_get_extension_index_response
 * @brief OmxRpc function to ubmarshall params of OmxRpc GetExtensionIndex msg response
 * @param pGetExtensionIndexResponseMsg   : Pointer to msg
 * @param pIndexType                      : Pointer to index type to be populated
 * @return None
 */
static Void
  omxrpc_msg_unmarshall_get_extension_index_response
    (OmxRpc_msgGetExtensionIndexResponse *pGetExtensionIndexResponseMsg,
     OmxIndex_OMX_INDEXTYPE *pIndexType)
{
  *pIndexType = pGetExtensionIndexResponseMsg->nIndexType;
}

/**
 * @name omxrpc_msg_marshall_usebuffer
 * @brief OmxRpc function to marshall params of OmxRpc UseBuffer msg
 * @param pUseBufferMsg                   : Pointer to msg
 * @param pAppPrivate                     : Pointer to app private
 * @param nSizeBytes                      : Size of buffer in bytes
 * @param nBufferMappedAddress            : Mapped buffer address
 * @return None
 */
static Void omxrpc_msg_marshall_usebuffer (OmxRpc_msgUseBuffer *pUseBufferMsg,
                                           OmxTypes_OMX_U32 nPortIndex,
                                           OmxTypes_OMX_PTR pAppPrivate,
                                           OmxTypes_OMX_U32 nSizeBytes,
                                           OmxTypes_OMX_U32
                                             nBufferMappedAddress)
{
  pUseBufferMsg->nPortIndex = nPortIndex;
  pUseBufferMsg->pAppPrivate = pAppPrivate;
  pUseBufferMsg->nSizeBytes = nSizeBytes;
  pUseBufferMsg->nBufferMapped = nBufferMappedAddress;
}

/**
 * @name omxrpc_msg_marshall_allocbuffer
 * @brief OmxRpc function to marshall params of OmxRpc AllocBuffer msg
 * @param pAllocBufferMsg                 : Pointer to msg
 * @param pAppPrivate                     : Pointer to app private
 * @param nSizeBytes                      : Size of buffer in bytes
 * @param nBufferMappedAddress            : Mapped buffer address
 * @return None
 */
static Void omxrpc_msg_marshall_allocbuffer (OmxRpc_msgAllocBuffer
                                               *pAllocBufferMsg,
                                             OmxTypes_OMX_U32 nPortIndex,
                                             OmxTypes_OMX_PTR pAppPrivate,
                                             OmxTypes_OMX_U32 nSizeBytes)
{
  pAllocBufferMsg->nPortIndex = nPortIndex;
  pAllocBufferMsg->pAppPrivate = pAppPrivate;
  pAllocBufferMsg->nSizeBytes = nSizeBytes;
}

/**
 * @name omxrpc_msg_unmarshall_usebuffer_response
 * @brief OmxRpc function to unmarshall params of OmxRpc UseBuffer msg response
 * @param pUseBufferResponseMsg           : Pointer to msg
 * @param pBufferHeaderRemote             : Pointer to remote OMXHeader buf
 * @param pBufferHdr                      : Pointer to local OMXHeader buf
 * @param nOMXReturn                      : OMX return value
 * @return None
 */
static Void
  omxrpc_msg_unmarshall_usebuffer_response (OmxRpc_msgUseBufferResponse
                                              *pUseBufferResponseMsg,
                                            OmxCore_OMX_BUFFERHEADERTYPE
                                              **pBufferHeaderRemote,
                                            OmxCore_OMX_BUFFERHEADERTYPE
                                              *pBufferHdr,
                                            OmxCore_OMX_ERRORTYPE nOMXReturn)
{
  if (nOMXReturn == OmxCore_OMX_ErrorNone)
  {
    *pBufferHeaderRemote = pUseBufferResponseMsg->pBufferHeaderRemote;
    *pBufferHdr = pUseBufferResponseMsg->bufferHeaderCopy;
  }
}

/**
 * @name omxrpc_msg_unmarshall_allocbuffer_response
 * @brief OmxRpc function to unmarshall params of OmxRpc AllocBuffer msg response
 * @param pAllocBufferResponseMsg         : Pointer to msg
 * @param pBufferHeaderRemote             : Pointer to remote OMXHeader buf ptr
 * @param pBufferHdr                      : Pointer to local OMXHeader buf
 * @param nBufferMapped                   : Pointer to mapped buffer address
 * @param nOMXReturn                      : OMX return value
 * @return None
 */
static Void
  omxrpc_msg_unmarshall_allocbuffer_response (OmxRpc_msgAllocBufferResponse
                                                *pAllocBufferResponseMsg,
                                              OmxCore_OMX_BUFFERHEADERTYPE
                                                **pBufferHeaderRemote,
                                              OmxCore_OMX_BUFFERHEADERTYPE
                                                *pBufferHdr,
                                              OmxTypes_OMX_U32 *nBufferMapped,
                                              OmxCore_OMX_ERRORTYPE nOMXReturn)
{
  if (nOMXReturn == OmxCore_OMX_ErrorNone)
  {
    *pBufferHeaderRemote = pAllocBufferResponseMsg->pBufferHeaderRemote;
    *pBufferHdr = pAllocBufferResponseMsg->bufferHeaderCopy;
    *nBufferMapped = pAllocBufferResponseMsg->nBufferMapped;
  }
}

/**
 * @name omxrpc_msg_marshall_freebuffer
 * @brief OmxRpc function to marshall params of OmxRpc FreeBuffer msg
 * @param freeBufferMsg                   : Pointer to msg
 * @param nPortIndex                      : Port index
 * @param pBufferHeaderRemote             : Pointer to remote OMXHeader buf ptr
 * @return None
 */
static Void omxrpc_msg_marshall_freebuffer (OMX_OUT OmxRpc_msgFreeBuffer
                                              *freeBufferMsg,
                                            OMX_IN OmxTypes_OMX_U32 nPortIndex,
                                            OMX_IN OmxCore_OMX_BUFFERHEADERTYPE
                                              *pBufHeaderRemote)
{
  freeBufferMsg->nPortIndex = nPortIndex;
  freeBufferMsg->pBufHeaderRemote = pBufHeaderRemote;
}

/**
 * @name omxrpc_msg_unmarshall_freebuffer_response
 * @brief OmxRpc function to unmarshall params of OmxRpc FreeBuffer msg response
 * @return None
 */
static Void omxrpc_msg_unmarshall_freebuffer_response ()
{
  /* Dummy function. FreeBuffer response has no api specific params */
}

/**
 * @name omxrpc_msg_marshall_etb_and_ftb
 * @brief OmxRpc function to marshall params of OmxRpc ETB/FTB msg
 * @param emptyFillBufferMsg              : Pointer to msg
 * @param pBufferHeaderRemote             : Pointer to remote OMXHeader ptr
 * @param pBufferHdr                      : Pointer tolocal OMXHeader  ptr 
 * @return None
 */
static Void 
  omxrpc_msg_marshall_etb_and_ftb (OmxRpc_msgEmptyAndFillThisBuffer
                                     *emptyFillBufferMsg,
                                   OMX_IN OmxCore_OMX_BUFFERHEADERTYPE
                                     *pBufferHdrRemote,
                                   OMX_IN OmxCore_OMX_BUFFERHEADERTYPE
                                     *pBufferHdr)
{
  emptyFillBufferMsg->pBufferHdrRemote = pBufferHdrRemote;
  emptyFillBufferMsg->bufferHdrCopy = *pBufferHdr;
}

/**
 * @name omxrpc_msg_unmarshall_etb_and_ftb_response
 * @brief OmxRpc function to unmarshall params of OmxRpc ETB/FTB msg response
 * @return None
 */
static Void omxrpc_msg_unmarshall_etb_and_ftb_response ()
{
  /* Dummy function. ETB/FTB has no API specific response */
}

/**
 * @name omxrpc_msg_marshall_freehandle
 * @brief OmxRpc function to marshall params of OmxRpc FreeHandle msg
 * @return None
 */
static Void omxrpc_msg_marshall_freehandle ()
{
  /* Dummy function. freeHandle has no API specific params */
}

/**
 * @name omxrpc_msg_unmarshall_freehandle_response
 * @brief OmxRpc function to unmarshall params of OmxRpcFreeHandle response msg
 * @return None
 */
static Void omxrpc_msg_unmarshall_freehandle_response ()
{
  /* Dummy function. freeHandle has no API specific response */
}

/**
 * @name omxrpc_msg_marshall_create_proxy_lite
 * @brief OmxRpc function to marshall params of OmxRpc createProxyLite msg
 * @param createProxyLiteMsg              : Pointer to msg
 * @param nRpcRemoteHandle                : RemoteRpc handle
 * @param pOmxComponentHandle             : Pointer to OMX component handle
 * @param cComponentName                  : OMX component name
 * @param cComponentRcmServerName         : OMX component rcm server name
 * @return None
 */
static Void
  omxrpc_msg_marshall_create_proxy_lite (OmxRpc_msgCreateProxyLite
                                           *createProxyLiteMsg,
                                         OMX_IN OmxTypes_OMX_U32
                                           nRpcRemoteHandle,
                                         OMX_IN OmxTypes_OMX_HANDLETYPE
                                           pOmxComponentHandle,
                                         OMX_IN OmxTypes_OMX_S8
                                           *cComponentName,
                                         OMX_IN OmxTypes_OMX_S8
                                           *cComponentRcmServerName)
{
  createProxyLiteMsg->nRpcRemoteHandle = nRpcRemoteHandle;
  createProxyLiteMsg->pOmxComponentHandle = pOmxComponentHandle;
  DOMX_UTL_Strncpy ((OMX_STRING) createProxyLiteMsg->cComponentName,
                    (OMX_STRING) cComponentName,
                    sizeof (createProxyLiteMsg->cComponentName));
  DOMX_UTL_Strncpy ((OMX_STRING) createProxyLiteMsg->cComponentRcmServerName,
                    (OMX_STRING) cComponentRcmServerName,
                    sizeof (createProxyLiteMsg->cComponentRcmServerName));
}

/**
 * @name omxrpc_msg_marshall_delete_proxy_lite
 * @brief OmxRpc function to marshall params of OmxRpc createProxyLite msg
 * @param createProxyLiteMsg              : Pointer to msg
 * @param nRpcRemoteHandle                : RemoteRpc handle
 * @param pOmxComponentHandle             : Pointer to OMX component handle
 * @param cComponentName                  : OMX component name
 * @param cComponentRcmServerName         : OMX component rcm server name
 * @return None
 */
static Void
  omxrpc_msg_marshall_delete_proxy_lite (OmxRpc_msgDeleteProxyLite
                                           *deleteProxyLiteMsg,
                                         OMX_IN OmxTypes_OMX_HANDLETYPE
                                           pProxyLiteHandle)
{
  deleteProxyLiteMsg->pProxyLiteHandle = pProxyLiteHandle;
}

/**
 * @name omxrpc_msg_unmarshall_delete_proxy_lite_response
 * @brief OmxRpc function to marshall params of OmxRpc createProxyLite msg
 * @param createProxyLiteMsg              : Pointer to msg
 * @param nRpcRemoteHandle                : RemoteRpc handle
 * @param pOmxComponentHandle             : Pointer to OMX component handle
 * @param cComponentName                  : OMX component name
 * @param cComponentRcmServerName         : OMX component rcm server name
 * @return None
 */

static Void omxrpc_msg_unmarshall_delete_proxy_lite_response ()
{
  /* Dummy function. deleteProxyLite has no API specific response */
}

/**
 * @name omxrpc_msg_unmarshall_create_proxy_lite_response
 * @brief OmxRpc function to unmarshall params of OmxRpc createProxyLite msg response
 * @param createProxyLiteResponseMsg      : Pointer to msg
 * @param ppProxyLiteHandlePtr            : Pointer to Proxylite Handle
 * @return None
 */
static Void
  omxrpc_msg_unmarshall_create_proxy_lite_response
    (OmxRpc_msgCreateProxyLiteResponse *createProxyLiteResponseMsg,
     OMX_OUT OmxTypes_OMX_HANDLETYPE *ppProxyLiteHandlePtr)
{
  *ppProxyLiteHandlePtr = createProxyLiteResponseMsg->pProxyLiteHandle;
}

/**
 * @name omxrpc_msg_marshall_component_tunnel_request
 * @brief OmxRpc function to marshall params of OmxRpc tunnel request msg 
 * @param componentTunnelRequestMsg       : Pointer to msg
 * @param nPort                           : Port index
 * @param hTunneledComp                   : Port index
 * @param nTunneledPort                   : Port index 
 * @param pTunnelSetup                    : Pointer to tunnel setup structure
 * @return None
 */
static Void
  omxrpc_msg_marshall_component_tunnel_request (OmxRpc_msgComponentTunnelRequest
                                                  *componentTunnelRequestMsg,
                                                OmxTypes_OMX_U32 nPort,
                                                OmxTypes_OMX_HANDLETYPE
                                                  hTunneledComp,
                                                OmxTypes_OMX_U32 nTunneledPort,
                                                OmxCore_OMX_TUNNELSETUPTYPE
                                                  *pTunnelSetup)
{
  componentTunnelRequestMsg->nPort = nPort;
  componentTunnelRequestMsg->hTunneledComp = hTunneledComp;
  componentTunnelRequestMsg->nTunneledPort = nTunneledPort;
  if (NULL != pTunnelSetup)
  {
    componentTunnelRequestMsg->sTunnelSetup.nTunnelFlags =
      pTunnelSetup->nTunnelFlags;
    componentTunnelRequestMsg->sTunnelSetup.eSupplier = pTunnelSetup->eSupplier;
    componentTunnelRequestMsg->bValidTunnelSetup = OmxTypes_OMX_TRUE;
  }
  else
  {
    componentTunnelRequestMsg->bValidTunnelSetup = OmxTypes_OMX_FALSE;
  }
}

/**
 * @name omxrpc_msg_unmarshall_component_tunnel_request_response
 * @brief OmxRpc function to unmarshall params of OmxRpc tunnel request msg response
 * @param tunnelRequestResponseMsg        : Pointer to msg
 * @param pTunnelSetup                    : Pointer to tunnel setup structure
 * @return None
 */
static Void
  omxrpc_msg_unmarshall_component_tunnel_request_response
    (OmxRpc_msgComponentTunnelRequestResponse *tunnelRequestResponseMsg,
     OMX_OUT OmxCore_OMX_TUNNELSETUPTYPE *pTunnelSetup)
{
  if (pTunnelSetup != NULL)
  {
    pTunnelSetup->nTunnelFlags =
      tunnelRequestResponseMsg->sTunnelSetup.nTunnelFlags;
    pTunnelSetup->eSupplier = tunnelRequestResponseMsg->sTunnelSetup.eSupplier;
  }
}

/**
 * @name omxrpc_set_get_param_config
 * @brief OmxRpc function to handle Set/Get param and config
 * @param pRpcHndl                        : OmxRpc stub handle
 * @param nParamConfigIndex               : Config/Param index
 * @param nSizeofCompParamStruct          : Size of Param/Config structure
 * @param nCmdStatus                      : Command status
 * @param apiType                         : enum to differentiate get/set APIS
 * @return OmxRpc_errorNone : SUCCESS
 */
static OmxRpc_errorType
  omxrpc_set_get_param_config (OmxRpc_Handle pRpcHndl,
                               OmxIndex_OMX_INDEXTYPE nParamConfigIndex,
                               OmxTypes_OMX_PTR pCompParamConfig,
                               SizeT nSizeofCompParamStruct,
                               OmxCore_OMX_ERRORTYPE *nCmdStatus,
                               OmxRpc_cmd cmdId,
                               OmxRpc_configParamApiType_e apiType)
{
  OmxRpc_errorType retVal = OmxRpc_errorNone;
  RcmClient_Message *rcmMsg;
  OmxRpc_msg *pRpcMsg;
  Bool msgAllocDone = FALSE;

  if (nSizeofCompParamStruct > OmxRpc_maxConfigStructSize)
  {
    return OmxRpc_errorMaxConfigParamStructSizeExceeded;
  }
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((TRUE ==
                                   OmxRpc_module->localCoreRcmServer.initDone),
                                  "localCoreRcmServer init not done");
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((NULL != pRpcHndl), "rpcHandle is NULL");
  DOMX_UTL_TRACE_FUNCTION_ASSERT
    ((TRUE ==
      OmxRpc_module->remoteCoreRcmClient[pRpcHndl->remoteCoreId].initDone),
     "remoteCoreRcmClient init not done");

  /* Initialize the API out params to NULL initially */
  *nCmdStatus = (OmxCore_OMX_ERRORTYPE) OMX_ErrorNone;

  /* allocate a remote command message */
  retVal = OmxRpc_rcmMsgAlloc ((RcmClient_Handle) pRpcHndl->client.handle,
                               &rcmMsg, __FILE__, __LINE__);
  if (OmxRpc_errorNone == retVal)
  {
    UInt32 fxnId;

    msgAllocDone = TRUE;
    pRpcMsg = (OmxRpc_msg *) (&rcmMsg->data);
    /* Packing GetHandle messages */
    /* Filling common part of the message */
    OmxRpc_msgMarshallCommonStub (pRpcMsg,
                                  pRpcHndl,
                                  pRpcHndl->layer.stub.skelHandle, cmdId);
    if (OmxRpc_configParamApiTypeSet == apiType)
    {
      /* Fill the api specific part of the message */
      omxrpc_msg_marshall_setparamconfig (&pRpcMsg->api.setParamConfig,
                                          nParamConfigIndex,
                                          pCompParamConfig,
                                          nSizeofCompParamStruct);
    }
    else
    {
      /* Fill the api specific part of the message */
      omxrpc_msg_marshall_getparamconfig (&pRpcMsg->api.getParamConfig,
                                          nParamConfigIndex,
                                          pCompParamConfig,
                                          nSizeofCompParamStruct);
    }
    fxnId = pRpcHndl->client.fxns.cmd2fxnIdxMap[cmdId];
    retVal = OmxRpc_rcmExec ((RcmClient_Handle) pRpcHndl->client.handle,
                             rcmMsg, fxnId);
    if (OmxRpc_errorNone == retVal)
    {
      OmxRpc_msgUnmarshallCommonStub (&pRpcMsg->common, nCmdStatus);
      if (OmxRpc_configParamApiTypeGet == apiType)
      {
        omxrpc_msg_unmarshall_getparamconfig_response (&pRpcMsg->api.
                                                       getParamConfig,
                                                       pCompParamConfig,
                                                       nSizeofCompParamStruct);
      }
    }
  }                             /* end of if (rcmMsgAlloc success) */
  if (msgAllocDone)
  {
    /* If msg was successfully executed free return the Rcm Msg to heap */
    RcmClient_free ((RcmClient_Handle) pRpcHndl->client.handle, rcmMsg);
  }
  return retVal;
}

/**
 * @name OmxRpc_stubGetHandle
 * @brief OmxRpc stub function for OMX_GetHandle
 * @param pRpcHndl                        : OmxRpc stub handle
 * @param pCallBacks                      : OmxRpc callbacks
 * @param pCbCtx                          : Callback context
 * @param nCmdStatus                      : Command status
 * @param ppOmxComponentHandlePtr         : Pointer to OMX Handle to be set
 * @param pRpcSkelHandle                  : Pointer to OmxRpc skel handle
 * @param pComponentRcmServerName         : Component Rcm server name
 * @param nComponentRcmServerNameMaxLen   : Max length of array holding rcmServer name
 * @return OmxRpc_errorNone : SUCCESS
 */
OmxRpc_errorType OmxRpc_stubGetHandle (OmxRpc_Handle pRpcHndl,
                                       OmxRpc_callBack *pCallBacks,
                                       OmxTypes_OMX_PTR pCbCtx,
                                       OmxCore_OMX_ERRORTYPE *nCmdStatus,
                                       OmxTypes_OMX_HANDLETYPE
                                         *ppOmxComponentHandlePtr,
                                       OmxTypes_OMX_U32 *pRpcSkelHandle,
                                       OmxTypes_OMX_S8 *pComponentRcmServerName,
                                       OmxTypes_OMX_U32
                                         nComponentRcmServerNameMaxLen)
{
  OmxRpc_errorType retVal = OmxRpc_errorNone;
  RcmClient_Message *rcmMsg;
  OmxRpc_msg *pRpcMsg;
  Int rsrcMask = OMXRPC_RSRC_GETHANDLE_NONE;

  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((TRUE ==
                                   OmxRpc_module->localCoreRcmServer.initDone),
                                  "localCoreRcmServer init not done");
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((NULL != pRpcHndl), "pRpcHndl is NULL");
  DOMX_UTL_TRACE_FUNCTION_ASSERT 
    ((TRUE ==
      OmxRpc_module->remoteCoreRcmClient[pRpcHndl->remoteCoreId].initDone),
     "remoteCoreRcmClient init not done");

  /* Initialize the API out params to NULL initially */
  *ppOmxComponentHandlePtr = NULL;
  *nCmdStatus = (OmxCore_OMX_ERRORTYPE) OMX_ErrorNone;
  *pRpcSkelHandle = 0; 
  pComponentRcmServerName[0] = 0;
  /* Store the Callback and CallbackCtx in instance object */
  pRpcHndl->layer.stub.rpcClientCb = *pCallBacks;
  pRpcHndl->layer.stub.pRpcClientCbCtx = pCbCtx;

  /* allocate a remote command message */
  retVal =
    OmxRpc_rcmMsgAlloc (OmxRpc_module->
                        remoteCoreRcmClient[pRpcHndl->remoteCoreId].handle,
                        &rcmMsg, __FILE__, __LINE__);
  if (OmxRpc_errorNone == retVal)
  {
    OmxRpc_cmd cmdId = OmxRpc_cmdGetHandle;
    UInt32 fxnId =
      OmxRpc_module->remoteCoreRcmClient[pRpcHndl->remoteCoreId].fxns.
      cmd2fxnIdxMap[cmdId];

    rsrcMask |= OMXRPC_RSRC_GETHANDLE_MSGALLOC;
    pRpcMsg = (OmxRpc_msg *) (&rcmMsg->data);
    /* Packing GetHandle messages */
    /* Filling common part of the message */
    OmxRpc_msgMarshallCommonStub (pRpcMsg, pRpcHndl, NULL, cmdId);
    /* Fill the api specific part of the message */
    omxrpc_msg_marshall_gethandle (&pRpcMsg->api.getHandle,
                                   (String) pRpcHndl->szComponentName,
                                   (String) pRpcHndl->server.name,
                                   pRpcHndl->layer.stub.skelRcmServerPriority,
                                   DomxCore_localCoreId, pRpcHndl);

    Log_print2 (Diags_USER1, "Calling OmxRpc_rcmExec: pRpcHndl->remoteCoreId %d pRpcHndl->fxnId %d ",
                              pRpcHndl->remoteCoreId,
                              fxnId);

    retVal =
      OmxRpc_rcmExec (OmxRpc_module->
                      remoteCoreRcmClient[pRpcHndl->remoteCoreId].handle,
                      rcmMsg, fxnId);

    if (OmxRpc_errorNone == retVal)
    {
      OmxRpc_msgUnmarshallCommonStub (&pRpcMsg->common, nCmdStatus);
      omxrpc_msg_unmarshall_gethandle_response (&pRpcMsg->api.
                                                getHandleResponse,
                                                &pRpcHndl->layer.stub.
                                                skelHandle,
                                                ppOmxComponentHandlePtr,
                                                (OmxTypes_OMX_S8 *)
                                                pRpcHndl->client.name,
                                                sizeof (pRpcHndl->client.name));
      if (OMX_ErrorNone == *nCmdStatus)
      {
        /* if the remote component was successfully created do the following: * 
           --> Create RcmClient to communicate with Component's Rcm Server -->
           * Get the fxnIdx corresponding to the componentAPIs */
        /* Create the Rcm Client associated with the component specific *
           RcmServer */
        retVal =
          OmxRpc_rcmClientCreate ((RcmClient_Handle *) (&pRpcHndl->client.
                                                        handle),
                                  (String) pRpcHndl->client.name,
                                  OmxRpc_module->
                                  remoteCoreRcmClient[pRpcHndl->
                                                      remoteCoreId].msgHeap.
                                  msgHeapId);
        /* Debug */
        if (pRpcHndl->client.fxns.numEntries == 0xFFFF)
        {
          pRpcHndl->client.fxns.numEntries = 0;
        }

        DOMX_UTL_Strncpy ((OMX_STRING) pRpcHndl->clientEtb.name,
                          (OMX_STRING) pRpcHndl->client.name,
                          sizeof (pRpcHndl->client.name));

        retVal =
          OmxRpc_rcmClientCreate ((RcmClient_Handle *) (&pRpcHndl->
                                                        clientEtb.
                                                        handle),
                                  (String) pRpcHndl->client.name,
                                  OmxRpc_module->
                                  remoteCoreRcmClient[pRpcHndl->
                                                      remoteCoreId].msgHeap.
                                  msgHeapId);

        retVal =
          OmxRpc_rcmClientCreate ((RcmClient_Handle *) (&pRpcHndl->
                                                        clientFtb.
                                                        handle),
                                  (String) pRpcHndl->client.name,
                                  OmxRpc_module->
                                  remoteCoreRcmClient[pRpcHndl->
                                                      remoteCoreId].msgHeap.
                                  msgHeapId);

        /* Debug */
        if (OmxRpc_errorNone == retVal)
        {
          rsrcMask |= OMXRPC_RSRC_GETHANDLE_COMPONENT_RCM_CLIENT_CREATE;
          /* Locate the fxnId to the component specifc RcmServer registered *
             functions */
          retVal = OmxRpc_rcmClientRemoteFxnLocate ((RcmClient_Handle)
                                                    pRpcHndl->client.handle,
                                                    OmxRpc_remoteFxnCtrl,
                                                    &pRpcHndl->client.fxns);
        }                       /* end of if (rcmClientCreate success) */
      }                         /* end of if (OMX_ErrorNone == *
                                   pRpcMsg->common.nOMXReturn) */
    }                           /* end of if (Rcm Exec of GetHandle * executed
                                   successfully ) */
  }                             /* end of (OmxRpc_errorNone * == retVal) - rcm 
                                   msg alloc success */
  if (OmxRpc_errorNone != retVal)
  {
    omxrpc_gethandle_freersrc (rsrcMask,
                               OmxRpc_module->remoteCoreRcmClient[pRpcHndl->
                                                                  remoteCoreId].
                               handle, rcmMsg,
                               (RcmClient_Handle *) (&pRpcHndl->client.
                                                     handle),
                               (RcmClient_Handle *) (&pRpcHndl->clientEtb.
                                                     handle),
                               (RcmClient_Handle *) (&pRpcHndl->clientFtb.
                                                     handle));
  }
  else
  {
    /* If msg was successfully executed free return the Rcm Msg to heap */
    DOMX_UTL_TRACE_FUNCTION_MSG ("Before RcmClient_free");
    RcmClient_free (OmxRpc_module->remoteCoreRcmClient[pRpcHndl->remoteCoreId].
                    handle, rcmMsg);
    DOMX_UTL_TRACE_FUNCTION_MSG ("After RcmClient_free");
    *pRpcSkelHandle = ((OmxTypes_OMX_U32) pRpcHndl->layer.stub.skelHandle);
    DOMX_UTL_Strncpy ((OMX_STRING) pComponentRcmServerName,
                      (OMX_STRING) pRpcHndl->client.name,
                      nComponentRcmServerNameMaxLen);
  }

  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (retVal);

  return retVal;
}

/**
 * @name OmxRpc_stubLiteGetHandle
 * @brief OmxRpc stub function for OMX_GetHandle for ProxyLite
 * @param pRpcHndl                        : OmxRpc stub handle
 * @param pCallBacks                      : OmxRpc callbacks
 * @param pCbCtx                          : Callback context
 * @param nCmdStatus                      : Command status
 * @param pRpcSkelHandle                  : Pointer to OmxRpc skel handle
 * @param pComponentRcmServerName         : Component Rcm server name
 * @return OmxRpc_errorNone : SUCCESS
 */
OmxRpc_errorType OmxRpc_stubLiteGetHandle (OmxRpc_Handle pRpcHndl,
                                           OmxRpc_callBack *pCallBacks,
                                           OmxTypes_OMX_PTR pCbCtx,
                                           OmxCore_OMX_ERRORTYPE *nCmdStatus,
                                           OmxTypes_OMX_U32 nRpcSkelHandle,
                                           OmxTypes_OMX_S8 *szCompRcmServerName)
{
  OmxRpc_errorType retVal = OmxRpc_errorNone;
  Int rsrcMask = OMXRPC_RSRC_GETHANDLELITE_NONE;

  Log_print1 (Diags_ENTRY, "Entered function:%s", (IArg) __FUNCTION__);
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((TRUE ==
                                   OmxRpc_module->localCoreRcmServer.initDone),
                                  "localCoreRcmServer init not done");
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((NULL != pRpcHndl), "rpcHandle is NULL");
  DOMX_UTL_TRACE_FUNCTION_ASSERT
    ((TRUE ==
      OmxRpc_module->remoteCoreRcmClient[pRpcHndl->remoteCoreId].initDone),
     "remoteCoreRcmClient init not done");

  /* Initialize the API out params to NULL initially */
  *nCmdStatus = (OmxCore_OMX_ERRORTYPE) OMX_ErrorNone;

  /* Store the Callback and CallbackCtx in instance object */
  pRpcHndl->layer.stub.rpcClientCb = *pCallBacks;
  pRpcHndl->layer.stub.pRpcClientCbCtx = pCbCtx;
  pRpcHndl->layer.stub.skelHandle = (OmxRpc_Handle) nRpcSkelHandle;
  DOMX_UTL_Strncpy ((OMX_STRING) pRpcHndl->client.name,
                    (OMX_STRING) szCompRcmServerName,
                    sizeof (pRpcHndl->client.name));

  retVal =
    OmxRpc_rcmClientCreate ((RcmClient_Handle *) & pRpcHndl->client.handle,
                            (String) pRpcHndl->client.name,
                            OmxRpc_module->remoteCoreRcmClient[pRpcHndl->
                                                               remoteCoreId].
                            msgHeap.msgHeapId);
  pRpcHndl->client.fxns.numEntries = 0;
  if (OmxRpc_errorNone == retVal)
  {
    rsrcMask |= OMXRPC_RSRC_GETHANDLELITE_COMPONENT_RCM_CLIENT_CREATE;
    /* Locate the fxnId to the component specifc RcmServer registered functions 
     */
    retVal =
      OmxRpc_rcmClientRemoteFxnLocate ((RcmClient_Handle) pRpcHndl->client.
                                       handle, OmxRpc_remoteFxnCtrl,
                                       &pRpcHndl->client.fxns);
  }                             /* end of if (rcmClientCreate success) */
  if (OmxRpc_errorNone != retVal)
  {
    omxrpc_gethandlelite_freersrc (rsrcMask,
                                   (RcmClient_Handle *) (&pRpcHndl->client.
                                                         handle));
  }
  return retVal;
}

/**
 * @name OmxRpc_stubLiteFreeHandle
 * @brief OmxRpc stub function for OMX_FreeHandle for ProxyLite
 * @param pRpcHndl                        : OmxRpc stub handle
 * @param nCmdStatus                      : Command status
 * @return OmxRpc_errorNone : SUCCESS
 */
OmxRpc_errorType OmxRpc_stubLiteFreeHandle (OmxRpc_Handle pRpcHndl,
                                            OmxCore_OMX_ERRORTYPE *nCmdStatus)
{
  omxrpc_gethandlelite_freersrc (OMXRPC_RSRC_GETHANDLELITE_ALL,
                                 (RcmClient_Handle *) (&pRpcHndl->client.
                                                       handle));

  return OmxRpc_errorNone;
}

/**
 * @name OmxRpc_stubSetParameter
 * @brief OmxRpc stub function for OMX_SetParameter
 * @param pRpcHndl                        : OmxRpc stub handle
 * @param nParamIndex                     : Param/Config index
 * @param pCompParam                      : Param/Config structure 
 * @param nSizeofCompParamStruct          : Size of param/config structrue
 * @param nCmdStatus                      : Command status
 * @return OmxRpc_errorNone : SUCCESS
 */
OmxRpc_errorType OmxRpc_stubSetParameter (OmxRpc_Handle pRpcHndl,
                                          OmxIndex_OMX_INDEXTYPE nParamIndex,
                                          OmxTypes_OMX_PTR pCompParam,
                                          SizeT nSizeofCompParamStruct,
                                          OmxCore_OMX_ERRORTYPE *nCmdStatus)
{
  OmxRpc_errorType retVal;

  Log_print1 (Diags_ENTRY, "Entered function:%s", (IArg) __FUNCTION__);
  retVal = omxrpc_set_get_param_config (pRpcHndl,
                                        nParamIndex,
                                        pCompParam,
                                        nSizeofCompParamStruct,
                                        nCmdStatus,
                                        OmxRpc_cmdSetParameter,
                                        OmxRpc_configParamApiTypeSet);
  return retVal;
}

/**
 * @name OmxRpc_stubSetConfig
 * @brief OmxRpc stub function for OMX_SetConfig
 * @param pRpcHndl                        : OmxRpc stub handle
 * @param nParamIndex                     : Param/Config index
 * @param pCompParam                      : Param/Config structure 
 * @param nSizeofCompParamStruct          : Size of param/config structrue
 * @param nCmdStatus                      : Command status
 * @return OmxRpc_errorNone : SUCCESS
 */
OmxRpc_errorType OmxRpc_stubSetConfig (OmxRpc_Handle pRpcHndl,
                                       OmxIndex_OMX_INDEXTYPE nConfigIndex,
                                       OmxTypes_OMX_PTR pCompConfig,
                                       SizeT nSizeofCompParamStruct,
                                       OmxCore_OMX_ERRORTYPE *nCmdStatus)
{
  OmxRpc_errorType retVal;

  Log_print1 (Diags_ENTRY, "Entered function:%s", (IArg) __FUNCTION__);
  retVal = omxrpc_set_get_param_config (pRpcHndl,
                                        nConfigIndex,
                                        pCompConfig,
                                        nSizeofCompParamStruct,
                                        nCmdStatus,
                                        OmxRpc_cmdSetConfig,
                                        OmxRpc_configParamApiTypeSet);
  return retVal;
}

/**
 * @name OmxRpc_stubGetParameter
 * @brief OmxRpc stub function for OMX_SetConfig
 * @param pRpcHndl                        : OmxRpc stub handle
 * @param nParamIndex                     : Param/Config index
 * @param pCompParam                      : Param/Config structure 
 * @param nSizeofCompParamStruct          : Size of param/config structrue
 * @param nCmdStatus                      : Command status
 * @return OmxRpc_errorNone : SUCCESS
 */
OmxRpc_errorType OmxRpc_stubGetParameter (OmxRpc_Handle pRpcHndl,
                                          OmxIndex_OMX_INDEXTYPE nParamIndex,
                                          OmxTypes_OMX_PTR pCompParam,
                                          SizeT nSizeofCompParamStruct,
                                          OmxCore_OMX_ERRORTYPE *nCmdStatus)
{
  OmxRpc_errorType retVal;

  Log_print1 (Diags_ENTRY, "Entered function:%s", (IArg) __FUNCTION__);
  retVal = omxrpc_set_get_param_config (pRpcHndl,
                                        nParamIndex,
                                        pCompParam,
                                        nSizeofCompParamStruct,
                                        nCmdStatus,
                                        OmxRpc_cmdGetParameter,
                                        OmxRpc_configParamApiTypeGet);
  return retVal;
}

/**
 * @name OmxRpc_stubGetConfig
 * @brief OmxRpc stub function for OMX_GetConfig
 * @param pRpcHndl                        : OmxRpc stub handle
 * @param nParamIndex                     : Param/Config index
 * @param pCompParam                      : Param/Config structure 
 * @param nSizeofCompParamStruct          : Size of param/config structrue
 * @param nCmdStatus                      : Command status
 * @return OmxRpc_errorNone : SUCCESS
 */
OmxRpc_errorType OmxRpc_stubGetConfig (OmxRpc_Handle pRpcHndl,
                                       OmxIndex_OMX_INDEXTYPE nConfigIndex,
                                       OmxTypes_OMX_PTR pCompConfig,
                                       SizeT nSizeofCompParamStruct,
                                       OmxCore_OMX_ERRORTYPE *nCmdStatus)
{
  OmxRpc_errorType retVal;

  Log_print1 (Diags_ENTRY, "Entered function:%s", (IArg) __FUNCTION__);
  retVal = omxrpc_set_get_param_config (pRpcHndl,
                                        nConfigIndex,
                                        pCompConfig,
                                        nSizeofCompParamStruct,
                                        nCmdStatus,
                                        OmxRpc_cmdGetConfig,
                                        OmxRpc_configParamApiTypeGet);
  return retVal;
}

static Int g_KillLoopInvariantWarning = FALSE;

#define OMXRPC_STUB_TEMPLATE(pRpcHndl,pRpcMsg,retVal,cmdId,stubId,             \
                             marshallApiParams,                                \
                     unMarshallApiParams,nCmdStatus)                           \
do {                                                                           \
  RcmClient_Message *rcmMsg;                                                   \
  Bool msgAllocDone = FALSE;                                                   \
                                                                               \
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((TRUE ==                                     \
                                   OmxRpc_module->localCoreRcmServer.initDone),\
                                  "localCoreRcmServer init not done");         \
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((NULL != pRpcHndl), "rpcHandle is NULL");    \
  DOMX_UTL_TRACE_FUNCTION_ASSERT                                               \
    ((TRUE ==                                                                  \
      OmxRpc_module->remoteCoreRcmClient[pRpcHndl->remoteCoreId].initDone),    \
     "remoteCoreRcmClient init not done");                                     \
                                                                               \
  /* allocate a remote command message */                                      \
  retVal = OmxRpc_rcmMsgAlloc ((RcmClient_Handle)pRpcHndl->client.handle,      \
                               &rcmMsg,                                        \
                               __FILE__,                                       \
                               __LINE__);                                      \
  if (OmxRpc_errorNone == retVal)                                              \
  {                                                                            \
    UInt32 fxnId;                                                              \
                                                                               \
    msgAllocDone = TRUE;                                                       \
    pRpcMsg = (OmxRpc_msg * )(&rcmMsg->data);                                  \
    /* Packing GetHandle messages */                                           \
    /* Filling common part of the message */                                   \
    OmxRpc_msgMarshallCommonStub (pRpcMsg,                                     \
                                  pRpcHndl,                                    \
                                  pRpcHndl->layer.stub.skelHandle,             \
                                  cmdId);                                      \
    omxrpc_msg_marshall_##stubId marshallApiParams;                            \
    if ((OmxRpc_cmdCreateProxyLite == cmdId) ||                                \
        (OmxRpc_cmdFreeHandle == cmdId))                                       \
    {                                                                          \
      fxnId =                                                                  \
        OmxRpc_module->remoteCoreRcmClient[pRpcHndl->remoteCoreId].fxns.       \
        cmd2fxnIdxMap[cmdId];                                                  \
    }                                                                          \
    else                                                                       \
    {                                                                          \
      fxnId = pRpcHndl->client.fxns.cmd2fxnIdxMap[cmdId];                      \
    }                                                                          \
    retVal = OmxRpc_rcmExec ((RcmClient_Handle)pRpcHndl->client.handle,        \
                             rcmMsg,                                           \
                             fxnId);                                           \
    if (OmxRpc_errorNone == retVal)                                            \
    {                                                                          \
      OmxRpc_msgUnmarshallCommonStub(&pRpcMsg->common,nCmdStatus);             \
      omxrpc_msg_unmarshall_##stubId##_response unMarshallApiParams;           \
    }                                                                          \
  } /* end of if (rcmMsgAlloc success) */                                      \
  if (msgAllocDone)                                                            \
  {                                                                            \
    /* If msg was successfully executed free return the Rcm Msg to heap */     \
    RcmClient_free ((RcmClient_Handle)pRpcHndl->client.handle,                 \
                    rcmMsg);                                                   \
  }                                                                            \
                                                                               \
} while (g_KillLoopInvariantWarning)

/**
 * @name OmxRpc_stubSendCommand
 * @brief OmxRpc stub function for OMX_SendCommand
 * @param pRpcHndl                        : OmxRpc stub handle
 * @param eCmd                            : SendCommand enum
 * @param nParam                          : Command param
 * @param pCmdData                        : Pointer to cmd data
 * @param nCmdStatus                      : Command status
 * @return OmxRpc_errorNone : SUCCESS
 */
OmxRpc_errorType OmxRpc_stubSendCommand (OmxRpc_Handle pRpcHndl,
                                         OmxCore_OMX_COMMANDTYPE eCmd,
                                         OmxTypes_OMX_U32 nParam,
                                         OmxTypes_OMX_PTR pCmdData,
                                         OmxCore_OMX_ERRORTYPE *nCmdStatus)
{
  OmxRpc_errorType retVal = OmxRpc_errorNone;
  OmxRpc_msg *pRpcMsg;

  /* Initialize the API out params to NULL initially */
  *nCmdStatus = (OmxCore_OMX_ERRORTYPE) OMX_ErrorNone;
  OMXRPC_STUB_TEMPLATE (pRpcHndl, pRpcMsg, retVal, OmxRpc_cmdSendCommand, 
                        sendcommand, (&pRpcMsg->api.sendCommand, eCmd, nParam, 
                        pCmdData), (), /* No unmarshall api for send cmd */
                        nCmdStatus);
  return retVal;
}

/**
 * @name OmxRpc_stubGetState
 * @brief OmxRpc stub function for OMX_GetState
 * @param pRpcHndl                        : OmxRpc stub handle
 * @param pState                          : Pointer to state param to be set
 * @param nCmdStatus                      : Command status
 * @return OmxRpc_errorNone : SUCCESS
 */
OmxRpc_errorType OmxRpc_stubGetState (OmxRpc_Handle pRpcHndl,
                                      OmxCore_OMX_STATETYPE *pState,
                                      OmxCore_OMX_ERRORTYPE *nCmdStatus)
{

  OmxRpc_errorType retVal = OmxRpc_errorNone;
  OmxRpc_msg *pRpcMsg;

  /* Initialize the API out params to NULL initially */
  *nCmdStatus = (OmxCore_OMX_ERRORTYPE) OMX_ErrorNone;
  *pState = OmxCore_OMX_StateInvalid;   /* This is the default return */
  OMXRPC_STUB_TEMPLATE (pRpcHndl, pRpcMsg, retVal, OmxRpc_cmdGetState,
                        getstate, (), /* No marshall api for send cmd */
                        (&pRpcMsg->api.getStateResponse, pState), nCmdStatus);
  return retVal;
}

/**
 * @name OmxRpc_stubGetComponentVersion
 * @brief OmxRpc stub function for OMX_GetComponentVersion
 * @param pRpcHndl                        : OmxRpc stub handle
 * @param pComponentName                  : Pointer to component name array
 * @param pComponentVersion               : Pointer to component version
 * @param pSpecVersion                    : Pointer to sepc version 
 * @param pComponentUUID                  : Pointer to component UUID
 * @param nCmdStatus                      : Command status
 * @return OmxRpc_errorNone : SUCCESS
 */
OmxRpc_errorType 
  OmxRpc_stubGetComponentVersion (OmxRpc_Handle pRpcHndl,
                                  OmxTypes_OMX_STRING pComponentName,
                                  OmxTypes_OMX_VERSIONTYPE *pComponentVersion,
                                  OmxTypes_OMX_VERSIONTYPE *pSpecVersion,
                                  OmxTypes_OMX_UUIDTYPE *pComponentUUID,
                                  OmxCore_OMX_ERRORTYPE *nCmdStatus)
{
  OmxRpc_errorType retVal = OmxRpc_errorNone;
  OmxRpc_msg *pRpcMsg;

  if ((NULL == pComponentName)
      || (NULL == pComponentVersion)
      || (NULL == pSpecVersion)
      || (NULL == pComponentUUID) || (NULL == nCmdStatus))
  {
    return OmxRpc_errorBadArgument;
  }

  /* Initialize the API out params to NULL initially */
  *nCmdStatus = (OmxCore_OMX_ERRORTYPE) OMX_ErrorNone;
  /* TODO Verify if these default values are okay */
  pComponentName[0] = '\0';     /* Init the componentName to Null string */
  pComponentVersion->nVersion = 0;
  pSpecVersion->nVersion = 0;
  pComponentUUID[0][0] = 0;
  OMXRPC_STUB_TEMPLATE (pRpcHndl, pRpcMsg, retVal,
                        OmxRpc_cmdGetComponentVersion, get_component_version,
                        (), /* No marshall api for get_component_version cmd */
                        (&pRpcMsg->api.getComponentVersionResponse,
                         pComponentName,
                         pComponentVersion,
                         pSpecVersion, pComponentUUID), nCmdStatus);
  return retVal;
}

/**
 * @name OmxRpc_stubGetExtensionIndex
 * @brief OmxRpc stub function for OMX_GetExtension index
 * @param pRpcHndl                        : OmxRpc stub handle
 * @param cParameterName                  : Parameter name
 * @param pIndexType                      : Pointer to index type
 * @param nCmdStatus                      : Command status
 * @return OmxRpc_errorNone : SUCCESS
 */
OmxRpc_errorType OmxRpc_stubGetExtensionIndex (OmxRpc_Handle pRpcHndl,
                                               OmxTypes_OMX_STRING
                                                 cParameterName,
                                               OmxIndex_OMX_INDEXTYPE
                                                 *pIndexType,
                                               OmxCore_OMX_ERRORTYPE
                                                 *nCmdStatus)
{
  OmxRpc_errorType retVal = OmxRpc_errorNone;
  OmxRpc_msg *pRpcMsg;

  if ((NULL == pIndexType) || (NULL == nCmdStatus))
  {
    return OmxRpc_errorBadArgument;
  }
  *pIndexType = OmxIndex_OMX_IndexMax;
  OMXRPC_STUB_TEMPLATE (pRpcHndl,
                        pRpcMsg,
                        retVal,
                        OmxRpc_cmdGetExtensionIndex,
                        get_extension_index,
                        (&pRpcMsg->api.getExtensionIndex, cParameterName),
                        (&pRpcMsg->api.getExtensionIndexResponse,
                         pIndexType), nCmdStatus);
  return retVal;
}

/**
 * @name OmxRpc_stubUseBuffer
 * @brief OmxRpc stub function for OMX_UseBuffer
 * @param pRpcHndl                        : OmxRpc stub handle
 * @param pBufferHdr                      : Pointer to OMXBUFHDR
 * @param nPortIndex                      : Port index
 * @param pAppPrivate                     : Pointer to appPrivate
 * @param nSizeBytes                      : Size of buffer in bytes 
 * @param pBuffer                         : Pointer to data buffer
 * @param pBufferMapped                   : Data buffer mapped value
 * @param ppBufHeaderRemote               : Pointer to Remote OMXBUFHDR pointer
 * @param nCmdStatus                      : Command status 
 * @return OmxRpc_errorNone : SUCCESS
 */
OmxRpc_errorType OmxRpc_stubUseBuffer (OmxRpc_Handle pRpcHndl,
                                       OMX_OUT OmxCore_OMX_BUFFERHEADERTYPE
                                         *pBufferHdr,
                                       OMX_IN OmxTypes_OMX_U32 nPortIndex,
                                       OMX_IN OmxTypes_OMX_PTR pAppPrivate,
                                       OMX_IN OmxTypes_OMX_U32 nSizeBytes,
                                       OMX_IN OmxTypes_OMX_U8 *pBuffer,
                                       OMX_OUT OmxTypes_OMX_U32
                                         *pBufferMapped,
                                       OMX_OUT OmxCore_OMX_BUFFERHEADERTYPE
                                         **ppBufHeaderRemote,
                                       OMX_OUT OmxCore_OMX_ERRORTYPE
                                         *nCmdStatus)
{
  OmxRpc_errorType retVal = OmxRpc_errorNone;
  OmxRpc_msg *pRpcMsg;
  UInt32 mappedBufAddress = 0x00;
  IDmm_errorType dmmErrorCode;

  *nCmdStatus = OmxCore_OMX_ErrorNone;

  dmmErrorCode = OmxRpc_PDmm_mapBuffer (pRpcHndl->remoteCoreId,
                                        pBuffer, nSizeBytes, &mappedBufAddress);
  if (IDmm_errorNone != dmmErrorCode)
  {
    return OmxRpc_errorDmmBufferMapFailed;
  }
  else
  {
    *pBufferMapped = mappedBufAddress;
  }

  OMXRPC_STUB_TEMPLATE (pRpcHndl,
                        pRpcMsg,
                        retVal,
                        OmxRpc_cmdUseBuffer,
                        usebuffer,
                        (&pRpcMsg->api.useBuffer, nPortIndex, pAppPrivate,
                         nSizeBytes, mappedBufAddress),
                        (&pRpcMsg->api.useBufferResponse, ppBufHeaderRemote,
                         pBufferHdr, *nCmdStatus), nCmdStatus);
  if (*nCmdStatus != OmxCore_OMX_ErrorNone)
  {
    OmxRpc_PDmm_unmapBuffer (pRpcHndl->remoteCoreId, mappedBufAddress);
  }
  return retVal;
}

/**
 * @name OmxRpc_stubAllocBuffer
 * @brief OmxRpc stub function for OMX_AllocBuffer
 * @param pRpcHndl                        : OmxRpc stub handle
 * @param pBufferHdr                      : Pointer to OMXBUFHDR
 * @param nPortIndex                      : Port index
 * @param pAppPrivate                     : Pointer to appPrivate
 * @param nSizeBytes                      : Size of buffer in bytes 
 * @param pBuffer                         : Pointer to Pointer to data buffer
 * @param pBufferMapped                   : Data buffer mapped value
 * @param ppBufHeaderRemote               : Pointer to Remote OMXBUFHDR pointer
 * @param nCmdStatus                      : Command status 
 * @return OmxRpc_errorNone : SUCCESS
 */
OmxRpc_errorType 
  OmxRpc_stubAllocBuffer (OmxRpc_Handle pRpcHndl,
                          OMX_OUT OmxCore_OMX_BUFFERHEADERTYPE *pBufferHdr,
                          OMX_IN OmxTypes_OMX_U32 nPortIndex,
                          OMX_IN OmxTypes_OMX_PTR pAppPrivate,
                          OMX_IN OmxTypes_OMX_U32 nSizeBytes,
                          OMX_OUT OmxTypes_OMX_U8 **pBuffer,
                          OMX_OUT OmxTypes_OMX_U32 *pBufferMapped,
                          OMX_OUT OmxCore_OMX_BUFFERHEADERTYPE 
                            **ppBufHeaderRemote,
                          OMX_OUT OmxCore_OMX_ERRORTYPE *nCmdStatus)
{
  OmxRpc_errorType retVal = OmxRpc_errorNone;
  OmxRpc_msg *pRpcMsg;
  UInt32 nBufferMapped = 0x0;
  IDmm_errorType dmmErrorCode;

  *nCmdStatus = OmxCore_OMX_ErrorNone;

  OMXRPC_STUB_TEMPLATE (pRpcHndl,
                        pRpcMsg,
                        retVal,
                        OmxRpc_cmdAllocBuffer,
                        allocbuffer,
                        (&pRpcMsg->api.allocBuffer, nPortIndex, pAppPrivate,
                         nSizeBytes),
                        (&pRpcMsg->api.allocBufferResponse, ppBufHeaderRemote,
                         pBufferHdr, &nBufferMapped, *nCmdStatus), nCmdStatus);
  if (*nCmdStatus == OmxCore_OMX_ErrorNone)
  {
    dmmErrorCode = OmxRpc_PDmm_xltMapBuf2Ptr (nBufferMapped, pBuffer);
    if (dmmErrorCode != IDmm_errorNone)
    {
      retVal = OmxRpc_errorBadArgument;
    }
    else
    {
      *pBufferMapped = nBufferMapped;
    }
  }

  return retVal;
}

/**
 * @name OmxRpc_stubFreeBuffer
 * @brief OmxRpc stub function for OMX_FreeBuffer
 * @param pRpcHndl                        : OmxRpc stub handle
 * @param nPortIndex                      : Port index
 * @param ppBufHeaderRemote               : Pointer to Remote OMXBUFHDR
 * @param nCmdStatus                      : Command status 
 * @return OmxRpc_errorNone : SUCCESS
 */
OmxRpc_errorType OmxRpc_stubFreeBuffer (OmxRpc_Handle pRpcHndl,
                                        OMX_IN OmxTypes_OMX_U32 nPortIndex,
                                        OMX_IN OmxCore_OMX_BUFFERHEADERTYPE
                                          *pBufHeaderRemote,
                                        OMX_OUT OmxCore_OMX_ERRORTYPE 
                                          *nCmdStatus)
{
  OmxRpc_errorType retVal = OmxRpc_errorNone;
  OmxRpc_msg *pRpcMsg;

  OMXRPC_STUB_TEMPLATE (pRpcHndl, pRpcMsg, retVal, OmxRpc_cmdFreeBuffer, 
                        freebuffer, (&pRpcMsg->api.freeBuffer, nPortIndex,
                                     pBufHeaderRemote),
                        (), /* No freeBufferResponse unmarshall api */
                        nCmdStatus);
  return retVal;
}

/**
 * @name OmxRpc_stubETB
 * @brief OmxRpc stub function for OMX_EmptyThisBuffer
 * @param pRpcHndl                        : OmxRpc stub handle
 * @param pBufferHdr                      : Pointer to Local OMXBUFHDR
 * @param ppBufHeaderRemote               : Pointer to Remote OMXBUFHDR
 * @param cmdId                           : OmxRpc commandID
 * @param nCmdStatus                      : Command status 
 * @return OmxRpc_errorNone : SUCCESS
 */
OmxRpc_errorType OmxRpc_stubETB (OmxRpc_Handle pRpcHndl,
                                 OMX_IN OmxCore_OMX_BUFFERHEADERTYPE
                                   *pBufferHdr,
                                 OMX_IN OmxCore_OMX_BUFFERHEADERTYPE
                                   *pBufferHdrRemote, OMX_IN OmxRpc_cmd cmdId,
                                 OMX_OUT OmxCore_OMX_ERRORTYPE *nCmdStatus)
{
  OmxRpc_errorType retVal = OmxRpc_errorNone;
  OmxRpc_msg *pRpcMsg;

  RcmClient_Message *rcmMsg;
  Bool msgAllocDone = FALSE;

  DOMX_UTL_TRACE_FUNCTION_ASSERT ((TRUE ==
                                   OmxRpc_module->localCoreRcmServer.initDone),
                                  "localCoreRcmServer init not done");
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((NULL != pRpcHndl), "rpcHandle is NULL");
  DOMX_UTL_TRACE_FUNCTION_ASSERT
    ((TRUE ==
      OmxRpc_module->remoteCoreRcmClient[pRpcHndl->remoteCoreId].initDone),
     "remoteCoreRcmClient init not done");

  /* allocate a remote command message */
  retVal = OmxRpc_rcmMsgAlloc ((RcmClient_Handle) pRpcHndl->clientEtb.handle,
                               &rcmMsg, __FILE__, __LINE__);
  if (OmxRpc_errorNone == retVal)
  {
    UInt32 fxnId;

    msgAllocDone = TRUE;
    pRpcMsg = (OmxRpc_msg *) (&rcmMsg->data);
    /* Packing GetHandle messages */
    /* Filling common part of the message */
    OmxRpc_msgMarshallCommonStub (pRpcMsg,
                                  pRpcHndl,
                                  pRpcHndl->layer.stub.skelHandle, cmdId);
    omxrpc_msg_marshall_etb_and_ftb (&pRpcMsg->api.emptyFillBuffer,
                                     pBufferHdrRemote, pBufferHdr);
    if ((OmxRpc_cmdCreateProxyLite == cmdId) || (OmxRpc_cmdFreeHandle == cmdId))
    {
      fxnId =
        OmxRpc_module->remoteCoreRcmClient[pRpcHndl->remoteCoreId].fxns.
        cmd2fxnIdxMap[cmdId];
    }
    else
    {
      fxnId = pRpcHndl->client.fxns.cmd2fxnIdxMap[cmdId];
    }
    retVal = OmxRpc_rcmExec ((RcmClient_Handle) pRpcHndl->clientEtb.handle,
                             rcmMsg, fxnId);
    if (OmxRpc_errorNone == retVal)
    {
      OmxRpc_msgUnmarshallCommonStub (&pRpcMsg->common, nCmdStatus);
      omxrpc_msg_unmarshall_etb_and_ftb_response ();
    }
  }                             /* end of if (rcmMsgAlloc success) */
  if (msgAllocDone)
  {
    /* If msg was successfully executed free return the Rcm Msg to heap */
    RcmClient_free ((RcmClient_Handle) pRpcHndl->clientEtb.handle, rcmMsg);
  }

  return retVal;
}

/**
 * @name OmxRpc_stubFTB
 * @brief OmxRpc stub function for OMX_FillThisBuffer
 * @param pRpcHndl                        : OmxRpc stub handle
 * @param pBufferHdr                      : Pointer to Local OMXBUFHDR
 * @param ppBufHeaderRemote               : Pointer to Remote OMXBUFHDR
 * @param cmdId                           : OmxRpc commandID
 * @param nCmdStatus                      : Command status 
 * @return OmxRpc_errorNone : SUCCESS
 */
OmxRpc_errorType OmxRpc_stubFTB (OmxRpc_Handle pRpcHndl,
                                 OMX_IN OmxCore_OMX_BUFFERHEADERTYPE
                                   *pBufferHdr,
                                 OMX_IN OmxCore_OMX_BUFFERHEADERTYPE
                                   *pBufferHdrRemote, OMX_IN OmxRpc_cmd cmdId,
                                 OMX_OUT OmxCore_OMX_ERRORTYPE *nCmdStatus)
{
  OmxRpc_errorType retVal = OmxRpc_errorNone;
  OmxRpc_msg *pRpcMsg;

  RcmClient_Message *rcmMsg;
  Bool msgAllocDone = FALSE;

  DOMX_UTL_TRACE_FUNCTION_ASSERT ((TRUE ==
                                   OmxRpc_module->localCoreRcmServer.initDone),
                                  "localCoreRcmServer init not done");
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((NULL != pRpcHndl), "rpcHandle is NULL");
  DOMX_UTL_TRACE_FUNCTION_ASSERT
    ((TRUE ==
      OmxRpc_module->remoteCoreRcmClient[pRpcHndl->remoteCoreId].initDone),
     "remoteCoreRcmClient init not done");

  /* allocate a remote command message */
  retVal = OmxRpc_rcmMsgAlloc ((RcmClient_Handle) pRpcHndl->clientFtb.handle,
                               &rcmMsg, __FILE__, __LINE__);
  if (OmxRpc_errorNone == retVal)
  {
    UInt32 fxnId;

    msgAllocDone = TRUE;
    pRpcMsg = (OmxRpc_msg *) (&rcmMsg->data);
    /* Packing GetHandle messages */
    /* Filling common part of the message */
    OmxRpc_msgMarshallCommonStub (pRpcMsg,
                                  pRpcHndl,
                                  pRpcHndl->layer.stub.skelHandle, cmdId);
    omxrpc_msg_marshall_etb_and_ftb (&pRpcMsg->api.emptyFillBuffer,
                                     pBufferHdrRemote, pBufferHdr);
    if ((OmxRpc_cmdCreateProxyLite == cmdId) || (OmxRpc_cmdFreeHandle == cmdId))
    {
      fxnId =
        OmxRpc_module->remoteCoreRcmClient[pRpcHndl->remoteCoreId].fxns.
        cmd2fxnIdxMap[cmdId];
    }
    else
    {
      fxnId = pRpcHndl->client.fxns.cmd2fxnIdxMap[cmdId];
    }
    retVal = OmxRpc_rcmExec ((RcmClient_Handle) pRpcHndl->clientFtb.handle,
                             rcmMsg, fxnId);
    if (OmxRpc_errorNone == retVal)
    {
      OmxRpc_msgUnmarshallCommonStub (&pRpcMsg->common, nCmdStatus);
      omxrpc_msg_unmarshall_etb_and_ftb_response ();
    }
  }                             /* end of if (rcmMsgAlloc success) */
  if (msgAllocDone)
  {
    /* If msg was successfully executed free return the Rcm Msg to heap */
    RcmClient_free ((RcmClient_Handle) pRpcHndl->clientFtb.handle, rcmMsg);
  }

  return retVal;
}

/**
 * @name OmxRpc_stubETBAndFTB
 * @brief OmxRpc stub function for both OMX_FillThisBuffer and OMX_EmptyThisBuffer
 * @param pRpcHndl                        : OmxRpc stub handle
 * @param pBufferHdr                      : Pointer to Local OMXBUFHDR
 * @param ppBufHeaderRemote               : Pointer to Remote OMXBUFHDR
 * @param cmdId                           : OmxRpc commandID to differentiate ETB/FTB
 * @param nCmdStatus                      : Command status 
 * @return OmxRpc_errorNone : SUCCESS
 */
OmxRpc_errorType OmxRpc_stubETBAndFTB (OmxRpc_Handle pRpcHndl,
                                       OMX_IN OmxCore_OMX_BUFFERHEADERTYPE
                                         *pBufferHdr,
                                       OMX_IN OmxCore_OMX_BUFFERHEADERTYPE
                                         *pBufferHdrRemote,
                                       OMX_IN OmxRpc_cmd cmdId,
                                       OMX_OUT OmxCore_OMX_ERRORTYPE 
                                         *nCmdStatus)
{
  OmxRpc_errorType retVal = OmxRpc_errorNone;
  OmxRpc_msg *pRpcMsg;

  OMXRPC_STUB_TEMPLATE (pRpcHndl, pRpcMsg, retVal, cmdId, etb_and_ftb,
                        (&pRpcMsg->api.emptyFillBuffer, pBufferHdrRemote,
                         pBufferHdr), (), /* No etbFtbResponse unmarshall api */
                        nCmdStatus);
  return retVal;
}

/**
 * @name OmxRpc_stubFreeHandle
 * @brief OmxRpc stub function for OMX_FreeHandle
 * @param pRpcHndl                        : OmxRpc stub handle
 * @param nCmdStatus                      : Command status 
 * @return OmxRpc_errorNone : SUCCESS
 */
OmxRpc_errorType OmxRpc_stubFreeHandle (OmxRpc_Handle pRpcHndl,
                                        OMX_OUT OmxCore_OMX_ERRORTYPE
                                          *nCmdStatus)
{
  OmxRpc_errorType retVal = OmxRpc_errorNone;
  OmxRpc_msg *pRpcMsg;
  RcmClient_Message *rcmMsg;
  Bool msgAllocDone = FALSE;

  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((TRUE ==
                                   OmxRpc_module->localCoreRcmServer.initDone),
                                  "localCoreRcmServer init not done");
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((NULL != pRpcHndl), "rpcHandle is NULL");
  DOMX_UTL_TRACE_FUNCTION_ASSERT
    ((TRUE ==
      OmxRpc_module->remoteCoreRcmClient[pRpcHndl->remoteCoreId].initDone),
     "remoteCoreRcmClient init not done");

  /* allocate a remote command message */
  retVal =
    OmxRpc_rcmMsgAlloc (OmxRpc_module->
                        remoteCoreRcmClient[pRpcHndl->remoteCoreId].handle,
                        &rcmMsg, __FILE__, __LINE__);

  if (OmxRpc_errorNone == retVal)
  {
    OmxRpc_cmd cmdId = OmxRpc_cmdFreeHandle;
    UInt32 fxnId =
      OmxRpc_module->remoteCoreRcmClient[pRpcHndl->remoteCoreId].fxns.
      cmd2fxnIdxMap[cmdId];

    msgAllocDone = TRUE;
    pRpcMsg = (OmxRpc_msg *) (&rcmMsg->data);
    /* Packing GetHandle messages */
    /* Filling common part of the message */
    OmxRpc_msgMarshallCommonStub (pRpcMsg,
                                  pRpcHndl,
                                  pRpcHndl->layer.stub.skelHandle, cmdId);
    omxrpc_msg_marshall_freehandle ();

    retVal = OmxRpc_rcmExec ((RcmClient_Handle) OmxRpc_module->
                             remoteCoreRcmClient[pRpcHndl->remoteCoreId].handle,
                             rcmMsg, fxnId);
    if (OmxRpc_errorNone == retVal)
    {
      OmxRpc_msgUnmarshallCommonStub (&pRpcMsg->common, nCmdStatus);
      omxrpc_msg_unmarshall_freehandle_response ();
    }
  }                             /* end of if (rcmMsgAlloc success) */
  if (msgAllocDone)
  {
    /* If msg was successfully executed free return the Rcm Msg to heap */
    RcmClient_free ((RcmClient_Handle) OmxRpc_module->
                    remoteCoreRcmClient[pRpcHndl->remoteCoreId].handle, rcmMsg);
  }
  if (OmxCore_OMX_ErrorNone == *nCmdStatus)
  {
    omxrpc_gethandle_freersrc ((OMXRPC_RSRC_GETHANDLE_COMPONENT_RCM_CLIENT_CREATE | OMXRPC_RSRC_GETHANDLE_COMPONENT_RCM_ETB_CLIENT_CREATE | OMXRPC_RSRC_GETHANDLE_COMPONENT_RCM_FTB_CLIENT_CREATE), NULL, NULL, (RcmClient_Handle *) & pRpcHndl->client.handle, (RcmClient_Handle *) & pRpcHndl->clientEtb.handle, (RcmClient_Handle *) & pRpcHndl->clientFtb.handle);
  }

  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (retVal);
  return retVal;
}

/**
 * @name OmxRpc_stubCreateProxyLite
 * @brief OmxRpc stub function to create a OMX proxy lite instance
 * @param pRpcHndl                        : OmxRpc stub handle
 * @param nRpcRemoteHandle                : Remote RPc skel handle value
 * @param pOmxComponentHandle             : Pointer to OMX component handle
 * @param cComponentName                  : OMX component name
 * @param cComponentRcmServerName         : OMX component rcmServer name
 * @param ppProxyLiteHandlePtr            : Pointer to ProxyLite Handle to be populated
 * @param nCmdStatus                      : Command status  
 * @return OmxRpc_errorNone : SUCCESS
 */
OmxRpc_errorType
  OmxRpc_stubCreateProxyLite (OmxRpc_Handle pRpcHndl,
                              OmxTypes_OMX_U32 nRpcRemoteHandle,
                              OmxTypes_OMX_HANDLETYPE pOmxComponentHandle,
                              OmxTypes_OMX_S8 *cComponentName,
                              OmxTypes_OMX_S8 *cComponentRcmServerName,
                              OmxTypes_OMX_HANDLETYPE *ppProxyLiteHandlePtr,
                              OmxCore_OMX_ERRORTYPE * nCmdStatus)
{
  OmxRpc_errorType retVal = OmxRpc_errorNone;
  OmxRpc_msg *pRpcMsg;

  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();

  OMXRPC_STUB_TEMPLATE (pRpcHndl,
                        pRpcMsg,
                        retVal,
                        OmxRpc_cmdCreateProxyLite,
                        create_proxy_lite,
                        (&pRpcMsg->api.createProxyLite, nRpcRemoteHandle,
                         pOmxComponentHandle,
                         cComponentName,
                         cComponentRcmServerName),
                        (&pRpcMsg->api.createProxyLiteResponse,
                         ppProxyLiteHandlePtr), nCmdStatus);
  return retVal;
}

/**
 * @name OmxRpc_stubComponentTunnelRequest
 * @brief OmxRpc stub function for OMX_COmponentTunnelRequest
 * @param pRpcHndl                        : OmxRpc stub handle
 * @param nPort                           : Port index
 * @param hTunneledComp                   : Handle to tunneled component
 * @param nTunneledPort                   : Tunneled port index
 * @param pTunnelSetup                    : Pointer to tunnel setup structure
 * @param nCmdStatus                      : Command status  
 * @return OmxRpc_errorNone : SUCCESS
 */
OmxRpc_errorType
  OmxRpc_stubComponentTunnelRequest (OmxRpc_Handle pRpcHndl,
                                     OmxTypes_OMX_U32 nPort,
                                     OmxTypes_OMX_HANDLETYPE hTunneledComp,
                                     OmxTypes_OMX_U32 nTunneledPort,
                                     OmxCore_OMX_TUNNELSETUPTYPE *pTunnelSetup,
                                     OmxCore_OMX_ERRORTYPE *nCmdStatus)
{
  OmxRpc_errorType retVal = OmxRpc_errorNone;
  OmxRpc_msg *pRpcMsg;

  OMXRPC_STUB_TEMPLATE (pRpcHndl,
                        pRpcMsg,
                        retVal,
                        OmxRpc_cmdComponentTunnelRequest,
                        component_tunnel_request,
                        (&pRpcMsg->api.componentTunnelRequest,
                         nPort,
                         hTunneledComp,
                         nTunneledPort,
                         pTunnelSetup),
                        (&pRpcMsg->api.componentTunnelRequestResponse,
                         pTunnelSetup), nCmdStatus);
  return retVal;
}

/**
 * @name OmxRpc_stubUnMapBuffer
 * @brief OmxRpc stub function to unmap a buffer
 * @param pRpcHndl                        : OmxRpc stub handle
 * @param mappedBuffer                    : Mapped buffer value
 * @return OmxRpc_errorNone : SUCCESS
 */
OmxRpc_errorType OmxRpc_stubUnMapBuffer (OmxRpc_Handle pRpcHndl,
                                         UInt32 mappedBuffer)
{
  IDmm_errorType dmmErrorCode;
  OmxRpc_errorType retVal = OmxRpc_errorNone;

  dmmErrorCode = OmxRpc_PDmm_unmapBuffer (pRpcHndl->remoteCoreId, mappedBuffer);
  if (IDmm_errorNone != dmmErrorCode)
  {
    retVal = OmxRpc_errorDmmBufferMapFailed;
  }
  return retVal;
}

/**
 * @name OmxRpc_stubFlushBuffer
 * @brief OmxRpc stub function to flush a buffer
 * @param pRpcHndl                        : OmxRpc stub handle
 * @param pBuffer                         : Pointer to buffer
 * @param size                            : Size of buffer in bytes
 * @return OmxRpc_errorNone : SUCCESS
 */
OmxRpc_errorType OmxRpc_stubFlushBuffer (OmxRpc_Handle pRpcHndl,
                                         OmxTypes_OMX_U8 *pBuffer,
                                         OmxTypes_OMX_U32 size)
{
  IDmm_errorType dmmErrorCode;
  OmxRpc_errorType retVal = OmxRpc_errorNone;

  dmmErrorCode = OmxRpc_PDmm_flushBuffer (pRpcHndl->remoteCoreId,
                                          pBuffer, size);
  if (IDmm_errorNone != dmmErrorCode)
  {
    retVal = OmxRpc_errorDmmBufferFlushFailed;
  }
  return retVal;
}

/**
 * @name OmxRpc_stubGetSkelHandle
 * @brief OmxRpc stub function to get a component's OMXRPC skeleton handle
 * @param pRpcHndl                        : OmxRpc stub handle
 * @param pRpcSkelHandlePtr               : Pointer to skelHandle to be set
 * @return OmxRpc_errorNone : SUCCESS
 */
OmxRpc_errorType OmxRpc_stubGetSkelHandle (OmxRpc_Handle pRpcHndl,
                                           OmxRpc_Handle *pRpcSkelHandlePtr)
{

  *pRpcSkelHandlePtr = pRpcHndl->layer.stub.skelHandle;
  return OmxRpc_errorNone;
}

OmxRpc_errorType
  OmxRpc_stubSendCommand_non_template_expansion (OmxRpc_Handle pRpcHndl,
                                                 OmxCore_OMX_COMMANDTYPE eCmd,
                                                 OmxTypes_OMX_U32 nParam,
                                                 OmxTypes_OMX_PTR pCmdData,
                                                 OmxCore_OMX_ERRORTYPE
                                                   *nCmdStatus)
{
  OmxRpc_errorType retVal = OmxRpc_errorNone;
  RcmClient_Message *rcmMsg;
  OmxRpc_msg *pRpcMsg;
  Bool msgAllocDone = FALSE;

  DOMX_UTL_TRACE_FUNCTION_ASSERT ((TRUE ==
                                   OmxRpc_module->localCoreRcmServer.initDone),
                                  "localCoreRcmServer init not done");
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((NULL != pRpcHndl), "rpcHandle is NULL");
  DOMX_UTL_TRACE_FUNCTION_ASSERT
    ((TRUE ==
      OmxRpc_module->remoteCoreRcmClient[pRpcHndl->remoteCoreId].initDone),
     "remoteCoreRcmClient init not done");

  /* Initialize the API out params to NULL initially */
  *nCmdStatus = (OmxCore_OMX_ERRORTYPE) OMX_ErrorNone;

  /* allocate a remote command message */
  retVal = OmxRpc_rcmMsgAlloc ((RcmClient_Handle) pRpcHndl->client.handle,
                               &rcmMsg, __FILE__, __LINE__);
  if (OmxRpc_errorNone == retVal)
  {
    UInt32 fxnId;

    msgAllocDone = TRUE;
    pRpcMsg = (OmxRpc_msg *) (&rcmMsg->data);
    /* Packing GetHandle messages */
    /* Filling common part of the message */
    OmxRpc_msgMarshallCommonStub (pRpcMsg,
                                  pRpcHndl,
                                  pRpcHndl->layer.stub.skelHandle,
                                  OmxRpc_cmdSendCommand);
    omxrpc_msg_marshall_sendcommand (&pRpcMsg->api.sendCommand,
                                     eCmd, nParam, pCmdData);
    fxnId = pRpcHndl->client.fxns.cmd2fxnIdxMap[OmxRpc_cmdSendCommand];
    retVal = OmxRpc_rcmExec ((RcmClient_Handle) pRpcHndl->client.handle,
                             rcmMsg, fxnId);
    if (OmxRpc_errorNone == retVal)
    {
      OmxRpc_msgUnmarshallCommonStub (&pRpcMsg->common, nCmdStatus);
    }
  }                             /* end of if (rcmMsgAlloc success) */
  if (msgAllocDone)
  {
    /* If msg was successfully executed free return the Rcm Msg to heap */
    RcmClient_free ((RcmClient_Handle) pRpcHndl->client.handle, rcmMsg);
  }
  return retVal;
}

OmxRpc_errorType
  OmxRpc_stubGetState_non_template_expansion (OmxRpc_Handle pRpcHndl,
                                              OmxCore_OMX_STATETYPE *pState,
                                              OmxCore_OMX_ERRORTYPE *nCmdStatus)
{

  OmxRpc_errorType retVal = OmxRpc_errorNone;
  RcmClient_Message *rcmMsg;
  OmxRpc_msg *pRpcMsg;
  Bool msgAllocDone = FALSE;

  DOMX_UTL_TRACE_FUNCTION_ASSERT ((TRUE ==
                                   OmxRpc_module->localCoreRcmServer.initDone),
                                  "localCoreRcmServer init not done");
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((NULL != pRpcHndl), "rpcHandle is NULL");
  DOMX_UTL_TRACE_FUNCTION_ASSERT
    ((TRUE ==
      OmxRpc_module->remoteCoreRcmClient[pRpcHndl->remoteCoreId].initDone),
     "remoteCoreRcmClient init not done");

  /* Initialize the API out params to NULL initially */
  *nCmdStatus = (OmxCore_OMX_ERRORTYPE) OMX_ErrorNone;
  *pState = OmxCore_OMX_StateInvalid;   /* This is the default return */
  /* allocate a remote command message */
  retVal = OmxRpc_rcmMsgAlloc ((RcmClient_Handle) pRpcHndl->client.handle,
                               &rcmMsg, __FILE__, __LINE__);
  if (OmxRpc_errorNone == retVal)
  {
    UInt32 fxnId;

    msgAllocDone = TRUE;
    pRpcMsg = (OmxRpc_msg *) (&rcmMsg->data);
    /* Packing GetHandle messages */
    /* Filling common part of the message */
    OmxRpc_msgMarshallCommonStub (pRpcMsg,
                                  pRpcHndl,
                                  pRpcHndl->layer.stub.skelHandle,
                                  OmxRpc_cmdGetState);
    fxnId = pRpcHndl->client.fxns.cmd2fxnIdxMap[OmxRpc_cmdGetState];
    retVal = OmxRpc_rcmExec ((RcmClient_Handle) pRpcHndl->client.handle,
                             rcmMsg, fxnId);
    if (OmxRpc_errorNone == retVal)
    {
      OmxRpc_msgUnmarshallCommonStub (&pRpcMsg->common, nCmdStatus);
      omxrpc_msg_unmarshall_getstate_response (&pRpcMsg->api.getStateResponse,
                                               pState);
    }
  }                             /* end of if (rcmMsgAlloc success) */
  if (msgAllocDone)
  {
    /* If msg was successfully executed free return the Rcm Msg to heap */
    RcmClient_free ((RcmClient_Handle) pRpcHndl->client.handle, rcmMsg);
  }
  return retVal;
}

OmxRpc_errorType
  OmxRpc_stubGetComponentVersion_non_template_expansion
    (OmxRpc_Handle pRpcHndl, OmxTypes_OMX_STRING pComponentName,
     OmxTypes_OMX_VERSIONTYPE *pComponentVersion,
     OmxTypes_OMX_VERSIONTYPE *pSpecVersion,
     OmxTypes_OMX_UUIDTYPE *pComponentUUID, OmxCore_OMX_ERRORTYPE *nCmdStatus)
{
  OmxRpc_errorType retVal = OmxRpc_errorNone;
  RcmClient_Message *rcmMsg;
  OmxRpc_msg *pRpcMsg;
  Bool msgAllocDone = FALSE;

  DOMX_UTL_TRACE_FUNCTION_ASSERT ((TRUE ==
                                   OmxRpc_module->localCoreRcmServer.initDone),
                                  "localCoreRcmServer init not done");
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((NULL != pRpcHndl), "rpcHandle is NULL");
  DOMX_UTL_TRACE_FUNCTION_ASSERT
    ((TRUE ==
      OmxRpc_module->remoteCoreRcmClient[pRpcHndl->remoteCoreId].initDone),
     "remoteCoreRcmClient init not done");

  if ((NULL == pComponentName)
      || (NULL == pComponentVersion)
      || (NULL == pSpecVersion)
      || (NULL == pComponentUUID) || (NULL == nCmdStatus))
  {
    return OmxRpc_errorBadArgument;
  }

  /* Initialize the API out params to NULL initially */
  *nCmdStatus = (OmxCore_OMX_ERRORTYPE) OMX_ErrorNone;
  /* TODO Verify if these default values are okay */
  pComponentName[0] = '\0';     /* Init the componentName to Null string */
  pComponentVersion->nVersion = 0;
  pSpecVersion->nVersion = 0;
  pComponentUUID[0][0] = 0;
  /* allocate a remote command message */
  retVal = OmxRpc_rcmMsgAlloc ((RcmClient_Handle) pRpcHndl->client.handle,
                               &rcmMsg, __FILE__, __LINE__);
  if (OmxRpc_errorNone == retVal)
  {
    UInt32 fxnId;

    msgAllocDone = TRUE;
    pRpcMsg = (OmxRpc_msg *) (&rcmMsg->data);
    /* Packing GetHandle messages */
    /* Filling common part of the message */
    OmxRpc_msgMarshallCommonStub (pRpcMsg,
                                  pRpcHndl,
                                  pRpcHndl->layer.stub.skelHandle,
                                  OmxRpc_cmdGetComponentVersion);
    fxnId = pRpcHndl->client.fxns.cmd2fxnIdxMap[OmxRpc_cmdGetComponentVersion];
    retVal = OmxRpc_rcmExec ((RcmClient_Handle) pRpcHndl->client.handle,
                             rcmMsg, fxnId);
    if (OmxRpc_errorNone == retVal)
    {
      OmxRpc_msgUnmarshallCommonStub (&pRpcMsg->common, nCmdStatus);
      omxrpc_msg_unmarshall_get_component_version_response (&pRpcMsg->api.
                                                            getComponentVersionResponse,
                                                            pComponentName,
                                                            pComponentVersion,
                                                            pSpecVersion,
                                                            pComponentUUID);
    }
  }                             /* end of if (rcmMsgAlloc success) */
  if (msgAllocDone)
  {
    /* If msg was successfully executed free return the Rcm Msg to heap */
    RcmClient_free ((RcmClient_Handle) pRpcHndl->client.handle, rcmMsg);
  }
  return retVal;
}

OmxRpc_errorType OmxRpc_stubGetHeapMemStats (OmxTypes_OMX_U32 *totalFreeSize,
                                             OmxCore_OMX_ERRORTYPE *nCmdStatus)
{
  OmxRpc_errorType retVal = OmxRpc_errorNone;
  OmxRpc_msg *pRpcMsg;
  RcmClient_Message *rcmMsg;
  Bool msgAllocDone = FALSE;
  OmxTypes_OMX_U32 remoteCoreId = 0;

  /* Initialize the API out params to NULL initially */
  *nCmdStatus = (OmxCore_OMX_ERRORTYPE) OMX_ErrorNone;

  DOMX_UTL_TRACE_FUNCTION_ASSERT ((TRUE ==
                                   OmxRpc_module->localCoreRcmServer.initDone),
                                  "localCoreRcmServer init not done");
  DOMX_UTL_TRACE_FUNCTION_ASSERT
    ((TRUE == OmxRpc_module->remoteCoreRcmClient[remoteCoreId].initDone),
     "remoteCoreRcmClient init not done");

  /* allocate a remote command message */
  retVal = OmxRpc_rcmMsgAlloc ((RcmClient_Handle) OmxRpc_module->
                               remoteCoreRcmClient[remoteCoreId].handle,
                               &rcmMsg, __FILE__, __LINE__);
  if (OmxRpc_errorNone == retVal)
  {
    UInt32 fxnId;
    UInt32 cmdId = (Uint32) OmxRpc_cmdGetHeapMemStats;

    msgAllocDone = TRUE;
    pRpcMsg = (OmxRpc_msg *) (&rcmMsg->data);
    /* Packing GetHandle messages */
    /* Filling common part of the message */
    OmxRpc_msgMarshallCommonStub (pRpcMsg, NULL, NULL, (OmxRpc_cmd) cmdId);
    omxrpc_msg_marshall_getheapmemstats ();

    fxnId = OmxRpc_rcmFxnRegistryTbl.elem[cmdId]->fxnIdx;

    retVal =
      OmxRpc_rcmExec ((RcmClient_Handle) OmxRpc_module->
                      remoteCoreRcmClient[remoteCoreId].handle, rcmMsg, fxnId);
    if (OmxRpc_errorNone == retVal)
    {
      OmxRpc_msgUnmarshallCommonStub (&pRpcMsg->common, nCmdStatus);
      omxrpc_msg_unmarshall_getheapmemstats_response (&pRpcMsg->api.
                                                      getHeapMemStatsResponse,
                                                      (OMX_U32 *)
                                                      totalFreeSize);
    }
  }                             /* end of if (rcmMsgAlloc success) */
  if (msgAllocDone)
  {
    /* If msg was successfully executed free return the Rcm Msg to heap */
    RcmClient_free ((RcmClient_Handle) OmxRpc_module->
                    remoteCoreRcmClient[remoteCoreId].handle, rcmMsg);
  }
  return retVal;
}

OmxRpc_errorType OmxRpc_stubDeleteProxyLite (DomxTypes_coreType eRemoteCoreId,
                                             OmxTypes_OMX_HANDLETYPE
                                               pProxyLiteHandlePtr,
                                             OmxCore_OMX_ERRORTYPE 
                                               *nCmdStatus)
{
  OmxRpc_errorType retVal = OmxRpc_errorNone;
  OmxRpc_msg *pRpcMsg;
  RcmClient_Message *rcmMsg;
  Bool msgAllocDone = FALSE;
  Bool initModuleRcmClientDone = FALSE;

  DOMX_UTL_TRACE_FUNCTION_ASSERT ((TRUE ==
                                   OmxRpc_module->localCoreRcmServer.initDone),
                                  "localCoreRcmServer init not done");
  if (FALSE == OmxRpc_module->remoteCoreRcmClient[eRemoteCoreId].initDone)
  {
    OmxRpc_moduleInitClient (eRemoteCoreId);
    initModuleRcmClientDone = TRUE;
  }
  DOMX_UTL_TRACE_FUNCTION_ASSERT
    ((TRUE == OmxRpc_module->remoteCoreRcmClient[eRemoteCoreId].initDone),
     "remoteCoreRcmClient init not done");

  /* allocate a remote command message */
  retVal =
    OmxRpc_rcmMsgAlloc (OmxRpc_module->
                        remoteCoreRcmClient[eRemoteCoreId].handle,
                        &rcmMsg, __FILE__, __LINE__);

  if (OmxRpc_errorNone == retVal)
  {
    OmxRpc_cmd cmdId = OmxRpc_cmdDeleteProxyLite;
    UInt32 fxnId =
      OmxRpc_module->remoteCoreRcmClient[eRemoteCoreId].fxns.
      cmd2fxnIdxMap[cmdId];

    msgAllocDone = TRUE;
    pRpcMsg = (OmxRpc_msg *) (&rcmMsg->data);
    /* Packing GetHandle messages */
    /* Filling common part of the message */
    OmxRpc_msgMarshallCommonStub (pRpcMsg, NULL, NULL, cmdId);
    omxrpc_msg_marshall_delete_proxy_lite (&pRpcMsg->api.deleteProxyLite,
                                           pProxyLiteHandlePtr);

    retVal = OmxRpc_rcmExec ((RcmClient_Handle) OmxRpc_module->
                             remoteCoreRcmClient[eRemoteCoreId].handle,
                             rcmMsg, fxnId);
    if (OmxRpc_errorNone == retVal)
    {
      OmxRpc_msgUnmarshallCommonStub (&pRpcMsg->common, nCmdStatus);
      omxrpc_msg_unmarshall_delete_proxy_lite_response ();
    }
  }                             /* end of if (rcmMsgAlloc success) */
  if (msgAllocDone)
  {
    /* If msg was successfully executed free return the Rcm Msg to heap */
    RcmClient_free ((RcmClient_Handle) OmxRpc_module->
                    remoteCoreRcmClient[eRemoteCoreId].handle, rcmMsg);
  }
  if (initModuleRcmClientDone)
  {
    OmxRpc_moduleDeinitClient (eRemoteCoreId);
  }
  return retVal;
}

/* Nothing beyond this point */
