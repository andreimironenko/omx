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
 *  @file  OmxRpcSkel.c
 *         DOMX RPC skeleton functionality
 *
 *  @path \avmecomp\ti\omx\domx\OmxRpcSkel.c
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
#include <xdc/runtime/Memory.h>

#include <xdc/runtime/IHeap.h>

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
#include <ti/sdo/rcm/RcmClient.h>
#include <ti/sdo/fc/memutils/memutils.h>
#include <ti/omx/interfaces/openMaxv11/OMX_Types.h>
#include <ti/omx/interfaces/openMaxv11/OMX_Index.h>
#include <ti/omx/interfaces/openMaxv11/OMX_Core.h>
#include <ti/omx/interfaces/openMaxv11/OMX_Component.h>
#include <ti/omx/interfaces/openMaxv11/OMX_Audio.h>
#include <ti/omx/interfaces/openMaxv11/OMX_IVCommon.h>
#include <ti/omx/interfaces/openMaxv11/OMX_Image.h>
#include <ti/omx/interfaces/openMaxv11/OMX_Video.h>
#include <ti/omx/interfaces/openMaxv11/OMX_Other.h>
#include <ti/omx/interfaces/openMaxv11/OMX_ContentPipe.h>

#include <ti/omx/interfaces/openMaxv11/interfaces_openmaxv11_internal.h>
#include <ti/omx/domx/interfaces/domx_interfaces_internal.h>
#include <ti/omx/domx/delegates/shmem/domx_delegates_shmem_internal.h>
#include "domx_internal.h"
#include "domx_internalcfg.h"
#include "OmxRpc__epilogue.h"
#include "domx_util.h"
#include "omx_utils.h"

#include <ti/ipc/SharedRegion.h>

#ifdef __ti__
//#define OMXRPCSKEL_MEMLOG
#endif

#ifdef OMXRPCSKEL_MEMLOG
/**
 * @name   omxrpcskel_memstats_getfreestats
 * @brief  Function to get free memory stats from all the heaps in the system
 * @param  freeSize                 : Pointer to freeSize array to be populated
 * @param  nMaxSize                 : Max size of free size array
 * @return Number of heaps in the system
 */
static Int omxrpcskel_memstats_getfreestats (OMX_U32 freeSize[],
                                             OMX_U32 nMaxSize)
{
  SharedRegion_Entry srEntry;
  Int i;
  Int heapIndex = 0;
  Memory_Stats memstats;

  Memory_getStats (NULL, &(memstats));
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((nMaxSize > heapIndex),
                                  "heapIndex > maxSize");
  freeSize[heapIndex++] = memstats.totalFreeSize;
  for (i = 0; i < SharedRegion_getNumRegions (); i++)
  {
    SharedRegion_getEntry (i, &srEntry);
    if (TRUE == srEntry.isValid)
    {
      if (TRUE == srEntry.createHeap)
      {
        IHeap_Handle srHeap = SharedRegion_getHeap (i);
        DOMX_UTL_TRACE_FUNCTION_ASSERT ((nMaxSize > heapIndex),
                                        "heapIndex > maxSize");
        Memory_getStats (srHeap, &(memstats));
        freeSize[heapIndex++] = memstats.totalFreeSize;
      }
    }
  }
  return heapIndex;
}

/**
 * @name   omxrpcskel_memstats_getusedsize
 * @brief  Function to get used memory stats from all the heaps in the system
 * @param  initialfreeSize          : Pointer to initial free size array
 * @param  usedSize                 : Pointer to used size array to be populated
 * @param  numHeaps                 : Number of heaps in the system
 * @return Number of heaps in the system
 */
static Void omxrpcskel_memstats_getusedsize (OMX_U32 initialfreeSize[],
                                             OMX_U32 usedSize[],
                                             OMX_U32 numHeaps)
{
  Int i;
  OMX_U32 currentfreeSize[DOMX_MEMUSAGE_MAX_HEAPS_PER_PHASE];
  OMX_U32 numHeapsEnd;

  DOMX_UTL_TRACE_FUNCTION_ASSERT ((numHeaps <=
                                   DOMX_MEMUSAGE_MAX_HEAPS_PER_PHASE),
                                  "numHeaps > maxHeaps");
  numHeapsEnd =
    omxrpcskel_memstats_getfreestats (&(currentfreeSize[0]),
                                      DOMX_MEMUSAGE_MAX_HEAPS_PER_PHASE);
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((numHeapsEnd == numHeaps),
                                  "numHeaps incorrect");

  for (i = 0; i < numHeaps; i++)
  {
    /* TBD: This error check needs to be corrected later */
    /* Assert_isTrue((initialfreeSize[i] >= currentfreeSize[i]) ,
       Assert_E_assertFailed); */
    usedSize[i] = initialfreeSize[i] - currentfreeSize[i];
  }
}

/**
 * @name   omxrpcskel_memstats_getfreedsize
 * @brief  Function to get freed memory stats from all the heaps in the system
 * @param  initialfreeSize          : Pointer to initial free size array
 * @param  freedSize                 : Pointer to freed size array to be populated
 * @param  numHeaps                 : Number of heaps in the system
 * @return Number of heaps in the system
 */
static Void omxrpcskel_memstats_getfreedsize (OMX_U32 initialfreeSize[],
                                              OMX_U32 freedSize[],
                                              OMX_U32 numHeaps)
{
  Int i;
  OMX_U32 currentfreeSize[DOMX_MEMUSAGE_MAX_HEAPS_PER_PHASE];
  OMX_U32 numHeapsEnd;

  DOMX_UTL_TRACE_FUNCTION_ASSERT ((numHeaps <=
                                   DOMX_MEMUSAGE_MAX_HEAPS_PER_PHASE),
                                  "numHeaps > maxHeaps");
  numHeapsEnd =
    omxrpcskel_memstats_getfreestats (&(currentfreeSize[0]),
                                      DOMX_MEMUSAGE_MAX_HEAPS_PER_PHASE);
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((numHeapsEnd == numHeaps),
                                  "numHeaps incorrect");

  for (i = 0; i < numHeaps; i++)
  {
    DOMX_UTL_TRACE_FUNCTION_ASSERT ((currentfreeSize[i] >= initialfreeSize[i]),
                                    "initialfreeSize > currentfreeSize");
    freedSize[i] = currentfreeSize[i] - initialfreeSize[i];
  }
}

/**
 * @name   omxrpcskel_memstats_checkleak
 * @brief  Function to check if memory leak occured
 * @param  allocSize                 : Pointer to alloc size array
 * @param  freedSize                 : Pointer to freed size array
 * @param  numHeaps                  : Number of heaps in the system
 * @param  id                        : ID used to identify stage when printing leaks. 
 * @return Number of heaps in the system
 */
static Void omxrpcskel_memstats_checkleak (OMX_U32 allocSize[],
                                           OMX_U32 freedSize[],
                                           OMX_U32 numHeaps, OMX_U32 id)
{
  Int i;
  for (i = 0; i < numHeaps; i++)
  {
    if (allocSize[i] != freedSize[i])
    {
      Log_print4 (Diags_INFO, "\nDOMXRPCSKEL:MemoryLeak:STAGE:%d\tHEAPNUM:%d\t"
                     "ALLOC=%d\tFREED=%d", id, i, allocSize[i], freedSize[i]);
    //  System_flush ();
    }
  }
}
#endif

#ifdef OMXRPCSKEL_MEMLOG
#define OMXRPCSKEL_MEMLOG_USED_START()                                         \
          do                                                                   \
          {                                                                    \
            OMX_U32 nNumHeaps =      0;                                        \
            OMX_U32 freeMemStart[DOMX_MEMUSAGE_MAX_HEAPS_PER_PHASE];           \
                                                                               \
            nNumHeaps =                                                        \
              omxrpcskel_memstats_getfreestats (freeMemStart,                  \
                                              DOMX_MEMUSAGE_MAX_HEAPS_PER_PHASE)
#else
#define OMXRPCSKEL_MEMLOG_USED_START()
#endif

#ifdef OMXRPCSKEL_MEMLOG
#define OMXRPCSKEL_MEMLOG_USED_END(pUsedSize)                                  \
            omxrpcskel_memstats_getusedsize (freeMemStart, pUsedSize,          \
                                             nNumHeaps);                       \
          } while (0)
