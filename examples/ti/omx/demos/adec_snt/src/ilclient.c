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
 *  @file  ilclient.c
 *  @brief This file contains all Functions related to Test Application
 *
 *         This is the example IL Client support to create, configure & execute
 *         adec omx-component using standard non-tunneling mode
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
#include <xdc/std.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Timestamp.h>
#ifdef CODEC_MP3DEC
#include <ti/sdo/codecs/mp3dec/imp3dec.h>
#endif
#include <ti/sdo/codecs/aaclcdec/iaacdec.h>
/*-------------------------program files -------------------------------------*/
#include "ti/omx/interfaces/openMaxv11/OMX_Audio.h"
#include "ti/omx/interfaces/openMaxv11/OMX_Core.h"
#include "ti/omx/interfaces/openMaxv11/OMX_Component.h"
#include "OMX_TI_Common.h"
#include "timm_osal_trace.h"
#include "timm_osal_interfaces.h"
#include "omx_adec.h"
#include <alsa/asoundlib.h>
/*******************************************************************************
 * EXTERNAL REFERENCES NOTE : only use if not found in header file
*******************************************************************************/

/****************************************************************
 * DEFINES
 ****************************************************************/

/** Event definition to indicate input buffer consumed */
#define ADEC_DECODER_INPUT_READY 1

/** Event definition to indicate output buffer consumed */
#define ADEC_DECODER_OUTPUT_READY   2

/** Event definition to indicate error in processing */
#define ADEC_DECODER_ERROR_EVENT 4

/** Event definition to indicate End of stream */
#define ADEC_DECODER_END_OF_STREAM 8

#define ADEC_STATETRANSITION_COMPLETE 16

#define ADEC_PORTCONFIGURATION_COMPLETE 32

/****************************************************************
 * GLOBALS
 ****************************************************************/

static TIMM_OSAL_PTR pSem_Events = NULL;
static TIMM_OSAL_PTR myEvent;
static TIMM_OSAL_PTR ADEC_CmdEvent;

/** Number of input buffers in the ADEC Decoder IL Client */
#define NUM_OF_IN_BUFFERS 1

/** Number of output buffers in the ADEC Decoder IL Client */
#define NUM_OF_OUT_BUFFERS 1

#define INPUT_BUF_SIZE (4096)
#define OUTPUT_BUF_SIZE (8192)

/** Macro to initialize memset and initialize the OMX structure */
#define OMX_ADEC_TEST_INIT_STRUCT_PTR(_s_, _name_)       \
 memset((_s_), 0x0, sizeof(_name_)); \
    (_s_)->nSize = sizeof(_name_);              \
    (_s_)->nVersion.s.nVersionMajor = 0x1;      \
    (_s_)->nVersion.s.nVersionMinor = 0x1;      \
    (_s_)->nVersion.s.nRevision  = 0x0;       \
    (_s_)->nVersion.s.nStep   = 0x0;

/* ========================================================================== */
/** ADEC_Client is the structure definition for the ADEC Decoder IL Client
*
* @param pHandle               OMX Handle
* @param pComponent            Component Data structure
* @param pCb                   Callback function pointer
* @param eState                Current OMX state
* @param pInPortDef            Structure holding input port definition
* @param pOutPortDef           Structure holding output port definition
* @param eCompressionFormat    Format of the input data
* @param pInBuff               Input Buffer pointer
* @param pOutBuff              Output Buffer pointer
* @param IpBuf_Pipe            Input Buffer Pipe
* @param OpBuf_Pipe            Output Buffer Pipe
* @param fIn                   File pointer of input file
* @param fOut                  Output file pointer
* @param nDecodedFrm           Total number of decoded frames
* @param stopFlag              Flag to indicate Stop further processing
*/
/* ========================================================================== */
typedef struct ADEC_Client
{
  OMX_HANDLETYPE pHandle;
  OMX_COMPONENTTYPE *pComponent;
  OMX_CALLBACKTYPE *pCb;
  OMX_STATETYPE eState;
  OMX_PARAM_PORTDEFINITIONTYPE *pInPortDef;
  OMX_PARAM_PORTDEFINITIONTYPE *pOutPortDef;
  OMX_U8 eCompressionFormat;
  OMX_BUFFERHEADERTYPE *pInBuff[NUM_OF_IN_BUFFERS];
  OMX_BUFFERHEADERTYPE *pOutBuff[NUM_OF_OUT_BUFFERS];
  OMX_PTR IpBuf_Pipe;
  OMX_PTR OpBuf_Pipe;

  FILE *fIn;
  FILE *fOut;
  OMX_U32 nDecodedFrms;
  OMX_U32 stopFlag;
} ADEC_Client;

/*---------------------function prototypes -----------------------------------*/
OMX_ERRORTYPE ADEC_SetParamPortDefinition(OMX_HANDLETYPE handle, Int32 decType,
										  Int aacRawFormat, Int aacRawSampleRate);

