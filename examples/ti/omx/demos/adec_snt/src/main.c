/*******************************************************************************
 * OMX mm stack - OMX Core, Components and IL client                           *
 *                                                                             *
 * "OMX mm stack " is a software module developed on TI's DM class of SOCs.    *
 * These modules are based on Khronos open max 1.1.2 specification.            *
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 ******************************************************************************/

/**
 *******************************************************************************
 *  @file  main.c
 *  @brief This file contains platform (DSP) specific initializatins and
 *         the main () of the test application.
 *
 *  @rev 1.0
 *******************************************************************************
 */


/*******************************************************************************
*                             Compilation Control Switches
*******************************************************************************/
/* None */

/*******************************************************************************
*                             INCLUDE FILES
*******************************************************************************/

/* -------------------- system and platform files ----------------------------*/
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <xdc/std.h>
#include <xdc/runtime/knl/Thread.h>

/*-------------------------program files --------------------------------------*/
#include <OMX_Core.h>
#include "memcfg.h"
#include "ilclient_utils.h"
/*#include "msgq.h"*/

/*******************************************************************************
 * EXTERNAL REFERENCES NOTE : only use if not found in header file
*******************************************************************************/
/*--------------------- function prototypes ----------------------------------*/
extern Int OMX_Audio_Decode_Test (char *inFileName, char *outFileName, char *format, 
								  int aacRawFormat, int aacRawSampleRate);

/*******************************************************************************
 * PUBLIC DECLARATIONS Defined here, used elsewhere
 ******************************************************************************/

/*---------------------data declarations -------------------------------------*/
/* None */

/*---------------------function prototypes -----------------------------------*/


/*******************************************************************************
 * PRIVATE DECLARATIONS Defined here, used only here
 ******************************************************************************/

/*--------------------------- macros  ----------------------------------------*/



/*---------------------- function prototypes ---------------------------------*/
/* None */

/**
********************************************************************************
 *  @fn     main 
 *  @brief  This function does the platform specific initialization and calls .
 *          the application specific test function OMX_Audio_Decode_Test()   
 *  @param[in ]  arg1  : input stream file
 *  @param[in ]  arg2  : output stream file
 *  @param[in ]  arg3  : decoder type 0:MP3 1:AAC]
 *  @param[in ]  arg4  : AAC stream format type 0:RAW 1:ADTS/ADIF]
 *  @param[in ]  arg5  : AAC RAW Source Sample Rate]
 *
 *  @returns none
********************************************************************************
*/
int main (int argc, char *argv[])
{
  //unsigned int eError;
  /*ConfigureUIA uiaCfg;*/
  IL_ARGS args;

  parse_args (argc, argv, &args);
  
  
  printf ("Audio Decoder example \n");
  printf ("===============================\n");


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
  OMX_Audio_Decode_Test( args.input_file, args.output_file, args.codec,
	  atoi(args.rawFormat), atoi(args.sampleRate));

  OMX_Deinit();


  exit (0);

}/* main */

/* main.c - EOF */
