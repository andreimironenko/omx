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
 *! 11-Mar-2009 Aditya Monga: admonga@ti.com Cleanup activity
 *! 23-May-2008 Gaurav Aradhya: gaurav.a@ti.com Ported to DSP BIOS 
 *! 07-May-2008 Rabbani Patan: rabbani@ti.com updated version 
 *! 0.1: Created the first draft version, ksrini@ti.com  
 * ========================================================================= */

/******************************************************************************
* Includes
******************************************************************************/
#include <string.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/heaps/HeapMem.h>

#include "timm_osal_types.h"
#include "timm_osal_error.h"
#include "timm_osal_memory.h"
#include "timm_osal_task.h"
#include "timm_osal_trace.h"
#include "../inc/timm_osal_internal.h"

#define TASK_NAME_SIZE 128
#define TASK_MIN_WAIT_TIME 8
#define TASK_MAX_WAIT_TIME (TASK_MIN_WAIT_TIME << 8)

/**
* TIMM_OSAL_TASK describe the different task information
*/
typedef struct TIMM_OSAL_TASK 
{
    /** Handle to the task context structure */
    Task_Handle   task;
    /** Name of the task */
    TIMM_OSAL_S8  name[TASK_NAME_SIZE + 1]; 
    /** Pointer to the task stack memory */
    TIMM_OSAL_PTR stackPtr;
    /** Size of the task stack */
    TIMM_OSAL_S32 stackSize;
    /** task priority */
    TIMM_OSAL_S32 priority;
    /** flag to check if task got created */
    TIMM_OSAL_BOOL isCreated;
} TIMM_OSAL_TASK;

//extern Task_Instance myTskInst;
/******************************************************************************
* Function Prototypes
******************************************************************************/


/* ========================================================================== */
/**
* @fn TIMM_OSAL_CreateTask function
*
* @see timm_osal_task.h file
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
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;
 
    TIMM_OSAL_TASK *pHandle = TIMM_OSAL_NULL;
    Task_Params  taskParams; 

    *pTask = TIMM_OSAL_NULL;
    /*Note: -1 is a valid priority in Sys BIOS */
    
    if ( (pFunc == NULL) || ( ( uPriority > 31 ) && (uPriority != (TIMM_OSAL_U32)-1) ) )
    {
       bReturnStatus = TIMM_OSAL_ERR_PARAMETER;	
	   goto EXIT;
    }
    
    pHandle = (TIMM_OSAL_TASK *) TIMM_OSAL_Malloc(sizeof(TIMM_OSAL_TASK), 
		                                         TIMM_OSAL_TRUE, 0,
                                                 TIMMOSAL_MEM_SEGMENT_EXT);

    if(TIMM_OSAL_NULL == pHandle)
    {
        bReturnStatus = TIMM_OSAL_ERR_ALLOC;
		goto EXIT;
    }

    /* Initial cleaning of the task structure */
    TIMM_OSAL_Memset((TIMM_OSAL_PTR)pHandle, 0, sizeof (TIMM_OSAL_TASK));

    pHandle->isCreated = TIMM_OSAL_FALSE;

	strncpy((TIMM_OSAL_CHAR *)pHandle->name, (TIMM_OSAL_CHAR *)pName, TASK_NAME_SIZE);
    pHandle->name[TASK_NAME_SIZE] = '\000';
    pHandle->stackSize = uStackSize;
    pHandle->priority  = uPriority;

    /* Allocate memory for task stack */
    pHandle->stackPtr = (TIMM_OSAL_PTR *)TIMM_OSAL_Malloc(pHandle->stackSize, 
                                                   TIMM_OSAL_TRUE, 0, 
                                                   TIMMOSAL_MEM_SEGMENT_EXT);
    if (TIMM_OSAL_NULL == pHandle->stackPtr) 
	{
        TIMM_OSAL_Error("TIMM_OSAL_Malloc failed during task stack allocation");
        bReturnStatus = TIMM_OSAL_ERR_ALLOC;
        goto EXIT;
    }
    TIMM_OSAL_Memset(pHandle->stackPtr, 0xFE, pHandle->stackSize);

    Task_Params_init(&taskParams);

    taskParams.arg0 = uArgc;
    taskParams.arg1 = (TIMM_OSAL_U32)pArgv;
    taskParams.priority = uPriority;
    taskParams.stack = pHandle->stackPtr;
    taskParams.stackSize = uStackSize;
	taskParams.instance->name = (xdc_String)pHandle->name;
     
    /* Create the task */
    pHandle->task = Task_create((Task_FuncPtr)pFunc, &taskParams, NULL);

    if (pHandle->task == NULL) 
	{
        bReturnStatus = (TIMM_OSAL_ERRORTYPE)TIMM_OSAL_ERR_CREATE
			                      (TIMM_OSAL_ERR, TIMM_OSAL_COMP_TASK, FALSE);
        goto EXIT;
    }

    /* Task was successfully created */
    pHandle->isCreated = TIMM_OSAL_TRUE;
    
    *pTask = (TIMM_OSAL_PTR *)pHandle;

