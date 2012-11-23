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

#ifndef _DOMX_INTERNAL_H_
#define _DOMX_INTERNAL_H_

#include <xdc/runtime/Error.h>
#include <ti/sdo/rcm/RcmClient.h>
#include "domx.h"
/* 
 * DomxTypes
 */

/* Defines used within */

/* Length of char array to hold a string - typically name of an entity */
#define DOMX_INT_NAMESTRING_SIZE        (128 + 1)

/* Max length of RPC table entries array */
#define DOMX_INT_MAX_RPCTBL_ENTRIES     (64)

/* Max number of responses for Heap mem stat */
#define DOMX_INT_RPC_MSGHEAPMEMSTATRESP (10)

/* Max size of Params buffer */
#define DOMX_INT_MAX_PARAMSTRUCTBUF     (1024)

/* Max value that can be stored in an unsigned 16-bit integer */
#define DOMX_INT_MAX_UINT16_VALUE       (0xFFFF)

/* Mask used for status/return value within OMX RPC */
#define DOMX_INT_OMXRPC_STATUS_MASK     (0x8000)

/* DOMX Tunnel Mgr - invalid entry */
#define DOMX_INT_TUNNELMGR_INVALID      (-1)

/* Thread_sleep compatible values; Thread_sleep() takes in number of micro-secs
   to sleep */
/* 1000 micro seconds */
#define DOMX_INT_THREADSLEEP_1000_MICROSEC (1000)
/* 1 second */
#define DOMX_INT_THREADSLEEP_1_SEC   (1000 * DOMX_INT_THREADSLEEP_1000_MICROSEC)

/*
 * OmxRpcStubCommon
 */

/**
 * @enum  OmxRpc_configParamApiType_e
 * @brief Internal enum to differentiate between Get & Set Param/Config
 * @param tunnelRequestResponseMsg        : Pointer to msg
 * @param pTunnelSetup                    : Pointer to tunnel setup structure
 * @return None
 */
typedef enum OmxRpc_configParamApiType_e
{

  OmxRpc_configParamApiTypeSet, /**!< SetParam/Config API */

  OmxRpc_configParamApiTypeGet  /**!< GetParam/Config API */
} OmxRpc_configParamApiType_e;

/* OmxRpc_errorType - enum to specify various error codes used within OMX RPC */
typedef enum OmxRpc_errorType_e
{
  OmxRpc_errorNone,
  OmxRpc_errorUnknown,
  OmxRpc_errorProccesorInit,
  OmxRpc_errorBadArgument,
  OmxRpc_errorInvalidRPCCmd,
  OmxRpc_errorHLOS,
  OmxRpc_errorResourceAllocFail,
  OmxRpc_errorRemoteServerLocateFail,
  OmxRpc_errorRemoteFxnNotFound,
  OmxRpc_errorRcmExecFailed,
  OmxRpc_errorInvalidLayerType,
  OmxRpc_errorMaxConfigParamStructSizeExceeded,
  OmxRpc_errorDmmBufferMapFailed,
  OmxRpc_errorDmmBufferFlushFailed,
  OmxRpc_errorSkelCreateFailed,
  OmxRpc_errorOmxComponentError
} OmxRpc_errorType;

/* OmxRpc_cmd - enum to specify commands that are sent across by OMX RPC */
typedef enum OmxRpc_cmd_e
{
  OmxRpc_cmdFirst = 0x0000,
  OmxRpc_cmdGetHandle = OmxRpc_cmdFirst,
  OmxRpc_cmdFreeHandle,
  OmxRpc_cmdAllocBuffer,
  OmxRpc_cmdUseBuffer,
  OmxRpc_cmdFreeBuffer,
  OmxRpc_cmdSetParameter,
  OmxRpc_cmdGetParameter,
  OmxRpc_cmdSetConfig,
  OmxRpc_cmdGetConfig,
  OmxRpc_cmdGetComponentVersion,
  OmxRpc_cmdGetExtensionIndex,
  OmxRpc_cmdGetState,
  OmxRpc_cmdSendCommand,
  OmxRpc_cmdEmptyThisBuffer,
  OmxRpc_cmdFillThisBuffer,
  OmxRpc_cmdCreateProxyLite,
  OmxRpc_cmdDeleteProxyLite,
  OmxRpc_cmdComponentTunnelRequest,
  OmxRpc_cmdCbEventHandler,
  OmxRpc_cmdCbEmptyBufferDone,
  OmxRpc_cmdCbFillBufferDone,
  OmxRpc_cmdGetHeapMemStats,
  OmxRpc_cmdLast = OmxRpc_cmdGetHeapMemStats,
  OmxRpc_cmdCount = (OmxRpc_cmdLast + 1)
} OmxRpc_cmd;

/* OMX RPC msgCommon: Common part of the OMX RPC message */
typedef struct OmxRpc_msgCommon_s
{
  xdc_UInt32 eRpcCmd;
  xdc_UInt32 nRPCCmdStatus;
  OmxCore_OMX_ERRORTYPE nOMXReturn;
  OmxTypes_OMX_PTR pRpcLocalHandle;
  OmxTypes_OMX_PTR pRpcRemoteHandle;
  OmxTypes_OMX_HANDLETYPE pOmxComponentHandle;
  OmxTypes_OMX_PTR pRpcClientHandle;
  OmxTypes_OMX_U32 nHoleForAlignment;
} OmxRpc_msgCommon;

/* OMX RPC layerType: enum to specify the RPC layer type */
typedef enum OmxRpc_layerType_e
{
  OmxRpc_layerTypeStub,
  OmxRpc_layerTypeStubLite,
  OmxRpc_layerTypeSkel
} OmxRpc_layerType;

/* Remote function entry structure */
typedef struct OmxRpc_remoteFxnEntry_s
{
  OmxTypes_OMX_U32 fxnId;
  xdc_UChar remoteFxnName[DOMX_INT_NAMESTRING_SIZE];
} OmxRpc_remoteFxnEntry;

