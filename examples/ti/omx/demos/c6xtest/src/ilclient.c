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
 *  @file  capture_encode_test.c
 *  @brief This file contains all Functions related to Test Application
 *
 *         This is the example IL Client support to create, configure & chaining
 *         of single channel omx-components using non tunneling 
 *         mode
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

/*--------------------- system and platform files ----------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <xdc/std.h>

/*-------------------------program files -------------------------------------*/
#include "ti/omx/interfaces/openMaxv11/OMX_Core.h"
#include "ti/omx/interfaces/openMaxv11/OMX_Component.h"
#include "ilclient.h"
#include "ilclient_utils.h"
#include <omx_vlpb.h>

typedef void *(*ILC_StartFcnPtr) (void *);
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
static OMX_ERRORTYPE IL_ClientCbEventHandler (OMX_HANDLETYPE hComponent,
                                              OMX_PTR ptrAppData,
                                              OMX_EVENTTYPE eEvent,
                                              OMX_U32 nData1,
                                              OMX_U32 nData2,
                                              OMX_PTR pEventData)
{
  IL_CLIENT_COMP_PRIVATE *comp;

  comp = ptrAppData;

  printf ("got event");
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
  IL_CLIENT_PIPE_MSG localPipeMsg;
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  int retVal = 0;

  if (thisComp->stopFlag)
  {
    return eError;
  }

  /* Create a message that EBD is done and this buffer is ready to be recycled
     through a ETB call */
  localPipeMsg.cmd = IL_CLIENT_PIPE_CMD_ETB;
  memcpy (&localPipeMsg.bufHeader, pBuffer, sizeof (OMX_BUFFERHEADERTYPE));

  retVal = write (thisComp->localPipe[1],
                  &localPipeMsg, sizeof (IL_CLIENT_PIPE_MSG));

  if (sizeof (IL_CLIENT_PIPE_MSG) != retVal)
  {
    printf ("Error writing to local Pipe!\n");
    eError = OMX_ErrorNotReady;
    return eError;
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
  IL_CLIENT_PIPE_MSG localPipeMsg;
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  int retVal = 0;
  static int frameCounter = 1;
  char *ptr = (char *)pBuffer->pBuffer;
  int i;

  if (thisComp->stopFlag)
  {
    return eError;
  }

  for(i = 0; i < IL_CLIENT_VLPB_BUFFER_SIZE; i++)
  {
    if(ptr[i] !=IL_CLIENT_VLPB_PATTERN)
    {
      printf ("Copy test failed at frame %d, elem %d\n", frameCounter, i);
      printf ("Value of failed compare %c\n", ptr[i]);
      return (OMX_ErrorStreamCorrupt);
    }
  }

  printf ("Copied frame %d\n", frameCounter++);
  if (frameCounter > (IL_CLIENT_VLPB_MAX_FRAMES))
  {
    /* Terminate this application by posting an EOS semaphore */
    thisComp->stopFlag = 1;
    frameCounter = 1;
    semp_post (thisComp->eos);
    return eError;
  }

  /* Create a message that FBD is done and this buffer is ready to be recycled 
     through a FTB call */
  localPipeMsg.cmd = IL_CLIENT_PIPE_CMD_FTB;
  memcpy (&localPipeMsg.bufHeader, pBuffer, sizeof (OMX_BUFFERHEADERTYPE));

  retVal = write (thisComp->localPipe[1],
                  &localPipeMsg, sizeof (IL_CLIENT_PIPE_MSG));
  if (sizeof (IL_CLIENT_PIPE_MSG) != retVal)
  {
    printf ("Error writing to local Pipe!\n");
    eError = OMX_ErrorNotReady;
    return eError;
  }

  return eError;
}

/* ========================================================================== */
/**
* IL_ClientConnInConnOutTask() : This task function is for passing buffers from
* one component to other connected component. This functions reads from local
* pipe of a perticular component , and takes action based on the message in the
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

  IL_ClientUseInitialInputOutputResources (thisComp);

  for (;;)
  {
    /* Read from its own local Pipe */
    read (thisComp->localPipe[0], &pipeMsg, sizeof (IL_CLIENT_PIPE_MSG));

    /* check the function type */
    switch (pipeMsg.cmd)
    {
      case IL_CLIENT_PIPE_CMD_EXIT:
        printf ("exiting thread \n ");
        pthread_exit (thisComp);
        break;
      case IL_CLIENT_PIPE_CMD_ETB:
        err = IL_ClientProcessPipeCmdETB (thisComp, &pipeMsg);
        /* If not in proper state, bufers may not be accepted by component */
        if (OMX_ErrorNone != err)
        {
          printf (" ETB: Error \n");
          /* since in this example we are changing states in other thread it will
             return error for giving ETB/FTB calls in non-execute state. Since
             example is shutting down, we exit the thread */
          pthread_exit (thisComp);
          /* if error is incorrect state operation, wait for state to change */
          /* waiting mechanism should be implemented here */
        }
        break;
      case IL_CLIENT_PIPE_CMD_FTB:
        err = IL_ClientProcessPipeCmdFTB (thisComp, &pipeMsg);
        if (OMX_ErrorNone != err)
        {
          printf (" FTB: Error \n");
          /* if error is incorrect state operation, wait for state to change */
          /* waiting mechanism should be implemented here */
          /* since in this example we are changing states in other thread it will
             return error for giving ETB/FTB calls in non-execute state. Since
             example is shutting down, we exit the thread */
          pthread_exit (thisComp);
        }
        break;
      default:
        printf ("invalid command\n ");
        pthread_exit (thisComp);
        break;
    }
  }
}