/* ========================================================================== */
/**
* ADEC_AllocateResources() : Allocates the resources required for Audio
* Decoder.
*
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
static OMX_ERRORTYPE ADEC_AllocateResources(ADEC_Client * pAppData)
{
  OMX_U32 retval;
  OMX_ERRORTYPE eError = OMX_ErrorNone;

  /* Creating IL client specific dtaa structure to maintained at appliaction/
     IL client */
  /* callback structure , standard OpenMax structure */
  pAppData->pCb =
    (OMX_CALLBACKTYPE *) TIMM_OSAL_Malloc (sizeof (OMX_CALLBACKTYPE),
                                           TIMM_OSAL_TRUE, 0,
                                           TIMMOSAL_MEM_SEGMENT_EXT);
  if (!pAppData->pCb)
  {
    eError = OMX_ErrorInsufficientResources;
    goto EXIT;
  }

  pAppData->pInPortDef =
    (OMX_PARAM_PORTDEFINITIONTYPE *)
    TIMM_OSAL_Malloc (sizeof (OMX_PARAM_PORTDEFINITIONTYPE), TIMM_OSAL_TRUE,
                      0, TIMMOSAL_MEM_SEGMENT_EXT);
  if (!pAppData->pInPortDef)
  {
    eError = OMX_ErrorInsufficientResources;
    goto EXIT;
  }

  pAppData->pOutPortDef =
    (OMX_PARAM_PORTDEFINITIONTYPE *)
    TIMM_OSAL_Malloc (sizeof (OMX_PARAM_PORTDEFINITIONTYPE), TIMM_OSAL_TRUE,
                      0, TIMMOSAL_MEM_SEGMENT_EXT);
  if (!pAppData->pOutPortDef)
  {
    eError = OMX_ErrorInsufficientResources;
    goto EXIT;
  }

  /* Create a pipes for Input and Output Buffers.. used to queue data from the
     callback. */
  retval =
    TIMM_OSAL_CreatePipe (&(pAppData->IpBuf_Pipe),
                          sizeof (OMX_BUFFERHEADERTYPE *) * NUM_OF_IN_BUFFERS,
                          sizeof (OMX_BUFFERHEADERTYPE *), OMX_TRUE);
  if (retval != 0)
  {
    printf ("Error: TIMM_OSAL_CreatePipe failed to open");
    eError = OMX_ErrorContentPipeCreationFailed;
    goto EXIT;
  }

  retval =
    TIMM_OSAL_CreatePipe (&(pAppData->OpBuf_Pipe),
                             sizeof(OMX_BUFFERHEADERTYPE *) *
                             NUM_OF_OUT_BUFFERS, sizeof(OMX_BUFFERHEADERTYPE *),
                             OMX_TRUE);
  if (retval != 0)
  {
    printf ("Error: TIMM_OSAL_CreatePipe failed to open");
    eError = OMX_ErrorContentPipeCreationFailed;
    goto EXIT;
  }

EXIT:

  return eError;
}

/* ========================================================================== */
/**
* ADEC_FreeResources() : Free the resources allocated for Audio
* Decoder.
*
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
static void ADEC_FreeResources(ADEC_Client * pAppData)
{
  /* freeing up IL client alloacted data structures */
  if (pAppData->pCb) {
    TIMM_OSAL_Free (pAppData->pCb);
  }

  if (pAppData->pInPortDef) {
    TIMM_OSAL_Free (pAppData->pInPortDef);
  }

  if (pAppData->pOutPortDef) {
    TIMM_OSAL_Free (pAppData->pOutPortDef);
  }

  if (pAppData->IpBuf_Pipe) {
    TIMM_OSAL_DeletePipe (pAppData->IpBuf_Pipe);
  }

  if (pAppData->OpBuf_Pipe) {
    TIMM_OSAL_DeletePipe (pAppData->OpBuf_Pipe);
  }

  return;
}

/* ========================================================================== */
/**
* ADEC_GetDecoderErrorString() : Function to map the OMX error enum to string
*
* @param error   : OMX Error type
*
*  @return
*  String conversion of the OMX_ERRORTYPE
*
*/
/* ========================================================================== */
static OMX_STRING ADEC_GetDecoderErrorString(OMX_ERRORTYPE error)
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
  }

  return errorString;
}

/* ========================================================================== */
/**
* ADEC_FillData() : Function to fill the input buffer with data.
* This function currently reads the entire file into one single memory chunk.
* May require modification to support bigger file sizes.
*
*
* @param pAppData   : Pointer to the application data
* @param pBuf       : Pointer to the input buffer
*
*  @return
*  OMX_ErrorNone = Successful
*
*  Other_value = Failed (Error code is returned)
*
*/
/* ========================================================================== */
static OMX_U32 ADEC_FillData(ADEC_Client *pAppData,
                             OMX_BUFFERHEADERTYPE *pBuf)
{
  OMX_U32 nRead = 0;

  nRead = fread(pBuf->pBuffer, sizeof(char),INPUT_BUF_SIZE, pAppData->fIn);

  pBuf->nFilledLen = nRead;
  pBuf->nAllocLen = nRead;
  pBuf->nOffset = 0;
  pBuf->nInputPortIndex = OMX_AUDDEC_INPUT_PORT;
  return nRead;
}

/* ========================================================================== */
/**
* ADEC_WaitForState() : This method will wait for the component to get
* to the correct state.
*
* @param pHandle        : Handle to the component
* @param DesiredState   : Desired
*
*  @return
*  OMX_ErrorNone = Successful
*
*  Other_value = Failed (Error code is returned)
*
*/
/* ========================================================================== */
static OMX_ERRORTYPE ADEC_WaitForState(OMX_HANDLETYPE * pHandle,
                                          OMX_STATETYPE DesiredState)
{
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  TIMM_OSAL_U32 uRequestedEvents, pRetrievedEvents;
  TIMM_OSAL_ERRORTYPE retval;

  /* Wait for an event, which would be triggered through callback function */
  uRequestedEvents = (ADEC_STATETRANSITION_COMPLETE );
  retval =
        TIMM_OSAL_EventRetrieve(ADEC_CmdEvent, uRequestedEvents,
                             TIMM_OSAL_EVENT_OR_CONSUME, &pRetrievedEvents,
                             TIMM_OSAL_SUSPEND);

  if (TIMM_OSAL_ERR_NONE != retval)
  {
    TIMM_OSAL_Trace ("\nError in EventRetrieve !\n");
    eError = OMX_ErrorInsufficientResources;
    goto EXIT;
  }

  if (pRetrievedEvents & ADEC_DECODER_ERROR_EVENT) {
    eError = OMX_ErrorUndefined;
  }
  else {
    eError = OMX_ErrorNone;
  }

  EXIT:
  return eError;
}

