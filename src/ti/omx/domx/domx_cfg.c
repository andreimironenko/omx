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
 *  @file   domx_cfg.c
 *  @brief  This file contains TI's implementation of the OMX methods for DOMX
 *
 *          This file contains code that statically configures the
 *          domx module
 *
 *  @rev 1.0
 *******************************************************************************
 */

#include <string.h>
#include <xdc/std.h>
#include <xdc/runtime/Gate.h>

#include "app_cfg.h"
#include <domx_cfg.h>
#include <ti/omx/interfaces/openMaxv11/interfaces_openmaxv11_internal.h>
#include <ti/omx/domx/domx_internal.h>

#include <omx_vdec.h>
#include <omx_venc.h>
#include <omx_vfcc.h>
#include <omx_vfdc.h>
#include <omx_vfpc.h>
#include <omx_vsplc.h>
#ifdef _BUILD_vsnk_
#include <omx_vsnk.h>
#endif
#ifdef _BUILD_vsrc_
#include <omx_vsrc.h>
#endif
#include <omx_ctrl.h>
#ifdef _BUILD_cmux_
#include <omx_cmux.h>
#endif
#ifdef _BUILD_dmux_
#include <omx_dmux.h>
#endif
#ifdef _BUILD_rtptx_
#include <omx_rtptx.h>
#endif
#ifdef _BUILD_rtprx_
#include <omx_rtprx.h>
#endif
#ifdef _BUILD_vlpb_
#include <omx_vlpb.h>
#endif
#ifdef _BUILD_vswmosaic_
#include <omx_vswmosaic.h>
#endif

#ifdef _BUILD_adec_
#include <omx_adec.h>
#endif
#ifdef _BUILD_aenc_
#include <omx_aenc.h>
#endif
/*
 * OmxRpc
 */

/* Maximum size of the config structure */
const xdc_Int32 OmxRpc_maxConfigStructSize = DOMX_OMXRPC_MAXCONFIGSTRUCTSIZE;

/* Based on the core for which the current iteration of the build is being
   done, the DomxCore_localCoreId is being set accordingly */
#ifdef _LOCAL_CORE_m3video_
  const DomxTypes_coreType DomxCore_localCoreId = DomxTypes_coreCortexVideoM3;
#endif
#ifdef _LOCAL_CORE_m3vpss_
  const DomxTypes_coreType DomxCore_localCoreId = DomxTypes_coreCortexHdvpssM3;
#endif
#ifdef _LOCAL_CORE_a8host_
  const DomxTypes_coreType DomxCore_localCoreId = DomxTypes_coreCortexA8;
#endif
#ifdef _LOCAL_CORE_c6xdsp_
  const DomxTypes_coreType DomxCore_localCoreId = DomxTypes_coreDsp;
#endif

/* Master CoreId is always Cortex A8 (TM) */
const DomxTypes_coreType DomxCore_masterCoreId = DomxTypes_coreCortexA8;

/* Number of RPC function calls registered. Currently this value and the
   register table itself are statically created below */
#define OMXRPC_RCMFXNREGISTRYTABLE_LENGTH (sizeof(OmxRpc_rcmFxnRegistryTblArr)/sizeof(OmxRpc_rcmFxnRegistryAry))

/* Value of fxnIdx which is invalid; also the initial value */
#define OMXRPC_FXNIDX_INVALID              (-1)

