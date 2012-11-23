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
 *  @file  OmxRpcCbSkel.c
 *         DOMX RPC Callback skeleton functionality
 *
 *  @path \avmecomp\ti\omx\domx\OmxRpcCbSkel.c
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
#include <ti/sdo/fc/memutils/memutils.h>
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
#include "domx_internal.h"
#include "domx_internalcfg.h"
#include "domx_util.h"



/**
 * @name   omxrpc_skel_cb_ebd_fbd
 * @brief  OmxRpc skeleton to EmptyBufferDone and FillBufferDone callbacks
 * @param  pRpcHndl : Pointer to OmxRpc handle
 * @param  pRpcMsg  : Pointer to OmxRpc message received
 * @param  omxErr   : Pointer to OMXERRORTYPE to be populated
 * @return OmxRpc_errorNone : SUCCESS
 */
static OmxRpc_errorType omxrpc_skel_cb_ebd_fbd (OmxRpc_Handle pRpcHndl,
                                                OmxRpc_msg *pRpcMsg,
                                                OmxCore_OMX_ERRORTYPE *omxErr)
{
  OmxTypes_OMX_HANDLETYPE hComp;
  OmxCore_OMX_BUFFERHEADERTYPE *pBufferHdrRemote;
  OmxCore_OMX_BUFFERHEADERTYPE *pBufferHdrInfo;

  // Extracting the handle
  hComp = pRpcMsg->api.bufferDoneCb.pOmxComponentHandle;
  pBufferHdrRemote = pRpcMsg->api.bufferDoneCb.pBufferHdrRemote;
  pBufferHdrInfo = &pRpcMsg->api.bufferDoneCb.bufferHdrCopy;

  Log_print6 (Diags_USER1,
              "%s:  hComp : 0x%x, bufHdr : 0x%x, nFilledLen : 0x%x, nOffset: 0x%x",
              (xdc_IArg) __FUNCTION__, (xdc_IArg) hComp,
              (xdc_IArg) pBufferHdrRemote,
              (xdc_IArg) pBufferHdrInfo->nFilledLen,
              (xdc_IArg) pBufferHdrInfo->nOffset,
              (xdc_IArg) pBufferHdrInfo->nFlags);

  if (OmxRpc_cmdCbEmptyBufferDone == pRpcMsg->common.eRpcCmd)
  {
    *omxErr = pRpcHndl->layer.stub.rpcClientCb.emptyBufferDoneCb (hComp,
                                                                  pRpcHndl->
                                                                  layer.stub.
                                                                  pRpcClientCbCtx,
                                                                  pBufferHdrRemote,
                                                                  pBufferHdrInfo);
  }
  else
  {
    *omxErr = pRpcHndl->layer.stub.rpcClientCb.fillBufferDoneCb (hComp,
                                                                 pRpcHndl->
                                                                 layer.stub.
                                                                 pRpcClientCbCtx,
                                                                 pBufferHdrRemote,
                                                                 pBufferHdrInfo);
  }
  return OmxRpc_errorNone;
}

/**
 * @name   omrpc_skel_cb_event_handler
 * @brief  OmxRpc skeleton to Event Handler callback
 * @param  pRpcHndl : Pointer to OmxRpc handle
 * @param  pRpcMsg  : Pointer to OmxRpc message received 
 * @param  omxErr   : Pointer to OMXERRORTYPE to be populated
 * @return OmxRpc_errorNone : SUCCESS
 */