/* ========================================================================== */
/**
* ADEC_WaitForPortConfig() : This method will wait for the component to get
* to the correct port configuration.
*
* @param pHandle        : Handle to the component
*
*  @return
*  OMX_ErrorNone = Successful
*
*  Other_value = Failed (Error code is returned)
*
*/
/* ========================================================================== */
static OMX_ERRORTYPE ADEC_WaitForPortConfig(OMX_HANDLETYPE * pHandle)
{
  //OMX_STATETYPE CurState = OMX_StateInvalid;
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  //OMX_U32 nCnt = 0;
  //OMX_COMPONENTTYPE *pComponent = (OMX_COMPONENTTYPE *) pHandle;
  TIMM_OSAL_U32 uRequestedEvents, pRetrievedEvents;
  TIMM_OSAL_ERRORTYPE retval;

  /* Wait for an event */
  uRequestedEvents = (ADEC_PORTCONFIGURATION_COMPLETE );
  retval =
      TIMM_OSAL_EventRetrieve(ADEC_CmdEvent, uRequestedEvents,
                              TIMM_OSAL_EVENT_OR_CONSUME, &pRetrievedEvents,
                              TIMM_OSAL_SUSPEND);
  if (TIMM_OSAL_ERR_NONE != retval) {
    TIMM_OSAL_Trace("\nError in EventRetrieve !\n");
    eError = OMX_ErrorInsufficientResources;
    goto EXIT;
  }

  if (pRetrievedEvents & ADEC_DECODER_ERROR_EVENT) {
    eError = OMX_ErrorUndefined;
  }
  else {
    eError = OMX_ErrorNone;
  }

EXIT:
  return eError;
}

/* ========================================================================== */
/**
* ADEC_ChangePortSettings() : This method will perform output Port
* settings change
*
* @param pHandle        : Handle to the component
*
*  @return
*  OMX_ErrorNone = Successful
*
*  Other_value = Failed (Error code is returned)
*
*/
/* ========================================================================== */
static OMX_ERRORTYPE ADEC_ChangePortSettings(ADEC_Client * pAppData)
{

  TIMM_OSAL_ERRORTYPE retval;
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  OMX_U32 i;

  /* in case we need to change the port setting, while executing, port needs to
     be disabled, parameters needs to be changed, and buffers would be
     alloacted as new sizes and port would be enabled */

  eError =
    OMX_SendCommand (pAppData->pHandle, OMX_CommandPortDisable,
                     pAppData->pOutPortDef->nPortIndex, NULL);
  if (eError != OMX_ErrorNone)
  {
    printf ("Error from SendCommand OMX_CommandPortDisable ");
    goto EXIT;
  }

  for (i = 0; i < pAppData->pOutPortDef->nBufferCountActual; i++)
  {
    eError =
      OMX_FreeBuffer (pAppData->pHandle, pAppData->pOutPortDef->nPortIndex,
                      pAppData->pOutBuff[i]);
    if (eError != OMX_ErrorNone)
    {
      printf ("Error in OMX_FreeBuffer");
      goto EXIT;
    }
  }

  eError =
    OMX_GetParameter (pAppData->pHandle, OMX_IndexParamPortDefinition,
                      pAppData->pOutPortDef);
  if (eError != OMX_ErrorNone)
  {
    printf ("Error in OMX_GetParameter");
    goto EXIT;
  }

  eError =
    OMX_SendCommand (pAppData->pHandle, OMX_CommandPortEnable,
                     pAppData->pOutPortDef->nPortIndex, NULL);
  if (eError != OMX_ErrorNone)
  {
    printf ("Error in OMX_SendCommand:OMX_CommandPortEnable");
    goto EXIT;
  }

  retval = TIMM_OSAL_ClearPipe (pAppData->OpBuf_Pipe);
  if (retval != TIMM_OSAL_ERR_NONE)
  {
    printf ("Error in clearing Output Pipe!");
    eError = OMX_ErrorNotReady;
    return eError;
  }

  for (i = 0; i < pAppData->pOutPortDef->nBufferCountActual; i++)
  {
    eError =
      OMX_AllocateBuffer (pAppData->pHandle, &pAppData->pOutBuff[i],
                          pAppData->pOutPortDef->nPortIndex, pAppData,
                          pAppData->pOutPortDef->nBufferSize);
    if (eError != OMX_ErrorNone)
    {
      printf ("Error in Allocating buffers");
      goto EXIT;
    }
  }

EXIT:

  return eError;
}

/* ========================================================================== */
/**
* ADEC_EventHandler() : This method is the event handler implementation to
* handle events from the OMX MP3 Derived component
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
static OMX_ERRORTYPE ADEC_EventHandler(OMX_HANDLETYPE hComponent,
                                          OMX_PTR ptrAppData,
                                          OMX_EVENTTYPE eEvent, OMX_U32 nData1,
                                          OMX_U32 nData2, OMX_PTR pEventData)
{
  ADEC_Client *pAppData = ptrAppData;
  /* OMX_STATETYPE state;*/
  TIMM_OSAL_ERRORTYPE retval;
  OMX_ERRORTYPE eError = OMX_ErrorNone;

  switch (eEvent)
  {
    case OMX_EventCmdComplete:
      /* callback from component indicated that command has been completed */
      if (nData1 == OMX_CommandStateSet)
      {
        TIMM_OSAL_SemaphoreRelease (pSem_Events);
        retval =
          TIMM_OSAL_EventSet (ADEC_CmdEvent,
                              ADEC_STATETRANSITION_COMPLETE,
                              TIMM_OSAL_EVENT_OR);
        if (retval != TIMM_OSAL_ERR_NONE)
        {
          TIMM_OSAL_Trace ("\nError in setting the event!\n");
          eError = OMX_ErrorNotReady;
          return eError;
        }
      }
      if (nData1 == OMX_CommandPortEnable || nData1 == OMX_CommandPortDisable) {
        retval =
            TIMM_OSAL_EventSet(ADEC_CmdEvent,
                               ADEC_PORTCONFIGURATION_COMPLETE,
                               TIMM_OSAL_EVENT_OR);
        if (retval != TIMM_OSAL_ERR_NONE) {
          TIMM_OSAL_Trace("\nError in setting the event!\n");
          printf("\nError in setting event.......\n");
          eError = OMX_ErrorNotReady;
          return eError;
		}
      }
      break;
    case OMX_EventError:
      printf("\nError event received from ADEC.......\n");
      break;
    case OMX_EventMark:
      break;
    case OMX_EventPortSettingsChanged:
      /* In case of change in output buffer sizes re-allocate the buffers */
      eError = ADEC_ChangePortSettings(pAppData);
      break;
    case OMX_EventBufferFlag:
      retval =
        TIMM_OSAL_EventSet (myEvent, ADEC_DECODER_END_OF_STREAM, TIMM_OSAL_EVENT_OR);
      if (retval != TIMM_OSAL_ERR_NONE)
      {
        printf ("Error in setting the event!");
        eError = OMX_ErrorNotReady;
        return eError;
      }
      break;
    case OMX_EventResourcesAcquired:
      break;
    case OMX_EventComponentResumed:
      break;
    case OMX_EventDynamicResourcesAvailable:
      break;
    case OMX_EventPortFormatDetected:
      break;
    case OMX_EventMax:
      break;
    default:
      break;
  }  // end of switch
  return eError;
}

