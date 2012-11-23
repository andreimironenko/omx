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
 *  @file  DomxProcMgrIf_HLOS.c
 *         This file contains methods that provides the functionality for
 *         the OpenMAX1.1 DOMX Framework that interfaces with syslink 
 *         ProcMgr modules
 *
 *  @path \avmecomp\ti\omx\domx\DomxProcMgrIf_HLOS.c
 *
 *  @rev 1.0
 */

/******************************************************************
 *   INCLUDE FILES
 ******************************************************************/

/* C headers */
#include <stdio.h>
#include <stdlib.h>

/* Standard headers */
#include <xdc/std.h>
#include <stdlib.h>
#include <stdio.h>

/* Module Specific header files */
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Gate.h>
#include <xdc/runtime/Startup.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/IGateProvider.h>
#include <xdc/runtime/Gate.h>
#include <xdc/runtime/knl/GateThread.h>
#include <xdc/runtime/knl/Thread.h>
/* Log header files */
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Diags.h>
#include <ti/ipc/MultiProc.h>
#include <ti/syslink/SysLink.h>
#include <ti/uia/linux/ServiceMgr.h>
#include <ti/omx/interfaces/openMaxv11/interfaces_openmaxv11_internal.h>
#include <ti/omx/interfaces/openMaxv11/OMX_Core.h>
#include "domx_internal.h"
#include "domx_internalcfg.h"
#include "domx_util.h"
#include "IpcSupport.h"



/** ============================================================================
 *  Globals
 *  ============================================================================
 */

/** ============================================================================
 *  Extern declarations
 *  ============================================================================
 */

/** ============================================================================
 *  Functions
 *  ============================================================================
 */

/**
 * @name DomxCore_procInit()
 * @brief DomxCore Processor Init function .
 *        Does IPC initialize with each of the slave cores .Also starts the
 *        RcmServer for the local core
 * @param eb  : Error Block to raise any errors
 * @return None
 */
Void DomxCore_procInit (Error_Block *eb)
{

  Int i;
  OmxRpc_errorType rpcError;

  Log_print1 (Diags_ENTRY, "Entered function:%s", (IArg) __FUNCTION__);

  if (DomxCore_masterCoreId == DomxCore_localCoreId)
  {

    /* SysLink_setup is the first APIs needs to be called by user side
       application to use any SysLink functionality */
    SysLink_setup ();
    Log_print0 (Diags_USER1, "SysLink_setup() complete");

    /* Master Core is assigned the highest number as id */

    for (i = 0; i < DomxCore_slaveCoreExecutableInfoTbl.length; i++)
    {
      DomxTypes_coreType coreId;

      if (DomxCore_ipcInfoTbl
          [DomxCore_slaveCoreExecutableInfoTbl.elem[i].coreId].ipcEnable)
      {
        coreId = DomxCore_slaveCoreExecutableInfoTbl.elem[i].coreId;
        ipc_initialize (DomxCore_slaveCoreExecutableInfoTbl.elem[i].
                        executableName, coreId,
                        (ProcMgr_Handle *) & (DomxCore_module->
                                              multiProcState[coreId].
                                              procMgrHandle),
                        &DomxCore_module->multiProcState[coreId].procId,
                        &(DomxCore_module->multiProcState[coreId].
                          procMgrFileId), eb);
        rpcError = OmxRpc_moduleRegisterMsgqHeap (coreId);
        DOMX_UTL_TRACE_FUNCTION_ASSERT ((OmxRpc_errorNone == rpcError),
                                       "OmxRpc_moduleRegisterMsgqHeap failed");
      }
    }                           /* for (i= 0; i <
                                   DomxCore_slaveCoreExecutableInfoTbl.length;i++) 
                                 */
    /* Start the UIA Service Manager here after SysLink_setup is called */
    ServiceMgr_start ();
  }
}

/**
 * @name DomxCore_procDeInit()
 * @brief DomxCore Processor Init function .
 *        Does IPC finalize with each of the slave cores .
 * @param eb  : Error Block to raise any errors
 * @return None
 */
