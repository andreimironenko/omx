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
 *  @file  DomxCore.c
 *         This file contains methods that provides the functionality for
 *         the OpenMAX1.1 DOMX Framework DomxCore.
 *
 *  @path \avmecomp\ti\omx\domx\DomxCore.c
 *
 *  @rev 1.0
 */

/******************************************************************
 *   INCLUDE FILES
 ******************************************************************/

/* ----- system and platform files ----------------------------*/

#include <xdc/std.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Gate.h>
#include <xdc/runtime/Startup.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/IGateProvider.h>
#include <xdc/runtime/Gate.h>
#include <xdc/runtime/knl/GateThread.h>
#include <xdc/runtime/knl/Thread.h>
#include <ti/omx/interfaces/openMaxv11/OMX_Core.h>
#include <ti/ipc/MultiProc.h>
#include <ti/omx/interfaces/openMaxv11/interfaces_openmaxv11_internal.h>
#include <ti/sdo/rcm/RcmServer.h>
#include <ti/sdo/rcm/RcmClient.h>

/* Log header files */
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Diags.h>

/*-------program files ----------------------------------------*/
#include "domx_internal.h"
#include "domx_internalcfg.h"

#include "domx_util.h"

/* **************************** MACROS DEFINES *************************** */

/* ******************************* EXTERNS ********************************* */
extern OmxRpc_Module_State OmxRpc_module_inst;

/* ******************************* GLOBALS ********************************* */

/* ************************* EXTERNS, FUNCTION DECLARATIONS ***************************** */

/*
 *************************************************************************
 *                       Module functions
 *************************************************************************
 */
/* ===========================================================================*/
/**
 * @name DomxCore_Module_startup()
 * @brief DomxCore module startup is used to for initializing the DomxCore Module level info at init time
 * @param state  : State of startup (Whether already done or not)
 * @return Startup_DONE = Successful
 */
Int DomxCore_Module_startup (Int state)
{
  Int i;

  if (state == Startup_DONE)
  {
    return (Startup_DONE);
  }
  for (i = 0; i < DomxTypes_coreCount; i++)
  {
    DomxCore_module->multiProcState[i].procMgrFileId = 0;
    DomxCore_module->multiProcState[i].procMgrHandle = NULL;
  }
  DomxCore_module->initDone = FALSE;
  return (Startup_DONE);
}

/**
 * @name DomxInit()
 * @brief DomxCore Init is used to performing the OmxRpc and IPC initialization
 *        This should be invoked before invoking any other DomxAPI.
 *        
 * @param none
 * @return OMX_ErrorNone = Successful
 */
OMX_ERRORTYPE DomxInit ()
{
  Error_Block ebObj;
  Error_Block *eb = &ebObj;
  OmxRpc_errorType rpcError;
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  Int i;

  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();

  Error_init (eb);

  RcmServer_init ();
  RcmClient_init ();

  memset ((void *) &OmxRpc_module_inst, 0, sizeof (OmxRpc_Module_State));
  OmxRpc_module->remoteCoreRcmClient[0].msgHeap.msgHeapId =
    OmxRpc_msgHeapId_core0;
  OmxRpc_module->remoteCoreRcmClient[1].msgHeap.msgHeapId =
    OmxRpc_msgHeapId_core1;
  OmxRpc_module->remoteCoreRcmClient[2].msgHeap.msgHeapId =
    OmxRpc_msgHeapId_core2;
  OmxRpc_module->remoteCoreRcmClient[3].msgHeap.msgHeapId =
    OmxRpc_msgHeapId_core3;

  memset ((void *) DomxTunnelMgr_connectionInfoArr, 0,
          sizeof (DomxTunnelMgr_connectionInfoAry) *
          DomxTunnelMgr_maxNumComponents);
  for (i = 0; i < DomxTunnelMgr_maxNumComponents; i++)
  {
    DomxTunnelMgr_connectionInfoArr[i][0].eCompCoreId = DomxTypes_coreCount;
  }

  if (OMX_ErrorNone == eError)
  {
    DomxCore_procInit (eb);
    if (Error_check (eb))
    {
      Log_print3 (Diags_USER1, "%s:%d:: DomxCore_procInit failed"
                  "Error_Id = %s",
                  (xdc_IArg) __FUNCTION__, (xdc_IArg) __LINE__,
                  (xdc_IArg) Error_getMsg (eb));
      eError = OMX_ErrorUndefined;
    }
  }

  if (MultiProc_self () == DomxTypes_coreCortexA8)
  {
    DOMX_UTL_TRACE_FUNCTION_MSG
      ("Waiting for Ipc_attach to happen b/w slave cores");
    Thread_sleep (DOMX_INT_THREADSLEEP_1_SEC, eb);
    DOMX_UTL_TRACE_FUNCTION_MSG
      ("Wait completed for Ipc_attach to happen b/w slave cores");
  }

  if (OMX_ErrorNone == eError)
  {
    rpcError = OmxRpc_moduleInitServer ();
    if (OmxRpc_errorNone != rpcError)
    {
      DOMX_UTL_TRACE_FUNCTION_MSG ("OmxRpc_moduleInitServer failed");
      eError = OMX_ErrorUndefined;
      DomxCore_procDeInit (eb);
      if (Error_check (eb))
      {
        Log_print3 (Diags_USER1,
                    "%s:%d:: Rolling back..DomxCore_procDeInit failed"
                    "Error_Id = %s", (xdc_IArg) __FUNCTION__,
                    (xdc_IArg) __LINE__, (xdc_IArg) Error_getMsg (eb));
        eError = OMX_ErrorUndefined;
      }
    }
  }

  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (eError);
  return eError;
}

