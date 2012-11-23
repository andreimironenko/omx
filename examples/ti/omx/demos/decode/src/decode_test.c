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
 *  @file  decode_test.c
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
#include <xdc/std.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Timestamp.h>
#include <ti/sdo/fc/rman/rman.h>
#include "decode_utils.h"
#include "timm_osal_trace.h"
/*-------------------------program files -------------------------------------*/
#include "ti/omx/interfaces/openMaxv11/OMX_Core.h"
#include "ti/omx/interfaces/openMaxv11/OMX_Component.h"
#include "OMX_TI_Common.h"
#include "timm_osal_interfaces.h"
#include "omx_vdec.h"
#include "OMX_TI_Video.h"
#include "OMX_TI_Index.h"

#define OMX_TEST_INIT_STRUCT_PTR(_s_, _name_)                                  \
          memset((_s_), 0x0, sizeof(_name_));                                  \
          (_s_)->nSize = sizeof(_name_);                                       \
          (_s_)->nVersion.s.nVersionMajor = 0x1;                               \
          (_s_)->nVersion.s.nVersionMinor = 0x1;                               \
          (_s_)->nVersion.s.nRevision  = 0x0;                                  \
          (_s_)->nVersion.s.nStep   = 0x0;

#define GOTO_EXIT_IF(_CONDITION,_ERROR)                                        \
        {                                                                      \
          if((_CONDITION))                                                     \
          {                                                                    \
            printf ("Error :: %s : %s : %d :: \n", __FILE__, __FUNCTION__,     \
                    __LINE__);                                                 \
            printf (" Exiting because: %s \n", #_CONDITION);                   \
            eError = (_ERROR);                                                 \
            goto EXIT;                                                         \
          }                                                                    \
        }

/* Align address "a" at "b" boundary */
#define UTIL_ALIGN(a,b)  ((((uint32_t)(a)) + (b)-1) & (~((uint32_t)((b)-1))))

/*******************************************************************************
 * EXTERNAL REFERENCES NOTE : only use if not found in header file
*******************************************************************************/

/****************************************************************
 * DEFINES
 ****************************************************************/

/** Event definition to indicate input buffer consumed */
#define DECODER_INPUT_READY 1

/** Event definition to indicate output buffer consumed */
#define DECODER_OUTPUT_READY   2

/** Event definition to indicate error in processing */
#define DECODER_ERROR_EVENT 4

/** Event definition to indicate End of stream */
#define DECODER_END_OF_STREAM 8

#define DECODER_STATETRANSITION_COMPLETE 16

#define     PADX_H264    32
#define     PADY_H264    24
#define	    PADX_MPEG4 	16
#define	    PADY_MPEG4 	16
#define	    PADX_VC1 	32
#define	    PADY_VC1 	40
#define     PADX_MPEG2   8
#define     PADY_MPEG2   8


int DEC_CHANNEL = 1;
int ENC_CHANNEL = 1;

/****************************************************************
 * GLOBALS
 ****************************************************************/

static TIMM_OSAL_PTR pSem_Events = NULL;
static TIMM_OSAL_PTR myEvent;
static TIMM_OSAL_PTR DECODER_CmdEvent;

static TIMM_OSAL_U32 num_out_buffers;
static OMX_BOOL bEOS_Sent;

/** Number of input buffers in the Decoder IL Client */
#define NUM_OF_IN_BUFFERS 4

/** Number of output buffers in the Decoder IL Client */
#define NUM_OF_OUT_BUFFERS 8

int PADX;
int PADY;
/* ========================================================================== */
/** Decode_Client is the structure definition for the Decoder IL Client
*
* @param pHandle               OMX Handle  
* @param pComponent            Component Data structure
* @param pCb                   Callback function pointer
* @param eState                Current OMX state
* @param pInPortDef            Structure holding input port definition
* @param pOutPortDef           Structure holding output port definition
* @param eCompressionFormat    Format of the input data
* @param pH264                 Pointer to H264 Video param
* @param pMPEG2                Pointer to MPEG2 Video param
* @param pInBuff               Input Buffer pointer
* @param pOutBuff              Output Buffer pointer
* @param IpBuf_Pipe            Input Buffer Pipe
* @param OpBuf_Pipe            Output Buffer Pipe
* @param fIn                   File pointer of input file
* @param fInFrmSz              File pointer of Frame Size file (unused)
* @param fOut                  Output file pointer
* @param ColorFormat           Input color format
* @param nWidth                Width of the input vector
* @param nHeight               Height of the input vector
* @param nEncodedFrm           Total number of encoded frames
*/
/* ========================================================================== */
typedef struct Decode_Client
{
  OMX_HANDLETYPE pHandle;
  OMX_COMPONENTTYPE *pComponent;
  OMX_CALLBACKTYPE *pCb;
  OMX_STATETYPE eState;
  OMX_PARAM_PORTDEFINITIONTYPE *pInPortDef;
  OMX_PARAM_PORTDEFINITIONTYPE *pOutPortDef;
  OMX_U8 eCompressionFormat;
  OMX_VIDEO_PARAM_AVCTYPE *pH264;
  OMX_VIDEO_PARAM_MPEG2TYPE *pMPEG2;

  OMX_BUFFERHEADERTYPE *pInBuff[NUM_OF_IN_BUFFERS];
  OMX_BUFFERHEADERTYPE *pOutBuff[NUM_OF_OUT_BUFFERS];
  OMX_PTR IpBuf_Pipe;
  OMX_PTR OpBuf_Pipe;

  FILE *fIn;
  FILE *fInFrmSz;
  FILE *fOut;
  OMX_COLOR_FORMATTYPE ColorFormat;
  OMX_U32 nWidth;
  OMX_U32 nHeight;
  OMX_U32 nPitch;
  OMX_U32 nEncodedFrms;
  OMX_U32 stopFlag;
  H264_ParsingCtx pc;
  MPEG2_ParsingCtx pcmpeg2;
  H263_ParsingCtx pch263;
  MPEG4_ParsingCtx pcmpeg4;
  VC1_ParsingCtx pcvc1;
  void *fieldBuf;
} Decode_Client;

/*--------------------- function prototypes ----------------------------------*/
/* None */

/*---------------------function prototypes -----------------------------------*/
Void getDefaultHeapStats ();

OMX_ERRORTYPE Decode_SetParamPortDefinition (OMX_HANDLETYPE pHandle,
                                             Int32 width, Int32 height,
                                             Int32 compressionFormat);
OMX_ERRORTYPE MPEG2DEC_SetParamPortDefinition (OMX_HANDLETYPE pHandle,
                                               Int32 width, Int32 height);

/*******************************************************************************
 * PRIVATE DECLARATIONS Defined here, used only here
 ******************************************************************************/

volatile Bool g_waitOnProcLoad = FALSE;
/* ==========================================================================
*
*@func   Decode_outputDisplayFrame()
*
*@brief  Sub function to write output data to a file
*
*@param  outArgs                  output arguments
*@param  xoff                     horizontal offset
*@param  yoff                     vertical offset
*
*@ret    none
*
* ============================================================================
*/
static unsigned short
  Decode_outputDisplayFrame (unsigned char *lBuffPtr,
                             unsigned char *cBuffPtr,
                             unsigned int xoff,
                             unsigned int yoff,
                             unsigned int ref_width,
                             unsigned int ref_height,
                             unsigned int width,
                             unsigned int height, void *fieldBuf, FILE *fout)
{

  /*--------------------------------------------------------------------------------
   Here the actual frame data (w/out padding), is extracted and dumped.
   Also UV is de-interleaved.   
  --------------------------------------------------------------------------------*/
  unsigned short retval = 0;
  unsigned char *CbBuf, *CrBuf, *YBuf, *lumaAddr, *chromaAddr;
  unsigned int pic_size, i, j;

  pic_size = width * height;

  lumaAddr =
    (unsigned char *) ((unsigned int) lBuffPtr + (yoff * ref_width) + xoff);
  chromaAddr =
    (unsigned char *) ((unsigned int) cBuffPtr +
                       ((yoff >> 1) * ref_width) + xoff);

  YBuf = (unsigned char *) fieldBuf;
  for (i = 0; i < height; i++)
  {
    memcpy (YBuf, lumaAddr, width);
    YBuf += width;
    lumaAddr += ref_width;
  }

  /* for viewing the decoded content cb cr are separated in different planes */
  CbBuf = (unsigned char *) ((unsigned int) fieldBuf + pic_size);
  CrBuf =
    (unsigned char *) ((unsigned int) fieldBuf + pic_size + (pic_size >> 2));
  for (i = 0; i < (height >> 1); i++)
  {
    for (j = 0; j < (width >> 1); j++)
    {
      CbBuf[j] = chromaAddr[(j * 2)];
      CrBuf[j] = chromaAddr[(j * 2) + 1];
    }
    CbBuf += (width >> 1);
    CrBuf += (width >> 1);
    chromaAddr += ref_width;
  }

  /* Buffer is in 420 format, hence size is width x height x (3/2) */
  fwrite ((void *) fieldBuf, sizeof (unsigned char), ((pic_size * 3) >> 1),
          fout);
  return (retval);
}

/* ==========================================================================
*
*@func   TestApp_WriteOutputData()
*
*@brief  Application function to write output data to a file
*
*@param  fOutFile           output file ptr
*
*@param  outArgs            OutArgs from process
*
*@param  fieldBuf            Scratch buffer for interleaving the data
*
*@ret    none
*
* ============================================================================
*/
static void TestApp_WriteOutputData (FILE *fOutFile, 
                                     OMX_BUFFERHEADERTYPE *pBufHeader, 
                                     unsigned int imageHeight,
                                     unsigned int imageWidth,
                                     unsigned int imagePitch,
                                     void *fieldBuf)
{
  unsigned int paddedheight;
  unsigned int xOffset, yOffset;
  unsigned char *cBuffer;
  /*-------------------------------------------------------------------------*/
  /* Pointer to Display buffer structure */
  /*-------------------------------------------------------------------------*/
  /* Actual display frame in video decoder o/p buffer
     __________________________ | ____________________ | | | | | | | display
     Frame | | | |____________________| | |__________________________| */

  xOffset = pBufHeader->nOffset % imagePitch;
  yOffset = pBufHeader->nOffset / imagePitch;
  paddedheight = imageHeight + (yOffset << 1);

  /* chroma buffers pointer extarction from decoder buffer pointer */
  cBuffer =
    (unsigned char *) ((OMX_U32) pBufHeader->pBuffer  +
                       (((pBufHeader->nFilledLen + pBufHeader->nOffset) / 3) << 1));

  Decode_outputDisplayFrame (pBufHeader->pBuffer,
                             cBuffer,
                             (unsigned int) xOffset,
                             (unsigned int) yOffset,
                             imagePitch,
                             paddedheight,
                             (unsigned int) imageWidth,
                             (unsigned int) imageHeight, fieldBuf, fOutFile);

}

/* ========================================================================== */
/**
* Decode_AllocateResources() : Allocates the resources required for Decoder. 
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
static OMX_ERRORTYPE Decode_AllocateResources (Decode_Client *pAppData)
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

  if (pAppData->eCompressionFormat == OMX_VIDEO_CodingAVC)
  {
  }
  else if (pAppData->eCompressionFormat == OMX_VIDEO_CodingMPEG2)
  {
  }
  else if ( pAppData->eCompressionFormat == OMX_VIDEO_CodingH263   )
  {
  }
  else if ( pAppData->eCompressionFormat == OMX_VIDEO_CodingMPEG4   )
  {
  }
  else if ( pAppData->eCompressionFormat == OMX_VIDEO_CodingWMV   )
  {
  }
  else
  {
    printf ("Invalid compression format value.");
    eError = OMX_ErrorUnsupportedSetting;
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

  num_out_buffers = NUM_OF_OUT_BUFFERS;

  retval =
    TIMM_OSAL_CreatePipe (&(pAppData->OpBuf_Pipe),
                          sizeof (OMX_BUFFERHEADERTYPE *) * num_out_buffers,
                          sizeof (OMX_BUFFERHEADERTYPE *), OMX_TRUE);
  if (retval != 0)
  {
    printf ("Error: TIMM_OSAL_CreatePipe failed to open");
    eError = OMX_ErrorContentPipeCreationFailed;
    goto EXIT;
  }
  /* used for de-inteleaving the Cb Cr for viewing purpose only */
  printf (" allocating field buffer \n");
  
  /* The buffer has to be aligned to 128 bytes boundary */
  pAppData->fieldBuf =
    TIMM_OSAL_Malloc ((UTIL_ALIGN ((pAppData->nWidth + (2 * PADX)), 128) *
                       ((pAppData->nHeight + (4 * PADY))) * 3 / 2),
                      TIMM_OSAL_TRUE, 0, TIMMOSAL_MEM_SEGMENT_EXT);
EXIT:

  return eError;
}

