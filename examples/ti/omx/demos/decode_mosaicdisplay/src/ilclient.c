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
 *  @file  decode_display_test.c
 *  @brief This file contains all Functions related to Test Application
 *
 *         This is the example IL Client support to create, configure & chaining
 *         of single channel omx-components using  non tunneling 
 *         mode
 *
 *  @rev 1.0
 *******************************************************************************
 */

/*******************************************************************************
*                             Compilation Control Switches
*******************************************************************************/
/*BUFFER_ALLOCATE_IN_ILCLIENT - controls from where the buffer allocation happens. 
 * if defined then buffer allocation happens in IL client and they are given to 
 * the component for use.
 */

#define BUFFER_ALLOCATE_IN_ILCLIENT 1

/*IL_CLIENT_SR - defines the shred region ID. 
 * This value is taken from the  
 * http://processors.wiki.ti.com/index.php/EZSDK_Memory_Map
 * Also, this is the value used by the firmware loader.
 * firmware_loader\src\memsegdef_default.c
 */
#define IL_CLIENT_SR                2

/*******************************************************************************
*                             INCLUDE FILES
*******************************************************************************/

/*--------------------- system and platform files ----------------------------*/
#include <xdc/std.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <ti/syslink/utils/IHeap.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/syslink/utils/Memory.h>

/*-------------------------program files -------------------------------------*/
#include "ti/omx/interfaces/openMaxv11/OMX_Core.h"
#include "ti/omx/interfaces/openMaxv11/OMX_Component.h"
#include "ilclient.h"
#include "ilclient_utils.h"
#include "h264_parser.h"
#include <omx_vdec.h>
#include <omx_vfpc.h>
#include <omx_vfdc.h>
#include <omx_ctrl.h>
#include <omx_vswmosaic.h>

typedef void *(*ILC_StartFcnPtr) (void *);

extern void IL_ClientFbDevAppTask (void *threadsArg);

/* ========================================================================== */
/**
* IL_ClientCbEventHandler() : This method is the event handler implementation to 
* handle events from the OMX Derived component
*
* @param hComponent        : Handle to the component
* @param ptrAppData        : 
* @param eEvent            :
* @param nData1            :
* @param nData2            :
* @param pEventData        :
*
*  @return      
*  OMX_ErrorNone = Successful 
*
*  Other_value = Failed (Error code is returned)
*
*/
/* ========================================================================== */
OMX_ERRORTYPE IL_ClientCbEventHandler (OMX_HANDLETYPE hComponent,
                                              OMX_PTR ptrAppData,
                                              OMX_EVENTTYPE eEvent,
                                              OMX_U32 nData1, OMX_U32 nData2,
                                              OMX_PTR pEventData)
{
  IL_CLIENT_COMP_PRIVATE *comp;

  comp = ptrAppData;

  printf ("got event :: ");
  if (eEvent == OMX_EventCmdComplete)
  {
    if (nData1 == OMX_CommandStateSet)
    {
      printf ("State changed to: ");
      switch ((int) nData2)
      {
        case OMX_StateInvalid:
          printf ("OMX_StateInvalid \n");
          break;
        case OMX_StateLoaded:
          printf ("OMX_StateLoaded \n");
          break;
        case OMX_StateIdle:
          printf ("OMX_StateIdle \n");
          break;
        case OMX_StateExecuting:
          printf ("OMX_StateExecuting \n");
          break;
        case OMX_StatePause:
          printf ("OMX_StatePause\n");
          break;
        case OMX_StateWaitForResources:
          printf ("OMX_StateWaitForResources\n");
          break;
      }
      /* post an semaphore, so that in IL Client we can confirm the state
         change */
      semp_post (comp->done_sem);
    }
    else if (OMX_CommandFlush == nData1) {
     printf("OMX_CommandFlush completed \n");
      semp_post (comp->done_sem);
     }
    
    else if (OMX_CommandPortEnable || OMX_CommandPortDisable)
    {
      printf ("Enable/Disable Event \n");
      semp_post (comp->port_sem);
    }
  }
  else if (eEvent == OMX_EventBufferFlag)
  {
    printf ("OMX_EventBufferFlag \n");
    if ((int) nData2 == OMX_BUFFERFLAG_EOS)
    {
      printf ("got EOS event \n");
      semp_post (comp->eos);
    }
  }
  else if (eEvent == OMX_EventError)
  {
    printf ("*** unrecoverable error: %s (0x%lx) \n",
            IL_ClientErrorToStr (nData1), nData1);
    printf ("Press a key to proceed\n");
  }
  else
  {
    printf ("unhandled event, param1 = %i, param2 = %i \n", (int) nData1,
            (int) nData2);
  }

  return OMX_ErrorNone;
}

/* ========================================================================== */
/**
* IL_ClientCbEmptyBufferDone() : This method is the callback implementation to 
* handle EBD events from the OMX Derived component
*
* @param hComponent        : Handle to the component
* @param ptrAppData        : app pointer, which was passed during the getHandle
* @param pBuffer           : buffer header, for the buffer which is consumed
*
*  @return      
*  OMX_ErrorNone = Successful 
*
*  Other_value = Failed (Error code is returned)
*
*/
/* ========================================================================== */

OMX_ERRORTYPE IL_ClientCbEmptyBufferDone (OMX_HANDLETYPE hComponent,
                                          OMX_PTR ptrAppData,
                                          OMX_BUFFERHEADERTYPE *pBuffer)
{
  IL_CLIENT_COMP_PRIVATE *thisComp = (IL_CLIENT_COMP_PRIVATE *) ptrAppData;
  IL_CLIENT_INPORT_PARAMS *inPortParamsPtr;
  IL_CLIENT_PIPE_MSG localPipeMsg;

  OMX_ERRORTYPE eError = OMX_ErrorNone;
  int retVal = 0;

  inPortParamsPtr = thisComp->inPortParams + pBuffer->nInputPortIndex;

  /* if the buffer is from file i/o, write the free buffer header into ipbuf
     pipe, else keep it in its local pipe. From local pipe It would be given to 
     remote component as "consumed buffer "*/

  if (inPortParamsPtr->connInfo.remotePipe[0] == (int)NULL)
  {
    /* write the empty buffer pointer to input pipe */
    retVal = write (inPortParamsPtr->ipBufPipe[1], &pBuffer, sizeof (pBuffer));

    if (sizeof (pBuffer) != retVal)
    {
      printf ("Error writing into Input buffer i/p Pipe!\n");
      eError = OMX_ErrorNotReady;
      return eError;
    }
  }
  else
  {
    /* Create a message that EBD is done and this buffer is ready to be
       recycled. This message will be read in buffer processing thread and
       remote component will be indicated about its status */
    localPipeMsg.cmd = IL_CLIENT_PIPE_CMD_EBD;
    localPipeMsg.pbufHeader = pBuffer;
    retVal = write (thisComp->localPipe[1],
                    &localPipeMsg, sizeof (IL_CLIENT_PIPE_MSG));
    if (sizeof (IL_CLIENT_PIPE_MSG) != retVal)
    {
      printf ("Error writing into local Pipe!\n");
      eError = OMX_ErrorNotReady;
      return eError;
    }
  }

  return eError;
}

/* ========================================================================== */
/**
* IL_ClientCbFillBufferDone() : This method is the callback implementation to 
* handle FBD events from the OMX Derived component
*
* @param hComponent        : Handle to the component
* @param ptrAppData        : app pointer, which was passed during the getHandle
* @param pBuffer           : buffer header, for the buffer which is produced
*
*  @return      
*  OMX_ErrorNone = Successful 
*
*  Other_value = Failed (Error code is returned)
*
*/
/* ========================================================================== */

