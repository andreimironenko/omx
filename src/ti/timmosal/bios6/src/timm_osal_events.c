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
*   @file  timm_osal_events.c
*   This file contains methods that provides the functionality 
*   for creating/using events.
*
*  @path \
*
*/
/* -------------------------------------------------------------------------- */
/* =========================================================================
 *!
 *! Revision History
 *! ===================================
 *! 11-Mar-2009 Aditya Monga: admonga@ti.com Cleanup activity
 *! 23-May-2008 Gaurav Aradhya: gaurav.a@ti.com Ported to DSP BIOS
 *! 07-May-2008 Rabbani Patan: rabbani@ti.com updated version 
 *! 0.1: Created the first draft version, ksrini@ti.com  
 * ========================================================================= */

/******************************************************************************
* Includes
******************************************************************************/
#include <stdio.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Event.h>
/*For using the global gate*/
#include <ti/sysbios/gates/GateMutexPri.h>

#include "timm_osal_types.h"
#include "timm_osal_error.h"
#include "timm_osal_memory.h"
#include "timm_osal_events.h"
#include "../inc/timm_osal_internal.h"

#define EVENTNAME_MAX 16

typedef struct TIMM_OSAL_EVENTS 
{
    Event_Handle event_group; 
    char name[EVENTNAME_MAX];
} TIMM_OSAL_EVENTS;

static TIMM_OSAL_U32 gEventGrpCount = 0;
static TIMM_OSAL_U32 gUniqueEventNameCount = 0;

/*Global OSAL gate*/
extern GateMutexPri_Handle gOSALgate;
/* ========================================================================== */
/**
* @fn TIMM_OSAL_EventCreate function
*
* @ see timm_osal_events.h file
*/
/* ========================================================================== */
TIMM_OSAL_ERRORTYPE TIMM_OSAL_EventCreate(TIMM_OSAL_PTR *pEvents)
{
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;
    TIMM_OSAL_EVENTS *pHandle = NULL;
    Event_Params params;
	IArg keyOSALgate;
    
    pHandle = (TIMM_OSAL_EVENTS *)TIMM_OSAL_Malloc(sizeof(TIMM_OSAL_EVENTS),
                               TIMM_OSAL_TRUE, 0, TIMMOSAL_MEM_SEGMENT_EXT);
    if (TIMM_OSAL_NULL == pHandle) 
    {
        bReturnStatus = TIMM_OSAL_ERR_ALLOC;
        goto EXIT;
    }
    TIMM_OSAL_Memset(pHandle, 0x0, sizeof(TIMM_OSAL_EVENTS));
    
	keyOSALgate = GateMutexPri_enter(gOSALgate);
    sprintf(pHandle->name, "EVT%lu", gUniqueEventNameCount++);
    /*To prevent array overflow*/
	if(gUniqueEventNameCount >= 9999)
		gUniqueEventNameCount = 0;
	GateMutexPri_leave(gOSALgate, keyOSALgate);

    /*Initialize with default params*/
    Event_Params_init (&params);
	params.instance->name = (xdc_String)(pHandle->name);

    pHandle->event_group = Event_create(&params, NULL);
       
    if(pHandle->event_group == NULL) 
    {
        bReturnStatus = (TIMM_OSAL_ERRORTYPE)TIMM_OSAL_ERR_CREATE
			                  (TIMM_OSAL_ERR, TIMM_OSAL_COMP_EVENTS, FALSE);
        goto EXIT;
    }

	/*Update counter*/
	gEventGrpCount++;
    *pEvents = (TIMM_OSAL_PTR *)pHandle;
    
EXIT:  
    if((TIMM_OSAL_ERR_NONE != bReturnStatus) && (TIMM_OSAL_NULL != pHandle)) 
    {
       TIMM_OSAL_Free(pHandle);
       pHandle = TIMM_OSAL_NULL;
   }
    return bReturnStatus;       
}



/* ========================================================================== */
/**
* @fn TIMM_OSAL_EventDelete function
*
* @ see timm_osal_events.h file
*/
/* ========================================================================== */
TIMM_OSAL_ERRORTYPE TIMM_OSAL_EventDelete(TIMM_OSAL_PTR pEvents)
{

    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;
    TIMM_OSAL_EVENTS *pHandle;

	if(pEvents == NULL)
	{
		bReturnStatus = TIMM_OSAL_ERR_PARAMETER;
		goto EXIT;
	}
	pHandle = (TIMM_OSAL_EVENTS *)pEvents;

    if (gEventGrpCount == 0) 
    {
        bReturnStatus = TIMM_OSAL_ERR_UNKNOWN;
        goto EXIT;
    }
    
    gEventGrpCount--;
    
    Event_delete(&(pHandle->event_group));
    
    TIMM_OSAL_Free(pEvents);
EXIT:    
    return bReturnStatus;       
}