Void DomxCore_procDeInit (Error_Block *eb)
{
  Int i;
  OmxRpc_errorType rpcError;

  /* Stop the UIA Service Manager here before SysLink_destroy is called */
  ServiceMgr_stop ();
  if (DomxCore_masterCoreId == DomxCore_localCoreId)
  {
    for (i = 0; i < DomxCore_slaveCoreExecutableInfoTbl.length; i++)
    {
      if (DomxCore_ipcInfoTbl
          [DomxCore_slaveCoreExecutableInfoTbl.elem[i].coreId].ipcEnable)
      {
        DomxTypes_coreType coreId;

        coreId = DomxCore_slaveCoreExecutableInfoTbl.elem[i].coreId;

        rpcError = OmxRpc_moduleUnregisterMsgqHeap (coreId);
        DOMX_UTL_TRACE_FUNCTION_ASSERT ((OmxRpc_errorNone == rpcError),
                                       "OmxRpc_moduleUnregisterMsgqHeap failed");

        DOMX_UTL_TRACE_FUNCTION_ASSERT
          ((DomxCore_module->multiProcState[coreId].procMgrHandle != NULL), 
           "procMgrHandle is NULL");

        ipc_finalize ((ProcMgr_Handle)
                      (DomxCore_module->multiProcState[coreId].procMgrHandle),
                      DomxCore_module->multiProcState[coreId].procId,
                      DomxCore_module->multiProcState[coreId].procMgrFileId,
                      eb);
      } /* if (DomxCore_ipcInfoTbl[]...) */
    } /* for (i) */
    SysLink_destroy ();
  } /* if (DomxCore_masterCoreId) */
}

/**
 * @name DomxCore_mapUsrVirtualAddr2phy()
 * @brief DomxCore function to map user virtual  to physical address
 * @param pUsrVirtAddr  : Usr Virtual Addr
 * @return none
 */
Ptr DomxCore_mapUsrVirtualAddr2phy (UInt32 pUsrVirtAddr)
{
  Int32 status = 0;
  Ptr phyAddr = NULL;
  ProcMgr_Handle pSlaveProcHandle = NULL;
  DomxTypes_coreType core_index;
  
  Log_print1 (Diags_ENTRY, "Entered function:%s", (IArg) __FUNCTION__);

  for (core_index = DomxTypes_coreFirst; core_index < DomxTypes_coreCortexA8;
       core_index++)
  {
    if (DomxCore_module->multiProcState[core_index].procMgrHandle != NULL)
    {
      pSlaveProcHandle =
        DomxCore_module->multiProcState[core_index].procMgrHandle;
      break;
    }
  }
  if ( pSlaveProcHandle != NULL ) { 
    status = ProcMgr_translateAddr (pSlaveProcHandle,
                                    (Ptr) & phyAddr, 
                                     ProcMgr_AddrType_MasterPhys,
                                    (Ptr) pUsrVirtAddr, ProcMgr_AddrType_MasterUsrVirt);

    if (status < 0)
    {
      phyAddr = NULL;
      DOMX_UTL_TRACE_FUNCTION_ASSERT ((status >= 0), 
                                      "ProcMgr_translateAddr failed");
    }
    else
    {
      Log_print2 (Diags_USER1, "\n\nProcMgr_translateAddr Status [0x%x]"
                  " Physical  Address [0x%x]\n",
                  status, (IArg) phyAddr);
    }
  }
  return (phyAddr);
}


/**
 * @name DomxCore_mapPhyAddr2UsrVirtual()
 * @brief DomxCore function to map physical address to user virtual
 * @param phyAddr  : Physical address to be translated
 * @param len      : Length of memory block to be translated
 * @return none
 */
Ptr DomxCore_mapPhyAddr2UsrVirtual (UInt32 phyAddr, UInt32 len)
{
  Int32 status = 0;
  ProcMgr_AddrInfo addrInfo;
  Ptr pUsrVirtAddr = NULL;
  ProcMgr_Handle pSlaveProcHandle = NULL;
  DomxTypes_coreType core_index;

  Log_print1 (Diags_ENTRY, "Entered function:%s", (IArg) __FUNCTION__);

  /* Map the kernel space address to user space */
  addrInfo.addr[ProcMgr_AddrType_MasterPhys] = phyAddr;
  addrInfo.addr[ProcMgr_AddrType_SlaveVirt] = phyAddr;
  addrInfo.size = len;
  addrInfo.isCached = FALSE;

  for (core_index = DomxTypes_coreFirst; core_index < DomxTypes_coreLast;
       core_index++)
  {
    if (DomxCore_module->multiProcState[core_index].procMgrHandle != NULL)
    {
      pSlaveProcHandle =
        DomxCore_module->multiProcState[core_index].procMgrHandle;
      break;
    }
  }

  DOMX_UTL_TRACE_FUNCTION_ASSERT ((pSlaveProcHandle != NULL),
                                  "slaveProcHandle is NULL");
  status = ProcMgr_translateAddr (pSlaveProcHandle,
                                  (Ptr) & pUsrVirtAddr,
                                  ProcMgr_AddrType_MasterUsrVirt,
                                  (Ptr) phyAddr, ProcMgr_AddrType_MasterPhys);
  if (status < 0)
  {
    status = ProcMgr_map (pSlaveProcHandle,
                          (ProcMgr_MASTERKNLVIRT |
                           ProcMgr_MASTERUSRVIRT),
                          &addrInfo, ProcMgr_AddrType_MasterPhys);
    if (status < 0)
    {
      Log_print1 (Diags_USER1, "ProcMgr_map Failed status: 0x%x", status);
    }
    else
    {
      Log_print1 (Diags_USER1, "ProcMgr_map Success VirtAddr: %p",
                  addrInfo.addr[ProcMgr_AddrType_MasterUsrVirt]);
    }
    status = ProcMgr_translateAddr (pSlaveProcHandle,
                                    (Ptr) & pUsrVirtAddr,
                                    ProcMgr_AddrType_MasterUsrVirt,
                                    (Ptr) phyAddr, ProcMgr_AddrType_MasterPhys);

    if (status < 0)
    {
      pUsrVirtAddr = NULL;
      DOMX_UTL_TRACE_FUNCTION_ASSERT ((status >= 0), 
                                      "ProcMgr_translateAddr failed");
    }
    else
    {
      Log_print2 (Diags_USER1, "\n\nProcMgr_translateAddr Status [0x%x]"
                  " User Virtual Address [0x%x]\n",
                  status, (IArg) pUsrVirtAddr);
    }
  }
  return (pUsrVirtAddr);
}

