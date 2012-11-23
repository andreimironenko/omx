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

#include <string.h>
#include <stdio.h>
#include <OMX_Core.h>
#include <OMX_Types.h>

/*-------------------------program files -------------------------------------*/
#include "domx_util.h"

/**
 *******************************************************************************
 *  @file domx_util.c
 *  @brief This file contains all utility Functions for core, error handeling
 *         etc
 *
 *
 *  @rev 1.0
 *******************************************************************************
 */

OMX_STRING DOMX_UTL_OmxErrorToString (OMX_ERRORTYPE eError)
{
  OMX_STRING errorString;

  /*--------------------------------------------------------------------------*/
  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
  /* For each of the error code, return appropriate error string */
  /* */
  /*--------------------------------------------------------------------------*/

  switch (eError)
  {
    case OMX_ErrorNone:
      errorString = "ErrorNone";
      break;
    case OMX_ErrorInsufficientResources:
      errorString = "ErrorInsufficientResources";
      break;
    case OMX_ErrorUndefined:
      errorString = "ErrorUndefined";
      break;
    case OMX_ErrorInvalidComponentName:
      errorString = "ErrorInvalidComponentName";
      break;
    case OMX_ErrorComponentNotFound:
      errorString = "ErrorComponentNotFound";
      break;
    case OMX_ErrorInvalidComponent:
      errorString = "ErrorInvalidComponent";
      break;
    case OMX_ErrorBadParameter:
      errorString = "ErrorBadParameter";
      break;
    case OMX_ErrorNotImplemented:
      errorString = "ErrorNotImplemented";
      break;
    case OMX_ErrorUnderflow:
      errorString = "ErrorUnderflow";
      break;
    case OMX_ErrorOverflow:
      errorString = "ErrorOverflow";
      break;
    case OMX_ErrorHardware:
      errorString = "ErrorHardware";
      break;
    case OMX_ErrorInvalidState:
      errorString = "ErrorInvalidState";
      break;
    case OMX_ErrorStreamCorrupt:
      errorString = "ErrorStreamCorrupt";
      break;
    case OMX_ErrorPortsNotCompatible:
      errorString = "ErrorPortsNotCompatible";
      break;
    case OMX_ErrorResourcesLost:
      errorString = "ErrorResourcesLost";
      break;
    case OMX_ErrorNoMore:
      errorString = "ErrorNoMore";
      break;
    case OMX_ErrorVersionMismatch:
      errorString = "ErrorVersionMismatch";
      break;
    case OMX_ErrorNotReady:
      errorString = "ErrorNotReady";
      break;
    case OMX_ErrorTimeout:
      errorString = "ErrorTimeout";
      break;
    case OMX_ErrorSameState:
      errorString = "ErrorSameState";
      break;
    case OMX_ErrorResourcesPreempted:
      errorString = "ErrorResourcesPreempted";
      break;
    case OMX_ErrorPortUnresponsiveDuringAllocation:
      errorString = "ErrorPortUnresponsiveDuringAllocation";
      break;
    case OMX_ErrorPortUnresponsiveDuringDeallocation:
      errorString = "ErrorPortUnresponsiveDuringDeallocation";
      break;
    case OMX_ErrorPortUnresponsiveDuringStop:
      errorString = "ErrorPortUnresponsiveDuringStop";
      break;
    case OMX_ErrorIncorrectStateTransition:
      errorString = "ErrorIncorrectStateTransition";
      break;
    case OMX_ErrorIncorrectStateOperation:
      errorString = "ErrorIncorrectStateOperation";
      break;
    case OMX_ErrorUnsupportedSetting:
      errorString = "ErrorUnsupportedSetting";
      break;
    case OMX_ErrorUnsupportedIndex:
      errorString = "ErrorUnsupportedIndex";
      break;
    case OMX_ErrorBadPortIndex:
      errorString = "ErrorBadPortIndex";
      break;
    case OMX_ErrorPortUnpopulated:
      errorString = "ErrorPortUnpopulated";
      break;
    case OMX_ErrorComponentSuspended:
      errorString = "ErrorComponentSuspended";
      break;
    case OMX_ErrorDynamicResourcesUnavailable:
      errorString = "ErrorDynamicResourcesUnavailable";
      break;
    case OMX_ErrorMbErrorsInFrame:
      errorString = "ErrorMbErrorsInFrame";
      break;
    case OMX_ErrorFormatNotDetected:
      errorString = "ErrorFormatNotDetected";
      break;
    case OMX_ErrorContentPipeOpenFailed:
      errorString = "ErrorContentPipeOpenFailed";
      break;
    case OMX_ErrorContentPipeCreationFailed:
      errorString = "ErrorContentPipeCreationFailed";
      break;
    case OMX_ErrorSeperateTablesUsed:
      errorString = "ErrorSeperateTablesUsed";
      break;
    case OMX_ErrorTunnelingUnsupported:
      errorString = "ErrorTunnelingUnsupported";
      break;
    default:
      errorString = "<unknown>";
      break;
  }                             /* end of Switch Block */

  return errorString;
}                               /* OMX_TEST_ErrorToString */

/* Nothing beyond this point */