/**
 * @name DomxDeInit()
 * @brief DomxCore Init is used to performing the OmxRpc and IPC de-initialization
 *        This should be the last DomxAPI invoked.
 *        
 * @param none
 * @return OMX_ErrorNone = Successful
 */
OMX_ERRORTYPE DomxDeInit ()
{
  Error_Block ebObj;
  Error_Block *eb = &ebObj;
  OmxRpc_errorType rpcError;
  OMX_ERRORTYPE eError = OMX_ErrorNone;

  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();

  Error_init (eb);

  rpcError = OmxRpc_moduleDeinitServer ();
  if (OmxRpc_errorNone != rpcError)
  {
    Log_print3 (Diags_USER1, "%s:%d:: OmxRpc_moduleDeinitServer failed"
                "RpcErrorCode = %d",
                (xdc_IArg) __FUNCTION__, __LINE__, rpcError);
    eError = OMX_ErrorUndefined;
  }
  if (OMX_ErrorNone == eError)
  {
    DomxCore_procDeInit (eb);
    if (Error_check (eb))
    {
      Log_print3 (Diags_USER1, "%s:%d:: DomxCore_procInit failed"
                  "Error_Id = %s",
                  (xdc_IArg) __FUNCTION__, (xdc_IArg) __LINE__,
                  (xdc_IArg) Error_getMsg (eb));
      eError = OMX_ErrorUndefined;
    }
  }

  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (eError);

  return eError;
}

/**
 * @name DomxCore_mapDomxCore2MultiProcId()
 * @brief Maps DomxCoreId passed into a IPC multiProcID
 *        
 * @param coreId: DomxCOreId to be mapped
 * @param eb: Error Block used to raise error
 * @return Mapped MultiProcId.If not able to ma returns MultiProc_INVALIDID
 */
UInt16 DomxCore_mapDomxCore2MultiProcId (DomxTypes_coreType coreId,
                                         Error_Block *eb)
{
  UInt16 id = MultiProc_INVALIDID;
  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();

  id = DomxCore_ipcInfoTbl[coreId].multiProcId;
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((MultiProc_INVALIDID != id),
                                  "Mapping to MultiProcId Failed");

  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (OMX_ErrorNone);
  return (id);
}

/**
 * @name DomxCore_mapComponentName2CoreId()
 * @brief Maps component name passed into its corresponding native DomxCoreID
 *        
 * @param component_name: OMX Component name
 * @param eb: Error Block used to raise error
 * @return Mapped DomxCoreId.If not able to ma returns DomxTypes_coreInvalid
 */
DomxTypes_coreType DomxCore_mapComponentName2CoreId (String component_name, 
                                                     Error_Block *eb)
{
  Int i;
  DomxCore_componentCoreInfoEntry *info;

  info = NULL;
  for (i = 0; i < DomxCore_componentCoreInfoTbl.length; i++)
  {
    if (strcmp ((DomxCore_componentCoreInfoTbl.elem[i][0]).name,
                component_name) == 0)
    {
      info = &(DomxCore_componentCoreInfoTbl.elem[i][0]);
      break;
    }
  }
  if (i == DomxCore_componentCoreInfoTbl.length)
  {
    DOMX_UTL_TRACE_FUNCTION_ASSERT ((i != DomxCore_componentCoreInfoTbl.length), 
                                    "Component not found in core info table");
    return DomxTypes_coreInvalid;
  }
  else
  {
    if (info != NULL) {
     return (info->coreId);
    }
    else {
     return DomxTypes_coreInvalid;
    }
     
  }
}

/* Nothing beyond this point */