#else
#define OMXRPCSKEL_MEMLOG_USED_END(pUsedSize)
#endif

#ifdef OMXRPCSKEL_MEMLOG
#define OMXRPCSKEL_MEMLOG_FREE_START()                                         \
          do                                                                   \
          {                                                                    \
            OMX_U32 nNumHeaps =      0;                                        \
            OMX_U32 freeMemStart[DOMX_MEMUSAGE_MAX_HEAPS_PER_PHASE];           \
            OMX_U32 freedSize[DOMX_MEMUSAGE_MAX_HEAPS_PER_PHASE];              \
                                                                               \
            nNumHeaps =                                                        \
              omxrpcskel_memstats_getfreestats (freeMemStart,                  \
                                             DOMX_MEMUSAGE_MAX_HEAPS_PER_PHASE)
#else
#define OMXRPCSKEL_MEMLOG_FREE_START()
#endif

#ifdef OMXRPCSKEL_MEMLOG
#define OMXRPCSKEL_MEMLOG_FREE_END(pAllocSize,id)                              \
            omxrpcskel_memstats_getfreedsize (freeMemStart, freedSize,         \
                                              nNumHeaps);                      \
            omxrpcskel_memstats_checkleak (pAllocSize, freedSize, nNumHeaps,   \
                                                                    id);       \
          } while (0)
#else
#define OMXRPCSKEL_MEMLOG_FREE_END(pAllocSize,id)
#endif

/**
 * @name   omxrpc_msg_marshall_gethandle_response
 * @brief  Function to marshall the arguments of GetHable API response into a msg
 * @param  pRpcGetHandleResponseMsg : Pointer to GetHandle response msg
 * @param  componentCtrlServerName  : Name of the components ctrlRcmServer
 * @param  pOmxComponentHandle      : Real component handle
 * @param  pRpcSkelHandle           : Pointer to RPCSkel Handle
 * @return None
 */
static Void
  omxrpc_msg_marshall_gethandle_response (OmxRpc_msgGetHandleResponse
                                            *pRpcGetHandleResponseMsg,
                                          String componentCtrlServerName,
                                          OmxTypes_OMX_HANDLETYPE
                                            pOmxComponentHandle,
                                          OmxRpc_Handle pRpcSkelHandle)
{
  DOMX_UTL_Strncpy ((OMX_STRING) pRpcGetHandleResponseMsg->
                    componentCtrlServerName, componentCtrlServerName,
                    sizeof (pRpcGetHandleResponseMsg->componentCtrlServerName));
  pRpcGetHandleResponseMsg->pOmxComponentHandle = pOmxComponentHandle;
  pRpcGetHandleResponseMsg->skelHandle = pRpcSkelHandle;
}

/* Bit values for each of the getHandle types */

/* Value 0x0 - binary value: 0000 0000 */
#define OMXRPC_RSRC_GETHANDLE_NONE                           0

/* Value 0x1 - binary value: 0000 0001 */
#define OMXRPC_RSRC_GETHANDLE_COMPONENT_RCM_CLIENT_CREATE    (1 << 0)

#define OMXRPC_RSRC_GETHANDLE_ALL        (                                     \
	                          OMXRPC_RSRC_GETHANDLE_COMPONENT_RCM_CLIENT_CREATE)

/**
 * @name   omxrpc_skel_gethandle
 * @brief  OmxRpc skeleton function for OMX_GetHandle API
 * @param  pRpcHndl                 : OmxRpc Skel handle
 * @param  pRpcMsg                  : Pointer to OmxRpc msg received
 * @param  pOmxErr                  : Pointer to OmxERROR_TYPE to be populated
 * @return OmxRpc_errorNone         : SUCCESS
 */
static OmxRpc_errorType omxrpc_skel_gethandle (OmxRpc_Handle pRpcHndl,
                                               OmxRpc_msg *pRpcMsg,
                                               OMX_ERRORTYPE *pOmxErr)
{
  OmxRpc_errorType retVal = OmxRpc_errorNone;
  Error_Block eb;
  Int rsrcMask = OMXRPC_RSRC_GETHANDLE_NONE;
  OMX_CALLBACKTYPE rpcCallBackInfo =
  {
    OmxRpc_stubCbEventHandler,
    OmxRpc_stubCbEmptyBufferDone,
    OmxRpc_stubCbFillBufferDone
  };

  /* Log function entry */
  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();
  /* must initialize the error block before using it */
  Error_init (&eb);
  /* The component name present in the getHandle message must match the skel
     handle component name.Else indicates instance memory is corrupted or
     component name is not assigned */
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((strcmp ((const char *)
                                           pRpcMsg->api.getHandle.componentName,
                                           (const char *) pRpcHndl->
                                           szComponentName) == 0),
                                  "compName mismatch");
  OMXRPCSKEL_MEMLOG_USED_START ();
  *pOmxErr =
    OMX_GetHandle ((OMX_HANDLETYPE *) & pRpcHndl->layer.skel.
                   pComponentHandle,
                   (OMX_STRING) pRpcMsg->api.getHandle.componentName,
                   /* Rpc Skel Handle is the pAppData for the component */
                   pRpcHndl, &rpcCallBackInfo);
  OMXRPCSKEL_MEMLOG_USED_END (&
                              (pRpcHndl->
                               memoryUsed
                               [DOMXRPCSKEL_MEMUSAGEPHASE_COMPONENTGETHANDLE]
                               [0]));
  if (OMX_ErrorNone != *pOmxErr)
  {
    Log_print1 (Diags_USER1, "Error Getting Handle: Error : %d", *pOmxErr);
    retVal = OmxRpc_errorOmxComponentError;
  }
  else
  {
    OMXRPCSKEL_MEMLOG_USED_START ();
    /* Create Rcm Client for Event Cb */
    /* if the remote component was successfully created do the following: -->
       Create RcmClient to communicate with Component's Rcm Server --> Get the
       fxnIdx corresponding to the componentAPIs */
    /* Create the Rcm Client associated with the component specific RcmServer */
    DOMX_UTL_Strncpy ((OMX_STRING) pRpcHndl->client.name,
                      (OMX_STRING) pRpcMsg->api.getHandle.
                      componentEventCbServerName,
                      sizeof (pRpcHndl->client.name));
    retVal =
      OmxRpc_rcmClientCreate ((RcmClient_Handle *) (&pRpcHndl->client.
                                                    handle),
                              (String) pRpcHndl->client.name,
                              OmxRpc_module->remoteCoreRcmClient[pRpcHndl->
                                                                 remoteCoreId].
                              msgHeap.msgHeapId);
    DOMX_UTL_TRACE_FUNCTION_ERROR ((OmxRpc_errorNone == retVal),
                                   "OmxRpc_rcmClientCreate failed");

    /* Debug */
    if (pRpcHndl->client.fxns.numEntries == DOMX_INT_MAX_UINT16_VALUE)
    {
      pRpcHndl->client.fxns.numEntries = 0;
    }
    /* Debug */
    if (OmxRpc_errorNone == retVal)
    {
      rsrcMask |= OMXRPC_RSRC_GETHANDLE_COMPONENT_RCM_CLIENT_CREATE;
      /* Locate the fxnId to the component specifc RcmServer registered
         functions */
      retVal = OmxRpc_rcmClientRemoteFxnLocate ((RcmClient_Handle) pRpcHndl->
                                                client.handle,
                                                OmxRpc_remoteFxnCb,
                                                &pRpcHndl->client.fxns);
      DOMX_UTL_TRACE_FUNCTION_ERROR ((OmxRpc_errorNone == retVal),
                                     "OmxRpc_rcmClientRemoteFxnLocate failed");
    }                           /* end of if (rcmClientCreate success) */
    OMXRPCSKEL_MEMLOG_USED_END (&
                                (pRpcHndl->
                                 memoryUsed
                                 [DOMXRPCSKEL_MEMUSAGEPHASE_RCMCLIENTCREATE]
                                 [0]));
  }
  if (OmxRpc_errorNone != retVal)
  {
    if (rsrcMask & OMXRPC_RSRC_GETHANDLE_COMPONENT_RCM_CLIENT_CREATE)
    {
      DOMX_UTL_TRACE_FUNCTION_MSG ("Before RcmClient_delete");
      RcmClient_delete ((RcmClient_Handle *) (&pRpcHndl->client.handle));
      DOMX_UTL_TRACE_FUNCTION_MSG ("After RcmClient_delete");
    }
  }
  else
  {
    if (OMX_ErrorNone == *pOmxErr)
    {
      DomxTunnelMgr_registerSkelInfo (pRpcHndl->layer.skel.pComponentHandle,
                                      (Char *) pRpcMsg->api.getHandle.
                                      componentName, pRpcHndl,
                                      (Char *) pRpcHndl->server.name);
    }
  }

  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (retVal);
  return retVal;
}

