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
#include <xdc/std.h>
#include <memory.h>
#include <getopt.h>
#include <ti/syslink/utils/Cache.h>


/*-------------------------program files -------------------------------------*/
#include "ti/omx/interfaces/openMaxv11/OMX_Core.h"
#include "ti/omx/interfaces/openMaxv11/OMX_Component.h"
#include "OMX_TI_Common.h"
#include "ilclient.h"
#include "ilclient_utils.h"
#include <omx_vdec.h>
#include <omx_vfpc.h>
#include <omx_vfdc.h>
#include <omx_ctrl.h>
#include <OMX_TI_Index.h>

/*--------------------------- defines ----------------------------------------*/
/* Align address "a" at "b" boundary */
#define UTIL_ALIGN(a,b)  ((((uint32_t)(a)) + (b)-1) & (~((uint32_t)((b)-1))))

#define HD_WIDTH       (1920)
#define HD_HEIGHT      (1080)

#define SD_WIDTH       (720)
#define SD_HEIGHT      (480)

void usage (IL_ARGS *argsp)
{
  printf ("display -d <0/1/2>\n"
          "-d | --display_id      0 - for on-chip HDMI, 1 for LCD, 2 for SD \n");
  printf(" example : ./display_a8host_debug.xv5T -d 2 \n");
  printf(" It would display color bar; press CTRL+C to exit \n");      
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
  const char shortOptions[] = "d:";
  const struct option longOptions[] = {
      {"display_id",      required_argument, NULL, ArgID_DISPLAYID        },
      {0, 0, 0, 0}
  };

  int index;
  int display_id = 0;
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
      case ArgID_DISPLAYID:
      case 'd':
        argsp->display_id = atoi (optarg);
        display_id = 1;
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

  if (!display_id)
  {
    usage (argsp);
    exit (1);
  }

  printf ("display_id: %d\n", argsp->display_id);

}

/* ========================================================================== */
/**
* IL_ClientInit() : This function is to allocate and initialize the application
*                   data structure. It is just to maintain application control.
*
* @param pAppData          : appliaction / client data Handle 
* @param displayId         : display instance id
*
*  @return      
*
*
*/
/* ========================================================================== */

void IL_ClientInit (IL_Client **pAppData, int displayId)
{
  int i;
  IL_Client *pAppDataPtr;
  IL_CLIENT_INPORT_PARAMS *inPortParamsPtr;
  IL_CLIENT_OUTPORT_PARAMS *outPortParamsPtr;

  /* Allocating data structure for IL client structure / buffer management */

  pAppDataPtr = (IL_Client *) malloc (sizeof (IL_Client));
  memset (pAppDataPtr, 0x0, sizeof (IL_Client));

  /* update the user provided parameters */
  pAppDataPtr->displayId = displayId;
  
 
  /* alloacte data structure for each component used in this IL Cleint */
  pAppDataPtr->disILComp =
    (IL_CLIENT_COMP_PRIVATE *) malloc (sizeof (IL_CLIENT_COMP_PRIVATE));
  memset (pAppDataPtr->disILComp, 0x0, sizeof (IL_CLIENT_COMP_PRIVATE));

  /* these semaphores are used for tracking the callbacks received from
     component */
  pAppDataPtr->disILComp->eos = malloc (sizeof (semp_t));
  semp_init (pAppDataPtr->disILComp->eos, 0);

  pAppDataPtr->disILComp->done_sem = malloc (sizeof (semp_t));
  semp_init (pAppDataPtr->disILComp->done_sem, 0);

  pAppDataPtr->disILComp->port_sem = malloc (sizeof (semp_t));
  semp_init (pAppDataPtr->disILComp->port_sem, 0);

  pAppDataPtr->disILComp->numInport = 1;

  /* display does not has o/pports */
  pAppDataPtr->disILComp->numOutport = 0;
  pAppDataPtr->disILComp->startOutportIndex = 0;

  pAppDataPtr->disILComp->inPortParams =
    malloc (sizeof (IL_CLIENT_INPORT_PARAMS) *
            pAppDataPtr->disILComp->numInport);

  memset (pAppDataPtr->disILComp->inPortParams, 0x0,
          sizeof (IL_CLIENT_INPORT_PARAMS));


  for (i = 0; i < pAppDataPtr->disILComp->numInport; i++)
  {
    inPortParamsPtr = pAppDataPtr->disILComp->inPortParams + i;
    inPortParamsPtr->nBufferCountActual = IL_CLIENT_DISPLAY_INPUT_BUFFER_COUNT;
    if (0 == pAppDataPtr->displayId)
    { 
      /* If On-Chip HDMI is chosen, configure the buffer size to that of the
      /* decoded output frame */
      inPortParamsPtr->nBufferSize = HD_HEIGHT * HD_WIDTH * 2;
    }
    else if (1 == pAppDataPtr->displayId)
    {
      /* If LCD is chosen, configure the buffer size to that of the
      /* LCD frame, so that the entire decoded output can span the LCD screen */
      inPortParamsPtr->nBufferSize = LCD_HEIGHT * LCD_WIDTH * 2;      
    }
    else
    {
      /* If SD display is used, the buffer will be a 420SP buffer, so buffer 
       * size will be height*width*1.5 */
      inPortParamsPtr->nBufferSize = (SD_HEIGHT * SD_WIDTH * 3) >> 1;
    }

    /* this pipe will not be used in this application, as scalar does not read
       / write into file */
    pipe (inPortParamsPtr->ipBufPipe);
  }

  /* each componet will have local pipe to take bufffes from other component or 
     its own consumed buffer, so that it can be passed to other conected
     components */
  pipe (pAppDataPtr->disILComp->localPipe);

  /* populate the pointer for allocated data structure */
  *pAppData = pAppDataPtr;
}

