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
 *  @file  configureUiaLogger.c
 *  @brief This file contains the functions required to support UIA on DSP
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
#include <xdc/std.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/Main.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/System.h>
#include <ti/uia/runtime/LoggerCircBuf.h>
#include <ti/uia/runtime/LoggerSM.h>
#include "msgq.h"
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/Error.h>
#include <ti/sysbios/family/arm/m3/Hwi.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/uia/events/UIAErr.h>
#include <xdc/cfg/global.h>

/*-------------------------program files --------------------------------------*/
/* None */

/*******************************************************************************
 * EXTERNAL REFERENCES NOTE : only use if not found in header file
*******************************************************************************/

/*-----------------------data declarations -----------------------------------*/
int numErrors = 0;
/*--------------------- function prototypes ----------------------------------*/
void myExceptionHook(Hwi_ExcContext *excCtx);
void myErrorHook(Error_Block *eb);
void configureUiaLogger(Types_ModuleId moduleIdToDisplayOnConsole,
                        Bool enableAnalysisEvents,
                        Int debugLevel,
                        Bool enableStatusLogger);

/*******************************************************************************
 * PUBLIC DECLARATIONS Defined here, used elsewhere
 ******************************************************************************/
/* None */

/*******************************************************************************
 * PRIVATE DECLARATIONS Defined here, used only here
 ******************************************************************************/
/* None */

/*--------------------------- macros  ----------------------------------------*/
/* None */

/** 
********************************************************************************
 *  @fn     configureUiaLogger
 *  @brief  This function configures the Main logger to route events for the 
 *          specified module to the Linux console
 * 
 *  @param[in ]  moduleIdToDisplayOnConsole  : the module ID of the module whose
 *    events are to be routed to the Linux console
 *    -1 = to turn off logging of events other than status events
 *    Main_Module_id() returns the module ID for the main module
 *    Load_Module_id() returns the module ID for the load module
 *  @param[in ]  enableAnalysisEvents  : specifies whether analysis events 
 *    (e.g. UIABenchmark events) are to be enabled or not
 *  @param[in ]  enableUser1Events: specifies whether events with a Diags_USER1
 *    diags mask are to be enabled or not
 *  @param[in ]  enableStatusLogger: used to configure whether the status
 *    logger is enabled or disabled
 * 
 *  @returns none 
********************************************************************************
*/
void configureUiaLogger(Types_ModuleId moduleIdToDisplayOnConsole, 
                        Bool enableAnalysisEvents,
                        Int  debugLevel,
                        Bool enableStatusLogger)
{
 LoggerCircBuf_setModuleIdToRouteToStatusLogger(moduleIdToDisplayOnConsole);
 if ((LoggerCircBuf_statusLogger != NULL)){
 if (enableStatusLogger){
  Log_print0(Diags_USER1, "Enabling Status Logger");
  LoggerSM_enable((ti_uia_runtime_LoggerSM_Handle)LoggerCircBuf_statusLogger);
 } 
 else
 {
  Log_print0(Diags_USER1, "Disabling Status Logger");
  LoggerSM_disable((ti_uia_runtime_LoggerSM_Handle)LoggerCircBuf_statusLogger);
 }
 }
 if (debugLevel & OMX_DEBUG_LEVEL1){
   Log_print0(Diags_USER1, "Enabling Level1 Logs");
   Diags_setMask("xdc.runtime.Main+1");
 } else {
   Log_print0(Diags_USER1, "Disabling Level1 Logs");
   Diags_setMask("xdc.runtime.Main-1");
 }
 if (debugLevel & OMX_DEBUG_LEVEL2){
   Log_print0(Diags_USER1, "Enabling Level2 Logs");
   Diags_setMask("xdc.runtime.Main+2");
 } else {
   Log_print0(Diags_USER1, "Disabling Level2 Logs");
   Diags_setMask("xdc.runtime.Main-2");
 }
 if (debugLevel & OMX_DEBUG_LEVEL3){
   Log_print0(Diags_USER1, "Enabling Level3 Logs");
   Diags_setMask("xdc.runtime.Main+3");
 } else {
   Log_print0(Diags_USER1, "Disabling Level3 Logs");
   Diags_setMask("xdc.runtime.Main-3");
 }
 if (debugLevel & OMX_DEBUG_LEVEL4){
   Log_print0(Diags_USER1, "Enabling Level4 Logs");
   Diags_setMask("xdc.runtime.Main+4");
 } else {
   Log_print0(Diags_USER1, "Disabling Level4 Logs");
   Diags_setMask("xdc.runtime.Main-4");
 }
 if (debugLevel & OMX_DEBUG_LEVEL5){
   Log_print0(Diags_USER1, "Enabling Level5 Logs");
   Diags_setMask("xdc.runtime.Main+EXFIL5678");
 } else {
   Log_print0(Diags_USER1, "Disabling Level5 Logs");
   Diags_setMask("xdc.runtime.Main-EXFIL5678");
 }
 if (enableAnalysisEvents){
   Log_print0(Diags_USER1, "Enabling Analysis Logs");
   Diags_setMask("xdc.runtime.Main+Z");
 } else {
   Log_print0(Diags_USER1, "Disabling Analysis Logs");
   Diags_setMask("xdc.runtime.Main-Z");
 }
}