/**
 * @name   omxrpc_skel_get_set_param_config
 * @brief  OmxRpc skeleton function for OMX_ Get/Set or Param/COnfig API
 * @param  pRpcHndl                 : OmxRpc Skel handle
 * @param  pRpcMsg                  : Pointer to OmxRpc msg received
 * @param  pOmxErr                  : Pointer to OmxERROR_TYPE to be populated
 * @return OmxRpc_errorNone         : SUCCESS
 */
static OmxRpc_errorType
  omxrpc_skel_get_set_param_config (OmxRpc_Handle pRpcHndle,
                                    OmxRpc_msg *pRpcMsg, OMX_ERRORTYPE *pOmxErr)
{
  OMX_HANDLETYPE hComp;
  OMX_COMPONENTTYPE *pHandle;
  OMX_INDEXTYPE nParamIndex;
  OMX_PTR pCompParamStruct = NULL;

  hComp = (OMX_HANDLETYPE) pRpcHndle->layer.skel.pComponentHandle;
  pHandle = (OMX_COMPONENTTYPE *) hComp;
  /* Ensure Rpc cmd is one of Get/Set Config/Param */
  DOMX_UTL_TRACE_FUNCTION_ASSERT
    (((OmxRpc_cmdGetParameter == pRpcMsg->common.eRpcCmd) ||
      (OmxRpc_cmdGetConfig == pRpcMsg->common.eRpcCmd)    ||
      (OmxRpc_cmdSetParameter == pRpcMsg->common.eRpcCmd) ||
      (OmxRpc_cmdSetConfig == pRpcMsg->common.eRpcCmd)),
     "rpcCmd invalid");
  if ((OmxRpc_cmdGetParameter == pRpcMsg->common.eRpcCmd) ||
      (OmxRpc_cmdGetConfig == pRpcMsg->common.eRpcCmd))
  {
    nParamIndex = (OMX_INDEXTYPE) pRpcMsg->api.getParamConfig.nParamConfigIndex;
    pCompParamStruct = &(pRpcMsg->api.getParamConfig.cCompParamStructBuf[0]);
  }
  else
  {
    nParamIndex = (OMX_INDEXTYPE) pRpcMsg->api.setParamConfig.nParamConfigIndex;
    pCompParamStruct = &(pRpcMsg->api.setParamConfig.cCompParamStructBuf[0]);
  }
  switch (pRpcMsg->common.eRpcCmd)
  {
    case OmxRpc_cmdGetParameter:
      /* TODO: Special Handling may be required for
         'OMX_IndexParamPortDefinition' because based on the type of port, we
         may need to handle a union withing this 'OMX_PARAM_PORTDEFINITIONTYPE'
         * * structure. */
      *pOmxErr = pHandle->GetParameter (hComp, nParamIndex, pCompParamStruct);
      break;
    case OmxRpc_cmdSetParameter:
      *pOmxErr = pHandle->SetParameter (hComp, nParamIndex, pCompParamStruct);
      break;
    case OmxRpc_cmdGetConfig:
      *pOmxErr = pHandle->GetConfig (hComp, nParamIndex, pCompParamStruct);
      break;
    case OmxRpc_cmdSetConfig:
      *pOmxErr = pHandle->SetConfig (hComp, nParamIndex, pCompParamStruct);
      break;
      /* default case is not required since Assert above ensure cmd is one of
         set/get param/config */
    default:
      DOMX_UTL_TRACE_FUNCTION_ASSERT
        (((OmxRpc_cmdGetParameter == pRpcMsg->common.eRpcCmd) ||
          (OmxRpc_cmdGetConfig == pRpcMsg->common.eRpcCmd)    ||
          (OmxRpc_cmdSetParameter == pRpcMsg->common.eRpcCmd) ||
          (OmxRpc_cmdSetConfig == pRpcMsg->common.eRpcCmd)),
         "rpcCmd invalid");
      break;
  } /* end of switch (pRpcMsg->common.eRpcCmd) */
  return OmxRpc_errorNone;
}

/**
 * @name   omxrpc_skel_getstate
 * @brief  OmxRpc skeleton function for OMX_ GetState API
 * @param  pRpcHndl                 : OmxRpc Skel handle
 * @param  pRpcMsg                  : Pointer to OmxRpc msg received
 * @param  pOmxErr                  : Pointer to OmxERROR_TYPE to be populated
 * @return OmxRpc_errorNone         : SUCCESS
 */
static OmxRpc_errorType omxrpc_skel_getstate (OmxRpc_Handle pRpcHndle,
                                              OmxRpc_msg *pRpcMsg,
                                              OMX_ERRORTYPE *pOmxErr)
{
  OMX_HANDLETYPE hComp;
  OMX_COMPONENTTYPE *pHandle;
  OMX_STATETYPE *pState;

  hComp = (OMX_HANDLETYPE) pRpcHndle->layer.skel.pComponentHandle;
  pHandle = (OMX_COMPONENTTYPE *) hComp;

    /*** point to the msg***/
  pState = (OMX_PTR) (&pRpcMsg->api.getStateResponse.eState);

    /*** call real handler to get the value back***/
  *pOmxErr = pHandle->GetState (hComp, pState);
  return OmxRpc_errorNone;
}

/**
 * @name   omxrpc_skel_sendcommand
 * @brief  OmxRpc skeleton function for OMX_SendCommand API
 * @param  pRpcHndl                 : OmxRpc Skel handle
 * @param  pRpcMsg                  : Pointer to OmxRpc msg received
 * @param  pOmxErr                  : Pointer to OmxERROR_TYPE to be populated
 * @return OmxRpc_errorNone         : SUCCESS
 */
static OmxRpc_errorType omxrpc_skel_sendcommand (OmxRpc_Handle pRpcHndle,
                                                 OmxRpc_msg *pRpcMsg,
                                                 OMX_ERRORTYPE *pOmxErr)
{
  OMX_HANDLETYPE hComp;
  OMX_COMPONENTTYPE *pHandle;
  OMX_COMMANDTYPE eCmd;
  OMX_PTR pCmdData;
  OMX_U32 nParam1;

  hComp = (OMX_HANDLETYPE) pRpcHndle->layer.skel.pComponentHandle;
  pHandle = (OMX_COMPONENTTYPE *) hComp;

  eCmd = (OMX_COMMANDTYPE) (pRpcMsg->api.sendCommand.eCmd);
  nParam1 = pRpcMsg->api.sendCommand.nParam;
  if (OMX_CommandMarkBuffer == eCmd)
  {
    pCmdData = (OMX_PTR) (&pRpcMsg->api.sendCommand.markData);
  }
  else
  {
    pCmdData = NULL;
  }
  *pOmxErr = pHandle->SendCommand (hComp, eCmd, nParam1, pCmdData);
  return OmxRpc_errorNone;
}

/**
 * @name   omxrpc_skel_getcomponentversion
 * @brief  OmxRpc skeleton function for OMX_GetComponentVersion API
 * @param  pRpcHndl                 : OmxRpc Skel handle
 * @param  pRpcMsg                  : Pointer to OmxRpc msg received
 * @param  pOmxErr                  : Pointer to OmxERROR_TYPE to be populated
 * @return OmxRpc_errorNone         : SUCCESS
 */
