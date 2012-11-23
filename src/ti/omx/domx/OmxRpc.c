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
 *  @file  OmxRpc.c
 *         This file contains methods that provides the functionality for
 *         Remote procedure call (RPC) in DOMX
 *
 *  @path \avmecomp\ti\omx\domx\OmxRpc.c
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
#include <assert.h>

#include <xdc/runtime/Gate.h>
#include <xdc/runtime/Startup.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/IGateProvider.h>
#include <xdc/runtime/Gate.h>
#include <xdc/runtime/IHeap.h>
#include <xdc/runtime/knl/GateThread.h>
#include <xdc/runtime/knl/Thread.h>
/* Log header files */
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Diags.h>
#include <ti/sdo/rcm/RcmServer.h>
#include <ti/sdo/rcm/RcmClient.h>
#include <ti/omx/interfaces/openMaxv11/OMX_Core.h>
#include <ti/omx/interfaces/openMaxv11/interfaces_openmaxv11_internal.h>
#include <ti/omx/domx/interfaces/domx_interfaces_internal.h>
#include <ti/omx/domx/delegates/shmem/domx_delegates_shmem_internal.h>
#include "domx_internal.h"
#include "domx_internalcfg.h"
#include "OmxRpc__epilogue.h"
#include "domx_util.h"
/*  ----------------------------------- Utils Headers                */
#include <ti/ipc/MultiProc.h>
#include <ti/ipc/MessageQ.h>



/*
 * ****************************************************************************
 * \def   OMXRPC_RCM_CLIENT_MSG_INTERNAL_SIZE
 * \brief Additional size of RCM message required for RCM internal msg structure
 * \todo  This value should come from RCM API query and should not be defined
 *         in OmxRpc layer.
 * TODO: Remove this hack
 * ****************************************************************************
 */
#define OMXRPC_RCM_CLIENT_MSG_INTERNAL_SIZE                              (0x100)

/*
 *************************************************************************
 *                       Static functions
 *************************************************************************
 */
/**
 * @def   OMXRPC_RSRC_MODULE_SRVR_INIT_NONE 
 * @def   OMXRPC_RSRC_MODULE_SRVR_INIT_RCMSVR
 * @def   OMXRPC_RSRC_MODULE_SRVR_INIT_RCMFXNREGISTER
 * @def   OMXRPC_RSRC_MODULE_SRVR_INIT_RCMSTART
 * @def   OMXRPC_RSRC_MODULE_SRVR_INIT_ALL
 * @brief Defines of resources allocated at OmxRpc Module Rcm Server Init time
 */

/* Bit values for each of the init types */

/* Value 0x0 - binary value: 0000 0000 */
#define OMXRPC_RSRC_MODULE_SRVR_INIT_NONE               0

/* Value 0x1 - binary value: 0000 0001 */
#define OMXRPC_RSRC_MODULE_SRVR_INIT_RCMSVR            (1 << 0)

/* Value 0x2 - binary value: 0000 0010 */
#define OMXRPC_RSRC_MODULE_SRVR_INIT_RCMFXNREGISTER    (1 << 1)

/* Value 0x4 - binary value: 0000 0100 */
#define OMXRPC_RSRC_MODULE_SRVR_INIT_RCMSTART          (1 << 2)

/* Value 0x7 - binary value: 0000 0111 */
#define OMXRPC_RSRC_MODULE_SRVR_INIT_ALL                                       \
                        (OMXRPC_RSRC_MODULE_SRVR_INIT_RCMSVR         |         \
                         OMXRPC_RSRC_MODULE_SRVR_INIT_RCMFXNREGISTER |         \
                         OMXRPC_RSRC_MODULE_SRVR_INIT_RCMSTART)

/**
 * @name omxrpc_module_free_server_rsrc
 * @brief Free OmxRpc Module rcm server init allocated resources
 * @param  rsrcMask           : bit mask indicating resources allocated
 * @return OmxRpc_errorNone : SUCCESS
 */
static OmxRpc_errorType omxrpc_module_free_server_rsrc (Int rsrcMask)
{
  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();

  if (OMXRPC_RSRC_MODULE_SRVR_INIT_RCMSVR & rsrcMask)
  {
    DOMX_UTL_TRACE_FUNCTION_MSG ("Before RcmServer_delete");
    RcmServer_delete ((RcmServer_Handle *) (&OmxRpc_module->
                                            localCoreRcmServer.handle));
    DOMX_UTL_TRACE_FUNCTION_MSG ("After RcmServer_delete");
  }

  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (OmxRpc_errorNone);

  return OmxRpc_errorNone;
}

/**
 * @def   OMXRPC_RSRC_MODULE_CLIENT_INIT_NONE 
 * @def   OMXRPC_RSRC_MODULE_CLIENT_INIT_MSGQHEAP
 * @def   OMXRPC_RSRC_MODULE_CLIENT_INIT_RCM
 * @def   OMXRPC_RSRC_MODULE_CLIENT_INIT_ALL
 * @brief Defines of resources allocated at OmxRpc Module Rcm Server Init time
 */
/* Bit values for each of the init types */

/* Value 0x0 - binary value: 0000 0000 */
#define OMXRPC_RSRC_MODULE_CLIENT_INIT_NONE      0

/* Value 0x1 - binary value: 0000 0001 */
#define OMXRPC_RSRC_MODULE_CLIENT_INIT_MSGQHEAP  (1 << 0)

