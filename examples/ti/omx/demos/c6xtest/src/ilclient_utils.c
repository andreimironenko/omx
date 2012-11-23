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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <xdc/std.h>
#include <memory.h>
#include <getopt.h>

/*-------------------------program files -------------------------------------*/
#include "ti/omx/interfaces/openMaxv11/OMX_Core.h"
#include "ti/omx/interfaces/openMaxv11/OMX_Component.h"
#include "OMX_TI_Common.h"
#include "OMX_Video.h"
#include "OMX_TI_Video.h"
#include "ilclient.h"
#include "ilclient_utils.h"
#include <omx_vlpb.h>
#include <OMX_TI_Index.h>
/*---------------------- function prototypes ---------------------------------*/
/* None */

/* ========================================================================== */
/**
* IL_ClientInit() : This function is to allocate and initialize the application
*                   data structure. It is just to maintain application control.
*
* @param pAppData          : appliaction / client data Handle 
* @param width             : stream width
* @param height            : stream height
* @param numFrames         : encoded number of frames
*
*  @return      
*
*
*/
/* ========================================================================== */

void IL_ClientInit (IL_Client **pAppData)
{
  int i;
  IL_Client *pAppDataPtr;
  IL_CLIENT_INPORT_PARAMS *inPortParamsPtr;
  IL_CLIENT_OUTPORT_PARAMS *outPortParamsPtr;

  /* Allocating data structure for IL client structure / buffer management */

  pAppDataPtr = (IL_Client *) malloc (sizeof (IL_Client));
  memset (pAppDataPtr, 0x00, sizeof (IL_Client));

  /* alloacte data structure for each component used in this IL Client */
  pAppDataPtr->vlpbILComp =
    (IL_CLIENT_COMP_PRIVATE *) malloc (sizeof (IL_CLIENT_COMP_PRIVATE));
  memset (pAppDataPtr->vlpbILComp, 0x00, sizeof (IL_CLIENT_COMP_PRIVATE));

  /* these semaphores are used for tracking the callbacks received from
     component */
  pAppDataPtr->vlpbILComp->eos = malloc (sizeof (semp_t));
  semp_init (pAppDataPtr->vlpbILComp->eos, 0);

  pAppDataPtr->vlpbILComp->done_sem = malloc (sizeof (semp_t));
  semp_init (pAppDataPtr->vlpbILComp->done_sem, 0);

  pAppDataPtr->vlpbILComp->port_sem = malloc (sizeof (semp_t));
  semp_init (pAppDataPtr->vlpbILComp->port_sem, 0);

  /* number of ports for each component, which this IL client will handle, this 
     will be equal to number of ports supported by component or less */

  pAppDataPtr->vlpbILComp->numInport = 1;
  pAppDataPtr->vlpbILComp->numOutport = 1;
  pAppDataPtr->vlpbILComp->startOutportIndex = OMX_VLPB_OUTPUT_PORT_START_INDEX;

  /* allocate data structure for input and output port params of IL client
     component, It is for maintaining data structure in IL Client only.
     Components will have its own data structure inside omx components */

  pAppDataPtr->vlpbILComp->inPortParams =
    malloc (sizeof (IL_CLIENT_INPORT_PARAMS) *
            pAppDataPtr->vlpbILComp->numInport);
  memset (pAppDataPtr->vlpbILComp->inPortParams, 0x00,
          sizeof (IL_CLIENT_INPORT_PARAMS));

  pAppDataPtr->vlpbILComp->outPortParams =
    malloc (sizeof (IL_CLIENT_INPORT_PARAMS) *
            pAppDataPtr->vlpbILComp->numOutport);
  memset (pAppDataPtr->vlpbILComp->outPortParams, 0x00,
          sizeof (IL_CLIENT_OUTPORT_PARAMS));

  /* specify some of the parameters, that will be used for initializing OMX
     component parameters */

  /* each componet will have local pipe to take bufffes from other component or 
     its own consumed buffer, so that it can be passed to other connected
     components */

  for (i = 0; i < pAppDataPtr->vlpbILComp->numInport; i++)
  {
    inPortParamsPtr = pAppDataPtr->vlpbILComp->inPortParams + i;
    inPortParamsPtr->nBufferCountActual = IL_CLIENT_VLPB_INPUT_BUFFER_COUNT;
    /* input buffers size for yuv buffers, format is YUV420 hence 3/2 */
    inPortParamsPtr->nBufferSize = IL_CLIENT_VLPB_BUFFER_SIZE;
  }
  for (i = 0; i < pAppDataPtr->vlpbILComp->numOutport; i++)
  {
    outPortParamsPtr = pAppDataPtr->vlpbILComp->outPortParams + i;
    outPortParamsPtr->nBufferCountActual = IL_CLIENT_VLPB_OUTPUT_BUFFER_COUNT;
    /* this size could be smaller than this value */
    outPortParamsPtr->nBufferSize = IL_CLIENT_VLPB_BUFFER_SIZE;
  }
  /* each componet will have local pipe to take bufffers from other component
     or its own consumed buffer, so that it can be passed to other connected
     components */
  pipe ((int *) pAppDataPtr->vlpbILComp->localPipe);

  pAppDataPtr->vlpbILComp->stopFlag = 0;

  /* populate the pointer for allocated data structure */
  *pAppData = pAppDataPtr;
}

