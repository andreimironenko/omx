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
 *  @file  DomxTunnelMgr.c
 *         This file contains methods that provides the functionality for
 *         tunnel connections between DOMX components
 *
 *  @path \avmecomp\ti\omx\domx\DomxTunnelMgr.c
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

#include <ti/omx/interfaces/openMaxv11/OMX_Core.h>
#include <ti/omx/interfaces/openMaxv11/OMX_TI_Index.h>
#include <ti/omx/interfaces/openMaxv11/interfaces_openmaxv11_internal.h>
#include "domx_internal.h"
#include "domx_internalcfg.h"
#include "OmxProxy__epilogue.h"
#include "domx_util.h"
#include <timm_osal_interfaces.h>



static Void domxtmgr_get_component_connection_info (OmxTypes_OMX_HANDLETYPE
                                                    hComponentHandle,
                                                    OMX_CONFIG_DOMXPROXYCOMPINFO
                                                      *pDomxProxyInfo,
                                                    Int32 *pCompIdx,
                                                    DomxTypes_coreType
                                                      *pCompCoreId,
                                                    Error_Block *eb);

static Void domxtmgr_delete_proxylite_connections (OMX_HANDLETYPE
                                                   hComponentHandle);

/*
 *************************************************************************
 *                       Static functions
 *************************************************************************
 */

/**
 * @name domxtmgr_map_component_name2info
 * @brief Map component name to DOMX Component COre Info Entry
 * @param  component_name  : OMX Component name
 * @param  info            : Info entry to be populated
 * @param  eb              : Used to raise error 
 * @return none
 */
static Void domxtmgr_map_component_name2info (String component_name,
                                              DomxCore_componentCoreInfoEntry
                                                **info,
                                              Error_Block *eb)
{
  Int i;

  Log_print4 (Diags_ENTRY, "Entered: %s (0x%x, 0x%x, 0x%x)",
              (xdc_IArg) __FUNCTION__, (xdc_IArg) component_name,
              (xdc_IArg) info, (xdc_IArg) eb);

  *info = NULL;
  for (i = 0; i < DomxCore_componentCoreInfoTbl.length; i++)
  {
    if (strcmp ((DomxCore_componentCoreInfoTbl.elem[i][0]).name,
                component_name) == 0)
    {
      *info = &(DomxCore_componentCoreInfoTbl.elem[i][0]);
      break;
    }
  }
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((i < DomxCore_componentCoreInfoTbl.length), 
                                  "Component not found in core info table");

  Log_print1 (Diags_EXIT, "Leaving: %s RetVal: void", (xdc_IArg) __FUNCTION__);

}

/**
 * @name domxtmgr_module_init
 * @brief DomxTunnelManager Module level init function
 * @param  component_name  : OMX Component name
 * @param  info            : Info entry to be populated
 * @param  eb              : Used to raise error 
 * @return none
 */
static Void domxtmgr_module_init ()
{
  IArg key;

  Log_print1 (Diags_ENTRY, "Entering %s (void)", (xdc_IArg) __FUNCTION__);

  key = Gate_enterModule ();

  DOMX_UTL_TRACE_FUNCTION_ASSERT
    ((DomxTunnelMgr_module->connectionInfoTbl.length == 
      DomxTunnelMgr_maxNumComponents), 
     "Table length != maxNumComponents");
  DomxTunnelMgr_module->numActiveComponents = 0;
  DomxTunnelMgr_module->initDone = TRUE;
  Gate_leaveModule (key);

  Log_print1 (Diags_EXIT, "Leaving %s retVal: void", (xdc_IArg) __FUNCTION__);
}

/**
 * @name domxtmgr_register_component
 * @brief Registers a new component in the TMgr connection table
 * @param  hComponentRealHandle  : Real Component handles
 * @param  szCompName            : Name of the component
 * @param  eCompCoreId           : Component Native core ID
 * @param  pRpcSkelHandle        : Pointer to RPC skeleton handle
 * @param  cComponentRcmSvrName  : Name of component server
 * @param  eb                    : Error block to be used for error raise
 * @return none
 */
