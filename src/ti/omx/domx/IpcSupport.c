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
 *  @file  IpcSupport.c
 *         This file contains methods that provides the functionality for
 *         interfacing with ProcMgr
 *
 *  @path \avmecomp\ti\omx\domx\IpcSupport.c
 *
 *  @rev 1.0
 */

 /******************************************************************
 *   INCLUDE FILES
 ******************************************************************/
/* xdctools header files */
#include <stdio.h>
#include <unistd.h>
#include <xdc/std.h>
#include <xdc/cfg/global.h>

/* syslink header files */
#include <ti/syslink/IpcHost.h>
#include <ti/syslink/ProcMgr.h>
#include <ti/ipc/MultiProc.h>

/* local header files */
#include <ti/omx/interfaces/openMaxv11/interfaces_openmaxv11_internal.h>
#include <ti/omx/interfaces/openMaxv11/OMX_Core.h>
#include "domx_util.h"

#include "domx_internal.h"
#include "domx_internalcfg.h"




#include "IpcSupport.h"
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
    #ifndef SLOG_DOMXIPC_Module__MID
    #define SLOG_DOMXIPC_Module__MID SLOG_DOMX_Module__MID
    #endif
    #undef Module__MID
    #define Module__MID (SLOG_DOMXIPC_Module__MID)
  #else
    #include <xdc/runtime/Log.h>
    #include <xdc/runtime/Assert.h>
    #include <xdc/runtime/Diags.h>
  #endif /* End Of #if (USE_SLOG_PRINT == 1) */
#endif /* End of #else */

static Char *procState[] =
{
  "Unknown", "Powered", "Reset", "Loaded", "Running"
};

/**
 * @name ipc_initialize
 * @brief Do ProcLoad and ProcStart of particular slave core
 * @param  filePath         : Path to slave executable to be loaded
 * @param  coreId           : Domx CoreID of slave core
 * @param  procMgrHandlePtr : Handle to slave processor to be initialized
 * @param  multiProcId      : MultiProcID corresponding to coreId
 * @param  procMgrFileIdPtr : ProcMgr fileID pointer
 * @param  eb               : Used to raise error 
 * @return none
 */
Void ipc_initialize (String filePath,
                     DomxTypes_coreType coreId,
                     ProcMgr_Handle *procMgrHandlePtr,
                     UInt16 *multiProcId,
                     UInt32 *procMgrFileIdPtr, Error_Block *eb)
{
  Int status;
  UInt16 procId;
  ProcMgr_AttachParams procMgrAttachParams;
  ProcMgr_StartParams procMgrStartParams;
  ProcMgr_State state;

  Log_print1 (Diags_ENTRY, "Entering %s: (void)\n", (xdc_IArg) __FUNCTION__);

  Log_print1 (Diags_USER1, "filePath = %s", (IArg) filePath);

  /* TODO comment */
  procId = DomxCore_mapDomxCore2MultiProcId (coreId, eb);
  *multiProcId = procId;
  if (MultiProc_INVALIDID == procId)
  {
    Error_raise (eb, Error_E_generic,
                 "ipc_initialize(), error in DomxCore_mapDomxCore2MultiProcId():%d",
                 coreId);
    goto leave;
  }

  Log_print0 (Diags_USER1, "Doing ProcMgr_open()");
  status = ProcMgr_open (procMgrHandlePtr, procId);
  if (0 > status)
  {
    Error_raise (eb, Error_E_generic,
                 "ipc_initialize(), error in ProcMgr_open()", 0);
    goto leave;
  }

  state = ProcMgr_getState (*procMgrHandlePtr);
  Log_print1 (Diags_USER1, "Processor state is %s", (IArg) procState[state]);

  if (DomxCore_doProcInit)
  {
    Log_print0 (Diags_USER1, "Doing ProcMgr_attach()");
    ProcMgr_getAttachParams (NULL, &procMgrAttachParams);
    status = ProcMgr_attach (*procMgrHandlePtr, &procMgrAttachParams);

    if (0 > status)
    {
      Error_raise (eb, Error_E_generic,
                   "ipc_initialize(), error in ProcMgr_attach()", 0);
      goto leave;
    }

    state = ProcMgr_getState (*procMgrHandlePtr);
    Log_print1 (Diags_USER1, "Processor state is %s", (IArg) procState[state]);

    Log_print0 (Diags_USER1, "Doing ProcMgr_load()");
    Log_print1 (Diags_USER1, "Loading Image %s\n", (IArg) filePath);
    status = ProcMgr_load (*procMgrHandlePtr, filePath,
                           0, NULL, NULL, procMgrFileIdPtr);

    if (0 > status)
    {
      Error_raise (eb, Error_E_generic,
                   "ipc_initialize(), error in ProcMgr_load()", 0);
      goto leave;
    }

    state = ProcMgr_getState (*procMgrHandlePtr);
    Log_print1 (Diags_USER1, "Processor state is %s", (IArg) procState[state]);

    Log_print0 (Diags_USER1, "Doing Ipc_CONTROLCMD_LOADCALLBACK()");

    status = Ipc_control (procId, Ipc_CONTROLCMD_LOADCALLBACK, NULL);
    if (status < 0)
    {
      Error_raise (eb, Error_E_generic,
                   "ipc_initialize(), error in Ipc_control LCB()", 0);
      goto leave;
    }

    state = ProcMgr_getState (*procMgrHandlePtr);
    Log_print1 (Diags_USER1, "Processor state is %s", (IArg) procState[state]);

    ProcMgr_getStartParams (*procMgrHandlePtr, &procMgrStartParams);

    Log_print0 (Diags_USER1, "Doing ProcMgr_start()");
    status = ProcMgr_start (*procMgrHandlePtr, &procMgrStartParams);

    if (0 > status)
    {
      Error_raise (eb, Error_E_generic,
                   "ipc_initialize(), error in ProcMgr_start()", 0);
      goto leave;
    }

    state = ProcMgr_getState (*procMgrHandlePtr);
    Log_print1 (Diags_USER1, "Processor state is %s", (IArg) procState[state]);

    usleep (3 * DOMX_INT_THREADSLEEP_1_SEC);

    Log_print0 (Diags_USER1, "Doing Ipc_CONTROLCMD_STARTCALLBACK()");
    status = Ipc_control (procId, Ipc_CONTROLCMD_STARTCALLBACK, NULL);
    if (status < 0)
    {
      Error_raise (eb, Error_E_generic,
                   "ipc_initialize(), error in Ipc_control LCB()", 0);
      goto leave;
    }

    state = ProcMgr_getState (*procMgrHandlePtr);
    Log_print1 (Diags_USER1, "Processor state is %s", (IArg) procState[state]);
  }                             /* if (DomxCore_doProcInit) */
leave:
  if (Error_check (eb))
  {
    Error_raise (eb, Error_E_generic, "ipc_initialize() failed", 0);
  }

  Log_print1 (Diags_EXIT, "Exiting %s: (void)\n", (xdc_IArg) __FUNCTION__);
}

