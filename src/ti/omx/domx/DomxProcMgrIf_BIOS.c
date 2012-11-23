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
 *  @file  DomxProcMgrIf_BIOS.c
 *         This file contains methods that provides the functionality for
 *         to interface with syslink procMgr module on the RTOS side
 *
 *  @path \avmecomp\ti\omx\domx\DomxProcMgrIf_BIOS.c
 *
 *  @rev 1.0
 */

/******************************************************************
 *   INCLUDE FILES
 ******************************************************************/

/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <xdc/std.h>

/* Module Specific header files */
#include <ti/sysbios/knl/Task.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Gate.h>
#include <xdc/runtime/Startup.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/IGateProvider.h>
#include <xdc/runtime/Gate.h>
#include <xdc/runtime/knl/GateThread.h>
#include <xdc/runtime/knl/Thread.h>

/* ipc header files */
#include <ti/ipc/MultiProc.h>

/* syslink include files */
#include <ti/ipc/Ipc.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/syslink/SysLink.h>

#include <ti/omx/interfaces/openMaxv11/interfaces_openmaxv11_internal.h>
#include <ti/omx/interfaces/openMaxv11/OMX_Core.h>
#include "domx_internal.h"
#include "domx_internalcfg.h"
#include "OmxRpc__epilogue.h"
#include "omx_utils.h"

/** ============================================================================
 *  Globals
 *  ============================================================================
 */
/*
*g_bDomxCore_procAttach - Used for indicating proc Attach has been called 
* separately
*/
OMX_BOOL g_bDomxCore_procAttach = OMX_FALSE;

/** ============================================================================
 *  Extern declarations
 *  ============================================================================
 */

/** ============================================================================
 *  Functions
 *  ============================================================================
 */

Void DomxCore_procMsgQInit (Error_Block *eb)
{
  DomxTypes_coreType i;
  OmxRpc_errorType rpcError;

  OMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();

  if (DomxCore_ipcInfoTbl[DomxCore_masterCoreId].ipcEnable)
  {
    rpcError = OmxRpc_moduleRegisterMsgqHeap (DomxCore_masterCoreId);
    OMX_UTL_TRACE_FUNCTION_ASSERT ((OmxRpc_errorNone == rpcError),
                                   "OmxRpc_moduleRegisterMsgqHeap failed");
  }

  /*-------------------------------------------------------------------------*/
  /* Ipc_attach with rest of the Cores */

  /*-------------------------------------------------------------------------*/

  for (i = DomxTypes_coreFirst; i < DomxTypes_coreCount; i++)
  {
    if ((DomxCore_localCoreId != i) && (DomxCore_masterCoreId != i))
    {
      if (DomxCore_ipcInfoTbl[i].ipcEnable)
      {
        UInt16 multiProcId = DomxCore_mapDomxCore2MultiProcId (i, eb);
        rpcError = OmxRpc_moduleRegisterMsgqHeap (i);
        OMX_UTL_TRACE_FUNCTION_ASSERT ((OmxRpc_errorNone == rpcError),
                                       "OmxRpc_moduleRegisterMsgqHeap failed");
      } /* if (DomxCore_ipcInfoTbl...) */
    } /* if (DomxCore_localCoreId) */
  } /* for (i) */
  OMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (OMX_ErrorNone);
}