static Void domxtmgr_register_component (OmxTypes_OMX_HANDLETYPE
                                           hComponentRealHandle,
                                         Char *szCompName,
                                         DomxTypes_coreType eCompCoreId,
                                         OmxTypes_OMX_PTR pRpcSkelHandle,
                                         Char *cComponentRcmSvrName,
                                         Error_Block *eb)
{

  Int nCompIdx, j;

  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();

  if (DomxTunnelMgr_module->numActiveComponents <
      DomxTunnelMgr_module->connectionInfoTbl.length)
  {
    nCompIdx = DomxTunnelMgr_module->numActiveComponents;
    DomxTunnelMgr_module->connectionInfoTbl.elem[nCompIdx][0].
      hComponentRealHandle = hComponentRealHandle;
    DOMX_UTL_Strncpy ((OMX_STRING) DomxTunnelMgr_module->connectionInfoTbl.
                      elem[nCompIdx][0].cComponentName, szCompName,
                      sizeof (DomxTunnelMgr_module->connectionInfoTbl.
                              elem[nCompIdx][0].cComponentName));
    DomxTunnelMgr_module->connectionInfoTbl.elem[nCompIdx][0].eCompCoreId =
      eCompCoreId;
    DomxTunnelMgr_module->connectionInfoTbl.elem[nCompIdx][0].sRpcSkelInfo.
      pRpcSkelHandle = pRpcSkelHandle;
    if (NULL != pRpcSkelHandle)
    {
      DOMX_UTL_Strncpy ((OMX_STRING) DomxTunnelMgr_module->connectionInfoTbl.
                        elem[nCompIdx][0].sRpcSkelInfo.cComponentRcmSvrName,
                        cComponentRcmSvrName,
                        sizeof (DomxTunnelMgr_module->connectionInfoTbl.
                                elem[nCompIdx][0].sRpcSkelInfo.
                                cComponentRcmSvrName));
    }
    DomxTunnelMgr_module->connectionInfoTbl.elem[nCompIdx][0].
      sRpcSkelInfo.bRpcSkelCreated = FALSE;
    for (j = DomxTypes_coreFirst; j < DomxTypes_coreCount; j++)
    {
      DomxTunnelMgr_module->connectionInfoTbl.elem[nCompIdx][0].
        sCoreConnectionInfo[j].nConnectionCount = 0;
      DomxTunnelMgr_module->connectionInfoTbl.elem[nCompIdx][0].
        sCoreConnectionInfo[j].hConnectionHandle = NULL;
      DomxTunnelMgr_module->connectionInfoTbl.elem[nCompIdx][0].
        sCoreConnectionInfo[j].bProxyLiteCreated = FALSE;
    }
    DomxTunnelMgr_module->numActiveComponents++;
  }                             /* if (numActiveComponents * <
                                   connectionInfoTbl.length */
  else
  {
    DOMX_UTL_TRACE_FUNCTION_ASSERT
      ((DomxTunnelMgr_module->numActiveComponents <
        DomxTunnelMgr_module->connectionInfoTbl.length),
       "NumActiveComponents > max");
  }

  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (OMX_ErrorNone);
}

/**
 * @name domxtmgr_unregister_component
 * @brief UnRegisters a component from the TMgr connection table
 * @param  nCompIdx             :  Connention table entry index to be removed
 * @return none
 */
static Void domxtmgr_unregister_component (Int nCompIdx)
{
  Int i;

  DOMX_UTL_TRACE_FUNCTION_ASSERT
    (((nCompIdx >= 0) && (nCompIdx < DomxTunnelMgr_module->numActiveComponents)),
     "Component index incorrect");
         
  for (i = (nCompIdx + 1); i < DomxTunnelMgr_module->numActiveComponents; i++)
  {
    DomxTunnelMgr_module->connectionInfoTbl.elem[(i - 1)][0] =
      DomxTunnelMgr_module->connectionInfoTbl.elem[i][0];
  }
  DomxTunnelMgr_module->numActiveComponents--;
  DOMX_UTL_TRACE_FUNCTION_ASSERT (((xdc_Int32) DomxTunnelMgr_module->numActiveComponents >=
                                   0), "numActiveComponents < 0");
}

/**
 * @name domxtmgr_get_component_registry_index
 * @brief  Maps component handle to DOmxTMgr connection table entry
 * @param  hComponentRealHandle  : Real Component handles
 * @return Entry in the connection table
 */
static Int32 domxtmgr_get_component_registry_index (OmxTypes_OMX_HANDLETYPE
                                                      hComponentRealHandle)
{
  UInt32 i;
  Int32 nCompIdx = -1;
  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();

  for (i = 0; i < DomxTunnelMgr_module->numActiveComponents; i++)
  {
    if (DomxTunnelMgr_module->connectionInfoTbl.elem[i][0].
        hComponentRealHandle == hComponentRealHandle)
    {
      nCompIdx = i;
      break;
    }
  }
  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (OMX_ErrorNone);
  return nCompIdx;
}

/**
 * @name domxtmgr_add_component_connection_info
 * @brief Add component connection info
 * @param  nCompIdx          : Index into the connection table
 * @param  eConnectedCoreId  : Connected Core ID
 * @param  hConnectionHandle : Handle to the tunneled component
 
 * @return none
 */
static Void domxtmgr_add_component_connection_info (Int32 nCompIdx,
                                                    DomxTypes_coreType
                                                      eConnectedCoreId,
                                                    OmxTypes_OMX_HANDLETYPE
                                                      hConnectionHandle)
{
  DomxTunnelMgr_coreConnectionInfo *connectInfo =
    &(DomxTunnelMgr_module->connectionInfoTbl.elem[nCompIdx][0].
      sCoreConnectionInfo[eConnectedCoreId]);
  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();

  if (0 == connectInfo->nConnectionCount)
  {
    connectInfo->hConnectionHandle = hConnectionHandle;
  }
  else
  {
    DOMX_UTL_TRACE_FUNCTION_ASSERT ((connectInfo->hConnectionHandle ==
                                     hConnectionHandle),
                                    "connectionHandle mismatch");
  }
  connectInfo->nConnectionCount++;

  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (OMX_ErrorNone);
}

