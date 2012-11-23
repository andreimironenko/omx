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
/*None*/

/*******************************************************************************
*                             INCLUDE FILES
*******************************************************************************/

/*--------------------- system and platform files ----------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <xdc/std.h>

/*-------------------------program files -------------------------------------*/
#include "ti/omx/interfaces/openMaxv11/OMX_Core.h"
#include "ti/omx/interfaces/openMaxv11/OMX_Component.h"
#include "ilclient.h"
#include "ilclient_utils.h"
#include <omx_vdec.h>
#include <omx_vfpc.h>
#include <omx_vfdc.h>
#include <omx_ctrl.h>

extern void IL_ClientFbDevAppTask (void *threadsArg);
OMX_BOOL gILClientExit = OMX_FALSE;

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
    getchar ();
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
* IL_ClientInputFrameReadTask() : This task function is file read task for
* display component.
*
* @param threadsArg        : Handle to the application
*
*/
/* ========================================================================== */

void
IL_ClientInputFrameReadTask (void *threadsArg)
{
  unsigned int dataRead = 0;
  OMX_ERRORTYPE err = OMX_ErrorNone;
  IL_CLIENT_COMP_PRIVATE *disILComp = NULL;
  OMX_BUFFERHEADERTYPE *pBufferIn = NULL;

  disILComp = ((IL_Client *) threadsArg)->disILComp;

  /* use the initial i/p buffers and make empty this buffer calls */
  err = IL_ClientDisUseInitialInputResources (threadsArg);

  while (1)
  {
    /* Read empty buffer pointer from the pipe */
    read (disILComp->inPortParams->ipBufPipe[0],
          &pBufferIn, sizeof (pBufferIn));

    /* Fill the data in the empty buffer */
    dataRead = IL_ClientFillData (threadsArg, pBufferIn);

    /* Exit the loop if no data available */
    if ((0 >= dataRead ) || (gILClientExit == OMX_TRUE))
    {
      printf ("No data available for Read\n");
      pBufferIn->nFlags |= OMX_BUFFERFLAG_EOS;
      err = OMX_EmptyThisBuffer (disILComp->handle, pBufferIn);
      pthread_exit (disILComp);
      /* can be handled as EOS .. disILComp->inPortParams->flagInputEos =
         OMX_TRUE; */
      break;
    }

    /* Pass the input buffer to the component */
    err = OMX_EmptyThisBuffer (disILComp->handle, pBufferIn);

    if (OMX_ErrorNone != err)
    {
      /* put back the frame in pipe and wait for state change */
      write (disILComp->inPortParams->ipBufPipe[1],
             &pBufferIn, sizeof (pBufferIn));
      printf (" waiting for action from IL Cleint \n");

      /* since in this example we are changing states in other thread it will
         return error for giving ETB/FTB calls in non-execute state. Since
         example is shutting down, we exit the thread */

      pthread_exit (disILComp);

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

Int Display_Example (IL_ARGS *args)
{

  OMX_U32 i;
  OMX_S32 ret_value;
  IL_Client *pAppData = NULL;
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  IL_CLIENT_PIPE_MSG pipeMsg;

  /* Initialize application specific data structures and buffer management
     data structure */
  IL_ClientInit (&pAppData, args->display_id);

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

  /* Display does not have an output port. Hence setting the Fill Buffer Done CB 
     to NULL*/
  pAppData->pCb.FillBufferDone = NULL;


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

  printf (" got display handle \n");
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

  printf (" ctrl-dc state IDLE \n ");

  eError =
    OMX_SendCommand (pAppData->pDisHandle, OMX_CommandStateSet,
                     OMX_StateIdle, NULL);
  if (eError != OMX_ErrorNone)
  {
    printf ("Error in SendCommand()-OMX_StateIdle State set : %s \n",
            IL_ClientErrorToStr (eError));
    goto EXIT;
  }
  /* Have to Allocate Buffers at the input port of the Display component as
   * there is no source OMX components to get buffers from. */

  for (i = 0; i < pAppData->disILComp->inPortParams->nBufferCountActual; i++)
  {

    eError = OMX_AllocateBuffer (pAppData->pDisHandle,
                                 &pAppData->disILComp->inPortParams->pInBuff[i],
                                 OMX_VFDC_INPUT_PORT_START_INDEX,
                                 pAppData->disILComp,
                                 pAppData->disILComp->inPortParams->nBufferSize);

    if (eError != OMX_ErrorNone)
    {
      printf ("Error in Display OMX_AllocateBuffer()- %s \n",
              IL_ClientErrorToStr (eError));
      goto EXIT;
    }
  }

  semp_pend (pAppData->disILComp->done_sem);

  printf (" display state IDLE \n ");

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

  printf (" display control state execute \n ");

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

  printf (" display state execute \n ");


  /* Create thread for reading bitstream and passing the buffers to decoder
     component */
  /* This thread would take the h264 elementary stream , parses it, give a
     frame to decoder at the input port. decoder */
  pthread_attr_init (&pAppData->disILComp->ThreadAttr);

  if (0 !=
      pthread_create (&pAppData->disILComp->inDataStrmThrdId,
                      &pAppData->disILComp->ThreadAttr,
                      IL_ClientInputFrameReadTask, pAppData))
  {
    printf ("Create_Task failed !");
    goto EXIT;
  }

  printf (" file read thread created \n ");

 
  printf (" executing the appliaction now!!!\n ");

  /* Waiting for this semaphore to be posted by the bitstream read thread */
  semp_pend (pAppData->disILComp->eos);

  printf (" tearing down the display example\n ");

  /* change the state to idle */
  /* before changing state to idle, buffer communication to component should be 
     stoped , writing an exit messages to threads */

  pipeMsg.cmd = IL_CLIENT_PIPE_CMD_EXIT;

  write (pAppData->disILComp->localPipe[1],
         &pipeMsg, sizeof (IL_CLIENT_PIPE_MSG));

  /* change state to idle so that buffers processing can stop */
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

  printf (" display state idle \n ");

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

  printf (" display control state idle \n ");

  /* change the state to loded */

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

  printf (" display state loaded \n ");

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

  printf (" ctrl-dc state loaded \n ");

  /* free handle for all component */

  eError = OMX_FreeHandle (pAppData->pDisHandle);
  if ((eError != OMX_ErrorNone))
  {
    printf ("Error in Free Handle function : %s \n",
            IL_ClientErrorToStr (eError));
    goto EXIT;
  }

  printf (" display free handle \n");

  eError = OMX_FreeHandle (pAppData->pctrlHandle);
  if ((eError != OMX_ErrorNone))
  {
    printf ("Error in Free Handle function : %s \n",
            IL_ClientErrorToStr (eError));
    goto EXIT;
  }

  printf (" ctrl-dc free handle \n");

  if (pAppData->fIn != NULL)
  {
    fclose (pAppData->fIn);
    pAppData->fIn = NULL;
  }

  pthread_join (pAppData->disILComp->inDataStrmThrdId, &ret_value);

  IL_ClientDeInit (pAppData);

  printf ("IL Client deinitialized \n");

  printf (" example exit \n");

EXIT:
  return (0);
}

/* Nothing beyond this point */