OMX_ERRORTYPE IL_ClientCbFillBufferDone (OMX_HANDLETYPE hComponent,
                                         OMX_PTR ptrAppData,
                                         OMX_BUFFERHEADERTYPE *pBuffer)
{
  IL_CLIENT_COMP_PRIVATE *thisComp = (IL_CLIENT_COMP_PRIVATE *) ptrAppData;
  IL_CLIENT_OUTPORT_PARAMS *outPortParamsPtr;
  IL_CLIENT_PIPE_MSG localPipeMsg;

  OMX_ERRORTYPE eError = OMX_ErrorNone;
  int retVal = 0;

  /* get the pipe corrsponding to this port, portIndex is part of bufferheader
     structure */
  outPortParamsPtr =
    thisComp->outPortParams + (pBuffer->nOutputPortIndex -
                               thisComp->startOutportIndex);

  /* if the buffer is from file i/o, write the free buffer header into outbuf
     pipe, else keep it in its local pipe. From local pipe It would be given to 
     remote component as "filled buffer "*/
  if (outPortParamsPtr->connInfo.remotePipe[0] == (int)NULL)
  {
    /* write the empty buffer pointer to input pipe */
    retVal = write (outPortParamsPtr->opBufPipe[1], &pBuffer, sizeof (pBuffer));

    if (sizeof (pBuffer) != retVal)
    {
      printf ("Error writing to Input buffer i/p Pipe!\n");
      eError = OMX_ErrorNotReady;
      return eError;
    }
  }
  else
  {
    /* Create a message that FBD is done and this buffer is ready to be used by 
       other component. This message will be read in buffer processing thread
       and and remote component will be indicated about its status */
    localPipeMsg.cmd = IL_CLIENT_PIPE_CMD_FBD;
    localPipeMsg.pbufHeader = pBuffer;
    retVal = write (thisComp->localPipe[1],
                    &localPipeMsg, sizeof (IL_CLIENT_PIPE_MSG));

    if (sizeof (IL_CLIENT_PIPE_MSG) != retVal)
    {
      printf ("Error writing to local Pipe!\n");
      eError = OMX_ErrorNotReady;
      return eError;
    }
  }
  return eError;
}

/* ========================================================================== */
/**
* IL_ClientInputBitStreamReadTask() : This task function is file read task for
* decoder component. This function calls parser functions, which provides frames
* in each buffer to be consumed by decoder.
*
* @param threadsArg        : Handle to the application
*
*/
/* ========================================================================== */

void
IL_ClientInputBitStreamReadTask (void *threadsArg)
{
  unsigned int dataRead = 0;
  OMX_ERRORTYPE err = OMX_ErrorNone;
  IL_CLIENT_COMP_PRIVATE *decILComp = NULL;
  OMX_BUFFERHEADERTYPE *pBufferIn = NULL;
  IL_CLIENT_DEC_THREAD_ARGS *pDecArgs = threadsArg;
  
  decILComp = ((IL_Client *) (pDecArgs->ptrAppData))->decILComp[pDecArgs->instId];

  /* use the initial i/p buffers and make empty this buffer calls */
  err = IL_ClientDecUseInitialInputResources (pDecArgs);

  while (1)
  {
    /* Read empty buffer pointer from the pipe */
    read (decILComp->inPortParams->ipBufPipe[0],
          &pBufferIn, sizeof (pBufferIn));

    /* Fill the data in the empty buffer */
    dataRead = IL_ClientFillBitStreamData (pDecArgs, pBufferIn);

    /* Exit the loop if no data available */
    if (0 >= dataRead)
    {
      printf ("No data available for Read\n");
     pBufferIn->nFlags |= OMX_BUFFERFLAG_EOS;
     err = OMX_EmptyThisBuffer (decILComp->handle, pBufferIn);
     pthread_exit (decILComp);
      /* can be handled as EOS .. decILComp->inPortParams->flagInputEos =
         OMX_TRUE; */
      break;
    }

    /* Pass the input buffer to the component */
    err = OMX_EmptyThisBuffer (decILComp->handle, pBufferIn);

    if (OMX_ErrorNone != err)
    {
      /* put back the frame in pipe and wait for state change */
      write (decILComp->inPortParams->ipBufPipe[1],
             &pBufferIn, sizeof (pBufferIn));
      printf ("waiting for action from IL Cleint \n");

      /* since in this example we are changing states in other thread it will
         return error for giving ETB/FTB calls in non-execute state. Since
         example is shutting down, we exit the thread */

      pthread_exit (decILComp);

    }
  }

}

/* ========================================================================== */
/**
* IL_ClientPipWindowDataUpdate() : This (task) function is file read task for
* mosaic component. The file read as frames into each buffer to be consumed 
* by mosaic.
*
* @param threadsArg        : Handle to the application
*
*/
/* ========================================================================== */

void
IL_ClientPipWindowDataUpdate (void *threadsArg)
{
  OMX_ERRORTYPE err = OMX_ErrorNone;

  /* use the initial i/p buffers and make empty this buffer calls */
  err = IL_ClientPipUseInitialInputResources (threadsArg);

}

/* ========================================================================== */
/**
* IL_ClientConnInConnOutTask() : This task function is for passing buffers from
* one component to other connected component. This functions reads from local
* pipe of a particular component , and takes action based on the message in the
* pipe. This pipe is written by callback ( EBD/FBD) function from component and
* from other component threads, which writes into this pipe for buffer 
* communication.
*
* @param threadsArg        : Handle to a particular component
*
*/
/* ========================================================================== */

void IL_ClientConnInConnOutTask (void *threadsArg)
{
  IL_CLIENT_PIPE_MSG pipeMsg;
  IL_CLIENT_COMP_PRIVATE *thisComp = (IL_CLIENT_COMP_PRIVATE *) threadsArg;
  OMX_ERRORTYPE err = OMX_ErrorNone;

  /* Initially pipes will not have any buffers, so components needs to be given 
     empty buffers for output ports. Input bufefrs are given by other
     component, or file read task */
  IL_ClientUseInitialOutputResources (thisComp);

  for (;;)
  {
    /* Read from its own local Pipe */
    read (thisComp->localPipe[0], &pipeMsg, sizeof (IL_CLIENT_PIPE_MSG));

    /* check the function type */

    switch (pipeMsg.cmd)
    {
      case IL_CLIENT_PIPE_CMD_EXIT:
        printf ("exiting thread\n");
        pthread_exit (thisComp);
        break;
      case IL_CLIENT_PIPE_CMD_ETB:
        err = IL_ClientProcessPipeCmdETB (thisComp, &pipeMsg);
        /* If not in proper state, bufers may not be accepted by component */
        if (OMX_ErrorNone != err)
        {
          write (thisComp->localPipe[1], &pipeMsg, sizeof (IL_CLIENT_PIPE_MSG));
          printf ("ETB: wait \n");
          /* since in this example we are changing states in other thread it
             will return error for giving ETB/FTB calls in non-execute state.
             Since example is shutting down, we exit the thread */
          pthread_exit (thisComp);
          /* if error is incorrect state operation, wait for state to change */
          /* waiting mechanism should be implemented here */
        }
  
        break;
      case IL_CLIENT_PIPE_CMD_FTB:
        err = IL_ClientProcessPipeCmdFTB (thisComp, &pipeMsg);
  
        if (OMX_ErrorNone != err)
        {
          write (thisComp->localPipe[1], &pipeMsg, sizeof (IL_CLIENT_PIPE_MSG));
          printf ("FTB: wait \n");
          /* if error is incorrect state operation, wait for state to change
             waiting mechanism should be implemented here */
          /* since in this example we are changing states in other thread it
             will return error for giving ETB/FTB calls in non-execute state.
             Since example is shutting down, we exit the thread */
          pthread_exit (thisComp);
        }
        break;
  
      case IL_CLIENT_PIPE_CMD_EBD:
        IL_ClientProcessPipeCmdEBD (thisComp, &pipeMsg);
  
        break;

      case IL_CLIENT_PIPE_CMD_FBD:
        IL_ClientProcessPipeCmdFBD (thisComp, &pipeMsg);
        break;

      default:
        break;
    } /* switch () */
  } /* for (;;) */
}


/* ========================================================================== */
/**
* IL_ClientProcessPipeCmdETB() : This function passes the buffers to component
* for consuming. This buffer will come from other component as an output. To 
* consume it, IL client finds its buffer header (for consumer component), and 
* calls ETB call.
* @param thisComp        : Handle to a particular component
* @param pipeMsg         : message structure, which is written in response to 
*                          callbacks
*
*/
/* ========================================================================== */

OMX_ERRORTYPE IL_ClientProcessPipeCmdETB (IL_CLIENT_COMP_PRIVATE *thisComp,
                                          IL_CLIENT_PIPE_MSG *pipeMsg)
{
  OMX_ERRORTYPE err = OMX_ErrorNone;
  OMX_BUFFERHEADERTYPE *pBufferIn;

  /* search its own buffer header based on submitted by connected comp */
  IL_ClientUtilGetSelfBufHeader (thisComp, pipeMsg->bufHeader.pBuffer,
                                 ILCLIENT_INPUT_PORT,
                                 pipeMsg->bufHeader.nInputPortIndex,
                                 &pBufferIn);

  /* populate buffer header */
  pBufferIn->nFilledLen = pipeMsg->bufHeader.nFilledLen;
  pBufferIn->nOffset = pipeMsg->bufHeader.nOffset;
  pBufferIn->nTimeStamp = pipeMsg->bufHeader.nTimeStamp;
  pBufferIn->nFlags = pipeMsg->bufHeader.nFlags;
  pBufferIn->hMarkTargetComponent = pipeMsg->bufHeader.hMarkTargetComponent;
  pBufferIn->pMarkData = pipeMsg->bufHeader.pMarkData;
  pBufferIn->nTickCount = 0;

  /* call etb to the component */
  err = OMX_EmptyThisBuffer (thisComp->handle, pBufferIn);
  return (err);
}