/* ========================================================================== */
/**
* Decode_FreeResources() : Free the resources allocated for Decoder. 
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
static void Decode_FreeResources (Decode_Client *pAppData)
{
  /* freeing up IL client alloacted data structures */
  if (pAppData->pCb)
    TIMM_OSAL_Free (pAppData->pCb);

  if (pAppData->pInPortDef)
    TIMM_OSAL_Free (pAppData->pInPortDef);

  if (pAppData->pOutPortDef)
    TIMM_OSAL_Free (pAppData->pOutPortDef);

  if (pAppData->fieldBuf)
    TIMM_OSAL_Free (pAppData->fieldBuf);

  if (pAppData->IpBuf_Pipe)
    TIMM_OSAL_DeletePipe (pAppData->IpBuf_Pipe);

  if (pAppData->OpBuf_Pipe)
    TIMM_OSAL_DeletePipe (pAppData->OpBuf_Pipe);

  if (pAppData->eCompressionFormat == OMX_VIDEO_CodingAVC)
  {
   if(pAppData->pc.readBuf)
   {
    TIMM_OSAL_Free(pAppData->pc.readBuf);
   }
  }
  else if (pAppData->eCompressionFormat == OMX_VIDEO_CodingMPEG2)
  {
   if(pAppData->pcmpeg2.working_frame)
   {
    TIMM_OSAL_Free(pAppData->pcmpeg2.working_frame);
   }
   if(pAppData->pcmpeg2.savedbuff)
   {
    TIMM_OSAL_Free(pAppData->pcmpeg2.savedbuff);
   }
  }
  else if ( pAppData->eCompressionFormat == OMX_VIDEO_CodingH263   )
  {
   if(pAppData->pch263.working_frame)
   {
    TIMM_OSAL_Free(pAppData->pch263.working_frame);
   }
   if(pAppData->pch263.savedbuff)
   {
    TIMM_OSAL_Free(pAppData->pch263.savedbuff);
   }
  }
  else if ( pAppData->eCompressionFormat == OMX_VIDEO_CodingMPEG4   )
  {
   if(pAppData->pcmpeg4.working_frame)
   {
    TIMM_OSAL_Free(pAppData->pcmpeg4.working_frame);
   }
   if(pAppData->pcmpeg4.savedbuff)
   {
    TIMM_OSAL_Free(pAppData->pcmpeg4.savedbuff);
   }
  }
  else if ( pAppData->eCompressionFormat == OMX_VIDEO_CodingWMV   )
  {
   if(pAppData->pcvc1.working_frame)
   {
    TIMM_OSAL_Free(pAppData->pcvc1.working_frame);
   }
   if(pAppData->pcvc1.savedbuff)
   {
    TIMM_OSAL_Free(pAppData->pcvc1.savedbuff);
   }
  }

  return;
}