static OmxRpc_errorType
  omxrpc_skel_getcomponentversion (OmxRpc_Handle pRpcHndle, OmxRpc_msg *pRpcMsg,
                                   OMX_ERRORTYPE *pOmxErr)
{
  OMX_HANDLETYPE hComp;
  OMX_COMPONENTTYPE *pHandle;
  OmxRpc_msgGetComponentVersionResponse *pVers;

  hComp = (OMX_HANDLETYPE) pRpcHndle->layer.skel.pComponentHandle;
  pHandle = (OMX_COMPONENTTYPE *) hComp;

  pVers = &pRpcMsg->api.getComponentVersionResponse;

    /*** call real handler to get the value back***/
  *pOmxErr = pHandle->GetComponentVersion (hComp,
                                           (OMX_STRING) pVers->componentName,
                                           (OMX_VERSIONTYPE *) & (pVers->
                                                                  componentVersion),
                                           (OMX_VERSIONTYPE *) & (pVers->
                                                                  specVersion),
                                           (OMX_UUIDTYPE *) & (pVers->
                                                               componentUUID));

  return OmxRpc_errorNone;
}

/**
 * @name   omxrpc_skel_getcomponentversion
 * @brief  OmxRpc skeleton function for OMX_GetExtensionIndex API
 * @param  pRpcHndl                 : OmxRpc Skel handle
 * @param  pRpcMsg                  : Pointer to OmxRpc msg received
 * @param  pOmxErr                  : Pointer to OmxERROR_TYPE to be populated
 * @return OmxRpc_errorNone         : SUCCESS
 */
static OmxRpc_errorType
  omxrpc_skel_getextensionindex (OmxRpc_Handle pRpcHndle, OmxRpc_msg *pRpcMsg,
                                 OMX_ERRORTYPE *pOmxErr)
{
  OMX_HANDLETYPE hComp;
  OMX_COMPONENTTYPE *pHandle;
  OMX_INDEXTYPE eIndex;

  /* parse the incoming msg** */
  hComp = (OMX_HANDLETYPE) pRpcHndle->layer.skel.pComponentHandle;
  pHandle = (OMX_COMPONENTTYPE *) hComp;

        /*** call real handler to get the value back***/
  *pOmxErr = pHandle->GetExtensionIndex (hComp,
                                         (OMX_STRING) pRpcMsg->api.
                                         getExtensionIndex.cParameterName,
                                         &eIndex);
  pRpcMsg->api.getExtensionIndexResponse.nIndexType =
    (OmxIndex_OMX_INDEXTYPE) eIndex;
  return OmxRpc_errorNone;
}

/**
 * @name   omxrpc_skel_emptythisbuffer
 * @brief  OmxRpc skeleton function for OMX_EmptyThisBuffer API
 * @param  pRpcHndl                 : OmxRpc Skel handle
 * @param  pRpcMsg                  : Pointer to OmxRpc msg received
 * @param  pOmxErr                  : Pointer to OmxERROR_TYPE to be populated
 * @return OmxRpc_errorNone         : SUCCESS
 */
static OmxRpc_errorType omxrpc_skel_emptythisbuffer (OmxRpc_Handle pRpcHndle, 
                                                     OmxRpc_msg *pRpcMsg,
                                                     OMX_ERRORTYPE *pOmxErr)
{
  OMX_HANDLETYPE hComp;
  OMX_COMPONENTTYPE *pHandle;
  OMX_BUFFERHEADERTYPE *pBufHdr = NULL;

  Log_print0 (Diags_USER2, "PROCESS_EmptyThisBuffer: Entered Function\n");

  hComp = (OMX_HANDLETYPE) pRpcHndle->layer.skel.pComponentHandle;
  pHandle = (OMX_COMPONENTTYPE *) hComp;

  pBufHdr =
    (OMX_BUFFERHEADERTYPE *) pRpcMsg->api.emptyFillBuffer.pBufferHdrRemote;
  /* Copy the contents of buffer header passed in the message into the remote
     buffer header pointer location This bufhdr is allocated by the component */
  memcpy (pBufHdr, &(pRpcMsg->api.emptyFillBuffer.bufferHdrCopy),
          sizeof (OMX_BUFFERHEADERTYPE));
  Log_print1 (Diags_USER2, "ETB: filledLen  %d", pBufHdr->nFilledLen);

  OmxRpc_PDmm_xltMapBuf2Ptr ((UInt32) pRpcMsg->api.emptyFillBuffer.
                             bufferHdrCopy.pBuffer, &(pBufHdr->pBuffer));
  /* calling cache invalidation */
  MEMUTILS_cacheInv (pBufHdr->pBuffer, pBufHdr->nAllocLen);
  *pOmxErr = pHandle->EmptyThisBuffer (hComp, pBufHdr);

  return OmxRpc_errorNone;
}

/**
 * @name   omxrpc_skel_fillthisbuffer
 * @brief  OmxRpc skeleton function for OMX_FillThisBuffer API
 * @param  pRpcHndl                 : OmxRpc Skel handle
 * @param  pRpcMsg                  : Pointer to OmxRpc msg received
 * @param  pOmxErr                  : Pointer to OmxERROR_TYPE to be populated
 * @return OmxRpc_errorNone         : SUCCESS
 */
static OmxRpc_errorType omxrpc_skel_fillthisbuffer (OmxRpc_Handle pRpcHndle,
                                                    OmxRpc_msg *pRpcMsg,
                                                    OMX_ERRORTYPE *pOmxErr)
{
  OMX_HANDLETYPE hComp;
  OMX_COMPONENTTYPE *pHandle;
  OMX_BUFFERHEADERTYPE *pBufHdr = NULL;

  Log_print1 (Diags_USER2, "Entered Function :%s", (xdc_IArg) __FUNCTION__);
  hComp = (OMX_HANDLETYPE) pRpcHndle->layer.skel.pComponentHandle;
  pHandle = (OMX_COMPONENTTYPE *) hComp;
  pBufHdr =
    (OMX_BUFFERHEADERTYPE *) pRpcMsg->api.emptyFillBuffer.pBufferHdrRemote;
  /* Copy the contents of buffer header passed in the message into the remote
     buffer header pointer location This bufhdr is allocated by the component */
  memcpy (pBufHdr, &(pRpcMsg->api.emptyFillBuffer.bufferHdrCopy),
          sizeof (OMX_BUFFERHEADERTYPE));
  OmxRpc_PDmm_xltMapBuf2Ptr ((UInt32) pRpcMsg->api.emptyFillBuffer.
                             bufferHdrCopy.pBuffer, &(pBufHdr->pBuffer));
  /* Calling the actual FTB onthe */
  *pOmxErr = pHandle->FillThisBuffer (hComp, pBufHdr);
  return OmxRpc_errorNone;
}

/**
 * @name   omxrpc_skel_usebuffer
 * @brief  OmxRpc skeleton function for OMX_UseBuffer API
 * @param  pRpcHndl                 : OmxRpc Skel handle
 * @param  pRpcMsg                  : Pointer to OmxRpc msg received
 * @param  pOmxErr                  : Pointer to OmxERROR_TYPE to be populated
 * @return OmxRpc_errorNone         : SUCCESS
 */
