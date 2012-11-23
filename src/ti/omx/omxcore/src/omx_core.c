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

/*
*   @file  omx_core.c
*   This file contains methods that provides the functionality for the OpenMAX Core
*   responsible for environmental setup, components tunneling and communication.
*
*  @path \ti\omx\omxcore\src\
*
*  @rev 1.0
*/
/* -------------------------------------------------------------------------- */
/* =========================================================================
 *! Revision History
 *! ========================================================================
 *!17-April-2008 Rabbani Patan rabbani@ti.com: Initial Version
 * ========================================================================= */

/****************************************************************
 * INCLUDE FILES
 ****************************************************************/
/* ----- system and platform files ----------------------------*/
#include <stdio.h>
#include <string.h>




#include <stdlib.h>


#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/knl/Thread.h>
#include <ti/ipc/MultiProc.h>
/*-------program files ----------------------------------------*/
#include <OMX_Core.h>
#include "OMX_ComponentTable.h"
#include "timm_osal_error.h"
#include "timm_osal_memory.h"
#include "domx_util.h"
#include <interfaces_openmaxv11_internal.h>
#include <OMX_Component.h>
#include <ti/omx/domx/interfaces/domx_interfaces_internal.h>
#include <ti/omx/domx/DomxCore__epilogue.h>
#include <ti/omx/domx/domx_internal.h>
#include <ti/omx/memcfg/memcfg.h>

/****************************************************************
 * EXTERNAL REFERENCES NOTE : only use if not found in header file
 ****************************************************************/
/*--------data declarations -----------------------------------*/


/*--------function prototypes ---------------------------------*/

/****************************************************************
 * PUBLIC DECLARATIONS Defined here, used elsewhere
 ****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/

/****************************************************************
 * PRIVATE DECLARATIONS Defined here, used only here
 ****************************************************************/
/*--------macros ----------------------------------------------*/
/*--------data declarations -----------------------------------*/
#ifdef TI_MOT_P2K_MM
  int timeOut = 40;
#endif

/* Note: This OpenMAX core is OMX 1.1.0.0 compliant(In BCD format) */
static const OMX_VERSIONTYPE OMXCoreVersion = { {0x01, 0x01, 0x02, 0x00} };

typedef struct OMX_Core_InternalObj
{
  OMX_BOOL isInitialized;
} OMX_Core_InternalObj;

OMX_Core_InternalObj OMX_Core_Context =
{
  OMX_FALSE         /* isInitialized */
};

/*--------function prototypes ---------------------------------*/

/* ==========================================================================*/
/**
 * @fn OMX_Init() --  This method is used to initialize the OMX core.
 *
 *  @ see OMX_Core.h
 */
/* ==========================================================================*/
OMX_ERRORTYPE OMX_Init (void)
{
  OMX_ERRORTYPE tError = OMX_ErrorNone;



  Log_print1 (Diags_ENTRY, "Entering %s: (void)\n", (xdc_IArg) __FUNCTION__);

  if (OMX_FALSE == OMX_Core_Context.isInitialized)
  {
    tError = DomxInit ();
    if (OMX_ErrorNone == tError)
    {
      OMX_Core_Context.isInitialized = OMX_TRUE;
    }
    if (MultiProc_self () == DomxTypes_coreCortexA8)
    {
      MEMCFG_ModuleInit();
      Thread_start (NULL);
    }
  }
  Log_print2 (Diags_EXIT, "Leaving %s: retVal OMX_ERRORTYPE: %d\n",
              (xdc_IArg) __FUNCTION__, tError);

  return tError;
}

/* ==========================================================================*/
/**
 * @fn OMX_Deinit() -- This method is used to deinitalize the core.
 *
 *  @ see OMX_Core.h
 */
/* ==========================================================================*/
OMX_ERRORTYPE OMX_Deinit (void)
{
  OMX_ERRORTYPE tError = OMX_ErrorNone;


  if (OMX_TRUE == OMX_Core_Context.isInitialized)
  {
    if (MultiProc_self () == DomxTypes_coreCortexA8)
    {
      MEMCFG_ModuleDeInit();
    }
    tError = DomxDeInit ();
    OMX_Core_Context.isInitialized = OMX_FALSE;
  }

  return tError;
}