const OmxRpc_rcmFxnRegistryAry OmxRpc_rcmFxnRegistryTblArr[] =
{
  {
   {
    "OmxRpcGetHandle",          /* fxnName */
    ((xdc_Int32 (*)(xdc_UInt32, xdc_UInt32 *)) ((xdc_Fxn) OmxRpc_rcmIfGetHandle)),      /* fxnPtr 
                                                                                         */
    OmxRpc_remoteFxnInit,       /* fxnCategory */
    OmxRpc_cmdFirst,            /* omxCmdId */
    (xdc_UInt32) OMXRPC_FXNIDX_INVALID, /* fxnIdx */
   },                          /* [0] */
  },                           /* [0] */
  {
   {
    "OmxRpcFreeHandle",         /* fxnName */
    ((xdc_Int32 (*)(xdc_UInt32, xdc_UInt32 *)) ((xdc_Fxn) OmxRpc_rcmIfFreeHandle)),     /* fxnPtr 
                                                                                         */
    OmxRpc_remoteFxnInit,       /* fxnCategory */
    OmxRpc_cmdFreeHandle,       /* omxCmdId */
    (xdc_UInt32) OMXRPC_FXNIDX_INVALID, /* fxnIdx */
   },                          /* [0] */
  },                           /* [1] */
  {
   {
    "OmxRpcCbEventHandler",     /* fxnName */
    ((xdc_Int32 (*)(xdc_UInt32, xdc_UInt32 *)) ((xdc_Fxn) OmxRpc_rcmIfCbEventHandler)), /* fxnPtr 
                                                                                         */
    OmxRpc_remoteFxnCb,         /* fxnCategory */
    OmxRpc_cmdCbEventHandler,   /* omxCmdId */
    (xdc_UInt32) OMXRPC_FXNIDX_INVALID, /* fxnIdx */
   },                          /* [0] */
  },                           /* [2] */
  {
   {
    "OmxRpcCbEmptyBufferDone",  /* fxnName */
    ((xdc_Int32 (*)(xdc_UInt32, xdc_UInt32 *)) ((xdc_Fxn) OmxRpc_rcmIfCbEmptyFillBufferDone)),  /* fxnPtr 
                                                                                                 */
    OmxRpc_remoteFxnCb,         /* fxnCategory */
    OmxRpc_cmdCbEmptyBufferDone,        /* omxCmdId */
    (xdc_UInt32) OMXRPC_FXNIDX_INVALID, /* fxnIdx */
   },                          /* [0] */
  },                           /* [3] */
  {
   {
    "OmxRpcCbFillBufferDone",   /* fxnName */
    ((xdc_Int32 (*)(xdc_UInt32, xdc_UInt32 *)) ((xdc_Fxn) OmxRpc_rcmIfCbEmptyFillBufferDone)),  /* fxnPtr 
                                                                                                 */
    OmxRpc_remoteFxnCb,         /* fxnCategory */
    OmxRpc_cmdCbFillBufferDone, /* omxCmdId */
    (xdc_UInt32) OMXRPC_FXNIDX_INVALID, /* fxnIdx */
   },                          /* [0] */
  },                           /* [4] */
  {
   {
    "OmxRpcSetParameter",       /* fxnName */
    ((xdc_Int32 (*)(xdc_UInt32, xdc_UInt32 *)) ((xdc_Fxn) OmxRpc_rcmIfSkelOmxApi)),     /* fxnPtr 
                                                                                         */
    OmxRpc_remoteFxnCtrl,       /* fxnCategory */
    OmxRpc_cmdSetParameter,     /* omxCmdId */
    (xdc_UInt32) OMXRPC_FXNIDX_INVALID, /* fxnIdx */
   },                          /* [0] */
  },                           /* [5] */
  {
   {
    "OmxRpcGetParameter",       /* fxnName */
    ((xdc_Int32 (*)(xdc_UInt32, xdc_UInt32 *)) ((xdc_Fxn) OmxRpc_rcmIfSkelOmxApi)),     /* fxnPtr 
                                                                                         */
    OmxRpc_remoteFxnCtrl,       /* fxnCategory */
    OmxRpc_cmdGetParameter,     /* omxCmdId */
    (xdc_UInt32) OMXRPC_FXNIDX_INVALID, /* fxnIdx */
   },                          /* [0] */
  },                           /* [6] */
  {
   {
    "OmxRpcSetConfig",          /* fxnName */
    ((xdc_Int32 (*)(xdc_UInt32, xdc_UInt32 *)) ((xdc_Fxn) OmxRpc_rcmIfSkelOmxApi)),     /* fxnPtr 
                                                                                         */
    OmxRpc_remoteFxnCtrl,       /* fxnCategory */
    OmxRpc_cmdSetConfig,        /* omxCmdId */
    (xdc_UInt32) OMXRPC_FXNIDX_INVALID, /* fxnIdx */
   },                          /* [0] */
  },                           /* [7] */
  {
   {
    "OmxRpcGetConfig",          /* fxnName */
    ((xdc_Int32 (*)(xdc_UInt32, xdc_UInt32 *)) ((xdc_Fxn) OmxRpc_rcmIfSkelOmxApi)),     /* fxnPtr 
                                                                                         */
    OmxRpc_remoteFxnCtrl,       /* fxnCategory */
    OmxRpc_cmdGetConfig,        /* omxCmdId */
    (xdc_UInt32) OMXRPC_FXNIDX_INVALID, /* fxnIdx */
   },                          /* [0] */
  },                           /* [8] */
  {
   {
    "OmxRpcGetComponentVersion",        /* fxnName */
    ((xdc_Int32 (*)(xdc_UInt32, xdc_UInt32 *)) ((xdc_Fxn) OmxRpc_rcmIfSkelOmxApi)),     /* fxnPtr 
                                                                                         */
    OmxRpc_remoteFxnCtrl,       /* fxnCategory */
    OmxRpc_cmdGetComponentVersion,      /* omxCmdId */
    (xdc_UInt32) OMXRPC_FXNIDX_INVALID, /* fxnIdx */
   },                          /* [0] */
  },                           /* [9] */
  {
   {
    "OmxRpcGetExtensionIndex",  /* fxnName */
    ((xdc_Int32 (*)(xdc_UInt32, xdc_UInt32 *)) ((xdc_Fxn) OmxRpc_rcmIfSkelOmxApi)),     /* fxnPtr 
                                                                                         */
    OmxRpc_remoteFxnCtrl,       /* fxnCategory */
    OmxRpc_cmdGetExtensionIndex,        /* omxCmdId */
    (xdc_UInt32) OMXRPC_FXNIDX_INVALID, /* fxnIdx */
   },                          /* [0] */
  },                           /* [10] */
  {
   {
    "OmxRpcGetState",           /* fxnName */
    ((xdc_Int32 (*)(xdc_UInt32, xdc_UInt32 *)) ((xdc_Fxn) OmxRpc_rcmIfSkelOmxApi)),     /* fxnPtr 
                                                                                         */
    OmxRpc_remoteFxnCtrl,       /* fxnCategory */
    OmxRpc_cmdGetState,         /* omxCmdId */
    (xdc_UInt32) OMXRPC_FXNIDX_INVALID, /* fxnIdx */
   },                          /* [0] */
  },                           /* [11] */
  {
   {
    "OmxRpcSendCommand",        /* fxnName */
    ((xdc_Int32 (*)(xdc_UInt32, xdc_UInt32 *)) ((xdc_Fxn) OmxRpc_rcmIfSkelOmxApi)),     /* fxnPtr 
                                                                                         */
    OmxRpc_remoteFxnCtrl,       /* fxnCategory */
    OmxRpc_cmdSendCommand,      /* omxCmdId */
    (xdc_UInt32) OMXRPC_FXNIDX_INVALID, /* fxnIdx */
   },                          /* [0] */
  },                           /* [12] */
  {
   {
    "OmxRpcUseBuffer",          /* fxnName */
    ((xdc_Int32 (*)(xdc_UInt32, xdc_UInt32 *)) ((xdc_Fxn) OmxRpc_rcmIfSkelOmxApi)),     /* fxnPtr 
                                                                                         */
    OmxRpc_remoteFxnCtrl,       /* fxnCategory */
    OmxRpc_cmdUseBuffer,        /* omxCmdId */
    (xdc_UInt32) OMXRPC_FXNIDX_INVALID, /* fxnIdx */
   },                          /* [0] */
  },                           /* [13] */
  {
   {
    "OmxRpcAllocBuffer",        /* fxnName */
    ((xdc_Int32 (*)(xdc_UInt32, xdc_UInt32 *)) ((xdc_Fxn) OmxRpc_rcmIfSkelOmxApi)),     /* fxnPtr 
                                                                                         */
    OmxRpc_remoteFxnCtrl,       /* fxnCategory */
    OmxRpc_cmdAllocBuffer,      /* omxCmdId */
    (xdc_UInt32) OMXRPC_FXNIDX_INVALID, /* fxnIdx */
   },                          /* [0] */
  },                           /* [14] */
  {
   {
    "OmxRpcFreeBuffer",         /* fxnName */
    ((xdc_Int32 (*)(xdc_UInt32, xdc_UInt32 *)) ((xdc_Fxn) OmxRpc_rcmIfSkelOmxApi)),     /* fxnPtr 
                                                                                         */
    OmxRpc_remoteFxnCtrl,       /* fxnCategory */
    OmxRpc_cmdFreeBuffer,       /* omxCmdId */
    (xdc_UInt32) OMXRPC_FXNIDX_INVALID, /* fxnIdx */
   },                          /* [0] */
  },                           /* [15] */
  {
   {
    "OmxRpcEmptyThisBuffer",    /* fxnName */
    ((xdc_Int32 (*)(xdc_UInt32, xdc_UInt32 *)) ((xdc_Fxn) OmxRpc_rcmIfSkelOmxApi)),     /* fxnPtr 
                                                                                         */
    OmxRpc_remoteFxnCtrl,       /* fxnCategory */
    OmxRpc_cmdEmptyThisBuffer,  /* omxCmdId */
    (xdc_UInt32) OMXRPC_FXNIDX_INVALID, /* fxnIdx */
   },                          /* [0] */
  },                           /* [16] */
  {
   {
    "OmxRpcFillThisBuffer",     /* fxnName */
    ((xdc_Int32 (*)(xdc_UInt32, xdc_UInt32 *)) ((xdc_Fxn) OmxRpc_rcmIfSkelOmxApi)),     /* fxnPtr 
                                                                                         */
    OmxRpc_remoteFxnCtrl,       /* fxnCategory */
    OmxRpc_cmdFillThisBuffer,   /* omxCmdId */
    (xdc_UInt32) OMXRPC_FXNIDX_INVALID, /* fxnIdx */
   },                          /* [0] */
  },                           /* [17] */
  {
   {
    "OmxRpcTunnelRequest",      /* fxnName */
    ((xdc_Int32 (*)(xdc_UInt32, xdc_UInt32 *)) ((xdc_Fxn) OmxRpc_rcmIfSkelOmxApi)),     /* fxnPtr 
                                                                                         */
    OmxRpc_remoteFxnCtrl,       /* fxnCategory */
    OmxRpc_cmdComponentTunnelRequest,   /* omxCmdId */
    (xdc_UInt32) OMXRPC_FXNIDX_INVALID, /* fxnIdx */
   },                          /* [0] */
  },                           /* [18] */
  {
   {
    "OmxRpcCreateProxyLite",    /* fxnName */
    ((xdc_Int32 (*)(xdc_UInt32, xdc_UInt32 *)) ((xdc_Fxn) OmxRpc_rcmIfSkelOmxApi)),     /* fxnPtr 
                                                                                         */
    OmxRpc_remoteFxnInit,       /* fxnCategory */
    OmxRpc_cmdCreateProxyLite,  /* omxCmdId */
    (xdc_UInt32) OMXRPC_FXNIDX_INVALID, /* fxnIdx */
   },                          /* [0] */
  },                           /* [19] */
  {
   {
    "OmxRpcGetHeapMemStats",    /* fxnName */
    ((xdc_Int32 (*)(xdc_UInt32, xdc_UInt32 *)) ((xdc_Fxn) OmxRpc_rcmIfSkelGetHeapMemStats)),    /* fxnPtr 
                                                                                                 */
    OmxRpc_remoteFxnInit,       /* fxnCategory */
    OmxRpc_cmdGetHeapMemStats,  /* omxCmdId */
    (xdc_UInt32) OMXRPC_FXNIDX_INVALID, /* fxnIdx */
   },                          /* [0] */
  },                           /* [20] */
  {
   {
    "OmxRpcDeleteProxyLite",    /* fxnName */
    ((xdc_Int32 (*)(xdc_UInt32, xdc_UInt32 *)) ((xdc_Fxn) OmxRpc_rcmIfSkelOmxApi)),     /* fxnPtr 
                                                                                         */
    OmxRpc_remoteFxnInit,       /* fxnCategory */
    OmxRpc_cmdDeleteProxyLite,  /* omxCmdId */
    (xdc_UInt32) OMXRPC_FXNIDX_INVALID, /* fxnIdx */
   },                          /* [0] */
  }                            /* [21] */
};