static OmxRpc_errorType omxrpc_skel_usebuffer (OmxRpc_Handle pRpcHndle,
                                               OmxRpc_msg *pRpcMsg,
                                               OMX_ERRORTYPE *pOmxErr)
{
  OMX_HANDLETYPE hComp;
  OMX_COMPONENTTYPE *pHandle;
  OMX_BUFFERHEADERTYPE *ppBufferHdr;
  OMX_U32 nPortIndex;
  OMX_U32 nSizeBytes;
  OMX_PTR pAppPrivate;
  OMX_U8 *pBuffer;

  Log_print1 (Diags_USER1, "Entered Function :%s", (xdc_IArg) __FUNCTION__);
  hComp = (OMX_HANDLETYPE) pRpcHndle->layer.skel.pComponentHandle;
  pHandle = (OMX_COMPONENTTYPE *) hComp;
  nPortIndex = pRpcMsg->api.useBuffer.nPortIndex;
  pAppPrivate = pRpcMsg->api.useBuffer.pAppPrivate;
  nSizeBytes = pRpcMsg->api.useBuffer.nSizeBytes;
  OmxRpc_PDmm_xltMapBuf2Ptr (pRpcMsg->api.useBuffer.nBufferMapped, &pBuffer);

  /* Now calling Use buffer */
  *pOmxErr = pHandle->UseBuffer (hComp, &ppBufferHdr, nPortIndex, pAppPrivate,
                                 nSizeBytes, pBuffer);

  /* Need the local BufferHeader address for filling it up during a ETB and FTB 
     calls */
  if (*pOmxErr == OMX_ErrorNone)
  {
    pRpcMsg->api.useBufferResponse.pBufferHeaderRemote =
      (OmxCore_OMX_BUFFERHEADERTYPE *) ppBufferHdr;
    /* Now we have received a pointer to local BUFFERHEADER in *ppBufferHdr The 
     * entire buffer header information retrieved from ppBufferHdr pointer * is 
     * copied into the message buffer and sent to MPU side for make a local
     * copy. * THe contents are copied at the end of argument list */
    memcpy (&pRpcMsg->api.useBufferResponse.bufferHeaderCopy,
            ppBufferHdr, sizeof (OMX_BUFFERHEADERTYPE));
  }
  else
  {
    Log_print1 (Diags_USER1,
                "PROCESS_UseBuffer: Error in UseBuffer 0x%x",
                (xdc_IArg) * pOmxErr);
  }
  return OmxRpc_errorNone;
}

/**
 * @name   omxrpc_skel_allocbuffer
 * @brief  OmxRpc skeleton function for OMX_AllocBuffer API
 * @param  pRpcHndl                 : OmxRpc Skel handle
 * @param  pRpcMsg                  : Pointer to OmxRpc msg received
 * @param  pOmxErr                  : Pointer to OmxERROR_TYPE to be populated
 * @return OmxRpc_errorNone         : SUCCESS
 */
static OmxRpc_errorType omxrpc_skel_allocbuffer (OmxRpc_Handle pRpcHndle,
                                                 OmxRpc_msg *pRpcMsg,
                                                 OMX_ERRORTYPE *pOmxErr)
{
  OMX_HANDLETYPE hComp;
  OMX_COMPONENTTYPE *pHandle;
  OMX_BUFFERHEADERTYPE *ppBufferHdr;
  OMX_U32 nPortIndex;
  OMX_U32 nSizeBytes;
  OMX_PTR pAppPrivate;
  IDmm_errorType dmmErrorCode;
  UInt32 mappedBufAddress = 0x0;

  Log_print1 (Diags_USER1, "Entered Function :%s", (xdc_IArg) __FUNCTION__);
  hComp = (OMX_HANDLETYPE) pRpcHndle->layer.skel.pComponentHandle;
  pHandle = (OMX_COMPONENTTYPE *) hComp;
  nPortIndex = pRpcMsg->api.allocBuffer.nPortIndex;
  pAppPrivate = pRpcMsg->api.allocBuffer.pAppPrivate;
  nSizeBytes = pRpcMsg->api.allocBuffer.nSizeBytes;

  /* Now calling Use buffer */
  *pOmxErr =
    pHandle->AllocateBuffer (hComp, &ppBufferHdr, nPortIndex, pAppPrivate,
                             nSizeBytes);

  /* Need the local BufferHeader address for filling it up during a ETB and FTB 
     calls */
  if (*pOmxErr == OMX_ErrorNone)
  {
    pRpcMsg->api.allocBufferResponse.pBufferHeaderRemote =
      (OmxCore_OMX_BUFFERHEADERTYPE *) ppBufferHdr;
    /* Now we have received a pointer to local BUFFERHEADER in *ppBufferHdr The 
     * entire buffer header information retrieved from ppBufferHdr pointer * is 
     * copied into the message buffer and sent to MPU side for make a local
     * copy. * THe contents are copied at the end of argument list */
    memcpy (&pRpcMsg->api.allocBufferResponse.bufferHeaderCopy,
            ppBufferHdr, sizeof (OMX_BUFFERHEADERTYPE));
    dmmErrorCode = OmxRpc_PDmm_mapBuffer (pRpcHndle->remoteCoreId,
                                          ppBufferHdr->pBuffer,
                                          ppBufferHdr->nAllocLen,
                                          &mappedBufAddress);
    if (IDmm_errorNone != dmmErrorCode)
    {
      *pOmxErr = OMX_ErrorUndefined;
    }
    else
    {
      pRpcMsg->api.allocBufferResponse.nBufferMapped = mappedBufAddress;
    }
  }
  else
  {
    Log_print1 (Diags_USER1,
                "PROCESS_AllocBuffer: Error in AllocBuffer 0x%x",
                (xdc_IArg) * pOmxErr);
  }
  return OmxRpc_errorNone;
}

/**
 * @name   omxrpc_skel_freebuffer
 * @brief  OmxRpc skeleton function for OMX_FreeBuffer API
 * @param  pRpcHndl                 : OmxRpc Skel handle
 * @param  pRpcMsg                  : Pointer to OmxRpc msg received
 * @param  pOmxErr                  : Pointer to OmxERROR_TYPE to be populated
 * @return OmxRpc_errorNone         : SUCCESS
 */
static OmxRpc_errorType omxrpc_skel_freebuffer (OmxRpc_Handle pRpcHndle,
                                                OmxRpc_msg *pRpcMsg,
                                                OMX_ERRORTYPE *pOmxErr)
{
  OMX_HANDLETYPE hComp;
  OMX_COMPONENTTYPE *pHandle;
  OMX_U32 nPortIndex = 0;
  OMX_BUFFERHEADERTYPE *pBufHdr;

  Log_print1 (Diags_USER1, "Entered Function :%s", (xdc_IArg) __FUNCTION__);
  hComp = (OMX_HANDLETYPE) pRpcHndle->layer.skel.pComponentHandle;
  pHandle = (OMX_COMPONENTTYPE *) hComp;
  nPortIndex = pRpcMsg->api.freeBuffer.nPortIndex;
  pBufHdr = (OMX_BUFFERHEADERTYPE *) pRpcMsg->api.freeBuffer.pBufHeaderRemote;

  *pOmxErr = pHandle->FreeBuffer (hComp, nPortIndex, pBufHdr);

  return OmxRpc_errorNone;
}

/**
 * @name   omxrpc_skel_freehandle
 * @brief  OmxRpc skeleton function for OMX_FreeHandle API
 * @param  pRpcHndl                 : OmxRpc Skel handle
 * @param  pRpcMsg                  : Pointer to OmxRpc msg received
 * @param  pOmxErr                  : Pointer to OmxERROR_TYPE to be populated
 * @return OmxRpc_errorNone         : SUCCESS
 */
static OmxRpc_errorType omxrpc_skel_freehandle (OmxRpc_Handle pRpcHndle,
                                                OmxRpc_msg *pRpcMsg,
                                                OMX_ERRORTYPE *pOmxErr)
{
  OMX_HANDLETYPE hComp;
  OMX_COMPONENTTYPE *pHandle;

  Log_print1 (Diags_ENTRY, "Entered Function :%s \n", (xdc_IArg) __FUNCTION__);

  (Void) pRpcMsg;
  hComp = (OMX_HANDLETYPE) pRpcHndle->layer.skel.pComponentHandle;
  pHandle = (OMX_COMPONENTTYPE *) hComp;
  OMXRPCSKEL_MEMLOG_FREE_START ();
  *pOmxErr = OMX_FreeHandle (pHandle);
  OMXRPCSKEL_MEMLOG_FREE_END (&(pRpcHndle->
                                memoryUsed
                                [DOMXRPCSKEL_MEMUSAGEPHASE_COMPONENTGETHANDLE]
                                [0]),
                              DOMXRPCSKEL_MEMUSAGEPHASE_COMPONENTGETHANDLE);

  Log_print1 (Diags_EXIT, "Leaving %s: \n", (xdc_IArg) __FUNCTION__);

  return OmxRpc_errorNone;
}