/* ==========================================================================*/
/**
 * @fn OMX_GetHandle() -- This method will locate the component specified by the
 *                        component name given, loads that component into memory
 *                        and validate it.If the component is valid, this method
 *                        will invoke the component's method to fill the comp 
 *                        handle and setup the callbacks. Each time this method
 *                        returns successfully, a new component instance is
 *                        created.
 *
 *  @ param [out]  pHandle        : pointer to the OMX_HANDLETYPE to be
 *                                  filled in by this method.
 *
 *  @ param [in]   cComponentName : pointer to Null terminated string with
 *                                  component name.
 *
 *  @ param [in]   pAppData       : pointer to an IL client-defined value that
 *                                  will be returned during callbacks, so that
 *                                  IL client can identify the source of the
 *                                  callback.
 *
 *  @ param [in]  pCallBacks      : pointer to an OMX_CALLBACKTYPE structure
 *                                  containing the callbacks that the comp
 *                                  will use for this IL client.
 *
 *  @ see OMX_Core.h
 */
/* ==========================================================================*/
OMX_ERRORTYPE OMX_GetHandle (OMX_HANDLETYPE *pHandle,
                             OMX_STRING cComponentName,
                             OMX_PTR pAppData, OMX_CALLBACKTYPE *pCallBacks)
{
  OMX_ERRORTYPE retVal = OMX_ErrorNone;
  OMX_COMPONENTTYPE *componentType = NULL;
  int i = 0;
  int bFound = 0;

  Log_print5 (Diags_ENTRY, "Entered: %s (0x%x, %s, 0x%x, 0x%x)",
              (xdc_IArg) __FUNCTION__, (xdc_IArg) pHandle,
              (xdc_IArg) cComponentName, (xdc_IArg) pAppData,
              (xdc_IArg) pCallBacks);

  /* As per OMX standard, OMX_init has to be called before get_handle */
  if (OMX_FALSE == OMX_Core_Context.isInitialized)
  {
    retVal = OMX_ErrorNotReady;
    goto EXIT;
  }

  /* Check for valid component handle, Application has to allocate and * fill
     the size of the structure and version fields; * Also validate other input
     parameters. */
  if ((NULL == pHandle) || (NULL == cComponentName) || (NULL == pCallBacks))
  {
    retVal = OMX_ErrorBadParameter;
    goto EXIT;
  }

  /* Verify that the name is not too long and could cause a crash.  Notice *
     that the comparison is a greater than or equals.  This is to make * sure
     that there is room for the terminating NULL at the end of the name */
  if (strlen (cComponentName) >= OMX_MAX_STRINGNAME_SIZE)
  {
    retVal = OMX_ErrorInvalidComponentName;
    goto EXIT;
  }

  *pHandle = NULL;

  /* Search through the component list for the matching component name * and
     execute the Init function of the component */
  i = 0;
  while (NULL != OMXComponentList[i].cComponentName)
  {
    Log_print3 (Diags_USER1, "Component %s In table %s idx %d",
                (xdc_IArg) cComponentName, (xdc_IArg) OMXComponentList[i].cComponentName, i);
    if (strcmp (cComponentName, OMXComponentList[i].cComponentName) == 0)
    {
      /* Component found */
      bFound = OMX_TRUE;
      break;
    }
    i++;
  }

  if (bFound)
  {
    Log_print2 (Diags_USER1, "Component %s found idx: %d\n",
                (xdc_IArg) cComponentName, i);
    /* Allocate memory for the component handle which is of the type *
       OMX_COMPONENTTYPE and fill in the size and version * field of the
       structure */

    /* if no component init function is provided, return error */
    if (NULL == OMXComponentList[i].pComponentInit)
    {
      retVal = OMX_ErrorInvalidComponent;
      goto EXIT;
    }

    *pHandle = (OMX_HANDLETYPE) TIMM_OSAL_Malloc (sizeof (OMX_COMPONENTTYPE),
                                                  TIMM_OSAL_TRUE, 0,
                                                  TIMMOSAL_MEM_SEGMENT_EXT);
    if (NULL == *pHandle)
    {
      retVal = OMX_ErrorInsufficientResources;
      goto EXIT;
    }
    TIMM_OSAL_Memset (*pHandle, 0x0, sizeof (OMX_COMPONENTTYPE));

    Log_print3 (Diags_USER1, "In %s, component %s, omxhandle 0x%x",
                (xdc_IArg) __FUNCTION__, (xdc_IArg) cComponentName, 
                (xdc_IArg) *pHandle);

    componentType = (OMX_COMPONENTTYPE *) * pHandle;
    componentType->nSize = sizeof (OMX_COMPONENTTYPE);

    /* OMX version is 1.1.2.0 */
    componentType->nVersion.nVersion = OMXCoreVersion.nVersion;

    /* Call the Init function of the component using Init function * pointer
       updated in the component list table */
    retVal = OMXComponentList[i].pComponentInit (*pHandle);
    if (OMX_ErrorNone != retVal)
    {
      Log_print1 (Diags_USER1, "Result of pComponentInit: %s\n",
                  (xdc_IArg) DOMX_UTL_OmxErrorToString (retVal));
      goto EXIT;
    }

    /* Setting the callback function pointers */
    retVal = (componentType->SetCallbacks) (*pHandle, pCallBacks, pAppData);
    Log_print1 (Diags_USER1, "Result of pComponentInit: %s\n",
                (xdc_IArg) DOMX_UTL_OmxErrorToString (retVal));
    if (OMX_ErrorNone != retVal)
    {
      /* Since the SetCallbacks() failed, need to restore to the original *
         state, hence call ComponentDeInit() here */
      componentType->ComponentDeInit (*pHandle);
      goto EXIT;
    }

  }
  else
  {
    /* Component was not found in the component table */
    retVal = OMX_ErrorComponentNotFound;
  }

  if (OMX_ErrorNone == retVal)
  {
    Log_print1 (Diags_USER1, "Registering the component %s with TunnelMgr", 
                (xdc_IArg)cComponentName);
    DomxTunnelMgr_registerHandle (*pHandle, cComponentName);
  }
EXIT:
  /* Perform cleanup on error */
  if ((OMX_ErrorNone != retVal) && (NULL != pHandle) && (NULL != *pHandle))
  {
    TIMM_OSAL_Free (*pHandle);
    *pHandle = NULL;
  }

  Log_print3 (Diags_EXIT, "Leaving %s: retVal OMX_ERRORTYPE: 0x%x : %s\n",
              (xdc_IArg) __FUNCTION__, retVal,
              (xdc_IArg) DOMX_UTL_OmxErrorToString (retVal));
  return retVal;
}