Void DomxCore_procAttach (Error_Block *eb)
{
  Int32 status = 0;
  DomxTypes_coreType i;

  OMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();

  /*-------------------------------------------------------------------------*/
  /* First do Ipc_attach with Master Core */

  /*-------------------------------------------------------------------------*/
  if ((DomxCore_localCoreId == DomxTypes_coreDsp)           || 
      (DomxCore_localCoreId == DomxTypes_coreCortexVideoM3) ||
      (DomxCore_localCoreId == DomxTypes_coreCortexHdvpssM3))
  {
    SysLink_setup ();  /* Required as in syslink ver .68 beta and above */
  }
  else
  {
    /* Do nothing for now */
  }

  if (DomxCore_ipcInfoTbl[DomxCore_masterCoreId].ipcEnable)
  {
    UInt16 multiProcId =
      DomxCore_mapDomxCore2MultiProcId (DomxCore_masterCoreId, eb);
    /* initialize the ipc layer */
    /* Video-M3 needs to do Ipc_attach with master core */
    if ((DomxCore_localCoreId == DomxTypes_coreCortexVideoM3) ||
        (DomxCore_localCoreId == DomxTypes_coreDsp))
    {
      OMX_UTL_TRACE_FUNCTION_MSG ("Before Ipc_attach");
      do
      {
        status = Ipc_attach (multiProcId);
        Task_sleep (100);
      }
      while (status < 0);
      OMX_UTL_TRACE_FUNCTION_MSG ("After Ipc_attach");
      OMX_UTL_TRACE_FUNCTION_ASSERT ((status >= 0), "Ipc_attach failed");
      OMX_UTL_TRACE_FUNCTION_MSG ("Ipc_attach successful");
    }
    else
    {
      /* Proxy server setup code would have done Ipc_attach with master core */
      OMX_UTL_TRACE_FUNCTION_MSG ("Before Ipc_attach");
      do
      {
        status = Ipc_attach (multiProcId);
        Task_sleep (100);
      } while (status < 0);
      
      OMX_UTL_TRACE_FUNCTION_MSG ("After Ipc_attach");
      OMX_UTL_TRACE_FUNCTION_ASSERT ((status >= 0), "Ipc_attach failed");
      OMX_UTL_TRACE_FUNCTION_MSG ("Ipc_attach successful");
    }
  }

  /*-------------------------------------------------------------------------*/
  /* Ipc_attach with rest of the Cores */

  /*-------------------------------------------------------------------------*/

  for (i = DomxTypes_coreFirst; i < DomxTypes_coreCount; i++)
  {
    if ((DomxCore_localCoreId != i) && (DomxCore_masterCoreId != i))
    {
      if (DomxCore_ipcInfoTbl[i].ipcEnable)
      {
        UInt16 multiProcId = DomxCore_mapDomxCore2MultiProcId (i, eb);

        /* initialize the ipc layer */
        OMX_UTL_TRACE_FUNCTION_MSG ("Before Ipc_attach");
        do
        {
          status = Ipc_attach (multiProcId);
          Task_sleep (100);
        } while (status < 0);

        OMX_UTL_TRACE_FUNCTION_MSG ("After Ipc_attach");

        OMX_UTL_TRACE_FUNCTION_ASSERT ((status >= 0), "Ipc_attach failed");
        OMX_UTL_TRACE_FUNCTION_MSG ("Ipc_attach successful");

      } /* if (DomxCore_ipcInfoTbl...) */
    } /* if (DomxCore_localCoreId) */
  } /* for (i) */
  OMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (OMX_ErrorNone);
}

/**
 * @name DomxCore_procInit()
 * @brief DomxCore_procInit does Ipc_attach with each of the enabled cores
 * @param eb  : Error Block used to raise errors 
 * @return none
 */


Void DomxCore_procInit (Error_Block *eb)
{
  Int32 status = 0;
  DomxTypes_coreType i;
  OmxRpc_errorType rpcError;

  OMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();

  /*-------------------------------------------------------------------------*/
  /* First do Ipc_attach with Master Core */

  /*-------------------------------------------------------------------------*/
  if (g_bDomxCore_procAttach == OMX_FALSE ) {
  if ((DomxCore_localCoreId == DomxTypes_coreDsp)           || 
      (DomxCore_localCoreId == DomxTypes_coreCortexVideoM3) ||
      (DomxCore_localCoreId == DomxTypes_coreCortexHdvpssM3))
  {
    SysLink_setup ();  /* Required as in syslink ver .68 beta and above */
  }
  else
  {
    /* Do nothing for now */
  }

  if (DomxCore_ipcInfoTbl[DomxCore_masterCoreId].ipcEnable)
  {
    UInt16 multiProcId =
      DomxCore_mapDomxCore2MultiProcId (DomxCore_masterCoreId, eb);
    /* initialize the ipc layer */
    /* Video-M3 needs to do Ipc_attach with master core */
    if ((DomxCore_localCoreId == DomxTypes_coreCortexVideoM3) ||
        (DomxCore_localCoreId == DomxTypes_coreDsp))
    {
      OMX_UTL_TRACE_FUNCTION_MSG ("Before Ipc_attach");
      do
      {
        status = Ipc_attach (multiProcId);
        Task_sleep (100);
      }
      while (status < 0);
      OMX_UTL_TRACE_FUNCTION_MSG ("After Ipc_attach");
      OMX_UTL_TRACE_FUNCTION_ASSERT ((status >= 0), "Ipc_attach failed");
      OMX_UTL_TRACE_FUNCTION_MSG ("Ipc_attach successful");
    }
    else
    {
      /* Proxy server setup code would have done Ipc_attach with master core */
      OMX_UTL_TRACE_FUNCTION_MSG ("Before Ipc_attach");
      do
      {
        status = Ipc_attach (multiProcId);
        Task_sleep (100);
      } while (status < 0);
      
      OMX_UTL_TRACE_FUNCTION_MSG ("After Ipc_attach");
      OMX_UTL_TRACE_FUNCTION_ASSERT ((status >= 0), "Ipc_attach failed");
      OMX_UTL_TRACE_FUNCTION_MSG ("Ipc_attach successful");
    }

    rpcError = OmxRpc_moduleRegisterMsgqHeap (DomxCore_masterCoreId);

    OMX_UTL_TRACE_FUNCTION_ASSERT ((OmxRpc_errorNone == rpcError),
                                   "OmxRpc_moduleRegisterMsgqHeap failed");
  }

  /*-------------------------------------------------------------------------*/
  /* Ipc_attach with rest of the Cores */

  /*-------------------------------------------------------------------------*/

  for (i = DomxTypes_coreFirst; i < DomxTypes_coreCount; i++)
  {
    if ((DomxCore_localCoreId != i) && (DomxCore_masterCoreId != i))
    {
      if (DomxCore_ipcInfoTbl[i].ipcEnable)
      {
        UInt16 multiProcId = DomxCore_mapDomxCore2MultiProcId (i, eb);

        /* initialize the ipc layer */
        OMX_UTL_TRACE_FUNCTION_MSG ("Before Ipc_attach");
        do
        {
          status = Ipc_attach (multiProcId);
          Task_sleep (100);
        } while (status < 0);

        OMX_UTL_TRACE_FUNCTION_MSG ("After Ipc_attach");

        OMX_UTL_TRACE_FUNCTION_ASSERT ((status >= 0), "Ipc_attach failed");
        OMX_UTL_TRACE_FUNCTION_MSG ("Ipc_attach successful");

        rpcError = OmxRpc_moduleRegisterMsgqHeap (i);

        OMX_UTL_TRACE_FUNCTION_ASSERT ((OmxRpc_errorNone == rpcError),
                                       "OmxRpc_moduleRegisterMsgqHeap failed");
      } /* if (DomxCore_ipcInfoTbl...) */
    } /* if (DomxCore_localCoreId) */
  } /* for (i) */
  }
  else {
    DomxCore_procMsgQInit (eb);
  }
  OMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (OMX_ErrorNone);
}


