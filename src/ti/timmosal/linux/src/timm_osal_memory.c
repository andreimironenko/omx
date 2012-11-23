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
 *!23-Oct-2008 Maiya ShreeHarsha: Linux specific changes
 *!0.1: Created the first draft version, ksrini@ti.com
 * ========================================================================= */

/******************************************************************************
* Includes
******************************************************************************/

#include <string.h>
#include <malloc.h>

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#endif

#include "timm_osal_types.h"
#include "timm_osal_trace.h"
#include "timm_osal_error.h"
#include "timm_osal_memory.h"

/* OMAP3MIG begin - this is dirty, but no other way*
typedef char *String;
typedef unsigned int UInt32;
typedef unsigned int Uint32;
typedef unsigned int UInt;
typedef void *Ptr;
typedef int Int;
typedef unsigned Uns;
typedef char Char;
typedef void Void;
typedef unsigned short Bool;    */

static TIMM_OSAL_U32 gMallocCounter = 0;
static TIMM_OSAL_U32 gSizeCounter = 0;  /* OMAP3MIG */

extern int globDebugDisable;
/******************************************************************************
* Function Prototypes
******************************************************************************/

/* ========================================================================== */
/**
* @fn TIMM_OSAL_createMemoryPool function
*
* @see
*/
/* ========================================================================== */
TIMM_OSAL_ERRORTYPE TIMM_OSAL_CreateMemoryPool (void)
{
  TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;
  return bReturnStatus;
}

/* ========================================================================== */
/**
* @fn TIMM_OSAL_DeleteMemoryPool function
*
* @see
*/
/* ========================================================================== */

TIMM_OSAL_ERRORTYPE TIMM_OSAL_DeleteMemoryPool (void)
{
  TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;
  return bReturnStatus;

}

/* ========================================================================== */
/**
* @fn TIMM_OSAL_Malloc function
*
* @see
*/
/* ========================================================================== */
/* OMAP3MIG - This is required since TIMM_OSAL does not support 
              CMEM allocation and free
   BEGIN
*/
#define TIMM_OSAL_MAX_CMEM_BUFFERS 32
TIMM_OSAL_U32 glob_timm_osal_cmem_addr[TIMM_OSAL_MAX_CMEM_BUFFERS];
TIMM_OSAL_U32 glob_timm_osal_cmem_size[TIMM_OSAL_MAX_CMEM_BUFFERS];
/* END - OMAP3MIG */
TIMM_OSAL_PTR TIMM_OSAL_Malloc_ori (TIMM_OSAL_U32 size,
                                    TIMM_OSAL_BOOL bBlockContiguous,
                                    TIMM_OSAL_U32 unBlockAlignment,
                                    TIMMOSAL_MEM_SEGMENTID tMemSegId)
{

  TIMM_OSAL_PTR pData = TIMM_OSAL_NULL;
  /* Memory_AllocParams memParams; */

  if (0 != unBlockAlignment)
  {
    /* TIMM_OSAL_Error("Memory Allocation:Not done for specified
       nBufferAlignment"); */
  }
  else
  {
#ifdef TIMMOSAL_SPECIALCASE_CMEM
    if (bBlockContiguous == 101)        /* Special case - CMEM */
    {
      int reg = FALSE, i;
      /* globDebugDisable || printf ("TIMM_OSAL_Malloc: stage 0.21; size=%d\n",
         size); */
      memParams.type = Memory_CONTIGHEAP;
      memParams.flags = Memory_NONCACHED;
      memParams.align = 0;
      pData = Memory_alloc (size, &memParams);
      for (i = 0; i < TIMM_OSAL_MAX_CMEM_BUFFERS; i++)
      {
        if (glob_timm_osal_cmem_addr[i] == 0)
        {
          glob_timm_osal_cmem_addr[i] = (TIMM_OSAL_U32) pData;
          glob_timm_osal_cmem_size[i] = size;
          reg = TRUE;
          break;
        }
      }
      if (reg == FALSE)
      {
        globDebugDisable ||
          printf ("TIMM_OSAL_Malloc: stage 0.215 - CMEM register failed\n");
      }
    }
    else
    {
#endif /* TIMMOSAL_SPECIALCASE_CMEM */
      pData = malloc ((size_t) size);   /* size_t is long long */
#ifdef TIMMOSAL_SPECIALCASE_CMEM
    }
#endif /* TIMMOSAL_SPECIALCASE_CMEM */
    if (TIMM_OSAL_NULL == pData)
    {
      TIMM_OSAL_Error ("Malloc failed!!!");
    }
    else
    {
      /* Memory Allocation was successfull */
      gMallocCounter++;
      gSizeCounter += size;
    }
  }

  return pData;
}

/* OMAP3MIG - just to maintain code compatibility between BIOS6 and Linux 
   TIMM OSAL*/
TIMM_OSAL_PTR TIMM_OSAL_MallocExtn (TIMM_OSAL_U32 size, 
                                    TIMM_OSAL_BOOL bBlockContiguous, 
                                    TIMM_OSAL_U32 unBlockAlignment, 
                                    TIMMOSAL_MEM_SEGMENTID tMemSegId, 
                                    TIMM_OSAL_PTR hHeap) /* This field ignored 
                                                            on linux */
{
  TIMM_OSAL_PTR pData;

  pData =
    TIMM_OSAL_Malloc_ori (size, bBlockContiguous, unBlockAlignment, tMemSegId);

  return pData;
}

