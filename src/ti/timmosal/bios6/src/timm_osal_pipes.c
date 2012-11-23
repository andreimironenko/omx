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
*   @file  timm_osal_pipes.c
*   This file contains methods that provides the functionality
*   for creating/using Nucleus pipes.
*
*  @path \
*
*/
/* -------------------------------------------------------------------------- */
/* =========================================================================
 *!
 *! Revision History
 *! ===================================
 *! 07-Aug-2009 Aditya Monga: admonga@ti.com Removing dependency on List module
 *!                           Using internal stack implementation to support
 *!                           mutiple write to front of pipe
 *! 30-June-2009 Aditya Monga: admonga@ti.com KW defects
 *! 1-June-2009 Aditya Monga: admonga@ti.com Using List module for supporting 
 *!                           mutiple write to front of pipe. Code cleanup.
 *! 23-May-2008 Gaurav Aradhya: gaurav.a@ti.com Ported to DSP BIOS
 *! 07-May-2008 Rabbani Patan: rabbani@ti.com updated version
 *! 0.1: Created the first draft version, ksrini@ti.com
 * ========================================================================= */

/******************************************************************************
* Includes
******************************************************************************/
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Mailbox.h>

/*For using the global gate*/
#include <ti/sysbios/gates/GateMutexPri.h>

#include "timm_osal_types.h"
#include "timm_osal_error.h"
#include "timm_osal_memory.h"
#include "timm_osal_trace.h"
#include "timm_osal_mutex.h"
#include "../inc/timm_osal_internal.h"

/**
* TIMM_OSAL_PIPE structure define the OSAL pipe
*/
typedef struct TIMM_OSAL_PIPE
{
	TIMM_OSAL_CHAR name[16];
    /** Bios Mail box handler structure */
    Mailbox_Handle  pipe;    
    /*To hold elements for write to front of pipe*/
    TIMM_OSAL_PTR pStack;	
	TIMM_OSAL_PTR pTopOfStack;
    TIMM_OSAL_PTR pMutex;
    TIMM_OSAL_U32 msgSize;
    TIMM_OSAL_U32 numFrontElements;
    TIMM_OSAL_U32 maxElements;
} TIMM_OSAL_PIPE;

static TIMM_OSAL_U32 gPipCnt = 0;
static TIMM_OSAL_U32 gUniquePipNameCnt = 0;

/*Global OSAL gate*/
extern GateMutexPri_Handle gOSALgate;
/******************************************************************************
* Function Prototypes
******************************************************************************/

/* ========================================================================== */
/**
* @fn TIMM_OSAL_CreatePipe function
*
* @see timm_osal_pipes.h
*/
/* ========================================================================== */

