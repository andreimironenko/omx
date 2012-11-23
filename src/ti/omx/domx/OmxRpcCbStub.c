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
 *  @file  OmxRpcCbStub.c
 *         DOMX RPC Callback skeleton functionality
 *
 *  @path \avmecomp\ti\omx\domx\OmxRpcCbStub.c
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

#include <xdc/runtime/Log.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Gate.h>
#include <xdc/runtime/Startup.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/IGateProvider.h>
#include <xdc/runtime/Gate.h>
#include <xdc/runtime/knl/GateThread.h>
#include <ti/sdo/rcm/RcmServer.h>
#include <ti/sdo/rcm/RcmClient.h>
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
 * @name   omxrpc_msg_marshall_cbevent
 * @brief  Function to marshall the arguments of OMX Callback event into a msg
 * @param  pEventCbMsg : Pointer to OmxRpc message
 * @param  hComponent  : Handle to the component
 * @param  eEvent      : Callback event enum
 * @param  nData1      : Data1 associaed with Callback event
 * @param  nData2      : Data2 associaed with Callback event
 * @param  pEventData  : Pointer to event data
 * @return None
 */
static Void omxrpc_msg_marshall_cbevent (OmxRpc_msgEventCb *pEventCbMsg,
                                         OmxTypes_OMX_HANDLETYPE hComponent,
                                         OmxCore_OMX_EVENTTYPE eEvent,
                                         OmxTypes_OMX_U32 nData1,
                                         OmxTypes_OMX_U32 nData2,
                                         OmxTypes_OMX_PTR pEventData)
{
  /* 
   * Note: pAppData is not marshalled although this was done in original
   * prototype code SYS_printf(" DSP: RPC_EventHandler : 0%x\n",pAppData);
   * memcpy(msgBody+offset,&pAppData,sizeof(OMX_PTR)); offset+=sizeof(OMX_PTR); */
  pEventCbMsg->pOmxComponentHandle = hComponent;
  pEventCbMsg->eEvent = eEvent;
  pEventCbMsg->nData1 = nData1;
  pEventCbMsg->nData2 = nData2;
  if (eEvent == OmxCore_OMX_CommandMarkBuffer)
  {
    pEventCbMsg->markData = *((OmxTypes_OMX_MARKTYPE *) pEventData);
  }
}

/**
 * @name   omxrpc_msg_marshall_ebd_fbd
 * @brief  Function to marshall the arguments of Empty buffer done and Fill Buffer done callback
 * @param  pBufferDoneCbMsg : Pointer to BufferDone Callback message
 * @param  hComponent  : Handle to the component
 * @param  pBuffer     : Pointer to OMX buffer header
 * @return None
 */
static Void omxrpc_msg_marshall_ebd_fbd (OmxRpc_msgBufferDoneCb
                                           *pBufferDoneCbMsg,
                                         OmxTypes_OMX_HANDLETYPE hComponent,
                                         OmxCore_OMX_BUFFERHEADERTYPE * pBuffer)
{
  /* Note: pAppData is not marshalled although this was done in original
     prototype code //pAppData Contains real component on MPU side //This
     information was passed to Component during the GetHandle Call
     memcpy(msgBody+offset,&pAppData,sizeof(OMX_PTR)); offset+=sizeof(OMX_PTR); 
   */
  pBufferDoneCbMsg->pOmxComponentHandle = hComponent;
  pBufferDoneCbMsg->pBufferHdrRemote = pBuffer;
  pBufferDoneCbMsg->bufferHdrCopy = *pBuffer;
}

/**
 * @name   OmxRpc_stubCbEventHandler
 * @brief  OmxRpc stub function to handle OMX event callback
 * @param  hComponent  : Handle to the component
 * @param  pAppData    : Pointer to appData associated with the event
 * @param  eEvent      : OMX Callback event enumerator
 * @param  nData1      : Data1 associated with callback event
 * @param  nData2      : Data2 associated with callback event 
 * @param  pEventData  : Pointer to event data
 * @return OMX_ErrorNone on success
 */