/* Remote function entry table structure */
typedef struct OmxRpc_remoteFxnEntryTbl_s
{
  OmxTypes_OMX_U16 numEntries;
  OmxRpc_remoteFxnEntry tbl[DOMX_INT_MAX_RPCTBL_ENTRIES];
  xdc_UInt32 cmd2fxnIdxMap[OmxRpc_cmdCount];
} OmxRpc_remoteFxnEntryTbl;

/* OMX RPC Handle - which is pointer to object */
typedef struct OmxRpc_Object_s *OmxRpc_Handle;

/* OMX RPC bufHdrCbMap */
typedef struct OmxRpc_bufHdrCbMap_s
{
  OmxCore_OMX_BUFFERHEADERTYPE *pBufferHdrRemote;
  OmxRpc_Handle pRemoteRpcHandle;
  xdc_Bool isValid;
} OmxRpc_bufHdrCbMap;

/* OMX RPC Skeleton object structure */
typedef struct OmxRpc_skelObject_s
{
  OmxTypes_OMX_HANDLETYPE pComponentHandle;
  OmxRpc_Handle stubHandle;
  OmxRpc_bufHdrCbMap bufHdrCbMapTbl[DOMX_INT_MAX_RPCTBL_ENTRIES];
} OmxRpc_skelObject;

/* OMX RPC callBack */
typedef struct OmxRpc_callBack_s
{
  OmxCore_OMX_ERRORTYPE (*eventCb) (OmxTypes_OMX_HANDLETYPE, OmxTypes_OMX_PTR,
                                    OmxCore_OMX_EVENTTYPE, OmxTypes_OMX_U32,
                                    OmxTypes_OMX_U32, OmxTypes_OMX_PTR);
  OmxCore_OMX_ERRORTYPE (*emptyBufferDoneCb) (OmxTypes_OMX_HANDLETYPE,
                                              OmxTypes_OMX_PTR,
                                              OmxCore_OMX_BUFFERHEADERTYPE *,
                                              OmxCore_OMX_BUFFERHEADERTYPE *);
  OmxCore_OMX_ERRORTYPE (*fillBufferDoneCb) (OmxTypes_OMX_HANDLETYPE,
                                             OmxTypes_OMX_PTR,
                                             OmxCore_OMX_BUFFERHEADERTYPE *,
                                             OmxCore_OMX_BUFFERHEADERTYPE *);
} OmxRpc_callBack;

/* OMX RPC Stub object structure */
typedef struct OmxRpc_stubObject_s
{
  OmxTypes_OMX_PTR pRpcClientCbCtx;
  OmxRpc_callBack rpcClientCb;
  OmxRpc_Handle skelHandle;
  xdc_Int skelRcmServerPriority;
} OmxRpc_stubObject;

/* Internal structures */
/* OmxRpc_server */
typedef struct OmxRpc_server_s
{
  xdc_Ptr handle;
  xdc_UChar name[DOMX_INT_NAMESTRING_SIZE];
  xdc_Int priority;
} OmxRpc_server;

/* OmxRpc_client */
typedef struct OmxRpc_client_s
{
  xdc_Ptr handle;
  xdc_UChar name[DOMX_INT_NAMESTRING_SIZE];
  OmxRpc_remoteFxnEntryTbl fxns;
} OmxRpc_client;

/* OmxRpc_layer */
typedef union OmxRpc_layer_u
{
  OmxRpc_skelObject skel;
  OmxRpc_stubObject stub;
} OmxRpc_layer;

/* OmxRpc_struct_etbftb */
typedef union OmxRpc_struct_etbftb_s
{
  xdc_Ptr handle;
  xdc_UChar name[DOMX_INT_NAMESTRING_SIZE];
} OmxRpc_struct_etbftb;

#define DOMX_MEMUSAGE_MAX_HEAPS_PER_PHASE (4)

/* OmxRpcSkelMemUsagePhase - enum to specify the memory usage phases in OMX
   RPC Skel layer */
typedef enum OmxRpcSkelMemUsagePhase_e
{
  DOMXRPCSKEL_MEMUSAGEPHASE_SKELGETHANDLE,
  DOMXRPCSKEL_MEMUSAGEPHASE_FIRST = DOMXRPCSKEL_MEMUSAGEPHASE_SKELGETHANDLE,
  DOMXRPCSKEL_MEMUSAGEPHASE_RPCSKELCREATE,
  DOMXRPCSKEL_MEMUSAGEPHASE_COMPONENTGETHANDLE,
  DOMXRPCSKEL_MEMUSAGEPHASE_RCMCLIENTCREATE,
  DOMXRPCSKEL_MEMUSAGEPHASE_PROXYLITECREATE,
  DOMXRPCSKEL_MEMUSAGEPHASE_LAST = DOMXRPCSKEL_MEMUSAGEPHASE_PROXYLITECREATE,
  DOMXRPCSKEL_MEMUSAGEPHASE_NUM_PHASES = (DOMXRPCSKEL_MEMUSAGEPHASE_LAST + 1)
} OmxRpcSkelMemUsagePhase;

/* OMX RPC object structure */
typedef struct OmxRpc_Object_s
{
  OmxRpc_layerType layerId;
  DomxTypes_coreType remoteCoreId;
  xdc_UInt16 instID;
  xdc_UChar szComponentName[DOMX_INT_NAMESTRING_SIZE];
  OmxRpc_server server;
  OmxRpc_client client;
  OmxRpc_struct_etbftb clientEtb;
  OmxRpc_struct_etbftb clientFtb;
  OmxRpc_layer layer;
  OMX_U32 memoryUsed[DOMXRPCSKEL_MEMUSAGEPHASE_NUM_PHASES][DOMX_MEMUSAGE_MAX_HEAPS_PER_PHASE];
  /**< Array storing memory used from each heap in different stages */
} OmxRpc_Object;

/* OMX RPC GetHandle structure */
typedef struct OmxRpc_msgGetHandle_s
{
  xdc_UChar componentName[DOMX_INT_NAMESTRING_SIZE];
  xdc_UChar componentEventCbServerName[DOMX_INT_NAMESTRING_SIZE];
  OmxRpc_Handle stubHandle;
  DomxTypes_coreType stubCoreId;
  xdc_Int rcmOmxComponentServerPriority;
} OmxRpc_msgGetHandle;