/* Value 0x2 - binary value: 0000 0010 */
#define OMXRPC_RSRC_MODULE_CLIENT_INIT_RCM       (1 << 1)

/* Value 0x3 - binary value: 0000 0011 */
#define OMXRPC_RSRC_MODULE_CLIENT_INIT_ALL       (OMXRPC_RSRC_MODULE_CLIENT_INIT_MSGQHEAP | \
                                                  OMXRPC_RSRC_MODULE_CLIENT_INIT_RCM)

/**
 * @name omxrpc_module_free_client_rsrc
 * @brief Free OmxRpc Module rcm client init allocated resources
 * @param  rsrcMask           : bit mask indicating resources allocated
 * @return OmxRpc_errorNone : SUCCESS
 */
static OmxRpc_errorType omxrpc_module_free_client_rsrc (DomxTypes_coreType
                                                          remoteCoreId,
                                                        Int rsrcMask)
{
  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();
  if (OMXRPC_RSRC_MODULE_CLIENT_INIT_RCM & rsrcMask)
  {
    DOMX_UTL_TRACE_FUNCTION_MSG ("Before RcmClient_delete");
    RcmClient_delete ((RcmClient_Handle *) (&OmxRpc_module->
                                            remoteCoreRcmClient
                                            [remoteCoreId].handle));
    DOMX_UTL_TRACE_FUNCTION_MSG ("After RcmClient_delete");
  }
  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (OmxRpc_errorNone);
  return OmxRpc_errorNone;
}

/**
 * @name omxrpc_rcm_server_create
 * @brief OmxRpc function to create an RcmServer
 * @param  rcmSrvHandlePtr : RcmServer Handle populated by RcmServer_create
 * @param  rcmServerName   : RcmServer name
 * @param  priority        : RcmServer thread prirority
 * @return OmxRpc_errorNone : SUCCESS
 */
static OmxRpc_errorType omxrpc_rcm_server_create (RcmServer_Handle
                                                    *rcmSrvHandlePtr,
                                                  String rcmServerName,
                                                  Int priority)
{
  Int status;
  OmxRpc_errorType retVal = OmxRpc_errorNone;
  RcmServer_Params rcmSrvParams;

  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();

  Log_print3 (Diags_USER1, "@ %s: rcmServerName %s, priority %d",
              (xdc_IArg) __FUNCTION__, (xdc_IArg) rcmServerName, priority);

  DOMX_UTL_TRACE_FUNCTION_MSG ("Before RcmServer_Params_init");

  RcmServer_Params_init (&rcmSrvParams);

  DOMX_UTL_TRACE_FUNCTION_MSG ("After RcmServer_Params_init");
  /* TODO what does priority for RcmServer mean ? */
  rcmSrvParams.osPriority = priority;
  DOMX_UTL_TRACE_FUNCTION_MSG ("Before RcmServer_create");
  status = RcmServer_create (rcmServerName, &rcmSrvParams, rcmSrvHandlePtr);
  if (RcmServer_S_SUCCESS != status)
  {
    DOMX_UTL_TRACE_FUNCTION_MSG ("RcmServer_create failed");
    retVal = OmxRpc_errorResourceAllocFail;
  }

  DOMX_UTL_TRACE_FUNCTION_MSG ("After RcmServer_create");
  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (retVal);
  return retVal;
}

/**
 * @name omxrpc_rcm_server_start
 * @brief OmxRpc function to start an RcmServer
 * @param  rcmSrvHandle    : RcmServer Handle 
 * @return OmxRpc_errorNone : SUCCESS
 */
static OmxRpc_errorType omxrpc_rcm_server_start (RcmServer_Handle rcmSrvHandle)
{
  OmxRpc_errorType retVal = OmxRpc_errorNone;

  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();
  DOMX_UTL_TRACE_FUNCTION_MSG ("Before RcmServer_start");
  RcmServer_start (rcmSrvHandle);
  DOMX_UTL_TRACE_FUNCTION_MSG ("After RcmServer_start");
  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (retVal);
  return retVal;
}

/**
 * @name omxrpc_rcm_server_remote_fxn_register
 * @brief OmxRpc function to register a fxn with  RcmServer
 * @param  rcmSrvHandle    : RcmServer Handle 
 * @param  regFxnCategory    : Remote function category (RemoteCore or RemoteInstance) 
 * @return OmxRpc_errorNone : SUCCESS
 */
static OmxRpc_errorType
  omxrpc_rcm_server_remote_fxn_register (RcmServer_Handle rcmHndl,
                                         OmxRpc_remoteFxnCategory
                                           regFxnCategory)
{
  Int i;
  Int status;
  OmxRpc_errorType retVal = OmxRpc_errorNone;

  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();
  Log_print2 (Diags_USER1, "@ %s regFxnCategory %d",
              (xdc_IArg) __FUNCTION__, regFxnCategory);

  /* must initialize the error block before using it */
  for (i = 0; i < OmxRpc_rcmFxnRegistryTbl.length; i++)
  {
    if (OmxRpc_rcmFxnRegistryTbl.elem[i]->fxnCategory == regFxnCategory)
    {
      Log_print1 (Diags_USER1, "Calling RcmServer_addSymbol(%s)",
                  (xdc_IArg) OmxRpc_rcmFxnRegistryTbl.elem[i]->fxnName);
      status = RcmServer_addSymbol (rcmHndl,
                                    OmxRpc_rcmFxnRegistryTbl.elem[i]->fxnName,
                                    OmxRpc_rcmFxnRegistryTbl.elem[i]->fxnPtr,
                                    &(OmxRpc_rcmFxnRegistryTbl.elem[i]->
                                      fxnIdx));
      if (RcmServer_S_SUCCESS != status)
      {
        Log_print2 (Diags_INTERNAL, "RcmServer_addSymbol failed:%s,%d",
                    (xdc_IArg) OmxRpc_rcmFxnRegistryTbl.elem[i]->fxnName,
                    status);
        retVal = OmxRpc_errorResourceAllocFail;
        break;
      }
    }
  }

  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (retVal);

  return retVal;
}

