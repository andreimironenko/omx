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
*   @file  timm_osal_Mutexs.c
*   This file contains methods that provides the functionality 
*
*  @path \
*
*/
/* -------------------------------------------------------------------------- */
/* =========================================================================
 *!
 *! Revision History
 *! ===================================
 *! 30-June-2009 Aditya Monga: admonga@ti.com KW defects
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
#include <ti/sysbios/gates/GateMutexPri.h> 

#include "timm_osal_types.h"
#include "timm_osal_error.h"
#include "timm_osal_memory.h"
#include "timm_osal_trace.h"

#define MUTEXNAME_MAX 16


typedef struct TIMM_OSAL_MUTEX 
{
	/* Using a GateMuexPri as it implements priority inheritance to 
	   protect against priority inversion */
    GateMutexPri_Handle  gate;
	IArg key;
    char name[MUTEXNAME_MAX];
} TIMM_OSAL_MUTEX;

static TIMM_OSAL_U32    gMtxCnt = 0;
static TIMM_OSAL_U32    gUniqueMtxNameCnt = 0;

/*Global OSAL gate*/
extern GateMutexPri_Handle gOSALgate;
/* ========================================================================== */
/**
* @fn TIMM_OSAL_MutexCreate function
*
* @ see timm_osal_mutex.h file
*/
/* ========================================================================== */
TIMM_OSAL_ERRORTYPE TIMM_OSAL_MutexCreate(TIMM_OSAL_PTR *pMutex)
{
	TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;
    TIMM_OSAL_MUTEX *pHandle = TIMM_OSAL_NULL;
    GateMutexPri_Params params;
	IArg keyOSALgate;

    *pMutex = TIMM_OSAL_NULL;

    pHandle = (TIMM_OSAL_MUTEX *)TIMM_OSAL_Malloc(sizeof(TIMM_OSAL_MUTEX),
                                  TIMM_OSAL_TRUE, 0, TIMMOSAL_MEM_SEGMENT_EXT);

    if(TIMM_OSAL_NULL == pHandle)
    {
        bReturnStatus = TIMM_OSAL_ERR_ALLOC;
		goto EXIT;
    }

    /* Generate name of the mutex */
	keyOSALgate = GateMutexPri_enter(gOSALgate);
    sprintf(pHandle->name, "MTX%lu", gUniqueMtxNameCnt++);
	/*To prevent array overflow*/
	if(gUniqueMtxNameCnt >= 9999)
		gUniqueMtxNameCnt = 0;
	GateMutexPri_leave(gOSALgate, keyOSALgate);

	pHandle->key = NULL;
    GateMutexPri_Params_init( &params ); 
    params.instance->name = (xdc_String)(pHandle->name);

	/*Creating a gate*/
	pHandle->gate = GateMutexPri_create( &params, NULL ); 

    if(pHandle->gate == NULL)
    {
        TIMM_OSAL_Free(pHandle);
        bReturnStatus = TIMM_OSAL_ERR_UNKNOWN;
		goto EXIT;
    }

    /* Update mutex counter */
    gMtxCnt++;

    *pMutex = (TIMM_OSAL_PTR)pHandle;
EXIT:
    return bReturnStatus;
}



/* ========================================================================== */
/**
* @fn TIMM_OSAL_MutexDelete function
*
* @ see timm_osal_mutex.h file
*/
/* ========================================================================== */
TIMM_OSAL_ERRORTYPE TIMM_OSAL_MutexDelete(TIMM_OSAL_PTR pMutex)
{
    TIMM_OSAL_MUTEX *pHandle = (TIMM_OSAL_MUTEX *)pMutex;
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;

	if(pHandle == NULL)
	{
		bReturnStatus = TIMM_OSAL_ERR_PARAMETER;
		goto EXIT;
	}

    /* Delete the mutex.  */
	GateMutexPri_delete(&(pHandle->gate));

    /* Update mutex counter */
    gMtxCnt--;

    TIMM_OSAL_Free(pMutex);
EXIT:
    return bReturnStatus;
}



/* ========================================================================== */
/**
* @fn TIMM_OSAL_MutexObtain function
*
* @ see timm_osal_mutex.h file
*/
/* ========================================================================== */

TIMM_OSAL_ERRORTYPE TIMM_OSAL_MutexObtain(TIMM_OSAL_PTR pMutex, 
										  TIMM_OSAL_U32 uTimeOut)
{
    TIMM_OSAL_MUTEX *pHandle = (TIMM_OSAL_MUTEX *)pMutex;
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;

	if(pHandle == NULL)
	{
		bReturnStatus = TIMM_OSAL_ERR_PARAMETER;
		goto EXIT;
	}

	if (TIMM_OSAL_SUSPEND != uTimeOut)
	{
        /*Gates do not support timeouts*/
		TIMM_OSAL_Warning(" Timeouts not supported for mutex obtain. Task will wait infinitely to obtain the mutex\n");
	}

    pHandle->key = GateMutexPri_enter(pHandle->gate);     
    
EXIT:
    return bReturnStatus;
}



/* ========================================================================== */
/**
* @fn TIMM_OSAL_MutexRelease function
*
* @ see timm_osal_mutex.h file
*/
/* ========================================================================== */
TIMM_OSAL_ERRORTYPE TIMM_OSAL_MutexRelease(TIMM_OSAL_PTR pMutex)
{
    TIMM_OSAL_MUTEX *pHandle = (TIMM_OSAL_MUTEX *)pMutex;
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;    

	if(pHandle == NULL)
	{
		bReturnStatus = TIMM_OSAL_ERR_PARAMETER;
		goto EXIT;
	}

    /* Release the mutex.  */
    GateMutexPri_leave(pHandle->gate, pHandle->key);
	pHandle->key = NULL;
EXIT:
    return bReturnStatus;
}