/**
 * @name domxtmgr_register_connection
 * @brief Register tunnel connection in connection table
 * @param  hComponentRealHandle  : Real Component handles
 * @param  szCompName            : Name of the component
 * @param  eCompCoreId           : Component Native core ID
 * @param  pRpcSkelHandle        : Pointer to RPC skeleton handle
 * @param  cComponentRcmSvrName  : Name of component server
 * @param  hConnectionHandle     : Handle to the connected component
 * @param  eConnectedCoreId      : TUnneled CoreID
 * @param  eb                    : Error block to raise errors
 * @return none
 */
static Void domxtmgr_register_connection (OmxTypes_OMX_HANDLETYPE
                                            hComponentRealHandle,
                                          Char *szCompName,
                                          DomxTypes_coreType eCompCoreId,
                                          OmxTypes_OMX_PTR pRpcSkelHandle,
                                          Char *cComponentRcmSvrName,
                                          OmxTypes_OMX_HANDLETYPE
                                            hConnectionHandle,
                                          DomxTypes_coreType eConnectedCoreId,
                                          Error_Block *eb)
{
  Int32 nCompIdx;

  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();
  nCompIdx = domxtmgr_get_component_registry_index (hComponentRealHandle);
  if (-1 == nCompIdx)
  {
    domxtmgr_register_component (hComponentRealHandle,
                                 szCompName,
                                 eCompCoreId,
                                 pRpcSkelHandle, cComponentRcmSvrName, eb);
    if (FALSE == Error_check (eb))
    {
      nCompIdx = domxtmgr_get_component_registry_index (hComponentRealHandle);
      DOMX_UTL_TRACE_FUNCTION_ASSERT ((nCompIdx != -1),
                                      "Invalid compIndex");
    }
  }
  else
  {
    /* Ensure component native coreId is same as what is already registered */
    DOMX_UTL_TRACE_FUNCTION_ASSERT ((eCompCoreId ==
                                     DomxTunnelMgr_module->connectionInfoTbl.
                                     elem[nCompIdx][0].eCompCoreId),
                                    "Invalid compCoreId");
    DOMX_UTL_TRACE_FUNCTION_ASSERT ((strcmp
                                     ((OMX_STRING) DomxTunnelMgr_module->
                                      connectionInfoTbl.elem[nCompIdx][0].
                                      cComponentName, szCompName) == 0),
                                    "Invalid compName");

    if (NULL != pRpcSkelHandle)
    {
      DOMX_UTL_TRACE_FUNCTION_ASSERT ((pRpcSkelHandle ==
                                       DomxTunnelMgr_module->connectionInfoTbl.
                                       elem[nCompIdx][0].sRpcSkelInfo.
                                       pRpcSkelHandle),
                                      "Invalid rpcSkelHandle");
    }
  }
  if (FALSE == Error_check (eb))
  {
    domxtmgr_add_component_connection_info (nCompIdx, eConnectedCoreId,
                                            hConnectionHandle);
  }

  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (OMX_ErrorNone);
}

/**
 * @name domxtmgr_register_skelinfo
 * @brief Register skeleton info
 * @param  nCompIdx              : Index into the DOMX connection table
 * @param  pRpcSkelHandle        : Pointer to RPC skeleton handle
 * @param  cComponentRcmSvrName  : Name of component server
 * @return none
 */
static Void domxtmgr_register_skelinfo (Int nCompIdx,
                                        OmxTypes_OMX_PTR pRpcSkelHandle,
                                        Char *cComponentRcmSvrName)
{
  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();
  DOMX_UTL_TRACE_FUNCTION_MSG (cComponentRcmSvrName);

  if (NULL ==
      DomxTunnelMgr_module->connectionInfoTbl.elem[nCompIdx][0].sRpcSkelInfo.
      pRpcSkelHandle)
  {
    DomxTunnelMgr_module->connectionInfoTbl.elem[nCompIdx][0].sRpcSkelInfo.
      pRpcSkelHandle = pRpcSkelHandle;
    DOMX_UTL_Strncpy ((OMX_STRING) DomxTunnelMgr_module->connectionInfoTbl.
                      elem[nCompIdx][0].sRpcSkelInfo.cComponentRcmSvrName,
                      cComponentRcmSvrName,
                      sizeof (DomxTunnelMgr_module->connectionInfoTbl.
                              elem[nCompIdx][0].sRpcSkelInfo.
                              cComponentRcmSvrName));
  }
  else
  {
    DOMX_UTL_TRACE_FUNCTION_ASSERT ((DomxTunnelMgr_module->connectionInfoTbl.
                                     elem[nCompIdx][0].sRpcSkelInfo.
                                     pRpcSkelHandle == pRpcSkelHandle),
                                    "Invalid skelInfoRegistration");
    DOMX_UTL_TRACE_FUNCTION_ASSERT ((strcmp
                                     ((const char *)
                                      DomxTunnelMgr_module->connectionInfoTbl.
                                      elem[nCompIdx][0].sRpcSkelInfo.
                                      cComponentRcmSvrName,
                                      (const char *) cComponentRcmSvrName) == 0),
                                    "Invalid skelInfoRegistration");
  }
  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (OMX_ErrorNone);
}