/* OMX RPC GetHandle response structure */
typedef struct OmxRpc_msgGetHandleResponse_s
{
  OmxTypes_OMX_HANDLETYPE pOmxComponentHandle;
  OmxRpc_Handle skelHandle;
  xdc_UChar componentCtrlServerName[DOMX_INT_NAMESTRING_SIZE];
} OmxRpc_msgGetHandleResponse;

/* OMX RPC message GetHeapMemstats response structure */
typedef struct OmxRpc_msgGetHeapMemStatsResponse_s
{
  OmxTypes_OMX_U32 totalFreeSize[DOMX_INT_RPC_MSGHEAPMEMSTATRESP];
} OmxRpc_msgGetHeapMemStatsResponse;

/* OMX RPC message config parameters structure */
typedef struct OmxRpc_msgConfigParam_s
{
  OmxIndex_OMX_INDEXTYPE nParamConfigIndex;
  xdc_UChar cCompParamStructBuf[DOMX_INT_MAX_PARAMSTRUCTBUF];
} OmxRpc_msgConfigParam;

/* OMX RPC message config parameters query structure */
typedef struct OmxRpc_msgConfigParamQuery_s
{
  OmxIndex_OMX_INDEXTYPE nParamConfigIndex;
  xdc_SizeT nSizeofCompParamStruct;
  xdc_UChar cCompParamStructBuf[DOMX_INT_MAX_PARAMSTRUCTBUF];
} OmxRpc_msgConfigParamQuery;

/* msgSendCommand structure */
typedef struct OmxRpc_msgSendCommand_s
{
  OmxCore_OMX_COMMANDTYPE eCmd;
  OmxTypes_OMX_U32 nParam;
  OmxTypes_OMX_MARKTYPE markData;
} OmxRpc_msgSendCommand;

/* OMX RPC msgGetState response */
typedef struct OmxRpc_msgGetStateResponse_s
{
  OmxCore_OMX_STATETYPE eState;
} OmxRpc_msgGetStateResponse;

/* OMX RPC msgGetComponentVersion response */
typedef struct OmxRpc_msgGetComponentVersionResponse_s
{
  xdc_UChar componentName[DOMX_INT_NAMESTRING_SIZE];
  OmxTypes_OMX_VERSIONTYPE componentVersion;
  OmxTypes_OMX_VERSIONTYPE specVersion;
  OmxTypes_OMX_UUIDTYPE componentUUID;
} OmxRpc_msgGetComponentVersionResponse;

/* OMX RPC msgGetExtensionIndex */
typedef struct OmxRpc_msgGetExtensionIndex_s
{
  xdc_UChar cParameterName[DOMX_INT_NAMESTRING_SIZE];
} OmxRpc_msgGetExtensionIndex;

/* OMX RPC msgGetExtensionIndex response */
typedef struct OmxRpc_msgGetExtensionIndexResponse_s
{
  OmxIndex_OMX_INDEXTYPE nIndexType;
} OmxRpc_msgGetExtensionIndexResponse;

/* OMX RPC msgUseBuffer */
typedef struct OmxRpc_msgUseBuffer_s
{
  OmxTypes_OMX_U32 nPortIndex;
  OmxTypes_OMX_PTR pAppPrivate;
  OmxTypes_OMX_U32 nSizeBytes;
  OmxTypes_OMX_U32 nBufferMapped;
} OmxRpc_msgUseBuffer;

/* OMX RPC msgUseBuffer response */
typedef struct OmxRpc_msgUseBufferResponse_s
{
  OmxCore_OMX_BUFFERHEADERTYPE *pBufferHeaderRemote;
  OmxCore_OMX_BUFFERHEADERTYPE bufferHeaderCopy;
} OmxRpc_msgUseBufferResponse;

/* OMX RPC msgAllocBuffer */
typedef struct OmxRpc_msgAllocBuffer_s
{
  OmxTypes_OMX_U32 nPortIndex;
  OmxTypes_OMX_PTR pAppPrivate;
  OmxTypes_OMX_U32 nSizeBytes;
} OmxRpc_msgAllocBuffer;

/* OMX RPC msgAllocBuffer response */
typedef struct OmxRpc_msgAllocBufferResponse_s
{
  OmxCore_OMX_BUFFERHEADERTYPE *pBufferHeaderRemote;
  OmxCore_OMX_BUFFERHEADERTYPE bufferHeaderCopy;
  OmxTypes_OMX_U32 nBufferMapped;
} OmxRpc_msgAllocBufferResponse;

/* OMX RPC msgFreeBuffer */
typedef struct OmxRpc_msgFreeBuffer_s
{
  OmxTypes_OMX_U32 nPortIndex;
  OmxCore_OMX_BUFFERHEADERTYPE *pBufHeaderRemote;
} OmxRpc_msgFreeBuffer;

/* OMX RPC msgEmptyAndFillThisBuffer */
typedef struct OmxRpc_msgEmptyAndFillThisBuffer_s
{
  OmxCore_OMX_BUFFERHEADERTYPE *pBufferHdrRemote;
  OmxCore_OMX_BUFFERHEADERTYPE bufferHdrCopy;
} OmxRpc_msgEmptyAndFillThisBuffer;

/* OMX RPC msgEventCb */
typedef struct OmxRpc_msgEventCb_s
{
  OmxTypes_OMX_HANDLETYPE pOmxComponentHandle;
  OmxCore_OMX_EVENTTYPE eEvent;
  OmxTypes_OMX_U32 nData1;
  OmxTypes_OMX_U32 nData2;
  OmxTypes_OMX_MARKTYPE markData;
} OmxRpc_msgEventCb;

/* OMX RPC msgBufferDoneCb */
typedef struct OmxRpc_msgBufferDoneCb_s
{
  OmxTypes_OMX_HANDLETYPE pOmxComponentHandle;
  OmxCore_OMX_BUFFERHEADERTYPE *pBufferHdrRemote;
  OmxCore_OMX_BUFFERHEADERTYPE bufferHdrCopy;
} OmxRpc_msgBufferDoneCb;

