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
*   @file  timm_osal_memory.c
*   This file contains methods that provides the functionality
*   for allocating/deallocating memory.
*
*  @path \
*
*/
/* -------------------------------------------------------------------------- */
/* =========================================================================
 *!
 *! Revision History
 *! ===================================
 *! 1-June-2009 Aditya Monga: admonga@ti.com General code cleanup activity.
 *! 23-May-2008 Gaurav Aradhya: gaurav.a@ti.com Ported to DSP BIOS
 *!07-May-2008 Rabbani Patan: rabbani@ti.com updated version
 *!0.1: Created the first draft version, ksrini@ti.com
 * ========================================================================= */

/******************************************************************************
* Includes
*****************************************************************************/
#include <string.h>
#include <assert.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/heaps/HeapMem.h>
#include <xdc/runtime/Memory.h>
#include "timm_osal_types.h"
#include "timm_osal_error.h"
#include "timm_osal_trace.h"
#include "timm_osal_memory.h"


#include <xdc/runtime/Error.h> 

/******************************************************************************
* Defines
******************************************************************************/
#define MEM_CORRUPT_CHECK_VAL   (0xABCD0000)
#define MEM_CORRUPT_MASK (0xFFFF0000)
#define MEM_SIZE_MASK (0x0000FFFF)
#define MEMORY_HEADER_SIZE 16

/******************************************************************************
* Structures
******************************************************************************/


/******************************************************************************
*
******************************************************************************/


static TIMM_OSAL_U32 gMallocCounter = 0;
static TIMM_OSAL_U32 gSizeCounter = 0;

TIMM_OSAL_PTR timmUserHeap;


/******************************************************************************
* Function Prototypes
******************************************************************************/
#define TIMM_OSAL_INTERNAL_MEM_HEAP0_SIZE   (256 * 1024)
#define TIMM_OSAL_INTERNAL_MEM_HEAP1_SIZE   (192 * 1024)
#define TIMM_OSAL_INTERNAL_MEM_HEAP2_SIZE   (500 * 1024)

#pragma DATA_ALIGN (g_IntHeapMemory_0, 64);
#pragma DATA_SECTION (g_IntHeapMemory_0, ".intHeap_0");
char g_IntHeapMemory_0[TIMM_OSAL_INTERNAL_MEM_HEAP0_SIZE];
/* Memory pool handle */
HeapMem_Handle  g_IntHeapMemory_Handle_0 = NULL;

#pragma DATA_ALIGN (g_IntHeapMemory_1, 64);
#pragma DATA_SECTION (g_IntHeapMemory_1, ".intHeap_1");
char g_IntHeapMemory_1[TIMM_OSAL_INTERNAL_MEM_HEAP1_SIZE];
/* Memory pool handle */
HeapMem_Handle  g_IntHeapMemory_Handle_1 = NULL;

#pragma DATA_ALIGN (g_IntHeapMemory_2, 64);
#pragma DATA_SECTION (g_IntHeapMemory_2, ".intHeap_2");
char g_IntHeapMemory_2[TIMM_OSAL_INTERNAL_MEM_HEAP2_SIZE];
/* Memory pool handle */
HeapMem_Handle  g_IntHeapMemory_Handle_2 = NULL;