/* ========================================================================== */
/**
* IL_ClientDeInit() : This function is to deinitialize the application
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
  int i;
  IL_CLIENT_INPORT_PARAMS *inPortParamsPtr;
  IL_CLIENT_OUTPORT_PARAMS *outPortParamsPtr;

  close (pAppData->disILComp->localPipe);

  for (i = 0; i < pAppData->disILComp->numInport; i++)
  {
    inPortParamsPtr = pAppData->disILComp->inPortParams + i;
    /* this pipe will not be used in this application, as scalar does not read
       / write into file */
    close (inPortParamsPtr->ipBufPipe);
  }

 
  free (pAppData->disILComp->inPortParams);

  semp_deinit (pAppData->disILComp->eos);
  free (pAppData->disILComp->eos);

  semp_deinit (pAppData->disILComp->done_sem);
  free (pAppData->disILComp->done_sem);

  semp_deinit (pAppData->disILComp->port_sem);
  free (pAppData->disILComp->port_sem);

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

/* color luma/chroma values */
static unsigned long yuyv[8] = {
	0x80eb80eb,
	0x801e801e,
	0xf0515a51,
	0x22913691,
	0x6e29f029,
	0x92d210d2,
	0x10aaa6aa,
	0xde6aca6a,
};

static unsigned long cbcr[8] = {
	0x80808080,
	0x80808080,
	0x5af05af0,
 0x36223622,
	0xf06ef06e,
 0x10921092,
	0xa610a610,
 0xcadecade
};

static unsigned long yy[8] = {
	0xebebebeb,
	0x1e1e1e1e,
	0x51515151,
	0x91919191,
	0x29292929,
	0xd2d2d2d2,
	0xaaaaaaaa,
	0x6a6a6a6a
};

/* This would fill the buffer with color bars */

void color_bar(char *addr, int width, int height, int id)
{
	unsigned long *ptr = (unsigned long *)addr;
	int i, j, k;
 /* 420 o/p this is display id for SD */
 if (id == 2) {
   /* Fill Luma values */
	  for(i = 0 ; i < 8 ; i++) {
		  for(j = 0 ; j < (height/8) ; j++) {
			  for(k = 0 ; k < (width >> 2) ; k++) {
				  ptr[k] = yy[i];
			  }
			  ptr = ptr + (width >> 2);
		  }
	  }
   /* Fill Chroma values */
	  for(i = 0 ; i < 8 ; i++) {
		  for(j = 0 ; j < (height/16) ; j++) {
			  for(k = 0 ; k < (width >> 2) ; k++) {
				  ptr[k] = cbcr[i];
			  }
			  ptr = ptr + (width >> 2);
		  }
	  }
  }
 /* 422 output */ 
  else {
	  for(i = 0 ; i < 8 ; i++) {
		  for(j = 0 ; j < (height/8) ; j++) {
			  for(k = 0 ; k < (width >> 1) ; k++) {
				  ptr[k] = yuyv[i];
			  }
			  ptr = ptr + (width >> 1);
			  if((unsigned int)ptr >=
					  ((unsigned int)addr + width * height * 2))
				  ptr = (unsigned long *)addr;
		  }
	  }
  }
  
}