/* OMX RPC msgComponentTunnelRequest */
typedef struct OmxRpc_msgComponentTunnelRequest_s
{
  OmxTypes_OMX_U32 nPort;
  OmxTypes_OMX_HANDLETYPE hTunneledComp;
  OmxTypes_OMX_U32 nTunneledPort;
  OmxCore_OMX_TUNNELSETUPTYPE sTunnelSetup;
  OmxTypes_OMX_BOOL bValidTunnelSetup;
} OmxRpc_msgComponentTunnelRequest;

/* OMX RPC msgComponentTunnelRequest response */
typedef struct OmxRpc_msgComponentTunnelRequestResponse_s
{
  OmxCore_OMX_TUNNELSETUPTYPE sTunnelSetup;
} OmxRpc_msgComponentTunnelRequestResponse;

/* OMX RPC msgCreateProxyLite */
typedef struct OmxRpc_msgCreateProxyLite_s
{
  OmxTypes_OMX_U32 nRpcRemoteHandle;
  OmxTypes_OMX_HANDLETYPE pOmxComponentHandle;
  xdc_UChar cComponentName[DOMX_INT_NAMESTRING_SIZE];
  xdc_UChar cComponentRcmServerName[DOMX_INT_NAMESTRING_SIZE];
} OmxRpc_msgCreateProxyLite;

/* OMX RPC msgCreateProxyLite response */
typedef struct OmxRpc_msgCreateProxyLiteResponse_s
{
  OmxTypes_OMX_HANDLETYPE pProxyLiteHandle;
} OmxRpc_msgCreateProxyLiteResponse;

/* OMX RPC msgDeleteProxyLite */
typedef struct OmxRpc_msgDeleteProxyLite_s
{
  OmxTypes_OMX_HANDLETYPE pProxyLiteHandle;
} OmxRpc_msgDeleteProxyLite;

/* Internal structure */
/* OmxRpc_apis */
typedef union OmxRpc_apis_u
{
  OmxRpc_msgGetHandle getHandle;
  OmxRpc_msgGetHandleResponse getHandleResponse;
  OmxRpc_msgConfigParam setParamConfig;
  OmxRpc_msgConfigParamQuery getParamConfig;
  OmxRpc_msgSendCommand sendCommand;
  OmxRpc_msgGetStateResponse getStateResponse;
  OmxRpc_msgGetComponentVersionResponse getComponentVersionResponse;
  OmxRpc_msgGetExtensionIndex getExtensionIndex;
  OmxRpc_msgGetExtensionIndexResponse getExtensionIndexResponse;
  OmxRpc_msgAllocBuffer allocBuffer;
  OmxRpc_msgAllocBufferResponse allocBufferResponse;
  OmxRpc_msgUseBuffer useBuffer;
  OmxRpc_msgUseBufferResponse useBufferResponse;
  OmxRpc_msgFreeBuffer freeBuffer;
  OmxRpc_msgEmptyAndFillThisBuffer emptyFillBuffer;
  OmxRpc_msgEventCb eventCb;
  OmxRpc_msgBufferDoneCb bufferDoneCb;
  OmxRpc_msgCreateProxyLite createProxyLite;
  OmxRpc_msgCreateProxyLiteResponse createProxyLiteResponse;
  OmxRpc_msgComponentTunnelRequest componentTunnelRequest;
  OmxRpc_msgComponentTunnelRequestResponse componentTunnelRequestResponse;
  OmxRpc_msgGetHeapMemStatsResponse getHeapMemStatsResponse;
  OmxRpc_msgDeleteProxyLite deleteProxyLite;
} OmxRpc_apis;

/* OMX RPC msg */
typedef struct OmxRpc_msg_s
{
  OmxRpc_msgCommon common;
  OmxRpc_apis api;
} OmxRpc_msg;

/* 
 * OmxRpcStub
 */

/* OMX RPC remoteFxnCategory */
typedef enum OmxRpc_remoteFxnCategory_e
{
  OmxRpc_remoteFxnInit,
  OmxRpc_remoteFxnCtrl,
  OmxRpc_remoteFxnCb
} OmxRpc_remoteFxnCategory;

/* OMX RPC msgHeapObj */
typedef struct OmxRpc_msgHeapObj_s
{
  xdc_runtime_IGateProvider_Handle gateHandle;
  xdc_runtime_IHeap_Handle msgHeapHandle;
  xdc_UInt16 msgHeapId;
  xdc_UChar msgqHeapName[DOMX_INT_NAMESTRING_SIZE];
  xdc_Ptr gateCtrlAddr;
  xdc_UInt32 gateCtrlSize;
  xdc_Ptr heapCtrlAddr;
  xdc_UInt32 heapCtrlSize;
  xdc_Ptr heapBufAddr;
  xdc_UInt32 heapBufSize;
} OmxRpc_msgHeapObj;

/* Internal structures */
typedef struct OmxRpc_rcmServer_s
{
  xdc_Ptr handle;
  xdc_UChar name[DOMX_INT_NAMESTRING_SIZE];
  xdc_Bool initDone;
} OmxRpc_rcmServer;

typedef struct OmxRpc_rcmClient_s
{
  xdc_Ptr handle;
  xdc_Int numInstances;
  xdc_UChar name[DOMX_INT_NAMESTRING_SIZE];
  xdc_Bool initDone;
  OmxRpc_remoteFxnEntryTbl fxns;
  OmxRpc_msgHeapObj msgHeap;
} OmxRpc_rcmClient;

/* OMX RPC Module_State */
typedef struct OmxRpc_Module_State_s
{
  OmxRpc_rcmServer localCoreRcmServer;
  OmxRpc_rcmClient remoteCoreRcmClient[DomxTypes_coreCount];
} OmxRpc_Module_State;

/*
 * OmxProxy
 */
/* OMX Proxy configParamStructInfo */
typedef struct OmxProxy_configParamStructInfo_s
{
  OmxIndex_OMX_INDEXTYPE nParamIndex;
  xdc_UInt32 nParamStructSizeof;
} OmxProxy_configParamStructInfo;

/* OMX Proxy configParamStructInfoMap */
typedef struct OmxProxy_configParamStructInfoMap_s
{
  xdc_String omxComponentName;
  OmxProxy_configParamStructInfo *configParamStructInfoTbl;
} OmxProxy_configParamStructInfoMap;