/* Structure that contains RPC function registry */
const OmxRpc_rcmFxnRegistryTbl_CT OmxRpc_rcmFxnRegistryTbl =
{
  OMXRPC_RCMFXNREGISTRYTABLE_LENGTH,
  ((OmxRpc_rcmFxnRegistryAry *) OmxRpc_rcmFxnRegistryTblArr)
};

/* Set whether MSG Heaps have to be created during RPC */
const xdc_Bool OmxRpc_createMsgHeaps = DOMX_OMXRPC_CREATEMSGHEAPS;

/* String that contains RPC server name prefix */
const xdc_String OmxRpc_rcmServerNamePrefix = DOMX_OMXRPC_RCMSVRNAMEPREFIX;

/* Maximum number of queue entries */
const xdc_UInt16 OmxRpc_maxQueuedBufLen = DOMX_OMXRPC_MAXQUEUEDBUFLEN;

/* Priority of BIOS task which runs RCM server */
const xdc_Int OmxRpc_rcmCoreServerPriority = DOMX_OMXRPC_RCMCORESVRPRI;

/* Global structure to hold the instance */
OmxRpc_Module_State OmxRpc_module_inst;
OmxRpc_Module_State *OmxRpc_module = &OmxRpc_module_inst;

/* MSG Heap IDs for each of the cores are set below. Please note that the value
   for a given core changes when this file is built for different cores. For
   example, DOMX_OMXRPC_MSGHEAPID_CORE0 could be (0) when this file is built for
   m3video core and (1) when built for m3vpss. The defines used below are
   ideally set in app_cfg.h file that belongs to the application */