static void _OMX_PrintMemUsage (void);

/* ==========================================================================*/
/**
 * @fn OMX_FreeHandle()--  This method will free a handle allocated by the
 *                                               OMX_GetHandle method.
 *
 *   param [in]    hComponent   :
 *
 *  @ see OMX_Core.h
 */
/* ==========================================================================*/
OMX_ERRORTYPE OMX_FreeHandle (OMX_HANDLETYPE hComponent)
{
  OMX_ERRORTYPE retVal = OMX_ErrorNone;
  OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *) hComponent;
  OMX_STATETYPE componentState;

  Log_print2 (Diags_ENTRY, "Entered: %s (0x%x )",
              (xdc_IArg) __FUNCTION__, (xdc_IArg) hComponent);

  /* Check for valid input handle */
  if (NULL == hComponent)
  {
    retVal = OMX_ErrorBadParameter;
    goto EXIT;
  }

  /* Get the present state of the component */
  retVal = pHandle->GetState (hComponent, &componentState);
  if (OMX_ErrorNone != retVal)
  {
    goto EXIT;
  }
  /* as per openmax spec, free handle can be called only in loaded or invalid
     state */
  if ((componentState == OMX_StateInvalid)
      || (componentState == OMX_StateLoaded))
  {
    DomxTunnelMgr_unRegisterHandle (hComponent);
    /* Call the ComponentDeInit function */
    retVal = pHandle->ComponentDeInit (hComponent);
    if (OMX_ErrorNone != retVal)
    {
      goto EXIT;
    }
    /* Free the memory for the component handle */
    TIMM_OSAL_Free (hComponent);
    _OMX_PrintMemUsage ();
  }
  else
  {
    retVal = OMX_ErrorIncorrectStateOperation;
  }