OMX_ERRORTYPE OmxRpc_stubCbEventHandler (OMX_HANDLETYPE hComponent,
                                         OMX_PTR pAppData,
                                         OMX_EVENTTYPE eEvent,
                                         OMX_U32 nData1, OMX_U32 nData2,
                                         OMX_PTR pEventData)
{
  OmxRpc_Handle pRpcHndl;
  RcmClient_Message *rcmMsg;
  OmxRpc_msg *pRpcMsg;
  OMX_ERRORTYPE omxErr = OMX_ErrorNone;
  OmxRpc_errorType rpcErr = OmxRpc_errorNone;
  Bool msgAllocDone = FALSE;

  pRpcHndl = (OmxRpc_Handle) pAppData;
  /* Ensure component pAppData points to the OmxRpcSkel handle by reference to
     pComponentHandle maintained by rpcSkelHandle matches the hComponent passed 
     * by component */
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((pRpcHndl->layer.skel.pComponentHandle ==
                                   hComponent), "compHandle incorrect");
  rpcErr = OmxRpc_rcmMsgAlloc ((RcmClient_Handle) pRpcHndl->client.handle,
                               &rcmMsg, __FILE__, __LINE__);
  if (OmxRpc_errorNone == rpcErr)
  {
    UInt32 fxnId;
    OmxRpc_cmd cmdId = OmxRpc_cmdCbEventHandler;

    msgAllocDone = TRUE;
    pRpcMsg = (OmxRpc_msg *) (&rcmMsg->data);
    /* Packing GetHandle messages */
    /* Filling common part of the message */
    OmxRpc_msgMarshallCommonStub (pRpcMsg,
                                  pRpcHndl,
                                  pRpcHndl->layer.skel.stubHandle, cmdId);
    omxrpc_msg_marshall_cbevent (&pRpcMsg->api.eventCb,
                                 hComponent, (OmxCore_OMX_EVENTTYPE) eEvent,
                                 nData1, nData2, pEventData);

    fxnId = pRpcHndl->client.fxns.cmd2fxnIdxMap[cmdId];
    rpcErr = OmxRpc_rcmExec ((RcmClient_Handle) pRpcHndl->client.handle,
                             rcmMsg, fxnId);
    if (OmxRpc_errorNone == rpcErr)
    {
      OmxRpc_msgUnmarshallCommonStub (&pRpcMsg->common,
                                      (OmxCore_OMX_ERRORTYPE *) & omxErr);
    }
  }                             /* end of if (rcmMsgAlloc success) */
  if (msgAllocDone)
  {
    /* If msg was successfully executed free return the Rcm Msg to heap */
    RcmClient_free ((RcmClient_Handle) (pRpcHndl->client.handle), rcmMsg);
  }
  return omxErr;
}

/**
 * @name   omxrpc_ebd_fbd
 * @brief  OmxRpc stub function to handle emtpy buffer done and fill buffer done callbacks
 * @param  hComponent  : Handle to the component
 * @param  pAppData    : Pointer to appData associated with the event
 * @param  pBuffer     : Pointer to OMX buffer
 * @param  cmdId       : CommandID used to differentiate empty buffer done and fill buffer done
 * @return OMX_ErrorNone on success
 */