/** 
********************************************************************************
 *  @fn     uiaServerTask
 *  @brief  This is the task that runs on the DSP to implement a simple
 *          MessageQ to configures various Diags masks on DSP. Through this
 *          the A8 can set appropriate diags masks on DSP
 * 
 *  @param[in ]  arg1  : Not used, Reserved for future use
 *  @param[in ]  arg2  : Not used, Reserved for future use
 * 
 *  @returns none 
********************************************************************************
*/
Void uiaServerTask(UArg arg0, UArg arg1)
{
    MessageQ_Handle  serverMessageQ = NULL;
    MessageQ_QueueId replyQueue;
    MessageQ_Msg     msg;
    UInt16           msgId;
    Int              status;
    Int              enableAnalysisEvents = FALSE;
    Int              debugLevel = FALSE;
    Int              enableStatusLogger = FALSE;

#ifdef _LOCAL_CORE_c6xdsp_
    serverMessageQ = MessageQ_create(DSPSERVERNAME, NULL);
#endif    
#ifdef _LOCAL_CORE_m3vpss_
    serverMessageQ = MessageQ_create(VPSSM3SERVERNAME, NULL);
#endif    
#if _LOCAL_CORE_m3video_
    serverMessageQ = MessageQ_create(VIDEOM3SERVERNAME, NULL);
#endif    
    if(serverMessageQ == NULL)
     {
       Log_print0 (Diags_USER1, "serverMessageQ is NULL");
       return;
     }

    /* Loop forever processing requests */
    Log_print0 (Diags_USER1,"Server is ready to start processing commands from client.");
    while (TRUE) {
    
        /* Wait for a request. */
        status = MessageQ_get(serverMessageQ, &msg, MessageQ_FOREVER);
        if (status < 0) {
            Log_print0 (Diags_USER1,"MessageQ_get failed");
            System_abort("MessageQ_get failed\n");
        }

        /* Get the id and increment it to send back as validation */
        msgId = MessageQ_getMsgId(msg);
        if (msgId == UIA_CONFIGURE_CMD){
            Log_print0 (Diags_USER1,"Server received UIA_CONFIGURE_CMD");
            enableAnalysisEvents = ((ConfigureUIA *)msg)->enableAnalysisEvents;
            debugLevel = ((ConfigureUIA *)msg)->debugLevel;
            enableStatusLogger = ((ConfigureUIA *)msg)->enableStatusLogger;
            configureUiaLogger(Main_Module_id(), 
                               enableAnalysisEvents, debugLevel,
                               enableStatusLogger);

            msgId = UIA_CONFIGURE_ACK;

        } else {
         msgId = NACK;
        }
        MessageQ_setMsgId(msg, msgId);

        /* Use the embedded reply destination */
        replyQueue = MessageQ_getReplyQueue(msg);

        /* Send the response back */
        status = MessageQ_put(replyQueue, msg);
        if (status < 0) {            
            Log_print0 (Diags_USER1,"MessageQ_put was not successful");
            System_abort("MessageQ_put was not successful\n");
        }
    }
}

