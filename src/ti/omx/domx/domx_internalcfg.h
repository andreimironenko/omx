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
 *******************************************************************************
 *  @file   domx_internalcfg.h
 *  @brief  This file contains TI's implementation of the OMX methods for DOMX
 *
 *          This file contains code that statically configures the
 *          domx module
 *
 *  @rev 1.0
 *******************************************************************************
 */

/* Constants */
extern const xdc_Int32 OmxRpc_maxConfigStructSize;
extern const OmxRpc_rcmFxnRegistryTbl_CT OmxRpc_rcmFxnRegistryTbl;
extern const xdc_UInt16 OmxRpc_maxQueuedBufLen;
extern const xdc_Int OmxRpc_rcmCoreServerPriority;
extern const xdc_Bool OmxRpc_createMsgHeaps;
extern const xdc_String OmxRpc_rcmServerNamePrefix;
extern const xdc_UInt16 OmxRpc_msgHeapId_core0;
extern const xdc_UInt16 OmxRpc_msgHeapId_core1;
extern const xdc_UInt16 OmxRpc_msgHeapId_core2;
extern const xdc_UInt16 OmxRpc_msgHeapId_core3;

extern const DomxTypes_coreType DomxCore_localCoreId;
extern const DomxTypes_coreType DomxCore_masterCoreId;
extern const xdc_Bool DomxCore_doProcInit;
extern const DomxCore_slaveCoreExecutableInfo
  DomxCore_slaveCoreExecutableInfoTbl;
extern const DomxCore_componentCoreInfoTbl_CT DomxCore_componentCoreInfoTbl;
extern const DomxCore_ipcInfo DomxCore_ipcInfoTbl[DomxTypes_coreCount];

extern const xdc_UInt32 DomxTunnelMgr_maxNumComponents;

extern const xdc_runtime_IHeap_Handle OmxProxy_Module_heap_obj;

/* Variables */
extern OmxRpc_Module_State *OmxRpc_module;
extern DomxCore_Module_State *DomxCore_module;
extern DomxTunnelMgr_Module_State *DomxTunnelMgr_module;
extern DomxTunnelMgr_connectionInfoAry DomxTunnelMgr_connectionInfoArr[];

/* defines */
#define OmxProxy_Module_heap()      OmxProxy_Module_heap_obj
#define OmxRpc_PDmm_flushBuffer     DmmDelegate_flushBuffer
#define OmxRpc_PDmm_mapBuffer       DmmDelegate_mapBuffer
#define OmxRpc_PDmm_unmapBuffer     DmmDelegate_unmapBuffer
#define OmxRpc_PDmm_xltMapBuf2Ptr   DmmDelegate_xltMapBuf2Ptr

void OmxRpc_Params_init (OmxRpc_Params *prms);
OmxRpc_Handle OmxRpc_create (const OmxRpc_Params *prms,
                             xdc_runtime_Error_Block *eb);

/* Nothing beyond this point */