/* ========================================================================== */
/**
* IL_ClientProcessPipeCmdFTB() : This function passes the buffers to component
* for consuming. This buffer will come from other component as consumed at input
* To  consume it, IL client finds its buffer header (for consumer component),
* and calls FTB call.
* @param thisComp        : Handle to a particular component
* @param pipeMsg         : message structure, which is written in response to 
*                          callbacks
*
*/
/* ========================================================================== */

OMX_ERRORTYPE IL_ClientProcessPipeCmdFTB (IL_CLIENT_COMP_PRIVATE *thisComp,
                                          IL_CLIENT_PIPE_MSG *pipeMsg)
{
  OMX_ERRORTYPE err = OMX_ErrorNone;
  OMX_BUFFERHEADERTYPE *pBufferOut;

  /* search its own buffer header based on submitted by connected comp */
  IL_ClientUtilGetSelfBufHeader (thisComp, pipeMsg->bufHeader.pBuffer,
                                 ILCLIENT_OUTPUT_PORT,
                                 pipeMsg->bufHeader.nOutputPortIndex,
                                 &pBufferOut);

  /* call etb to the component */
  err = OMX_FillThisBuffer (thisComp->handle, pBufferOut);

  return (err);
}

/* ========================================================================== */
/**
* IL_ClientProcessPipeCmdEBD() : This function passes the bufefrs to component
* for consuming. This empty buffer will go to other component to be reused at 
* output port.
* @param thisComp        : Handle to a particular component
* @param pipeMsg         : message structure, which is written in response to 
*                          callbacks
*
*/
/* ========================================================================== */

OMX_ERRORTYPE IL_ClientProcessPipeCmdEBD (IL_CLIENT_COMP_PRIVATE *thisComp,
                                          IL_CLIENT_PIPE_MSG *pipeMsg)
{
  OMX_ERRORTYPE err = OMX_ErrorNone;
  OMX_BUFFERHEADERTYPE *pBufferIn;
  IL_CLIENT_PIPE_MSG remotePipeMsg;
  IL_CLIENT_INPORT_PARAMS *inPortParamsPtr;
  int retVal = 0;

  pBufferIn = pipeMsg->pbufHeader;

  /* find the input port structure (pipe) */
  inPortParamsPtr = thisComp->inPortParams + pBufferIn->nInputPortIndex;

  remotePipeMsg.cmd = IL_CLIENT_PIPE_CMD_FTB;
  remotePipeMsg.bufHeader.pBuffer = pBufferIn->pBuffer;
  remotePipeMsg.bufHeader.nOutputPortIndex =
    inPortParamsPtr->connInfo.remotePort;

  /* write the fill buffer message to remote pipe */
  retVal = write (inPortParamsPtr->connInfo.remotePipe[1],
                  &remotePipeMsg, sizeof (IL_CLIENT_PIPE_MSG));

  if (sizeof (IL_CLIENT_PIPE_MSG) != retVal)
  {
    printf ("Error writing to remote Pipe!\n");
    err = OMX_ErrorNotReady;
    return err;
  }

  return (err);
}

/* ========================================================================== */
/**
* IL_ClientProcessPipeCmdFBD() : This function passes the bufefrs to component
* for consuming. This buffer will go to other component to be consumed at input
* port.
* @param thisComp        : Handle to a particular component
* @param pipeMsg         : message structure, which is written in response to 
*                          callbacks
*
*/
/* ========================================================================== */

OMX_ERRORTYPE IL_ClientProcessPipeCmdFBD (IL_CLIENT_COMP_PRIVATE *thisComp,
                                          IL_CLIENT_PIPE_MSG *pipeMsg)
{
  OMX_ERRORTYPE err = OMX_ErrorNone;
  OMX_BUFFERHEADERTYPE *pBufferOut;
  IL_CLIENT_PIPE_MSG remotePipeMsg;
  IL_CLIENT_OUTPORT_PARAMS *outPortParamsPtr;
  int retVal = 0;
  pBufferOut = pipeMsg->pbufHeader;

  remotePipeMsg.cmd = IL_CLIENT_PIPE_CMD_ETB;
  remotePipeMsg.bufHeader.pBuffer = pBufferOut->pBuffer;

  outPortParamsPtr =
    thisComp->outPortParams + (pBufferOut->nOutputPortIndex -
                               thisComp->startOutportIndex);

  /* populate buffer header */
  remotePipeMsg.bufHeader.nFilledLen = pBufferOut->nFilledLen;
  remotePipeMsg.bufHeader.nOffset = pBufferOut->nOffset;
  remotePipeMsg.bufHeader.nTimeStamp = pBufferOut->nTimeStamp;
  remotePipeMsg.bufHeader.nFlags = pBufferOut->nFlags & ~OMX_BUFFERFLAG_EOS;
  
  remotePipeMsg.bufHeader.hMarkTargetComponent =
    pBufferOut->hMarkTargetComponent;
  remotePipeMsg.bufHeader.pMarkData = pBufferOut->pMarkData;
  remotePipeMsg.bufHeader.nTickCount = pBufferOut->nTickCount;
  remotePipeMsg.bufHeader.nInputPortIndex =
    outPortParamsPtr->connInfo.remotePort;

  /* write the fill buffer message to remote pipe */
  retVal = write (outPortParamsPtr->connInfo.remotePipe[1],
                  &remotePipeMsg, sizeof (IL_CLIENT_PIPE_MSG));

  if (sizeof (IL_CLIENT_PIPE_MSG) != retVal)
  {
    printf ("Error writing to remote Pipe!\n");
    err = OMX_ErrorNotReady;
    return err;
  }

  return (err);
}

/* ========================================================================== */
/**
* Decode_Display_Test() : This method is the IL Client implementation for 
* connecting decoder, scalar and display OMX components. This function creates
* configures, and connects the components. it manages the buffer communication.
*
*  @param args         : arg pointer for parameters e.g. width, height, framerate
*  @return      
*  OMX_ErrorNone = Successful 
*
*  Other_value = Failed (Error code is returned)
*
*/
/* ========================================================================== */

/* Main IL Client application to create , intiate and connect components */
extern int g_max_decode;

IL_CLIENT_DEC_THREAD_ARGS g_dec_args[IL_CLIENT_MAX_DECODE];