/**
 * @name DomxCore_unmapPhyAddr2UsrVirtual()
 * @brief DomxCore_mapPhyAddr2UsrVirtual unmaps previous mapped  physical address
 * @param phyAddr  : Physical address to be unmapped
 * @param len      : Length of memory block to be unmapped
 * @return none
 */
Void DomxCore_unmapPhyAddr (UInt32 phyAddr, UInt32 len)
{

  Int32 status = 0;
  ProcMgr_AddrInfo addrInfo;
  ProcMgr_Handle pSlaveProcHandle = NULL;
  DomxTypes_coreType core_index;
  Ptr pUsrVirtAddr = NULL;
  Ptr pKnlVirtAddr = NULL;

  /* Map the kernel space address to user space */
  addrInfo.addr[ProcMgr_AddrType_MasterPhys] = phyAddr;
  addrInfo.addr[ProcMgr_AddrType_SlaveVirt] = phyAddr;
  addrInfo.size = len;
  addrInfo.isCached = FALSE;

  for (core_index = DomxTypes_coreFirst; core_index < DomxTypes_coreLast;
       core_index++)
  {
    if (DomxCore_module->multiProcState[core_index].procMgrHandle != NULL)
    {
      pSlaveProcHandle =
        DomxCore_module->multiProcState[core_index].procMgrHandle;
      break;
    }
  }

  DOMX_UTL_TRACE_FUNCTION_ASSERT ((pSlaveProcHandle != NULL),
                                  "slaveProcHandle is NULL");
  status = ProcMgr_translateAddr (pSlaveProcHandle,
                                  (Ptr) & pUsrVirtAddr,
                                  ProcMgr_AddrType_MasterUsrVirt,
                                  (Ptr) phyAddr, ProcMgr_AddrType_MasterPhys);
  if (status < 0)
  {
    Log_print1 (Diags_USER1, "Error in ProcMgr_translateAddr [0x%x]\n", status);

  }
  else
  {
    Log_print2 (Diags_USER1, "ProcMgr_translateAddr Status [0x%x]"
                " User Virtual Address [0x%x]\n", status, (IArg) pUsrVirtAddr);
  }

  addrInfo.addr[ProcMgr_AddrType_MasterUsrVirt] = (UInt32) pUsrVirtAddr;

  status = ProcMgr_translateAddr (pSlaveProcHandle,
                                  (Ptr) & pKnlVirtAddr,
                                  ProcMgr_AddrType_MasterKnlVirt,
                                  (Ptr) phyAddr, ProcMgr_AddrType_MasterPhys);
  if (status < 0)
  {
    Log_print1 (Diags_USER1, "Error in ProcMgr_translateAddr [0x%x]\n", status);

  }
  else
  {
    Log_print2 (Diags_USER1,
                "ProcMgr_translateAddr Status [0x%x]"
                " KNl Virtual Address [0x%x]\n", status, (IArg) pKnlVirtAddr);
  }

  addrInfo.addr[ProcMgr_AddrType_MasterKnlVirt] = (UInt32) pKnlVirtAddr;

  status = ProcMgr_unmap (pSlaveProcHandle,
                          (ProcMgr_MASTERKNLVIRT |
                           ProcMgr_MASTERUSRVIRT),
                          &addrInfo, ProcMgr_AddrType_MasterPhys);
  if (status < 0)
  {
    Log_print1 (Diags_USER1, "ProcMgr_unmap Failed status: 0x%x", status);
  }
  else
  {
    Log_print0 (Diags_USER1, "ProcMgr_unmap Success ");
  }
  Log_print3 (Diags_USER1, "Unmap done:%p:%d:%d\n", phyAddr, len, status);

  return;
}

/* Nothing beyond this point */
