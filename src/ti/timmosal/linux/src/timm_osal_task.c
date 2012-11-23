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
*   @file  timm_osal_task.c
*   This file contains methods that provides the functionality
*   for creating/destroying tasks.
*
*  @path \
*
*/
/* -------------------------------------------------------------------------- */
/* =========================================================================
 *!
 *! Revision History
 *! ===================================
 *! 21-Oct-2008 Maiya ShreeHarsha: Linux specific changes
 *! 0.1: Created the first draft version, ksrini@ti.com
 * ========================================================================= */

/******************************************************************************
* Includes
******************************************************************************/

#include <stdio.h>
#include <pthread.h>            /* for POSIX calls */
#include <sched.h>              /* for sched structure */
#include <unistd.h>

#include "timm_osal_types.h"
#include "timm_osal_trace.h"
#include "timm_osal_error.h"
#include "timm_osal_memory.h"
#include "timm_osal_task.h"

/**
* TIMM_OSAL_TASK describe the different task information
*/
typedef struct TIMM_OSAL_TASK
{
  pthread_t threadID;           /* SHM check */
  /* To set the priority and stack size */
  pthread_attr_t ThreadAttr;    /* For setting the priority and stack size */
  /* parameters to the task */
  TIMM_OSAL_U32 uArgc;
  TIMM_OSAL_PTR pArgv;
  /** flag to check if task got created */
  TIMM_OSAL_BOOL isCreated;
  TIMM_OSAL_TaskProc pThFunc;
} TIMM_OSAL_TASK;

typedef void *(*TIMM_OSAL_START_THREAD_FCNPTR) (void *);
/******************************************************************************
* Function Prototypes
******************************************************************************/

/******************************************************************************
* Helper function to match the Bios OSAL implementation 
* It is required to pass parameter as follows
* 1. The first argument says number of arguments in the function
* 2. Second argument carries the argument value
******************************************************************************/

void TIMM_OSAL_CreateTaskHelperFunc (TIMM_OSAL_TASK *pHandle)
{
  TIMM_OSAL_U32 uArgc;
  TIMM_OSAL_PTR pArgv;

  /* Validate the arguments */
  if (pHandle == NULL)
  {
    TIMM_OSAL_Error ("Invalid Argument!");
    goto EXIT;
  }

  /* Compose the argument to thread function */
  uArgc = pHandle->uArgc;
  pArgv = pHandle->pArgv;

  pHandle->pThFunc (uArgc, pArgv);

EXIT:
  return;
}

/* ========================================================================== */
/**
* @fn TIMM_OSAL_CreateTask function
*
* @see
*/
/* ========================================================================== */

TIMM_OSAL_ERRORTYPE TIMM_OSAL_CreateTask (TIMM_OSAL_PTR *pTask,
                                          TIMM_OSAL_TaskProc pFunc,
                                          TIMM_OSAL_U32 uArgc,
                                          TIMM_OSAL_PTR pArgv,
                                          TIMM_OSAL_U32 uStackSize,
                                          TIMM_OSAL_U32 uPriority,
                                          TIMM_OSAL_S8 *pName)
{

  TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_UNKNOWN;
  TIMM_OSAL_TASK *pHandle = TIMM_OSAL_NULL;
  struct sched_param sched;
  size_t stackSize;
  *pTask = TIMM_OSAL_NULL;

  /* Task structure allocation */
  pHandle =
    (TIMM_OSAL_TASK *) TIMM_OSAL_Malloc (sizeof (TIMM_OSAL_TASK), 0, 0, 0);
  if (pHandle == TIMM_OSAL_NULL)
  {
    bReturnStatus = TIMM_OSAL_ERR_ALLOC;
    goto EXIT;
  }

  /* Initial cleaning of the task structure */
  TIMM_OSAL_Memset ((TIMM_OSAL_PTR) pHandle, 0, sizeof (TIMM_OSAL_TASK));

  /* Arguments for task */
  pHandle->uArgc = uArgc;
  pHandle->pArgv = pArgv;
  pHandle->pThFunc = pFunc;
  pHandle->isCreated = TIMM_OSAL_FALSE;
  /* printf ("pArgv : 0x%x\n", pArgv); */

  if (0 != pthread_attr_init (&pHandle->ThreadAttr))
  {
    TIMM_OSAL_Error ("Task Init Attr Init failed!");
    goto EXIT;
  }
  /* It is not valid to set a scheduling priority under the default scheduling
  * policy (SCHED_OTHER).  Set the policy to SCHED_RR first.
  */
  if (0 != pthread_attr_setschedpolicy(&pHandle->ThreadAttr, SCHED_RR))
  {
   TIMM_OSAL_Error ("Task Init Set Sched Policy failed!");
   goto EXIT;
  }
  
  /* Updation of the priority and the stack size */

  if (0 != pthread_attr_getschedparam (&pHandle->ThreadAttr, &sched))
  {
    TIMM_OSAL_Error ("Task Init Get Sched Params failed!");
    goto EXIT;
  }

  sched.sched_priority = uPriority;     /* relative to the default priority */
  if (0 != pthread_attr_setschedparam (&pHandle->ThreadAttr, &sched))
  {
    TIMM_OSAL_Error ("Task Init Set Sched Paramsfailed!");
    goto EXIT;
  }

  /* First get the default stack size */
  if (0 != pthread_attr_getstacksize (&pHandle->ThreadAttr, &stackSize))
  {
    TIMM_OSAL_Error ("Task Init Set Stack Size failed!");
    goto EXIT;
  }

  /* Check if requested stack size is larger than the current default stack
     size */
  if (uStackSize > stackSize)
  {
    stackSize = uStackSize;
    if (0 != pthread_attr_setstacksize (&pHandle->ThreadAttr, stackSize))
    {
      TIMM_OSAL_Error ("Task Init Set Stack Size failed!");
      goto EXIT;
    }
  }

  if (0 !=
      pthread_create (&pHandle->threadID, &pHandle->ThreadAttr,
                      (TIMM_OSAL_START_THREAD_FCNPTR)
                      TIMM_OSAL_CreateTaskHelperFunc, pHandle))
  {
    TIMM_OSAL_Error ("Create_Task failed !");
    goto EXIT;
  }

  /* Task was successfully created */
  pHandle->isCreated = TIMM_OSAL_TRUE;
  *pTask = (TIMM_OSAL_PTR) pHandle;
  bReturnStatus = TIMM_OSAL_ERR_NONE;
    /**pTask = (TIMM_OSAL_PTR *)pHandle;*/

EXIT:
/*    if((TIMM_OSAL_ERR_NONE != bReturnStatus) && (TIMM_OSAL_NULL != pHandle)) {
       TIMM_OSAL_Free (pHandle->stackPtr);*/
  if ((TIMM_OSAL_ERR_NONE != bReturnStatus))
  {
    TIMM_OSAL_Free (pHandle);
  }
  return bReturnStatus;

}