/**
 * @name omxrpc_module_init_client
 * @brief OmxRpc function to init module level rcmCLient
 * @param  remoteCoreId      : Remote coreID of RcmClient
 * @return OmxRpc_errorNone : SUCCESS
 */
static OmxRpc_errorType omxrpc_module_init_client (DomxTypes_coreType
                                                     remoteCoreId)
{
  IArg key;
  OmxRpc_errorType retVal = OmxRpc_errorNone;
  Int rsrcMask = OMXRPC_RSRC_MODULE_CLIENT_INIT_NONE;

  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();
  Log_print2 (Diags_USER1, "Entered function:%s (%d)", (xdc_IArg) __FUNCTION__,
              remoteCoreId);
  /* Ensure RpcServer Init is done */
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((TRUE ==
                                   OmxRpc_module->localCoreRcmServer.initDone),
                                  "serverInitNotDone");
  if (((int) remoteCoreId < DomxTypes_coreFirst) ||
      ((int) remoteCoreId > DomxTypes_coreLast))
  {
    retVal = OmxRpc_errorBadArgument;
  }
  else if (TRUE == OmxRpc_module->remoteCoreRcmClient[remoteCoreId].initDone)
  {
    /* Return if init is already done */
    DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (OmxRpc_errorNone);
    return OmxRpc_errorNone;
  }
  if (OmxRpc_errorNone == retVal)
  {
    RcmClient_Handle *rcmHndlPtr;

    key = Gate_enterModule ();
    OmxRpc_module->remoteCoreRcmClient[remoteCoreId].numInstances = 0;
    OmxRpc_genCoreRcmServerName ((char *) OmxRpc_module->
                                 remoteCoreRcmClient[remoteCoreId].name,
                                 remoteCoreId);
    rcmHndlPtr =
      (RcmClient_Handle *) & (OmxRpc_module->
                              remoteCoreRcmClient[remoteCoreId].handle);
    retVal =
      OmxRpc_rcmClientCreate (rcmHndlPtr,
                              (String) OmxRpc_module->
                              remoteCoreRcmClient[remoteCoreId].name,
                              OmxRpc_module->
                              remoteCoreRcmClient[remoteCoreId].msgHeap.
                              msgHeapId);
    if (OmxRpc_errorNone == retVal)
    {
      OmxRpc_remoteFxnEntryTbl *pFxnsTbl;

      Log_print1 (Diags_USER1, "%s: Located the remoteCoreRcmServer",
                  (xdc_IArg) __FUNCTION__);
      rsrcMask |= OMXRPC_RSRC_MODULE_CLIENT_INIT_RCM;
      /* get remote function index */
      pFxnsTbl = &(OmxRpc_module->remoteCoreRcmClient[remoteCoreId].fxns);
      pFxnsTbl->numEntries = 0;
      retVal = OmxRpc_rcmClientRemoteFxnLocate (*rcmHndlPtr,
                                                OmxRpc_remoteFxnInit, pFxnsTbl);
    }
    Gate_leaveModule (key);
  }
  if (OmxRpc_errorNone != retVal)
  {
    omxrpc_module_free_client_rsrc (remoteCoreId, rsrcMask);
  }
  else
  {
    key = Gate_enterModule ();
    OmxRpc_module->remoteCoreRcmClient[remoteCoreId].initDone = TRUE;
    Gate_leaveModule (key);
  }
  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (retVal);
  return retVal;
}

/**
 * @name omxrpc_module_deinit_client
 * @brief OmxRpc function to deinit module level rcmCLient
 * @param  remoteCoreId      : Remote coreID of RcmClient
 * @return OmxRpc_errorNone : SUCCESS
 */
static OmxRpc_errorType omxrpc_module_deinit_client (DomxTypes_coreType
                                                       remoteCoreId)
{
  OmxRpc_errorType retVal;

  retVal = omxrpc_module_free_client_rsrc (remoteCoreId,
                                           OMXRPC_RSRC_MODULE_CLIENT_INIT_ALL);
  if (OmxRpc_errorNone == retVal)
  {
    IArg key;

    key = Gate_enterModule ();
    OmxRpc_module->remoteCoreRcmClient[remoteCoreId].initDone = FALSE;
    Gate_leaveModule (key);
  }
  return retVal;
}

/**
 * @name omxrpc_module_register_instance
 * @brief OmxRpc function to register an instance with the module
 * @param  remoteCoreId      : Remote coreID of RpcInstance
 * @return OmxRpc_errorNone : SUCCESS
 */
static Void omxrpc_module_register_instance (DomxTypes_coreType remoteCoreId)
{
  IArg key;
  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((TRUE ==
                                   OmxRpc_module->localCoreRcmServer.initDone),
                                  "serverInitNotDone");
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((TRUE ==
                                   OmxRpc_module->
                                   remoteCoreRcmClient[remoteCoreId].initDone),
                                  "clientInitNotDone");
  key = Gate_enterModule ();
  OmxRpc_module->remoteCoreRcmClient[remoteCoreId].numInstances++;
  Log_print1 (Diags_USER1, "Instance Register with omxRpc Module.Cnt = %d",
              OmxRpc_module->remoteCoreRcmClient[remoteCoreId].numInstances);
  Gate_leaveModule (key);
  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (OMX_ErrorNone);
}