int Decode_MosaicDisplay_Example ( IL_ARGS *args )
{

  OMX_U32 i, j;
  OMX_S32 ret_value;
  IL_Client *pAppData = NULL;
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  IL_CLIENT_PIPE_MSG pipeMsg;
  OMX_BOOL bUseSwMosaic;
  IHeap_Handle    heap;
  OMX_PTR pBuffer;

  /* Initialize application specific data structures and buffer management
     data structure */
  IL_ClientInit (&pAppData, args);
  bUseSwMosaic = (args->pip) ? OMX_TRUE: OMX_FALSE;

  printf ("opening file \n");
  
  for( i = 0; i < g_max_decode; i++) {
  /* Open the file of data to be decoded and rendered...  */
   pAppData->fIn[i] = fopen (args->input_file[i], "rb");
   if (pAppData->fIn[i] == NULL)
   {
     printf ("Error: failed to open the file %s for reading\n",
             args->input_file[i]);
     goto EXIT;
   }
 }

  /* H264 elementary stream parser is included in this example, which parses
     the elementary streams, and gives a single frame in each buffer */
  for (i = 0; i < g_max_decode; i++) {   
   Decode_ParserInit (&pAppData->pc[i], pAppData->fIn[i]);
  }
  
  /* Initialize application / IL Client callback functions */
  /* Callbacks are passed during getHandle call to component, Component uses
     these callaback to communicate with IL Client */
  /* event handler is to handle the state changes , omx commands and any
     message for IL client */
  pAppData->pCb.EventHandler = IL_ClientCbEventHandler;

  /* Empty buffer done is data callback at the input port, where component lets 
     the application know that buffer has been consumned, this is not
     applicable if there is no input port in the component */
  pAppData->pCb.EmptyBufferDone = IL_ClientCbEmptyBufferDone;

  /* fill buffer done is callback at the output port, where component lets the
     application know that an output buffer is available with the processed data 
   */
  pAppData->pCb.FillBufferDone = IL_ClientCbFillBufferDone;

  heap = SharedRegion_getHeap(IL_CLIENT_SR);

/******************************************************************************/
  /* Create the H264Decoder Component, component handle would be returned
     component name is unique and fixed for a component, callbacks are passed
     to component in this function. Component would be loaded state post this
     call */
 for (i = 0; i < g_max_decode; i++) {
   eError =
     OMX_GetHandle (&pAppData->pDecHandle[i],
                    (OMX_STRING) "OMX.TI.DUCATI.VIDDEC", pAppData->decILComp[i],
                    &pAppData->pCb);

   printf ("decoder component %d is created \n", i);

   if ((eError != OMX_ErrorNone) || (pAppData->pDecHandle[i] == NULL))
   {
     printf ("Error in Get Handle %d function : %s \n",
             i,IL_ClientErrorToStr (eError));
     goto EXIT;
   }

   pAppData->decILComp[i]->handle = pAppData->pDecHandle[i];

   /* Configute the decoder componet */
   /* calling OMX_Setparam in this function */
   IL_ClientSetDecodeParams (pAppData, i);
 }
/******************************************************************************/
  /* Create Scalar component, it created OMX component for scalar writeback ,
     In this client we are passing the same callbacks to all the components */
  for (i = 0; i < g_max_decode; i++) {
   eError =
     OMX_GetHandle (&pAppData->pScHandle[i],
                    (OMX_STRING) "OMX.TI.VPSSM3.VFPC.INDTXSCWB",
                    pAppData->scILComp[i], &pAppData->pCb);

   if ((eError != OMX_ErrorNone) || (pAppData->pScHandle[i] == NULL))
   {
     printf ("Error in Get Handle function : %s \n",
             IL_ClientErrorToStr (eError));
     goto EXIT;
   }
   pAppData->scILComp[i]->handle = pAppData->pScHandle[i];

   printf ("scalar component %d is created \n", i);

   /* omx calls are made in this function for setting the parameters for scalar
      component, For clarity purpose it is written as separate function */

   IL_ClientSetScalarParams (pAppData, i);

   /* enable input and output port */
   /* as per openmax specs all the ports should be enabled by default but EZSDK
      OMX component does not enable it hence we manually need to enable it. */

   printf ("enable scalar input port \n");
   OMX_SendCommand (pAppData->pScHandle[i], OMX_CommandPortEnable,
                    OMX_VFPC_INPUT_PORT_START_INDEX, NULL);

   /* wait for both ports to get enabled, event handler would be notified from
      the component after enabling the port, which inturn would post this
      semaphore */
   semp_pend (pAppData->scILComp[i]->port_sem);

   printf ("enable scalar output port \n");
   OMX_SendCommand (pAppData->pScHandle[i], OMX_CommandPortEnable,
                    OMX_VFPC_OUTPUT_PORT_START_INDEX, NULL);
   semp_pend (pAppData->scILComp[i]->port_sem);
 } 
/******************************************************************************/
  if ( bUseSwMosaic == OMX_TRUE) 
  {
    /**************************************************************************/
    /* Create and Configure the swmosaic component. It will use SWMOSAIC      */
    /* component on media controller                                          */
    /**************************************************************************/

    /* Create the swmosaic component */
    /* getting swmosaic component handle */
    eError =
      OMX_GetHandle (&pAppData->pVswmosaicHandle, "OMX.TI.VPSSM3.VSWMOSAIC",
                     pAppData->vswmosaicILComp, &pAppData->pCb);
    if (eError != OMX_ErrorNone)
      ERROR ("failed to get handle\n");

    printf ("found handle %p for component %s \n", pAppData->pVswmosaicHandle,
            "OMX.TI.VPSSM3.VSWMOSAIC");

    pAppData->vswmosaicILComp->handle = pAppData->pVswmosaicHandle;

    printf ("got vswmosaic handle \n");

    /* omx calls are made in this function for setting the parameters for display 
       component, For clarity purpose it is written as separate function */

    IL_ClientSetSwMosaicParams (pAppData);

    /* as per openmax specs all the ports should be enabled by default but EZSDK
       OMX component does not enable it hence we manually need to enable it */
    /* as per openmax specs all the ports should be enabled by default but EZSDK
       OMX component does not enable it hence we manually need to enable it */
    for (i = 0; i < g_max_decode; i++) {
    
     printf ("enable input port %d \n", i);
    
     OMX_SendCommand (pAppData->pVswmosaicHandle, OMX_CommandPortEnable,
                      OMX_VSWMOSAIC_INPUT_PORT_START_INDEX+i, NULL);

     /* wait for port to get enabled, event handler would be notified from the
        component after enabling the port, which inturn would post this semaphore */
     semp_pend (pAppData->vswmosaicILComp->port_sem);
     }
    /* as per openmax specs all the ports should be enabled by default but EZSDK
       OMX component does not enable it hence we manually need to enable it */
    printf ("enable output port 16\n");

    OMX_SendCommand (pAppData->pVswmosaicHandle, OMX_CommandPortEnable,
                     OMX_VSWMOSAIC_OUTPUT_PORT_START_INDEX, NULL);

    /* wait for port to get enabled, event handler would be notified from the
       component after enabling the port, which inturn would post this semaphore */
    semp_pend (pAppData->vswmosaicILComp->port_sem);
  }

/******************************************************************************/
/* Create and Configure the display component. It will use VFDC component on  */
/* media controller.                                                          */
/******************************************************************************/

  /* Create the display component */
  /* getting display component handle */
  eError =
    OMX_GetHandle (&pAppData->pDisHandle, "OMX.TI.VPSSM3.VFDC",
                   pAppData->disILComp, &pAppData->pCb);
  if (eError != OMX_ErrorNone)
    ERROR ("failed to get handle\n");

  printf ("found handle %p for component %s \n", pAppData->pDisHandle,
          "OMX.TI.VPSSM3.VFDC");

  pAppData->disILComp->handle = pAppData->pDisHandle;

  printf ("got display handle \n");
  /* getting display controller component handle, Display contrller is
     implemented as an OMX component, however it does not have any input or
     output ports. It is used only for controling display hw */
  eError =
    OMX_GetHandle (&pAppData->pctrlHandle, "OMX.TI.VPSSM3.CTRL.DC",
                   pAppData->disILComp, &pAppData->pCb);
  if (eError != OMX_ErrorNone)
    ERROR ("failed to get handle\n");

  printf ("found handle %p for component %s\n", pAppData->pctrlHandle,
          "OMX.TI.VPSSM3.CTRL.DC");

  /* omx calls are made in this function for setting the parameters for display 
     component, For clarity purpose it is written as separate function */

  IL_ClientSetDisplayParams (pAppData);

  /* as per openmax specs all the ports should be enabled by default but EZSDK
     OMX component does not enable it hence we manually need to enable it */
  printf ("enable input port \n");

  OMX_SendCommand (pAppData->pDisHandle, OMX_CommandPortEnable,
                   OMX_VFDC_INPUT_PORT_START_INDEX, NULL);

  /* wait for port to get enabled, event handler would be notified from the
     component after enabling the port, which inturn would post this semaphore */
  semp_pend (pAppData->disILComp->port_sem);

/******************************************************************************/
  /* Connect the decoder to scalar, This application uses 'pipe' to pass the
     buffers between different components. each compponent has a local pipe,
     which It reads for taking buffers. By connecting this functions informs
     about local pipe to other component, so that other component can pass
     buffers to this 'remote' pipe */

  printf ("connect call for decoder-scalar \n");

  for(i=0; i < g_max_decode; i++) {

  IL_ClientConnectComponents (pAppData->decILComp[i], OMX_VIDDEC_OUTPUT_PORT,
                              pAppData->scILComp[i],
                              OMX_VFPC_INPUT_PORT_START_INDEX);
  }
  
  if ( bUseSwMosaic == OMX_FALSE ) {
    printf ("connect call for scalar-display \n");

    /* Connect the scalar to display */
    IL_ClientConnectComponents (pAppData->scILComp[0],
                                OMX_VFPC_OUTPUT_PORT_START_INDEX,
                                pAppData->disILComp,
                                OMX_VFDC_INPUT_PORT_START_INDEX);

  }
  else {
    printf ("connect call for scalar-swmosaic \n");

    for(i=0; i < g_max_decode; i++) {
     /* Connect the scalar to swmosaic */
     IL_ClientConnectComponents (pAppData->scILComp[i],
                                 OMX_VFPC_OUTPUT_PORT_START_INDEX,
                                 pAppData->vswmosaicILComp,
                                 OMX_VSWMOSAIC_INPUT_PORT_START_INDEX + i);
    }
    printf ("connect call for swmosaic-display \n");
    /* Connect the mosaic to display */
    IL_ClientConnectComponents (pAppData->vswmosaicILComp,
                                OMX_VSWMOSAIC_OUTPUT_PORT_START_INDEX,
                                pAppData->disILComp,
                                OMX_VFDC_INPUT_PORT_START_INDEX);
  }

  /* OMX_SendCommand expecting OMX_StateIdle, after this command component
     would create codec, and will wait for all buffers to be allocated as per
     omx buffers are created during loaded to Idle transition ( if ports are
     enabled ) */
  for (i =0 ; i < g_max_decode; i++) {   
   eError =
     OMX_SendCommand (pAppData->pDecHandle[i], OMX_CommandStateSet,
                      OMX_StateIdle, NULL);
   if (eError != OMX_ErrorNone)
   {
     printf ("Error in SendCommand()-OMX_StateIdle State set : %s \n",
             IL_ClientErrorToStr (eError));
     goto EXIT;
   }
   /* Allocate I/O Buffers; component would allocate buffers and would return
      the buffer header containing the pointer to buffer */
   for (j = 0; j < pAppData->decILComp[i]->inPortParams->nBufferCountActual; j++)
   {
     eError = OMX_AllocateBuffer (pAppData->pDecHandle[i],
                                  &pAppData->decILComp[i]->
                                  inPortParams->pInBuff[j],
                                  OMX_VIDDEC_INPUT_PORT, pAppData,
                                  pAppData->decILComp[i]->
                                  inPortParams->nBufferSize);
     if (eError != OMX_ErrorNone)
     {
       printf ("Error in OMX_AllocateBuffer()- Input Port State set : %s \n",
               IL_ClientErrorToStr (eError));
       goto EXIT;
     }
   }

  printf ("decoder %d inport buffers allocated \n", i);

  /* buffer alloaction for output port */
  for (j = 0; j < pAppData->decILComp[i]->outPortParams->nBufferCountActual; j++)
  {
#ifdef BUFFER_ALLOCATE_IN_ILCLIENT
   OMX_PTR pTempBuffer;
   extern Ptr DomxCore_mapUsrVirtualAddr2phy (UInt32 pUsrVirtAddr);
   pBuffer = Memory_alloc (heap, pAppData->decILComp[i]->outPortParams->nBufferSize, 128, NULL);
   if (pBuffer == NULL) {
     printf ("Buffer Allocation Failed \n");
     exit (1);
   }
   else {
     pTempBuffer = (OMX_PTR) DomxCore_mapUsrVirtualAddr2phy ((uint32_t)pBuffer);

     printf ("Allocated buffer from shared region: %0x Physical addr: %0x \n", (uint32_t)pBuffer, (uint32_t)pTempBuffer);
   }
   eError = OMX_UseBuffer (pAppData->pDecHandle[i],
                           &pAppData->decILComp[i]->outPortParams->pOutBuff[j],
                           OMX_VIDDEC_OUTPUT_PORT, pAppData,
                           pAppData->decILComp[i]->outPortParams->nBufferSize,
                           pBuffer );
   if (eError != OMX_ErrorNone)
   {
     printf
       (" DEC: Error in OMX_UseBuffer() : %s \n",
        IL_ClientErrorToStr (eError));
     goto EXIT;
   }
#else
     eError = OMX_AllocateBuffer (pAppData->pDecHandle[i],
                                  &pAppData->decILComp[i]->
                                  outPortParams->pOutBuff[j],
                                  OMX_VIDDEC_OUTPUT_PORT, pAppData,
                                  pAppData->decILComp[i]->
                                  outPortParams->nBufferSize);
     if (eError != OMX_ErrorNone)
     {
       printf ("Error in OMX_AllocateBuffer()-Output Port State set : %s \n",
               IL_ClientErrorToStr (eError));
       goto EXIT;
     }
#endif
   }

   printf ("decoder outport buffers allocated \n");

  /* Wait for initialization to complete.. Wait for Idle stete of component
     after all buffers are alloacted componet would chnage to idle */
   semp_pend (pAppData->decILComp[i]->done_sem);

  printf ("decode %d state IDLE \n", i);

   eError =
     OMX_SendCommand (pAppData->pScHandle[i], OMX_CommandStateSet,
                      OMX_StateIdle, NULL);
   if (eError != OMX_ErrorNone)
   {
     printf ("Error in SendCommand()-OMX_StateIdle State set : %s \n",
             IL_ClientErrorToStr (eError));
     goto EXIT;
   }

   /* since decoder is connected to scalar, buffers are supplied by decoder to
      scalar, so scalar does not allocate the buffers. However it is informed to 
      use the buffers created by decoder. scalar component would create only
      buffer headers corresponding to these buffers */

   for (j = 0; j < pAppData->decILComp[i]->outPortParams->nBufferCountActual; j++)
   {

     eError = OMX_UseBuffer (pAppData->pScHandle[i],
                             &pAppData->scILComp[i]->inPortParams->pInBuff[j],
                             OMX_VFPC_INPUT_PORT_START_INDEX,
                             pAppData->scILComp[i],
                             pAppData->decILComp[i]->outPortParams->nBufferSize,
                             pAppData->decILComp[i]->outPortParams->pOutBuff[j]->
                             pBuffer);

     if (eError != OMX_ErrorNone)
     {
       printf ("Error in OMX_UseBuffer()-input Port State set : %s \n",
               IL_ClientErrorToStr (eError));
       goto EXIT;
     }
   }
   printf ("Scalar %d input port use buffer done \n", i);

   /* in SDK conventionally output port allocates the buffers, scalar would
      create the buffers which would be consumed by display component */
   /* buffer alloaction for output port */
   for (j = 0; j < pAppData->scILComp[i]->outPortParams->nBufferCountActual; j++)
   {
     eError = OMX_AllocateBuffer (pAppData->pScHandle[i],
                                  &pAppData->scILComp[i]->
                                  outPortParams->pOutBuff[j],
                                  OMX_VFPC_OUTPUT_PORT_START_INDEX, pAppData,
                                  pAppData->scILComp[i]->
                                  outPortParams->nBufferSize);
     if (eError != OMX_ErrorNone)
     {
       printf ("Error in OMX_AllocateBuffer()-Output Port State set : %s \n",
               IL_ClientErrorToStr (eError));
       goto EXIT;
     }
   }

   printf ("scalar %d outport buffers allocated \n", i);

   semp_pend (pAppData->scILComp[i]->done_sem);
   if (eError != OMX_ErrorNone)
   {
     printf ("Error %s:    WaitForState has timed out \n",
             IL_ClientErrorToStr (eError));
     goto EXIT;
   }
   printf ("scalar %d state IDLE \n", i);
 }
  if ( bUseSwMosaic  == OMX_FALSE ) 
  {
    /* control component does not allocate any data buffers, It's interface is
       though as it is omx componenet */
    eError =
      OMX_SendCommand (pAppData->pctrlHandle, OMX_CommandStateSet,
                       OMX_StateIdle, NULL);
    if (eError != OMX_ErrorNone)
    {
      printf ("Error in SendCommand()-OMX_StateIdle State set : %s \n",
              IL_ClientErrorToStr (eError));
      goto EXIT;
    }

    semp_pend (pAppData->disILComp->done_sem);

    printf ("ctrl-dc state IDLE \n");

    eError =
      OMX_SendCommand (pAppData->pDisHandle, OMX_CommandStateSet,
                       OMX_StateIdle, NULL);
    if (eError != OMX_ErrorNone)
    {
      printf ("Error in SendCommand()-OMX_StateIdle State set : %s \n",
              IL_ClientErrorToStr (eError));
      goto EXIT;
    }
    /* Since display has only input port and buffers are already created by
       scalar components, only use_buffer call is used at input port. there is no 
       output port in the display component */
    /* Display input will have same number of buffers as scalar output port */
    for (i = 0; i < pAppData->scILComp[0]->outPortParams->nBufferCountActual; i++)
    {

      eError = OMX_UseBuffer (pAppData->pDisHandle,
                              &pAppData->disILComp->inPortParams->pInBuff[i],
                              OMX_VFDC_INPUT_PORT_START_INDEX,
                              pAppData->disILComp,
                              pAppData->scILComp[0]->outPortParams->nBufferSize,
                              pAppData->scILComp[0]->outPortParams->pOutBuff[i]->
                              pBuffer);

      if (eError != OMX_ErrorNone)
      {
        printf ("Error in Display OMX_UseBuffer()- %s \n",
                IL_ClientErrorToStr (eError));
        goto EXIT;
      }
    }

    semp_pend (pAppData->disILComp->done_sem);

    printf ("display state IDLE \n");
  }
  else 
  {
    eError =
      OMX_SendCommand (pAppData->pVswmosaicHandle, OMX_CommandStateSet,
                       OMX_StateIdle, NULL);
    if (eError != OMX_ErrorNone)
    {
      printf ("Error in SendCommand()-OMX_StateIdle State set : %s \n",
              IL_ClientErrorToStr (eError));
      goto EXIT;
    }

    /* Since vswmosaic has only input port and buffers are already created by
       scalar components, only use_buffer call is used at input port. there is no 
       output port in the display component */
    /* Display input will have same number of buffers as vswmosaic output port */
    for ( i = 0; i < g_max_decode; i++) {
    IL_CLIENT_INPORT_PARAMS *inPortParamsPtr = pAppData->vswmosaicILComp->inPortParams + i;
    for (j = 0; j < pAppData->scILComp[i]->outPortParams->nBufferCountActual; j++)
    {
      eError = OMX_UseBuffer (pAppData->pVswmosaicHandle,
                              &(inPortParamsPtr->pInBuff[j]),
                              OMX_VSWMOSAIC_INPUT_PORT_START_INDEX + i,
                              pAppData->vswmosaicILComp,
                              pAppData->scILComp[i]->outPortParams->nBufferSize,
                              pAppData->scILComp[i]->outPortParams->pOutBuff[j]->
                              pBuffer);

      if (eError != OMX_ErrorNone)
      {
        printf ("Error in VSWMOSAIC OMX_UseBuffer()- %s \n",
                IL_ClientErrorToStr (eError));
        goto EXIT;
      }
    }
  }
    printf ("vswmosaic inport buffers allocated \n");
    /* in SDK conventionally output port allocates the buffers, scalar would
       create the buffers which would be consumed by display component */
    /* buffer alloaction for output port */
    for (i = 0; 
         i < pAppData->vswmosaicILComp->outPortParams->nBufferCountActual; i++)
    {
      eError = OMX_AllocateBuffer (pAppData->pVswmosaicHandle,
                                   &pAppData->vswmosaicILComp->
                                   outPortParams->pOutBuff[i],
                                   OMX_VSWMOSAIC_OUTPUT_PORT_START_INDEX, 
                                   pAppData,
                                   pAppData->vswmosaicILComp->
                                   outPortParams->nBufferSize);
      if (eError != OMX_ErrorNone)
      {
        printf ("Error in OMX_AllocateBuffer()-Output Port State set : %s \n",
                IL_ClientErrorToStr (eError));
        goto EXIT;
      }
    }
    printf ("vswmosaic outport buffers allocated \n");

    semp_pend (pAppData->vswmosaicILComp->done_sem);

    printf ("vswmosaic IDLE \n");

    /* control component does not allocate any data buffers, It's interface is
       though as it is omx componenet */
    eError =
      OMX_SendCommand (pAppData->pctrlHandle, OMX_CommandStateSet,
                       OMX_StateIdle, NULL);
    if (eError != OMX_ErrorNone)
    {
      printf ("Error in SendCommand()-OMX_StateIdle State set : %s \n",
              IL_ClientErrorToStr (eError));
      goto EXIT;
    }
    semp_pend (pAppData->disILComp->done_sem);

    printf ("ctrl-dc state IDLE \n");

    eError =
      OMX_SendCommand (pAppData->pDisHandle, OMX_CommandStateSet,
                       OMX_StateIdle, NULL);
    if (eError != OMX_ErrorNone)
    {
      printf ("Error in SendCommand()-OMX_StateIdle State set : %s \n",
              IL_ClientErrorToStr (eError));
      goto EXIT;
    }

    /* Since display has only input port and buffers are already created by
       scalar components, only use_buffer call is used at input port. there is no 
       output port in the display component */
    /* Display input will have same number of buffers as scalar output port */
    for (i = 0; 
         i < pAppData->vswmosaicILComp->outPortParams->nBufferCountActual; i++)
    {

      eError = OMX_UseBuffer (pAppData->pDisHandle,
                              &pAppData->disILComp->inPortParams->pInBuff[i],
                              OMX_VFDC_INPUT_PORT_START_INDEX,
                              pAppData->disILComp,
                              pAppData->vswmosaicILComp->outPortParams->nBufferSize,
                              pAppData->vswmosaicILComp->outPortParams->pOutBuff[i]->
                              pBuffer);

      if (eError != OMX_ErrorNone)
      {
        printf ("Error in Display OMX_UseBuffer()- %s \n",
                IL_ClientErrorToStr (eError));
        goto EXIT;
      }
    }
    semp_pend (pAppData->disILComp->done_sem);
    printf ("display state IDLE \n");
  }

  /* change state tho execute, so that component can accept buffers from IL
     client. Please note the ordering of components is from consumer to
     producer component i.e. display-scalar-decoder */
  eError =
    OMX_SendCommand (pAppData->pctrlHandle, OMX_CommandStateSet,
                     OMX_StateExecuting, NULL);
  if (eError != OMX_ErrorNone)
  {
    printf ("Error in SendCommand()-OMX_StateIdle State set : %s \n",
            IL_ClientErrorToStr (eError));
    goto EXIT;
  }

  semp_pend (pAppData->disILComp->done_sem);

  printf ("display control state execute \n");

  /* change state to execute so that buffers processing can start */
  eError =
    OMX_SendCommand (pAppData->pDisHandle, OMX_CommandStateSet,
                     OMX_StateExecuting, NULL);
  if (eError != OMX_ErrorNone)
  {
    printf ("Error from SendCommand-Executing State set :%s \n",
            IL_ClientErrorToStr (eError));
    goto EXIT;
  }

  semp_pend (pAppData->disILComp->done_sem);

  printf ("display state execute \n");

  if ( bUseSwMosaic  == OMX_TRUE ) 
  {
    /* change state to execute so that buffers processing can start */
    eError =
      OMX_SendCommand (pAppData->pVswmosaicHandle, OMX_CommandStateSet,
                       OMX_StateExecuting, NULL);
    if (eError != OMX_ErrorNone)
    {
      printf ("Error from SendCommand-Executing State set :%s \n",
              IL_ClientErrorToStr (eError));
      goto EXIT;
    }

    semp_pend (pAppData->vswmosaicILComp->done_sem);

    printf ("vswmosaic state execute \n");
  }
  /* change state to execute so that buffers processing can start */
  for (i = 0 ; i< g_max_decode; i++) { 
   eError =
     OMX_SendCommand (pAppData->pScHandle[i], OMX_CommandStateSet,
                      OMX_StateExecuting, NULL);
   if (eError != OMX_ErrorNone)
   {
     printf ("Error from SendCommand-Executing State set :%s \n",
             IL_ClientErrorToStr (eError));
     goto EXIT;
   }

   semp_pend (pAppData->scILComp[i]->done_sem);

   printf ("scalar state execute \n");
   /* change state to execute so that buffers processing can start */
   eError =
     OMX_SendCommand (pAppData->pDecHandle[i], OMX_CommandStateSet,
                      OMX_StateExecuting, NULL);
   if (eError != OMX_ErrorNone)
   {
     printf ("Error from SendCommand-Executing State set :%s \n",
             IL_ClientErrorToStr (eError));
     goto EXIT;
   }

   semp_pend (pAppData->decILComp[i]->done_sem);

   printf ("decoder state execute \n");
  }
  /* Create Graphics Thread */
  if (args->gfx)
  {
    /* Set the Graphics (FB) node to be the same as for the video display 
       chosen*/
    pAppData->gfx.gfxId = pAppData->displayId;
    
    pthread_attr_init (&pAppData->gfx.ThreadAttr);
    if (0 !=
        pthread_create (&pAppData->gfx.ThrdId,
                        &pAppData->gfx.ThreadAttr,
                        (ILC_StartFcnPtr) IL_ClientFbDevAppTask, &(pAppData->gfx)))
    {
      printf ("Create_Task failed !: %s", "IL_ClientFbDevAppTask");
      goto EXIT;
    }
    printf ("Graphics thread Created \n");
  }
  pthread_attr_init (&pAppData->disILComp->ThreadAttr);

  if (0 !=
      pthread_create (&pAppData->disILComp->connDataStrmThrdId,
                      &pAppData->disILComp->ThreadAttr,
                      (ILC_StartFcnPtr) IL_ClientConnInConnOutTask, pAppData->disILComp))
  {
    printf ("Create_Task failed !");
    goto EXIT;
  }
  printf ("display connect thread created \n");

  if ( bUseSwMosaic  == OMX_TRUE ) 
  {
    pthread_attr_init (&pAppData->vswmosaicILComp->ThreadAttr);

    if (0 !=
        pthread_create (&pAppData->vswmosaicILComp->connDataStrmThrdId,
                        &pAppData->vswmosaicILComp->ThreadAttr,
                        (ILC_StartFcnPtr) IL_ClientConnInConnOutTask, pAppData->vswmosaicILComp))
    {
      printf ("Create_Task failed !");
      goto EXIT;
    }
    printf ("swmosaic connect thread created \n");
    
    // IL_ClientPipWindowDataUpdate ( pAppData );
    
  }

  /* Create thread for reading bitstream and passing the buffers to decoder
     component */
  /* This thread would take the h264 elementary stream , parses it, give a
     frame to decoder at the input port. decoder */ 
  for ( i = 0; i < g_max_decode; i++) {   
   pthread_attr_init (&pAppData->decILComp[i]->ThreadAttr);

   /* creating thread for multiple channels requires unshared variables */
   g_dec_args[i].ptrAppData = pAppData;
   g_dec_args[i].instId     = i;

   if (0 !=
       pthread_create (&pAppData->decILComp[i]->inDataStrmThrdId,
                       &pAppData->decILComp[i]->ThreadAttr,
                       (ILC_StartFcnPtr) IL_ClientInputBitStreamReadTask, &g_dec_args[i]))
   {
     printf ("Create_Task failed !");
     goto EXIT;
   }

   printf ("file read thread created \n");

   pthread_attr_init (&pAppData->decILComp[i]->ThreadAttr);

   /* These are thread create for each component to pass the buffers to each
      other. this thread function reads the buffers from pipe and feeds it to
      componenet or for processed buffers, passes the buffers to connceted
      component */
   if (0 !=
       pthread_create (&pAppData->decILComp[i]->connDataStrmThrdId,
                       &pAppData->decILComp[i]->ThreadAttr,
                       (ILC_StartFcnPtr) IL_ClientConnInConnOutTask, pAppData->decILComp[i]))
   {
     printf ("Create_Task failed !");
     goto EXIT;
   }

   printf ("decoder %d connect thread created \n", i);

   pthread_attr_init (&pAppData->scILComp[i]->ThreadAttr);

   if (0 !=
       pthread_create (&pAppData->scILComp[i]->connDataStrmThrdId,
                       &pAppData->scILComp[i]->ThreadAttr,
                       (ILC_StartFcnPtr) IL_ClientConnInConnOutTask, pAppData->scILComp[i]))
   {
     printf ("Create_Task failed !");
     goto EXIT;
   }

   printf ("scalar %d connect thread created \n", i);
  }

  printf ("executing the application now!!!\n");

  for (i = 0; i < g_max_decode; i++) {
  /* Waiting for this semaphore to be posted by the bitstream read thread */
  semp_pend (pAppData->decILComp[i]->eos);
  }
  printf ("tearing down the decode-display example\n");

  /* change the state to idle */
  /* before changing state to idle, buffer communication to component should be 
     stoped , writing an exit messages to threads */

  pipeMsg.cmd = IL_CLIENT_PIPE_CMD_EXIT;
  for (i = 0; i < g_max_decode; i++) {

   write (pAppData->decILComp[i]->localPipe[1],
          &pipeMsg, sizeof (IL_CLIENT_PIPE_MSG));

   write (pAppData->scILComp[i]->localPipe[1],
          &pipeMsg, sizeof (IL_CLIENT_PIPE_MSG));
  }
  
  if ( bUseSwMosaic  == OMX_TRUE ) 
  {
    write (pAppData->vswmosaicILComp->localPipe[1],
           &pipeMsg, sizeof (IL_CLIENT_PIPE_MSG));
  }

  write (pAppData->disILComp->localPipe[1],
         &pipeMsg, sizeof (IL_CLIENT_PIPE_MSG));

  for (i = 0; i < g_max_decode; i++) {
  
   /* change state to idle so that buffers processing would stop */
   eError =
     OMX_SendCommand (pAppData->pDecHandle[i], OMX_CommandStateSet,
                      OMX_StateIdle, NULL);
   if (eError != OMX_ErrorNone)
   {
     printf ("Error from SendCommand-Idle State set :%s \n",
             IL_ClientErrorToStr (eError));
     goto EXIT;
   }

   semp_pend (pAppData->decILComp[i]->done_sem);

   printf ("decoder%d  state idle \n", i);

   /* change state to idle so that buffers processing can stop */
   eError =
     OMX_SendCommand (pAppData->pScHandle[i], OMX_CommandStateSet,
                      OMX_StateIdle, NULL);
   if (eError != OMX_ErrorNone)
   {
     printf ("Error from SendCommand-Idle State set :%s \n",
             IL_ClientErrorToStr (eError));
     goto EXIT;
   }

   semp_pend (pAppData->scILComp[i]->done_sem);

   printf ("Scalar %d state  idle \n", i);
  } 

  if ( bUseSwMosaic  == OMX_TRUE ) 
  {
    eError =
      OMX_SendCommand (pAppData->pVswmosaicHandle, OMX_CommandStateSet,
                       OMX_StateIdle, NULL);
    if (eError != OMX_ErrorNone)
    {
      printf ("Error from SendCommand-Idle State set :%s \n",
              IL_ClientErrorToStr (eError));
      goto EXIT;
    }

    semp_pend (pAppData->vswmosaicILComp->done_sem);

    printf ("vswmosaic state idle \n");
  }
  /* change state to execute so that buffers processing can stop */
  eError =
    OMX_SendCommand (pAppData->pDisHandle, OMX_CommandStateSet,
                     OMX_StateIdle, NULL);
  if (eError != OMX_ErrorNone)
  {
    printf ("Error from SendCommand-Idle State set :%s \n",
            IL_ClientErrorToStr (eError));
    goto EXIT;
  }

  semp_pend (pAppData->disILComp->done_sem);

  printf ("display state idle \n");

  eError =
    OMX_SendCommand (pAppData->pctrlHandle, OMX_CommandStateSet,
                     OMX_StateIdle, NULL);
  if (eError != OMX_ErrorNone)
  {
    printf ("Error in SendCommand()-OMX_StateIdle State set : %s \n",
            IL_ClientErrorToStr (eError));
    goto EXIT;
  }

  semp_pend (pAppData->disILComp->done_sem);

  printf ("display control state idle \n");

  /* change the state to loded */
  for (i = 0; i < g_max_decode; i++) {

   eError =
     OMX_SendCommand (pAppData->pDecHandle[i], OMX_CommandStateSet,
                      OMX_StateLoaded, NULL);
   if (eError != OMX_ErrorNone)
   {
     printf ("Error from SendCommand-Idle State set :%s \n",
             IL_ClientErrorToStr (eError));
     goto EXIT;
   }
   /* During idle-> loaded state transition buffers need to be freed up */
   for (j = 0; j < pAppData->decILComp[i]->inPortParams->nBufferCountActual; j++)
   {
     eError =
       OMX_FreeBuffer (pAppData->pDecHandle[i], OMX_VIDDEC_INPUT_PORT,
                       pAppData->decILComp[i]->inPortParams->pInBuff[j]);
     if (eError != OMX_ErrorNone)
     {
       printf ("Error in OMX_FreeBuffer : %s \n", IL_ClientErrorToStr (eError));
       goto EXIT;
     }
   }

   for (j = 0; j < pAppData->decILComp[i]->outPortParams->nBufferCountActual; j++)
   {
     eError =
       OMX_FreeBuffer (pAppData->pDecHandle[i], OMX_VIDDEC_OUTPUT_PORT,
                       pAppData->decILComp[i]->outPortParams->pOutBuff[j]);
     if (eError != OMX_ErrorNone)
     {
       printf ("Error in OMX_FreeBuffer : %s \n", IL_ClientErrorToStr (eError));
       goto EXIT;
     }
 #ifdef BUFFER_ALLOCATE_IN_ILCLIENT
    Memory_free ( heap, pAppData->decILComp[i]->outPortParams->pOutBuff[j]->pBuffer, pAppData->decILComp[i]->outPortParams->nBufferSize);
 #endif
   }

   semp_pend (pAppData->decILComp[i]->done_sem);

   printf ("decoder %d state loaded \n", i);

   eError =
     OMX_SendCommand (pAppData->pScHandle[i], OMX_CommandStateSet,
                      OMX_StateLoaded, NULL);
   if (eError != OMX_ErrorNone)
   {
     printf ("Error from SendCommand-Idle State set :%s \n",
             IL_ClientErrorToStr (eError));
     goto EXIT;
   }
   /* During idle-> loaded state transition buffers need to be freed up */
   for (j = 0; j < pAppData->scILComp[i]->inPortParams->nBufferCountActual; j++)
   {
     eError =
       OMX_FreeBuffer (pAppData->pScHandle[i], OMX_VFPC_INPUT_PORT_START_INDEX,
                       pAppData->scILComp[i]->inPortParams->pInBuff[j]);
     if (eError != OMX_ErrorNone)
     {
       printf ("Error in OMX_FreeBuffer : %s \n", IL_ClientErrorToStr (eError));
       goto EXIT;
     }
   }

   for (j = 0; j < pAppData->scILComp[i]->outPortParams->nBufferCountActual; j++)
   {
     eError =
       OMX_FreeBuffer (pAppData->pScHandle[i], OMX_VFPC_OUTPUT_PORT_START_INDEX,
                       pAppData->scILComp[i]->outPortParams->pOutBuff[j]);
     if (eError != OMX_ErrorNone)
     {
       printf ("Error in OMX_FreeBuffer : %s \n", IL_ClientErrorToStr (eError));
       goto EXIT;
     }
   }

   semp_pend (pAppData->scILComp[i]->done_sem);

   printf ("Scalar %d state loaded \n", i);
 } 

  if ( bUseSwMosaic  == OMX_TRUE ) 
  {
    eError =
      OMX_SendCommand (pAppData->pVswmosaicHandle, OMX_CommandStateSet,
                       OMX_StateLoaded, NULL);
    if (eError != OMX_ErrorNone)
    {
      printf ("Error from SendCommand-Idle State set :%s \n",
              IL_ClientErrorToStr (eError));
      goto EXIT;
    }
    /* During idle-> loaded state transition buffers need to be freed up */
    for (j = 0; j<pAppData->vswmosaicILComp->numInport; j++)
    {
      IL_CLIENT_INPORT_PARAMS *inPortParamsPtr = pAppData->vswmosaicILComp->inPortParams+j;
      /* During idle-> loaded state transition buffers need to be freed up */
      for (i = 0; i < inPortParamsPtr->nBufferCountActual; i++)
      {
        eError =
          OMX_FreeBuffer (pAppData->pVswmosaicHandle, 
                          OMX_VSWMOSAIC_INPUT_PORT_START_INDEX+j,
                          inPortParamsPtr->pInBuff[i]);
        if (eError != OMX_ErrorNone)
        {
          printf ("Error in OMX_FreeBuffer : %s \n", IL_ClientErrorToStr (eError));
          goto EXIT;
        }
      }
    }
    for (i = 0; 
         i < pAppData->vswmosaicILComp->outPortParams->nBufferCountActual; i++)
    {
      eError =
        OMX_FreeBuffer (pAppData->pVswmosaicHandle, 
                        OMX_VSWMOSAIC_OUTPUT_PORT_START_INDEX,
                        pAppData->vswmosaicILComp->outPortParams->pOutBuff[i]);
      if (eError != OMX_ErrorNone)
      {
        printf ("Error in OMX_FreeBuffer : %s \n", IL_ClientErrorToStr (eError));
        goto EXIT;
      }
    }

    semp_pend (pAppData->vswmosaicILComp->done_sem);

    printf ("VSWMOSAIC state loaded \n");
  }
  eError =
    OMX_SendCommand (pAppData->pDisHandle, OMX_CommandStateSet,
                     OMX_StateLoaded, NULL);
  if (eError != OMX_ErrorNone)
  {
    printf ("Error from SendCommand-Idle State set :%s \n",
            IL_ClientErrorToStr (eError));
    goto EXIT;
  }
  /* During idle-> loaded state transition buffers need to be freed up */
  for (i = 0; i < pAppData->disILComp->inPortParams->nBufferCountActual; i++)
  {
    eError =
      OMX_FreeBuffer (pAppData->pDisHandle, OMX_VFDC_INPUT_PORT_START_INDEX,
                      pAppData->disILComp->inPortParams->pInBuff[i]);
    if (eError != OMX_ErrorNone)
    {
      printf ("Error in OMX_FreeBuffer : %s \n", IL_ClientErrorToStr (eError));
      goto EXIT;
    }
  }

  semp_pend (pAppData->disILComp->done_sem);

  printf ("display state loaded \n");

  /* control component does not alloc/free any data buffers, It's interface is
     though as it is omx componenet */
  eError =
    OMX_SendCommand (pAppData->pctrlHandle, OMX_CommandStateSet,
                     OMX_StateLoaded, NULL);
  if (eError != OMX_ErrorNone)
  {
    printf ("Error in SendCommand()-OMX_StateLoaded State set : %s \n",
            IL_ClientErrorToStr (eError));
    goto EXIT;
  }

  semp_pend (pAppData->disILComp->done_sem);

  printf ("ctrl-dc state loaded \n");

  /* free handle for all component */

 for (i = 0; i < g_max_decode; i++) {
  eError = OMX_FreeHandle (pAppData->pDecHandle[i]);
  if ((eError != OMX_ErrorNone))
  {
    printf ("Error in Free Handle function : %s \n",
            IL_ClientErrorToStr (eError));
    goto EXIT;
  }
  printf ("decoder free handle \n");

  eError = OMX_FreeHandle (pAppData->pScHandle[i]);
  if ((eError != OMX_ErrorNone))
  {
    printf ("Error in Free Handle function : %s \n",
            IL_ClientErrorToStr (eError));
    goto EXIT;
  }

  printf ("scalar free handle \n");
 }
  if ( bUseSwMosaic  == OMX_TRUE ) 
  {
    eError = OMX_FreeHandle (pAppData->pVswmosaicHandle);
    if ((eError != OMX_ErrorNone))
    {
      printf ("Error in Free Handle function : %s \n",
              IL_ClientErrorToStr (eError));
      goto EXIT;
    }

    printf ("vswmosaic free handle \n");
  }
  eError = OMX_FreeHandle (pAppData->pDisHandle);
  if ((eError != OMX_ErrorNone))
  {
    printf ("Error in Free Handle function : %s \n",
            IL_ClientErrorToStr (eError));
    goto EXIT;
  }

  printf ("display free handle \n");

  eError = OMX_FreeHandle (pAppData->pctrlHandle);
  if ((eError != OMX_ErrorNone))
  {
    printf ("Error in Free Handle function : %s \n",
            IL_ClientErrorToStr (eError));
    goto EXIT;
  }

  printf ("ctrl-dc free handle \n");

 for (i = 0; i < g_max_decode; i++) {
   if (pAppData->fIn[i] != NULL)
   {
     fclose (pAppData->fIn[i]);
     pAppData->fIn[i] = NULL;
   }
 }
  printf(" files closed \n");
  
  /* terminate the threads */
  if (args->gfx)
  {
    pAppData->gfx.terminateGfx = 1;
    while (!pAppData->gfx.terminateDone);
    pthread_join (pAppData->gfx.ThrdId, (void **) &ret_value);
  }
 for (i = 0; i < g_max_decode; i++) {
   pthread_join (pAppData->decILComp[i]->inDataStrmThrdId, (void **) &ret_value);

   pthread_join (pAppData->decILComp[i]->connDataStrmThrdId, (void **) &ret_value);

   pthread_join (pAppData->scILComp[i]->connDataStrmThrdId, (void **) &ret_value);
 } 

  printf(" decode / scale threads closed \n");
  
  if ( bUseSwMosaic  == OMX_TRUE ) 
  {
    pthread_join (pAppData->vswmosaicILComp->connDataStrmThrdId, (void **) &ret_value);
  }
  
  printf(" vswmosaicILComp threads closed \n");
  
  pthread_join (pAppData->disILComp->connDataStrmThrdId, (void **) &ret_value);

  printf(" display threads closed \n");
  
  IL_ClientDeInit (pAppData);

  printf ("IL Client deinitialized \n");

  printf ("example exit \n");

EXIT:
  return (0);
}

/* ilclient.c - Nothing beyond this point */