/* ========================================================================== */
/**
* Decode_GetDecoderErrorString() : Function to map the OMX error enum to string
*
* @param error   : OMX Error type
*
*  @return      
*  String conversion of the OMX_ERRORTYPE
*
*/
/* ========================================================================== */
static OMX_STRING Decode_GetDecoderErrorString (OMX_ERRORTYPE error)
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
* Decode_FillData() : Function to fill the input buffer with data.
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
static OMX_U32 Decode_FillData (Decode_Client *pAppData,
                                OMX_BUFFERHEADERTYPE *pBuf)
{
  OMX_U32 nRead = 0;

  if (pAppData->eCompressionFormat == OMX_VIDEO_CodingAVC)
  {
    pAppData->pc.outBuf.ptr = pBuf->pBuffer;
    pAppData->pc.outBuf.bufsize = pBuf->nAllocLen;
    pAppData->pc.outBuf.bufused = 0;
    nRead = Decode_GetNextFrameSize (&pAppData->pc);
  }
  else if (pAppData->eCompressionFormat == OMX_VIDEO_CodingMPEG2)
  {
    pAppData->pcmpeg2.buff_in = pBuf->pBuffer;
    nRead = Decode_GetNextMpeg2FrameSize (&pAppData->pcmpeg2);
  }
  else if (pAppData->eCompressionFormat == OMX_VIDEO_CodingH263 )
  {
    pAppData->pch263.buff_in = pBuf->pBuffer;
    nRead = Decode_GetNextH263FrameSize (&pAppData->pch263, pAppData->IpBuf_Pipe);
  }
  else if (pAppData->eCompressionFormat == OMX_VIDEO_CodingMPEG4 )
  {
    pAppData->pcmpeg4.buff_in = pBuf->pBuffer;
    nRead = Decode_GetNextMpeg4FrameSize (&pAppData->pcmpeg4, pAppData->IpBuf_Pipe);
  }
  else if (pAppData->eCompressionFormat == OMX_VIDEO_CodingWMV )
  {
    pAppData->pcvc1.buff_in = pBuf->pBuffer;
    nRead = Decode_GetNextVC1FrameSize (&pAppData->pcvc1, pAppData->IpBuf_Pipe);
  }

  pBuf->nFilledLen = nRead;
  pBuf->nOffset = 0;

  return nRead;
}

/* ========================================================================== */
/**
* Decode_WaitForState() : This method will wait for the component to get 
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
static OMX_ERRORTYPE Decode_WaitForState (OMX_HANDLETYPE *pHandle,
                                          OMX_STATETYPE DesiredState)
{
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  TIMM_OSAL_U32 uRequestedEvents, pRetrievedEvents;
  TIMM_OSAL_ERRORTYPE retval;

  /* Wait for an event, which would be triggered through callback function */
  uRequestedEvents = (DECODER_STATETRANSITION_COMPLETE | DECODER_ERROR_EVENT);
  retval =
    TIMM_OSAL_EventRetrieve (DECODER_CmdEvent, uRequestedEvents,
                             TIMM_OSAL_EVENT_OR_CONSUME, &pRetrievedEvents,
                             TIMM_OSAL_SUSPEND);

  if (TIMM_OSAL_ERR_NONE != retval)
  {
    TIMM_OSAL_Trace ("\nError in EventRetrieve !\n");
    eError = OMX_ErrorInsufficientResources;
    goto EXIT;
  }

  if (pRetrievedEvents & DECODER_ERROR_EVENT)
  {
    eError = OMX_ErrorUndefined;
  }
  else
  {
    eError = OMX_ErrorNone;
  }