const xdc_UInt16 OmxRpc_msgHeapId_core0 =
  (xdc_UInt16) DOMX_OMXRPC_MSGHEAPID_CORE0;
const xdc_UInt16 OmxRpc_msgHeapId_core1 =
  (xdc_UInt16) DOMX_OMXRPC_MSGHEAPID_CORE1;
const xdc_UInt16 OmxRpc_msgHeapId_core2 =
  (xdc_UInt16) DOMX_OMXRPC_MSGHEAPID_CORE2;
const xdc_UInt16 OmxRpc_msgHeapId_core3 =
  (xdc_UInt16) DOMX_OMXRPC_MSGHEAPID_CORE3;

/*
 * DomxCore
 */

/* 
 * OMX component information table:
 *   Each OMX component has an entry. They are conditionally compiled in based
 *   on whether they are in the build or not. Each entry for an OMX component
 *   have the following elements defined:
 *     - Name of the component (String)
 *     - CoreID of the core on which the component runs 
 *     - Priority of the BIOS task which runs Control RCM server
 *     - Priority of the BIOS task which runs CB RCM server
 *     - Maximum number of buffers
 *
 */
const DomxCore_componentCoreInfoEntry
  DomxCore_componentCoreInfoTblEntries[] =
{
#ifdef _BUILD_venc_
  {
   OMX_VIDENC_COMPONENT_NAME,
   DOMX_CORE_COMPINFOTBL_COREID_venc,
   DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_venc,
   DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_venc,
   DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_venc,
  },
#endif
#ifdef _BUILD_vdec_
  {
   OMX_VIDDEC_COMPONENT_NAME,
   DOMX_CORE_COMPINFOTBL_COREID_vdec,
   DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_vdec,
   DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_vdec,
   DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_vdec,
  },
#endif
#ifdef _BUILD_vfcc_
  {
   OMX_VFCC_COMP_NAME,
   DOMX_CORE_COMPINFOTBL_COREID_vfcc,
   DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_vfcc,
   DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_vfcc,
   DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_vfcc,
  },
#endif
#ifdef _BUILD_vswmosaic_
  {
   OMX_VSWMOSAIC_COMP_NAME,
   DOMX_CORE_COMPINFOTBL_COREID_vswmosaic,
   DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_vswmosaic,
   DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_vswmosaic,
   DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_vswmosaic,
  },
#endif

#ifdef _BUILD_vfdc_
  {
   OMX_VFDC_COMP_NAME,
   DOMX_CORE_COMPINFOTBL_COREID_vfdc,
   DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_vfdc,
   DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_vfdc,
   DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_vfdc,
  },
#endif
#ifdef _BUILD_vfpc_
  {
   OMX_VFPC_DEIDUALOUT_HIGH_COMP_NAME,
   DOMX_CORE_COMPINFOTBL_COREID_vfpc_deih,
   DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_vfpc_deih,
   DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_vfpc_deih,
   DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_vfpc_deih,
  },
#endif
#ifdef _BUILD_vfpc_
  {
   OMX_VFPC_DEIDUALOUT_MEDIUM_COMP_NAME,
   DOMX_CORE_COMPINFOTBL_COREID_vfpc_deim,
   DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_vfpc_deim,
   DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_vfpc_deim,
   DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_vfpc_deim,
  },
#endif
#ifdef _BUILD_vfpc_
  {
   OMX_VFPC_NF_COMP_NAME,
   DOMX_CORE_COMPINFOTBL_COREID_vfpc_nf,
   DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_vfpc_nf,
   DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_vfpc_nf,
   DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_vfpc_nf,
  },
#endif
#ifdef _BUILD_vfpc_
  {
   OMX_VFPC_INDTXSCWB_COMP_NAME,
   DOMX_CORE_COMPINFOTBL_COREID_vfpc_indtxscwb,
   DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_vfpc_indtxscwb,
   DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_vfpc_indtxscwb,
   DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_vfpc_indtxscwb,
  },
#endif
#ifdef _BUILD_vsrc_
  {
   OMX_VSRC_COMP_NAME,
   DOMX_CORE_COMPINFOTBL_COREID_vsrc,
   DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_vsrc,
   DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_vsrc,
   DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_vsrc,
  },
#endif
#ifdef _BUILD_vsnk_
  {
   OMX_VSNK_COMP_NAME,
   DOMX_CORE_COMPINFOTBL_COREID_vsnk,
   DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_vsnk,
   DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_vsnk,
   DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_vsnk,
  },
#endif
#ifdef _BUILD_ctrl_
  {
   OMX_CTRL_TVP_COMP_NAME,
   DOMX_CORE_COMPINFOTBL_COREID_ctrl,
   DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_ctrl,
   DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_ctrl,
   DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_ctrl,
  },
#endif
#ifdef _BUILD_ctrl_
  {
   OMX_CTRL_DC_COMP_NAME,
   DOMX_CORE_COMPINFOTBL_COREID_ctrldc,
   DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_ctrldc,
   DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_ctrldc,
   DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_ctrldc,
  },
#endif
#ifdef _BUILD_cmux_
  {
   OMX_CMUX_COMP_NAME,
   DOMX_CORE_COMPINFOTBL_COREID_cmux,
   DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_cmux,
   DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_cmux,
   DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_cmux,
  },
#endif
#ifdef _BUILD_dmux_
  {
   OMX_DMUX_COMP_NAME,
   DOMX_CORE_COMPINFOTBL_COREID_dmux,
   DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_dmux,
   DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_dmux,
   DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_dmux,
  },
#endif
#ifdef _BUILD_rtptx_
  {
   OMX_RTPTX_COMP_NAME,
   DOMX_CORE_COMPINFOTBL_COREID_rtptx,
   DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_rtptx,
   DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_rtptx,
   DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_rtptx,
  },
#endif
#ifdef _BUILD_rtprx_
  {
   OMX_RTPRX_COMP_NAME,
   DOMX_CORE_COMPINFOTBL_COREID_rtprx,
   DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_rtprx,
   DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_rtprx,
   DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_rtprx,
  },
#endif
  #ifdef _BUILD_vsplc_
  {
    OMX_VIDSPLC_COMPONENT_NAME,
    DOMX_CORE_COMPINFOTBL_COREID_vsplc,
    DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_vsplc,
    DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_vsplc,
    DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_vsplc,
  },
  #endif
#ifdef _BUILD_vlpb_
  {
  OMX_VLPB_HOSTCOMP_NAME,
   DomxTypes_coreCortexA8,
   DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_vlpb,
   DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_vlpb,
   DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_vlpb,
  },
#endif
#ifdef _BUILD_vlpb_
  {
   OMX_VLPB_DSPCOMP_NAME,
   DomxTypes_coreDsp,
   DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_vlpb,
   DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_vlpb,
   DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_vlpb,
  },
#endif

#ifdef _BUILD_adec_
  {
    OMX_AUDDEC_COMPONENT_NAME,
    DOMX_CORE_COMPINFOTBL_COREID_auddec,
    DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_auddec,
    DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_auddec,
    DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_auddec,
  },
#endif

#ifdef _BUILD_aenc_
  {
	OMX_AUDENC_COMPONENT_NAME,
	DOMX_CORE_COMPINFOTBL_COREID_audenc,
	DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_audenc,
	DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_audenc,
	DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_audenc,
  },
#endif
};