/** 
********************************************************************************
 *  @fn     myExceptionHook
 *  @brief  This function directs the exception message to Logger buffers
 *          so that they can be retrieved on A8
 * 
 *          By default, when an exception occurs, an ExcContext structure is 
 *          allocated on the ISR stack and filled in within the exception 
 *          handler. If excContextBuffer is initialized by the user,
 *          the ExcContext structure will be placed at that address instead.
 *          The buffer must be large enough to contain an ExcContext structure.
 *
 *          By default, when an exception occurs, a pointer to the base address
 *          of the stack being used by the thread causing the exception is 
 *          used. If excStackBuffer is initialized by the user, the stack 
 *          contents of the thread causing the exception will be copied to
 *          that address instead. The buffer must be large enough to contain 
 *          the largest task stack or ISR stack defined in the application.
 * 
 *  @returns none 
********************************************************************************
*/
Void myExceptionHook(Hwi_ExcContext *excCtx){
  int myErrorCode = 1;

  Log_write2(UIAErr_fatalWithStr,myErrorCode,(IArg)"Unhandled Exception:");
  if (excCtx->threadType == BIOS_ThreadType_Hwi){
    Log_print0(Diags_USER1|Diags_CRITICAL,"Exception occurred in ThreadType_HWI");
  } else if (excCtx->threadType == BIOS_ThreadType_Swi){
    Log_print0(Diags_USER1|Diags_CRITICAL,"Exception occurred in ThreadType_SWI");
  } else if (excCtx->threadType == BIOS_ThreadType_Task){
    Log_print0(Diags_USER1|Diags_CRITICAL,"Exception occurred in ThreadType_Task");
  } else if (excCtx->threadType == BIOS_ThreadType_Main){
    Log_print0(Diags_USER1|Diags_CRITICAL,"Exception occurred in ThreadType_Main");
  }

  Log_print1(Diags_USER1|Diags_CRITICAL,"handle: 0x%x.\n", (xdc_IArg) excCtx->threadHandle);
  Log_print1(Diags_USER1|Diags_CRITICAL,"stack base: 0x%x.\n", (xdc_IArg) excCtx->threadStack);
  Log_print1(Diags_USER1|Diags_CRITICAL,"stack size: 0x%x.\n", excCtx->threadStackSize);

  Log_print2(Diags_USER1|Diags_CRITICAL,"R0 = 0x%08x  R8  = 0x%08x\n", (xdc_IArg) excCtx->r0, (xdc_IArg) excCtx->r8);
  Log_print2(Diags_USER1|Diags_CRITICAL,"R1 = 0x%08x  R9  = 0x%08x\n", (xdc_IArg) excCtx->r1, (xdc_IArg) excCtx->r9);
  Log_print2(Diags_USER1|Diags_CRITICAL,"R2 = 0x%08x  R10 = 0x%08x\n", (xdc_IArg) excCtx->r2, (xdc_IArg) excCtx->r10);
  Log_print2(Diags_USER1|Diags_CRITICAL,"R3 = 0x%08x  R11 = 0x%08x\n", (xdc_IArg) excCtx->r3, (xdc_IArg) excCtx->r11);
  Log_print2(Diags_USER1|Diags_CRITICAL,"R4 = 0x%08x  R12 = 0x%08x\n", (xdc_IArg) excCtx->r4, (xdc_IArg) excCtx->r12);
  Log_print2(Diags_USER1|Diags_CRITICAL,"R5 = 0x%08x  SP(R13) = 0x%08x\n", (xdc_IArg) excCtx->r5, (xdc_IArg) excCtx->sp);
  Log_print2(Diags_USER1|Diags_CRITICAL,"R6 = 0x%08x  LR(R14) = 0x%08x\n", (xdc_IArg) excCtx->r6, (xdc_IArg) excCtx->lr);
  Log_print2(Diags_USER1|Diags_CRITICAL,"R7 = 0x%08x  PC(R15) = 0x%08x\n", (xdc_IArg) excCtx->r7, (xdc_IArg) excCtx->pc);
  Log_print1(Diags_USER1|Diags_CRITICAL,"PSR = 0x%08x\n", (xdc_IArg) excCtx->psr);
  Log_print1(Diags_USER1|Diags_CRITICAL,"ICSR = 0x%08x\n", Hwi_nvic.ICSR);
  Log_print1(Diags_USER1|Diags_CRITICAL,"MMFSR = 0x%02x\n", Hwi_nvic.MMFSR);
  Log_print1(Diags_USER1|Diags_CRITICAL,"BFSR = 0x%02x\n", Hwi_nvic.BFSR);
  Log_print1(Diags_USER1|Diags_CRITICAL,"UFSR = 0x%04x\n", Hwi_nvic.UFSR);
  Log_print1(Diags_USER1|Diags_CRITICAL,"HFSR = 0x%08x\n", Hwi_nvic.HFSR);
  Log_print1(Diags_USER1|Diags_CRITICAL,"DFSR = 0x%08x\n", Hwi_nvic.DFSR);
  Log_print1(Diags_USER1|Diags_CRITICAL,"MMAR = 0x%08x\n", Hwi_nvic.MMAR);
  Log_print1(Diags_USER1|Diags_CRITICAL,"BFAR = 0x%08x\n", Hwi_nvic.BFAR);
  Log_print1(Diags_USER1|Diags_CRITICAL,"AFSR = 0x%08x\n", Hwi_nvic.AFSR);
  Log_write2(UIAErr_fatalWithStr,myErrorCode,(IArg)"Terminating Execution...");
}

/** 
********************************************************************************
 *  @fn     myErrorHook
 *  @brief  Errors will be automatically logged by BIOS so no special UIA code
 *          is required in the hook function to log the errors.
 * 
 *  @returns none 
********************************************************************************
*/
Void myErrorHook(Error_Block *eb)
{
}

/* Nothing beyond this point */