static OmxCore_OMX_ERRORTYPE omxrpc_ebd_fbd (OmxTypes_OMX_HANDLETYPE hComponent,
                                             OmxTypes_OMX_PTR pAppData,
                                             OmxCore_OMX_BUFFERHEADERTYPE 
                                               *pBuffer, OmxRpc_cmd cmdId)
{
  OmxRpc_Handle pRpcHndl;
  RcmClient_Message *rcmMsg;
  OmxRpc_msg *pRpcMsg;
  OmxCore_OMX_ERRORTYPE omxErr = OmxCore_OMX_ErrorNone;
  OmxRpc_errorType rpcErr = OmxRpc_errorNone;
  Bool msgAllocDone = FALSE;

  pRpcHndl = (OmxRpc_Handle) pAppData;
  /* Ensure component pAppData points to the OmxRpcSkel handle by reference to
     pComponentHandle maintained by rpcSkelHandle matches the hComponent passed 
     * by component */
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((pRpcHndl->layer.skel.pComponentHandle ==
                                   hComponent), "compHandle incorrect");
  rpcErr = OmxRpc_rcmMsgAlloc ((RcmClient_Handle) pRpcHndl->client.handle,
                               &rcmMsg, __FILE__, __LINE__);
  if (OmxRpc_errorNone == rpcErr)
  {
    UInt32 fxnId;

    msgAllocDone = TRUE;
    pRpcMsg = (OmxRpc_msg *) (&rcmMsg->data);
    /* Packing GetHandle messages */
    /* Filling common part of the message */
    OmxRpc_msgMarshallCommonStub (pRpcMsg,
                                  pRpcHndl,
                                  pRpcHndl->layer.skel.stubHandle, cmdId);
    omxrpc_msg_marshall_ebd_fbd (&pRpcMsg->api.bufferDoneCb,
                                 hComponent, pBuffer);

    fxnId = pRpcHndl->client.fxns.cmd2fxnIdxMap[cmdId];
    rpcErr = OmxRpc_rcmExec ((RcmClient_Handle) pRpcHndl->client.handle,
                             rcmMsg, fxnId);
    if (OmxRpc_errorNone == rpcErr)
    {
      OmxRpc_msgUnmarshallCommonStub (&pRpcMsg->common, &omxErr);
    }
  }                             /* end of if (rcmMsgAlloc success) */
  if (msgAllocDone)
  {
    /* If msg was successfully executed free return the Rcm Msg to heap */
    RcmClient_free ((RcmClient_Handle) pRpcHndl->client.handle, rcmMsg);
  }
  return omxErr;
}

/**
 * @name   OmxRpc_stubCbEmptyBufferDone
 * @brief  OmxRpc stub function to handle emtpy buffer done callbacks
 * @param  hComponent  : Handle to the component
 * @param  pAppData    : Pointer to appData associated with the event
 * @param  pBuffer     : Pointer to OMX buffer
 * @return OMX_ErrorNone on success
 */
OMX_ERRORTYPE OmxRpc_stubCbEmptyBufferDone (OMX_HANDLETYPE hComponent,
                                            OMX_PTR pAppData,
                                            OMX_BUFFERHEADERTYPE *pBufHdr)
{
  /* calling cache invalidation */
#ifdef _LOCAL_CORE_c6xdsp_
  MEMUTILS_cacheWb (pBufHdr->pBuffer,pBufHdr->nAllocLen);
#endif
  return (OMX_ERRORTYPE) (omxrpc_ebd_fbd (hComponent, pAppData,
                                          (OmxCore_OMX_BUFFERHEADERTYPE *)
                                          pBufHdr,
                                          OmxRpc_cmdCbEmptyBufferDone));
}

/**
 * @name   OmxRpc_stubCbFillBufferDone
 * @brief  OmxRpc stub function to handle emtpy buffer done callbacks
 * @param  hComponent  : Handle to the component
 * @param  pAppData    : Pointer to appData associated with the event
 * @param  pBuffer     : Pointer to OMX buffer
 * @return OMX_ErrorNone on success
 */
OMX_ERRORTYPE OmxRpc_stubCbFillBufferDone (OMX_HANDLETYPE hComponent,
                                           OMX_PTR pAppData,
                                           OMX_BUFFERHEADERTYPE *pBufHdr)
{
  /* Need to do cache invalidation of the buffer */
#ifdef _LOCAL_CORE_c6xdsp_
  MEMUTILS_cacheWb(pBufHdr->pBuffer,pBufHdr->nAllocLen);
#endif
  /* DEBUG_DUMP_BYTES(pBuffer->pBuffer,32,counter); */
  return (OMX_ERRORTYPE) (omxrpc_ebd_fbd (hComponent, pAppData,
                                          (OmxCore_OMX_BUFFERHEADERTYPE *)
                                          pBufHdr, OmxRpc_cmdCbFillBufferDone));
}

/* Nothing beyond this point */