/**
 * @name domxtmgr_get_component_connection_info
 * @brief Get connection info for a component handle
 * @param  hComponentHandle      : Component Handle
 * @param  pDomxProxyInfo        : Pointer to ProxyInfo structure
 * @param  pCompIdx              : Pointer to connection table component entry index
 * @param  pCompCoreId           : Pointer to component coreID
 * @param  eb                    : Error Block to be used for Error raise
 * @return none
 */
static Void domxtmgr_get_component_connection_info (OmxTypes_OMX_HANDLETYPE
                                                      hComponentHandle,
                                                    OMX_CONFIG_DOMXPROXYCOMPINFO
                                                      *pDomxProxyInfo,
                                                    Int32 *pCompIdx,
                                                    DomxTypes_coreType
                                                      *pCompCoreId,
                                                    Error_Block * eb)
{
  Int32 nCompIdx;
  OMX_ERRORTYPE eError;

  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();

  nCompIdx = domxtmgr_get_component_registry_index (hComponentHandle);
  if (-1 == nCompIdx)
  {
    eError = OMX_GetConfig (hComponentHandle, (OMX_INDEXTYPE)
                            OMX_TI_IndexConfigGetDomxCompInfo, pDomxProxyInfo);
    DOMX_UTL_TRACE_FUNCTION_ERROR ((eError == OMX_ErrorNone),
                                   "OMX_GetConfig returned error");
    if (OMX_ErrorNone == eError)
    {
      nCompIdx =
        domxtmgr_get_component_registry_index (pDomxProxyInfo->hCompRealHandle);
      DOMX_UTL_TRACE_FUNCTION_ASSERT ((-1 != nCompIdx),
                                      "Invalid compIndex");
    }
    else
    {
      DOMX_UTL_TRACE_FUNCTION_ASSERT ((OMX_ErrorNone == eError),
                                      "OMX_GetConfig returned error");
    }
  }
  *pCompIdx = nCompIdx;
  if (FALSE == Error_check (eb))
  {
    *pCompCoreId =
      DomxTunnelMgr_module->connectionInfoTbl.elem[nCompIdx][0].eCompCoreId;
  }
  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (OMX_ErrorNone);
}

/**
 * @name domxtmgr_get_connection_handle
 * @brief Get connection handle corresponding to compIndex
 * @param  nCompIdx              : Component index in connection table
 * @param  eConnectCoreId        : Connected coreID
 * @param  ppComponentHandlePtr  : Pointer to connection handle to be  populated
 * @return none
 */
static Void domxtmgr_get_connection_handle (Int32 nCompIdx,
                                            DomxTypes_coreType eConnectCoreId,
                                            OmxTypes_OMX_HANDLETYPE 
                                              *ppComponentHandlePtr)
{
  DomxTunnelMgr_coreConnectionInfo *info;

  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();

  *ppComponentHandlePtr = NULL;
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((nCompIdx <
                                   DomxTunnelMgr_module->numActiveComponents),
                                  "nCompIdx exceeds numActiveComponents");
  info =
    &(DomxTunnelMgr_module->connectionInfoTbl.elem[nCompIdx][0].
      sCoreConnectionInfo[eConnectCoreId]);
  if (info->nConnectionCount > 0)
  {
    *ppComponentHandlePtr = info->hConnectionHandle;
  }
  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (OMX_ErrorNone);
}

/**
 * @name domxtmgr_create_rpcskel
 * @brief Create the RPCSkeleton for a component
 * @param  nCompIdx              : Component index in connection table
 * @param  pRpcStubHandle        : Pointer to rpc stub handle
 * @param  eb                    : Error block to raise errors
 * @return none
 */
static Int domxtmgr_create_rpcskel (Int32 nCompIdx,
                                    DomxTypes_coreType eConnectCoreId,
                                    OmxRpc_Handle pRpcStubHandle,
                                    Error_Block *eb)
{
  OmxRpc_Params omxRpcParams;
  OmxRpc_Handle rpcSkelHandle = NULL;
  DomxCore_componentCoreInfoEntry *entry;
  Int status = 0;

  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();

  Log_print5 (Diags_USER1, "Entered %s (%d, %d, 0x%x, 0x%x)",
              (xdc_IArg) __FUNCTION__, nCompIdx, eConnectCoreId,
              (xdc_IArg) pRpcStubHandle, (xdc_IArg) eb);

  OmxRpc_Params_init (&omxRpcParams);
  omxRpcParams.componentName =
    (xdc_String) DomxTunnelMgr_module->connectionInfoTbl.elem[nCompIdx][0].
    cComponentName;
  domxtmgr_map_component_name2info (omxRpcParams.componentName, &entry, eb);
  if (NULL != entry)
  {
    omxRpcParams.remoteCoreId = eConnectCoreId;
    /* Create an OmxRpc Skel instance */
    omxRpcParams.layerId = OmxRpc_layerTypeSkel;
    rpcSkelHandle = OmxRpc_create (&omxRpcParams, eb);
  }
  if (NULL == rpcSkelHandle)
  {
    DOMX_UTL_TRACE_FUNCTION_ERROR ((NULL != entry),
                                   "OmxRpc Skel create failed");
    status = -1;
  }
  else
  {
    OmxRpc_errorType eRpcError;
    OmxRpc_skelComponentConfigInfo skelCfg;

    DomxTunnelMgr_module->connectionInfoTbl.elem[nCompIdx][0].sRpcSkelInfo.
      pRpcSkelHandle = rpcSkelHandle;
    OmxRpc_skelGetCompRcmServerName (rpcSkelHandle,
                                     (OmxTypes_OMX_S8 *)
                                     DomxTunnelMgr_module->connectionInfoTbl.
                                     elem[nCompIdx][0].sRpcSkelInfo.
                                     cComponentRcmSvrName,
                                     sizeof (DomxTunnelMgr_module->
                                             connectionInfoTbl.
                                             elem[nCompIdx][0].sRpcSkelInfo.
                                             cComponentRcmSvrName));
    skelCfg.stubHandle = NULL;
    skelCfg.pComponentHandle =
      DomxTunnelMgr_module->connectionInfoTbl.elem[nCompIdx][0].
      hComponentRealHandle;
    eRpcError = OmxRpc_skelConfigCompInfo (rpcSkelHandle, &skelCfg);
    DOMX_UTL_TRACE_FUNCTION_ASSERT ((OmxRpc_errorNone == eRpcError),
                                    "OmxRpc_skelConfigCompInfo failed");
  }

  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (OmxRpc_errorNone);

  return status;
}