/**
 * @name omxrpc_module_unregister_instance
 * @brief OmxRpc function to unregister an rpc instance with the module
 * @param  remoteCoreId      : Remote coreID of RpcInstance
 * @return OmxRpc_errorNone : SUCCESS
 */
static Void omxrpc_module_unregister_instance (DomxTypes_coreType remoteCoreId)
{
  IArg key;

  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((TRUE ==
                                   OmxRpc_module->localCoreRcmServer.initDone),
                                  "serverInitNotDone");
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((TRUE ==
                                   OmxRpc_module->
                                   remoteCoreRcmClient[remoteCoreId].initDone),
                                  "clientInitNotDone");

  key = Gate_enterModule ();
  OmxRpc_module->remoteCoreRcmClient[remoteCoreId].numInstances--;

  /* Ensure that the number of instances is never negative. -ve value indicated 
   * register and unregister have gone out of sync */
  DOMX_UTL_TRACE_FUNCTION_ASSERT 
    ((OmxRpc_module->remoteCoreRcmClient[remoteCoreId].numInstances >= 0),
     "NumInstances < 0");

  Log_print1 (Diags_USER1, "Instance UnRegister with omxRpc Module.Cnt = %d",
              OmxRpc_module->remoteCoreRcmClient[remoteCoreId].numInstances);
  Gate_leaveModule (key);

  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (OMX_ErrorNone);
}

/**
 * @name omxrpc_module_get_instance_count
 * @brief OmxRpc function to get the number of active rpc instances
 * @param  remoteCoreId      : Remote coreID of RpcInstance
 * @return OmxRpc_errorNone : SUCCESS
 */
static Int omxrpc_module_get_instance_count (DomxTypes_coreType remoteCoreId)
{
  DOMX_UTL_TRACE_FUNCTION_ASSERT
    ((TRUE == OmxRpc_module->localCoreRcmServer.initDone),
     "LocalRcmServerInitNotDone");
  DOMX_UTL_TRACE_FUNCTION_ASSERT
    ((TRUE == OmxRpc_module->remoteCoreRcmClient[remoteCoreId].initDone),
     "RemoteRcmServerInitNotDone");
  return (OmxRpc_module->remoteCoreRcmClient[remoteCoreId].numInstances);
}

/*
 *************************************************************************
 *                       Instance functions
 *************************************************************************
 */
/**
 * @def    OMXRPC_RSRC_INSTANCE_CREATE_NONE
 * @def    OMXRPC_RSRC_INSTANCE_CREATE_MODULECLIENTINIT
 * @def    OMXRPC_RSRC_INSTANCE_CREATE_RCMSERVERCREATE
 * @def    OMXRPC_RSRC_INSTANCE_CREATE_RCMSERVERSTART
 * @def    OMXRPC_RSRC_INSTANCE_CREATE_ALL
 * @brief  Defines of resources allocated at OmxRpc Instance Init time 
 */

/* Bit values for each of the create types */

/* Value 0x0 - binary value: 0000 0000 */
#define OMXRPC_RSRC_INSTANCE_CREATE_NONE      0

/* Value 0x1 - binary value: 0000 0001 */
#define OMXRPC_RSRC_INSTANCE_CREATE_MODULECLIENTINIT      (1 << 0)

/* Value 0x2 - binary value: 0000 0010 */
#define OMXRPC_RSRC_INSTANCE_CREATE_RCMSERVERCREATE       (1 << 1)

/* Value 0x4 - binary value: 0000 0100 */
#define OMXRPC_RSRC_INSTANCE_CREATE_RCMSERVERSTART        (1 << 2)

/* Value 0x7 - binary value: 0000 0111 */
#define OMXRPC_RSRC_INSTANCE_CREATE_ALL                   (                    \
                             OMXRPC_RSRC_INSTANCE_CREATE_MODULECLIENTINIT | \
                                OMXRPC_RSRC_INSTANCE_CREATE_RCMSERVERCREATE  | \
                                OMXRPC_RSRC_INSTANCE_CREATE_RCMSERVERSTART)

#define OMXRPC_RSRC_INSTANCE_STUBLITE_CREATE_ALL          (                    \
                             OMXRPC_RSRC_INSTANCE_CREATE_MODULECLIENTINIT)

/**
 * @name omxrpc_instance_free_rsrc
 * @brief OmxRpc function to free resources allocated at instance init time
 * @param  obj      : Pointer to OmxRpc object
 * @param  rsrc_mask: Resource allocation bitmask
 * @return OmxRpc_errorNone : SUCCESS
 */
static void omxrpc_instance_free_rsrc (OmxRpc_Object *obj, Int rsrc_mask)
{

  if (rsrc_mask & OMXRPC_RSRC_INSTANCE_CREATE_RCMSERVERCREATE)
  {
    RcmServer_delete ((RcmServer_Handle *) (&obj->server.handle));
  }
  if (rsrc_mask & OMXRPC_RSRC_INSTANCE_CREATE_MODULECLIENTINIT)
  {
    /* If no other instances are alive, deinit the Module level client as well */
    if (omxrpc_module_get_instance_count (obj->remoteCoreId) == 0)
    {
      omxrpc_module_deinit_client (obj->remoteCoreId);
    }
  }
}

