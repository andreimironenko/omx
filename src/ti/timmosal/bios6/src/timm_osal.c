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
*   @file  tiimm_osal.c
*   This file contains methods that provides the functionality 
*   initializing/deinitializing the osal.
*
*  @path \
*
*/
/* -------------------------------------------------------------------------- */
/* =========================================================================
 *!
 *! Revision History
 *! ===================================
 *! 8/15/2009 Narendrna M R: narendranmr@ti.com. STM trace integration
 *! 11-Mar-2009 Aditya Monga: admonga@ti.com Cleanup activity
 *! 23-May-2008 Gaurav Aradhya: gaurav.a@ti.com Ported to DSP BIOS 
 *! 07-May-2008 Rabbani Patan: rabbani@ti.com updated version 
 *! 0.1: Created the first draft version, ksrini@ti.com  
 * ========================================================================= */

/******************************************************************************
* Includes
******************************************************************************/
#include "timm_osal_types.h"
#include "timm_osal_error.h"
#include "timm_osal_memory.h"
#include "timm_osal_trace.h"
#include "../../StmLibrary/StmLibrary.h"
#include "../../StmLibrary/STMHelper.h"

#ifdef _DEBUG
#include <stdio.h>
#endif

STMHandle *phandle;
int iChannel = 0;
STMXport_callback pSTMCallBack;
unsigned int major_version = 0, minor_version = 0;
TIMM_OSAL_U32 ulEnabledTraceGroups;

typedef enum STMMode { blocking, buffered, CIO } STMMode;

#if defined(_STM) || ( defined(_COMPACT) && !defined(_CIO))
STMMode STMstartUpMode = blocking;
#endif
#ifdef _CIO
STMMode STMstartUpMode = CIO;
#endif

//#include <ti/sysbios/gates/GateMutexPri.h>

/******************************************************************************
* Function Prototypes
******************************************************************************/

/****************************************************************
*  PRIVATE DECLARATIONS  : only used in this file
****************************************************************/
/*--------data declarations -----------------------------------*/
static TIMM_OSAL_BOOL bOSALInitialized = TIMM_OSAL_FALSE;


/* Common OSAL gate - will be used by all OSAL modules when accesing 
   any common data */
//GateMutexPri_Handle gOSALgate = NULL;
/* ========================================================================== */
/**
* @fn TIMM_OSAL_Init function initilize the osal with initial settings.
*
* @return  TIMM_OSAL_ERR_NONE if successful
*               !TIMM_OSAL_ERR_NONE if an error occurs
*/
/* ========================================================================== */

TIMM_OSAL_ERRORTYPE TIMM_OSAL_Init (void)
{
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;
    //GateMutexPri_Params params;
    
    if (bOSALInitialized == TIMM_OSAL_TRUE) 
	{
        TIMM_OSAL_Warning("OSAL has already been initialized!!!\n");
        goto EXIT;
    }

    /*  
     *  STM lib initialization 
     *  */  
    if ( blocking == STMstartUpMode ) {
		// This function opens the STM Library API for blocking IO with callbacks disabled
		STMLog_EnableBlkMode();
	} 
	else if (buffered == STMstartUpMode) {
		// This functions opens the STM Library API for bufferred IO with callbacks set to STMLog_CallBack
		unsigned int maxHeapAllowed = 1024*1024;
		int dmaCh_Msg = 0;
		int dmaCh_Ts = 1;
		STMLog_EnableBufMode(maxHeapAllowed, dmaCh_Msg, dmaCh_Ts, STMLog_CallBack );
    } 
	else  { //Must be CIO
		STMLog_EnableCIOMode();	
	}

	STMXport_getVersion(phandle, &major_version, &minor_version);

    //sample printf to stm port
    STMLog_checkFlush();
	STMXport_printf(phandle, 0, NULL, "Sample STM print - no varables\0");
	STMLog_checkFlush();

    //enable all trace groups at bootup
    ulEnabledTraceGroups = 0xFFFFFFFF;
	
    /* Initialize the Memory Pool */
    bReturnStatus = TIMM_OSAL_CreateMemoryPool ();
    if (TIMM_OSAL_ERR_NONE != bReturnStatus) {
        TIMM_OSAL_Error ("TIMM_OSAL_CreateMemoryPool failed!!!");
        goto EXIT;
    }

	//gOSALgate = GateMutexPri_create( &params, NULL ); 

    bOSALInitialized = TIMM_OSAL_TRUE;
    
EXIT:
    return bReturnStatus;
}



/* ========================================================================== */
/**
* @fn TIMM_OSAL_Init function de-initilize the osal.
*
* @return  TIMM_OSAL_ERR_NONE if successful
*               !TIMM_OSAL_ERR_NONE if an error occurs
*/
/* ========================================================================== */

TIMM_OSAL_ERRORTYPE TIMM_OSAL_Deinit (void)
{
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;

    /* Delete the Memory Pool */
	/*
    bReturnStatus = TIMM_OSAL_DeleteMemoryPool ();

    if (TIMM_OSAL_ERR_NONE != bReturnStatus) {
        TIMM_OSAL_Error ("TIMM_OSAL_DeleteMemoryPool failed!!!");
        goto EXIT;
    }
    */
#if defined(_STM) 
    STMLog_exit();
#endif

	//GateMutexPri_delete(&gOSALgate);
    bOSALInitialized = TIMM_OSAL_FALSE;

//EXIT:    
    return bReturnStatus;
}


