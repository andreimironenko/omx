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
 *  @file  OmxRpcStubCommon.c
 *         DOMX RPC stub functionality common to Cb and control
 *
 *  @path \avmecomp\ti\omx\domx\OmxRpcStubCommon.c
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
#include "domx_internal.h"
#include "domx_internalcfg.h"
#include "domx_util.h"



/**
 * @name   OmxRpc_rcmMsgAlloc
 * @brief  Function to allocate an rcmMsg
 * @param  rcmHndl                  : RcmClient handle
 * @param  rcmMsgPtr                : Pointer to RcmMsg to be populated
 * @param  fileName                 : FIle name from where alloc msg was called
 * @param  line_num                 : Linenum in file where allocMsg was called
 * @return OmxRpc_errorNone on success
 */
OmxRpc_errorType OmxRpc_rcmMsgAlloc (RcmClient_Handle rcmHndl,
                                     RcmClient_Message **rcmMsgPtr,
                                     String fileName, Int line_num)
{
  Int status;
  UInt16 rcmMsgSize;
  OmxRpc_errorType retVal = OmxRpc_errorNone;

  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL2 ();

  /* allocate a remote command message */
  rcmMsgSize = sizeof (OmxRpc_msg);
  DOMX_UTL_TRACE_FUNCTION_MSG_LEVEL2 ("Before RcmClient_alloc");
  status = RcmClient_alloc (rcmHndl, rcmMsgSize, rcmMsgPtr);
  DOMX_UTL_TRACE_FUNCTION_MSG_LEVEL2 ("After RcmClient_alloc");
  if (RcmClient_S_SUCCESS != status)
  {
    retVal = OmxRpc_errorResourceAllocFail;
  }
  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL2 (retVal);
  return retVal;
}

/**
 * @name   OmxRpc_rcmExec
 * @brief  Function to execute an Rcm msg
 * @param  rcmHndl                  : RcmClient handle
 * @param  rcmMsg                   : Pointer to RcmMsg
 * @param  fxnIdx                   : Function index of remote function to be execed
 * @return OmxRpc_errorNone on success
 */
OmxRpc_errorType OmxRpc_rcmExec (RcmClient_Handle rcmHndl,
                                 RcmClient_Message *rcmMsg, UInt32 fxnIdx)
{
  OmxRpc_errorType retVal = OmxRpc_errorNone;
  Int status;
  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL2 ();
  Log_print1 (Diags_USER2, "Rcm_Exec for fxnId:%d", fxnIdx);
  /* fill in the remote command message */
  rcmMsg->fxnIdx = fxnIdx;
  rcmMsg->result = OmxRpc_errorNone;

  /* execute the remote command message */
  DOMX_UTL_TRACE_FUNCTION_MSG_LEVEL2 ("Before RcmClient_exec");
  status = RcmClient_exec (rcmHndl, rcmMsg, &rcmMsg);
  DOMX_UTL_TRACE_FUNCTION_MSG_LEVEL2 ("After RcmClient_exec");

  if (RcmClient_S_SUCCESS != status)
  {
    retVal = OmxRpc_errorRcmExecFailed;
    DOMX_UTL_TRACE_FUNCTION_ERROR ((retVal == OmxRpc_errorNone),
                                   "RcmClient_exec failed");
  }
  else
  {
    /* If exec cmd was successful, assign retVal from the rcmMsg result which
       is populated by remote skeleton */
    retVal =
      (OmxRpc_errorType) ((OmxRpc_msg *) (&(rcmMsg->data)))->common.
      nRPCCmdStatus;
  }
  Log_print1 (Diags_USER2, "result = %d", rcmMsg->result);
  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL2 (retVal);
  return retVal;
}

/**
 * @name   OmxRpc_msgMarshallCommonStub
 * @brief  Function to marshall commonMsg fields of OmxRpc msg
 * @param  pRpcMsg                  : Pointer to OmxRPc message
 * @param  pRpcLocalHndl            : Pointer to local RPC handle
 * @param  pRpcRemoteHndl           : Pointer to remote RPC handle
 * @param  eRpcCmd                  : OmxRpc cmd ID
 * @return None
 */
Void OmxRpc_msgMarshallCommonStub (OmxRpc_msg * pRpcMsg,
                                   OmxRpc_Handle pRpcLocalHndl,
                                   OmxRpc_Handle pRpcRemoteHndl,
                                   OmxRpc_cmd eRpcCmd)
{
  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL2 ();
  pRpcMsg->common.nOMXReturn = (OmxCore_OMX_ERRORTYPE) OMX_ErrorNone;
  pRpcMsg->common.pRpcLocalHandle = pRpcLocalHndl;
  pRpcMsg->common.pRpcRemoteHandle = pRpcRemoteHndl;
  pRpcMsg->common.nRPCCmdStatus = OmxRpc_errorNone;
  pRpcMsg->common.eRpcCmd = eRpcCmd;
  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL2 (OMX_ErrorNone);
}

/**
 * @name   OmxRpc_msgUnmarshallCommonStub
 * @brief  Function to unmarshall commonMsg fields of OmxRpc msg
 * @param  pRpcMsgCommon            : Pointer to OmxRPc message (common part)
 * @param  nCmdStatus               : Pointer to Command status to be unmarsahlled
 * @return None
 */
Void OmxRpc_msgUnmarshallCommonStub (OmxRpc_msgCommon *pRpcMsgCommon,
                                     OmxCore_OMX_ERRORTYPE *nCmdStatus)
{
  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL2 ();
  *nCmdStatus = pRpcMsgCommon->nOMXReturn;
  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL2 (OMX_ErrorNone);
}

/* Nothing beyond this point */