/**
 * @name   OmxRpc_Instance_init
 * @brief  OmxRpc instance initialization function
 * @param  obj      : Pointer to OmxRpc object
 * @param  params   : Pointer to create params structure
 * @param  eb       : Error block to raise errors
 * @return Int : 0 if successful
 */
Int OmxRpc_Instance_init (OmxRpc_Object *obj, const OmxRpc_Params *params,
                          Error_Block *eb)
{
  Int status = 0;               // Indicates success
  OmxRpc_errorType retVal = OmxRpc_errorNone;
  Int rsrcMask = OMXRPC_RSRC_INSTANCE_CREATE_NONE;
  /* Static variable used to generate unique instance IDs */
  static UInt16 instIDctr = 0;
  IArg key;

  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((TRUE ==
                                   OmxRpc_module->localCoreRcmServer.initDone),
                                  "serverInitNotDone");
  obj->client.fxns.numEntries = 0;
  obj->layerId = params->layerId;
  obj->remoteCoreId = params->remoteCoreId;
  /* Critical section around instID assignment and static counter increment */
  key = Gate_enterSystem ();
  obj->instID = instIDctr++;
  Gate_leaveSystem (key);
  DOMX_UTL_Strncpy ((OMX_STRING) obj->szComponentName,
                    (OMX_STRING) params->componentName,
                    sizeof (obj->szComponentName));
  retVal = omxrpc_module_init_client (params->remoteCoreId);
  if (OmxRpc_errorNone != retVal)
  {
    DOMX_UTL_TRACE_FUNCTION_ASSERT ((OmxRpc_errorNone == retVal),
                                    "omxrpc_module_init_client failed");
    /* TODO Assign proper return value to status */
    status = DOMX_INT_OMXRPC_STATUS_MASK + retVal;
  }
  if (OmxRpc_errorNone == retVal)
  {
    rsrcMask |= OMXRPC_RSRC_INSTANCE_CREATE_MODULECLIENTINIT;

    Log_print1 (Diags_USER1, "LayerId: %d\n", obj->layerId);
    switch (obj->layerId)
    {
      case OmxRpc_layerTypeStub:
        {
          obj->layer.stub.skelHandle = NULL;
          obj->server.priority = params->rcmEventCbServerPriority;
          /* For the stub layer, RPC client is the Omx Component. Store the
             priority for the remote component RCM server. Prority will be used 
             * when the remote component RCM server is created. */
          obj->layer.stub.skelRcmServerPriority =
            params->rcmOmxComponentServerPriority;
          OmxRpc_genComponentCbRcmServerName ((char *) obj->server.name,
                                              obj->szComponentName,
                                              DomxCore_localCoreId,
                                              obj->instID);
          retVal =
            omxrpc_rcm_server_create ((RcmServer_Handle *) & obj->server.
                                      handle, (String) obj->server.name,
                                      obj->server.priority);

          if (OmxRpc_errorNone == retVal)
          {
            rsrcMask |= OMXRPC_RSRC_INSTANCE_CREATE_RCMSERVERCREATE;
          }
          else
          {
            DOMX_UTL_TRACE_FUNCTION_ASSERT ((OmxRpc_errorNone == retVal),
                                            "rcmServerCreateFailed");
          }
          if (OmxRpc_errorNone == retVal)
          {
            retVal = omxrpc_rcm_server_remote_fxn_register (obj->server.handle,
                                                            OmxRpc_remoteFxnCb);
            DOMX_UTL_TRACE_FUNCTION_ASSERT ((OmxRpc_errorNone == retVal),
                                            "rcmServerFxnRegisterFailed");
          }
          if (OmxRpc_errorNone == retVal)
          {
            retVal = omxrpc_rcm_server_start (obj->server.handle);
            if (OmxRpc_errorNone == retVal)
            {
              rsrcMask |= OMXRPC_RSRC_INSTANCE_CREATE_RCMSERVERSTART;
            }
            else
            {
              DOMX_UTL_TRACE_FUNCTION_ASSERT ((OmxRpc_errorNone == retVal),
                                              "rcmServerStartFailed");
            }
          }
          break;
        }                       /* end of case OmxRpc_layerTypeStub */
      case OmxRpc_layerTypeStubLite:
        {
          obj->layer.stub.skelHandle = NULL;
          obj->server.priority = params->rcmEventCbServerPriority;
          /* For the stub layer, RPC client is the Omx Component. Store the
             priority for the remote component RCM server. Prority will be used 
             * when the remote component RCM server is created. */
          obj->layer.stub.skelRcmServerPriority =
            params->rcmOmxComponentServerPriority;
          OmxRpc_genComponentCbRcmServerName ((char *) obj->server.name,
                                              obj->szComponentName,
                                              DomxCore_localCoreId,
                                              obj->instID);
          break;
        }                       /* end of case OmxRpc_layerTypeStub */
      case OmxRpc_layerTypeSkel:
        {
          Int i;

          /* init the bufHdr to Cb map tbl for mapping emptyThisBuffer & *
             fillThisBuffer to respective done API cb. */
          for (i = 0; i < OmxRpc_maxQueuedBufLen; i++)
          {
            obj->layer.skel.bufHdrCbMapTbl[i].isValid = FALSE;
          }
          obj->server.priority = params->rcmOmxComponentServerPriority;
          OmxRpc_genComponentCtrlRcmServerName ((char *) obj->server.name,
                                                obj->szComponentName,
                                                DomxCore_localCoreId,
                                                obj->instID);
          retVal =
            omxrpc_rcm_server_create ((RcmServer_Handle *) & obj->server.
                                      handle, (String) obj->server.name,
                                      obj->server.priority);

          if (OmxRpc_errorNone == retVal)
          {
            rsrcMask |= OMXRPC_RSRC_INSTANCE_CREATE_RCMSERVERCREATE;
          }
          else
          {
            DOMX_UTL_TRACE_FUNCTION_ASSERT ((OmxRpc_errorNone == retVal),
                                            "rcmServerCreateFailed");
          }
          if (OmxRpc_errorNone == retVal)
          {
            retVal = omxrpc_rcm_server_remote_fxn_register (obj->server.handle,
                                                            OmxRpc_remoteFxnCtrl);
            DOMX_UTL_TRACE_FUNCTION_ASSERT ((OmxRpc_errorNone == retVal),
                                            "rcmServerFxnRegisterFailed");
          }
          if (OmxRpc_errorNone == retVal)
          {
            retVal = omxrpc_rcm_server_start (obj->server.handle);
            if (OmxRpc_errorNone == retVal)
            {
              rsrcMask |= OMXRPC_RSRC_INSTANCE_CREATE_RCMSERVERSTART;
            }
            else
            {
              DOMX_UTL_TRACE_FUNCTION_ASSERT ((OmxRpc_errorNone == retVal),
                                              "rcmServerStartFailed");
            }
          }
          break;
        }                       /* end of case OmxRpc_layerTypeSkel */
      default:
        {
          DOMX_UTL_TRACE_FUNCTION_ASSERT (0, "invalidLayerType");
          retVal = OmxRpc_errorInvalidLayerType;
          /* TODO Assign proper return value to status */
          status = DOMX_INT_OMXRPC_STATUS_MASK + retVal;
        }
    }                           /* end of switch (obj->layerId) */
  }
  if (OmxRpc_errorNone == retVal)
  {
    omxrpc_module_register_instance (obj->remoteCoreId);
    Log_print2 (Diags_USER1, "L_create: OmxRpc create: Component:%s, Layer:%d",
                (IArg) params->componentName, (IArg) params->layerId);
  }

  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (status);

  return status;
}