/* ========================================================================== */
/**
* @fn TIMM_OSAL_createMemoryPool function
*
* @see timm_osal_memory.h
*/
/* ========================================================================== */
TIMM_OSAL_ERRORTYPE TIMM_OSAL_CreateMemoryPool (void)
{
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;
    HeapMem_Params heapMemPrm;

    /* create memory pool heap  */

    HeapMem_Params_init(&heapMemPrm);

    heapMemPrm.buf = g_IntHeapMemory_0;
    heapMemPrm.size = sizeof(g_IntHeapMemory_0);

    g_IntHeapMemory_Handle_0 = HeapMem_create(&heapMemPrm, NULL);

    assert(g_IntHeapMemory_Handle_0 != NULL);

    HeapMem_Params_init(&heapMemPrm);

    heapMemPrm.buf = g_IntHeapMemory_1;
    heapMemPrm.size = sizeof(g_IntHeapMemory_1);

    g_IntHeapMemory_Handle_1 = HeapMem_create(&heapMemPrm, NULL);

    assert(g_IntHeapMemory_Handle_1 != NULL);

    HeapMem_Params_init(&heapMemPrm);

    heapMemPrm.buf = g_IntHeapMemory_2;
    heapMemPrm.size = sizeof(g_IntHeapMemory_2);

    g_IntHeapMemory_Handle_2 = HeapMem_create(&heapMemPrm, NULL);

    assert(g_IntHeapMemory_Handle_2 != NULL);

    /*Nothing to be done*/
    return bReturnStatus;
}



/* ========================================================================== */
/**
* @fn TIMM_OSAL_DeleteMemoryPool function
*
* @see timm_osal_memory.h
*/
/* ========================================================================== */

TIMM_OSAL_ERRORTYPE TIMM_OSAL_DeleteMemoryPool (void)
{
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;
    /*Nothing to be done*/
    return bReturnStatus;

}

/* ========================================================================== */
/**
* @fn TIMM_OSAL_Malloc function
*
* @see timm_osal_memory.h
*/
/* ========================================================================== */
TIMM_OSAL_PTR TIMM_OSAL_MallocExtn(TIMM_OSAL_U32 size,
                                   TIMM_OSAL_BOOL bBlockContiguous,
                                   TIMM_OSAL_U32 unBlockAlignment,
                                   TIMMOSAL_MEM_SEGMENTID tMemSegId,
							       TIMM_OSAL_PTR hHeap)
{

    TIMM_OSAL_PTR pData = TIMM_OSAL_NULL;
    TIMM_OSAL_U32 * pIntPtr = NULL;
    TIMM_OSAL_U32 total_alloc_size;
    TIMM_OSAL_U32 bytes_to_skip;
	Error_Block eb;

    /* If the alignment is 0, 2, 4, 8 or 16*/
    if((MEMORY_HEADER_SIZE >= unBlockAlignment))
    { 
       total_alloc_size = size + MEMORY_HEADER_SIZE;
       bytes_to_skip = 0;
    }
    else
    { 
       /* For 32 byte alignment or more */
       total_alloc_size = size + unBlockAlignment;
       bytes_to_skip = total_alloc_size - (size + MEMORY_HEADER_SIZE);
    }

    pData = Memory_alloc((xdc_runtime_IHeap_Handle)(hHeap), total_alloc_size, 
		                 unBlockAlignment, &eb);

    if (pData == NULL) 
    {
       TIMM_OSAL_Error("Memory Allocation failed!!!");
       pData = TIMM_OSAL_NULL;
    } 
    else 
    {
        /* Memory Allocation was successfull
          Store the size and corruption check symb in
          extra allocated bytes
        */

        gMallocCounter++;
        gSizeCounter += total_alloc_size;

        pIntPtr = (TIMM_OSAL_U32 *)pData;
        /* Because this is U32 pointer */
        pIntPtr = pIntPtr + bytes_to_skip/sizeof(TIMM_OSAL_U32);

	   /*
	   First 4 bytes of the 16 byte header are empty.
	   Next 4 bytes contain the heap handle
	   Next 2 bytes have the magic number for checking corruption
	   Next 2 bytes contain the bytes_to_skip (in case of alignment)
	   Final 4 bytes contain the total allocated size
	   */
	    pIntPtr++;
	    *pIntPtr++ = (TIMM_OSAL_U32)hHeap;
        *pIntPtr++ = (MEM_CORRUPT_CHECK_VAL | bytes_to_skip);
        *pIntPtr++ = total_alloc_size;
        pData = pIntPtr;
    }
    return pData;
}



