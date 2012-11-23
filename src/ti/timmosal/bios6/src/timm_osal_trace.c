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
*   @file  timm_osal_trace.c
*   This file contains methods that provides the functionality 
*   for logging errors/warings/information/etc.
*
*  @path \
*
*/
/* -------------------------------------------------------------------------- */
/* =========================================================================
 *!
 *! Revision History
 *! ===================================
 *!
 * ========================================================================= */

/******************************************************************************
* Includes
******************************************************************************/

//#include "typedefs.h"
//#include "vsi.h"
#include <stdarg.h>

#include "timm_osal_types.h"
#include "timm_osal_trace.h"

#include "../../StmLibrary/StmLibrary.h"
#include "../../StmLibrary/STMHelper.h"


extern STMHandle *phandle;
extern int iChannel;
extern STMXport_callback pSTMCallBack;
extern TIMM_OSAL_U32 ulEnabledTraceGroups;

/******************************************************************************
* Function Prototypes
******************************************************************************/
#if 0
extern int int_vsi_o_ttrace (T_HANDLE Caller, 
                             ULONG TraceClass, 
                             const char * const format, 
                             va_list varpars);

/*--------data declarations -----------------------------------*/
T_HANDLE TIMM_OSAL_handle;
#endif

//TODO: Implement this function using a bit position count function
int getChannelNum(TIMM_OSAL_TRACEGRP eTraceGrp)
{
  return 0;
}

/* ========================================================================== */
/**
* @fn TIMM_OSAL_TraceFunction function 
*
*
*/
/* ========================================================================== */
void TIMM_OSAL_TraceFunction (char *pcFormat, ...)
{
    int chNum;
    va_list varArgs;

	va_start(varArgs, pcFormat);

    //trace only if system trace group is enabled
    if(ulEnabledTraceGroups & TIMM_OSAL_TRACEGRP_SYSTEM) {
        chNum= getChannelNum(TIMM_OSAL_TRACEGRP_SYSTEM);
        STMXport_printf(phandle, chNum, pSTMCallBack, "Depreciated API:\n");
        STMXport_printfV(phandle, chNum, pSTMCallBack, pcFormat, varArgs);
    }

	va_end(vaArgs);

    return;
}

void TIMM_OSAL_TracePrintf(TIMM_OSAL_TRACEGRP eTraceGrp, TIMM_OSAL_CHAR *pcFormat, ...)
{
    int chNum;
	va_list varArgs;

	va_start(varArgs, pcFormat);

    if(ulEnabledTraceGroups & eTraceGrp) {
        chNum= getChannelNum(eTraceGrp);
        STMXport_printfV(phandle, chNum, pSTMCallBack, pcFormat, varArgs);
    }

	va_end(vaArgs);
}

void TIMM_OSAL_TracePutBuf(TIMM_OSAL_TRACEGRP eTraceGrp, TIMM_OSAL_PTR* pDataBuf, TIMM_OSAL_S8 sElementSize, TIMM_OSAL_S32 lElementCount)
{
    int chNum;
    //export trace only if channel is enabled
    if(ulEnabledTraceGroups & eTraceGrp) {
        chNum= getChannelNum(eTraceGrp);
        STMXport_putBuf(phandle, chNum, pSTMCallBack, pDataBuf, (eSTMElementSize)sElementSize, lElementCount);
    }
}

void TIMM_OSAL_TraceFlush()
{
	STMXport_flush(phandle);
}

void TIMM_OSAL_SetTraceGrp(TIMM_OSAL_U32 ulTraceGroups)
{
	ulEnabledTraceGroups = ulTraceGroups;
    //let the user know that there was change in trace mask
    STMXport_printf(phandle, TIMM_OSAL_TRACEGRP_SYSTEM, pSTMCallBack, "New Trace group set: %lu",ulEnabledTraceGroups );
}

TIMM_OSAL_U32 TIMM_OSAL_GetTraceGrp()
{
	return ulEnabledTraceGroups;
}