/**
 * @name   omxrpc_skel_createproxylite
 * @brief  OmxRpc skeleton function to create a proxy lite instance
 * @param  pRpcHndl                 : OmxRpc Skel handle
 * @param  pRpcMsg                  : Pointer to OmxRpc msg received
 * @param  pOmxErr                  : Pointer to OmxERROR_TYPE to be populated
 * @return OmxRpc_errorNone         : SUCCESS
 */
static OmxRpc_errorType omxrpc_skel_createproxylite (OmxRpc_Handle pRpcHndle,
                                                     OmxRpc_msg *pRpcMsg,
                                                     OMX_ERRORTYPE *pOmxErr)
{
  OMX_HANDLETYPE pProxyLiteHandlePtr;
  OmxProxy_proxyLiteCreateParams cp;

  Log_print1 (Diags_USER1, "Entered Function :%s", (xdc_IArg) __FUNCTION__);
  cp.nRpcRemoteHandle = pRpcMsg->api.createProxyLite.nRpcRemoteHandle;
  cp.pOmxComponentHandle = pRpcMsg->api.createProxyLite.pOmxComponentHandle;
  cp.cComponentName =
    (OmxTypes_OMX_S8 *) pRpcMsg->api.createProxyLite.cComponentName;
  cp.cComponentRcmServerName =
    (OmxTypes_OMX_S8 *) pRpcMsg->api.createProxyLite.cComponentRcmServerName;
  *pOmxErr = OmxProxy_createLite (&pProxyLiteHandlePtr, &cp);

  if (*pOmxErr == OMX_ErrorNone)
  {
    pRpcMsg->api.createProxyLiteResponse.pProxyLiteHandle = pProxyLiteHandlePtr;
  }
  else
  {
    Log_print1 (Diags_USER1,
                "PROCESS_UseBuffer: Error in UseBuffer 0x%x",
                (xdc_IArg) * pOmxErr);
    pRpcMsg->api.createProxyLiteResponse.pProxyLiteHandle = NULL;
  }
  return OmxRpc_errorNone;
}

/**
 * @name   omxrpc_skel_deleteproxylite
 * @brief  OmxRpc skeleton function for deleting proxy
 * @param  pRpcHndl                 : OmxRpc Skel handle
 * @param  pRpcMsg                  : Pointer to OmxRpc msg received
 * @param  pOmxErr                  : Pointer to OmxERROR_TYPE to be populated
 * @return OmxRpc_errorNone         : SUCCESS
 */

static OmxRpc_errorType omxrpc_skel_deleteproxylite (OmxRpc_Handle pRpcHndle,
                                                     OmxRpc_msg *pRpcMsg,
                                                     OMX_ERRORTYPE *pOmxErr)
{
  OMX_HANDLETYPE pProxyLiteHandlePtr;

  pProxyLiteHandlePtr = pRpcMsg->api.deleteProxyLite.pProxyLiteHandle;
  Log_print1 (Diags_USER1, "Entered Function :%s", (xdc_IArg) __FUNCTION__);
  *pOmxErr = OmxProxy_deleteLite (&pProxyLiteHandlePtr);

  if (*pOmxErr != OMX_ErrorNone)
  {
    Log_print1 (Diags_USER1,
                "omxrpc_skel_deleteproxylite: Error in DeleteProxyLite 0x%x",
                (xdc_IArg) * pOmxErr);
  }
  return OmxRpc_errorNone;
}

/**
 * @name   omxrpc_skel_componenttunnelrequest
 * @brief  OmxRpc skeleton function for OMX_ComponentTunnelRequest API
 * @param  pRpcHndl                 : OmxRpc Skel handle
 * @param  pRpcMsg                  : Pointer to OmxRpc msg received
 * @param  pOmxErr                  : Pointer to OmxERROR_TYPE to be populated
 * @return OmxRpc_errorNone         : SUCCESS
 */
static OmxRpc_errorType
  omxrpc_skel_componenttunnelrequest (OmxRpc_Handle pRpcHndle,
                                      OmxRpc_msg *pRpcMsg,
                                      OMX_ERRORTYPE *pOmxErr)
{
  OMX_HANDLETYPE hComp;
  OMX_COMPONENTTYPE *pHandle;
  OMX_U32 nPort, nTunneledPort;
  OMX_HANDLETYPE hTunneledComp;
  OMX_TUNNELSETUPTYPE sTunnelSetup;
  OMX_TUNNELSETUPTYPE *pTunnelSetup;
  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();
  hComp = (OMX_HANDLETYPE) pRpcHndle->layer.skel.pComponentHandle;
  pHandle = (OMX_COMPONENTTYPE *) hComp;
  nPort = pRpcMsg->api.componentTunnelRequest.nPort;
  hTunneledComp = pRpcMsg->api.componentTunnelRequest.hTunneledComp;
  nTunneledPort = pRpcMsg->api.componentTunnelRequest.nTunneledPort;
  if (OmxTypes_OMX_TRUE ==
      pRpcMsg->api.componentTunnelRequest.bValidTunnelSetup)
  {
    sTunnelSetup.nTunnelFlags =
      pRpcMsg->api.componentTunnelRequest.sTunnelSetup.nTunnelFlags;
    sTunnelSetup.eSupplier =
      (OMX_BUFFERSUPPLIERTYPE) pRpcMsg->api.componentTunnelRequest.
      sTunnelSetup.eSupplier;
    pTunnelSetup = &sTunnelSetup;
  }
  else
  {
    pTunnelSetup = NULL;
  }

  *pOmxErr = pHandle->ComponentTunnelRequest (hComp,
                                              nPort,
                                              hTunneledComp,
                                              nTunneledPort, pTunnelSetup);
  if (NULL != pTunnelSetup)
  {
    pRpcMsg->api.componentTunnelRequestResponse.sTunnelSetup.nTunnelFlags =
      pTunnelSetup->nTunnelFlags;
    pRpcMsg->api.componentTunnelRequestResponse.sTunnelSetup.eSupplier =
      (OmxCore_OMX_BUFFERSUPPLIERTYPE) pTunnelSetup->eSupplier;
  }
  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (OmxRpc_errorNone);
  return OmxRpc_errorNone;
}

/**
 * @name   omxRpcSkelApi
 * @brief  Function pointer type for handling RPCSkel functions
 */
typedef OmxRpc_errorType (*omxRpcSkelApi) (OmxRpc_Handle pRpcHndle,
                                           OmxRpc_msg *pRpcMsg,
                                           OMX_ERRORTYPE *pOmxErr);

/* The skel_api_tbl directly maps OmxRpc_cmd enums to function handlers. Any
 * change in enum values breaks the mapping table. Hence a check is added which 
 * * * will cause compilation error if enum values change from original values.
 * If error is encountered, modify the skel_api_tbl to correct the mapping as
 * per modified enum values */