/**
 * @name ipc_finalize
 * @brief Do ProcStop on slave core
 * @param  procMgrHandle    : Handle to slave processor to be finalized
 * @param  multiProcId      : MultiProcID to be finalized
 * @param  procMgrFileId    : ProcMgr fileID
 * @param  eb               : Used to raise error 
 * @return none
 */
Void ipc_finalize (ProcMgr_Handle procMgrHandlePtr,
                   UInt16 multiProcId, UInt32 procMgrFileId, Error_Block *eb)
{
  Int status;
  ProcMgr_State state;

  Log_print1 (Diags_ENTRY, "Entering %s: (void)\n", (xdc_IArg) __FUNCTION__);

  if (DomxCore_doProcInit)
  {
    Log_print0 (Diags_USER1, "Doing Ipc_control SCB()");

    status = Ipc_control (multiProcId, Ipc_CONTROLCMD_STOPCALLBACK, NULL);
    if (0 > status)
    {
      Error_raise (eb, Error_E_generic,
                   "ipc_finalize(), error in Ipc_control SCB()", 0);
    }

    state = ProcMgr_getState (procMgrHandlePtr);
    Log_print1 (Diags_USER1, "Processor state is %s", (IArg) procState[state]);

    Log_print0 (Diags_USER1, "Doing ProcMgr_stop()");
    status = ProcMgr_stop (procMgrHandlePtr);

    if (0 > status)
    {
      Error_raise (eb, Error_E_generic,
                   "ipc_finalize(), error in ProcMgr_stop()", 0);
      goto leave;
    }

    state = ProcMgr_getState (procMgrHandlePtr);
    Log_print1 (Diags_USER1, "Processor state is %s", (IArg) procState[state]);

    Log_print0 (Diags_USER1, "Doing ProcMgr_unload()");
    status = ProcMgr_unload (procMgrHandlePtr, procMgrFileId);

    if (0 > status)
    {
      Error_raise (eb, Error_E_generic,
                   "ipc_finalize(), error in ProcMgr_unload()", 0);
      goto leave;
    }

    state = ProcMgr_getState (procMgrHandlePtr);
    Log_print1 (Diags_USER1, "Processor state is %s", (IArg) procState[state]);

    Log_print0 (Diags_USER1, "Doing ProcMgr_detach()");
    status = ProcMgr_detach (procMgrHandlePtr);

    if (0 > status)
    {
      Error_raise (eb, Error_E_generic,
                   "ipc_finalize(), error in ProcMgr_detach()", 0);
      goto leave;
    }
  }                             /* if (DomxCore_doProcInit) */
  state = ProcMgr_getState (procMgrHandlePtr);
  Log_print1 (Diags_USER1, "Processor state is %s", (IArg) procState[state]);

  Log_print0 (Diags_USER1, "Doing ProcMgr_close()");
  status = ProcMgr_close (&procMgrHandlePtr);

  if (0 > status)
  {
    Error_raise (eb, Error_E_generic,
                 "ipc_finalize(), error in ProcMgr_close()", 0);
    goto leave;
  }

  /* TODO comment */
  /* Log_print0 (Diags_USER1, "Doing SysMgr_destroy()"); */
  /* SysMgr_destroy(0); */

leave:
  if (Error_check (eb))
  {
    Error_raise (eb, Error_E_generic, "ipc_finalize() failed", 0);
  }
  Log_print1 (Diags_EXIT, "Exiting %s: (void)\n", (xdc_IArg) __FUNCTION__);
}

/* Nothing beyond this point */