EXIT:
  Log_print3 (Diags_EXIT, "Leaving %s: retVal OMX_ERRORTYPE: 0x%x : %s\n",
              (xdc_IArg) __FUNCTION__, retVal,
              (xdc_IArg) DOMX_UTL_OmxErrorToString (retVal));
  return retVal;
}

/* ==========================================================================*/
/**
 * @fn OMX_SetupTunnel()--  This method setup tunneled communciation between an
 *     output port and input port.This method calls the component's
 *     ComponentTunnelRequest method to setup the tunnel. This method
 *     first issues a call to ComponentTunnelRequest on the comp with
 *     output port, if the call is sucessfull a second call to CompTunnelRequst
 *     on the comp with input port is made. shoudl either the call to
 *     CompTunnelRequest fail, the method will set up both the
 *     o/p and i/p for non-tunneled communication.
 *
 *   @ param [in]    hOutput     : Handle of the comp containing the o/p port
 *                                 used in tunnel
 *
 *   @ param [in]   nPortOutput  : indicates the o/p port of comp specified by
 *                                 hOutput.
 *
 *   @ param [in]   hInput       : Handle of the comp containing the I/p port
 *                                 used in tunnel
 *
 *   @ param [in]   nPortInput   : indicates the I/p port of comp specified by
 *                                 hInput.
 *
 *  @ see OMX_Core.h
 */
/* ==========================================================================*/
OMX_ERRORTYPE OMX_SetupTunnel (OMX_IN OMX_HANDLETYPE hOutput, 
                               OMX_IN OMX_U32 nPortOutput,
                               OMX_IN OMX_HANDLETYPE hInput,
                               OMX_IN OMX_U32 nPortInput)
{
  OMX_ERRORTYPE retVal = OMX_ErrorNone;
  OMX_COMPONENTTYPE *pInComp, *pOutComp;
  OMX_TUNNELSETUPTYPE tunnelSetup;
  OMX_HANDLETYPE hOutputPeerMapped, hInputPeerMapped;

  Log_print5 (Diags_ENTRY, "Entered: %s (0x%x, %d, 0x%x, %d)",
              (xdc_IArg) __FUNCTION__, (xdc_IArg) hOutput, nPortOutput,
              (xdc_IArg) hInput, nPortInput);

  /* Check for input and output handles */
  if (hOutput == NULL && hInput == NULL)
  {
    retVal = OMX_ErrorBadParameter;
    goto EXIT;
  }

  DomxTunnelMgr_mapTunnelComponentPeerHandles (hOutput, hInput,
                                               &hOutputPeerMapped,
                                               &hInputPeerMapped);
  pInComp = (OMX_COMPONENTTYPE *) hInput;
  pOutComp = (OMX_COMPONENTTYPE *) hOutput;

  /* setting default values before tunneling */
  tunnelSetup.nTunnelFlags = 0;
  tunnelSetup.eSupplier = OMX_BufferSupplyUnspecified;
  if (hOutput)
  {

    /* Calling ComponentTunelRequest() on the component with the output port */
    retVal = pOutComp->ComponentTunnelRequest (hOutput,
                                               nPortOutput, hOutputPeerMapped,
                                               nPortInput, &tunnelSetup);
  }

  if (retVal == OMX_ErrorNone && hInput)
  {
    /* Calling ComponentTunelRequest() on the component with the Input port */
    retVal = pInComp->ComponentTunnelRequest (hInput,
                                              nPortInput, hInputPeerMapped,
                                              nPortOutput, &tunnelSetup);
    if (retVal != OMX_ErrorNone)
    {
      /* cancelling the tunnel request on output port since the input port
         failed */
      pOutComp->ComponentTunnelRequest (hOutput, nPortOutput, NULL, 0, NULL);

    }
  }

EXIT:
  Log_print2 (Diags_EXIT, "Leaving %s: retVal OMX_ERRORTYPE: %d\n",
              (xdc_IArg) __FUNCTION__, retVal);
  return retVal;
}