/* ========================================================================== */
/**
* ADEC_FillBufferDone() : This method handles the fill buffer done event
* got from the derived component
*
* @param hComponent        : Handle to the component
* @param ptrAppData        : Pointer to the app data
*
*  @return
*  OMX_ErrorNone = Successful
*
*  Other_value = Failed (Error code is returned)
*
*/
/* ========================================================================== */
static OMX_ERRORTYPE ADEC_FillBufferDone(OMX_HANDLETYPE hComponent,
                                            OMX_PTR ptrAppData,
                                            OMX_BUFFERHEADERTYPE *pBuffer)
{
  ADEC_Client *pAppData = ptrAppData;
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  TIMM_OSAL_ERRORTYPE retval;

  if(pAppData->stopFlag)
  {
    return eError;
  }
  
  /* Output buffers is available now, put in the queue */
  retval =
    TIMM_OSAL_WriteToPipe (pAppData->OpBuf_Pipe, &pBuffer, sizeof (pBuffer),
                           TIMM_OSAL_SUSPEND);
  if (retval != TIMM_OSAL_ERR_NONE)
  {
    printf ("Error writing to Output buffer Pipe!");
    eError = OMX_ErrorNotReady;
    return eError;
  }
  /* IL client checks this even for recycling the buffer */
  retval =
    TIMM_OSAL_EventSet (myEvent, ADEC_DECODER_OUTPUT_READY, TIMM_OSAL_EVENT_OR);
  if (retval != TIMM_OSAL_ERR_NONE)
  {
    printf ("Error in setting the o/p event!");
    eError = OMX_ErrorNotReady;
    return eError;
  }
  return eError;
}

/* ========================================================================== */
/**
* ADEC_FillBufferDone() : This method handles the Empty buffer done event
* got from the derived component
*
* @param hComponent        : Handle to the component
* @param ptrAppData        : Pointer to the app data
*
*  @return
*  OMX_ErrorNone = Successful
*
*  Other_value = Failed (Error code is returned)
*
*/
/* ========================================================================== */
int gEOF=0, gbytesInInputBuffer=INPUT_BUF_SIZE;
static OMX_ERRORTYPE ADEC_EmptyBufferDone(OMX_HANDLETYPE hComponent,
                                             OMX_PTR ptrAppData,
                                             OMX_BUFFERHEADERTYPE *pBuffer)
{
  ADEC_Client *pAppData = ptrAppData;
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  TIMM_OSAL_ERRORTYPE retval;
  if (pAppData->stopFlag) {
    return eError;
  }  

  /* If Bytes Consumed is 0, stop decoding */
  if((gbytesInInputBuffer - pBuffer->nFilledLen) == 0) {
    gEOF = 1;
    gbytesInInputBuffer = 0;
    pBuffer->nFilledLen = 0;
    printf("\nBytes Consumed is Zero by decoder......\nHence Stopping further decoding\n");
  }

  /* input buffer is consumed and recycled in the queue */
  retval =
    TIMM_OSAL_WriteToPipe (pAppData->IpBuf_Pipe, &pBuffer, sizeof (pBuffer),
                           TIMM_OSAL_SUSPEND);
  if (retval != TIMM_OSAL_ERR_NONE)
  {
    printf ("Error writing to Input buffer i/p Pipe!");
    eError = OMX_ErrorNotReady;
    return eError;
  }

  /* IL client in this example is checking for this event to re-use the buffer
   */
  retval =
    TIMM_OSAL_EventSet (myEvent, ADEC_DECODER_INPUT_READY, TIMM_OSAL_EVENT_OR);
  if (retval != TIMM_OSAL_ERR_NONE)
  {
    printf ("Error in setting the event!");
    eError = OMX_ErrorNotReady;
    return eError;
  }
  return eError;
}

