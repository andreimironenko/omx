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
*   @file  timm_osal_semaphores.c
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
#include <ti/sysbios/knl/Semaphore.h>
/*For using the global gate*/
#include <ti/sysbios/gates/GateMutexPri.h>

#include "timm_osal_types.h"
#include "timm_osal_error.h"
#include "timm_osal_memory.h"
#include "../inc/timm_osal_internal.h"

#define SEMNAME_MAX 16


typedef struct TIMM_OSAL_SEMAPHORE 
{
    Semaphore_Handle  sem; 
    char name[SEMNAME_MAX];
} TIMM_OSAL_SEMAPHORE;

static TIMM_OSAL_U32    gSemCnt = 0;
static TIMM_OSAL_U32    gUniqueSemNameCnt = 0;

/*Global OSAL gate*/
extern GateMutexPri_Handle gOSALgate;
/* ========================================================================== */
/**
* @fn TIMM_OSAL_SemaphoreCreate function
*
* @ see timm_osal_semaphores.h file
*/
/* ========================================================================== */
TIMM_OSAL_ERRORTYPE TIMM_OSAL_SemaphoreCreate(TIMM_OSAL_PTR *pSemaphore,
                                              TIMM_OSAL_U32  uInitCount)
{
	TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;
    TIMM_OSAL_SEMAPHORE *pHandle = TIMM_OSAL_NULL;
    Semaphore_Params params;
	IArg keyOSALgate;

    *pSemaphore = TIMM_OSAL_NULL;
    pHandle = (TIMM_OSAL_SEMAPHORE *) TIMM_OSAL_Malloc(sizeof(TIMM_OSAL_SEMAPHORE),
                                      TIMM_OSAL_TRUE, 0, TIMMOSAL_MEM_SEGMENT_EXT);

    if(TIMM_OSAL_NULL == pHandle)
    {
        bReturnStatus = TIMM_OSAL_ERR_ALLOC;
		goto EXIT;
    }

    /* Generate name of the semaphore */
	keyOSALgate = GateMutexPri_enter(gOSALgate);
    sprintf(pHandle->name, "SEM%lu", gUniqueSemNameCnt++);
	/*To prevent array overflow*/
	if(gUniqueSemNameCnt == 9999)
		gUniqueSemNameCnt = 0;
	GateMutexPri_leave(gOSALgate, keyOSALgate);

	/*Initialize with default values*/
    Semaphore_Params_init(&params);
	params.instance->name = (xdc_String)(pHandle->name);
    params.mode = Semaphore_Mode_COUNTING; 
    
	pHandle->sem = Semaphore_create(uInitCount,&params,NULL);

    if(pHandle->sem == NULL)
    {
        TIMM_OSAL_Free(pHandle);
        bReturnStatus = TIMM_OSAL_ERR_UNKNOWN;
    }

    /* Update sem counter */
    gSemCnt++;

    *pSemaphore = (TIMM_OSAL_PTR)pHandle;

EXIT:
    return bReturnStatus;
}



/* ========================================================================== */
/**
* @fn TIMM_OSAL_SemaphoreDelete function
*
* @ see timm_osal_semaphores.h file
*/
/* ========================================================================== */
TIMM_OSAL_ERRORTYPE TIMM_OSAL_SemaphoreDelete(TIMM_OSAL_PTR pSemaphore)
{
    TIMM_OSAL_SEMAPHORE *pHandle = (TIMM_OSAL_SEMAPHORE *)pSemaphore;
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;

	if(pHandle == NULL)
	{
		bReturnStatus = TIMM_OSAL_ERR_PARAMETER;
		goto EXIT;
	}

    /* Delete the semaphore.  */
	Semaphore_delete(&(pHandle->sem));

    /* Update sem counter */
    gSemCnt--;

    TIMM_OSAL_Free(pSemaphore);

EXIT:
    return bReturnStatus;
}



/* ========================================================================== */
/**
* @fn TIMM_OSAL_SemaphoreObtain function
*
* @ see timm_osal_semaphores.h file
*/
/* ========================================================================== */


TIMM_OSAL_ERRORTYPE TIMM_OSAL_SemaphoreObtain(TIMM_OSAL_PTR pSemaphore, TIMM_OSAL_U32 uTimeOut)
{
    TIMM_OSAL_SEMAPHORE *pHandle = (TIMM_OSAL_SEMAPHORE *)pSemaphore;
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;
	TIMM_OSAL_U32 timeout;
	Bool status;

	if(pHandle == NULL)
	{
		bReturnStatus = TIMM_OSAL_ERR_PARAMETER;
		goto EXIT;
	}

    if (TIMM_OSAL_SUSPEND == uTimeOut) {
        timeout = BIOS_WAIT_FOREVER;
    }
	else
	{
       timeout = _TIMM_OSAL_GetTicks(uTimeOut);
	}

    status = Semaphore_pend(pHandle->sem, timeout);

    if(status == FALSE) 
    {
        bReturnStatus = TIMM_OSAL_ERR_CREATE(TIMM_OSAL_ERR, TIMM_OSAL_COMP_SEMAPHORES, status);
    }
EXIT:
    return bReturnStatus;
}



/* ========================================================================== */
/**
* @fn TIMM_OSAL_SemaphoreRelease function
*
* @ see timm_osal_semaphores.h file
*/
/* ========================================================================== */
TIMM_OSAL_ERRORTYPE TIMM_OSAL_SemaphoreRelease(TIMM_OSAL_PTR pSemaphore)
{
    TIMM_OSAL_SEMAPHORE *pHandle = (TIMM_OSAL_SEMAPHORE *)pSemaphore;
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;    

	if(pHandle == NULL)
	{
		bReturnStatus = TIMM_OSAL_ERR_PARAMETER;
		goto EXIT;
	}

    /* Release the semaphore.  */
    Semaphore_post(pHandle->sem); 

EXIT:
    return bReturnStatus;
}



/* ========================================================================== */
/**
* @fn TIMM_OSAL_SemaphoreReset function
*
* @ see timm_osal_semaphores.h file
*/
/* ========================================================================== */
TIMM_OSAL_ERRORTYPE TIMM_OSAL_SemaphoreReset(TIMM_OSAL_PTR pSemaphore, TIMM_OSAL_U32 uInitCount)
{
    TIMM_OSAL_SEMAPHORE *pHandle = (TIMM_OSAL_SEMAPHORE *)pSemaphore;
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_UNKNOWN;

	if(pHandle == NULL)
	{
		bReturnStatus = TIMM_OSAL_ERR_PARAMETER;
		goto EXIT;
	}

	Semaphore_reset(pHandle->sem, uInitCount);

EXIT:
    return bReturnStatus;
}
