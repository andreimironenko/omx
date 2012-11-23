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
 *  @file  DmmDelegateHeap.c
 *         DOMX DmmDelegate that implements SharedMemory based Heap
 *
 *  @path \avmecomp\ti\omx\domx\delegates\shmem\DmmDelegateHeap.c
 *
 *  @rev 1.0
 */

 /******************************************************************
 *   INCLUDE FILES
 ******************************************************************/

#include <stdio.h>
#include <string.h>

/* xdctools header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/IHeap.h>
#include <xdc/runtime/Gate.h>
#include <xdc/runtime/Startup.h>
#include <xdc/runtime/knl/Cache.h>
#include <xdc/runtime/knl/GateH.h>

/* Log related files */
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
    #ifndef SLOG_DOMXDH_Module__MID
      #define SLOG_DOMXDH_Module__MID (SLOG_DOMX_Module__MID)
    #endif
    #undef Module__MID
    #define Module__MID SLOG_DOMXDH_Module__MID
  #else
    #include <xdc/runtime/Log.h>
    #include <xdc/runtime/Assert.h>
    #include <xdc/runtime/Diags.h>
  #endif /* End Of #if (USE_SLOG_PRINT == 1) */
#endif /* End of #else */

#include <string.h>

#include <ti/ipc/SharedRegion.h>

#include <ti/omx/interfaces/openMaxv11/interfaces_openmaxv11_internal.h>
#include <ti/omx/interfaces/openMaxv11/OMX_Core.h>
#include <ti/omx/domx/interfaces/domx_interfaces_internal.h>
#include "domx_delegates_shmem_internal.h"
#include "domx_delegates_shmem_internalcfg.h"
#include "domx_util.h"

/** ============================================================================
 *  @const  IPC_BUFFER_ALIGN
 *
 *  @desc   Macro to align a number.
 *          x: The number to be aligned
 *          y: The value that the number should be aligned to.
 *  ============================================================================
 */
#define IPC_BUFFER_ALIGN(x, y) (UInt32)((UInt32)((x + y - 1) / y) * y)

/**
 * @name   DmmDelegate_Module_startup
 * @brief  DmmDelegate Module statrup fucntion
 * @param  state            : Current startup State (DONE/NOT_DONE)
 * @return Startup_DONE     : SUCCESS
 */
Int DmmDelegate_Module_startup (Int state)
{
  if (state == Startup_DONE)
  {
    return (Startup_DONE);
  }
  return (Startup_DONE);
}

/**
 * @name   createHeapBufIpcData
 * @brief  Internal function to create a IPC heap for data buffers
 * @param  heapName         : Name of the heap
 * @param   eb              : Used to raise errors
 * @return Heap Handle
 */
static IHeap_Handle createHeapBufIpcData (String heapName, Error_Block *eb)
{
  IHeap_Handle heapHandle = NULL;
  IArg key;
  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();
  key = Gate_enterModule ();
  heapHandle =
    SharedRegion_getHeap (DmmDelegate_ipcDataHeapCfg.sharedRegionIndex);
  Gate_leaveModule (key);
  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (OMX_ErrorNone);
  return heapHandle;
}

/**
 * @name   createHeapBufIpcComm
 * @brief  Internal function to create a IPC heap for control messages
 * @param  heapName         : Name of the heap
 * @param  remoteMultiProcId: Remote multiProcID with which communication heap is created
 * @param   eb              : Used to raise errors
 * @return Heap Handle
 */
static IHeap_Handle createHeapBufIpcComm (String heapName,
                                          UInt16 remoteMultiProcId,
                                          Error_Block *eb)
{
  IHeap_Handle heapHandle = NULL;
  IArg key;

  key = Gate_enterModule ();
  heapHandle = SharedRegion_getHeap (0);
  Gate_leaveModule (key);
  return heapHandle;
}

/**
 * @name   DmmDelegate_createIpcHeap
 * @brief  Function to create a IPC heap for control messages
 * @param  heapInfo         : Heap create params (Name of heap/Type etc)
 * @param   eb              : Used to raise errors
 * @return Heap Handle
 */
IHeap_Handle DmmDelegate_createIpcHeap (DmmDelegate_ipcHeapInfo *heapInfo,
                                        Error_Block *eb)
{
  IHeap_Handle heapHandle;

  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();

  if (DmmDelegate_heapIpcData == heapInfo->heapType)
  {
    heapHandle = createHeapBufIpcData (heapInfo->heapName, eb);
  }
  else
  {
    heapHandle = createHeapBufIpcComm (heapInfo->heapName,
                                       heapInfo->remoteMultiProcId, eb);
  }
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((heapHandle != NULL),
                                  "createIpcHeap() failed");

  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (OMX_ErrorNone);

  return (heapHandle);
}

/* Nothing beyond this point */
