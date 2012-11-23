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
 *  @file  DmmDelegateHeap_Common.c
 *         DOMX DmmDelegate that implements SharedMemory based Heap
 *         common to HLOS and RTOS
 *
 *  @path \avmecomp\ti\omx\domx\delegates\shmem\DmmDelegateHeap_Common.c
 *
 *  @rev 1.0
 */

 /******************************************************************
 *   INCLUDE FILES
 ******************************************************************/

/* xdctools header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/IHeap.h>
#include <xdc/runtime/Gate.h>
#include <xdc/runtime/IGateProvider.h>
#include <xdc/runtime/Startup.h>

/* ipc header files */

#include <ti/sdo/utils/NameServer.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/ipc/gates/GatePeterson.h>
#include <ti/ipc/ListMP.h>
#include <ti/ipc/HeapBuf.h>
#include "package/internal/DmmDelegate.xdc.h"

IHeap_Handle DmmDelegate_createIpcHeapInternal (String heapName,
                                                Void *shAddrBaseVirtualStart,
                                                UInt32 sharedMemSize,
                                                UInt32 heapBufSize,
                                                UInt32 numBufs, Error_Block *eb)
{
  Int32 status = 0;
  UInt32 shAddrBaseVirtual;
  GatePeterson_Params gateParams;
  HeapBuf_Params heapParams;
  HeapBuf_Handle heapHandle;
  GatePeterson_Handle gateHandle;
  Int sharedRegionIndex, invalidIndex;
  IArg key;

  heapHandle = NULL;

  key = Gate_enterModule ();
  sharedRegionIndex = DmmDelegate_module->sharedRegionIndex++;
  Gate_leaveModule (key);

  shAddrBaseVirtual = (UInt32) shAddrBaseVirtualStart;
#ifdef USE_SYSLINK_APIS
  invalidIndex = -1;
#else
  invalidIndex = SharedRegion_INVALIDINDEX;
#endif
  if (SharedRegion_getIndex (shAddrBaseVirtualStart) == invalidIndex)
  {
    if ((sharedRegionIndex - DmmDelegate_sharedRegionStartIndex) <
        DmmDelegate_numSharedRegions)
    {
      /* Add the region to SharedRegion module. */
#ifdef USE_SYSLINK_APIS
      status = SharedRegion_add (sharedRegionIndex,
                                 shAddrBaseVirtualStart, sharedMemSize);
#else
      status = SharedRegion_add (sharedRegionIndex,
                                 shAddrBaseVirtualStart, sharedMemSize, eb);
      if (Error_check (eb))
      {
        Error_raise (eb, Error_E_generic,
                     "createIpcHeapInternal(), error SharedRegion_add ", NULL);
        status = -1;
      }
#endif
    }
    else
    {
      Error_raise (eb, Error_E_generic,
                   "createIpcHeapInternal(), error numSharedRegions Exceeded. Cant SharedRegion_add",
                   NULL);
      status = -1;
    }
  }

  if (status < 0)
  {
    Error_raise (eb, Error_E_generic,
                 "createIpcHeapInternal(), error in SharedRegion_add():%d",
                 status);
    goto leave;
  }
  else
  {
#ifdef USE_SYSLINK_APIS
    GatePeterson_Params_init (NULL, &gateParams);
#else
    GatePeterson_Params_init (&gateParams);
#endif
    gateParams.sharedAddr = (Ptr) shAddrBaseVirtual;
    gateParams.sharedAddrSize = GatePeterson_sharedMemReq (&gateParams);
    shAddrBaseVirtual += gateParams.sharedAddrSize;

#ifdef USE_SYSLINK_APIS
    do
    {
      status = GatePeterson_open (&gateHandle, &gateParams);
    }
    while (status < 0);
#else
    do
    {
      status = GatePeterson_open (&gateHandle, &gateParams, eb);
    }
    while (status == GatePeterson_Status_FAIL);
#endif
  }
  if (gateHandle == NULL)
  {
    Error_raise (eb, Error_E_generic,
                 "Error in GatePeterson_open [0x%x]\n", status);
    goto leave;
  }
  else
  {
#ifdef USE_SYSLINK_APIS
    /* Create the heap. */
    HeapBuf_Params_init (NULL, &heapParams);
    heapParams.gate = (Gate_Handle) (gateHandle);
#else
    /* Create the heap. */
    HeapBuf_Params_init (&heapParams);
    heapParams.gate = GatePeterson_Handle_upCast (gateHandle);
#endif
    heapParams.name = heapName;
    heapParams.sharedAddr = (Ptr) shAddrBaseVirtual;
    heapParams.align = 128;
    heapParams.numBlocks = numBufs;
    heapParams.blockSize = heapBufSize;
    heapParams.sharedAddrSize = HeapBuf_sharedMemReq (&heapParams,
                                                      &heapParams.
                                                      sharedBufSize);
    heapParams.sharedBuf =
      (Ptr) (shAddrBaseVirtual + heapParams.sharedAddrSize);

    shAddrBaseVirtual += heapParams.sharedBufSize;
#ifdef USE_SYSLINK_APIS
    /* Create the heap. */
    status = HeapBuf_open (&heapHandle, &heapParams);
#else
    /* Create the heap. */
    status = HeapBuf_open (&heapHandle, &heapParams, eb);
    if (Error_check (eb))
    {
      status = -1;
    }
#endif
  }
  if (status < 0)
  {
    Error_raise (eb, Error_E_generic,
                 "Error in HeapBuf_create [0x%x]\n", status);
  }
  if ((shAddrBaseVirtual - ((UInt32) shAddrBaseVirtualStart)) > sharedMemSize)
  {
    Error_raise (eb, DmmDelegate_E_outOfMemory,
                 (shAddrBaseVirtual - ((UInt32) shAddrBaseVirtualStart)),
                 sharedMemSize);
  }
leave:
  if (Error_check (eb))
  {
    Error_raise (eb, Error_E_generic, "createIpcHeapInternal() failed", NULL);
    heapHandle = NULL;
  }
  Log_print0 (Diags_USER1, "<-- createIpcHeapInternal()");
#ifdef USE_SYSLINK_APIS
  return ((IHeap_Handle) heapHandle);
#else
  return (HeapBuf_Handle_upCast (heapHandle));
#endif
}

/* Nothing beyond this point */