/**
 * @name domxtmgr_create_connection
 * @brief Create a DOMXTMgr connection
 * @param  nCompIdx              : Component index in connection table
 * @param  pRpcStubHandle        : Pointer to rpc stub handle
 * @param  ppComponentHandlePtr  : Pointer to Component Handle to be populated
 * @param  eb                    : Error block to raise errors
 * @return none
 */
static Void domxtmgr_create_connection (Int32 nCompIdx,
                                        DomxTypes_coreType eConnectCoreId,
                                        OmxRpc_Handle pRpcStubHandle,
                                        OmxTypes_OMX_HANDLETYPE
                                          *ppComponentHandlePtr,
                                        Error_Block *eb)
{
  OmxRpc_errorType eRpcError;
  OmxCore_OMX_ERRORTYPE eError;

  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();

  Log_print6 (Diags_USER1, "Entered :%s (%d, %d, 0x%x, 0x%x, 0x%x)",
              (xdc_IArg) __FUNCTION__, nCompIdx, eConnectCoreId,
              (xdc_IArg) pRpcStubHandle, (xdc_IArg) ppComponentHandlePtr,
              (xdc_IArg) eb);

  *ppComponentHandlePtr = NULL;
  if (NULL ==
      DomxTunnelMgr_module->connectionInfoTbl.elem[nCompIdx][0].sRpcSkelInfo.
      pRpcSkelHandle)
  {
    Int status;

    status =
      domxtmgr_create_rpcskel (nCompIdx, eConnectCoreId, pRpcStubHandle, eb);
    if (status >= 0)
    {
      DomxTunnelMgr_module->connectionInfoTbl.elem[nCompIdx][0].sRpcSkelInfo.
        bRpcSkelCreated = TRUE;
    }
  }
  if (FALSE == Error_check (eb))
  {
    OmxTypes_OMX_U32 nRpcRemoteHandle;
    OmxTypes_OMX_HANDLETYPE pProxyLiteHandlePtr;

    nRpcRemoteHandle = (OmxTypes_OMX_U32)
      DomxTunnelMgr_module->connectionInfoTbl.elem[nCompIdx][0].sRpcSkelInfo.
      pRpcSkelHandle;
    DOMX_UTL_TRACE_FUNCTION_ASSERT ((NULL !=
                                     DomxTunnelMgr_module->connectionInfoTbl.
                                     elem[nCompIdx][0].sRpcSkelInfo.
                                     pRpcSkelHandle),
                                    "Invalid skelInfoRegistration");
    eRpcError =
      OmxRpc_stubCreateProxyLite (pRpcStubHandle, nRpcRemoteHandle,
                                  DomxTunnelMgr_module->connectionInfoTbl.
                                  elem[nCompIdx][0].hComponentRealHandle,
                                  (OmxTypes_OMX_S8 *) DomxTunnelMgr_module->
                                  connectionInfoTbl.elem[nCompIdx][0].
                                  cComponentName,
                                  (OmxTypes_OMX_S8 *) DomxTunnelMgr_module->
                                  connectionInfoTbl.elem[nCompIdx][0].
                                  sRpcSkelInfo.cComponentRcmSvrName,
                                  &pProxyLiteHandlePtr, &eError);
    if ((OmxRpc_errorNone != eRpcError) || (OMX_ErrorNone != eError))
    {
      DOMX_UTL_TRACE_FUNCTION_ASSERT ((OmxRpc_errorNone == eRpcError),
                                      "RpcError in OmxRpc_stubCreateProxyLite");
      DOMX_UTL_TRACE_FUNCTION_ASSERT ((OMX_ErrorNone == eError),
                                      "OMXError in OmxRpc_stubCreateProxyLite");
    }
    else
    {
      *ppComponentHandlePtr = pProxyLiteHandlePtr;
      DomxTunnelMgr_module->connectionInfoTbl.elem[nCompIdx][0].
        sCoreConnectionInfo[eConnectCoreId].bProxyLiteCreated = TRUE;
    }
  }

  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (OMX_ErrorNone);
}