/* ========================================================================== */
/**
* IL_ClientFillData() : Function to read a  frame and copy it to 
*                                decoder input buffer.
*
* @param pAppData   : application data structure
* @param pbuf       : OMX buffer header, it has pointer to actual buffer
*
*  @return      
*  String conversion of the OMX_ERRORTYPE
*
*/
/* ========================================================================== */

unsigned int IL_ClientFillData (IL_Client *pAppData,
                                         OMX_BUFFERHEADERTYPE *pBuf)
{
  unsigned int dataRead = 0, width, height;
  int frameSize = 0;
  IL_CLIENT_COMP_PRIVATE *disILComp = NULL;
  disILComp = ((IL_Client *) pAppData)->disILComp;
  
  /*Read the Frame from the file into data buffer associated with the OMX Buf 
   * header at hand*/
  
  if (0 == pAppData->displayId)  
  {
    /* Setting frame size for HD sized buffer*/
    frameSize = HD_WIDTH * HD_HEIGHT * 2;
    width  =  HD_WIDTH;
    height =  HD_HEIGHT;
  }
  else if (1 == pAppData->displayId)
  {
    /* Setting frame size for LCD sized buffer*/    
    frameSize = LCD_WIDTH * LCD_HEIGHT * 2;
    width  =  LCD_WIDTH;
    height =  LCD_HEIGHT;
    
  }
  else
  {
    /* Setting frame size for SD sized buffer*/    
    frameSize = (SD_WIDTH * SD_HEIGHT * 3) >> 1;
    width  =  SD_WIDTH;
    height =  SD_HEIGHT;
    
  }

  /* Fill the buffer with color bar */
  //printf( " filling buffer \n");
  color_bar(pBuf->pBuffer, width, height, pAppData->displayId );
  //printf( " filled buffer \n");

  /* Update the buffer header with buffer filled length and alloc length */
  pBuf->nFilledLen = frameSize;

  return frameSize;
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
* IL_ClientDisUseInitialInputResources() : This function gives initially all
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

OMX_ERRORTYPE IL_ClientDisUseInitialInputResources (IL_Client *pAppdata)
{

  OMX_ERRORTYPE err = OMX_ErrorNone;
  unsigned int i = 0;
  int frameSize = 0;
  IL_CLIENT_COMP_PRIVATE *disILComp = NULL;
  disILComp = ((IL_Client *) pAppdata)->disILComp;

  /* Give input buffers to component which is limited by no of input buffers
     available. Rest of the data will be read on the callback from input data
     read thread */
  for (i = 0; i < disILComp->inPortParams->nBufferCountActual; i++)
  {

    /* Get the size of one frame at a time */
    frameSize = IL_ClientFillData (pAppdata, disILComp->inPortParams->pInBuff[i]);

    /* Exit the loop if no data available */
    if (!frameSize)
    {
      break;
    }

    disILComp->inPortParams->pInBuff[i]->nFilledLen = frameSize;
    disILComp->inPortParams->pInBuff[i]->nOffset = 0;
    disILComp->inPortParams->pInBuff[i]->nAllocLen = frameSize;
    disILComp->inPortParams->pInBuff[i]->nInputPortIndex = 
                                                OMX_VFDC_INPUT_PORT_START_INDEX;

    /* Pass the input buffer to the component */

    err = OMX_EmptyThisBuffer (disILComp->handle,
                               disILComp->inPortParams->pInBuff[i]);

  }
  return err;
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

OMX_ERRORTYPE
  IL_ClientUseInitialOutputResources (IL_CLIENT_COMP_PRIVATE *thisComp)

{
  OMX_ERRORTYPE err = OMX_ErrorNone;
  unsigned int i = 0, j;
  IL_CLIENT_OUTPORT_PARAMS *outPortParamPtr = NULL;
  OMX_PARAM_PORTDEFINITIONTYPE param;

  memset (&param, 0, sizeof (param));

  OMX_INIT_PARAM (&param);

  /* Give output buffers to component which is limited by no of output buffers
     available. Rest of the data will be written on the callback from output
     data write thread */
  for (j = 0; j < thisComp->numOutport; j++)
  {
    param.nPortIndex = j + thisComp->startOutportIndex;

    OMX_GetParameter (thisComp->handle, OMX_IndexParamPortDefinition, &param);

    outPortParamPtr = thisComp->outPortParams + j;

    if (OMX_TRUE == param.bEnabled)
    {
      if (outPortParamPtr->connInfo.remotePipe != NULL)
      {

        for (i = 0; i < thisComp->outPortParams->nBufferCountActual; i++)
        {
          /* Pass the output buffer to the component */
          err =
            OMX_FillThisBuffer (thisComp->handle, outPortParamPtr->pOutBuff[i]);

        } /* for (i) */
      } /* if (outPortParamPtr->...) */
    } /* if (OMX_TRUE...) */
  } /* for (j) */

  return err;
}

/* ========================================================================== */
/**
* IL_ClientSetDisplayParams() : Function to fill the port definition 
* structures and call the Set_Parameter function on to the display
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

OMX_ERRORTYPE IL_ClientSetDisplayParams (IL_Client *pAppData)
{
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  OMX_PARAM_BUFFER_MEMORYTYPE memTypeCfg;
  OMX_PARAM_PORTDEFINITIONTYPE paramPort;
  OMX_PARAM_VFPC_NUMCHANNELPERHANDLE sNumChPerHandle;
  OMX_PARAM_VFDC_DRIVERINSTID driverId;
  OMX_PARAM_VFDC_CREATEMOSAICLAYOUT mosaicLayout;
  OMX_CONFIG_VFDC_MOSAICLAYOUT_PORT2WINMAP port2Winmap;
  OMX_PARAM_DC_CUSTOM_MODE_INFO customModeInfo;
     
  OMX_INIT_PARAM (&paramPort);

  /* set input height/width and color format */
  paramPort.nPortIndex = OMX_VFDC_INPUT_PORT_START_INDEX;
  OMX_GetParameter (pAppData->pDisHandle, OMX_IndexParamPortDefinition,
                    &paramPort);
  paramPort.nPortIndex = OMX_VFDC_INPUT_PORT_START_INDEX;
 
  if (0 == pAppData->displayId)
  {
    /* For the case of On-chip HDMI, setting the display input buffer size params
    */
    paramPort.format.video.nFrameWidth = HD_WIDTH;
    paramPort.format.video.nFrameHeight = HD_HEIGHT;
    paramPort.format.video.nStride = HD_WIDTH * 2;
  }
  else if (1 == pAppData->displayId)
  {
    /* For the case of LCD display, setting the display input buffer size params
    */
    paramPort.format.video.nFrameWidth = LCD_WIDTH;
    paramPort.format.video.nFrameHeight = LCD_HEIGHT;
    paramPort.format.video.nStride = LCD_WIDTH * 2;  
  }
  else
  {
    /* For the case of SD display, setting the display input buffer size 
     * params*/
    paramPort.format.video.nFrameWidth = SD_WIDTH;
    paramPort.format.video.nFrameHeight = SD_HEIGHT;
    paramPort.format.video.nStride = SD_WIDTH;    
     
  }
  
  paramPort.nBufferCountActual = IL_CLIENT_DISPLAY_INPUT_BUFFER_COUNT;
  paramPort.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
  
  paramPort.format.video.eColorFormat = OMX_COLOR_FormatYCbYCr;

  paramPort.nBufferSize = paramPort.format.video.nStride * 
                          paramPort.format.video.nFrameHeight;
  if (2 == pAppData->displayId)
  {
    /* The SD display accepts 420SP buffers at input port*/
    paramPort.format.video.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
    paramPort.nBufferSize = ((paramPort.format.video.nStride * 
                             paramPort.format.video.nFrameHeight) * 3 ) >> 1;
  }
  printf ("Buffer Size computed: %d\n", paramPort.nBufferSize);
  printf ("set input port params (width = %d, height = %d) \n",
          paramPort.format.video.nFrameWidth, 
          paramPort.format.video.nFrameHeight);
  OMX_SetParameter (pAppData->pDisHandle, OMX_IndexParamPortDefinition,
                    &paramPort);
  /* --------------------------------------------------------------------------*
     Supported display IDs by VFDC and DC are below The names will be renamed in
     future releases as some of the driver names & interfaces will be changed in
     future @ param OMX_VIDEO_DISPLAY_ID_HD0: 422P On-chip HDMI @ param
     OMX_VIDEO_DISPLAY_ID_HD1: 422P HDDAC component output @ param
     OMX_VIDEO_DISPLAY_ID_SD0: 420T/422T SD display (NTSC): Not supported yet.
     -------------------------------------------------------------------------- */

  /* set the parameter to the display component to 1080P @60 or LCD or 
   * SD Display mode */
  OMX_INIT_PARAM (&driverId);
  if (0 == pAppData->displayId)
  {
    /* Configured to use on-chip HDMI */
    driverId.nDrvInstID = OMX_VIDEO_DISPLAY_ID_HD0;
    driverId.eDispVencMode = OMX_DC_MODE_1080P_60;
  } 
  else if (1 == pAppData->displayId)
  {
    /* Configured to use LCD Display */
    driverId.nDrvInstID = OMX_VIDEO_DISPLAY_ID_HD1;
    driverId.eDispVencMode = OMX_DC_MODE_CUSTOM;
  } 
  else if (2 == pAppData->displayId)
  {
    /* Configured to use SD Display */
    driverId.nDrvInstID = OMX_VIDEO_DISPLAY_ID_SD0;
    driverId.eDispVencMode = OMX_DC_MODE_NTSC; 
  }
  else
  {
    ERROR ("Incorrect Display Id configured\n");
  }
  
  eError =
    OMX_SetParameter (pAppData->pDisHandle,
                      (OMX_INDEXTYPE) OMX_TI_IndexParamVFDCDriverInstId,
                      &driverId);
  if (eError != OMX_ErrorNone)
  {
    ERROR ("failed to set driver mode to 1080P@60\n");
  }
  /* set the parameter to the display controller component to 1080P @60 mode
     or LCD display  or SD display depending on Display Id chosen  */
  OMX_INIT_PARAM (&driverId);
  if (0 == pAppData->displayId)
  {
    /* Configured to use on-chip HDMI */
    driverId.nDrvInstID = OMX_VIDEO_DISPLAY_ID_HD0;
    driverId.eDispVencMode = OMX_DC_MODE_1080P_60;
  } 
  else if (1 == pAppData->displayId)
  {
    /* Configured to use LCD Display */
    driverId.nDrvInstID = OMX_VIDEO_DISPLAY_ID_HD1;
    driverId.eDispVencMode = OMX_DC_MODE_CUSTOM;
  } 
  else if (2 == pAppData->displayId)
  {
    /* Configured to use SD Display */
    driverId.nDrvInstID = OMX_VIDEO_DISPLAY_ID_SD0;
    driverId.eDispVencMode = OMX_DC_MODE_NTSC;    
  }
  else
  {
    ERROR ("Incorrect Display Id configured\n");
  }

  eError =
    OMX_SetParameter (pAppData->pctrlHandle,
                      (OMX_INDEXTYPE) OMX_TI_IndexParamVFDCDriverInstId,
                      &driverId);
  if (eError != OMX_ErrorNone)
    ERROR ("failed to set driver mode to 1080P@60\n");
   
  if (1 == pAppData->displayId) {
    /* For LCD display configuration, custom mode parameters need to be 
    /* configured */
    OMX_INIT_PARAM (&customModeInfo);
    
    customModeInfo.width = LCD_WIDTH;
    customModeInfo.height = LCD_HEIGHT;
    customModeInfo.scanFormat = OMX_SF_PROGRESSIVE;
    customModeInfo.pixelClock = LCD_PIXEL_CLOCK;
    customModeInfo.hFrontPorch = LCD_H_FRONT_PORCH;
    customModeInfo.hBackPorch = LCD_H_BACK_PORCH;
    customModeInfo.hSyncLen = LCD_H_SYNC_LENGTH;
    customModeInfo.vFrontPorch = LCD_V_FRONT_PORCH;
    customModeInfo.vBackPorch = LCD_V_BACK_PORCH;
    customModeInfo.vSyncLen = LCD_V_SYNC_LENGTH;
    /*Configure Display component and Display controller with these parameters*/
    eError = OMX_SetParameter (pAppData->pDisHandle, (OMX_INDEXTYPE)
                               OMX_TI_IndexParamVFDCCustomModeInfo,
                               &customModeInfo);    
    if (eError != OMX_ErrorNone)
      ERROR ("failed to set custom mode setting for Display component\n");

    eError = OMX_SetParameter (pAppData->pctrlHandle, (OMX_INDEXTYPE)
                               OMX_TI_IndexParamVFDCCustomModeInfo,
                               &customModeInfo);    
    if (eError != OMX_ErrorNone)
      ERROR ("failed to set custom mode setting for Display Controller \
             component\n"); 
      
  }
  /* set mosaic layout info */

  OMX_INIT_PARAM (&mosaicLayout);
  /* Configuring the first (and only) window */
  /* position of window can be changed by following cordinates, keeping in
     center by default */

  
  if (0 == pAppData->displayId)
  {
    /* For On-chip HDMI, configure startX and startY to center the image */
    mosaicLayout.sMosaicWinFmt[0].winStartX = 0;
    mosaicLayout.sMosaicWinFmt[0].winStartY = 0;  
    /* If chosen On-chip HDMI as display, fit the mosaic window to the size of
    /* decoded output */
    mosaicLayout.sMosaicWinFmt[0].winWidth = HD_WIDTH;
    mosaicLayout.sMosaicWinFmt[0].winHeight = HD_HEIGHT;
    mosaicLayout.sMosaicWinFmt[0].pitch[VFDC_YUV_INT_ADDR_IDX] = 
                                       HD_WIDTH * 2;
  }
  else if (1 == pAppData->displayId)
  {
    /* For LCD Display, start the window at (0,0) */
    mosaicLayout.sMosaicWinFmt[0].winStartX = 0;
    mosaicLayout.sMosaicWinFmt[0].winStartY = 0;
    
    /*If LCD is chosen, fir the mosaic window to the size of the LCD display*/
    mosaicLayout.sMosaicWinFmt[0].winWidth = LCD_WIDTH;
    mosaicLayout.sMosaicWinFmt[0].winHeight = LCD_HEIGHT;
    mosaicLayout.sMosaicWinFmt[0].pitch[VFDC_YUV_INT_ADDR_IDX] = 
                                       LCD_WIDTH * 2;  
  }
 
  if ((0 == pAppData->displayId) || (1 == pAppData->displayId))
  {
    /*Mosaic configuration is not supported for SD Display*/
    /*Doing this SetParam for SD Display is a workaround to avoid firmware mod*/
    mosaicLayout.sMosaicWinFmt[0].dataFormat = VFDC_DF_YUV422I_YVYU;
    mosaicLayout.sMosaicWinFmt[0].bpp = VFDC_BPP_BITS16;
    mosaicLayout.sMosaicWinFmt[0].priority = 0;
    mosaicLayout.nLayoutId = 0;
    mosaicLayout.nDisChannelNum = 0;
    /* Only one window in this layout, hence setting it to 1 */
    mosaicLayout.nNumWindows = 1;

    eError = OMX_SetParameter (pAppData->pDisHandle, (OMX_INDEXTYPE)
                               OMX_TI_IndexParamVFDCCreateMosaicLayout,
                               &mosaicLayout);
    if (eError != OMX_ErrorNone)
      ERROR ("failed to set mosaic window parameter\n");

    /* map OMX port to window */
    OMX_INIT_PARAM (&port2Winmap);
    /* signifies the layout id this port2win mapping refers to */
    port2Winmap.nLayoutId = 0;
    /* Just one window in this layout, hence setting the value to 1 */
    port2Winmap.numWindows = 1;
    /* Only 1st input port used here */
    port2Winmap.omxPortList[0] = OMX_VFDC_INPUT_PORT_START_INDEX + 0;
    eError =
      OMX_SetConfig (pAppData->pDisHandle,
                     (OMX_INDEXTYPE) OMX_TI_IndexConfigVFDCMosaicPort2WinMap,
                     &port2Winmap);
    if (eError != OMX_ErrorNone)
    {
      ERROR ("failed to map port to windows\n");
    }
  }
  /* Setting Memory type at input port to Raw Memory */
  printf ("setting input and output memory type to default\n");
  OMX_INIT_PARAM (&memTypeCfg);
  memTypeCfg.nPortIndex = OMX_VFPC_INPUT_PORT_START_INDEX;
  memTypeCfg.eBufMemoryType = OMX_BUFFER_MEMORY_DEFAULT;
  eError =
    OMX_SetParameter (pAppData->pDisHandle, OMX_TI_IndexParamBuffMemType,
                      &memTypeCfg);

  if (eError != OMX_ErrorNone)
  {
    ERROR ("failed to set memory Type at input port\n");
  }
}

/* Nothing beyond this point */