/* Structure that contains the OMX component info table */

const DomxCore_componentCoreInfoTbl_CT DomxCore_componentCoreInfoTbl =
{
  sizeof (DomxCore_componentCoreInfoTblEntries)/sizeof (DomxCore_componentCoreInfoEntry),
  ((DomxCore_componentCoreInfo *) DomxCore_componentCoreInfoTblEntries)
};

/* IPC info table. This table would define which cores are going to 
   communicate with other cores. For example, if ipcEnable is enabled for
   two cores, each tries to "ipc-attach" the other core during the
   initialization */
const DomxCore_ipcInfo DomxCore_ipcInfoTbl[DomxTypes_coreCount] =
{
  {
   DOMX_CORE_IPCINFO_CORE0_ENABLE_SWITCH,       /* ipcEnable */
   (xdc_UInt16) DomxTypes_coreDsp,      /* multiProcId */
  },                           /* [0] */
  {
   DOMX_CORE_IPCINFO_CORE1_ENABLE_SWITCH,       /* ipcEnable */
   (xdc_UInt16) DomxTypes_coreCortexVideoM3,    /* multiProcId */
  },                           /* [1] */
  {
   DOMX_CORE_IPCINFO_CORE2_ENABLE_SWITCH,       /* ipcEnable */
   (xdc_UInt16) DomxTypes_coreCortexHdvpssM3,   /* multiProcId */
  },                           /* [2] */
  {
   DOMX_CORE_IPCINFO_CORE3_ENABLE_SWITCH,       /* ipcEnable */
   (xdc_UInt16) DomxTypes_coreCortexA8, /* multiProcId */
  },                           /* [3] */
};