EXIT:

  return eError;
}
#ifdef DECODE_CHANGEPORTSETTINGS
/* ========================================================================== */
/**
* Decode_ChangePortSettings() : This method will 
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
static OMX_ERRORTYPE Decode_ChangePortSettings (Decode_Client *pAppData)
{

  TIMM_OSAL_ERRORTYPE retval;
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  OMX_U32 i;

  /* in case we need to change the port setting, while executing, port needs to 
     be disabled, parameters needs to be changed, and buffers would be
     alloacted as new sizes and port would be enabled */
 printf("Decode_ChangePortSettings\n");
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

    retval =
      TIMM_OSAL_WriteToPipe (pAppData->OpBuf_Pipe, &pAppData->pOutBuff[i],
                             sizeof (pAppData->pOutBuff[i]), TIMM_OSAL_SUSPEND);
    if (retval != TIMM_OSAL_ERR_NONE)
    {
      printf ("Error in writing to out pipe!");
      eError = OMX_ErrorNotReady;
      return eError;
    }
  }

EXIT:

  return eError;
}
#endif
/* ========================================================================== */
/**
* Decode_EventHandler() : This method is the event handler implementation to 
* handle events from the decoder
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
static OMX_ERRORTYPE Decode_EventHandler (OMX_HANDLETYPE hComponent,
                                          OMX_PTR ptrAppData,
                                          OMX_EVENTTYPE eEvent, OMX_U32 nData1,
                                          OMX_U32 nData2, OMX_PTR pEventData)
{
/*  Decode_Client *pAppData = ptrAppData;*/
/*    OMX_STATETYPE state;*/
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
          TIMM_OSAL_EventSet (DECODER_CmdEvent,
                              DECODER_STATETRANSITION_COMPLETE,
                              TIMM_OSAL_EVENT_OR);
        if (retval != TIMM_OSAL_ERR_NONE)
        {
          TIMM_OSAL_Trace ("\nError in setting the event!\n");
          eError = OMX_ErrorNotReady;
          return eError;
        }
      }
      break;
    case OMX_EventError:
      /* component has generated error */
      printf ("Component generated error; OMX error ID = 0x%x\n",
              (unsigned int) nData1);
      /*TIMM_OSAL_SemaphoreRelease (pSem_Events);
      retval =
        TIMM_OSAL_EventSet (DECODER_CmdEvent, DECODER_ERROR_EVENT,
                            TIMM_OSAL_EVENT_OR);
      if (retval != TIMM_OSAL_ERR_NONE)
      {
        TIMM_OSAL_Trace ("\nError in setting the event!\n");
        eError = OMX_ErrorNotReady;
        return eError;
      }*/
      break;
    case OMX_EventMark:
      break;
    case OMX_EventPortSettingsChanged:
      /* In case of change in output buffer sizes re-allocate the buffers */
      /*eError = Decode_ChangePortSettings (pAppData);*/
      printf ("Component generated OMX_EventPortSettingsChanged; likely cause: change in resolution\n");
      break;
    case OMX_EventBufferFlag:
      retval =
        TIMM_OSAL_EventSet (myEvent, DECODER_END_OF_STREAM, TIMM_OSAL_EVENT_OR);
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
  }                             // end of switch

  return eError;
}