/**
 * @name   domxtmgr_map_connection_handle
 * @brief  Map component handle to connection handle
 * @param  nCompIdx              : Component index in connection table
 * @param  ePeerCompCoreId       : Tunneled peer coreID
 * @param  hTunneledPeerMap      : Pointer to handle of Peer component
 * @param  eb                    : Error block to raise errors
 * @return none
 */
static Void domxtmgr_map_connection_handle (Int nCompIdx,
                                            DomxTypes_coreType ePeerCompCoreId,
                                            OmxRpc_Handle pRpcStubHandle,
                                            OmxTypes_OMX_HANDLETYPE
                                              *hTunneledPeerMap,
                                            Error_Block * eb)
{
  OmxTypes_OMX_HANDLETYPE hCompConnectionHandle;

  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();

  Log_print6 (Diags_USER1, "Entered: %s (%d, %d, 0x%x, 0x%x, 0x%x)",
              (xdc_IArg) __FUNCTION__, nCompIdx,
              ePeerCompCoreId, (xdc_IArg) pRpcStubHandle,
              (xdc_IArg) hTunneledPeerMap, (xdc_IArg) eb);

  *hTunneledPeerMap = NULL;

  if (FALSE == Error_check (eb))
  {
    domxtmgr_get_connection_handle (nCompIdx, ePeerCompCoreId,
                                    &hCompConnectionHandle);
    if (NULL == hCompConnectionHandle)
    {
      domxtmgr_create_connection (nCompIdx, ePeerCompCoreId,
                                  pRpcStubHandle, &hCompConnectionHandle, eb);
      if (FALSE == Error_check (eb))
      {
        domxtmgr_add_component_connection_info (nCompIdx,
                                                ePeerCompCoreId,
                                                hCompConnectionHandle);
        domxtmgr_get_connection_handle (nCompIdx, ePeerCompCoreId,
                                        &hCompConnectionHandle);
        DOMX_UTL_TRACE_FUNCTION_ASSERT ((NULL != hCompConnectionHandle),
                                        "CompConnectionHandle is NULL");
      }
    }
    else
    {
      domxtmgr_add_component_connection_info (nCompIdx,
                                              ePeerCompCoreId,
                                              hCompConnectionHandle);
    }
  }
  if (FALSE == Error_check (eb))
  {
    *hTunneledPeerMap = hCompConnectionHandle;
  }

  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (OMX_ErrorNone);

}

/**
 * @name   DomxTunnelMgr_registerHandle
 * @brief  Resgiter a component handle
 * @param  hComponentHandle           : Handle to component to register
 * @param  szCompName                 : Component name
 * @return none
 */
Void DomxTunnelMgr_registerHandle (OmxTypes_OMX_HANDLETYPE hComponentHandle,
                                   Char *szCompName)
{
  IArg key;
  OMX_CONFIG_DOMXPROXYCOMPINFO sDomxProxyInfo;
  OMX_ERRORTYPE eError;
  DomxCore_componentCoreInfoEntry *entry;
  Error_Block ebObj;
  Error_Block *eb = &ebObj;

  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();

  Log_print3 (Diags_USER1, "Entered: %s (0x%x, 0x%x)", (xdc_IArg) __FUNCTION__,
              (xdc_IArg) hComponentHandle, (xdc_IArg) szCompName);

  Error_init (eb);
  if (FALSE == DomxTunnelMgr_module->initDone)
  {
    domxtmgr_module_init ();
  }
  key = Gate_enterModule ();
  domxtmgr_map_component_name2info (szCompName, &entry, eb);
  if (NULL != entry)
  {
    if (entry->coreId == DomxCore_localCoreId)
    {
      domxtmgr_register_connection (hComponentHandle,
                                    szCompName,
                                    DomxCore_localCoreId,
                                    NULL,
                                    NULL,
                                    hComponentHandle, DomxCore_localCoreId, eb);
    }
    else
    {
      eError = OMX_GetConfig (hComponentHandle, (OMX_INDEXTYPE)
                              OMX_TI_IndexConfigGetDomxCompInfo,
                              &sDomxProxyInfo);
      if (OMX_ErrorNone == eError)
      {
        domxtmgr_register_connection (sDomxProxyInfo.hCompRealHandle,
                                      szCompName,
                                      entry->coreId,
                                      (OmxTypes_OMX_PTR) sDomxProxyInfo.
                                      nRpcSkelPtr,
                                      (OmxTypes_OMX_S8 *) sDomxProxyInfo.
                                      cComponentRcmSvrName, hComponentHandle,
                                      DomxCore_localCoreId, eb);
        /* Also register connenction on remote core */
        domxtmgr_register_connection (sDomxProxyInfo.hCompRealHandle,
                                      szCompName,
                                      entry->coreId,
                                      NULL,
                                      NULL,
                                      sDomxProxyInfo.hCompRealHandle,
                                      entry->coreId, eb);
      }
      else
      {
        DOMX_UTL_TRACE_FUNCTION_ASSERT ((OMX_ErrorNone == eError),
                                        "OMX_GetConfig returned error");
      }
    }
  }
  Gate_leaveModule (key);
  /* TODO: Component should return error code */
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((FALSE == Error_check (eb)),
                                  "Error in DomxTunnelMgr_registerHandle");

  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (OMX_ErrorNone);
}

