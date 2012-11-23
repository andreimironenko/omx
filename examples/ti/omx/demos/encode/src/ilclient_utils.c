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
#include <pthread.h>
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
#include <omx_venc.h>
#include <omx_vfpc.h>
#include <omx_vfdc.h>
#include <omx_ctrl.h>
#include <omx_vfcc.h>
#include <OMX_TI_Index.h>
/*---------------------- function prototypes ---------------------------------*/
/* None */

void usage (IL_ARGS *argsp)
{
  printf
    ("encode_a8host_debug.xv5T \n"
     "-o | --output          output filename \n"
     "-i | --input           input filename \n"
     "-f | --framerate       encode frame rate \n"
     "-b | --bitrate         encode bit rate \n"
     "-w | --width           encode width \n"
     "-h | --height          encode height \n"
     "-c | --codec           h264, mpeg4, h263 \n"
   );                      
  printf(" example -    ./encode_a8host_debug.xv5T -o sample.h264 -i sample.yuv -f 60 -b 1000000 -w 1920 -h 1080 -c h264\n");
  
  exit (1);
}

/* ========================================================================== */
/**
* parse_args() : This function parses the input arguments provided to app.
*
* @param argc             : number of args 
* @param argv             : args passed by app
* @param argsp            : parsed data pointer
*
*  @return      
*
*
*/
/* ========================================================================== */

void parse_args (int argc, char *argv[], IL_ARGS *argsp)
{
  const char shortOptions[] = "o:i:w:h:f:b:c:";
  const struct option longOptions[] =
  {
    {"output", required_argument, NULL, ArgID_OUTPUT_FILE},
    {"input", required_argument, NULL, ArgID_INPUT_FILE},
    {"width", required_argument, NULL, ArgID_WIDTH},
    {"height", required_argument, NULL, ArgID_HEIGHT},
    {"framerate", required_argument, NULL, ArgID_FRAMERATE},
    {"bitrate", required_argument, NULL, ArgID_BITRATE},
    {"codec", required_argument, NULL, ArgID_CODEC},
    {0, 0, 0, 0}
  };

  int index, outfile = 0, infile = 0, codec = 0, width = 0, height =0;
  int argID;

  for (;;)
  {
    argID = getopt_long (argc, argv, shortOptions, longOptions, &index);

    if (argID == -1)
    {
      break;
    }

    switch (argID)
    {
      case ArgID_OUTPUT_FILE:
      case 'o':
        strncpy (argsp->output_file, optarg, MAX_FILE_NAME_SIZE);
        outfile = 1;
        break;
      case 'i':
        strncpy (argsp->input_file, optarg, MAX_FILE_NAME_SIZE);
        infile = 1;
        break;
      case ArgID_FRAMERATE:
      case 'f':
        argsp->frame_rate = atoi (optarg);
        break;
      case ArgID_WIDTH:
      case 'w':
        argsp->width = atoi (optarg);
        width = 1;
        break;
      case ArgID_HEIGHT:
      case 'h':
        argsp->height = atoi (optarg);
        height = 1;
        break;
      case ArgID_BITRATE:
      case 'b':
        argsp->bit_rate = atoi (optarg);
        break;
      case ArgID_CODEC:
      case 'c':
        strncpy (argsp->codec, optarg, MAX_CODEC_NAME_SIZE);
        codec = 1;
        break;
      default:
        usage (argsp);
        exit (1);
    }
  }

  if (optind < argc)
  {
    usage (argsp);
    exit (EXIT_FAILURE);
  }

  if (argsp->bit_rate == 0 || !outfile || !infile || argsp->frame_rate == 0  || !width || !height || !codec)
  {
    usage (argsp);
    exit (1);
  }

  printf ("output file: %s\n", argsp->output_file);
  printf ("input file: %s\n", argsp->input_file);
  printf ("bit_rate: %d\n", argsp->bit_rate);
  printf ("frame_rate: %d\n", argsp->frame_rate);
  printf ("codec: %s\n", argsp->codec);
  printf ("width: %d\n", argsp->width);
  printf ("height: %d\n", argsp->height);
}