/* OMX Proxy proxyLiteCreateParams */
typedef struct OmxProxy_proxyLiteCreateParams_s
{
  OmxTypes_OMX_U32 nRpcRemoteHandle;
  OmxTypes_OMX_HANDLETYPE pOmxComponentHandle;
  OmxTypes_OMX_S8 *cComponentName;
  OmxTypes_OMX_S8 *cComponentRcmServerName;
} OmxProxy_proxyLiteCreateParams;

/*!
 *  @struct OmxProxy_configStructCommonHeader
 *  @brief  Structure defining common part of all OMX
 *          config/param structures
 */
typedef struct OmxProxy_configStructCommonHeader_s
{
  OMX_U32 nSize;                /**< size of the structure in bytes */
  OMX_VERSIONTYPE nVersion;     /**< OMX specification version information */
} OmxProxy_configStructCommonHeader;

/*!
 *  @struct OmxProxy_proxyBufferInfo
 *  @brief  Structure defining Buffer Info maintained by proxy
 *          component. Used to map localCoreBuffer to remoteCoreBuffer
 */
typedef struct OmxProxy_proxyBufferInfo_s
{
  OMX_BUFFERHEADERTYPE *pBufHeader;       /**< Pointer to local buffer header */
  OMX_BUFFERHEADERTYPE *pBufHeaderRemote; /**< Pointer to remote buffer header */
  OMX_U8 *pBufferMapped;                  /**< Pointer to buffer mapped value */
  OMX_U8 *pBufferActual;                  /**< Pointer to buffer actual value */
  OMX_PTR actualContent;                  /**< actual Content */
} OmxProxy_proxyBufferInfo;

typedef enum DomxProxyMemUsagePhase_e
{
  DOMXPROXY_MEMUSAGEPHASE_PROXYCREATE,
  DOMXPROXY_MEMUSAGEPHASE_FIRST = DOMXPROXY_MEMUSAGEPHASE_PROXYCREATE,
  DOMXPROXY_MEMUSAGEPHASE_RPCSTUBCREATE,
  DOMXPROXY_MEMUSAGEPHASE_RPCGETHANDLE,
  DOMXPROXY_MEMUSAGEPHASE_PROXYLITECREATE,
  DOMXPROXY_MEMUSAGEPHASE_STUBLITECREATE,
  DOMXPROXY_MEMUSAGEPHASE_RPCLITEGETHANDLE,
  DOMXPROXY_MEMUSAGEPHASE_LAST = DOMXPROXY_MEMUSAGEPHASE_RPCLITEGETHANDLE,
  DOMXPROXY_MEMUSAGEPHASE_NUM_PHASES = (DOMXPROXY_MEMUSAGEPHASE_LAST + 1)
} DomxProxyMemUsagePhase_e;

/* Params */
typedef struct OmxRpc_Params_s
{
  size_t __size;
  const void *__self;
  void *__fxns;
  xdc_runtime_IInstance_Params *instance;
  OmxRpc_layerType layerId;
  DomxTypes_coreType remoteCoreId;
  xdc_Int rcmEventCbServerPriority;
  xdc_Int rcmOmxComponentServerPriority;
  xdc_String componentName;
  xdc_runtime_IInstance_Params __iprms;
} OmxRpc_Params;

/* DOMX Core componentCoreInfoEntry */
typedef struct DomxCore_componentCoreInfoEntry_s
{
  xdc_String name;
  DomxTypes_coreType coreId;
  xdc_Int ctrlRcmServerPriority;
  xdc_Int cbRcmServerPriority;
  xdc_UInt maxNumBuffers;
} DomxCore_componentCoreInfoEntry;

typedef DomxCore_componentCoreInfoEntry DomxCore_componentCoreInfo[1];

/* DOMX Core componentCoreInfo table */
typedef struct DomxCore_componentCoreInfoTbl_CT_s
{
  int length;
  DomxCore_componentCoreInfo *elem;
} DomxCore_componentCoreInfoTbl_CT;

/* DOMX Core slaveCoreExecutableInfo element */
typedef struct DomxCore_slaveCoreExecutableInfoElem_s
{
  xdc_String executableName;
  DomxTypes_coreType coreId;
  xdc_String multiProcName;
} DomxCore_slaveCoreExecutableInfoElem;

/* DOMX Core slaveCoreExecutableInfo */
typedef struct DomxCore_slaveCoreExecutableInfo_s
{
  int length;
  DomxCore_slaveCoreExecutableInfoElem *elem;
} DomxCore_slaveCoreExecutableInfo;

/*
 * OmxRpcSkel
 */

/* OMX RPC skelComponentConfigInfo */
typedef struct OmxRpc_skelComponentConfigInfo_s
{
  OmxTypes_OMX_HANDLETYPE pComponentHandle;
  OmxRpc_Handle stubHandle;
} OmxRpc_skelComponentConfigInfo;

/*
 * OmxRpc
 */

/* OMX RPC remoteFuncPtr */
typedef xdc_Int32 (*OmxRpc_remoteFuncPtr) (xdc_UInt32, xdc_UInt32 *);

/* OMX RPC rcmFxnRegistry */
typedef struct OmxRpc_rcmFxnRegistry_s
{
  xdc_String fxnName;
  OmxRpc_remoteFuncPtr fxnPtr;
  OmxRpc_remoteFxnCategory fxnCategory;
  OmxRpc_cmd omxCmdId;
  xdc_UInt32 fxnIdx;
} OmxRpc_rcmFxnRegistry;

typedef OmxRpc_rcmFxnRegistry OmxRpc_rcmFxnRegistryAry[1];

/* OMX RPC rcmFxnRegistry table structure */
typedef struct OmxRpc_rcmFxnRegistryTbl_CT_s
{
  int length;
  OmxRpc_rcmFxnRegistryAry *elem;
} OmxRpc_rcmFxnRegistryTbl_CT;

/*
 * DomxCore
 */

/* DOMX Core slaveProcState */
typedef struct DomxCore_slaveProcState_s
{
  xdc_UInt32 procMgrFileId;
  xdc_Void *procMgrHandle;
  xdc_UInt16 procId;
} DomxCore_slaveProcState;