/* ========================================================================== */
/**
* @fn TIMM_OSAL_Free function ....
*
* @see
*/
/* ========================================================================== */

void TIMM_OSAL_Free (TIMM_OSAL_PTR pData)
{
  if (TIMM_OSAL_NULL == pData)
  {
    /* TIMM_OSAL_Warning("TIMM_OSAL_Free called on NULL pointer"); */
    goto EXIT;
  }

  free (pData);
  pData = NULL;
  gMallocCounter--;
EXIT:
  return;
}

#ifdef TIMMOSAL_SPECIALCASE_CMEM
void TIMM_OSAL_CMEM_Free (TIMM_OSAL_PTR pData, TIMM_OSAL_U32 size)
{
  Memory_AllocParams memParams;

  /* globDebugDisable || printf ("TIMM_OSAL_CMEM_Free: first stage 0.1\n"); */
  if (TIMM_OSAL_NULL == pData)
  {
    /* TIMM_OSAL_Warning("TIMM_OSAL_Free called on NULL pointer"); */
    goto EXIT;
  }

  memParams.type = Memory_CONTIGHEAP;
  memParams.flags = Memory_NONCACHED;
  memParams.align = 0;
  /* OMAP3MIG begin */
  if (size == 0)
  {                             /* Retrieve the corresponding size in the
                                   global CMEM register */
    int reg = FALSE, i;

    for (i = 0; i < TIMM_OSAL_MAX_CMEM_BUFFERS; i++)
    {
      if (glob_timm_osal_cmem_addr[i] == (TIMM_OSAL_U32) pData)
      {
        Memory_free (pData, glob_timm_osal_cmem_size[i], &memParams);
        glob_timm_osal_cmem_addr[i] = 0;
        glob_timm_osal_cmem_size[i] = 0;

        pData = NULL;
        reg = TRUE;
        break;
      } /* if (glob_timm_osal_cmem_addr[i]) */
    } /* for (i) */
    if (reg == FALSE)
    {
      globDebugDisable ||
        printf
        ("TIMM_OSAL_CMEM_Free: stage 0.2 - CMEM entry not found in registry\n");
    }
  } /* if (size) */
  else
  {
    Memory_free (pData, size, &memParams);
    pData = NULL;
  } /* if (size) else ... */
  /* OMAP3MIG end */
  gMallocCounter--;
EXIT:
  /* globDebugDisable || printf ("TIMM_OSAL_CMEM_Free: last stage 2.0\n"); */
  return;
}
#endif /* TIMMOSAL_SPECIALCASE_CMEM */
/* ========================================================================== */
/**
* @fn TIMM_OSAL_Memset function ....
*
* @see
*/
/* ========================================================================== */

TIMM_OSAL_ERRORTYPE TIMM_OSAL_Memset (TIMM_OSAL_PTR pBuffer,
                                      TIMM_OSAL_U8 uValue,
                                      TIMM_OSAL_U32 uSize)
{
  TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_UNKNOWN;

  void *pBufferSet = memset ((void *) pBuffer, (int) uValue, (size_t) uSize);
  if (pBufferSet != pBuffer)
  {
    /* TIMM_OSAL_Error("Memset failed!!!"); */
    goto EXIT;
  }

  bReturnStatus = TIMM_OSAL_ERR_NONE;

EXIT:
  return bReturnStatus;
}

/* ========================================================================== */
/**
* @fn TIMM_OSAL_Memcmp function ....
*
* @see
*/
/* ========================================================================== */

TIMM_OSAL_S32 TIMM_OSAL_Memcmp (TIMM_OSAL_PTR pBuffer1, TIMM_OSAL_PTR pBuffer2,
                                TIMM_OSAL_U32 uSize)
{
  TIMM_OSAL_S32 result = memcmp (pBuffer1, pBuffer2, uSize);

  if (result > 0)
  {
    return 1;
  }
  else if (result < 0)
  {
    return -1;
  }

  return 0;
}

/* ========================================================================== */
/**
* @fn TIMM_OSAL_Memcpy function ....
*
* @see
*/
/* ========================================================================== */

TIMM_OSAL_ERRORTYPE TIMM_OSAL_Memcpy (TIMM_OSAL_PTR pBufDst,
                                      TIMM_OSAL_PTR pBufSrc,
                                      TIMM_OSAL_U32 uSize)
{
  TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_UNKNOWN;

  void *lpDest = memcpy (pBufDst, pBufSrc, uSize);
  if (lpDest != pBufDst)
  {
    /* TIMM_OSAL_Error("Memcpy failed!!!"); */
    goto EXIT;
  }
  bReturnStatus = TIMM_OSAL_ERR_NONE;
EXIT:
  return bReturnStatus;
}

/* ========================================================================== */
/**
* @fn TIMM_OSAL_GetMemCounter function ....
*
* @see
*/
/* ========================================================================== */

TIMM_OSAL_ERRORTYPE TIMM_OSAL_GetMemCounter (void)
{

  return gMallocCounter;
}

#define TIMM_OSAL_MEM_ARBITRARY_SIZE 0
/* ========OMAP3MIG========================================================== */
/**
* @fn TIMM_OSAL_GetMemUsage function ....
*
* @see
*/
/* ========================================================================== */

TIMM_OSAL_U32 TIMM_OSAL_GetMemUsage (void)
{
  return TIMM_OSAL_MEM_ARBITRARY_SIZE;
}

/* Nothing beyond this point */