/* ========================================================================== */
/**
* Decode_FillBufferDone() : This method handles the fill buffer done event
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
static OMX_ERRORTYPE Decode_FillBufferDone (OMX_HANDLETYPE hComponent,
                                            OMX_PTR ptrAppData,
                                            OMX_BUFFERHEADERTYPE *pBuffer)
{
  Decode_Client *pAppData = ptrAppData;
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  TIMM_OSAL_ERRORTYPE retval;

  if(pAppData->stopFlag)
  { 
    return eError;
  }


  {
#ifdef H264_LINUX_CLIENT
#ifdef SRCHANGES
    TIMM_OSAL_Trace ("\npBuffer SR after FBD = %x\n", pBuffer->pBuffer);
    pBuffer->pBuffer = SharedRegion_getPtr (pBuffer->pBuffer);
    TIMM_OSAL_Trace ("\npBuffer after FBD = %x\n", pBuffer->pBuffer);
#endif
#endif

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
      TIMM_OSAL_EventSet (myEvent, DECODER_OUTPUT_READY, TIMM_OSAL_EVENT_OR);
    if (retval != TIMM_OSAL_ERR_NONE)
    {
      printf ("Error in setting the o/p event!");
      eError = OMX_ErrorNotReady;
      return eError;
    }
  }

  return eError;
}

/* ========================================================================== */
/**
* Decode_FillBufferDone() : This method handles the Empty buffer done event
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
static OMX_ERRORTYPE Decode_EmptyBufferDone (OMX_HANDLETYPE hComponent,
                                             OMX_PTR ptrAppData,
                                             OMX_BUFFERHEADERTYPE *pBuffer)
{
  Decode_Client *pAppData = ptrAppData;
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  TIMM_OSAL_ERRORTYPE retval;

  if(pAppData->stopFlag)
  { 
    return eError;
  }


#ifdef H264_LINUX_CLIENT
#ifdef SRCHANGES
  TIMM_OSAL_Trace ("\npBuffer SR after EBD = %x\n", pBuffer->pBuffer);
  pBuffer->pBuffer = SharedRegion_getPtr (pBuffer->pBuffer);
  TIMM_OSAL_Trace ("\npBuffer after EBD = %x\n", pBuffer->pBuffer);
#endif
#endif
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
    TIMM_OSAL_EventSet (myEvent, DECODER_INPUT_READY, TIMM_OSAL_EVENT_OR);
  if (retval != TIMM_OSAL_ERR_NONE)
  {
    printf ("Error in setting the event!");
    eError = OMX_ErrorNotReady;
    return eError;
  }

  return eError;
}

/* ========================================================================== */
/**
* Decode_SetParamPortDefinition() : Function to fill the port definition 
* structures and call the Set_Parameter function on to the H264 Decoder
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
OMX_ERRORTYPE Decode_SetParamPortDefinition (OMX_HANDLETYPE handle,
                                             Int32 width,
                                             Int32 height, 
                                             Int32 compressionFormat)
{
  OMX_ERRORTYPE eError = OMX_ErrorUndefined;
  OMX_HANDLETYPE pHandle = handle;
  OMX_PORT_PARAM_TYPE portInit;
  OMX_PARAM_PORTDEFINITIONTYPE pInPortDef, pOutPortDef;
  OMX_PARAM_COMPPORT_NOTIFYTYPE pNotifyType;

  if (!pHandle)
  {
    eError = OMX_ErrorBadParameter;
    goto EXIT;
  }

  OMX_TEST_INIT_STRUCT_PTR (&portInit, OMX_PORT_PARAM_TYPE);

  portInit.nPorts = 2;
  portInit.nStartPortNumber = 0;
  eError = OMX_SetParameter (pHandle, OMX_IndexParamVideoInit, &portInit);
  if (eError != OMX_ErrorNone)
  {
    goto EXIT;
  }

  /* Set the component's OMX_PARAM_PORTDEFINITIONTYPE structure (input) */

  OMX_TEST_INIT_STRUCT_PTR (&pInPortDef, OMX_PARAM_PORTDEFINITIONTYPE);

  /* populate the input port definataion structure, It is Standard OpenMax
     structure */
  /* set the port index */
  pInPortDef.nPortIndex = OMX_VIDDEC_INPUT_PORT;
  /* It is input port so direction is set as Input, Empty buffers call would be 
     accepted based on this */
  pInPortDef.eDir = OMX_DirInput;
  /* number of buffers are set here */
  pInPortDef.nBufferCountActual = 4;
  pInPortDef.nBufferCountMin = 1;
  /* buffer size by deafult is assumed as width * height for input bitstream
     which would suffice most of the cases */
  pInPortDef.nBufferSize = width * height;

  pInPortDef.bEnabled = OMX_TRUE;
  pInPortDef.bPopulated = OMX_FALSE;
  pInPortDef.eDomain = OMX_PortDomainVideo;
  pInPortDef.bBuffersContiguous = OMX_FALSE;
  pInPortDef.nBufferAlignment = 0x0;

  /* OMX_VIDEO_PORTDEFINITION values for input port */
  pInPortDef.format.video.cMIMEType = "H264";
  pInPortDef.format.video.pNativeRender = NULL;
  /* set the width and height, used for buffer size calculation */
  pInPortDef.format.video.nFrameWidth = width;
  pInPortDef.format.video.nFrameHeight = height;
  /* for bitstream buffer stride is not a valid parameter */
  pInPortDef.format.video.nStride = -1;
  /* component supports only frame based processing */
  pInPortDef.format.video.nSliceHeight = 0;

  /* bitrate does not matter for decoder */
  pInPortDef.format.video.nBitrate = 104857600;
  /* as per openmax frame rate is in Q16 format */
  pInPortDef.format.video.xFramerate = 60 << 16;
  /* input port would receive H264 stream */
  pInPortDef.format.video.eCompressionFormat = compressionFormat;
  /* this is codec setting, OMX component does not support it */
  pInPortDef.format.video.bFlagErrorConcealment = OMX_FALSE;
  /* color format is irrelavant */
  pInPortDef.format.video.eColorFormat = OMX_COLOR_FormatYUV420Planar;

  eError =
    OMX_SetParameter (pHandle, OMX_IndexParamPortDefinition, &pInPortDef);
  if (eError != OMX_ErrorNone)
  {
    printf("input port OMX_SetParameter failed\n");
    goto EXIT;
  }

  /* Set the component's OMX_PARAM_PORTDEFINITIONTYPE structure (output) */
  OMX_TEST_INIT_STRUCT_PTR (&pOutPortDef, OMX_PARAM_PORTDEFINITIONTYPE);

  /* setting the port index for output port, properties are set based on this
     index */
  pOutPortDef.nPortIndex = OMX_VIDDEC_OUTPUT_PORT;
  pOutPortDef.eDir = OMX_DirOutput;
  /* componet would expect these numbers of buffers to be allocated */
  pOutPortDef.nBufferCountActual = 8;
  pOutPortDef.nBufferCountMin = 1;

  /* Codec requires padded height and width and width needs to be aligned at
     128 byte boundary */
  pOutPortDef.nBufferSize =
    (UTIL_ALIGN ((width + (2 * PADX)), 128) * ((((height + 15) & 0xfffffff0) + (4 * PADY))) * 3) >> 1;
  
  printf("Output port buffer size: %d\n", (int) pOutPortDef.nBufferSize);

  pOutPortDef.bEnabled = OMX_TRUE;
  pOutPortDef.bPopulated = OMX_FALSE;
  pOutPortDef.eDomain = OMX_PortDomainVideo;
  /* currently component alloactes contigous buffers with 128 alignment, these
     values are do't care */
  pOutPortDef.bBuffersContiguous = OMX_FALSE;
  pOutPortDef.nBufferAlignment = 0x0;

  /* OMX_VIDEO_PORTDEFINITION values for output port */
  pOutPortDef.format.video.cMIMEType = "H264";
  pOutPortDef.format.video.pNativeRender = NULL;
  pOutPortDef.format.video.nFrameWidth = width;
  pOutPortDef.format.video.nFrameHeight = ((height + 15) & 0xfffffff0);

  /* stride is set as buffer width */
  pOutPortDef.format.video.nStride = UTIL_ALIGN (width + (2 * PADX), 128);
  pOutPortDef.format.video.nSliceHeight = 0;

  /* bitrate does not matter for decoder */
  pOutPortDef.format.video.nBitrate = 25000000;
  /* as per openmax frame rate is in Q16 format */
  pOutPortDef.format.video.xFramerate = 60 << 16;
  pOutPortDef.format.video.bFlagErrorConcealment = OMX_FALSE;
  /* output is raw YUV 420 SP format, It support only this */
  pOutPortDef.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
  pOutPortDef.format.video.eColorFormat = OMX_COLOR_FormatYUV420PackedSemiPlanar;

  eError =
    OMX_SetParameter (pHandle, OMX_IndexParamPortDefinition, &pOutPortDef);
  if (eError != OMX_ErrorNone)
  {
    printf("output port OMX_SetParameter failed\n");
    eError = OMX_ErrorBadParameter;
    goto EXIT;
  }

  /* Make VDEC execute periodically based on fps */
  OMX_TEST_INIT_STRUCT_PTR(&pNotifyType, OMX_PARAM_COMPPORT_NOTIFYTYPE);
  pNotifyType.eNotifyType = OMX_NOTIFY_TYPE_NONE;
  pNotifyType.nPortIndex =  OMX_VIDDEC_INPUT_PORT;
  eError = 
    OMX_SetParameter (pHandle, OMX_TI_IndexParamCompPortNotifyType,
                      &pNotifyType);
  if (eError != OMX_ErrorNone)
  {
    printf("input port OMX_SetParameter failed\n");
    eError = OMX_ErrorBadParameter;
    goto EXIT;
  }
  pNotifyType.eNotifyType = OMX_NOTIFY_TYPE_NONE;
  pNotifyType.nPortIndex =  OMX_VIDDEC_OUTPUT_PORT;
  eError = 
    OMX_SetParameter (pHandle, OMX_TI_IndexParamCompPortNotifyType,
                      &pNotifyType);
  if (eError != OMX_ErrorNone)
  {
    printf("output port OMX_SetParameter failed\n");
    eError = OMX_ErrorBadParameter;
    goto EXIT;
  }
