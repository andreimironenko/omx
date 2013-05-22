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
 *  @file  encode_test.c
 *  @brief This file contains all Functions related to Test Application
 *
 *         This is the example IL Client support to create, configure & chaining
 *         of multi channel omx-components using proprietary tunneling 
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
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <xdc/std.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Timestamp.h>
#include "timm_osal_trace.h"
/*-------------------------program files -------------------------------------*/
#include "ti/omx/interfaces/openMaxv11/OMX_Core.h"
#include "ti/omx/interfaces/openMaxv11/OMX_Component.h"
#include "ilclient.h"
#include "ilclient_utils.h"
#include "OMX_TI_Common.h"
#include "OMX_TI_Index.h"
#include "OMX_TI_Video.h"
#include "timm_osal_interfaces.h"
#include "omx_venc.h"

OMX_BOOL gILClientExit = OMX_FALSE;
OMX_BOOL gILClientExitRead = OMX_FALSE;

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
OMX_ERRORTYPE IL_ClientCbEventHandler (OMX_HANDLETYPE hComponent,
                                              OMX_PTR ptrAppData,
                                              OMX_EVENTTYPE eEvent,
                                              OMX_U32 nData1, OMX_U32 nData2,
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
    else if (OMX_CommandFlush == nData1) {
     printf(" OMX_CommandFlush completed \n");
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
     remote component as "consumed buffer " */

  if (inPortParamsPtr->connInfo.remotePipe[0] == NULL)
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
     remote component as "filled buffer " */
  if (outPortParamsPtr->connInfo.remotePipe[0] == NULL)
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
       other compoenent. This message will be read in buffer processing thread
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
int g_EXIT_TASK = 0;

void
IL_ClientInputBitStreamReadTask (void *threadsArg)
{
  unsigned int dataRead = 0, frameSize = 0, frameCounter = IL_CLIENT_ENC_INPUT_BUFFER_COUNT;
  OMX_ERRORTYPE err = OMX_ErrorNone;
  IL_CLIENT_COMP_PRIVATE *encILComp = NULL;
  OMX_BUFFERHEADERTYPE *pBufferIn = NULL;

  encILComp = ((IL_Client *) threadsArg)->encILComp;

  /* use the initial i/p buffers and make empty this buffer calls */
  err = IL_ClientEncUseInitialInputResources (threadsArg);

  while (1)
  {
    /* Read empty buffer pointer from the pipe */
    read (encILComp->inPortParams->ipBufPipe[0],
          &pBufferIn, sizeof (pBufferIn));
    
    frameSize = (((IL_Client *) threadsArg)->nHeight * ((IL_Client *) threadsArg)->nWidth * 3) >> 1;
    /* Fill the data in the empty buffer */
    dataRead = fread (pBufferIn->pBuffer, 1, frameSize, ((IL_Client *) threadsArg)->fIn);
    /* Exit the loop if no data available */

    if ((0 >= dataRead) || (gILClientExitRead == OMX_TRUE))
    {
      g_EXIT_TASK = 1;
      encILComp->numFrames = frameCounter; 
      printf ("No data available for Read encILComp->numFrames %d \n", frameCounter);
      printf(" read task exiting \n ");
      semp_post(encILComp->eos);
     // break;
      pthread_exit (NULL);
      /* can be handled as EOS .. encILComp->inPortParams->flagInputEos =
         OMX_TRUE; */
    }

    pBufferIn->nFilledLen = frameSize;
    pBufferIn->nOffset = 0;
    pBufferIn->nAllocLen = frameSize;
    pBufferIn->nInputPortIndex = 0;
    
    /* Pass the input buffer to the component */
    err = OMX_EmptyThisBuffer (encILComp->handle, pBufferIn);
    
    frameCounter++;    
    if (OMX_ErrorNone != err)
    {
      /* put back the frame in pipe and wait for state change */
      write (encILComp->inPortParams->ipBufPipe[1],
             &pBufferIn, sizeof (pBufferIn));
      printf (" waiting for action from IL Cleint \n");

      /* since in this example we are changing states in other thread it will
         return error for giving ETB/FTB calls in non-execute state. Since
         example is shutting down, we exit the thread */

      pthread_exit (encILComp);

    }
  }

}

/* ========================================================================== */
/**
* IL_ClientOutputBitStreamWriteTask() : This task function is file writetask for
* encoder component. 
*
* @param threadsArg        : Handle to the application
*
*/
/* ========================================================================== */
static int frame_count = 0;
void IL_ClientOutputBitStreamWriteTask (void *threadsArg)
{
  unsigned int frame_counter = 0;
  OMX_ERRORTYPE err = OMX_ErrorNone;
  IL_CLIENT_COMP_PRIVATE *encILComp = NULL;
  OMX_BUFFERHEADERTYPE *pBufferOut = NULL;
  
  OMX_VIDEO_CONFIG_GDRINFOTYPE gdrInfo;
  OMX_VIDEO_CONFIG_DYNAMICPARAMS dynamicParam;

  encILComp = ((IL_Client *) threadsArg)->encILComp;

  /* use the initial i/p buffers and make empty this buffer calls */
  err = IL_ClientEncUseInitialOutputResources (encILComp);

  while (1)
  {
    /* Read filled buffer pointer from the pipe */
    read (encILComp->outPortParams->opBufPipe[0],
          &pBufferOut, sizeof (pBufferOut));
    /* write data to output file */
    fwrite (pBufferOut->pBuffer,
            sizeof (char),
            pBufferOut->nFilledLen, ((IL_Client *) threadsArg)->fOut);
    frame_counter++;
    frame_count = frame_counter;
    if((frame_counter == encILComp->numFrames) || (gILClientExit == 1) || (g_EXIT_TASK == 1))
    {
      gILClientExitRead = 1;
      printf(" write task exited \n");
      semp_post(encILComp->eos);
      pthread_exit(NULL);
      break;
    }
    
#if ENABLE_GDR	
    if(frame_count == 10)
    {
        OMX_INIT_PARAM (&gdrInfo);
        gdrInfo.enableGDR = 1;
        err = OMX_SetConfig (((IL_Client *) threadsArg)->pEncHandle,
                                (OMX_INDEXTYPE) OMX_TI_IndexConfigGDRSettings,
                                (OMX_PTR)&gdrInfo);
    }

    if(frame_count == 11)
    {        
        OMX_INIT_PARAM (&gdrInfo);
        err = OMX_GetConfig (((IL_Client *) threadsArg)->pEncHandle,
                            (OMX_INDEXTYPE) OMX_TI_IndexConfigGDRSettings,
                            (OMX_PTR)&gdrInfo);
        printf (" Get gdr status %d enableGDR %d intraRefreshRateGDRDynamic %d gdrOverlapRowsBtwFramesDynamic %d\n", 
               err, gdrInfo.enableGDR, gdrInfo.intraRefreshRateGDRDynamic, gdrInfo.gdrOverlapRowsBtwFramesDynamic);
    }   
#endif /* ENABLE_GDR */	

    /* Pass the input buffer to the component */
    err = OMX_FillThisBuffer (encILComp->handle, pBufferOut);

    if (OMX_ErrorNone != err)
    {
      /* put back the frame in pipe and wait for state change */
      write (encILComp->outPortParams->opBufPipe[1],
             &pBufferOut, sizeof (pBufferOut));
      printf (" waiting for action from IL Client \n");

      /* since in this example we are changing states in other thread it will
         return error for giving ETB/FTB calls in non-execute state. Since
         example is shutting down, we exit the thread */

      pthread_exit (encILComp);

    }
  }

}

/* ========================================================================== */
/**
* IL_ClientSIGINTHandler() : This function is the SIGINT handler that will be
* called when the user invokes CTRL-C. This is for demonstration purpose. Also
* it assumes that the OMX chain is in EXECUTING state when CTRL-C is invoked
*
* @param sig             : Signal identifier
*/
/* ========================================================================== */

void IL_ClientSIGINTHandler(int sig)
{
 gILClientExit = OMX_TRUE;
}


Int Encode_Example (IL_ARGS *args)
{

  OMX_U32 i;
  OMX_S32 ret_value;
  IL_Client *pAppData = NULL;
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  //IL_CLIENT_PIPE_MSG pipeMsg;

  /* Initialize application specific data structures and buffer management
     data structure */
  IL_ClientInit (&pAppData, args->width, args->height, args->frame_rate, 
               args->bit_rate, args->codec);

  printf (" opening file \n");
  /* Open the file of data to be rendered.  */
  pAppData->fIn = fopen (args->input_file, "rb");;

  if (pAppData->fIn == NULL)
  {
    printf ("Error: failed to open the file %s for reading\n",
            args->input_file);
    goto EXIT;
  }

  /* Open the file of data to be rendered.  */
  pAppData->fOut = fopen (args->output_file, "wb");

  if (pAppData->fOut == NULL)
  {
    printf ("Error: failed to open the file %s for writing \n",
            args->output_file);
    goto EXIT;
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

/******************************************************************************/
  /* Create the H264 encoder Component, component handle would be returned
     component name is unique and fixed for a componnet, callback are passed
     to componnet in this function. component would be loaded state post this
     call */

  eError =
    OMX_GetHandle (&pAppData->pEncHandle,
                   (OMX_STRING) "OMX.TI.DUCATI.VIDENC", pAppData->encILComp,
                   &pAppData->pCb);

  printf (" encoder component is created \n");

  if ((eError != OMX_ErrorNone) || (pAppData->pEncHandle == NULL))
  {
    printf ("Error in Get Handle function : %s \n",
            IL_ClientErrorToStr (eError));
    goto EXIT;
  }

  pAppData->encILComp->handle = pAppData->pEncHandle;

  /* Configute the encode componet, ports are default enabled for encode comp,
     so no need to enable from IL Client */
  /* calling OMX_Setparam in this function */
  IL_ClientSetEncodeParams (pAppData);

/*******************************************************************************/

  /* OMX_SendCommand expecting OMX_StateIdle, after this command component
     would create codec, and will wait for all buffers to be allocated */
  eError =
    OMX_SendCommand (pAppData->pEncHandle, OMX_CommandStateSet,
                     OMX_StateIdle, NULL);
  if (eError != OMX_ErrorNone)
  {
    printf ("Error in SendCommand()-OMX_StateIdle State set : %s \n",
            IL_ClientErrorToStr (eError));
    goto EXIT;
  }
  /* since encoder is connected to dei, buffers are supplied by dei to
     encoder, so encoder does not allocate the buffers. However it is informed
     to use the buffers created by dei. encode component would create only
     buffer headers corresponding to these buffers */

  for (i = 0; i < pAppData->encILComp->inPortParams->nBufferCountActual; i++)
  {

    eError = OMX_AllocateBuffer (pAppData->pEncHandle,
                            &pAppData->encILComp->inPortParams->pInBuff[i],
                            OMX_VIDENC_INPUT_PORT,
                            pAppData,
                            pAppData->encILComp->inPortParams->nBufferSize);

    if (eError != OMX_ErrorNone)
    {
      printf ("Error in encode OMX_UseBuffer(): %s \n",
              IL_ClientErrorToStr (eError));
      goto EXIT;
    }
  }
  printf (" encoder input port allocate buffer done \n ");

  /* in SDK conventionally output port allocates the buffers, encode would
     create the buffers which would be consumed by filewrite thread */
  /* buffer alloaction for output port */
  for (i = 0; i < pAppData->encILComp->outPortParams->nBufferCountActual; i++)
  {
    eError = OMX_AllocateBuffer (pAppData->pEncHandle,
                                 &pAppData->encILComp->outPortParams->
                                 pOutBuff[i], OMX_VIDENC_OUTPUT_PORT,
                                 pAppData,
                                 pAppData->encILComp->outPortParams->
                                 nBufferSize);
    if (eError != OMX_ErrorNone)
    {
      printf ("Error in OMX_AllocateBuffer()-Output Port State set : %s \n",
              IL_ClientErrorToStr (eError));
      goto EXIT;
    }
  }

  printf (" encoder outport buffers allocated \n ");

  semp_pend (pAppData->encILComp->done_sem);

  printf (" Encoder state IDLE \n ");
/******************************************************************************/
/******************************************************************************/

  /* change state to execute so that buffers processing can start */
  eError =
    OMX_SendCommand (pAppData->pEncHandle, OMX_CommandStateSet,
                     OMX_StateExecuting, NULL);
  if (eError != OMX_ErrorNone)
  {
    printf ("Error from SendCommand-Executing State set :%s \n",
            IL_ClientErrorToStr (eError));
    goto EXIT;
  }

  semp_pend (pAppData->encILComp->done_sem);

  printf (" encoder state execute \n ");
/******************************************************************************/

  /* Create thread for writing bitstream and passing the buffers to encoder
     component */
  pthread_attr_init (&pAppData->encILComp->ThreadAttr);

  if (0 !=
      pthread_create (&pAppData->encILComp->outDataStrmThrdId,
                      &pAppData->encILComp->ThreadAttr,
                      (ILC_StartFcnPtr) IL_ClientOutputBitStreamWriteTask, pAppData))
  {
    printf ("Create_Task failed !");
    goto EXIT;
  }

  printf (" file write thread created \n ");

  pthread_attr_init (&pAppData->encILComp->ThreadAttr);
  if (0 !=
      pthread_create (&pAppData->encILComp->inDataStrmThrdId,
                      &pAppData->encILComp->ThreadAttr,
                      (ILC_StartFcnPtr) IL_ClientInputBitStreamReadTask, pAppData))
  {
    printf ("Create_Task failed !");
    goto EXIT;
  }

  printf (" file read thread created \n ");
/******************************************************************************/
  /* Waiting for this semaphore to be posted by the bitstream write thread */
  semp_pend(pAppData->encILComp->eos);
  semp_pend(pAppData->encILComp->eos);
  printf(" received eof \n");
/******************************************************************************/



  /* change state to execute so that buffers processing can stop */
  eError =
  OMX_SendCommand (pAppData->pEncHandle, OMX_CommandStateSet,
                   OMX_StateIdle, NULL);
  if (eError != OMX_ErrorNone)
  {
    printf ("Error from SendCommand-Idle State set :%s \n",
            IL_ClientErrorToStr (eError));
    goto EXIT;
  }

  semp_pend (pAppData->encILComp->done_sem);

  printf (" Encoder state idle \n ");

/******************************************************************************/

  /* change the encoder state to loded */
  eError =
    OMX_SendCommand (pAppData->pEncHandle, OMX_CommandStateSet,
                     OMX_StateLoaded, NULL);
  if (eError != OMX_ErrorNone)
  {
    printf ("Error from SendCommand-Idle State set :%s \n",
            IL_ClientErrorToStr (eError));
    goto EXIT;
  }
  /* During idle-> loaded state transition buffers need to be freed up */
  for (i = 0; i < pAppData->encILComp->inPortParams->nBufferCountActual; i++)
  {
    eError =
      OMX_FreeBuffer (pAppData->pEncHandle, OMX_VIDENC_INPUT_PORT,
                      pAppData->encILComp->inPortParams->pInBuff[i]);
    if (eError != OMX_ErrorNone)
    {
      printf ("Error in OMX_FreeBuffer : %s \n", IL_ClientErrorToStr (eError));
      goto EXIT;
    }
  }

  for (i = 0; i < pAppData->encILComp->outPortParams->nBufferCountActual; i++)
  {
    eError =
      OMX_FreeBuffer (pAppData->pEncHandle, OMX_VIDENC_OUTPUT_PORT,
                      pAppData->encILComp->outPortParams->pOutBuff[i]);
    if (eError != OMX_ErrorNone)
    {
      printf ("Error in OMX_FreeBuffer : %s \n", IL_ClientErrorToStr (eError));
      goto EXIT;
    }
  }

  semp_pend (pAppData->encILComp->done_sem);

  printf (" encoder state loaded \n ");
/******************************************************************************/

  eError = OMX_FreeHandle (pAppData->pEncHandle);
  if ((eError != OMX_ErrorNone))
  {
    printf ("Error in Free Handle function : %s \n",
            IL_ClientErrorToStr (eError));
    goto EXIT;
  }
  printf (" encoder free handle \n");

 
  pthread_join (pAppData->encILComp->inDataStrmThrdId, (void **) &ret_value);

  pthread_join(pAppData->encILComp->outDataStrmThrdId, (void **) &ret_value);

  fclose(pAppData->fIn);
  
  fclose(pAppData->fOut);
  
  IL_ClientDeInit (pAppData);

  printf ("IL Client deinitialized \n");

  printf (" example exit \n");

EXIT:
  return (0);
}