/* DOMX Core Module_State */
typedef struct DomxCore_Module_State_s
{
  xdc_Bool initDone;
  DomxCore_slaveProcState multiProcState[DomxTypes_coreCount];
} DomxCore_Module_State;

/* DOMX Core ipcInfo */
typedef struct DomxCore_ipcInfo_s
{
  xdc_Bool ipcEnable;
  xdc_UInt16 multiProcId;
} DomxCore_ipcInfo;

/*
 * DomxTunnelMgr
 */

/* DOMX TunnelMgr compRpcSkelInfo */
typedef struct DomxTunnelMgr_compRpcSkelInfo_s
{
  OmxTypes_OMX_PTR pRpcSkelHandle;
  xdc_Bool bRpcSkelCreated;
  xdc_UChar cComponentRcmSvrName[DOMX_INT_NAMESTRING_SIZE];
} DomxTunnelMgr_compRpcSkelInfo;

/* DOMX TunnelMgr coreConnectionInfo */
typedef struct DomxTunnelMgr_coreConnectionInfo_s
{
  xdc_UInt32 nConnectionCount;
  OmxTypes_OMX_HANDLETYPE hConnectionHandle;
  xdc_Bool bProxyLiteCreated;
} DomxTunnelMgr_coreConnectionInfo;

/* DOMX TunnelMgr connectionInfo */
typedef struct DomxTunnelMgr_connectionInfo_s
{
  OmxTypes_OMX_HANDLETYPE hComponentRealHandle;
  xdc_UChar cComponentName[DOMX_INT_NAMESTRING_SIZE];
  DomxTypes_coreType eCompCoreId;
  DomxTunnelMgr_compRpcSkelInfo sRpcSkelInfo;
  DomxTunnelMgr_coreConnectionInfo sCoreConnectionInfo[DomxTypes_coreCount];
} DomxTunnelMgr_connectionInfo;

typedef DomxTunnelMgr_connectionInfo DomxTunnelMgr_connectionInfoAry[1];

/* DOMX TunnelMgr Module_State connectionInfo table structure */
typedef struct DomxTunnelMgr_Module_State__connectionInfoTbl_CT_s
{
  int length;
  DomxTunnelMgr_connectionInfoAry *elem;
} DomxTunnelMgr_Module_State__connectionInfoTbl_CT;

/* DOMX TunnelMgr Module_State */
typedef struct DomxTunnelMgr_Module_State_s
{
  xdc_Bool initDone;
  xdc_UInt32 numActiveComponents;
  DomxTunnelMgr_Module_State__connectionInfoTbl_CT connectionInfoTbl;
} DomxTunnelMgr_Module_State;

/* externs */
extern Int32 OmxRpc_rcmIfGetHandle (UInt32 rcmMsgSize, UInt32 *rcmMsg);
extern Int32 OmxRpc_rcmIfFreeHandle (UInt32 rcmMsgSize, UInt32 *rcmMsg);
extern Int32 OmxRpc_rcmIfCbEventHandler (UInt32 rcmMsgSize, UInt32 *rcmMsg);
extern Int32 OmxRpc_rcmIfCbEmptyFillBufferDone (UInt32 rcmMsgSize,
                                                UInt32 *rcmMsg);
extern Int32 OmxRpc_rcmIfSkelOmxApi (UInt32 rcmMsgSize, UInt32 *rcmMsg);
extern Int32 OmxRpc_rcmIfSkelGetHeapMemStats (UInt32 rcmMsgSize,
                                              UInt32 *rcmMsg);

/* Functions */
/* OmxRpc */
OmxRpc_errorType OmxRpc_skelGetCompRcmServerName (OmxRpc_Handle pRpcHndl,
                                                  OmxTypes_OMX_S8
                                                    *cComponentRcmServerName,
                                                  OmxTypes_OMX_U32
                                                  nComponentRcmServerNameMaxLen);

OmxRpc_errorType OmxRpc_stubCreateProxyLite (OmxRpc_Handle pRpcHndl,
                                             OmxTypes_OMX_U32 nRpcRemoteHandle,
                                             OmxTypes_OMX_HANDLETYPE
                                             pOmxComponentHandle,
                                             OmxTypes_OMX_S8 *cComponentName,
                                             OmxTypes_OMX_S8
                                               *cComponentRcmServerName,
                                             OmxTypes_OMX_HANDLETYPE
                                               *ppProxyLiteHandlePtr,
                                             OmxCore_OMX_ERRORTYPE
                                               *nCmdStatus);

OmxRpc_errorType OmxRpc_stubDeleteProxyLite (DomxTypes_coreType eRemoteCoreId,
                                             OmxTypes_OMX_HANDLETYPE
                                             pProxyLiteHandlePtr,
                                             OmxCore_OMX_ERRORTYPE
                                               *nCmdStatus);

OmxRpc_errorType OmxRpc_skelConfigCompInfo (OmxRpc_Handle pRpcHndl,
                                            OmxRpc_skelComponentConfigInfo
                                              *cfg);

OmxRpc_errorType OmxRpc_moduleInitServer (Void);
OmxRpc_errorType OmxRpc_moduleDeinitServer (Void);
OmxRpc_errorType OmxRpc_moduleRegisterMsgqHeap (DomxTypes_coreType
                                                remoteCoreId);
OmxRpc_errorType OmxRpc_moduleUnregisterMsgqHeap (DomxTypes_coreType
                                                  remoteCoreId);

Int OmxRpc_Instance_init (OmxRpc_Object *obj,
                          const OmxRpc_Params *params, Error_Block *eb);

Void OmxRpc_msgMarshallCommonResponse (OmxRpc_msgCommon *pRpcCommonMsg,
                                       OmxRpc_Handle pRpcLocalHndl,
                                       OmxRpc_Handle pRpcRemoteHndl,
                                       UInt32 eRpcCmd,
                                       OmxCore_OMX_ERRORTYPE nOmxReturnCode,
                                       OmxRpc_errorType nRpcCmdStatus);

Void OmxRpc_msgMarshallCommonStub (OmxRpc_msg *pRpcMsg,
                                   OmxRpc_Handle pRpcLocalHndl,
                                   OmxRpc_Handle pRpcRemoteHndl,
                                   OmxRpc_cmd eRpcCmd);