EXIT:
  return eError;
}

/******************************************************************************/
/* Main entrypoint into the Test */
/******************************************************************************/
Int Decode_Test (unsigned int nFrameWidth, unsigned int nFrameHeight,
                 char *sFileName, char *sOutFileName, char *format)
{
  Decode_Client *pAppData = TIMM_OSAL_NULL;
  OMX_HANDLETYPE pHandle;
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  OMX_BUFFERHEADERTYPE *pBufferIn = NULL;
  OMX_BUFFERHEADERTYPE *pBufferOut = NULL;
  TIMM_OSAL_ERRORTYPE tTIMMSemStatus;
  OMX_U32 i;
  OMX_U32 actualSize;
  OMX_U32 nRead;
  OMX_CALLBACKTYPE appCallbacks;
  int frames_decoded;

  TIMM_OSAL_U32 uRequestedEvents, pRetrievedEvents;

  printf ("Iteration %d - Start\n", 0);


  /* Callbacks are passed during getHandle call to component, Componnet uses
     these callaback to communicate with IL Client */

  appCallbacks.EventHandler = Decode_EventHandler;
  appCallbacks.EmptyBufferDone = Decode_EmptyBufferDone;
  appCallbacks.FillBufferDone = Decode_FillBufferDone;

  frames_decoded = 0;

  bEOS_Sent = OMX_FALSE;

  /* Create evenets, which will be triggered during callback from componnet */

  tTIMMSemStatus = TIMM_OSAL_EventCreate (&myEvent);
  if (TIMM_OSAL_ERR_NONE != tTIMMSemStatus)
  {
    printf ("Error in creating event!");
    eError = OMX_ErrorInsufficientResources;
    goto EXIT;
  }

  tTIMMSemStatus = TIMM_OSAL_EventCreate (&DECODER_CmdEvent);
  if (TIMM_OSAL_ERR_NONE != tTIMMSemStatus)
  {
    TIMM_OSAL_Trace ("Error in creating event!\n");
    eError = OMX_ErrorInsufficientResources;
    goto EXIT;
  }
  /* Allocating data structure for IL client structure / buffer management */

  pAppData =
    (Decode_Client *) TIMM_OSAL_Malloc (sizeof (Decode_Client),
                                        TIMM_OSAL_TRUE, 0,
                                        TIMMOSAL_MEM_SEGMENT_EXT);
  if (!pAppData)
  {
    printf ("Error allocating pAppData!");
    eError = OMX_ErrorInsufficientResources;
    goto EXIT;
  }
  memset (pAppData, 0x0, sizeof (Decode_Client));

  pAppData->stopFlag = 0;
  /* Open the file of data to be rendered.  */
  pAppData->fIn = fopen (sFileName, "rb");;

  if (pAppData->fIn == NULL)
  {
    printf ("Error: failed to open the file %s for reading\n", sFileName);
    goto EXIT;
  }
  /* opening the output data file, decoded data is written back to this file */

  pAppData->fOut = fopen (sOutFileName, "wb");

  if (pAppData->fOut == NULL)
  {
    printf ("Error: failed to open the file  for writing \n");
    goto EXIT;
  }
  /* populating the parameter in IL client structure, from this it will be
     passed to component via setparam call */

  /* compression format as H264, OMX enumeration */
  if (strcmp (format, "h264") == 0)
  {
    PADX = PADX_H264;
    PADY = PADY_H264;
    pAppData->eCompressionFormat = OMX_VIDEO_CodingAVC;
    pAppData->nPitch = UTIL_ALIGN (nFrameWidth + (2 * PADX), 128);
    Decode_ParserInit (&pAppData->pc, pAppData->fIn);
  }
  else if (strcmp (format, "mpeg2") == 0)
  {
    PADX = PADX_MPEG2;
    PADY = PADY_MPEG2;
    pAppData->eCompressionFormat = OMX_VIDEO_CodingMPEG2;
    pAppData->nPitch = nFrameWidth;
    Decode_Mpeg2ParserInit (&pAppData->pcmpeg2, pAppData->fIn);
  }
  else if (strcmp (format, "mpeg4") == 0)
  {
    PADX = PADX_MPEG4;
    PADY = PADY_MPEG4;
    pAppData->eCompressionFormat = OMX_VIDEO_CodingMPEG4;
    pAppData->nPitch = UTIL_ALIGN (nFrameWidth + (2 * PADX), 128);
    Decode_Mpeg4ParserInit (&pAppData->pcmpeg4, pAppData->fIn);
  }
  else if (strcmp (format, "h263") == 0)
  {
    PADX = PADX_MPEG4;
    PADY = PADY_MPEG4;
    pAppData->eCompressionFormat = OMX_VIDEO_CodingH263;
    pAppData->nPitch = UTIL_ALIGN (nFrameWidth + (2 * PADX), 128);
    Decode_H263ParserInit (&pAppData->pch263, pAppData->fIn);
  }
  else if (strcmp (format, "vc1") == 0)
  {
    PADX = PADX_VC1;
    PADY = PADY_VC1;
    pAppData->eCompressionFormat = OMX_VIDEO_CodingWMV;
    pAppData->nPitch = UTIL_ALIGN (nFrameWidth + (2 * PADX), 128);
    Decode_VC1ParserInit (&pAppData->pcvc1, pAppData->fIn);
  }
  else
  {
    printf
      (" Invalid bitstream format specified, should be either h264,mpeg2,mpeg4 or h263. %s is invalid \n",format);
    return -1;
  }

  pAppData->nWidth = nFrameWidth;
  pAppData->nHeight = nFrameHeight;

  /* decoder supports 420 packed semeplanner format only, luma is in one plane 
     and Cb Cr are interleaved in another plane */
  pAppData->ColorFormat = OMX_COLOR_FormatYUV420PackedSemiPlanar;

  /* Allocating data structure for buffer queues in IL client */
  eError = Decode_AllocateResources (pAppData);
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

  eError =
    OMX_GetHandle (&pHandle, (OMX_STRING) "OMX.TI.DUCATI.VIDDEC", pAppData,
                   pAppData->pCb);

  if ((eError != OMX_ErrorNone) || (pHandle == NULL))
  {
    printf ("Error in Get Handle function : %s \n",
            Decode_GetDecoderErrorString (eError));
    goto EXIT;
  }

  pAppData->pHandle = pHandle;
  pAppData->pComponent = (OMX_COMPONENTTYPE *) pHandle;

  /* for input port parameter settings */

  /* number of bufferes are port properties, component tracks number of buffers 
     allocated during loaded to idle transition */
  pAppData->pInPortDef->nBufferCountActual = 4;
  pAppData->pInPortDef->nPortIndex = OMX_VIDDEC_INPUT_PORT;
  pAppData->pInPortDef->nBufferSize = pAppData->nWidth * pAppData->nHeight;

  /* for output port parameters setting */
  pAppData->pOutPortDef->nBufferCountActual = 8;
  pAppData->pOutPortDef->nPortIndex = OMX_VIDDEC_OUTPUT_PORT;

  /* H264 codec requires padded and aligned buffers at output port; in
     horizonatl direction padding is 32 bytes on either side, In vertical
     direction padding is 24 on top and bottom, for both planes Y and CbCr it
     becomes 24*4, buffer width should be aligned to 128 bytes */

  pAppData->pOutPortDef->nBufferSize =
    (UTIL_ALIGN (pAppData->nWidth + (2 * PADX), 128) * ((((pAppData->nHeight + 15) & 0xfffffff0) +
                                                         (4 * PADY))) * 3) >> 1;

  /* calling OMX_Setparam in this function */
  Decode_SetParamPortDefinition (pAppData->pHandle, pAppData->nWidth,
                                 pAppData->nHeight,
                                 pAppData->eCompressionFormat);

  /* OMX_SendCommand expecting OMX_StateIdle, after this command component
     would create codec, and will wait for all buffers to be allocated */
  eError = OMX_SendCommand (pHandle, OMX_CommandStateSet, OMX_StateIdle, NULL);
  if (eError != OMX_ErrorNone)
  {
    printf ("Error in SendCommand()-OMX_StateIdle State set : %s \n",
            Decode_GetDecoderErrorString (eError));
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
              Decode_GetDecoderErrorString (eError));
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
              Decode_GetDecoderErrorString (eError));
      goto EXIT;
    }
  }

  /* Wait for initialization to complete.. Wait for Idle stete of component
     after all buffers are alloacted componet would chnage to idle */

  eError = Decode_WaitForState (pHandle, OMX_StateIdle);
  if (eError != OMX_ErrorNone)
  {
    printf ("Error %s:    WaitForState has timed out \n",
            Decode_GetDecoderErrorString (eError));
    goto EXIT;
  }
  printf (" state IDLE \n ");

  /* change state to execute so that buffers processing can start */
  eError =
    OMX_SendCommand (pHandle, OMX_CommandStateSet, OMX_StateExecuting, NULL);
  if (eError != OMX_ErrorNone)
  {
    printf ("Error from SendCommand-Executing State set :%s \n",
            Decode_GetDecoderErrorString (eError));
    goto EXIT;
  }

  eError = Decode_WaitForState (pHandle, OMX_StateExecuting);
  if (eError != OMX_ErrorNone)
  {
    printf ("Error %s:    WaitForState has timed out \n",
            Decode_GetDecoderErrorString (eError));
    goto EXIT;
  }
  printf (" state execute \n ");

  /* parser would fill the chunked frames in input buffers, which needs to be
     passed to component in empty buffer call; initially all buffers are
     avaialble so they be filled and given to component */

  for (i = 0; i < pAppData->pInPortDef->nBufferCountActual; i++)
  {
    nRead = Decode_FillData (pAppData, pAppData->pInBuff[i]);

    eError =
      pAppData->pComponent->EmptyThisBuffer (pHandle, pAppData->pInBuff[i]);

    if (eError != OMX_ErrorNone)
    {
      printf ("Error from Empty this buffer : %s \n",
              Decode_GetDecoderErrorString (eError));
      goto EXIT;
    }
  }

  /* Initially all bufers are available in IL client, so we can pass all free
     buffers to component */
  for (i = 0; i < pAppData->pOutPortDef->nBufferCountActual; i++)
  {
    eError =
      pAppData->pComponent->FillThisBuffer (pHandle, pAppData->pOutBuff[i]);
    if (eError != OMX_ErrorNone)
    {
      printf ("Error from Fill this buffer : %s \n",
              Decode_GetDecoderErrorString (eError));
      goto EXIT;
    }
  }

  printf (" etb / ftb done \n ");

  eError = OMX_GetState (pHandle, &pAppData->eState);

  /* Initialize the number of encoded frames to zero */
  pAppData->nEncodedFrms = 0;

  /* all available buffers have been passed to component, now wait for
     processed buffers to come back via eventhandler callback */

  while ((eError == OMX_ErrorNone) && (pAppData->eState != OMX_StateIdle))
  {

    TIMM_OSAL_U32 numRemaining = 0;

    uRequestedEvents =
      (DECODER_INPUT_READY | DECODER_OUTPUT_READY |
       DECODER_ERROR_EVENT | DECODER_END_OF_STREAM);
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

    if (pRetrievedEvents & DECODER_END_OF_STREAM)
    {
      printf ("End of stream processed\n");

      break;
    }

    if (pRetrievedEvents & DECODER_OUTPUT_READY)
    {
      /* Check if buffer is avaialble in the pipe, componet would return the
         buffer after processing, which IL client keeps in a queue which is
         implemented as pipe in this example */
      TIMM_OSAL_GetPipeReadyMessageCount (pAppData->OpBuf_Pipe, &numRemaining);

      while (numRemaining)
      {
        /* read from the pipe */
        TIMM_OSAL_ReadFromPipe (pAppData->OpBuf_Pipe, &pBufferOut,
                                sizeof (pBufferOut), &actualSize,
                                TIMM_OSAL_SUSPEND);
        if (pBufferOut->nFilledLen != 0)
        {

          printf (" frame %d  displayed \n", (int) ++pAppData->nEncodedFrms);

          /* decoded data is written back to file */
          TestApp_WriteOutputData (pAppData->fOut,
                                   pBufferOut, pAppData->nHeight,
                                   pAppData->nWidth, pAppData->nPitch,
                                   pAppData->fieldBuf);

          fflush (pAppData->fOut);
        }

        /* Buffer is recycled to component */
        pBufferOut->nFilledLen = 0;

        eError = pAppData->pComponent->FillThisBuffer (pHandle, pBufferOut);
        if (eError != OMX_ErrorNone)
        {
          printf ("Error from Fill this buffer : %s \n",
                  Decode_GetDecoderErrorString (eError));
          goto EXIT;
        }
        TIMM_OSAL_GetPipeReadyMessageCount (pAppData->OpBuf_Pipe,
                                            &numRemaining);
      
      } /* while (numRemaining) */
    } /* if (pRetrievedEvents...) */

    if (pRetrievedEvents & DECODER_INPUT_READY)
    {
      TIMM_OSAL_GetPipeReadyMessageCount (pAppData->IpBuf_Pipe, &numRemaining);
      while (numRemaining && !bEOS_Sent)
      {

        /* read from the pipe, pipe would have consumed buffers returned by
           component */

        TIMM_OSAL_ReadFromPipe (pAppData->IpBuf_Pipe, &pBufferIn,
                                sizeof (pBufferIn), &actualSize,
                                TIMM_OSAL_SUSPEND);

        if (pBufferIn == TIMM_OSAL_NULL)
        {
          printf ("\n Null received from pipe");
        }
        /* fill new frames in the buffer and pass it to component */
        nRead = Decode_FillData(pAppData, pBufferIn);

        pBufferIn->nTickCount = 0;

        printf (" frame %d  decoded \n", ++frames_decoded);

        /* in this example, we decode 30 frames and start de-initialization of 
           the component */

        if(!nRead)
        {
          pAppData->stopFlag = 1;
          goto EXIT1;
        }
        eError = pAppData->pComponent->EmptyThisBuffer (pHandle, pBufferIn);

        if (eError != OMX_ErrorNone)
        {
          printf ("Error from Empty this buffer : %s \n",
                  Decode_GetDecoderErrorString (eError));
          goto EXIT;
        }
        /* Check if more buffers are available */
        eError = TIMM_OSAL_GetPipeReadyMessageCount (pAppData->IpBuf_Pipe,
                                            &numRemaining);
      }
    }

    if (pRetrievedEvents & DECODER_ERROR_EVENT)
    {
      eError = OMX_ErrorUndefined;
    }

    eError = OMX_GetState (pHandle, &pAppData->eState);
  }