/* ========================================================================== */
/**
* IL_ClientInit() : This function is to deinitialize the application
*                   data structure.
*
* @param pAppData          : appliaction / client data Handle 
*  @return      
*
*
*/
/* ========================================================================== */

void IL_ClientDeInit (IL_Client *pAppData)
{
  close ((int) pAppData->vlpbILComp->localPipe);

  free (pAppData->vlpbILComp->inPortParams);

  free (pAppData->vlpbILComp->outPortParams);

  /* these semaphores are used for tracking the callbacks received from
     component */
  semp_deinit (pAppData->vlpbILComp->eos);
  free (pAppData->vlpbILComp->eos);

  semp_deinit (pAppData->vlpbILComp->done_sem);
  free (pAppData->vlpbILComp->done_sem);

  semp_deinit (pAppData->vlpbILComp->port_sem);
  free (pAppData->vlpbILComp->port_sem);

  free (pAppData->vlpbILComp);

  free (pAppData);
}


/* ========================================================================== */
/**
* IL_ClientErrorToStr() : Function to map the OMX error enum to string
*
* @param error   : OMX Error type
*
*  @return      
*  String conversion of the OMX_ERRORTYPE
*
*/
/* ========================================================================== */

OMX_STRING IL_ClientErrorToStr (OMX_ERRORTYPE error)
{
  OMX_STRING errorString;

  /* used for printing purpose */
  switch (error)
  {
    case OMX_ErrorNone:
      errorString = "OMX_ErrorNone";
      break;
    case OMX_ErrorInsufficientResources:
      errorString = "OMX_ErrorInsufficientResources";
      break;
    case OMX_ErrorUndefined:
      errorString = "OMX_ErrorUndefined";
      break;
    case OMX_ErrorInvalidComponentName:
      errorString = "OMX_ErrorInvalidComponentName";
      break;
    case OMX_ErrorComponentNotFound:
      errorString = "OMX_ErrorComponentNotFound";
      break;
    case OMX_ErrorInvalidComponent:
      errorString = "OMX_ErrorInvalidComponent";
      break;
    case OMX_ErrorBadParameter:
      errorString = "OMX_ErrorBadParameter";
      break;
    case OMX_ErrorNotImplemented:
      errorString = "OMX_ErrorNotImplemented";
      break;
    case OMX_ErrorUnderflow:
      errorString = "OMX_ErrorUnderflow";
      break;
    case OMX_ErrorOverflow:
      errorString = "OMX_ErrorOverflow";
      break;
    case OMX_ErrorHardware:
      errorString = "OMX_ErrorHardware";
      break;
    case OMX_ErrorInvalidState:
      errorString = "OMX_ErrorInvalidState";
      break;
    case OMX_ErrorStreamCorrupt:
      errorString = "OMX_ErrorStreamCorrupt";
      break;
    case OMX_ErrorPortsNotCompatible:
      errorString = "OMX_ErrorPortsNotCompatible";
      break;
    case OMX_ErrorResourcesLost:
      errorString = "OMX_ErrorResourcesLost";
      break;
    case OMX_ErrorNoMore:
      errorString = "OMX_ErrorNoMore";
      break;
    case OMX_ErrorVersionMismatch:
      errorString = "OMX_ErrorVersionMismatch";
      break;
    case OMX_ErrorNotReady:
      errorString = "OMX_ErrorNotReady";
      break;
    case OMX_ErrorTimeout:
      errorString = "OMX_ErrorTimeout";
      break;
    default:
      errorString = "<unknown>";
      break;
  }

  return errorString;
}