Void OmxRpc_msgUnmarshallCommonStub (OmxRpc_msgCommon *pRpcMsgCommon,
                                     OmxCore_OMX_ERRORTYPE *nCmdStatus);

OmxRpc_errorType OmxRpc_rcmExec (RcmClient_Handle rcmHndl,
                                 RcmClient_Message *rcmMsg, UInt32 fxnIdx);

void OmxRpc_delete (OmxRpc_Handle *instp);

OmxRpc_errorType OmxRpc_rcmMsgAlloc (RcmClient_Handle rcmHndl,
                                     RcmClient_Message **rcmMsgPtr,
                                     String fileName, Int line_num);

OmxRpc_errorType OmxRpc_stubGetHandle (OmxRpc_Handle pRpcHndl,
                                       OmxRpc_callBack *pCallBacks,
                                       OmxTypes_OMX_PTR pCbCtx,
                                       OmxCore_OMX_ERRORTYPE *nCmdStatus,
                                       OmxTypes_OMX_HANDLETYPE
                                         *ppOmxComponentHandlePtr,
                                       OmxTypes_OMX_U32 *pRpcSkelHandle,
                                       OmxTypes_OMX_S8
                                         *pComponentRcmServerName,
                                       OmxTypes_OMX_U32
                                       nComponentRcmServerNameMaxLen);

OmxRpc_errorType OmxRpc_stubFreeHandle (OmxRpc_Handle pRpcHndl,
                                        OMX_OUT OmxCore_OMX_ERRORTYPE
                                          *nCmdStatus);

OmxRpc_errorType OmxRpc_stubLiteGetHandle (OmxRpc_Handle pRpcHndl,
                                           OmxRpc_callBack *pCallBacks,
                                           OmxTypes_OMX_PTR pCbCtx,
                                           OmxCore_OMX_ERRORTYPE *nCmdStatus,
                                           OmxTypes_OMX_U32 nRpcSkelHandle,
                                           OmxTypes_OMX_S8
                                             *szCompRcmServerName);

OmxRpc_errorType OmxRpc_stubLiteFreeHandle (OmxRpc_Handle pRpcHndl,
                                            OmxCore_OMX_ERRORTYPE *nCmdStatus);

OmxRpc_errorType OmxRpc_stubUnMapBuffer (OmxRpc_Handle pRpcHndl,
                                         UInt32 mappedBuffer);

OmxRpc_errorType OmxRpc_stubUseBuffer (OmxRpc_Handle pRpcHndl,
                                       OMX_OUT OmxCore_OMX_BUFFERHEADERTYPE
                                         *pBufferHdr,
                                       OMX_IN OmxTypes_OMX_U32 nPortIndex,
                                       OMX_IN OmxTypes_OMX_PTR pAppPrivate,
                                       OMX_IN OmxTypes_OMX_U32 nSizeBytes,
                                       OMX_IN OmxTypes_OMX_U8 *pBuffer,
                                       OMX_OUT OmxTypes_OMX_U32 *pBufferMapped,
                                       OMX_OUT OmxCore_OMX_BUFFERHEADERTYPE
                                         **ppBufHeaderRemote,
                                       OMX_OUT OmxCore_OMX_ERRORTYPE
                                         *nCmdStatus);

OmxRpc_errorType OmxRpc_stubAllocBuffer (OmxRpc_Handle pRpcHndl,
                                         OMX_OUT OmxCore_OMX_BUFFERHEADERTYPE
                                           *pBufferHdr,
                                         OMX_IN OmxTypes_OMX_U32 nPortIndex,
                                         OMX_IN OmxTypes_OMX_PTR pAppPrivate,
                                         OMX_IN OmxTypes_OMX_U32 nSizeBytes,
                                         OMX_OUT OmxTypes_OMX_U8 **pBuffer,
                                         OMX_OUT OmxTypes_OMX_U32
                                           *pBufferMapped,
                                         OMX_OUT OmxCore_OMX_BUFFERHEADERTYPE
                                           **ppBufHeaderRemote,
                                         OMX_OUT OmxCore_OMX_ERRORTYPE
                                           *nCmdStatus);

OmxRpc_errorType OmxRpc_stubFlushBuffer (OmxRpc_Handle pRpcHndl,
                                         OmxTypes_OMX_U8 *pBuffer,
                                         OmxTypes_OMX_U32 size);

OmxRpc_errorType OmxRpc_stubFreeBuffer (OmxRpc_Handle pRpcHndl,
                                        OMX_IN OmxTypes_OMX_U32 nPortIndex,
                                        OMX_IN OmxCore_OMX_BUFFERHEADERTYPE
                                          *pBufHeaderRemote,
                                        OMX_OUT OmxCore_OMX_ERRORTYPE
                                          *nCmdStatus);

OmxRpc_errorType OmxRpc_stubETBAndFTB (OmxRpc_Handle pRpcHndl,
                                       OMX_IN OmxCore_OMX_BUFFERHEADERTYPE
                                         *pBufferHdr,
                                       OMX_IN OmxCore_OMX_BUFFERHEADERTYPE
                                         *pBufferHdrRemote,
                                       OMX_IN OmxRpc_cmd cmdId,
                                       OMX_OUT OmxCore_OMX_ERRORTYPE
                                         *nCmdStatus);

OmxRpc_errorType OmxRpc_stubETB (OmxRpc_Handle pRpcHndl,
                                 OMX_IN OmxCore_OMX_BUFFERHEADERTYPE
                                   *pBufferHdr,
                                 OMX_IN OmxCore_OMX_BUFFERHEADERTYPE
                                   *pBufferHdrRemote, OMX_IN OmxRpc_cmd cmdId,
                                 OMX_OUT OmxCore_OMX_ERRORTYPE *nCmdStatus);

OmxRpc_errorType OmxRpc_stubFTB (OmxRpc_Handle pRpcHndl,
                                 OMX_IN OmxCore_OMX_BUFFERHEADERTYPE
                                   *pBufferHdr,
                                 OMX_IN OmxCore_OMX_BUFFERHEADERTYPE
                                   *pBufferHdrRemote, OMX_IN OmxRpc_cmd cmdId,
                                 OMX_OUT OmxCore_OMX_ERRORTYPE *nCmdStatus);