#define OMXRPC_CMD_ENUM_VALUE_VALIDATE                                         \
 ((OmxRpc_cmdGetHandle              == 0)                                  &&  \
  (OmxRpc_cmdFreeHandle             == (OmxRpc_cmdGetHandle + 1))          &&  \
  (OmxRpc_cmdAllocBuffer            == (OmxRpc_cmdFreeHandle + 1))         &&  \
  (OmxRpc_cmdUseBuffer              == (OmxRpc_cmdAllocBuffer + 1))        &&  \
  (OmxRpc_cmdFreeBuffer             == (OmxRpc_cmdUseBuffer + 1))          &&  \
  (OmxRpc_cmdSetParameter           == (OmxRpc_cmdFreeBuffer + 1))         &&  \
  (OmxRpc_cmdGetParameter           == (OmxRpc_cmdSetParameter + 1))       &&  \
  (OmxRpc_cmdSetConfig              == (OmxRpc_cmdGetParameter + 1))       &&  \
  (OmxRpc_cmdGetConfig              == (OmxRpc_cmdSetConfig + 1))          &&  \
  (OmxRpc_cmdGetComponentVersion    == (OmxRpc_cmdGetConfig + 1))          &&  \
  (OmxRpc_cmdGetExtensionIndex      == (OmxRpc_cmdGetComponentVersion + 1))&&  \
  (OmxRpc_cmdGetState               == (OmxRpc_cmdGetExtensionIndex + 1))  &&  \
  (OmxRpc_cmdSendCommand            == (OmxRpc_cmdGetState + 1))           &&  \
  (OmxRpc_cmdEmptyThisBuffer        == (OmxRpc_cmdSendCommand + 1))        &&  \
  (OmxRpc_cmdFillThisBuffer         == (OmxRpc_cmdEmptyThisBuffer + 1))    &&  \
  (OmxRpc_cmdCreateProxyLite        == (OmxRpc_cmdFillThisBuffer  + 1))    &&  \
  (OmxRpc_cmdDeleteProxyLite        == (OmxRpc_cmdCreateProxyLite  + 1))   &&  \
  (OmxRpc_cmdComponentTunnelRequest == (OmxRpc_cmdDeleteProxyLite + 1)))

/**
 * @name   skel_api_tbl
 * @brief  Table mapping OmxRpc command ID to skeleton handler function
 */
static const omxRpcSkelApi skel_api_tbl[] =
{
  omxrpc_skel_gethandle,                /* cmdGetHandle */
  omxrpc_skel_freehandle,               /* cmdFreeHandle */
  omxrpc_skel_allocbuffer,              /* cmdAllocBuffer */
  omxrpc_skel_usebuffer,                /* cmdUseBuffer */
  omxrpc_skel_freebuffer,               /* cmdFreeBuffer */
  omxrpc_skel_get_set_param_config,     /* cmdSetParameter */
  omxrpc_skel_get_set_param_config,     /* cmdGetParameter */
  omxrpc_skel_get_set_param_config,     /* cmdSetConfig */
  omxrpc_skel_get_set_param_config,     /* cmdGetConfig */
  omxrpc_skel_getcomponentversion,      /* cmdGetComponentVersion */
  omxrpc_skel_getextensionindex,        /* cmdGetExtensionIndex */
  omxrpc_skel_getstate,                 /* cmdGetState */
  omxrpc_skel_sendcommand,              /* cmdSendCommand */
  omxrpc_skel_emptythisbuffer,          /* cmdEmptyThisBuffer */
  omxrpc_skel_fillthisbuffer,           /* cmdFillThisBuffer */
  omxrpc_skel_createproxylite,          /* cmdCreateProxyLite */
  omxrpc_skel_deleteproxylite,          /* cmdDeleteProxyLite */
  omxrpc_skel_componenttunnelrequest    /* cmdComponentTunnelRequest */
};

/**
 * @name   OmxRpc_rcmIfSkelOmxApi
 * @brief  OmxRpc Generic Skeleton RcmServer Handler function for all OMX APIs
 * @param  rcmMsgSize               : Size of Rcm message in btes
 * @param  rcmMsg                   : Pointer to Rcm msg received
 * @return OmxRpc_errorNone         : SUCCESS
 */
Int32 OmxRpc_rcmIfSkelOmxApi (UInt32 rcmMsgSize, UInt32 *rcmMsg)
{
  OmxRpc_msg *pRpcMsg = NULL;
  OmxRpc_Handle rpcSkelHandle = NULL;
  OmxRpc_errorType retVal;
  OMX_ERRORTYPE omxErr = OMX_ErrorNone;;

  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL2 ();

  DOMX_UTL_TRACE_FUNCTION_ASSERT ((rcmMsgSize == sizeof (OmxRpc_msg)),
                                  "Invalid msgSize");

  pRpcMsg = (OmxRpc_msg *) (rcmMsg);
  rpcSkelHandle = pRpcMsg->common.pRpcRemoteHandle;
  Log_print1 (Diags_USER2, "OmxRpc_rcmIfSkelOmxApi:Entered..Cmd:%d",
              pRpcMsg->common.eRpcCmd);
  retVal =
    (*skel_api_tbl[pRpcMsg->common.eRpcCmd]) (rpcSkelHandle, pRpcMsg, &omxErr);
  OmxRpc_msgMarshallCommonResponse (&pRpcMsg->common, rpcSkelHandle,
                                    rpcSkelHandle->layer.skel.stubHandle,
                                    pRpcMsg->common.eRpcCmd,
                                    (OmxCore_OMX_ERRORTYPE) omxErr, retVal);
  Log_print1 (Diags_USER2, "OmxRpc_rcmIfSkelOmxApi:Exiting..Cmd:%d",
              pRpcMsg->common.eRpcCmd);

  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL2 (retVal);
  return retVal;
}

/**
 * @name   OmxRpc_skelConfigCompInfo
 * @brief  OmxRpc function for configuring Component RPC info
 * @param  pRpcHndl                 : RPC instance handle
 * @param  cfg                      : Pointer to Component RPCinfo structure
 * @return OmxRpc_errorNone         : SUCCESS
 */
OmxRpc_errorType OmxRpc_skelConfigCompInfo (OmxRpc_Handle pRpcHndl,
                                            OmxRpc_skelComponentConfigInfo *cfg)
{
  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();
  pRpcHndl->layer.skel.stubHandle = cfg->stubHandle;
  pRpcHndl->layer.skel.pComponentHandle = cfg->pComponentHandle;

  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (OmxRpc_errorNone);
  return OmxRpc_errorNone;
}

/**
 * @name   OmxRpc_skelGetCompRcmServerName
 * @brief  OmxRpc function for getting the components RcmServer name
 * @param  pRpcHndl                 : RPC instance handle
 * @param  cComponentRcmServerName  : Character array to store the RcmServer name
 * @param  nComponentRcmServerNameMaxLen : Max length of character array passed (arg2)
 * @return OmxRpc_errorNone         : SUCCESS
 */
OmxRpc_errorType
  OmxRpc_skelGetCompRcmServerName (OmxRpc_Handle pRpcHndl,
                                   OmxTypes_OMX_S8 *cComponentRcmServerName,
                                   OmxTypes_OMX_U32
                                     nComponentRcmServerNameMaxLen)
{
  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();
  DOMX_UTL_Strncpy ((OMX_STRING) cComponentRcmServerName,
                    (OMX_STRING) pRpcHndl->server.name,
                    nComponentRcmServerNameMaxLen);
  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (OmxRpc_errorNone);
  return OmxRpc_errorNone;
}

/**
 * @name   OmxRpc_rcmIfGetHandle
 * @brief  OmxRpc rcm handler function for GetHandle msg
 * @param  rcmMsgSize               : Rcm message size
 * @param  rcmMsg                   : Pointer to RcmMsg received
 * @return OmxRpc_errorNone         : SUCCESS
 */