/**
 * @name   OmxRpc_Instance_finalize
 * @brief  OmxRpc instance finalization function
 * @param  obj      : Pointer to OmxRpc object
 * @param  status   : Status value - Not used
 * @return None
 */
Void OmxRpc_Instance_finalize (OmxRpc_Object *obj, Int status)
{
  Int rsrcMask = OMXRPC_RSRC_INSTANCE_CREATE_ALL;

  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();

  (Void) status;                /* Kill warnings */
  omxrpc_module_unregister_instance (obj->remoteCoreId);
  if (OmxRpc_layerTypeStubLite == obj->layerId)
  {
    rsrcMask = OMXRPC_RSRC_INSTANCE_STUBLITE_CREATE_ALL;
  }
  omxrpc_instance_free_rsrc (obj, rsrcMask);

  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (status);
}

/*
 *************************************************************************
 *                       Module functions
 *************************************************************************
 */
/**
 * @name   OmxRpc_Module_startup
 * @brief  OmxRpc Module startup function
 * @param  state    : Startup state (DONE/NOT_DONE)
 * @return Startup_DONE : Startup done if successful
 */
Int OmxRpc_Module_startup (Int state)
{
  if (state == Startup_DONE)
  {
    return (Startup_DONE);
  }

  return (Startup_DONE);
}

/**
 * @name   OmxRpc_moduleInitServer
 * @brief  OmxRpc Module init Rcm server function
 * @return OmxRpc_errorNone : SUCCESS
 */
OmxRpc_errorType OmxRpc_moduleInitServer (Void)
{
  Error_Block eb;
  IArg key;
  Int rsrcMask = OMXRPC_RSRC_MODULE_SRVR_INIT_NONE;
  OmxRpc_errorType retVal = OmxRpc_errorNone;
  Bool gateEntered = FALSE;

  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();

  if (TRUE == OmxRpc_module->localCoreRcmServer.initDone)
  {
    /* Return if init is already done */
    DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (OmxRpc_errorNone);
    return OmxRpc_errorNone;
  }
  /* must initialize the error block before using it */
  Error_init (&eb);
  if (OmxRpc_errorNone == retVal)
  {
    key = Gate_enterModule ();
    gateEntered = TRUE;
    /* create an rcm server instance */
    OmxRpc_genCoreRcmServerName ((char *) OmxRpc_module->localCoreRcmServer.
                                 name, DomxCore_localCoreId);
    retVal = omxrpc_rcm_server_create ((RcmServer_Handle
                                        *) (&OmxRpc_module->
                                            localCoreRcmServer.handle),
                                       (String) OmxRpc_module->
                                       localCoreRcmServer.name,
                                       OmxRpc_rcmCoreServerPriority);

    if (OmxRpc_errorNone == retVal)
    {
      rsrcMask |= OMXRPC_RSRC_MODULE_SRVR_INIT_RCMSVR;
    }
  }                             /* end of if (OmxRpc_errorNone == retVal) */
  if (OmxRpc_errorNone == retVal)
  {
    retVal =
      omxrpc_rcm_server_remote_fxn_register (OmxRpc_module->
                                             localCoreRcmServer.handle,
                                             OmxRpc_remoteFxnInit);
    if (OmxRpc_errorNone == retVal)
    {
      rsrcMask |= OMXRPC_RSRC_MODULE_SRVR_INIT_RCMFXNREGISTER;
    }
  }
  if (OmxRpc_errorNone == retVal)
  {
    retVal = omxrpc_rcm_server_start (OmxRpc_module->localCoreRcmServer.handle);
    if (OmxRpc_errorNone == retVal)
    {
      rsrcMask |= OMXRPC_RSRC_MODULE_SRVR_INIT_RCMSTART;
    }
  }
  if (OmxRpc_errorNone == retVal)
  {
    OmxRpc_module->localCoreRcmServer.initDone = TRUE;
    Gate_leaveModule (key);
  }
  else
  {
    omxrpc_module_free_server_rsrc (rsrcMask);
    if (TRUE == gateEntered)
    {
      Gate_leaveModule (key);
    }
  }
  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (retVal);
  return retVal;
}