/* ========================================================================== */
/**
* @fn TIMM_OSAL_Task_yield - yield to other tasks
*
*/
/* ========================================================================== */

TIMM_OSAL_ERRORTYPE TIMM_OSAL_Task_yield ()
{
  TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;

  int sched_ret_val;

  sched_ret_val = sched_yield ();       /* But yields all thread in the current 
                                           process */

  if (sched_ret_val != 0)
  {
    bReturnStatus = TIMM_OSAL_ERR_NOT_SUPPORTED;
  }

  return bReturnStatus;
}

/* ========================================================================== */
/**
* @fn TIMM_OSAL_SleepTask
*
* @see timm_osal_task.h file
*/
/* ========================================================================== */
TIMM_OSAL_ERRORTYPE TIMM_OSAL_SleepTask (TIMM_OSAL_U32 msec)
{

  usleep (msec * 1000);
  return TIMM_OSAL_ERR_NONE;
}

/* ========================================================================== */
/**
* @fn TIMM_OSAL_DeleteTask
*
* @see
*/
/* ========================================================================== */

TIMM_OSAL_ERRORTYPE TIMM_OSAL_DeleteTask (TIMM_OSAL_PTR pTask)
{
  TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_UNKNOWN;

  TIMM_OSAL_TASK *pHandle = (TIMM_OSAL_TASK *) pTask;
  void *retVal;

  if ((NULL == pHandle) || (TIMM_OSAL_TRUE != pHandle->isCreated))
  {
    /* this task was never created */
    bReturnStatus = TIMM_OSAL_ERR_PARAMETER;
    goto EXIT;
  }
  if (pthread_attr_destroy (&pHandle->ThreadAttr))
  {
    /* TIMM_OSAL_Error("Delete_Task failed !"); */
    goto EXIT;
  }
  if (pthread_join (pHandle->threadID, &retVal))
  {
    /* TIMM_OSAL_Error("Delete_Task failed !"); */
    goto EXIT;
    /* bReturnStatus = TIMM_OSAL_ERR_CREATE(TIMM_OSAL_ERR, TIMM_OSAL_COMP_TASK,
                                          status);*//*shm to be done */
  }
  bReturnStatus = TIMM_OSAL_ERR_NONE;
  TIMM_OSAL_Free (pHandle);
EXIT:
  return bReturnStatus;
}

/* ========================================================================== */
/**
* @fn TIMM_OSAL_ExitTask
*
* @see
*/
/* ========================================================================== */

TIMM_OSAL_ERRORTYPE TIMM_OSAL_ExitTask (TIMM_OSAL_PTR pTask, void *value_ptr)
{
  TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_UNKNOWN;

  TIMM_OSAL_TASK *pHandle = (TIMM_OSAL_TASK *) pTask;

  if ((NULL == pHandle) || (TIMM_OSAL_TRUE != pHandle->isCreated))
  {
    /* this task was never created */
    bReturnStatus = TIMM_OSAL_ERR_PARAMETER;
    goto EXIT;
  }
  if (pthread_attr_destroy (&pHandle->ThreadAttr))
  {
    /* TIMM_OSAL_Error("Delete_Task failed !"); */
    goto EXIT;
  }
  bReturnStatus = TIMM_OSAL_ERR_NONE;
  pthread_exit (value_ptr);
  /* pthread_exit does not return */
EXIT:
  return bReturnStatus;
}

/* Nothing beyond this point */