/* ========================================================================== */
/**
* @fn TIMM_OSAL_EventSet function
*
* @ see timm_osal_events.h file
*/
/* ========================================================================== */
TIMM_OSAL_ERRORTYPE TIMM_OSAL_EventSet(TIMM_OSAL_PTR pEvents,
                                       TIMM_OSAL_U32 uEventFlags,
                                       TIMM_OSAL_EVENT_OPERATION eOperation)
{
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;
    
    TIMM_OSAL_EVENTS *pHandle = (TIMM_OSAL_EVENTS *)pEvents;

	if(pHandle == NULL || uEventFlags == 0)
	{
		bReturnStatus = TIMM_OSAL_ERR_PARAMETER;
		goto EXIT;
	}

    switch (eOperation) 
    {
	/*To AND with the event flag that is currently set - this means clear the
	current flag (via pend with OR mask equal to 0xFFFF) and then set the new
	flag*/
    case TIMM_OSAL_EVENT_AND:
		Event_pend(pHandle->event_group, 0, 0xFFFFFFFF, 0);
		break;
    case TIMM_OSAL_EVENT_OR:
        break;
    default:
        bReturnStatus = TIMM_OSAL_ERR_PARAMETER;
        goto EXIT;
    }
    Event_post(pHandle->event_group, uEventFlags);
    
EXIT:  
    return bReturnStatus;       
}



/* ========================================================================== */
/**
* @fn TIMM_OSAL_EventRetrieve function
*
* @ see timm_osal_events.h file
*/
/* ========================================================================== */
TIMM_OSAL_ERRORTYPE TIMM_OSAL_EventRetrieve(TIMM_OSAL_PTR pEvents,
                                            TIMM_OSAL_U32 uRequestedEvents,
                                            TIMM_OSAL_EVENT_OPERATION eOperation,
                                            TIMM_OSAL_U32 *pRetrievedEvents,    
                                            TIMM_OSAL_U32 uTimeOutMsec)
{
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;
    TIMM_OSAL_EVENTS *pHandle = (TIMM_OSAL_EVENTS *)pEvents;
	TIMM_OSAL_U32 andMask, orMask, timeOut;

	if(pEvents == NULL)
	{
		bReturnStatus = TIMM_OSAL_ERR_PARAMETER;
		goto EXIT;
	}
    
    *pRetrievedEvents = 0;
    if (TIMM_OSAL_SUSPEND == uTimeOutMsec) 
    {
        timeOut = BIOS_WAIT_FOREVER;
    }
    else
    {
        timeOut = _TIMM_OSAL_GetTicks(uTimeOutMsec);
    }

    switch (eOperation) 
    {
    case TIMM_OSAL_EVENT_AND:

        orMask = 0;
        andMask = uRequestedEvents;

        *pRetrievedEvents = Event_pend(pHandle->event_group, andMask, orMask, timeOut);
        if(*pRetrievedEvents == 0)
        {
            bReturnStatus = TIMM_OSAL_WAR_TIMEOUT;
            goto EXIT;
        }
        else
        {
            /*Posting the event again as it is not supposed to be consumed*/
            Event_post(pHandle->event_group, *pRetrievedEvents);
        }
      
    break;
      
    case TIMM_OSAL_EVENT_AND_CONSUME:

        orMask = 0;
        andMask = uRequestedEvents;

        *pRetrievedEvents = Event_pend(pHandle->event_group, andMask, orMask, timeOut);
        if(*pRetrievedEvents == 0)
        {
            bReturnStatus = TIMM_OSAL_WAR_TIMEOUT;
            goto EXIT;
        }

    break;
        
    case TIMM_OSAL_EVENT_OR:

        orMask = uRequestedEvents;
        andMask = 0;

        *pRetrievedEvents = Event_pend(pHandle->event_group, andMask, orMask, timeOut);
        if(*pRetrievedEvents == 0)
        {
            bReturnStatus = TIMM_OSAL_WAR_TIMEOUT;
            goto EXIT;
        }
        else
        {
            /*Posting the event again as it is not supposed to be consumed*/
            Event_post(pHandle->event_group, *pRetrievedEvents);
        }

    break;
       
    case TIMM_OSAL_EVENT_OR_CONSUME:

        orMask = uRequestedEvents;
        andMask = 0;

        *pRetrievedEvents = Event_pend(pHandle->event_group, andMask, orMask, timeOut);
        if(*pRetrievedEvents == 0)
        {
            bReturnStatus = TIMM_OSAL_WAR_TIMEOUT;
            goto EXIT;
        }

    break;
        
    default:
        bReturnStatus = TIMM_OSAL_ERR_PARAMETER;
        goto EXIT;
    }
    
EXIT:    
    return bReturnStatus;       
}