/**
 * @name   OmxRpc_moduleDeinitServer
 * @brief  OmxRpc Module deInit Rcm server function
 * @return OmxRpc_errorNone : SUCCESS
 */
OmxRpc_errorType OmxRpc_moduleDeinitServer (Void)
{
  OmxRpc_errorType retVal;

  retVal = omxrpc_module_free_server_rsrc (OMXRPC_RSRC_MODULE_SRVR_INIT_ALL);
  if (OmxRpc_errorNone == retVal)
  {
    IArg key;

    key = Gate_enterSystem ();
    OmxRpc_module->localCoreRcmServer.initDone = FALSE;
    Gate_leaveSystem (key);
  }
  return retVal;
}

/**
 * @name   OmxRpc_moduleRegisterMsgqHeap
 * @brief  OmxRpc Module function to register MsgQ heap
 * @return OmxRpc_errorNone : SUCCESS
 */
OmxRpc_errorType OmxRpc_moduleRegisterMsgqHeap (DomxTypes_coreType remoteCoreId)
{
  IHeap_Handle heapHandle;
  OmxRpc_errorType retVal = OmxRpc_errorNone;
  UInt16 remoteMultiProcId;
  Error_Block ebObj;
  Error_Block *eb = &ebObj;

  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();

  Error_init (eb);
  if (OmxRpc_createMsgHeaps)
  {
    OmxRpc_genCoreMsgqHeapName ((char *) OmxRpc_module->
                                remoteCoreRcmClient[remoteCoreId].msgHeap.
                                msgqHeapName, remoteCoreId,
                                DomxCore_localCoreId);

    remoteMultiProcId = DomxCore_mapDomxCore2MultiProcId (remoteCoreId, eb);

    if (Error_check (eb))
    {
      Log_print1 (Diags_USER1, "DomxCore_mapDomxCore2MultiProcId failed:%d",
                  Error_getId (eb));
      retVal = OmxRpc_errorBadArgument;
    }
    else
    {
      DmmDelegate_ipcHeapInfo ipcHeapInfo;

      ipcHeapInfo.heapName =
        (xdc_String) OmxRpc_module->remoteCoreRcmClient[remoteCoreId].msgHeap.
        msgqHeapName;
      ipcHeapInfo.heapType = DmmDelegate_heapIpcComm;
      ipcHeapInfo.remoteMultiProcId = remoteMultiProcId;
      heapHandle =
        (xdc_runtime_IHeap_Handle) DmmDelegate_createIpcHeap (&ipcHeapInfo, eb);
    }
    if (FALSE == Error_check (eb))
    {
      OmxRpc_module->remoteCoreRcmClient[remoteCoreId].msgHeap.msgHeapHandle
        = heapHandle;
      /* Register this heap with MessageQ */
      DOMX_UTL_TRACE_FUNCTION_MSG ("Before MessageQ_registerHeap");
      MessageQ_registerHeap (heapHandle,
                             OmxRpc_module->remoteCoreRcmClient[remoteCoreId].
                             msgHeap.msgHeapId);

      DOMX_UTL_TRACE_FUNCTION_MSG ("After MessageQ_registerHeap");
    }
  }
  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (retVal);
  return retVal;
}

/**
 * @name   OmxRpc_moduleUnregisterMsgqHeap
 * @brief  OmxRpc Module function to unregister MsgQ heap
 * @return OmxRpc_errorNone : SUCCESS
 */
OmxRpc_errorType OmxRpc_moduleUnregisterMsgqHeap (DomxTypes_coreType
                                                    remoteCoreId)
{
  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();
  Log_print1 (Diags_USER1, "Unregistering Msgqheap for :%d\n", remoteCoreId);

  MessageQ_unregisterHeap (OmxRpc_module->remoteCoreRcmClient[remoteCoreId].
                           msgHeap.msgHeapId);

  /* DmmDelegate_deleteIpcHeap (&OmxRpc_module->
                                remoteCoreRcmClient[remoteCoreId].
                                msgHeap.msgHeapHandle); */

  OmxRpc_module->remoteCoreRcmClient[remoteCoreId].msgHeap.msgHeapHandle = NULL;

  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (OmxRpc_errorNone);
  return OmxRpc_errorNone;
}

/**
 * @name   OmxRpc_rcmClientCreate
 * @brief  OmxRpc  function to create rcmClient
 * @return OmxRpc_errorNone : SUCCESS
 */