EXIT:
    if((TIMM_OSAL_ERR_NONE != bReturnStatus) && (TIMM_OSAL_NULL != pHandle)) 
	{
		if(TIMM_OSAL_NULL != pHandle->stackPtr) 
            TIMM_OSAL_Free (pHandle->stackPtr);
        TIMM_OSAL_Free (pHandle);
    }
    return bReturnStatus;
}


/* ========================================================================== */
/**
* @fn TIMM_OSAL_CreateTask function
*
* @see timm_osal_task.h file
*/
/* ========================================================================== */

TIMM_OSAL_ERRORTYPE TIMM_OSAL_CreateTaskExtn (TIMM_OSAL_PTR *pTask,
                                              TIMM_OSAL_TaskProc pFunc,
                                              TIMM_OSAL_U32 uArgc,
                                              TIMM_OSAL_PTR pArgv,
                                              TIMM_OSAL_U32 uStackSize,
                                              TIMM_OSAL_U32 uPriority,
                                              TIMM_OSAL_S8 *pName,
                                              TIMM_OSAL_PTR hStackHeapHandle)
{
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;
 
    TIMM_OSAL_TASK *pHandle = TIMM_OSAL_NULL;
    Task_Params  taskParams; 

    *pTask = TIMM_OSAL_NULL;
    
    if((pFunc == NULL)|| (uPriority > 31))
    {
       bReturnStatus = TIMM_OSAL_ERR_PARAMETER;	
	   goto EXIT;
    }
    
    pHandle = (TIMM_OSAL_TASK *) TIMM_OSAL_Malloc(sizeof(TIMM_OSAL_TASK), 
		                                         TIMM_OSAL_TRUE, 0,
                                                 TIMMOSAL_MEM_SEGMENT_EXT);

    if(TIMM_OSAL_NULL == pHandle)
    {
        bReturnStatus = TIMM_OSAL_ERR_ALLOC;
		goto EXIT;
    }

    /* Initial cleaning of the task structure */
    TIMM_OSAL_Memset((TIMM_OSAL_PTR)pHandle, 0, sizeof (TIMM_OSAL_TASK));

    pHandle->isCreated = TIMM_OSAL_FALSE;

	strncpy((TIMM_OSAL_CHAR *)pHandle->name, (TIMM_OSAL_CHAR *)pName, TASK_NAME_SIZE);
    pHandle->name[TASK_NAME_SIZE] = '\000';
    pHandle->stackSize = uStackSize;
    pHandle->priority  = uPriority;

    /* Allocate memory for task stack */
    pHandle->stackPtr = (TIMM_OSAL_PTR *)TIMM_OSAL_MallocExtn(pHandle->stackSize, 
                                                   TIMM_OSAL_TRUE, 0, 
                                                   TIMMOSAL_MEM_SEGMENT_EXT,
                                                   hStackHeapHandle);
    if (TIMM_OSAL_NULL == pHandle->stackPtr) 
	{
        TIMM_OSAL_Error("TIMM_OSAL_Malloc failed during task stack allocation");
        bReturnStatus = TIMM_OSAL_ERR_ALLOC;
        goto EXIT;
    }
    TIMM_OSAL_Memset(pHandle->stackPtr, 0xFE, pHandle->stackSize);

    Task_Params_init(&taskParams);

    taskParams.arg0 = uArgc;
    taskParams.arg1 = (TIMM_OSAL_U32)pArgv;
    taskParams.priority = uPriority;
    taskParams.stack = pHandle->stackPtr;
    taskParams.stackSize = uStackSize;
	taskParams.instance->name = (xdc_String)pHandle->name;
     
    /* Create the task */
    pHandle->task = Task_create((Task_FuncPtr)pFunc, &taskParams, NULL);

    if (pHandle->task == NULL) 
	{
        bReturnStatus = (TIMM_OSAL_ERRORTYPE)TIMM_OSAL_ERR_CREATE
			                      (TIMM_OSAL_ERR, TIMM_OSAL_COMP_TASK, FALSE);
        goto EXIT;
    }

    /* Task was successfully created */
    pHandle->isCreated = TIMM_OSAL_TRUE;
    
    *pTask = (TIMM_OSAL_PTR *)pHandle;

EXIT:
    if((TIMM_OSAL_ERR_NONE != bReturnStatus) && (TIMM_OSAL_NULL != pHandle)) 
	{
		if(TIMM_OSAL_NULL != pHandle->stackPtr) 
            TIMM_OSAL_Free (pHandle->stackPtr);
        TIMM_OSAL_Free (pHandle);
    }
    return bReturnStatus;
}