/* Flag to set whether procInit has to happen for the core or not */
const xdc_Bool DomxCore_doProcInit = DOMX_CORE_DOPROCINIT;

/* Number of slave cores in the system */
#define DOMX_CORE_NUMSLAVECORES       (3)

/* Table containing executable names for each of the slave cores. Currently
   it is set statically */
const DomxCore_slaveCoreExecutableInfoElem
  DomxCore_slaveCoreExecutableInfoElemArr[DOMX_CORE_NUMSLAVECORES] =
{
  {
   DOMX_CORE_SLAVECORE_M3VIDEO_EXEC_NAME,       /* executableName */
   DomxTypes_coreCortexVideoM3, /* coreId */
   "VIDEO-M3",                  /* multiProcName */
  },                           /* [0] */
  {
   DOMX_CORE_SLAVECORE_M3VPSS_EXEC_NAME,        /* executableName */
   DomxTypes_coreCortexHdvpssM3,        /* coreId */
   "VPSS-M3",                   /* multiProcName */
  },                           /* [1] */
  {
   DOMX_CORE_SLAVECORE_C6XDSP_EXEC_NAME,        /* executableName */
   DomxTypes_coreDsp,           /* coreId */
   "DSP",                       /* multiProcName */
  },                           /* [2] */
};