/**
 * @name   DomxTunnelMgr_unRegisterHandle
 * @brief  UnResgiter a component handle
 * @param  hComponentHandle           : Handle to component to be unregistered
 * @return none
 */
Void DomxTunnelMgr_unRegisterHandle (OmxTypes_OMX_HANDLETYPE hComponentHandle)
{
  OMX_CONFIG_DOMXPROXYCOMPINFO sDomxProxyInfo;
  OMX_ERRORTYPE eError;
  Int nCompIdx;
  IArg key;

  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();
  nCompIdx = domxtmgr_get_component_registry_index (hComponentHandle);
  if (-1 == nCompIdx)
  {
    eError = OMX_GetConfig (hComponentHandle, (OMX_INDEXTYPE)
                            OMX_TI_IndexConfigGetDomxCompInfo, &sDomxProxyInfo);
    if (OMX_ErrorNone == eError)
    {
      nCompIdx =
        domxtmgr_get_component_registry_index (sDomxProxyInfo.hCompRealHandle);
      DOMX_UTL_TRACE_FUNCTION_ASSERT ((-1 != nCompIdx),
                                      "Component handle not found");
    }
    else
    {
      DOMX_UTL_TRACE_FUNCTION_ASSERT ((OMX_ErrorNone == eError),
                                      "OMX_GetConfig returned error");
    }
  }
  domxtmgr_delete_proxylite_connections (hComponentHandle);
  key = Gate_enterModule ();
  domxtmgr_unregister_component (nCompIdx);
  Gate_leaveModule (key);

  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (eError);

  return;
}

/**
 * @name   DomxTunnelMgr_registerSkelInfo
 * @brief  Register a components skeleton info
 * @param  hComponentHandle           : Handle to component to register
 * @param  szCompName                 : Component name
 * @param  pRpcSkelHandle             : Pointer to RPC skel handle
 * @param  cComponentRcmSvrName       : Name of component rcm server name
 * @return none
 */
Void DomxTunnelMgr_registerSkelInfo (OmxTypes_OMX_HANDLETYPE hComponentHandle,
                                     Char *szCompName,
                                     OmxTypes_OMX_PTR pRpcSkelHandle,
                                     Char *cComponentRcmSvrName)
{
  DomxCore_componentCoreInfoEntry *entry;
  Error_Block ebObj;
  Error_Block *eb = &ebObj;
  Int nCompIdx = DOMX_INT_TUNNELMGR_INVALID;
  IArg key;

  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();

  Error_init (eb);
  if (FALSE == DomxTunnelMgr_module->initDone)
  {
    domxtmgr_module_init ();
  }
  key = Gate_enterModule ();
  domxtmgr_map_component_name2info (szCompName, &entry, eb);
  if (NULL != entry)
  {
    DOMX_UTL_TRACE_FUNCTION_ASSERT ((entry->coreId == DomxCore_localCoreId),
                                    "Entry coreID != localCoreID");
    nCompIdx = domxtmgr_get_component_registry_index (hComponentHandle);
    DOMX_UTL_TRACE_FUNCTION_ASSERT ((nCompIdx != DOMX_INT_TUNNELMGR_INVALID),
                                    "Invalid Comp index");
    domxtmgr_register_skelinfo (nCompIdx, pRpcSkelHandle, cComponentRcmSvrName);
  }
  Gate_leaveModule (key);
  /* TODO: Component should return error code */
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((FALSE == Error_check (eb)),
                                  "DomxTunnelMgr_registerSkelInfo failed");

  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (OMX_ErrorNone);
}

/**
 * @name   DomxTunnelMgr_mapTunnelComponentPeerHandles
 * @brief  Map component handle to peer handles to be used in tunnel request
 * @param  hTunneledOutput            : Handle to output port component 
 * @param  hTunneledInput             : Handle to input port component 
 * @param  hTunneledOutputPeerMap     : Handle to mapped output port component 
 * @param  hTunneledOutputPeerMap     : Handle to mapped input port component 
 * @return none
 */