/* ========================================================================== */
/**
* @fn TIMM_OSAL_DeleteTask
*
* @see timm_osal_task.h file
*/
/* ========================================================================== */

TIMM_OSAL_ERRORTYPE TIMM_OSAL_DeleteTask (TIMM_OSAL_PTR pTask)
{
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;
    TIMM_OSAL_TASK *pHandle = (TIMM_OSAL_TASK *)pTask;
    TIMM_OSAL_U32 uSleepTime = TASK_MIN_WAIT_TIME;

    if (TIMM_OSAL_NULL == pHandle) 
	{
		bReturnStatus = TIMM_OSAL_ERR_PARAMETER;
        goto EXIT;
    }

    if (pHandle->isCreated != TIMM_OSAL_TRUE) 
	{
		bReturnStatus = TIMM_OSAL_ERR_UNKNOWN;
        goto EXIT;
    }
    /*Check the status of the Task*/
    while ( Task_Mode_TERMINATED != Task_getMode ( pHandle->task) ) {
      TIMM_OSAL_SleepTask ( uSleepTime );
      uSleepTime <<= 1;
      if ( uSleepTime >= TASK_MAX_WAIT_TIME) {
        bReturnStatus = TIMM_OSAL_ERR_UNKNOWN;
        goto EXIT;
      }
    }
    Task_delete ( &(pHandle->task) );

    TIMM_OSAL_Free (pHandle->stackPtr);
    TIMM_OSAL_Free (pHandle);
EXIT:
    return bReturnStatus;
}



/* ========================================================================== */
/**
* @fn TIMM_OSAL_ExitTask
*
* @see timm_osal_task.h file
*/
/* ========================================================================== */

TIMM_OSAL_ERRORTYPE TIMM_OSAL_ExitTask (TIMM_OSAL_PTR pTask, void *value_ptr)
{
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;
    TIMM_OSAL_TASK *pHandle = (TIMM_OSAL_TASK *)pTask;

    if (TIMM_OSAL_NULL == pHandle) 
	{
		bReturnStatus = TIMM_OSAL_ERR_PARAMETER;
        goto EXIT;
    }

    if (pHandle->isCreated != TIMM_OSAL_TRUE) 
	{
		bReturnStatus = TIMM_OSAL_ERR_UNKNOWN;
        goto EXIT;
    }
    Task_exit (/*&(pHandle->task)*/);

EXIT:
    return bReturnStatus;
}

/* ========================================================================== */
/**
* @fn TIMM_OSAL_Task_newpri
*
* @see timm_osal_task.h file
*/
/* ========================================================================== */

TIMM_OSAL_ERRORTYPE TIMM_OSAL_Task_newpri ( TIMM_OSAL_PTR pTask, 
                                            TIMM_OSAL_S32 sNewPriority )
{
  TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;
  TIMM_OSAL_TASK *pHandle = (TIMM_OSAL_TASK *)pTask;

  if ( ( TIMM_OSAL_NULL == pHandle ) || ( 0 == sNewPriority ) || 
       ( sNewPriority >= Task_numPriorities ) ) 
  {
      bReturnStatus = TIMM_OSAL_ERR_PARAMETER;
      goto EXIT;
  }

  if (pHandle->isCreated != TIMM_OSAL_TRUE) 
 {
      bReturnStatus = TIMM_OSAL_ERR_UNKNOWN;
      goto EXIT;
  }

  if ( Task_Mode_TERMINATED == Task_getMode ( pHandle->task) )
  {
		bReturnStatus = TIMM_OSAL_ERR_UNKNOWN;
        goto EXIT;
    }

  Task_setPri ( pHandle->task, (Int) sNewPriority );

EXIT:
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
  
  Task_yield(); /* But yields all thread in the current process */

  return bReturnStatus ;
}

/* ========================================================================== */
/**
* @fn TIMM_OSAL_SleepTask
*
* @see timm_osal_task.h file
*/
/* ========================================================================== */
TIMM_OSAL_ERRORTYPE TIMM_OSAL_SleepTask (TIMM_OSAL_U32 mSec)
{
    TIMM_OSAL_U32 ticks = 0;

	ticks = _TIMM_OSAL_GetTicks(mSec);
    Task_sleep(ticks);
    
    return TIMM_OSAL_ERR_NONE;
}