TIMM_OSAL_ERRORTYPE TIMM_OSAL_CreatePipe (TIMM_OSAL_PTR *pPipe,
                                          TIMM_OSAL_U32  pipeSize,
                                          TIMM_OSAL_U32  messageSize,
                                          TIMM_OSAL_U8   isFixedMessage)
{
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;
    Bool bStatus = FALSE;
    Mailbox_Params mbParams;
    IArg keyOSALgate;

    TIMM_OSAL_PIPE *pHandle = TIMM_OSAL_NULL;

    if (messageSize > pipeSize) {
        bReturnStatus = TIMM_OSAL_ERR_PARAMETER;
        TIMM_OSAL_Error ("Bad parameter!!!");
        goto EXIT;
    }
    
    if(!isFixedMessage)
    {
        bReturnStatus = TIMM_OSAL_ERR_PARAMETER;
        TIMM_OSAL_Error ("Variable message size not supported!!!");
        goto EXIT;
    }

    pHandle = (TIMM_OSAL_PIPE *)TIMM_OSAL_Malloc(sizeof(TIMM_OSAL_PIPE),
                              TIMM_OSAL_TRUE, 0, TIMMOSAL_MEM_SEGMENT_EXT);
    if (TIMM_OSAL_NULL == pHandle) 
    {
        bReturnStatus = TIMM_OSAL_ERR_ALLOC;
        goto EXIT;
    }
    TIMM_OSAL_Memset(pHandle, 0x0, sizeof(TIMM_OSAL_PIPE));
    pHandle->msgSize = messageSize;
    pHandle->numFrontElements = 0;
    pHandle->maxElements = pipeSize/messageSize;
    pHandle->pStack = NULL;

    keyOSALgate = GateMutexPri_enter(gOSALgate);
    sprintf(pHandle->name, "PIP%lu", gUniquePipNameCnt++);
	/*To prevent array overflow*/
	if(gUniquePipNameCnt == 9999)
		gUniquePipNameCnt = 0;
	GateMutexPri_leave(gOSALgate, keyOSALgate);

	/*Creating stack for write to front of pipe*/
    pHandle->pStack = TIMM_OSAL_MallocExtn(pipeSize, TIMM_OSAL_TRUE, 0,
		                                   TIMMOSAL_MEM_SEGMENT_EXT, NULL);
    if(pHandle->pStack == NULL)
    {
        TIMM_OSAL_Error ("Create Pipe failed!!!");
        bReturnStatus = TIMM_OSAL_ERR_ALLOC;
        goto EXIT;
    }
	pHandle->pTopOfStack = pHandle->pStack;
    
    Mailbox_Params_init (&mbParams);
    pHandle->pipe = Mailbox_create (messageSize,pipeSize/messageSize,
                                    NULL,NULL);

    if (pHandle->pipe == NULL) 
    {
        TIMM_OSAL_Error ("Create Pipe failed!!!");
        bReturnStatus = TIMM_OSAL_ERR_CREATE(TIMM_OSAL_ERR, 
                                             TIMM_OSAL_COMP_PIPES, bStatus);
        goto EXIT;
    }


	bReturnStatus = TIMM_OSAL_MutexCreate(&(pHandle->pMutex));
	if(bReturnStatus != TIMM_OSAL_ERR_NONE)
    {
        TIMM_OSAL_Error ("Create Pipe failed!!!");
        goto EXIT;
    }

    gPipCnt++;
    *pPipe = pHandle;

EXIT:
    if (bReturnStatus != TIMM_OSAL_ERR_NONE && pHandle != NULL) 
    {
        if(pHandle->pStack != NULL)
            TIMM_OSAL_Free(pHandle->pStack);
		if(pHandle->pipe != NULL)
			Mailbox_delete(&(pHandle->pipe));
        TIMM_OSAL_Free(pHandle);
    }
    return bReturnStatus;
}



/* ========================================================================== */
/**
* @fn TIMM_OSAL_DeletePipe function
*
* @see timm_osal_pipes.h
*/
/* ========================================================================== */

TIMM_OSAL_ERRORTYPE TIMM_OSAL_DeletePipe (TIMM_OSAL_PTR pPipe)
{
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;
    TIMM_OSAL_PIPE *pHandle = (TIMM_OSAL_PIPE *)pPipe;

    if(pHandle == NULL)
	{
		bReturnStatus = TIMM_OSAL_ERR_PARAMETER;
		goto EXIT;
	}
	
	TIMM_OSAL_Free(pHandle->pStack);
    Mailbox_delete(&(pHandle->pipe));
    TIMM_OSAL_MutexDelete(pHandle->pMutex);

    gPipCnt--;

    TIMM_OSAL_Free(pHandle);
	pPipe = NULL;

EXIT:
    return bReturnStatus;
}



/* ========================================================================== */
/**
* @fn TIMM_OSAL_WriteToPipe function
*
* @see timm_osal_pipes.h
*/
/* ========================================================================== */

TIMM_OSAL_ERRORTYPE TIMM_OSAL_WriteToPipe (TIMM_OSAL_PTR pPipe,
                                           TIMM_OSAL_PTR pMessage,
                                           TIMM_OSAL_U32 size,
                                           TIMM_OSAL_S32 timeout)
{
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;
    Bool bStatus;
    TIMM_OSAL_U32 uTimeOut;
    TIMM_OSAL_PIPE *pHandle = (TIMM_OSAL_PIPE *)pPipe;

    if(pHandle == NULL)
	{
		bReturnStatus = TIMM_OSAL_ERR_PARAMETER;
		goto EXIT;
	}
	
    if (size > pHandle->msgSize) 
    {
        bReturnStatus = TIMM_OSAL_ERR_PARAMETER;
        TIMM_OSAL_Error ("Bad parameter!!!");
        goto EXIT;
    }

    if (TIMM_OSAL_SUSPEND == timeout) 
    {
        uTimeOut = BIOS_WAIT_FOREVER;
    }
	else if(TIMM_OSAL_NO_SUSPEND == timeout)
	{
		uTimeOut = BIOS_NO_WAIT;
	}
	else
	{
       uTimeOut = _TIMM_OSAL_GetTicks(timeout);
	}

    bStatus = Mailbox_post(pHandle->pipe, pMessage, uTimeOut);

    if (FALSE == bStatus) 
    {
        TIMM_OSAL_Error (" Write to Pipe failed!!!");
        bReturnStatus = TIMM_OSAL_ERR_CREATE(TIMM_OSAL_ERR, 
                                             TIMM_OSAL_COMP_PIPES, bStatus);
        goto EXIT;
    }

EXIT:
    return bReturnStatus;
}



