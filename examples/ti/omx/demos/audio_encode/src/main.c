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
#include <xdc/std.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/IHeap.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Timestamp.h>
#include <xdc/cfg/global.h>
#include <ti/ipc/Ipc.h>
#include <ti/omx/omxutils/omx_utils.h>
#include <xdc/runtime/knl/Thread.h>

#include "ilclient_utils.h"
#include<signal.h>
/*#include "msgq.h"*/

/*-------------------------program files --------------------------------------*/
/* None */

/*******************************************************************************
 * EXTERNAL REFERENCES NOTE : only use if not found in header file
*******************************************************************************/
/*--------------------- function prototypes ----------------------------------*/
extern Int OMX_Audio_Encode_Test(char *infileName, char *outFileName,
                                 char *format, int nChannel, int bitrate,
                                 int samperate, char *outputFormat);

extern void SignalProcess(int seg);

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

typedef void (*sighandler_t) (int);

/*---------------------- function prototypes ---------------------------------*/
/* None */

/**
********************************************************************************
 *  @fn     main 
 *  @brief  This function does the platform specific initialization and calls .
 *          the application specific test function OMX_Audio_Encode_Test()   
 *  @param[in ]  arg1  : input stream file
 *  @param[in ]  arg2  : encoder type 0:AAC]
 *
 *  @returns none
********************************************************************************
*/

int main(int argc, char **argv)
{
    unsigned int eError;
    /*ConfigureUIA uiaCfg;*/

    IL_ARGS args;

    (void) signal(SIGINT, (sighandler_t) SignalProcess);

    parse_args(argc, argv, &args);

    printf("===============================\n");
    printf("Audio Encoder Example \n");
    fflush(stdout);
    eError = OMX_Init();

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

    OMX_Audio_Encode_Test(args.input_file, args.output_file, args.codec,
                          args.no_channels, args.bitrate, args.samplerate,
                          args.format);
    eError = OMX_Deinit();
    printf("Done!\n");
    fflush(stdout);
    return (0);

}                               /* main */

/* main.c - EOF */