/* ========================================================================== */
/**
* IL_ClientInit() : This function is to allocate and initialize the application
*                   data structure. It is just to maintain application control.
*
* @param pAppData          : appliaction / client data Handle 
* @param width             : stream width
* @param height            : stream height
* @param frameRate         : encode frame rate
* @param bitrate           : encoder bit rate
* @param numFrames         : encoded number of frames
* @param displayId         : display instance id
*
*  @return      
*
*
*/
/* ========================================================================== */

void IL_ClientInit (IL_Client **pAppData, int width, int height, int frameRate,
                    int bitRate, char *codec)
{
  int i;
  IL_Client *pAppDataPtr;
  IL_CLIENT_INPORT_PARAMS *inPortParamsPtr;
  IL_CLIENT_OUTPORT_PARAMS *outPortParamsPtr;

  /* Allocating data structure for IL client structure / buffer management */

  pAppDataPtr = (IL_Client *) malloc (sizeof (IL_Client));
  memset (pAppDataPtr, 0x0, sizeof (IL_Client));

  /* update the user provided parameters */
  pAppDataPtr->nFrameRate   = frameRate;
  pAppDataPtr->nBitRate     = bitRate;
  /* pAppDataPtr->nEncodedFrms = numFrames; */
  
  pAppDataPtr->nHeight      = height;
  pAppDataPtr->nWidth       =  width; 

  if (strcmp (codec, "h264") == 0)
  {
    pAppDataPtr->eCompressionFormat = OMX_VIDEO_CodingAVC;
  }
  else if (strcmp (codec, "mpeg4") == 0)
  {
    pAppDataPtr->eCompressionFormat = OMX_VIDEO_CodingMPEG4;
  }
  else if (strcmp (codec, "h263") == 0)
  {
    pAppDataPtr->eCompressionFormat = OMX_VIDEO_CodingH263;
  }
  
  /* alloacte data structure for each component used in this IL Client */
  pAppDataPtr->encILComp =
    (IL_CLIENT_COMP_PRIVATE *) malloc (sizeof (IL_CLIENT_COMP_PRIVATE));
  memset (pAppDataPtr->encILComp, 0x0, sizeof (IL_CLIENT_COMP_PRIVATE));

  /* these semaphores are used for tracking the callbacks received from
     component */
  pAppDataPtr->encILComp->eos = malloc (sizeof (semp_t));
  semp_init (pAppDataPtr->encILComp->eos, 0);

  pAppDataPtr->encILComp->done_sem = malloc (sizeof (semp_t));
  semp_init (pAppDataPtr->encILComp->done_sem, 0);

  pAppDataPtr->encILComp->port_sem = malloc (sizeof (semp_t));
  semp_init (pAppDataPtr->encILComp->port_sem, 0);

  pAppDataPtr->encILComp->numInport = 1;
  pAppDataPtr->encILComp->numOutport = 1;
  pAppDataPtr->encILComp->startOutportIndex = 1;

  pAppDataPtr->encILComp->inPortParams =
    malloc (sizeof (IL_CLIENT_INPORT_PARAMS) *
            pAppDataPtr->encILComp->numInport);
  memset (pAppDataPtr->encILComp->inPortParams, 0x0,
          sizeof (IL_CLIENT_INPORT_PARAMS));

  pAppDataPtr->encILComp->outPortParams =
    malloc (sizeof (IL_CLIENT_INPORT_PARAMS) *
            pAppDataPtr->encILComp->numOutport);
  memset (pAppDataPtr->encILComp->outPortParams, 0x0,
          sizeof (IL_CLIENT_OUTPORT_PARAMS));

  for (i = 0; i < pAppDataPtr->encILComp->numInport; i++)
  {
    inPortParamsPtr = pAppDataPtr->encILComp->inPortParams + i;
    inPortParamsPtr->nBufferCountActual = IL_CLIENT_ENC_INPUT_BUFFER_COUNT;
    /* input buffers size for yuv buffers, format is YUV420 hence 3/2 */
    inPortParamsPtr->nBufferSize =
      (pAppDataPtr->nHeight * pAppDataPtr->nWidth * 3) >> 1;
    /* this pipe is used for taking buffers from file read thread; in this
       example, file read is not used */
    pipe ((int *) inPortParamsPtr->ipBufPipe);
  }
  for (i = 0; i < pAppDataPtr->encILComp->numOutport; i++)
  {
    outPortParamsPtr = pAppDataPtr->encILComp->outPortParams + i;
    outPortParamsPtr->nBufferCountActual = IL_CLIENT_ENC_OUTPUT_BUFFER_COUNT;
    /* this size could be smaller than this value */
    outPortParamsPtr->nBufferSize =
      (pAppDataPtr->nHeight * pAppDataPtr->nWidth * 3) >> 1;

    /* This pipe is used if output is directed to file write thread, */
    pipe ((int *) outPortParamsPtr->opBufPipe);
  }
  /* each componet will have local pipe to take bufffers from other component
     or its own consumed buffer, so that it can be passed to other connected
     components */
  pipe ((int *) pAppDataPtr->encILComp->localPipe);

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

void IL_ClientDeInit (IL_Client * pAppData)
{
  int i;
  IL_CLIENT_INPORT_PARAMS *inPortParamsPtr;
  IL_CLIENT_OUTPORT_PARAMS *outPortParamsPtr;


  close ((int) pAppData->encILComp->localPipe);

  for (i = 0; i < pAppData->encILComp->numInport; i++)
  {
    inPortParamsPtr = pAppData->encILComp->inPortParams + i;
    /* This pipe is used if output is directed to file write thread, in this
       example, file read is not used */
    close ((int) inPortParamsPtr->ipBufPipe);
  }
  for (i = 0; i < pAppData->encILComp->numOutport; i++)
  {
    outPortParamsPtr = pAppData->encILComp->outPortParams + i;
    close ((int) outPortParamsPtr->opBufPipe);
  }

  free (pAppData->encILComp->inPortParams);

  free (pAppData->encILComp->outPortParams);

  /* these semaphores are used for tracking the callbacks received from
     component */
  semp_deinit (pAppData->encILComp->eos);
  free(pAppData->encILComp->eos);

  semp_deinit (pAppData->encILComp->done_sem);
  free(pAppData->encILComp->done_sem);

  semp_deinit (pAppData->encILComp->port_sem);

  free(pAppData->encILComp->port_sem);

  free (pAppData->encILComp);

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
  }

  return errorString;
}