/* ========================================================================== */
/**
* @fn TIMM_OSAL_WriteToFrontOfPipe function
*
* @see timm_osal_pipes.h
*/
/* ========================================================================== */

TIMM_OSAL_ERRORTYPE TIMM_OSAL_WriteToFrontOfPipe (TIMM_OSAL_PTR pPipe,
                                                  TIMM_OSAL_PTR pMessage,
                                                  TIMM_OSAL_U32 size,
                                                  TIMM_OSAL_S32 timeout)
{
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;
    TIMM_OSAL_PIPE *pHandle = (TIMM_OSAL_PIPE *)pPipe;

	if(pHandle == NULL)
	{
		bReturnStatus = TIMM_OSAL_ERR_PARAMETER;
		goto EXIT;
	}

    if (size > pHandle->msgSize) 
	{
        bReturnStatus = TIMM_OSAL_ERR_PARAMETER;
        TIMM_OSAL_Error ("Bad parameter!!!");
        goto EXIT;
    }

	TIMM_OSAL_MutexObtain(pHandle->pMutex, TIMM_OSAL_SUSPEND);
	
	/*Not possible to check timeout here so return error if full*/
	if(pHandle->numFrontElements == pHandle->maxElements)
	{
		TIMM_OSAL_Error("Pipe is full!!");
		bReturnStatus = TIMM_OSAL_ERR_PIPE_FULL;
		goto EXIT;
	}
	TIMM_OSAL_Memcpy(pHandle->pTopOfStack, pMessage, size);
	pHandle->pTopOfStack = (TIMM_OSAL_PTR)
		     ((TIMM_OSAL_U32)(pHandle->pTopOfStack) + pHandle->msgSize);
	pHandle->numFrontElements++;
	TIMM_OSAL_MutexRelease(pHandle->pMutex);

EXIT:
    return bReturnStatus;
}



/* ========================================================================== */
/**
* @fn TIMM_OSAL_ReadFromPipe function
*
* @see timm_osal_pipes.h
*/
/* ========================================================================== */

TIMM_OSAL_ERRORTYPE TIMM_OSAL_ReadFromPipe (TIMM_OSAL_PTR pPipe,
                                            TIMM_OSAL_PTR pMessage,
                                            TIMM_OSAL_U32 size,
                                            TIMM_OSAL_U32 *actualSize,
                                            TIMM_OSAL_S32 timeout)
{
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;
    TIMM_OSAL_U32 uTimeOut;
    TIMM_OSAL_BOOL bInStack = TIMM_OSAL_FALSE;
    Bool bStatus;

    TIMM_OSAL_PIPE *pHandle = (TIMM_OSAL_PIPE *)pPipe;

	if(pHandle == NULL)
	{
		bReturnStatus = TIMM_OSAL_ERR_PARAMETER;
		goto EXIT;
	}
	if(size != pHandle->msgSize)
	{
		bReturnStatus = TIMM_OSAL_ERR_PARAMETER;
		goto EXIT;
	}

    if (TIMM_OSAL_SUSPEND == timeout) 
	{
        uTimeOut = BIOS_WAIT_FOREVER;
    }
	else if(TIMM_OSAL_NO_SUSPEND == timeout)
	{
		uTimeOut = BIOS_NO_WAIT;
	}
	else
	{
       uTimeOut = _TIMM_OSAL_GetTicks(timeout);
	}

    TIMM_OSAL_MutexObtain(pHandle->pMutex, TIMM_OSAL_SUSPEND);	

	if(pHandle->pStack != pHandle->pTopOfStack)
	{
		pHandle->pTopOfStack = (TIMM_OSAL_PTR)((TIMM_OSAL_U32)(pHandle->pTopOfStack) -
			                             pHandle->msgSize);
		TIMM_OSAL_Memcpy(pMessage, pHandle->pTopOfStack, pHandle->msgSize);
		
		pHandle->numFrontElements--;
		bInStack = TIMM_OSAL_TRUE;
	}
	TIMM_OSAL_MutexRelease(pHandle->pMutex);

/*Read is not synchronised with clear pipe. No workaround to implement the 
  sync for now*/
	if(!bInStack)
    {
        bStatus = Mailbox_pend (pHandle->pipe, pMessage ,uTimeOut);
        if (FALSE == bStatus) 
		{
            TIMM_OSAL_Error (" Read to Pipe failed!!!");
            bReturnStatus = TIMM_OSAL_ERR_CREATE(TIMM_OSAL_ERR, 
                                                 TIMM_OSAL_COMP_PIPES, bStatus);
            goto EXIT;
        }
    }

    
EXIT:
	if(pHandle != NULL)
        *actualSize = pHandle->msgSize;
    return bReturnStatus;
}