/* ========================================================================== */
/**
* IL_ClientUtilGetSelfBufHeader() : This util function is to get buffer header
*                                   specific to one component, from the buffer
*                                   received from other component  .
*
* @param thisComp   : application component data structure
* @param pBuffer    : OMX buffer pointer
* @param type       : it is to identfy teh port type
* @param portIndex  : port number of the component
* @param pBufferOut : components buffer header correponding to pBuffer
*
*  @return      
*  String conversion of the OMX_ERRORTYPE
*
*/
/* ========================================================================== */

OMX_ERRORTYPE IL_ClientUtilGetSelfBufHeader (IL_CLIENT_COMP_PRIVATE *thisComp,
                                             OMX_U8 *pBuffer,
                                             ILCLIENT_PORT_TYPE type,
                                             OMX_U32 portIndex,
                                             OMX_BUFFERHEADERTYPE **pBufferOut)
{
  int i;
  IL_CLIENT_INPORT_PARAMS *inPortParamsPtr;
  IL_CLIENT_OUTPORT_PARAMS *outPortParamsPtr;
  OMX_ERRORTYPE eError = OMX_ErrorNone;

  /* Check for input port buffer header queue */
  if (type == ILCLIENT_INPUT_PORT)
  {
    inPortParamsPtr = thisComp->inPortParams + portIndex;
    for (i = 0; i < inPortParamsPtr->nBufferCountActual; i++)
    {
      if (pBuffer == inPortParamsPtr->pInBuff[i]->pBuffer)
      {
        *pBufferOut = inPortParamsPtr->pInBuff[i];
      }
    }
  }
  /* Check for output port buffer header queue */
  else
  {
    outPortParamsPtr =
      thisComp->outPortParams + portIndex - thisComp->startOutportIndex;
    for (i = 0; i < outPortParamsPtr->nBufferCountActual; i++)
    {
      if (pBuffer == outPortParamsPtr->pOutBuff[i]->pBuffer)
      {
        *pBufferOut = outPortParamsPtr->pOutBuff[i];
      }
    }
  }

  return (eError);
}

/* ========================================================================== */
/**
* IL_ClientUseInitialOutputResources() :  This function gives initially all
*                                         output buffers to a component.
*                                         after consuming component would keep
*                                         in local pipe for connect thread use. 
*
* @param pAppdata   : application data structure
*
*  @return      
*  String conversion of the OMX_ERRORTYPE
*
*/
/* ========================================================================== */