/**
 * @name DomxCore_procDeInit()
 * @brief DomxCore_procInit does Ipc_finalize with each of the enabled cores
 * @param eb  : Error Block used to raise errors 
 * @return none
 */
Void DomxCore_procDeInit (Error_Block *eb)
{
  DomxTypes_coreType i;
  OmxRpc_errorType rpcError;

  for (i = DomxTypes_coreFirst; i < DomxTypes_coreCount; i++)
  {
    if ((DomxCore_localCoreId != i))
    {
      if (DomxCore_ipcInfoTbl[i].ipcEnable)
      {
        UInt16 multiProcId = DomxCore_mapDomxCore2MultiProcId (i, eb);

        rpcError = OmxRpc_moduleUnregisterMsgqHeap (i);
        OMX_UTL_TRACE_FUNCTION_ASSERT ((OmxRpc_errorNone == rpcError),
                                       "OmxRpc_moduleUnregisterMsgqHeap failed");

        if (DomxCore_doProcInit)
        {
          if ((DomxCore_localCoreId == 1) || (DomxCore_localCoreId == 0))
          {
            Ipc_detach (multiProcId);
          }
          else
          {
#ifndef ADD_FBDEV_SUPPORT
            Ipc_detach (multiProcId);
#endif
          }
        } /* if (DomxCore_doProcInit) */
      } /* if (DomxCore_ipcInfoTbl[]...) */
    } /* if (DomxCore_localCoreId) */
  } /* for (i) */
}

/**
 * @name DomxCore_mapPhyAddr2UsrVirtual()
 * @brief DomxCore_mapPhyAddr2UsrVirtual maps physical address into user virtual address
 * @param phyAddr  : Physical address to be translated
 * @param len      : Length of memory block to be translated
 * @return none
 */
Ptr DomxCore_mapPhyAddr2UsrVirtual (UInt32 phyAddr, UInt32 len)
{
  (Void) len;                   /* kill warnings */
  /* On RTOS side we are using PhyAddr = VirtualAddr */
  return ((Ptr) phyAddr);
}

/**
 * @name DomxCore_unmapPhyAddr()
 * @brief DomxCore_mapPhyAddr2UsrVirtual unmaps physical address that was previously mapped
 * @param phyAddr  : Physical address to be unmapped
 * @param len      : Length of memory block to be unmapped
 * @return none
 */
Void DomxCore_unmapPhyAddr (UInt32 phyAddr, UInt32 len)
{
  (Void) len;                   /* kill warnings */
  (Void) phyAddr;
  return;
}

/* Nothing beyond this point */