OmxRpc_errorType OmxRpc_stubGetParameter (OmxRpc_Handle pRpcHndl,
                                          OmxIndex_OMX_INDEXTYPE nParamIndex,
                                          OmxTypes_OMX_PTR pCompParam,
                                          SizeT nSizeofCompParamStruct,
                                          OmxCore_OMX_ERRORTYPE *nCmdStatus);

OmxRpc_errorType OmxRpc_stubSetParameter (OmxRpc_Handle pRpcHndl,
                                          OmxIndex_OMX_INDEXTYPE nParamIndex,
                                          OmxTypes_OMX_PTR pCompParam,
                                          SizeT nSizeofCompParamStruct,
                                          OmxCore_OMX_ERRORTYPE *nCmdStatus);

OmxRpc_errorType OmxRpc_stubGetConfig (OmxRpc_Handle pRpcHndl,
                                       OmxIndex_OMX_INDEXTYPE nConfigIndex,
                                       OmxTypes_OMX_PTR pCompConfig,
                                       SizeT nSizeofCompParamStruct,
                                       OmxCore_OMX_ERRORTYPE *nCmdStatus);

OmxRpc_errorType OmxRpc_stubSetConfig (OmxRpc_Handle pRpcHndl,
                                       OmxIndex_OMX_INDEXTYPE nConfigIndex,
                                       OmxTypes_OMX_PTR pCompConfig,
                                       SizeT nSizeofCompParamStruct,
                                       OmxCore_OMX_ERRORTYPE *nCmdStatus);

OmxRpc_errorType OmxRpc_stubGetState (OmxRpc_Handle pRpcHndl,
                                      OmxCore_OMX_STATETYPE *pState,
                                      OmxCore_OMX_ERRORTYPE *nCmdStatus);

OmxRpc_errorType OmxRpc_stubSendCommand (OmxRpc_Handle pRpcHndl,
                                         OmxCore_OMX_COMMANDTYPE eCmd,
                                         OmxTypes_OMX_U32 nParam,
                                         OmxTypes_OMX_PTR pCmdData,
                                         OmxCore_OMX_ERRORTYPE *nCmdStatus);

OmxRpc_errorType OmxRpc_stubGetComponentVersion (OmxRpc_Handle pRpcHndl,
                                                 OmxTypes_OMX_STRING
                                                 pComponentName,
                                                 OmxTypes_OMX_VERSIONTYPE
                                                   *pComponentVersion,
                                                 OmxTypes_OMX_VERSIONTYPE
                                                   *pSpecVersion,
                                                 OmxTypes_OMX_UUIDTYPE
                                                   *pComponentUUID,
                                                 OmxCore_OMX_ERRORTYPE
                                                   *nCmdStatus);

OmxRpc_errorType OmxRpc_stubGetExtensionIndex (OmxRpc_Handle pRpcHndl,
                                               OmxTypes_OMX_STRING
                                               cParameterName,
                                               OmxIndex_OMX_INDEXTYPE
                                                 *pIndexType,
                                               OmxCore_OMX_ERRORTYPE
                                                 *nCmdStatus);

OmxRpc_errorType OmxRpc_stubComponentTunnelRequest (OmxRpc_Handle pRpcHndl,
                                                    OmxTypes_OMX_U32 nPort,
                                                    OmxTypes_OMX_HANDLETYPE
                                                    hTunneledComp,
                                                    OmxTypes_OMX_U32
                                                    nTunneledPort,
                                                    OmxCore_OMX_TUNNELSETUPTYPE
                                                      *pTunnelSetup,
                                                    OmxCore_OMX_ERRORTYPE
                                                      *nCmdStatus);

OmxRpc_errorType OmxRpc_stubGetHeapMemStats (OmxTypes_OMX_U32 *totalFreeSize,
                                             OmxCore_OMX_ERRORTYPE
                                               *nCmdStatus);

/* OmxProxy */
OMX_ERRORTYPE OmxProxy_createLite (OMX_HANDLETYPE *pProxyLiteHandlePtr,
                                   OmxProxy_proxyLiteCreateParams *cp);

OMX_ERRORTYPE OmxProxy_deleteLite (OMX_HANDLETYPE *pProxyLiteHandlePtr);

/* DomxCore */
Void DomxCore_procInit (Error_Block *eb);
Void DomxCore_procDeInit (Error_Block *eb);
UInt16 DomxCore_mapDomxCore2MultiProcId (DomxTypes_coreType coreId,
                                         Error_Block *eb);
DomxTypes_coreType DomxCore_mapComponentName2CoreId (String component_name,
                                                     Error_Block *eb);
Ptr DomxCore_mapPhyAddr2UsrVirtual (UInt32 phyAddr, UInt32 len);

Ptr DomxCore_mapUsrVirtualAddr2phy (UInt32 pUsrVirtAddr);

Void DomxCore_unmapPhyAddr (UInt32 phyAddr, UInt32 len);

/* DomxTunnelMgr */
Void DomxTunnelMgr_registerSkelInfo (OmxTypes_OMX_HANDLETYPE hComponentHandle,
                                     Char *szCompName,
                                     OmxTypes_OMX_PTR pRpcSkelHandle,
                                     Char *cComponentRcmSvrName);

Void DomxTunnelMgr_mapTunnelComponentPeerHandles (OmxTypes_OMX_HANDLETYPE
                                                  hTunneledOutput,
                                                  OmxTypes_OMX_HANDLETYPE
                                                  hTunneledInput,
                                                  OmxTypes_OMX_HANDLETYPE
                                                    *hTunneledOutputPeerMap,
                                                  OmxTypes_OMX_HANDLETYPE
                                                    *hTunneledInputPeerMap);
Void DomxTunnelMgr_registerHandle (OmxTypes_OMX_HANDLETYPE hComponentHandle,
                                   Char *szCompName);

Void DomxTunnelMgr_unRegisterHandle (OmxTypes_OMX_HANDLETYPE hComponentHandle);
#endif /* _DOMX_INTERNAL_H_ */

/* Nothing beyond this point */