/* Structure containing slave core executable information */
const DomxCore_slaveCoreExecutableInfo DomxCore_slaveCoreExecutableInfoTbl = {
  DOMX_CORE_NUMSLAVECORES,
  ((DomxCore_slaveCoreExecutableInfoElem *)
   DomxCore_slaveCoreExecutableInfoElemArr)
};

/* Global instance structure */
DomxCore_Module_State DomxCore_module_obj = {0};
DomxCore_Module_State *DomxCore_module = &DomxCore_module_obj;

/*
 * DomxTunnelMgr
 */

/* Maximum OMX components that the tunnel manager can handle */
const xdc_UInt32 DomxTunnelMgr_maxNumComponents = DOMX_TUNNELMGR_MAXNUMCOMPS;

DomxTunnelMgr_connectionInfoAry
  DomxTunnelMgr_connectionInfoArr[DOMX_TUNNELMGR_MAXNUMCOMPS];

/* DOMX tunnel manager instance */
DomxTunnelMgr_Module_State DomxTunnelMgr_module_inst =
{
  DOMX_TUNNELMGR_MODSTATE_INITDONE,
  DOMX_TUNNELMGR_MODSTATE_NUMACTIVECOMPS,
  {
   DOMX_TUNNELMGR_MAXNUMCOMPS,
   DomxTunnelMgr_connectionInfoArr,
  },
};
DomxTunnelMgr_Module_State *DomxTunnelMgr_module = &DomxTunnelMgr_module_inst;

/* 
 * OmxProxy
 */

const xdc_runtime_IHeap_Handle OmxProxy_Module_heap_obj = NULL;

OmxProxy_configParamStructInfo customConfigInfo_0[] =
{
  /* Table terminator */
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexMax, 0}
};
OmxProxy_configParamStructInfo customConfigInfo_1[] =
{
  /* Table terminator */
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexMax, 0}
};
OmxProxy_configParamStructInfo customConfigInfo_2[] =
{
  /* Table terminator */
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexMax, 0}
};
OmxProxy_configParamStructInfo customConfigInfo_3[] =
{
  /* Table terminator */
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexMax, 0}
};
OmxProxy_configParamStructInfo customConfigInfo_4[] =
{
  /* Table terminator */
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexMax, 0}
};
OmxProxy_configParamStructInfo customConfigInfo_5[] =
{
  /* Table terminator */
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexMax, 0}
};
OmxProxy_configParamStructInfo customConfigInfo_6[] =
{
  /* Table terminator */
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexMax, 0}
};
OmxProxy_configParamStructInfo customConfigInfo_7[] =
{
  /* Table terminator */
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexMax, 0}
};
OmxProxy_configParamStructInfo customConfigInfo_8[] =
{
  /* Table terminator */
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexMax, 0}
};
OmxProxy_configParamStructInfo customConfigInfo_9[] =
{
  /* Table terminator */
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexMax, 0}
};
OmxProxy_configParamStructInfo customConfigInfo_10[] =
{
  /* Table terminator */
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexMax, 0}
};
OmxProxy_configParamStructInfo customConfigInfo_11[] =
{
  /* Table terminator */
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexMax, 0}
};
OmxProxy_configParamStructInfo customConfigInfo_12[] =
{
  /* Table terminator */
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexMax, 0}
};
OmxProxy_configParamStructInfo customConfigInfo_13[] =
{
  /* Table terminator */
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexMax, 0}
};
OmxProxy_configParamStructInfo customConfigInfo_14[] =
{
  /* Table terminator */
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexMax, 0}
};
OmxProxy_configParamStructInfo customConfigInfo_15[] =
{
  /* Table terminator */
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexMax, 0}
};
OmxProxy_configParamStructInfo customConfigInfo_16[] =
{
  /* Table terminator */
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexMax, 0}
};

OmxProxy_configParamStructInfo customConfigInfo_17[] =
{
  /* Table terminator */
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexMax, 0}
};

OmxProxy_configParamStructInfo customConfigInfo_18[] =
{
  /* Table terminator */
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexMax, 0}
};

