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

/**
 *******************************************************************************
 *  @file  platform_c674.c
 *  @brief This file contains all Functions related to platform initialization
 *         and deinitialization
 *
 *  @rev 1.0
 *******************************************************************************
 */

/*******************************************************************************
*                             Compilation Control Switches
*******************************************************************************/
/* None */

/* -------------------- system and platform files ----------------------------*/
#include <xdc/std.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Diags.h>
#include <ti/timmosal/timm_osal_error.h>
#include <ti/timmosal/timm_osal_osal.h>
#include <ti/omx/interfaces/openMaxv11/OMX_Types.h>
#include <ti/omx/interfaces/openMaxv11/OMX_Core.h>
#include <ti/omx/domx/domx_util.h>
#include <ti/omx/memcfg/memcfg.h>

/*-------------------------program files -------------------------------------*/
/* None */

/*******************************************************************************
 * EXTERNAL REFERENCES NOTE : only use if not found in header file
*******************************************************************************/

/*-----------------------data declarations -----------------------------------*/
/* None */

/*--------------------- function prototypes ----------------------------------*/
/* None */

/*******************************************************************************
 * PUBLIC DECLARATIONS Defined here, used elsewhere
 ******************************************************************************/

/*---------------------data declarations -------------------------------------*/
/* None */

/*---------------------function prototypes -----------------------------------*/
void platform_init ();

void platform_deinit ();

/*******************************************************************************
 * PRIVATE DECLARATIONS Defined here, used only here
 ******************************************************************************/

/*--------------------------- macros  ----------------------------------------*/
/*None */

/** 
********************************************************************************
 *  @fn     platform_init
 *  @brief  Calls various initialization routines for different modules.
 *          
 *          Following modules are initialized. 
 *          OSAL, OMX core, MEMCFG
 *
 *  @return none
********************************************************************************
*/
void platform_init ()
{
  OMX_ERRORTYPE eError = OMX_ErrorUndefined;
  TIMM_OSAL_ERRORTYPE tStatus = TIMM_OSAL_ERR_NONE;


  /*--------------------------------------------------------------------------*/
  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
  /* Initialize the OSAL */
  /* */
  /*--------------------------------------------------------------------------*/
  Log_print0 (Diags_USER1,"in DSP platform Init: Calling TIMM_OSAL_Init");
  tStatus = TIMM_OSAL_Init ();
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((TIMM_OSAL_ERR_NONE == tStatus),
                                  "TIMM_OSAL_Init failed");
  Log_print0 (Diags_USER1,"in DSP platform Init: TIMM_OSAL_Init Success");

  /*--------------------------------------------------------------------------*/
  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
  /* Initialize the OMX core */
  /* */
  /*--------------------------------------------------------------------------*/
  Log_print0 (Diags_USER1,"in DSP platform Init: Calling OMX_Init");
  eError = OMX_Init ();
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((OMX_ErrorNone == eError), "OMX_Init failed");
  Log_print0 (Diags_USER1,"in DSP platform Init: OMX_Init Success");

  /*--------------------------------------------------------------------------*/
  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
  /* Initialize MEMCFG module */
  /* */
  /*--------------------------------------------------------------------------*/
  Log_print0 (Diags_USER1,"in DSP platform Init: Calling MEMCFG_ModuleInit");
  MEMCFG_ModuleInit ();
  DioFQCFG_ModuleInit ();
  Log_print0 (Diags_USER1,"in DSP platform Init: MEMCFG_ModuleInit Success");

  /*--------------------------------------------------------------------------*/
  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
  /* Initialize Codec Engine */
  /* */
  /*--------------------------------------------------------------------------*/
  Log_print0 (Diags_USER1,"in DSP platform Init: Calling CERuntime_init");
  CERuntime_init();
  Log_print0 (Diags_USER1,"in DSP platform Init: CERuntime_init Success");
}                               /* platform_init */

/** 
********************************************************************************
 *  @fn     platform_deinit
 *  @brief  Calls various de-initialization routines for different modules.
 *          
 *          Following modules are de-initialized. 
 *          OSAL, OMX core, MEMCFG
 *
 *  @return none
********************************************************************************
*/
void platform_deinit ()
{
  OMX_ERRORTYPE eError = OMX_ErrorUndefined;
  TIMM_OSAL_ERRORTYPE tStatus = TIMM_OSAL_ERR_NONE;

  /*--------------------------------------------------------------------------*/
  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
  /* DeInitialize Codec Engine */
  /* */
  /*--------------------------------------------------------------------------*/
  Log_print0 (Diags_USER1,"in DSP platform deInit: Calling CERuntime_exit");
  CERuntime_exit ();
  Log_print0 (Diags_USER1,"in DSP platform deInit: CERuntime_exit Success");

  /*--------------------------------------------------------------------------*/
  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
  /* DeInitialize MEMCFG */
  /* */
  /*--------------------------------------------------------------------------*/
  Log_print0 (Diags_USER1,"in DSP platform deInit: Calling MEMCFG_ModuleDeInit");
  MEMCFG_ModuleDeInit ();
  Log_print0 (Diags_USER1,"in DSP platform deInit: MEMCFG_ModuleDeInit Success");

  /*--------------------------------------------------------------------------*/
  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
  /* DeInitialize the OMX core */
  /* */
  /*--------------------------------------------------------------------------*/
  Log_print0 (Diags_USER1,"in DSP platform deInit: Calling OMX_Deinit");
  eError = OMX_Deinit ();
  Log_print0 (Diags_USER1,"in DSP platform deInit: OMX_Deinit Success");

  /*--------------------------------------------------------------------------*/
  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
  /* DeInitialize the OSAL */
  /* */
  /*--------------------------------------------------------------------------*/
  Log_print0 (Diags_USER1,"in DSP platform deInit: Calling TIMM_OSAL_Deinit");
  tStatus = TIMM_OSAL_Deinit ();
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((TIMM_OSAL_ERR_NONE == tStatus),
                                  "TIMM_OSAL_Deinit failed");
  Log_print0 (Diags_USER1,"in DSP platform deInit: TIMM_OSAL_Deinit Success");
}                               /* platform_deinit */

/* Nothing beyond this point */