/* ==========================================================================*/
/**
 * @fn OMX_ComponentNameEnum()-- This method enumerates through all the names of
 *                               the recognized components in the system to
 *                               detect all the components in the system-runtime
 *
 *   @ param [out] cComponentName    : pointer to an Null terminated string
 *                                     with the componentname.
 *
 *   @ param [in]  nNameLength       : number of characters in the
 *                                     cComponentName string
 *
 *   @ param [in]  nIndex            : number containing the enumeration
 *                                     index for the comp.
 *
 * @see  OMX_Core.h
 */
/* ==========================================================================*/
OMX_ERRORTYPE OMX_ComponentNameEnum (OMX_OUT OMX_STRING cComponentName,
                                     OMX_IN OMX_U32 nNameLength,
                                     OMX_IN OMX_U32 nIndex)
{
  OMX_ERRORTYPE retVal = OMX_ErrorNone;
  OMX_U32 nCompIdx = 0;
  OMX_BOOL bFound = OMX_FALSE;

  if (nNameLength == 0)
  {
    retVal = OMX_ErrorInvalidComponentName;
    goto EXIT;
  }

  if (cComponentName == NULL)
  {
    retVal = OMX_ErrorBadParameter;
    goto EXIT;
  }

  /* Enumerate through all the names of the component to detect * all the
     components in the system run-time . */
  while (NULL != OMXComponentList[nCompIdx].cComponentName)
  {
    if (nCompIdx == nIndex)
    {
      strncpy (cComponentName,
               OMXComponentList[nCompIdx].cComponentName, nNameLength);
      bFound = OMX_TRUE;
      break;
    }
    nCompIdx++;
  }

  /* Component has not found and return OMX_ErrorNoMore */
  if (!bFound)
  {
    cComponentName = NULL;
    retVal = OMX_ErrorNoMore;
  }

EXIT:
  return retVal;
}

/* ==========================================================================*/
/**
 * @fn OMX_GetRolesOfComponent()-- This method enables the IL client to query
 *                                 all the fulfilled by a component.
 *
 *   @ param [in]      compName  : Name of the comp being queried
 *
 *    @ param [inout]  pNumRoles : This is used as both input and ouput, on
 *                                 input it bounds size of the input strucutre,
 *                                 on ouput it specifies how many roles were
 *                                 retrieved.
 *
 *    @ param [out]    roles     : list of the names of all standard components
 *                                 implemented on the specified physical
 *                                 component name. If this pointer is NULL this
 *                                 function populates the pNumRoles filed with
 *                                 the number of roles the component supports
 *                                 and ignore the roles field.
 *
 * @see  OMX_Core.h
 */
/* ==========================================================================*/
OMX_ERRORTYPE OMX_GetRolesOfComponent (OMX_IN OMX_STRING compName,
                                       OMX_INOUT OMX_U32 *pNumRoles,
                                       OMX_OUT OMX_U8 **roles)
{
  OMX_BOOL bFound = OMX_FALSE;
  OMX_U32 nCompIndx, nRoleIndx;
  OMX_U32 nMaxRoles;
  OMX_ERRORTYPE retVal = OMX_ErrorNone;

  /* Check for an Valid component name */
  if (compName == NULL)
  {
    retVal = OMX_ErrorBadParameter;
    goto EXIT;
  }

  /* Verify that the compName is not more than * OMX_MAX_STRINGNAME_SIZE */
  if (strlen (compName) >= OMX_MAX_STRINGNAME_SIZE)
  {
    retVal = OMX_ErrorInvalidComponentName;
    goto EXIT;
  }

  nMaxRoles = *pNumRoles;

  nCompIndx = 0;

  /* Search through the component list for the matching component name * and
     query the number of roles and list of roles supported */
  while (NULL != OMXComponentList[nCompIndx].cComponentName)
  {
    if (strcmp (compName, OMXComponentList[nCompIndx].cComponentName) == 0)
    {

      /* Component Found over here, so identify the num of roles * that a
         component supports */
      bFound = OMX_TRUE;
      *pNumRoles = OMXComponentList[nCompIndx].nNumRoles;

      /* if "roles" ptr is NULL, just populate the pNumRoles field with the *
         number of roles the component supports and ignore the roles field , *
         to allow the client to properly size the roles array on a subsequent
         call */
      if (NULL == roles)
        goto EXIT;

      /* copy the roles that a component supports */
      for (nRoleIndx = 0;
           nRoleIndx < OMXComponentList[nCompIndx].nNumRoles; nRoleIndx++)
      {
        if (nRoleIndx < nMaxRoles)
        {
          strcpy ((char *) roles[nRoleIndx],
                  (const char *) OMXComponentList[nCompIndx].
                  roleArray[nRoleIndx]);
        }
      }
      break;
    }
    nCompIndx++;
  }

  if (bFound == OMX_FALSE)
  {
    /* Component was not found in the component table */
    *pNumRoles = 0;
    retVal = OMX_ErrorComponentNotFound;
  }

EXIT:
  return retVal;
}