Void DomxTunnelMgr_mapTunnelComponentPeerHandles (OmxTypes_OMX_HANDLETYPE
                                                    hTunneledOutput,
                                                  OmxTypes_OMX_HANDLETYPE
                                                    hTunneledInput,
                                                  OmxTypes_OMX_HANDLETYPE
                                                    *hTunneledOutputPeerMap,
                                                  OmxTypes_OMX_HANDLETYPE
                                                    *hTunneledInputPeerMap)
{
  OMX_CONFIG_DOMXPROXYCOMPINFO sDomxProxyInfohOut, sDomxProxyInfohIn;
  DomxTypes_coreType eOutCompCoreId, eInCompCoreId;
  Int32 nCompIdxhOut, nCompIdxhIn;
  Error_Block ebObj;
  Error_Block *eb = &ebObj;
  IArg key;
  Bool bDisconnecthOut = FALSE;
  Bool bDisconnecthIn = FALSE;

  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();

  Log_print5 (Diags_USER1, "Entered: %s (0x%x, 0x%x, 0x%x, 0x%x)",
              (xdc_IArg) __FUNCTION__, (xdc_IArg) hTunneledOutput,
              (xdc_IArg) hTunneledInput, (xdc_IArg) hTunneledOutputPeerMap,
              (xdc_IArg) hTunneledInputPeerMap);

  Error_init (eb);
  if (FALSE == DomxTunnelMgr_module->initDone)
  {
    domxtmgr_module_init ();
  }
  key = Gate_enterModule ();
  *hTunneledOutputPeerMap = NULL;
  *hTunneledInputPeerMap = NULL;
  sDomxProxyInfohOut.pRpcStubHandle = NULL;
  sDomxProxyInfohIn.pRpcStubHandle = NULL;
  if (NULL != hTunneledOutput)
  {
    domxtmgr_get_component_connection_info (hTunneledOutput,
                                            &sDomxProxyInfohOut, &nCompIdxhOut,
                                            &eOutCompCoreId, eb);
  }
  else
  {
    bDisconnecthOut = TRUE;
  }

  if (FALSE == Error_check (eb))
  {
    if (NULL != hTunneledInput)
    {
      Log_print1 (Diags_USER1, "eOutCompCoreId: %d\n", eOutCompCoreId);

      domxtmgr_get_component_connection_info (hTunneledInput,
                                              &sDomxProxyInfohIn, &nCompIdxhIn,
                                              &eInCompCoreId, eb);
    }
    else
    {
      bDisconnecthIn = TRUE;
    }
  }
  
  if (FALSE == Error_check (eb))
  {
    if (NULL != hTunneledOutput)
    {
      if (FALSE == bDisconnecthIn)
      {
        Log_print1 (Diags_USER1, "eInCompCoreId: %d\n", eInCompCoreId);

        domxtmgr_map_connection_handle (nCompIdxhOut, eInCompCoreId,
                                        sDomxProxyInfohIn.pRpcStubHandle,
                                        hTunneledInputPeerMap, eb);
      }
      else
      {
        /* Do nothing for now */
      }
    }
  }

  if (FALSE == Error_check (eb))
  {
    if (NULL != hTunneledInput)
    {
      if (FALSE == bDisconnecthOut)
      {
        domxtmgr_map_connection_handle (nCompIdxhIn, eOutCompCoreId,
                                        sDomxProxyInfohOut.pRpcStubHandle,
                                        hTunneledOutputPeerMap, eb);
      }
      else
      {
        /* Do nothing for now */
      }
    }
  }

  Gate_leaveModule (key);
  /* TODO: Component should return error code */
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((FALSE == Error_check (eb)),
                                  " in mapTunnelComponentPeerHandles");

  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (OMX_ErrorNone);
}

/**
 * @name domxtmgr_delete_proxylite_connections
 * @brief UnRegisters a component from the TMgr connection table
 * @param  nCompIdx             :  Connention table entry index to be removed
 * @return none
 */
static Void domxtmgr_delete_proxylite_connections (OMX_HANDLETYPE
                                                     hComponentHandle)
{
  Int i;
  OMX_CONFIG_DOMXPROXYCOMPINFO sDomxProxyInfo;
  Int32 nCompIdx;
  DomxTypes_coreType eCompCoreId;
  Error_Block ebObj;
  Error_Block *eb = &ebObj;
  OmxCore_OMX_ERRORTYPE eError;

  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();

  Error_init (eb);

  domxtmgr_get_component_connection_info (hComponentHandle,
                                          &sDomxProxyInfo,
                                          &nCompIdx, &eCompCoreId, eb);

  DOMX_UTL_TRACE_FUNCTION_ASSERT
    (((nCompIdx >= 0) && (nCompIdx < DomxTunnelMgr_module->numActiveComponents)),
     "Comp index invalid");

  for (i = DomxTypes_coreFirst; i < DomxTypes_coreCount; i++)
  {
    DomxTunnelMgr_coreConnectionInfo *connectInfo =
      &(DomxTunnelMgr_module->connectionInfoTbl.elem[nCompIdx][0].
        sCoreConnectionInfo[i]);

    if (connectInfo->bProxyLiteCreated)
    {
      DOMX_UTL_TRACE_FUNCTION_ASSERT ((NULL != connectInfo->hConnectionHandle),
                                      "connectionHandle is NULL");

      OmxRpc_stubDeleteProxyLite ((DomxTypes_coreType) i,
                                  connectInfo->hConnectionHandle, &eError);
    }
  }
  if (DomxTunnelMgr_module->connectionInfoTbl.elem[nCompIdx][0].sRpcSkelInfo.
      bRpcSkelCreated)
  {
    DOMX_UTL_TRACE_FUNCTION_ASSERT
      ((NULL != DomxTunnelMgr_module->connectionInfoTbl.elem[nCompIdx][0].
         sRpcSkelInfo.pRpcSkelHandle), "rpcSkelHandle is NULL");
    OmxRpc_delete ((OmxRpc_Handle *) & DomxTunnelMgr_module->connectionInfoTbl.
                   elem[nCompIdx][0].sRpcSkelInfo.pRpcSkelHandle);
  }

  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (OMX_ErrorNone);
}

/* Nothing beyond this point */