/* ========================================================================== */
/**
* @fn TIMM_OSAL_Free function ....
*
* @see timm_osal_memory.h
*/
/* ========================================================================== */

void TIMM_OSAL_Free (TIMM_OSAL_PTR pData)
{
    TIMM_OSAL_U32 *pIntPtr = NULL, total_alloc_size;
    TIMM_OSAL_U32 bytes_to_skip;
	xdc_runtime_IHeap_Handle hHeap = NULL;

    if (TIMM_OSAL_NULL == pData) 
	{
        TIMM_OSAL_Error("TIMM_OSAL_Free called on NULL pointer");
        goto EXIT;
    }

    pIntPtr = (TIMM_OSAL_U32 *)pData;
    pIntPtr--;
    total_alloc_size = *pIntPtr--;
    if((*pIntPtr & MEM_CORRUPT_MASK) != MEM_CORRUPT_CHECK_VAL)
    {
        TIMM_OSAL_Error("TIMM_OSAL_Free called with Corrupted pointer");
        goto EXIT;
    }

    bytes_to_skip = (*pIntPtr & MEM_SIZE_MASK);
	pIntPtr--;
	TIMM_OSAL_Memcpy(&hHeap, pIntPtr, sizeof(TIMM_OSAL_U32));
	pIntPtr--;
    pData = pIntPtr - bytes_to_skip/sizeof(TIMM_OSAL_U32);

    Memory_free(hHeap, pData, total_alloc_size);

    /* Memory free was successfull */
    gMallocCounter--;
    gSizeCounter -= total_alloc_size;

EXIT:
    return;
}



/* ========================================================================== */
/**
* @fn TIMM_OSAL_Memset function ....
*
* @see timm_osal_memory.h
*/
/* ========================================================================== */

TIMM_OSAL_ERRORTYPE TIMM_OSAL_Memset (TIMM_OSAL_PTR pBuffer, 
									  TIMM_OSAL_U8 uValue, 
									  TIMM_OSAL_U32 uSize)
{
    memset((void*)pBuffer, uValue, uSize);
    return TIMM_OSAL_ERR_NONE;
}



/* ========================================================================== */
/**
* @fn TIMM_OSAL_Memcmp function ....
*
* @see timm_osal_memory.h
*/
/* ========================================================================== */

TIMM_OSAL_S32 TIMM_OSAL_Memcmp (TIMM_OSAL_PTR pBuffer1, 
								TIMM_OSAL_PTR pBuffer2, 
								TIMM_OSAL_U32 uSize)
{
    TIMM_OSAL_S32 result = memcmp(pBuffer1, pBuffer2, uSize);

    if (result > 0)  
	{
       return 1;
    }
    else if (result < 0)  
	{
        return((TIMM_OSAL_S32)-1);
    }
    return 0;
}



/* ========================================================================== */
/**
* @fn TIMM_OSAL_Memcpy function ....
*
* @see timm_osal_memory.h
*/
/* ========================================================================== */

TIMM_OSAL_ERRORTYPE TIMM_OSAL_Memcpy (TIMM_OSAL_PTR pBufDst, 
									  TIMM_OSAL_PTR pBufSrc, 
									  TIMM_OSAL_U32 uSize)
{
    memcpy(pBufDst, pBufSrc, uSize);
    return TIMM_OSAL_ERR_NONE;
}



/* ========================================================================== */
/**
* @fn TIMM_OSAL_GetMemCounter function ....
*
* @see timm_osal_memory.h
*/
/* ========================================================================== */

TIMM_OSAL_U32 TIMM_OSAL_GetMemCounter(void) 
{
    return gMallocCounter;
}



/* ========================================================================== */
/**
* @fn TIMM_OSAL_GetMemUsage function ....
*
* @see timm_osal_memory.h
*/
/* ========================================================================== */

TIMM_OSAL_U32 TIMM_OSAL_GetMemUsage(void) 
{
    return gSizeCounter;
}