/* ========================================================================== */
/**
* IL_ClientProcessPipeCmdETB() : This function passes the bufefrs to component
* for consuming. This buffer will come from other component as an output. To 
* consume it, IL client finds its bufefr header (for consumer component), and 
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

  if (thisComp->stopFlag)
    return (OMX_ErrorNone);

  /* search its own buffer header based on submitted by connected comp */
  IL_ClientUtilGetSelfBufHeader (thisComp, pipeMsg->bufHeader.pBuffer,
                                 ILCLIENT_INPUT_PORT,
                                 pipeMsg->bufHeader.nInputPortIndex,
                                 &pBufferIn);

  /* populate buffer header */
  pBufferIn->nFilledLen = IL_CLIENT_VLPB_BUFFER_SIZE;
  pBufferIn->nOffset = pipeMsg->bufHeader.nOffset;
  pBufferIn->nTimeStamp = pipeMsg->bufHeader.nTimeStamp;
  pBufferIn->nFlags = pipeMsg->bufHeader.nFlags;
  pBufferIn->hMarkTargetComponent = pipeMsg->bufHeader.hMarkTargetComponent;
  pBufferIn->pMarkData = pipeMsg->bufHeader.pMarkData;
  pBufferIn->nTickCount = 0;

  /* Read input from file */
  memset (pBufferIn->pBuffer, IL_CLIENT_VLPB_PATTERN,
          IL_CLIENT_VLPB_BUFFER_SIZE);

  /* thisComp->stopFlag = 1; */

  /* call etb to the component */
  if (!thisComp->stopFlag)
  {
    err = OMX_EmptyThisBuffer (thisComp->handle, pBufferIn);
  }

  return (err);
}

/* ========================================================================== */
/**
* IL_ClientProcessPipeCmdFTB() : This function passes the bufefrs to component
* for consuming. This buffer will come from other component as consumed at input
* To  consume it, IL client finds its bufefr header (for consumer component),
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

  if (thisComp->stopFlag)
  {
    return (OMX_ErrorNone);
  }

  /* search its own buffer header based on submitted by connected comp */
  IL_ClientUtilGetSelfBufHeader (thisComp, pipeMsg->bufHeader.pBuffer,
                                 ILCLIENT_OUTPUT_PORT,
                                 pipeMsg->bufHeader.nOutputPortIndex,
                                 &pBufferOut);

  /* call etb to the component */
  if (!thisComp->stopFlag)
  {
    err = OMX_FillThisBuffer (thisComp->handle, pBufferOut);
  }

  return (err);
}

/* ========================================================================== */
/**
* Vlpb_Copy_Example() : This method is the IL Client implementation for 
* the OMX copy component (VLPB) on DSP. This function creates configures, and
* connects the components. It manages the buffer communication.
*
* @param args         : parameters( widt,height,n_frames etc) for this function
*
*  @return      
*  OMX_ErrorNone = Successful 
*
*  Other_value = Failed (Error code is returned)
*
*/
/* ========================================================================== */