/* ========================================================================== */
/**
* IL_ClientEncUseInitialOutputResources() :  This function gives initially all
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

OMX_ERRORTYPE IL_ClientEncUseInitialOutputResources (IL_CLIENT_COMP_PRIVATE 
                                                       *thisComp)
{
  OMX_ERRORTYPE err = OMX_ErrorNone;
  unsigned int i = 0;

  for (i = 0; i < thisComp->outPortParams->nBufferCountActual; i++)
  {
    /* Pass the output buffer to the component */
    err = OMX_FillThisBuffer (thisComp->handle,
                              thisComp->outPortParams->pOutBuff[i]);
  }

  return err;
}

/* ========================================================================== */
/**
* IL_ClientDecUseInitialInputResources() : This function gives initially all
*                                          input buffers to decoder component.
*                                          after consuming decoder would keep
*                                          in ipbufpipe for file read thread. 
*
* @param pAppdata   : application data structure
*
*  @return      
*  String conversion of the OMX_ERRORTYPE
*
*/
/* ========================================================================== */

OMX_ERRORTYPE IL_ClientEncUseInitialInputResources (IL_Client *pAppdata)
{

  OMX_ERRORTYPE err = OMX_ErrorNone;
  unsigned int i = 0;
  int frameSize = 0;
  IL_CLIENT_COMP_PRIVATE *encILComp = NULL;
  encILComp = ((IL_Client *) pAppdata)->encILComp;

  /* Give input buffers to component which is limited by no of input buffers
     available. Rest of the data will be read on the callback from input data
     read thread */
  for (i = 0; i < encILComp->inPortParams->nBufferCountActual; i++)
  {
    frameSize = (pAppdata->nHeight * pAppdata->nWidth * 3) >> 1;
    /* Get the size of one frame at a time */
    frameSize = fread (encILComp->inPortParams->pInBuff[i]->pBuffer, 1, frameSize, pAppdata->fIn);

    /* Exit the loop if no data available */
    if (!frameSize)
    {
      break;
    }

    encILComp->inPortParams->pInBuff[i]->nFilledLen = frameSize;
    encILComp->inPortParams->pInBuff[i]->nOffset = 0;
    encILComp->inPortParams->pInBuff[i]->nAllocLen = frameSize;
    encILComp->inPortParams->pInBuff[i]->nInputPortIndex = 0;

    /* Pass the input buffer to the component */

    err = OMX_EmptyThisBuffer (encILComp->handle,
                               encILComp->inPortParams->pInBuff[i]);

  }
  return err;
}