OMX_ERRORTYPE IL_ClientUseInitialInputOutputResources (IL_CLIENT_COMP_PRIVATE
                                                         *thisComp)
{

  OMX_ERRORTYPE err = OMX_ErrorNone;
  unsigned int i = 0;
  //unsigned int dataRead;

  for (i = 0; i < thisComp->outPortParams->nBufferCountActual; i++)
  {
    /* Pass the output buffer to the component */
    err = OMX_FillThisBuffer (thisComp->handle,
                              thisComp->outPortParams->pOutBuff[i]);
  }

  for (i = 0; i < thisComp->inPortParams->nBufferCountActual; i++)
  {
    /* Pass the input buffer to the component */
    memset (thisComp->inPortParams->pInBuff[i]->pBuffer,
            IL_CLIENT_VLPB_PATTERN, IL_CLIENT_VLPB_BUFFER_SIZE);
    thisComp->inPortParams->pInBuff[i]->nFilledLen = IL_CLIENT_VLPB_BUFFER_SIZE;
    err = OMX_EmptyThisBuffer (thisComp->handle,
                               thisComp->inPortParams->pInBuff[i]);
  }

  return err;
}

/* ========================================================================== */
/**
* IL_ClientSetVlpbParams() : Function to fill the port definition 
* structures and call the Set_Parameter function on to the VLPB
* Component
*
* @param pAppData   : Pointer to the application data
*
*  @return      
*  OMX_ErrorNone = Successful 
*
*  Other_value = Failed (Error code is returned)
*
*/
/* ========================================================================== */


OMX_ERRORTYPE IL_ClientSetVlpbParams (IL_Client *pAppData)
{
  OMX_ERRORTYPE eError = OMX_ErrorUndefined;
  OMX_HANDLETYPE pHandle = NULL;
  OMX_PARAM_PORTDEFINITIONTYPE tPortDef;

  pHandle = pAppData->pVlpbHandle;

  OMX_INIT_PARAM (&tPortDef);
  /* Get the Number of Ports */

  tPortDef.nPortIndex = OMX_VLPB_INPUT_PORT_START_INDEX;
  eError = OMX_GetParameter (pHandle, OMX_IndexParamPortDefinition, &tPortDef);
  /* set the actual number of buffers required */
  tPortDef.nBufferCountActual = IL_CLIENT_VLPB_INPUT_BUFFER_COUNT;
  /* set the video format settings */
  tPortDef.format.video.nFrameWidth = IL_CLIENT_VLPB_BUFFER_WIDTH;
  tPortDef.format.video.nStride = IL_CLIENT_VLPB_BUFFER_WIDTH;
  tPortDef.format.video.nFrameHeight = IL_CLIENT_VLPB_BUFFER_HEIGHT;
  /* settings for OMX_IndexParamVideoPortFormat */
  tPortDef.nBufferSize = IL_CLIENT_VLPB_BUFFER_SIZE;
  eError = OMX_SetParameter (pHandle, OMX_IndexParamPortDefinition, &tPortDef);
  if (eError != OMX_ErrorNone)
  {
    ERROR ("failed to set Encode OMX_IndexParamPortDefinition for input \n");
  }


  OMX_INIT_PARAM (&tPortDef);

  tPortDef.nPortIndex = OMX_VLPB_OUTPUT_PORT_START_INDEX;
  eError = OMX_GetParameter (pHandle, OMX_IndexParamPortDefinition, &tPortDef);
  /* settings for OMX_IndexParamPortDefinition */
  /* set the actual number of buffers required */
  tPortDef.nBufferCountActual = IL_CLIENT_VLPB_OUTPUT_BUFFER_COUNT;
  tPortDef.format.video.nFrameWidth = IL_CLIENT_VLPB_BUFFER_WIDTH;
  tPortDef.format.video.nStride = IL_CLIENT_VLPB_BUFFER_WIDTH;
  tPortDef.format.video.nFrameHeight = IL_CLIENT_VLPB_BUFFER_HEIGHT;
  tPortDef.nBufferSize = IL_CLIENT_VLPB_BUFFER_SIZE;
  /* settings for OMX_IndexParamVideoPortFormat */


  eError = OMX_SetParameter (pHandle, OMX_IndexParamPortDefinition, &tPortDef);
  if (eError != OMX_ErrorNone)
  {
    ERROR ("failed to set Encode OMX_IndexParamPortDefinition for output \n");
  }

  return eError;

}

/* Nothing beyond this point */