/* Main IL Client application to create , intiate and connect components */
int Vlpb_Copy_Example ()
{
  IL_Client *pAppData = NULL;
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  OMX_U32 i;
  OMX_S32 ret_value;
  IL_CLIENT_PIPE_MSG pipeMsg;

  /* Initialize application specific data structures and buffer management
     data */
  IL_ClientInit (&pAppData);

  /* do omx_init for sytem level initialization */
  /* Initializing OMX core , functions related to platform specific
     initialization could be placed inside this */

  eError = OMX_Init ();

  printf (" OMX_Init completed \n ");

  /* Initialize application / IL Client callback functions */
  /* Callbacks are passed during getHandle call to component, component uses
     these callabacks to communicate with IL Client */
  /* event handler is to handle the state changes , omx commands and any
     message for IL client */
  pAppData->pCb.EventHandler = IL_ClientCbEventHandler;

  /* Empty buffer done is data callback at the input port, where component let
     the application know that buffer has been consumed, this is not applicabel 
     if there is no input port in the component */
  pAppData->pCb.EmptyBufferDone = IL_ClientCbEmptyBufferDone;

  /* fill buffer done is callback at the output port, where component lets the
     application know that an output buffer is available with the processed data 
   */
  pAppData->pCb.FillBufferDone = IL_ClientCbFillBufferDone;


/******************************************************************************/
  /* Create the VLPB Component, component handle would be returned component
     name is unique and fixed for a componnet, callback are passed to
     componnet in this function. component would be loaded state post this call 
   */

  eError =
    OMX_GetHandle (&pAppData->pVlpbHandle,
                   (OMX_STRING) "OMX.TI.C67X.VLPB", pAppData->vlpbILComp,
                   &pAppData->pCb);

  printf (" vlpb(on dsp) compoenent is created\n");

  if ((eError != OMX_ErrorNone) || (pAppData->pVlpbHandle == NULL))
  {
    printf ("Error in Get Handle function : %s \n",
            IL_ClientErrorToStr (eError));
    goto EXIT;
  }

  pAppData->vlpbILComp->handle = pAppData->pVlpbHandle;

  /* Configute the vlpb componet, enable the ports - calling OMX_Setparam in
     this function */
  IL_ClientSetVlpbParams (pAppData);

  /* enable input and output port */
  /* as per openmax specs all the ports should be enabled by default but EZSDK
     OMX component does not enable it hence we manually need to enable it. */

  printf ("enable vlpb input port \n");
  OMX_SendCommand (pAppData->pVlpbHandle, OMX_CommandPortEnable,
                   OMX_VLPB_INPUT_PORT_START_INDEX, NULL);

  /* wait for both ports to get enabled, event handler would be notified from
     the component after enabling the port, which inturn would post this
     semaphore */
  semp_pend (pAppData->vlpbILComp->port_sem);

  printf ("enable vlpb output port \n");
  OMX_SendCommand (pAppData->pVlpbHandle, OMX_CommandPortEnable,
                   OMX_VLPB_OUTPUT_PORT_START_INDEX, NULL);
  semp_pend (pAppData->vlpbILComp->port_sem);

/******************************************************************************/
/******************************************************************************/

  /* OMX_SendCommand expecting OMX_StateIdle, after this command component will 
     wait for all buffers to be allocated as per omx buffers are created during 
     loaded to Idle transition IF ports are enabled ) */

  eError =
    OMX_SendCommand (pAppData->pVlpbHandle, OMX_CommandStateSet,
                     OMX_StateIdle, NULL);
  if (eError != OMX_ErrorNone)
  {
    printf ("Error in SendCommand()-OMX_StateIdle State set : %s \n",
            IL_ClientErrorToStr (eError));
    goto EXIT;
  }

  /* Allocate I/O Buffers; componnet would allocated buffers and would return
     the buffer header containing the pointer to buffer */
  for (i = 0; i < pAppData->vlpbILComp->inPortParams->nBufferCountActual; i++)
  {
    eError = OMX_AllocateBuffer (pAppData->pVlpbHandle,
                                 &pAppData->vlpbILComp->inPortParams->
                                 pInBuff[i],
                                 OMX_VLPB_INPUT_PORT_START_INDEX, pAppData,
                                 pAppData->vlpbILComp->inPortParams->
                                 nBufferSize);

    if (eError != OMX_ErrorNone)
    {
      printf
        (" VLPB: Error in OMX_AllocateBuffer() : %s \n",
         IL_ClientErrorToStr (eError));
      goto EXIT;
    }
  }
  printf (" vlpb input buffers allocated \n ");

  for (i = 0; i < pAppData->vlpbILComp->outPortParams->nBufferCountActual; i++)
  {
    eError = OMX_AllocateBuffer (pAppData->pVlpbHandle,
                                 &pAppData->vlpbILComp->outPortParams->
                                 pOutBuff[i],
                                 OMX_VLPB_OUTPUT_PORT_START_INDEX, pAppData,
                                 pAppData->vlpbILComp->outPortParams->
                                 nBufferSize);

    if (eError != OMX_ErrorNone)
    {
      printf
        ("Capture: Error in OMX_AllocateBuffer() : %s \n",
         IL_ClientErrorToStr (eError));
      goto EXIT;
    }
  }
  printf (" vlpb outport buffers allocated \n ");

  semp_pend (pAppData->vlpbILComp->done_sem);

  printf (" VLPB is in IDLE state \n");
/******************************************************************************/

/******************************************************************************/

  /* change state tho execute, so that component can accept buffers from IL
     client */
  eError =
    OMX_SendCommand (pAppData->pVlpbHandle, OMX_CommandStateSet,
                     OMX_StateExecuting, NULL);
  if (eError != OMX_ErrorNone)
  {
    printf ("Error in SendCommand()-OMX_StateIdle State set : %s \n",
            IL_ClientErrorToStr (eError));
    goto EXIT;
  }

  semp_pend (pAppData->vlpbILComp->done_sem);

  printf (" vlpb state execute \n ");

/******************************************************************************/

/******************************************************************************/

  /* These threads are created for each component to pass the buffers to each
     other. this thread function reads the buffers from pipe and feeds it to
     component or for processed buffers, passes the buffers to connected
     component */
  pthread_attr_init (&pAppData->vlpbILComp->ThreadAttr);
  if (0 !=
      pthread_create (&pAppData->vlpbILComp->connDataStrmThrdId,
                      &pAppData->vlpbILComp->ThreadAttr,
                      (ILC_StartFcnPtr) IL_ClientConnInConnOutTask, pAppData->vlpbILComp))
  {
    printf ("Create_Task failed !");
    goto EXIT;
  }

  printf (" vlpb connect thread created \n ");

  printf (" executing the application now!!! \n");

/******************************************************************************/
  /* Waiting for this semaphore to be posted by the bitstream write thread */
  semp_pend (pAppData->vlpbILComp->eos);

/******************************************************************************/
  printf (" tearing down the vlpb example\n ");

  /* exit the threads */
  pipeMsg.cmd = IL_CLIENT_PIPE_CMD_EXIT;

  write (pAppData->vlpbILComp->localPipe[1],
         &pipeMsg, sizeof (IL_CLIENT_PIPE_MSG));

  /* terminate the threads */
  do
  {
    pthread_join (pAppData->vlpbILComp->connDataStrmThrdId, (void **) &ret_value);
  } while ((IL_CLIENT_COMP_PRIVATE *) ret_value != pAppData->vlpbILComp);

  /* tear down sequence */
  printf (" change vlpb state to idle \n ");

  /* change state to idle so that buffers processing would stop */
  eError =
    OMX_SendCommand (pAppData->pVlpbHandle, OMX_CommandStateSet,
                     OMX_StateIdle, NULL);
  if (eError != OMX_ErrorNone)
  {
    printf ("Error from SendCommand-Idle State set :%s \n",
            IL_ClientErrorToStr (eError));
    goto EXIT;
  }

  semp_pend (pAppData->vlpbILComp->done_sem);
  printf (" vlpb state idle \n ");

/******************************************************************************/

  eError =
    OMX_SendCommand (pAppData->pVlpbHandle, OMX_CommandStateSet,
                     OMX_StateLoaded, NULL);
  if (eError != OMX_ErrorNone)
  {
    printf ("Error from SendCommand-Loaded State set :%s \n",
            IL_ClientErrorToStr (eError));
    goto EXIT;
  }
  /* During idle-> loaded state transition buffers need to be freed up */
  for (i = 0; i < pAppData->vlpbILComp->inPortParams->nBufferCountActual; i++)
  {
    eError =
      OMX_FreeBuffer (pAppData->pVlpbHandle,
                      OMX_VLPB_INPUT_PORT_START_INDEX,
                      pAppData->vlpbILComp->inPortParams->pInBuff[i]);
    if (eError != OMX_ErrorNone)
    {
      printf ("Error in OMX_FreeBuffer : %s \n", IL_ClientErrorToStr (eError));
      goto EXIT;
    }
  }

  for (i = 0; i < pAppData->vlpbILComp->outPortParams->nBufferCountActual; i++)
  {
    eError =
      OMX_FreeBuffer (pAppData->pVlpbHandle,
                      OMX_VLPB_OUTPUT_PORT_START_INDEX,
                      pAppData->vlpbILComp->outPortParams->pOutBuff[i]);
    if (eError != OMX_ErrorNone)
    {
      printf ("Error in OMX_FreeBuffer : %s \n", IL_ClientErrorToStr (eError));
      goto EXIT;
    }
  }

  semp_pend (pAppData->vlpbILComp->done_sem);

  printf (" vlpb state loaded \n ");

/******************************************************************************/

  /* free handle for all component */

  eError = OMX_FreeHandle (pAppData->pVlpbHandle);
  if ((eError != OMX_ErrorNone))
  {
    printf ("Error in Free Handle function : %s \n",
            IL_ClientErrorToStr (eError));
    goto EXIT;
  }
  printf (" vlpb free handle \n");

  IL_ClientDeInit (pAppData);

  printf ("IL Client deinitialized \n");

  printf (" example exit \n");

EXIT:
  return (0);

}

/* Nothing beyond this point */