EXIT1:
  printf ("Tearing down the decode example\n");  

#if 0
/*Retrieve any error events. There might be decode errors*/
  uRequestedEvents = DECODER_ERROR_EVENT;
  eError =
    TIMM_OSAL_EventRetrieve (DECODER_CmdEvent, uRequestedEvents,
                             TIMM_OSAL_EVENT_OR_CONSUME, &pRetrievedEvents,
                             TIMM_OSAL_NO_SUSPEND);

  if (TIMM_OSAL_ERR_NONE != eError)
  {
    TIMM_OSAL_Trace ("\nError in EventRetrieve !\n");
    eError = OMX_ErrorInsufficientResources;
    goto EXIT;
  }

#endif
  /* change the state to idle, bufefr communication would stop in this state */
  eError = OMX_SendCommand (pHandle, OMX_CommandStateSet, OMX_StateIdle, NULL);
  if (eError != OMX_ErrorNone)
  {
    printf ("Error from SendCommand-Idle State set : %s \n",
            Decode_GetDecoderErrorString (eError));
    goto EXIT;
  }

  eError = Decode_WaitForState (pHandle, OMX_StateIdle);
  if (eError != OMX_ErrorNone)
  {
    printf ("Error %s:    WaitForState has timed out \n",
            Decode_GetDecoderErrorString (eError));
    goto EXIT;
  }

  /* change the state to loaded, componenet would wait for all buffers to be
     freed up, then only state transition would complete */
  eError =
    OMX_SendCommand (pHandle, OMX_CommandStateSet, OMX_StateLoaded, NULL);
  if (eError != OMX_ErrorNone)
  {
    printf ("Error from SendCommand-Loaded State set : %s \n",
            Decode_GetDecoderErrorString (eError));
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
              Decode_GetDecoderErrorString (eError));
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
              Decode_GetDecoderErrorString (eError));
      goto EXIT;
    }
  }
  /* wait for state transition to complete, componnet would generate an event,
     when state transition is complete */

  eError = Decode_WaitForState (pHandle, OMX_StateLoaded);
  if (eError != OMX_ErrorNone)
  {
    printf ("Error %s:    WaitForState has timed out \n",
            Decode_GetDecoderErrorString (eError));
    goto EXIT;
  }
  printf (" free handle \n ");

  /* UnLoad the Decoder Component */
  eError = OMX_FreeHandle (pHandle);
  if ((eError != OMX_ErrorNone))
  {
    printf ("Error in Free Handle function : %s \n",
            Decode_GetDecoderErrorString (eError));
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
    if (pAppData->fIn)
      fclose (pAppData->fIn);

    if (pAppData->fOut)
      fclose (pAppData->fOut);

    Decode_FreeResources (pAppData);

    TIMM_OSAL_Free (pAppData);
  }

  tTIMMSemStatus = TIMM_OSAL_EventDelete (myEvent);
  if (TIMM_OSAL_ERR_NONE != tTIMMSemStatus)
  {
    printf ("Error in deleting event!");
  }

  tTIMMSemStatus = TIMM_OSAL_EventDelete (DECODER_CmdEvent);
  if (TIMM_OSAL_ERR_NONE != tTIMMSemStatus)
  {
    TIMM_OSAL_Trace ("Error in deleting event!\n");
  }
  
  
  printf ("Decoder Test End\n");

  return (0);
}                               /* DECODE_Test */

/* decode_test.c - EOF */
