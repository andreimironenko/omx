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
 *  @file  main.c
 *  @brief This file contains platform (A8) specific initializations and 
 *         the main () of the test application.
 *
 *  @rev 1.0
 *******************************************************************************
 */

/*******************************************************************************
*                             Compilation Control Switches
*******************************************************************************/
/*None*/

/*******************************************************************************
*                             INCLUDE FILES
*******************************************************************************/

/* -------------------- system and platform files ----------------------------*/
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <stdio.h>
#include <xdc/runtime/knl/Thread.h>
/*-------------------------program files --------------------------------------*/
#include "ti/omx/interfaces/openMaxv11/OMX_Core.h"
#include "ti/omx/interfaces/openMaxv11/OMX_Component.h"
#include "ilclient.h"
#include "ilclient_utils.h"
/*#include "msgq.h"*/

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
/* None */

/*******************************************************************************
 * PRIVATE DECLARATIONS Defined here, used only here
 ******************************************************************************/

/*--------------------------- macros  ----------------------------------------*/
/* None */

/*---------------------- function prototypes ---------------------------------*/
/* None */

/** 
********************************************************************************
 *  @fn     main
 *  @brief  This function does the platform specific initialization. It then 
 *          calls the DSP Copy Example IL Client function. Finally, it performs
 *          platform specific de-initializations                               
 * 
 *  @param[in ]  arg1  : Not used, Reserved for future use
 *  @param[in ]  arg2  : Not used, Reserved for future use
 * 
 *  @returns none 
********************************************************************************
*/

int main (int argc, char **argv)
{
  /*ConfigureUIA uiaCfg;*/
  printf (" Vlpb-Copy example \n");
  printf ("===============================\n");

  printf ("\nStarting threads\n"); 
  fflush (stdout);
  
 /* Initializing OMX core , functions releated to platform specific
    initialization could be placed inside this */
  
  OMX_Init ();

  /* UIA is an utility to get the logs from firmware on Linux terminal, and also
     to use System Analyzer, Following UIA configuration is required to change 
     the default debug logging configuration, and is optional  */

  /* Configuring logging options on slave cores */
  /* can be 0 or 1 */
  /*uiaCfg.enableAnalysisEvents = 0;*/
  /* can be 0 or 1 */
  /*uiaCfg.enableStatusLogger = 1;*/
  /* can be OMX_DEBUG_LEVEL1|2|3|4|5 */
  /*uiaCfg.debugLevel = OMX_DEBUG_LEVEL1;*/
  /* configureUiaLoggerClient( COREID, &Cfg); */
  /*configureUiaLoggerClient(0, &uiaCfg);*/

 /* OMX IL client for decoder component */
  Vlpb_Copy_Example ();

  OMX_Deinit();
  
  exit (0);
} /* main */

/* main_ca8.c - EOF*/