/* Handle for the PCM device */
snd_pcm_t *playback_handle = NULL;
int configureaudiodrv(int rate)
{
	int err;
	int exact_rate;
	/* Playback stream */
	snd_pcm_stream_t stream = SND_PCM_STREAM_PLAYBACK;
	/* This structure contains information about the hardware and can be
	used to specify the configuration to be used for */
	/* the PCM stream. */
	snd_pcm_hw_params_t *hw_params;

	/* Name of the PCM device, like plughw:0,0 */
	/* The first number is the number of the soundcard, the second number
	is the number of the device. */
	static char *device = "default"; /* playback device */

	/* Open PCM. The last parameter of this function is the mode. */
	if ((err = snd_pcm_open (&playback_handle, device, stream, 0))< 0) {
		printf("Could not open audio device\n");
		return(1);
	}


	/* Allocate the snd_pcm_hw_params_t structure on the stack. */
	if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
		fprintf (stderr, "cannot allocate hardware parameters (%s)\n",
		snd_strerror (err));
		return(1);
	}

	/* Init hwparams with full configuration space */
	if ((err = snd_pcm_hw_params_any (playback_handle, hw_params)) <0) {
		fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n", snd_strerror (err));
		return(1);
	}

	/* Set access type. */
	if ((err = snd_pcm_hw_params_set_access (playback_handle, hw_params,SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		fprintf (stderr, "cannot set access type (%s)\n", snd_strerror
		(err));
			return(1);
	}
	/* Set sample format */
	if ((err = snd_pcm_hw_params_set_format (playback_handle, hw_params,SND_PCM_FORMAT_S16_LE)) < 0) {
		fprintf (stderr, "cannot set sample format (%s)\n", snd_strerror
		(err));
			return(1);
	}

	/* Set sample rate. If the exact rate is not supported by the
	hardware, use nearest possible rate. */
	exact_rate = rate;
	if ((err =
       snd_pcm_hw_params_set_rate_near (playback_handle,
                                        hw_params, (unsigned int *) &rate, 0))
     < 0)
 {
   fprintf (stderr, "cannot set sample rate (%s)\n", snd_strerror
		(err));
		return(1);
	}
	if (rate != exact_rate) {
		fprintf(stderr, "The rate %d Hz is not supported by the hardware.\n \
			==> Using %d Hz instead.\n", rate, exact_rate);
	}

	/* Set number of channels */
	if ((err = snd_pcm_hw_params_set_channels (playback_handle,hw_params, 2)) < 0) {
		fprintf (stderr, "cannot set channel count (%s)\n", snd_strerror
		(err));
			return(1);
	}
	/* Apply HW parameter settings to PCM device and prepare device. */
	if ((err = snd_pcm_hw_params (playback_handle, hw_params)) < 0) {
		fprintf (stderr, "cannot set parameters (%s)\n", snd_strerror
		(err));
			return(1);
	}

	snd_pcm_hw_params_free (hw_params);

	if ((err = snd_pcm_prepare (playback_handle)) < 0) {
		fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
		snd_strerror (err));
		return(1);
	}

    return 0;
}