OmxRpc_errorType OmxRpc_rcmClientCreate (RcmClient_Handle *rcmHndlPtr,
                                         String rcmServerName,
                                         UInt16 rcmMsgHeapId)
{
  RcmClient_Params rcmParams;
  OmxRpc_errorType retVal = OmxRpc_errorNone;
  Int status;

  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();

  Log_print4 (Diags_USER1, "Entered function:%s (0x%x, %s, %d)",
              (IArg) __FUNCTION__, (xdc_IArg) rcmHndlPtr,
              (xdc_IArg) rcmServerName, rcmMsgHeapId);

  /* must initialize the error block before using it */
  DOMX_UTL_TRACE_FUNCTION_MSG ("Before RcmClient_Params_init");
  RcmClient_Params_init (&rcmParams);
  DOMX_UTL_TRACE_FUNCTION_MSG ("After RcmClient_Params_init");
  rcmParams.heapId = rcmMsgHeapId;
  *rcmHndlPtr = NULL;
  while (*rcmHndlPtr == NULL)
  {
    DOMX_UTL_TRACE_FUNCTION_MSG ("Before RcmClient_create");
    status = RcmClient_create (rcmServerName, &rcmParams, rcmHndlPtr);
    DOMX_UTL_TRACE_FUNCTION_MSG ("After RcmClient_create");
    if (RcmClient_S_SUCCESS != status)
    {
      if (status == RcmClient_E_SERVERNOTFOUND)
      {
        DOMX_UTL_TRACE_FUNCTION_MSG ("Server not found, try again...");
        Thread_sleep (DOMX_INT_THREADSLEEP_1000_MICROSEC, NULL);
      }
      else
      {
        break;
      }
    }
  }
  if (NULL == *rcmHndlPtr)
  {
    retVal = OmxRpc_errorRemoteServerLocateFail;
    DOMX_UTL_TRACE_FUNCTION_ERROR ((NULL != *rcmHndlPtr),
                                   "RemoteServerLocateFail");
  }

  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (retVal);

  return retVal;
}

/**
 * @name   OmxRpc_rcmClientRemoteFxnLocate
 * @brief  OmxRpc  function to locate remote rcmServer function
 * @return OmxRpc_errorNone : SUCCESS
 */
OmxRpc_errorType OmxRpc_rcmClientRemoteFxnLocate (RcmClient_Handle rcmHndl,
                                                  OmxRpc_remoteFxnCategory
                                                    regFxnCategory,
                                                  OmxRpc_remoteFxnEntryTbl
                                                    *pFxnsTbl)
{
  Int i;
  Int status;
  OmxRpc_errorType retVal = OmxRpc_errorNone;

  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();

  Log_print4 (Diags_ENTRY, "Entered %s (0x%x, %d, 0x%x)",
              (xdc_IArg) __FUNCTION__, (xdc_IArg) rcmHndl,
              (xdc_IArg) regFxnCategory, (xdc_IArg) pFxnsTbl);

  /* must initialize the error block before using it */

  for (i = 0; i < OmxRpc_rcmFxnRegistryTbl.length; i++)
  {
    if (OmxRpc_rcmFxnRegistryTbl.elem[i]->fxnCategory == regFxnCategory)
    {
      DOMX_UTL_TRACE_FUNCTION_ASSERT ((pFxnsTbl->numEntries <
                                       (sizeof (pFxnsTbl->tbl) /
                                        sizeof (pFxnsTbl->tbl[0]))),
                                      "large value for numEntries");

      DOMX_UTL_Strncpy ((OMX_STRING) pFxnsTbl->tbl[pFxnsTbl->numEntries].
                        remoteFxnName,
                        (OMX_STRING) OmxRpc_rcmFxnRegistryTbl.elem[i]->fxnName,
                        sizeof (pFxnsTbl->tbl[pFxnsTbl->numEntries].
                                remoteFxnName));
      Log_print1 (Diags_USER1, "main: calling RcmClient_getSymbolIndex(%s)",
                  (xdc_IArg) pFxnsTbl->tbl[pFxnsTbl->numEntries].remoteFxnName);
      status =
        RcmClient_getSymbolIndex (rcmHndl,
                                  (String) pFxnsTbl->tbl[pFxnsTbl->numEntries].
                                  remoteFxnName,
                                  &pFxnsTbl->tbl[pFxnsTbl->numEntries].fxnId);
      if (RcmClient_S_SUCCESS != status)
      {
        Log_print2 (Diags_USER1, "RcmClient_getSymbolIndex failed:%s,%d",
                    (xdc_IArg) pFxnsTbl->tbl[pFxnsTbl->numEntries].
                    remoteFxnName, status);
        retVal = OmxRpc_errorRemoteFxnNotFound;
        break;
      }
      pFxnsTbl->cmd2fxnIdxMap[OmxRpc_rcmFxnRegistryTbl.elem[i]->omxCmdId] =
        pFxnsTbl->tbl[pFxnsTbl->numEntries].fxnId;
      pFxnsTbl->numEntries++;
    }                           /* end of (fxnCategory == regFxnCategory) */
  }                             /* end of for (i = 0; i <
                                   OmxRpc_rcmFxnRegistryTbl.length; i++) */

  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (retVal);

  return retVal;
}

OmxRpc_errorType OmxRpc_moduleInitClient (DomxTypes_coreType remoteCoreId)
{
  return (omxrpc_module_init_client (remoteCoreId));
}

OmxRpc_errorType OmxRpc_moduleDeinitClient (DomxTypes_coreType remoteCoreId)
{
  return (omxrpc_module_deinit_client (remoteCoreId));
}

/* Nothing beyond this point */