static OmxRpc_errorType omrpc_skel_cb_event_handler (OmxRpc_Handle pRpcHndl,
                                                     OmxRpc_msg *pRpcMsg,
                                                     OmxCore_OMX_ERRORTYPE
                                                       *omxErr)
{
  OmxRpc_errorType rpcError = OmxRpc_errorNone;
  OmxCore_OMX_EVENTTYPE eEvent;
  OmxTypes_OMX_U32 nData1;
  OmxTypes_OMX_U32 nData2;
  OmxTypes_OMX_PTR pEventData;
  OmxTypes_OMX_HANDLETYPE hComponent;

  Log_print1 (Diags_USER1, "Entered Function :%s", (xdc_IArg) __FUNCTION__);
  hComponent = pRpcMsg->api.eventCb.pOmxComponentHandle;
  eEvent = pRpcMsg->api.eventCb.eEvent;
  nData1 = pRpcMsg->api.eventCb.nData1;
  nData2 = pRpcMsg->api.eventCb.nData2;

  if (eEvent == OmxCore_OMX_CommandMarkBuffer)
  {
    pEventData = &pRpcMsg->api.eventCb.markData;
  }
  else
  {
    pEventData = NULL;
  }
  *omxErr = pRpcHndl->layer.stub.rpcClientCb.eventCb (hComponent,
                                                      pRpcHndl->layer.stub.
                                                      pRpcClientCbCtx, eEvent,
                                                      nData1, nData2,
                                                      pEventData);
  return rpcError;
}

/**
 * @name   OmxRpc_rcmIfCbEventHandler
 * @brief  OmxRpc RcmServer handler function for EventHandle Msg
 * @param  rcmMsgSize : Size of RcmMessage received
 * @param  rcmMsg     : Pointer to OmxRpc Rcm message received 
 * @return 0 : SUCCESS
 */
Int32 OmxRpc_rcmIfCbEventHandler (UInt32 rcmMsgSize, UInt32 *rcmMsg)
{
  OmxRpc_msg *pRpcMsg = NULL;
  OmxRpc_Handle rpcStubHandle = NULL;
  OmxRpc_errorType retVal;
  OmxCore_OMX_ERRORTYPE omxErr = OmxCore_OMX_ErrorNone;

  DOMX_UTL_TRACE_FUNCTION_ASSERT ((rcmMsgSize == sizeof (OmxRpc_msg)), 
                                  "rcmMsgSize incorrect");
  pRpcMsg = (OmxRpc_msg *) (rcmMsg);
  rpcStubHandle = pRpcMsg->common.pRpcRemoteHandle;
  retVal = omrpc_skel_cb_event_handler (rpcStubHandle, pRpcMsg, &omxErr);
  OmxRpc_msgMarshallCommonResponse (&pRpcMsg->common,
                                    rpcStubHandle,
                                    rpcStubHandle->layer.stub.skelHandle,
                                    pRpcMsg->common.eRpcCmd, omxErr, retVal);
  return retVal;
}

/**
 * @name   OmxRpc_rcmIfCbEmptyFillBufferDone
 * @brief  OmxRpc RcmServer handler function for Empty and Fill Buffer done Msg
 * @param  rcmMsgSize : Size of RcmMessage received
 * @param  rcmMsg     : Pointer to OmxRpc Rcm message received 
 * @return 0 : SUCCESS
 */
Int32 OmxRpc_rcmIfCbEmptyFillBufferDone (UInt32 rcmMsgSize, UInt32 *rcmMsg)
{
  OmxRpc_msg *pRpcMsg = NULL;
  OmxRpc_Handle rpcStubHandle = NULL;
  OmxRpc_errorType retVal;
  OmxCore_OMX_ERRORTYPE omxErr = OmxCore_OMX_ErrorNone;

  DOMX_UTL_TRACE_FUNCTION_ASSERT ((rcmMsgSize == sizeof (OmxRpc_msg)), 
                                  "rcmMsgSize incorrect");
  pRpcMsg = (OmxRpc_msg *) (rcmMsg);
  rpcStubHandle = pRpcMsg->common.pRpcRemoteHandle;
  retVal = omxrpc_skel_cb_ebd_fbd (rpcStubHandle, pRpcMsg, &omxErr);
  OmxRpc_msgMarshallCommonResponse (&pRpcMsg->common,
                                    rpcStubHandle,
                                    rpcStubHandle->layer.stub.skelHandle,
                                    pRpcMsg->common.eRpcCmd, omxErr, retVal);
  return retVal;
}

/* Nothing beyond this point */