/* ========================================================================== */
/**
* @fn TIMM_OSAL_ClearPipe function
*
* @see timm_osal_pipes.h
*/
/* ========================================================================== */

TIMM_OSAL_ERRORTYPE TIMM_OSAL_ClearPipe (TIMM_OSAL_PTR pPipe)
{
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;
    TIMM_OSAL_U8 * msgPtr;
    TIMM_OSAL_PIPE *pHandle = (TIMM_OSAL_PIPE *)pPipe;
	Bool bStatus;

	if(pHandle == NULL)
	{
		bReturnStatus = TIMM_OSAL_ERR_PARAMETER;
		goto EXIT;
	}
    msgPtr = (TIMM_OSAL_U8 *)TIMM_OSAL_Malloc(pHandle->msgSize,
                              TIMM_OSAL_TRUE, 0, TIMMOSAL_MEM_SEGMENT_EXT);
    if(msgPtr == NULL)
	{
         TIMM_OSAL_Error (" Pipe Clear: Malloc failed!!!");
         bReturnStatus = TIMM_OSAL_ERR_ALLOC;
    }

    /*Not synchronized with write/read*/

    TIMM_OSAL_MutexObtain(pHandle->pMutex, TIMM_OSAL_SUSPEND);

    pHandle->pTopOfStack = pHandle->pStack;
    pHandle->numFrontElements = 0;

    bStatus = Mailbox_pend(pHandle->pipe, msgPtr ,BIOS_NO_WAIT);
    while(TRUE == bStatus) 
	{
        bStatus = Mailbox_pend(pHandle->pipe, msgPtr ,BIOS_NO_WAIT);
    }

    TIMM_OSAL_MutexRelease(pHandle->pMutex);

    TIMM_OSAL_Free(msgPtr);
EXIT:
    return bReturnStatus;
}



/* ========================================================================== */
/**
* @fn TIMM_OSAL_IsPipeReady function
*
* @see timm_osal_pipes.h
*/
/* ========================================================================== */

TIMM_OSAL_ERRORTYPE TIMM_OSAL_IsPipeReady (TIMM_OSAL_PTR pPipe)
{
    TIMM_OSAL_PIPE *pHandle = (TIMM_OSAL_PIPE *)pPipe;
	TIMM_OSAL_U32 nCount = 0;

    if(pHandle == NULL)
	{
		return TIMM_OSAL_ERR_PARAMETER;
	}
	
	TIMM_OSAL_MutexObtain(pHandle->pMutex, TIMM_OSAL_SUSPEND);
	nCount = pHandle->numFrontElements + 
             Mailbox_getNumPendingMsgs(pHandle->pipe);
    TIMM_OSAL_MutexRelease(pHandle->pMutex);
    
    if (0 != nCount) 
	{
        return TIMM_OSAL_ERR_NONE;
    } 
	else 
	{
        return TIMM_OSAL_ERR_NOT_READY;
    }
}



/* ========================================================================== */
/**
* @fn TIMM_OSAL_GetPipeReadyMessageCount function
*
* @see timm_osal_pipes.h
*/
/* ========================================================================== */

TIMM_OSAL_ERRORTYPE TIMM_OSAL_GetPipeReadyMessageCount (TIMM_OSAL_PTR pPipe,
                                                        TIMM_OSAL_U32 *count)
{
    TIMM_OSAL_PIPE *pHandle = (TIMM_OSAL_PIPE *)pPipe;

	if(pHandle == NULL)
	{
		return TIMM_OSAL_ERR_PARAMETER;
	}

    TIMM_OSAL_MutexObtain(pHandle->pMutex, TIMM_OSAL_SUSPEND);	
    *count = pHandle->numFrontElements +
             Mailbox_getNumPendingMsgs(pHandle->pipe);
    TIMM_OSAL_MutexRelease(pHandle->pMutex);
                 
    return TIMM_OSAL_ERR_NONE;
}
