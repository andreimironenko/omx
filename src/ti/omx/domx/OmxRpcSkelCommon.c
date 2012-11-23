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
 *  @file  OmxRpcSkelCommon.c
 *         DOMX RPC skeleton functionality common to Cb and control
 *
 *  @path \avmecomp\ti\omx\domx\OmxRpcSkelCommon.c
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
#include "domx_util.h"



/**
 * @name   OmxRpc_msgMarshallCommonResponse
 * @brief  Function to marshall the response of OmxRpc skeleton functions 
 * @param  pRpcCommonMsg            : Pointer to OmxRpc msg (common portion)
 * @param  pRpcLocalHndl            : Local Rpc Handle
 * @param  pRpcRemoteHndl           : Remote RPC handle
 * @param  eRpcCmd                  : OmxRpc command enumerator
 * @param  nOmxReturnCode           : OMX API error code to be returned
 * @param  nRpcCmdStatus            : OmxRpc command execution status
 * @return None
 */
Void OmxRpc_msgMarshallCommonResponse (OmxRpc_msgCommon *pRpcCommonMsg,
                                       OmxRpc_Handle pRpcLocalHndl,
                                       OmxRpc_Handle pRpcRemoteHndl,
                                       UInt32 eRpcCmd,
                                       OmxCore_OMX_ERRORTYPE nOmxReturnCode,
                                       OmxRpc_errorType nRpcCmdStatus)
{
  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL2 ();

  pRpcCommonMsg->nOMXReturn = (OmxCore_OMX_ERRORTYPE) nOmxReturnCode;
  pRpcCommonMsg->pRpcLocalHandle = pRpcLocalHndl;
  pRpcCommonMsg->pRpcRemoteHandle = pRpcRemoteHndl;
  pRpcCommonMsg->nRPCCmdStatus = nRpcCmdStatus;
  pRpcCommonMsg->eRpcCmd = eRpcCmd;

  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL2 (OMX_ErrorNone);
}

/* Nothing beyond this point */