/* ========================================================================== */
/**
* IL_ClientSetEncodeParams() : Function to fill the port definition 
* structures and call the Set_Parameter function on to the Encode
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

OMX_ERRORTYPE IL_ClientSetEncodeParams (IL_Client *pAppData)
{
  OMX_ERRORTYPE eError = OMX_ErrorUndefined;
  OMX_HANDLETYPE pHandle = NULL;
  OMX_VIDEO_PARAM_PROFILELEVELTYPE tProfileLevel;
  OMX_VIDEO_PARAM_ENCODER_PRESETTYPE tEncoderPreset;
  OMX_VIDEO_PARAM_BITRATETYPE tVidEncBitRate;
  //OMX_VIDEO_PARAM_PORTFORMATTYPE tVideoParams;
  OMX_PARAM_PORTDEFINITIONTYPE tPortDef;
  OMX_VIDEO_CONFIG_DYNAMICPARAMS tDynParams;
  OMX_VIDEO_PARAM_STATICPARAMS   tStaticParam;
  //OMX_VIDEO_PARAM_FRAMEDATACONTENTTYPE  tFrameType;
  
  pHandle = pAppData->pEncHandle;

  /* Number of frames to be encoded, not used by defaualt */
  pAppData->encILComp->numFrames = pAppData->nEncodedFrms;


  OMX_INIT_PARAM (&tPortDef);
  /* Get the Number of Ports */

  tPortDef.nPortIndex = OMX_VIDENC_INPUT_PORT;
  eError = OMX_GetParameter (pHandle, OMX_IndexParamPortDefinition, &tPortDef);
  /* set the actual number of buffers required */
  tPortDef.nBufferCountActual = IL_CLIENT_ENC_INPUT_BUFFER_COUNT;
  /* set the video format settings */
  tPortDef.format.video.nFrameWidth = pAppData->nWidth;
  tPortDef.format.video.nStride = pAppData->nWidth;
  tPortDef.format.video.nFrameHeight = pAppData->nHeight;
  tPortDef.format.video.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
  /* settings for OMX_IndexParamVideoPortFormat */
  tPortDef.nBufferSize = (pAppData->nWidth * pAppData->nHeight * 3) >> 1;
  eError = OMX_SetParameter (pHandle, OMX_IndexParamPortDefinition, &tPortDef);
  if (eError != OMX_ErrorNone)
  {
    ERROR ("failed to set Encode OMX_IndexParamPortDefinition for input \n");
  }

  OMX_INIT_PARAM (&tPortDef);

  tPortDef.nPortIndex = OMX_VIDENC_OUTPUT_PORT;
  eError = OMX_GetParameter (pHandle, OMX_IndexParamPortDefinition, &tPortDef);
  /* settings for OMX_IndexParamPortDefinition */
  /* set the actual number of buffers required */
  tPortDef.nBufferCountActual = IL_CLIENT_ENC_OUTPUT_BUFFER_COUNT;
  tPortDef.format.video.nFrameWidth = pAppData->nWidth;
  tPortDef.format.video.nFrameHeight = pAppData->nHeight;
  tPortDef.format.video.eCompressionFormat = pAppData->eCompressionFormat;
  tPortDef.format.video.xFramerate = (pAppData->nFrameRate << 16);
  tPortDef.format.video.nBitrate = pAppData->nBitRate;
  /* settings for OMX_IndexParamVideoPortFormat */

  eError = OMX_SetParameter (pHandle, OMX_IndexParamPortDefinition, &tPortDef);
  if (eError != OMX_ErrorNone)
  {
    ERROR ("failed to set Encode OMX_IndexParamPortDefinition for output \n");
  }

  /* For changing bit rate following index can be used */
  OMX_INIT_PARAM (&tVidEncBitRate);

  tVidEncBitRate.nPortIndex = OMX_VIDENC_OUTPUT_PORT;
  eError = OMX_GetParameter (pHandle, OMX_IndexParamVideoBitrate,
                             &tVidEncBitRate);

  tVidEncBitRate.eControlRate = OMX_Video_ControlRateVariable;
  tVidEncBitRate.nTargetBitrate = pAppData->nBitRate;
  eError = OMX_SetParameter (pHandle, OMX_IndexParamVideoBitrate,
                             &tVidEncBitRate);

  if (eError != OMX_ErrorNone)
  {
    ERROR ("failed to set Encode bitrate \n");
  }
  /* Set the profile and level for H264 */
  OMX_INIT_PARAM (&tProfileLevel);
  tProfileLevel.nPortIndex = OMX_VIDENC_OUTPUT_PORT;

  eError = OMX_GetParameter (pHandle, OMX_IndexParamVideoProfileLevelCurrent,
                             &tProfileLevel);

  /* set as profile / level */
  if(pAppData->eCompressionFormat == OMX_VIDEO_CodingAVC) { 
  tProfileLevel.eProfile = OMX_VIDEO_AVCProfileBaseline;
  tProfileLevel.eLevel = OMX_VIDEO_AVCLevel42;
  }
  else if (pAppData->eCompressionFormat == OMX_VIDEO_CodingMPEG4) {
  tProfileLevel.eProfile = OMX_VIDEO_MPEG4ProfileSimple;
  tProfileLevel.eLevel = OMX_VIDEO_MPEG4Level5;
   }
  else if (pAppData->eCompressionFormat == OMX_VIDEO_CodingH263) {
  tProfileLevel.eProfile = OMX_VIDEO_H263ProfileBaseline;
  tProfileLevel.eLevel = OMX_VIDEO_H263Level40;
   }
  

  eError = OMX_SetParameter (pHandle, OMX_IndexParamVideoProfileLevelCurrent,
                             &tProfileLevel);
  if (eError != OMX_ErrorNone)
    ERROR ("failed to set encoder pfofile \n");
  /* before creating use set_parameters, for run-time change use set_config
     all codec supported parameters can be set using this index       */
  
  /* example for h264 parameters settings */
     
  if(pAppData->eCompressionFormat == OMX_VIDEO_CodingAVC) {   
  
  /* Encoder Preset settings */
  OMX_INIT_PARAM (&tEncoderPreset);
  tEncoderPreset.nPortIndex = OMX_VIDENC_OUTPUT_PORT;
  eError = OMX_GetParameter (pHandle, OMX_TI_IndexParamVideoEncoderPreset,
                             &tEncoderPreset);

  tEncoderPreset.eEncodingModePreset =  OMX_Video_Enc_Default; 
  tEncoderPreset.eRateControlPreset = OMX_Video_RC_None;

  eError = OMX_SetParameter (pHandle, OMX_TI_IndexParamVideoEncoderPreset,
                             &tEncoderPreset);
  if (eError != OMX_ErrorNone)
  {
    ERROR ("failed to Encoder Preset \n");
  }
  
  OMX_INIT_PARAM (&tDynParams);

  tDynParams.nPortIndex = OMX_VIDENC_OUTPUT_PORT;
  
  eError = OMX_GetParameter (pHandle, OMX_TI_IndexParamVideoDynamicParams,
                             &tDynParams);
  
  /* setting I frame interval */
  tDynParams.videoDynamicParams.h264EncDynamicParams.videnc2DynamicParams.targetFrameRate = pAppData->nFrameRate * 1000;
  tDynParams.videoDynamicParams.h264EncDynamicParams.videnc2DynamicParams.targetBitRate = pAppData->nBitRate;
  tDynParams.videoDynamicParams.h264EncDynamicParams.rateControlParams.rateControlParamsPreset = IH264_RATECONTROLPARAMS_DEFAULT;
  tDynParams.videoDynamicParams.h264EncDynamicParams.rateControlParams.HRDBufferSize = pAppData->nBitRate * 2;
  
  tDynParams.videoDynamicParams.h264EncDynamicParams.videnc2DynamicParams.intraFrameInterval = 90;
                         
  eError = OMX_SetParameter (pHandle, OMX_TI_IndexParamVideoDynamicParams,
                             &tDynParams);

  OMX_INIT_PARAM (&tStaticParam);

  tStaticParam.nPortIndex = OMX_VIDENC_OUTPUT_PORT;
  
  eError = OMX_GetParameter (pHandle, OMX_TI_IndexParamVideoStaticParams,
                             &tStaticParam);
  
  tStaticParam.videoStaticParams.h264EncStaticParams.rateControlParams.HRDBufferSize = pAppData->nBitRate * 2;
  tStaticParam.videoStaticParams.h264EncStaticParams.videnc2Params.encodingPreset = XDM_DEFAULT;
  tStaticParam.videoStaticParams.h264EncStaticParams.videnc2Params.rateControlPreset = IVIDEO_LOW_DELAY;
  
                        
  eError = OMX_SetParameter (pHandle, OMX_TI_IndexParamVideoStaticParams,
                             &tStaticParam);
  }
  if((pAppData->eCompressionFormat == OMX_VIDEO_CodingMPEG4) || 
     (pAppData->eCompressionFormat == OMX_VIDEO_CodingH263))
  {
    OMX_INIT_PARAM(&tStaticParam);
    tStaticParam.nPortIndex = OMX_VIDENC_OUTPUT_PORT;

    eError =
        OMX_GetParameter(pHandle, OMX_TI_IndexParamVideoStaticParams,
                         &tStaticParam);
    if (eError != OMX_ErrorNone)
    {
      ERROR ("failed to Encoder OMX_GetParameter StaticParams \n");
    }

    tStaticParam.videoStaticParams.mpeg4EncStaticParams.videnc2Params.encodingPreset
                                                     = XDM_USER_DEFINED;
    tStaticParam.videoStaticParams.mpeg4EncStaticParams.vopTimeIncrementResolution
                                                     = pAppData->nFrameRate;

    eError =
        OMX_SetParameter(pHandle, OMX_TI_IndexParamVideoStaticParams,
                         &tStaticParam);
    if (eError != OMX_ErrorNone)
    {
      ERROR ("failed to Encoder OMX_SetParameter StaticParams \n");
    }
  }

  return eError;
}

/* Nothing beyond this point */