/* ========================================================================== */
/**
* OMX_Audio_Decode_Test() : This method handles the ADEC component IL-client
* create, decode & delete calls
*
* @param hComponent        : Handle to the component
* @param ptrAppData        : Pointer to the app data
*
*  @return
*  OMX_ErrorNone = Successful
*
*  Other_value = Failed (Error code is returned)
*
*/
/* ========================================================================== */
Int OMX_Audio_Decode_Test(char *inFileName, char *outFileName, char *format,
                          int aacRawFormat, int aacRawSampleRate)
{
  ADEC_Client *pAppData = TIMM_OSAL_NULL;
  OMX_HANDLETYPE pHandle;
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  OMX_BUFFERHEADERTYPE *pBufferIn = NULL;
  OMX_BUFFERHEADERTYPE *pBufferOut = NULL;
  TIMM_OSAL_ERRORTYPE tTIMMSemStatus;
  OMX_U32 i;
  OMX_U32 actualSize;
  OMX_U32 nRead;
  OMX_CALLBACKTYPE appCallbacks;

  TIMM_OSAL_U32 uRequestedEvents, pRetrievedEvents;
  OMX_U32 bytesRead=0;
  int decType;
  OMX_AUDIO_PARAM_AACPROFILETYPE aacParams;
  OMX_ADEC_TEST_INIT_STRUCT_PTR(&aacParams, OMX_AUDIO_PARAM_AACPROFILETYPE);

  printf ("Iteration %d - Start\n", 0);

  /* Callbacks are passed during getHandle call to component, Componnet uses
     these callaback to communicate with IL Client */

  appCallbacks.EventHandler = ADEC_EventHandler;
  appCallbacks.EmptyBufferDone = ADEC_EmptyBufferDone;
  appCallbacks.FillBufferDone = ADEC_FillBufferDone;

  /* Create evenets, which will be triggered during callback from componnet */

  tTIMMSemStatus = TIMM_OSAL_EventCreate (&myEvent);
  if (TIMM_OSAL_ERR_NONE != tTIMMSemStatus)
  {
    printf ("Error in creating event!");
    eError = OMX_ErrorInsufficientResources;
    goto EXIT;
  }

  tTIMMSemStatus = TIMM_OSAL_EventCreate (&ADEC_CmdEvent);
  if (TIMM_OSAL_ERR_NONE != tTIMMSemStatus)
  {
    TIMM_OSAL_Trace ("Error in creating event!\n");
    eError = OMX_ErrorInsufficientResources;
    goto EXIT;
  }
  /* Allocating data structure for IL client structure / buffer management */

  pAppData =
    (ADEC_Client *) TIMM_OSAL_Malloc (sizeof (ADEC_Client),
                                        TIMM_OSAL_TRUE, 0,
                                        TIMMOSAL_MEM_SEGMENT_EXT);
  if (!pAppData)
  {
    printf ("Error allocating pAppData!");
    eError = OMX_ErrorInsufficientResources;
    goto EXIT;
  }
  memset (pAppData, 0x0, sizeof (ADEC_Client));

  pAppData->stopFlag = 0;
  printf (" opening file \n");
  /* Open the file of data to be rendered.  */
  pAppData->fIn = fopen (inFileName, "rb");
  printf (" opened file \n");

  if (pAppData->fIn == NULL)
  {
    printf ("Error: failed to open the file %s for reading\n", inFileName);
    goto EXIT;
  }

  /* populating the parameter in IL client structure, from this it will be
     passed to component via setparam call */

  /* compression format as aaclc, OMX enumeration */
  if (strcmp (format, "aaclc") == 0)
  {
    pAppData->eCompressionFormat = OMX_AUDIO_CodingAAC;
    decType = 1;
  }
  else if (strcmp (format, "mp3") == 0)
  {
    pAppData->eCompressionFormat = OMX_AUDIO_CodingMP3;
    decType = 0;
  }
  else
  {
    printf
      (" Invalid bitstream format specified, should be either aaclc or mp3 \n");
    return -1;
  }

  /* Allocating data structure for buffer queues in IL client */
  eError = ADEC_AllocateResources(pAppData);
  if (eError != OMX_ErrorNone)
  {
    printf ("Error allocating resources in main!");
    eError = OMX_ErrorInsufficientResources;
    goto EXIT;
  }

  pAppData->eState = OMX_StateInvalid;
  *pAppData->pCb = appCallbacks;

  tTIMMSemStatus = TIMM_OSAL_SemaphoreCreate (&pSem_Events, 0);
  if (tTIMMSemStatus != TIMM_OSAL_ERR_NONE)
  {
    printf ("Semaphore Create failed!");
    goto EXIT;
  }


  printf (" calling getHandle \n");

  /* Create the Decoder Component, component handle would be returned component
     name is unique and fixed for a componnet, callback are passed to
     componnet in this function. Componnet would be loaded state post this call
   */

  eError = OMX_GetHandle(&pHandle, (OMX_STRING) "OMX.TI.DSP.AUDDEC", pAppData,
                   pAppData->pCb);
  printf (" got handle \n");

  if ((eError != OMX_ErrorNone) || (pHandle == NULL))
  {
    printf ("Error in Get Handle function : %s \n",
            ADEC_GetDecoderErrorString (eError));
    goto EXIT;
  }

  pAppData->pHandle = pHandle;
  pAppData->pComponent = (OMX_COMPONENTTYPE *) pHandle;

  /* for input port parameter settings */

  /* number of bufferes are port properties, component tracks number of buffers
     allocated during loaded to idle transition */
  pAppData->pInPortDef->nBufferCountActual = 1;
  pAppData->pInPortDef->nPortIndex = OMX_AUDDEC_INPUT_PORT;
  pAppData->pInPortDef->nBufferSize = INPUT_BUF_SIZE;
  pAppData->pInPortDef->format.audio.eEncoding = pAppData->eCompressionFormat;

  /* for output port parameters setting */
  pAppData->pOutPortDef->nBufferCountActual = 1;
  pAppData->pOutPortDef->nPortIndex = OMX_AUDDEC_OUTPUT_PORT;
  pAppData->pOutPortDef->nBufferSize = OUTPUT_BUF_SIZE;

  ADEC_SetParamPortDefinition(pAppData->pHandle, decType,
            aacRawFormat, aacRawSampleRate);

  printf("\nADEC_SetParamPortDefinition done\n");
  fflush(stdout);

  OMX_SendCommand  ( pAppData->pHandle, OMX_CommandPortEnable,
                     OMX_AUDDEC_INPUT_PORT, NULL );
  /* Wait for initialization to complete.. Wait for port enable of component  */
  eError = ADEC_WaitForPortConfig(pHandle);
  if (eError != OMX_ErrorNone) {
    goto EXIT;
  }

  OMX_SendCommand  ( pAppData->pHandle, OMX_CommandPortEnable,
                     OMX_AUDDEC_OUTPUT_PORT, NULL );
  /* Wait for initialization to complete.. Wait for port enable of component  */
  eError = ADEC_WaitForPortConfig(pHandle);
  if (eError != OMX_ErrorNone) {
    goto EXIT;
  }

  /* OMX_SendCommand expecting OMX_StateIdle, after this command component
     would create codec, and will wait for all buffers to be allocated */
  eError = OMX_SendCommand (pHandle, OMX_CommandStateSet, OMX_StateIdle, NULL);
  if (eError != OMX_ErrorNone)
  {
    printf ("Error in SendCommand()-OMX_StateIdle State set : %s \n",
            ADEC_GetDecoderErrorString (eError));
    goto EXIT;
  }
  TIMM_OSAL_Trace ("\nCame back from send command without error\n");

  /* Allocate I/O Buffers; componnet would allocated buffers and would return
     the buffer header containing the pointer to buffer */
  for (i = 0; i < pAppData->pInPortDef->nBufferCountActual; i++)
  {
    eError = OMX_AllocateBuffer (pHandle,       /* &pBufferIn */
                                 &pAppData->pInBuff[i],
                                 pAppData->pInPortDef->nPortIndex, pAppData,
                                 pAppData->pInPortDef->nBufferSize);
    if (eError != OMX_ErrorNone)
    {
      printf ("Error in OMX_AllocateBuffer()- Input Port State set : %s \n",
              ADEC_GetDecoderErrorString (eError));
      goto EXIT;
    }
  }
  /* buffer alloaction for output port */
  for (i = 0; i < pAppData->pOutPortDef->nBufferCountActual; i++)
  {
    eError = OMX_AllocateBuffer (pHandle,       /* &pBufferOut */
                                 &pAppData->pOutBuff[i],
                                 pAppData->pOutPortDef->nPortIndex, pAppData,
                                 pAppData->pOutPortDef->nBufferSize);
    if (eError != OMX_ErrorNone)
    {
      printf ("Error in OMX_AllocateBuffer()-Output Port State set : %s \n",
              ADEC_GetDecoderErrorString (eError));
      goto EXIT;
    }
  }

  /* Wait for initialization to complete.. Wait for Idle stete of component
     after all buffers are alloacted componet would chnage to idle */

  eError = ADEC_WaitForState(pHandle, OMX_StateIdle);
  if (eError != OMX_ErrorNone)
  {
    printf ("Error %s:    WaitForState has timed out \n",
            ADEC_GetDecoderErrorString (eError));
    goto EXIT;
  }
  printf (" state IDLE \n ");

  /* change state to execute so that buffers processing can start */
  eError =
    OMX_SendCommand (pHandle, OMX_CommandStateSet, OMX_StateExecuting, NULL);
  if (eError != OMX_ErrorNone)
  {
    printf ("Error from SendCommand-Executing State set :%s \n",
            ADEC_GetDecoderErrorString (eError));
    goto EXIT;
  }

  eError = ADEC_WaitForState(pHandle, OMX_StateExecuting);
  if (eError != OMX_ErrorNone)
  {
    printf ("Error %s:    WaitForState has timed out \n",
            ADEC_GetDecoderErrorString (eError));
    goto EXIT;
  }
  printf (" state execute \n ");

  /* parser would fill the chunked frames in input buffers, which needs to be
     passed to component in empty buffer call; initially all buffers are
     avaialble so they be filled and given to component */

  for (i = 0; i < pAppData->pInPortDef->nBufferCountActual; i++)
  {
    nRead = ADEC_FillData (pAppData, pAppData->pInBuff[i]);

    eError =
      pAppData->pComponent->EmptyThisBuffer (pHandle, pAppData->pInBuff[i]);

    if (eError != OMX_ErrorNone)
    {
      printf ("Error from Empty this buffer : %s \n",
              ADEC_GetDecoderErrorString (eError));
      goto EXIT;
    }
  }

  /* Initially all bufers are available in IL client, so we can pass all free
     buffers to component */
  for (i = 0; i < pAppData->pOutPortDef->nBufferCountActual; i++)
  {
    ((OMX_BUFFERHEADERTYPE *) (pAppData->pOutBuff[i]))->nOffset = 0;
    eError =
      pAppData->pComponent->FillThisBuffer (pHandle, pAppData->pOutBuff[i]);
    if (eError != OMX_ErrorNone)
    {
      printf ("Error from Fill this buffer : %s \n",
              ADEC_GetDecoderErrorString (eError));
      goto EXIT;
    }
  }

  printf (" etb / ftb done \n ");

  eError = OMX_GetState (pHandle, &pAppData->eState);

  /* Initialize the number of encoded frames to zero */
  pAppData->nDecodedFrms = 0;

  /* all available buffers have been passed to component, now wait for
     processed buffers to come back via eventhandler callback */

  while ((eError == OMX_ErrorNone) && (pAppData->eState != OMX_StateIdle))
  {

    uRequestedEvents =
                (ADEC_DECODER_INPUT_READY | ADEC_DECODER_OUTPUT_READY |
                 ADEC_DECODER_ERROR_EVENT | ADEC_DECODER_END_OF_STREAM);
    tTIMMSemStatus =
      TIMM_OSAL_EventRetrieve (myEvent, uRequestedEvents,
                               TIMM_OSAL_EVENT_OR_CONSUME,
                               &pRetrievedEvents, TIMM_OSAL_SUSPEND);
    if (TIMM_OSAL_ERR_NONE != tTIMMSemStatus)
    {
      printf ("Error in creating event!");
      eError = OMX_ErrorUndefined;
      goto EXIT;
    }

    if (pRetrievedEvents & ADEC_DECODER_END_OF_STREAM)
    {
      printf ("End of stream processed\n");
      break;
    }

    if ((pRetrievedEvents & ADEC_DECODER_OUTPUT_READY) && (!pAppData->stopFlag)) {
      int err;
      /* read from the pipe */
      TIMM_OSAL_ReadFromPipe (pAppData->OpBuf_Pipe, &pBufferOut,
                          sizeof (pBufferOut), &actualSize,
                          TIMM_OSAL_SUSPEND);
      eError = OMX_GetParameter(pHandle, OMX_IndexParamAudioAac, &aacParams);
      if(pBufferOut != NULL) {
        if(strcmp(outFileName,"")!=0) {
          if(pAppData->nDecodedFrms == 0) {
              /* opening the output data file, decoded data is written back to this file */
              printf("\nWriting to File...\n");
              printf("\n\nSample Rate: %d", (int) aacParams.nSampleRate);
              pAppData->fOut = fopen (outFileName, "wb");
              if (pAppData->fOut == NULL)
              {
                printf ("Error: failed to open the file  for writing \n");
                goto EXIT;
              }
            }
          fwrite(pBufferOut->pBuffer, 1, pBufferOut->nFilledLen,
                 pAppData->fOut);
        }
        else {
          if(pAppData->nDecodedFrms == 0){
            /* Configure ALSA Audio driver */
            printf("\nPlayback through Audio Driver...\n");
            printf("\n\nSample Rate: %d", (int) aacParams.nSampleRate);
            i = configureaudiodrv(aacParams.nSampleRate);
            if(i)
            {
              printf("Audio driver configuration failed \n");
              return -1;
            }
          }
          if ((err = snd_pcm_writei (playback_handle, pBufferOut->pBuffer,
                                     pBufferOut->nFilledLen/4)) !=
                                     pBufferOut->nFilledLen/4) {
            printf("write to audio interface failed\n");
          }
          else {
            //fprintf (stdout, "snd_pcm_writei successful\n");
          }
        }
      }
      if (!pAppData->stopFlag) {
        ((OMX_BUFFERHEADERTYPE *) (pAppData->pOutBuff[0]))->nOffset = 0;
        eError =
             pAppData->pComponent->FillThisBuffer(pHandle,
                                                  pAppData->pOutBuff[0]);
        if (eError != OMX_ErrorNone) {
            goto EXIT;
        }
      }
      pAppData->nDecodedFrms++;
    } //if ((pRetrievedEvents & ADEC_DECODER_OUTPUT_READY) && (!pAppData->stopFlag))

    if ((pRetrievedEvents & ADEC_DECODER_INPUT_READY) && (!pAppData->stopFlag)) {
                /*read from the pipe */
     TIMM_OSAL_ReadFromPipe(pAppData->IpBuf_Pipe, &pBufferIn,
                             sizeof(pBufferIn), &actualSize,
                             TIMM_OSAL_SUSPEND);
	 if(pBufferIn !=NULL) {
        memcpy(&pBufferIn->pBuffer[0],
               &pBufferIn->pBuffer[gbytesInInputBuffer-pBufferIn->nFilledLen],
               pBufferIn->nFilledLen);
		if(gEOF == 0) {
          bytesRead =
              fread(&pBufferIn->pBuffer[pBufferIn->nFilledLen],
                    sizeof(char),
                    (gbytesInInputBuffer - pBufferIn->nFilledLen),
                    pAppData->fIn);
          if (bytesRead !=
              (gbytesInInputBuffer - pBufferIn->nFilledLen)) {
		    gEOF=1;
			gbytesInInputBuffer = pBufferIn->nFilledLen + bytesRead;
            printf("\nEOF reached...\nBytes Remaining - %d\n",
                   gbytesInInputBuffer);
          }
        } else {
		  gbytesInInputBuffer = pBufferIn->nFilledLen;
		  if(gbytesInInputBuffer == 0){
		    printf
			  ("End Of Stream reached.. making last process call \n");
			  ((OMX_BUFFERHEADERTYPE *) (pAppData->pInBuff[0]))->
			nFlags = OMX_BUFFERFLAG_EOS;
			if (gbytesInInputBuffer == 0) {
			  pAppData->stopFlag = TRUE;
			}
			printf("\nTotal Frames decoded-%d\n", (int) pAppData->nDecodedFrms);
			eError =
				TIMM_OSAL_EventSet(myEvent,
								   ADEC_DECODER_END_OF_STREAM,
								   TIMM_OSAL_EVENT_OR);
			if (eError != TIMM_OSAL_ERR_NONE) {
				goto EXIT;
			}
          }
		}
     }
     ((OMX_BUFFERHEADERTYPE *) (pAppData->pInBuff[0]))->nFilledLen =
            gbytesInInputBuffer;
     ((OMX_BUFFERHEADERTYPE *) (pAppData->pInBuff[0]))->nOffset = 0;
     if (!pAppData->stopFlag) {
       eError =
          pAppData->pComponent->EmptyThisBuffer(pHandle,
                                                pAppData->pInBuff[0]);
	   if (eError != OMX_ErrorNone) {
	     goto EXIT;
	   }
	   if (gbytesInInputBuffer == 0) {
	     pAppData->stopFlag = TRUE;
	   }
	 }
    }
    if (pRetrievedEvents & ADEC_DECODER_ERROR_EVENT) {
        eError = OMX_ErrorUndefined;
    }
    eError = OMX_GetState(pHandle, &pAppData->eState);
  }
  printf("\nComponent transitioning from Executing to Idle state\n");

//  EXIT1:
  printf ("Tearing down the decode example\n");
  /* change the state to idle, bufefr communication would stop in this state */
  eError = OMX_SendCommand (pHandle, OMX_CommandStateSet, OMX_StateIdle, NULL);
  if (eError != OMX_ErrorNone)
  {
    printf ("Error from SendCommand-Idle State set : %s \n",
            ADEC_GetDecoderErrorString (eError));
    goto EXIT;
  }

  eError = ADEC_WaitForState(pHandle, OMX_StateIdle);
  if (eError != OMX_ErrorNone)
  {
    printf ("Error %s:    WaitForState has timed out \n",
            ADEC_GetDecoderErrorString (eError));
    goto EXIT;
  }

  /* change the state to loaded, componenet would wait for all buffers to be
     freed up, then only state transition would complete */
  eError =
    OMX_SendCommand (pHandle, OMX_CommandStateSet, OMX_StateLoaded, NULL);
  if (eError != OMX_ErrorNone)
  {
    printf ("Error from SendCommand-Loaded State set : %s \n",
            ADEC_GetDecoderErrorString (eError));
    goto EXIT;
  }
  /* During idle-> loaded state transition buffers need to be freed up */
  for (i = 0; i < pAppData->pInPortDef->nBufferCountActual; i++)
  {
    eError =
      OMX_FreeBuffer (pHandle, pAppData->pInPortDef->nPortIndex,
                      pAppData->pInBuff[i]);
    if (eError != OMX_ErrorNone)
    {
      printf ("Error in OMX_FreeBuffer : %s \n",
              ADEC_GetDecoderErrorString (eError));
      goto EXIT;
    }
  }

  for (i = 0; i < pAppData->pOutPortDef->nBufferCountActual; i++)
  {
    eError =
      OMX_FreeBuffer (pHandle, pAppData->pOutPortDef->nPortIndex,
                      pAppData->pOutBuff[i]);
    if (eError != OMX_ErrorNone)
    {
      printf ("Error in OMX_FreeBuffer : %s \n",
              ADEC_GetDecoderErrorString (eError));
      goto EXIT;
    }
  }
  /* wait for state transition to complete, componnet would generate an event,
     when state transition is complete */

  eError = ADEC_WaitForState(pHandle, OMX_StateLoaded);
  if (eError != OMX_ErrorNone)
  {
    printf ("Error %s:    WaitForState has timed out \n",
            ADEC_GetDecoderErrorString (eError));
    goto EXIT;
  }
  printf (" free handle \n ");

  /* UnLoad the Decoder Component */
  eError = OMX_FreeHandle (pHandle);
  if ((eError != OMX_ErrorNone))
  {
    printf ("Error in Free Handle function : %s \n",
            ADEC_GetDecoderErrorString (eError));
    goto EXIT;
  }

  printf (" free handle done \n ");

  tTIMMSemStatus = TIMM_OSAL_SemaphoreDelete (pSem_Events);
  if (tTIMMSemStatus != TIMM_OSAL_ERR_NONE)
  {
    printf ("Semaphore Delete failed!");
    goto EXIT;
  }

EXIT:
  /* releasing IL client data structure */
  if (pAppData)
  {
	if (pAppData->fIn) {
      fclose (pAppData->fIn);
	}
	if (pAppData->fOut) {
      fclose (pAppData->fOut);
	}
    ADEC_FreeResources (pAppData);

    TIMM_OSAL_Free (pAppData);
  }
  if (playback_handle)
  {
    int err;
	if ((err = snd_pcm_drain (playback_handle))< 0)
	{
	  printf("Could not drain audio device\n");
	  return(1);
    }
    snd_pcm_close (playback_handle);
  }

  tTIMMSemStatus = TIMM_OSAL_EventDelete (myEvent);
  if (TIMM_OSAL_ERR_NONE != tTIMMSemStatus)
  {
    printf ("Error in deleting event!");
  }

  tTIMMSemStatus = TIMM_OSAL_EventDelete (ADEC_CmdEvent);
  if (TIMM_OSAL_ERR_NONE != tTIMMSemStatus)
  {
    TIMM_OSAL_Trace ("Error in deleting event!\n");
  }
  printf ("Decoder Test End\n");
  
  return (0);
} /* OMX_Audio_Decode_Test */

/* ilclient.c - EOF */