Int32 OmxRpc_rcmIfGetHandle (UInt32 rcmMsgSize, UInt32 *rcmMsg)
{
  OmxRpc_Params omxRpcParams;
  Error_Block eb;
  OmxRpc_msg *pRpcMsg = NULL;
  OmxRpc_msgGetHandle *pGetHandleMsg;
  OmxRpc_Handle rpcSkelHandle = NULL;
  OmxRpc_errorType retVal;
  OMX_ERRORTYPE omxErr = OMX_ErrorNone;

  /* Must intialize error block before using it */
  Error_init (&eb);
  /* Ensure enum value assumption is valid */
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((OMXRPC_CMD_ENUM_VALUE_VALIDATE),
                                  "rpcCmd enum invalid");
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((rcmMsgSize == sizeof (OmxRpc_msg)),
                                  "rcmMsgSize incorrect");

  pRpcMsg = (OmxRpc_msg *) (rcmMsg);
  pGetHandleMsg = &(pRpcMsg->api.getHandle);

  OMXRPCSKEL_MEMLOG_USED_START ();
  Log_print1 (Diags_USER1, "OmxRpc_rcmIfGetHandle:ComponentName:%s",
              (IArg) pGetHandleMsg->componentName);
  OmxRpc_Params_init (&omxRpcParams);
  omxRpcParams.componentName = (xdc_String) pGetHandleMsg->componentName;
  omxRpcParams.rcmOmxComponentServerPriority =
    pGetHandleMsg->rcmOmxComponentServerPriority;
  omxRpcParams.remoteCoreId = (DomxTypes_coreType) pGetHandleMsg->stubCoreId;
  /* Create an OmxRpc Skel instance */
  omxRpcParams.layerId = OmxRpc_layerTypeSkel;
  OMXRPCSKEL_MEMLOG_USED_START ();
  rpcSkelHandle = OmxRpc_create (&omxRpcParams, &eb);
  OMXRPCSKEL_MEMLOG_USED_END (&
                              (rpcSkelHandle->
                               memoryUsed
                               [DOMXRPCSKEL_MEMUSAGEPHASE_RPCSKELCREATE][0]));
  if (Error_check (&eb))
  {
    Log_print1 (Diags_USER1, "OmxRpc Skel create failed.Staus = %d",
                Error_getId (&eb));
    OmxRpc_msgMarshallCommonResponse (&pRpcMsg->common,
                                      /* Skel Handle is Null since create *
                                         failed */
                                      NULL,
                                      pRpcMsg->api.getHandle.stubHandle,
                                      OmxRpc_cmdGetHandle,
                                      (OmxCore_OMX_ERRORTYPE)
                                      OMX_ErrorUndefined,
                                      /* TODO Cleanup Hack. Need to map *
                                         errorId to OmxRpc_errorType */
                                      OmxRpc_errorSkelCreateFailed);
    return OmxRpc_errorSkelCreateFailed;
  }
  rpcSkelHandle->layer.skel.stubHandle = pRpcMsg->api.getHandle.stubHandle;
  retVal = omxrpc_skel_gethandle (rpcSkelHandle, pRpcMsg, &omxErr);
  OmxRpc_msgMarshallCommonResponse (&pRpcMsg->common,
                                    rpcSkelHandle,
                                    rpcSkelHandle->layer.skel.stubHandle,
                                    OmxRpc_cmdGetHandle,
                                    (OmxCore_OMX_ERRORTYPE) omxErr, retVal);
  omxrpc_msg_marshall_gethandle_response (&pRpcMsg->api.getHandleResponse,
                                          (String) rpcSkelHandle->server.
                                          name,
                                          rpcSkelHandle->layer.skel.
                                          pComponentHandle, rpcSkelHandle);
  Log_print1 (Diags_USER1, "OmxRpc_rcmIfGetHandle:ComponentName:%s.Exiting..",
              (IArg) rpcSkelHandle->server.name);
  OMXRPCSKEL_MEMLOG_USED_END (&
                              (rpcSkelHandle->
                               memoryUsed
                               [DOMXRPCSKEL_MEMUSAGEPHASE_SKELGETHANDLE][0]));
  return retVal;
}

/**
 * @name   OmxRpc_rcmIfFreeHandle
 * @brief  OmxRpc rcm handler function for FreeHandle msg
 * @param  rcmMsgSize               : Rcm message size
 * @param  rcmMsg                   : Pointer to RcmMsg received
 * @return OmxRpc_errorNone         : SUCCESS
 */
Int32 OmxRpc_rcmIfFreeHandle (UInt32 rcmMsgSize, UInt32 *rcmMsg)
{
  OmxRpc_msg *pRpcMsg = NULL;
  OmxRpc_Handle rpcSkelHandle = NULL;
  OmxRpc_Handle rpcStubHandle;
  OmxRpc_errorType retVal;
  OMX_ERRORTYPE omxErr = OMX_ErrorNone;;
  OMX_U32 allocSize[DOMX_MEMUSAGE_MAX_HEAPS_PER_PHASE];

  DOMX_UTL_TRACE_FUNCTION_ASSERT ((rcmMsgSize == sizeof (OmxRpc_msg)),
                                  "rcmMsgSize incorrect");
  pRpcMsg = (OmxRpc_msg *) (rcmMsg);
  Log_print0 (Diags_USER1, "OmxRpc_rcmIfFreeHandle:Entered..");

  rpcSkelHandle = pRpcMsg->common.pRpcRemoteHandle;
  memcpy (allocSize,
          &(rpcSkelHandle->
            memoryUsed[DOMXRPCSKEL_MEMUSAGEPHASE_SKELGETHANDLE][0]),
          sizeof (rpcSkelHandle->
                  memoryUsed[DOMXRPCSKEL_MEMUSAGEPHASE_SKELGETHANDLE][0]) *
          DOMX_MEMUSAGE_MAX_HEAPS_PER_PHASE);
  OMXRPCSKEL_MEMLOG_FREE_START ();

  rpcStubHandle = rpcSkelHandle->layer.skel.stubHandle;
  /* TODO Invoke shutdown instead of directly deleting the OmxComponent from
     RCM server core thread */
  retVal = omxrpc_skel_freehandle (rpcSkelHandle, pRpcMsg, &omxErr);

  if (OmxRpc_errorNone == retVal)
  {
    OMXRPCSKEL_MEMLOG_FREE_START ();
    RcmClient_delete ((RcmClient_Handle *) (&rpcSkelHandle->client.handle));
    OMXRPCSKEL_MEMLOG_FREE_END (&
                                (rpcSkelHandle->
                                 memoryUsed
                                 [DOMXRPCSKEL_MEMUSAGEPHASE_RCMCLIENTCREATE]
                                 [0]),
                                DOMXRPCSKEL_MEMUSAGEPHASE_RCMCLIENTCREATE);
  }
  /* Delete the rpcSkel instance on free handle */
  OmxRpc_delete (&rpcSkelHandle);
  OmxRpc_msgMarshallCommonResponse (&pRpcMsg->common,
                                    rpcSkelHandle,
                                    rpcStubHandle,
                                    pRpcMsg->common.eRpcCmd,
                                    (OmxCore_OMX_ERRORTYPE) omxErr, retVal);
  OMXRPCSKEL_MEMLOG_FREE_END (allocSize,
                              DOMXRPCSKEL_MEMUSAGEPHASE_SKELGETHANDLE);
  Log_print0 (Diags_USER1, "OmxRpc_rcmIfFreeHandle:Exiting..");
  return retVal;
}

/**
 * @name   OmxRpc_rcmIfSkelGetHeapMemStats
 * @brief  OmxRpc rcm handler function for getting heap memory stats
 * @param  rcmMsgSize               : Rcm message size
 * @param  rcmMsg                   : Pointer to RcmMsg received
 * @return OmxRpc_errorNone         : SUCCESS
 */

Int32 OmxRpc_rcmIfSkelGetHeapMemStats (UInt32 rcmMsgSize, UInt32 *rcmMsg)
{
  OmxRpc_msg *pRpcMsg = NULL;

  DOMX_UTL_TRACE_FUNCTION_ASSERT ((rcmMsgSize == sizeof (OmxRpc_msg)),
                                  "rcmMsgSize incorrect");

  pRpcMsg = (OmxRpc_msg *) (rcmMsg);
  Log_print1 (Diags_USER1, "OmxRpc_rcmIfSkelGetHeapMemStats:Entered..Cmd:%d",
              pRpcMsg->common.eRpcCmd);

#ifdef _LOCAL_CORE_m3video_
  OmxUtils_getAllVideoM3HeapMemStats (&pRpcMsg->api.getHeapMemStatsResponse.
                                      totalFreeSize[0]);
#endif

#ifdef _LOCAL_CORE_m3vpss_
  OmxUtils_getAllVpssM3HeapMemStats (&pRpcMsg->api.getHeapMemStatsResponse.
                                     totalFreeSize[0]);
#endif

  Log_print1 (Diags_USER1, "OmxRpc_rcmIfSkelGetHeapMemStats:Exiting..Cmd:%d",
              pRpcMsg->common.eRpcCmd);

  return OMX_ErrorNone;
}

/* Nothing beyond this point */