OmxProxy_configParamStructInfo customConfigInfo_19[] =
{
  /* Table terminator */
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexMax, 0}
};

OmxProxy_configParamStructInfo customConfigInfo_20[] =
{
  /* Table terminator */
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexMax, 0}
};

OmxProxy_configParamStructInfo customConfigInfo_21[] =
{
  /* Table terminator */
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexMax, 0}
};

OmxProxy_configParamStructInfoMap OmxProxy_omxCustomStructInfoMapArray[] =
{
#ifdef DOMX_OMXPROXY_VFCC
  {
   OMX_VFCC_COMP_NAME,
   customConfigInfo_0,
  },
#endif
#ifdef DOMX_OMXPROXY_VFDC
  {
   OMX_VFDC_COMP_NAME,
   customConfigInfo_1,
  },
#endif
#ifdef DOMX_OMXPROXY_VFPC_DEIH
  {
   OMX_VFPC_DEIDUALOUT_HIGH_COMP_NAME,
   customConfigInfo_2,
  },
#endif
#ifdef DOMX_OMXPROXY_VFPC_DEIM
  {
   OMX_VFPC_DEIDUALOUT_MEDIUM_COMP_NAME,
   customConfigInfo_3,
  },
#endif
#ifdef DOMX_OMXPROXY_VFPC_NF
  {
   OMX_VFPC_NF_COMP_NAME,
   customConfigInfo_4,
  },
#endif
#ifdef DOMX_OMXPROXY_VFPC_INDTXSCWB
  {
   OMX_VFPC_INDTXSCWB_COMP_NAME,
   customConfigInfo_5,
  },
#endif
#ifdef DOMX_OMXPROXY_VSRC
  {
   OMX_VSRC_COMP_NAME,
   customConfigInfo_6,
  },
#endif
#ifdef DOMX_OMXPROXY_VSNK
  {
   OMX_VSNK_COMP_NAME,
   customConfigInfo_7,
  },
#endif
#ifdef DOMX_OMXPROXY_CTRL
  {
   OMX_CTRL_TVP_COMP_NAME,
   customConfigInfo_8,
  },
#endif
#ifdef DOMX_OMXPROXY_CTRLDC
  {
   OMX_CTRL_DC_COMP_NAME,
   customConfigInfo_9,
  },
#endif
#ifdef DOMX_OMXPROXY_VENC
  {
   OMX_VIDENC_COMPONENT_NAME,
   customConfigInfo_10,
  },
#endif
#ifdef DOMX_OMXPROXY_VDEC
  {
   OMX_VIDDEC_COMPONENT_NAME,
   customConfigInfo_11,
  },
#endif
#ifdef DOMX_OMXPROXY_CMUX
  {
   OMX_CMUX_COMP_NAME,
   customConfigInfo_12,
  },
#endif
#ifdef DOMX_OMXPROXY_DMUX
  {
   OMX_DMUX_COMP_NAME,
   customConfigInfo_13,
  },
#endif
#ifdef DOMX_OMXPROXY_RTPTX
  {
   OMX_RTPTX_COMP_NAME,
   customConfigInfo_14,
  },
#endif
#ifdef DOMX_OMXPROXY_RTPRX
  {
   OMX_RTPRX_COMP_NAME,
   customConfigInfo_15,
  },
#endif
#ifdef DOMX_OMXPROXY_VLPB
  {
   OMX_VLPB_HOSTCOMP_NAME,
   customConfigInfo_16,
  },
#endif
#ifdef DOMX_OMXPROXY_VLPB
  {
   OMX_VLPB_DSPCOMP_NAME,
   customConfigInfo_17,
  },
#endif  
#ifdef DOMX_OMXPROXY_AUDDEC
  {
    OMX_AUDDEC_COMPONENT_NAME,
    customConfigInfo_18,
  },
#endif
#ifdef DOMX_OMXPROXY_AUDENC
  {
    OMX_AUDENC_COMPONENT_NAME,
    customConfigInfo_19,
  },
#endif
#ifdef DOMX_OMXPROXY_VSWMOSAIC
  {
   OMX_VSWMOSAIC_COMP_NAME,
   customConfigInfo_20,
  },
#endif
 #ifdef DOMX_OMXPROXY_VSPLC
  {
    OMX_VIDSPLC_COMPONENT_NAME,
    customConfigInfo_21,
  },
 #endif
  {
   NULL,                        /* Table Terminator . Should be last entry */
   NULL
  }
};

OmxProxy_configParamStructInfoMap *OmxProxy_omxCustomStructInfoMap =
  &OmxProxy_omxCustomStructInfoMapArray[0];

/* Nothing beyond this point */