/* ==========================================================================*/
/*
 * @fn OMX_GetRolesOfComponent()-   This method enables the IL client to query
 *         the names  of all installed components that supports a given role.
 *
 *   @ param [in]    role      : Name of the Specified role.
 *
 *   @ param [inout] pNumComps : This is use both as input and output, on input
 *                               it bounds the size of input struct. on ouptu it
 *                               specifies how many names were retrieved.
 *
 *   @ param [inout] compNames : This is a list of the names of all physical
*                                components that implement the specified
*                                standard component name. If this pointer is
*                                NULL thisfunction populates the pNumComps field
*                                with the number of components that support the
*                                given role and ignores the compNames field.
*                                This allows the client to properly size the
*                                compNames field on a subsequent call.
 *
 * @see  OMX_Core.h
 */
/* ==========================================================================*/
OMX_ERRORTYPE OMX_GetComponentsOfRole (OMX_IN OMX_STRING role,
                                       OMX_INOUT OMX_U32 *pNumComps,
                                       OMX_INOUT OMX_U8 **compNames)
{
  OMX_ERRORTYPE retVal = OMX_ErrorNone;
  OMX_U32 nNumComps = 0;
  OMX_U32 maxNumComp;
  OMX_U32 nCompIndx = 0, nRoleIndx;

  /* Check for the valid role name */
  if (NULL == role)
  {
    retVal = OMX_ErrorBadParameter;
    goto EXIT;
  }

  maxNumComp = *pNumComps;

  /* Search through the component list and query the names of * components that 
     supports the given role */
  while (NULL != OMXComponentList[nCompIndx].cComponentName)
  {
    for (nRoleIndx = 0;
         nRoleIndx < OMXComponentList[nCompIndx].nNumRoles; nRoleIndx++)
    {
      if (strcmp
          ((char *) OMXComponentList[nCompIndx].roleArray[nRoleIndx],
           role) == 0)
      {

        /* given role found over here, now identify the number of * components
           that support a given role */

        /* If "compNames" ptr is NULL , just populate the number of comp's *
           that supports the given role and ignore the compNames field, this
           allows the * client to properly size the compNames field on a
           subsequent call */
        if (compNames != NULL)
        {
          if (nNumComps < maxNumComp)
          {
            strcpy ((char *) compNames[nNumComps],
                    OMXComponentList[nCompIndx].cComponentName);
          }
        }
        nNumComps++;
      }
    }
    nCompIndx++;
  }

  /* Assigning the num of comp's supporting the given role */
  *pNumComps = nNumComps;

EXIT:
  return retVal;
}

#ifdef __ti__
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/System.h>

/* ==========================================================================*/
/**
 * @fn _OMX_PrintMemUsage()-- Internal function to print current free memory in
 *                            the default heap
 */
/* ==========================================================================*/
static void _OMX_PrintMemUsage (void)
{
  Memory_Stats memstats;

  Memory_getStats (NULL, &(memstats));
  System_printf ("\nFreeMemory =%d\n", memstats.totalFreeSize);
}
#else
static void _OMX_PrintMemUsage (void)
{
  /* Do nothing for now */
}
#endif

/* Nothing beyond this point */
