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
#include "ilclient.h"
#include "ilclient_utils.h"
#include "platform_utils.h"
#include <omx_vdec.h>
#include <omx_vfpc.h>
#include <omx_vfdc.h>
#include <omx_ctrl.h>
#include <OMX_TI_Index.h>
#include "OMX_TI_Video.h"
#include "es_parser.h"

/*--------------------------- defines ----------------------------------------*/
/* Align address "a" at "b" boundary */
#define UTIL_ALIGN(a,b)  ((((uint32_t)(a)) + (b)-1) & (~((uint32_t)((b)-1))))

#define HD_WIDTH       (1920)
#define HD_HEIGHT      (1080)

/* DM8168 PG1.1 SD display takes only 720 as width */

#define SD_WIDTH       (720)

unsigned int CROPPED_WIDTH = 1920;
unsigned int CROPPED_HEIGHT = 1080;


#define CROP_START_X    0
#define CROP_START_Y    0

void usage (IL_ARGS *argsp)
{
  printf ("decode_display -w <image_width> -h <image_height> -f <frame_rate> "
          "-i <input_file> -g <gfx on/off> -d <0/1>\n"
          "-i | --input           input filename \n"
          "-w | --width           image width \n"
          "-h | --height          image height \n"
          "-f | --framerate       decode frame rate - max 60 \n"
          "-c | --codec           codec to be used - should be one of h264,h263,mpeg4,vc1,mpeg2,mjpeg \n"
          "-g | --gfx             gfx - 0 - off, 1 - on \n"
          "-d | --display_id      0 - for on-chip HDMI, 1 for secondary display \n");
  printf(" example : ./decode_display_a8host_debug.xv5T -i sample.h264 -w 1920 -h 1080 -f 30 -g 0 -d 0  -c h264 \n");       
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
  const char shortOptions[] = "i:w:h:f:g:c:d:";
  const struct option longOptions[] =
  {
    {"input", required_argument, NULL, ArgID_INPUT_FILE},
    {"width", required_argument, NULL, ArgID_WIDTH},
    {"height", required_argument, NULL, ArgID_HEIGHT},
    {"framerate", required_argument, NULL, ArgID_FRAMERATE},
    {"gfx", required_argument, NULL, ArgID_GFX},
    {"codec", required_argument, NULL, ArgID_CODEC},
    {"display_id", required_argument, NULL, ArgID_DISPLAYID},
    {0, 0, 0, 0}
  };
  char *gfxen = "fbdev enable";
  char *gfxdis = "fbdev disable";
  char *gfxoption = "fbdev disable";

  int index, infile = 0, width = 0, height = 0, framerate = 0, gfx = 0, codec = 0;
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
      case ArgID_INPUT_FILE:
      case 'i':
        strncpy (argsp->input_file, optarg, MAX_FILE_NAME_SIZE);
        infile = 1;
        break;
     case ArgID_CODEC:
      case 'c':
        strncpy (argsp->codec_name, optarg, MAX_CODEC_NAME_SIZE);
        codec = 1;
        break;
  
      case ArgID_FRAMERATE:
      case 'f':
        argsp->frame_rate = atoi (optarg);
        framerate = 1;
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
  
      case ArgID_GFX:
      case 'g':
        argsp->gfx = atoi (optarg);
        gfx = 1;
        if (argsp->gfx)
        {
          gfxoption = gfxen;
        }
        else
        {
          gfxoption = gfxdis;
        }
        break;
  
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

  if (!infile || !framerate || !width || !height || !gfx || !codec || !display_id)
  {
    usage (argsp);
    exit (1);
  }

  printf ("input file: %s\n", argsp->input_file);
  printf ("width: %d\n", argsp->width);
  printf ("height: %d\n", argsp->height);
  printf ("frame_rate: %d\n", argsp->frame_rate);
  printf ("gfx: %s\n", gfxoption);
  printf ("codec: %s\n", argsp->codec_name);
  printf ("display_id: %d\n", argsp->display_id);

}

/* ========================================================================== */
/**
* IL_ClientInit() : This function is to allocate and initialize the application
*                   data structure. It is just to maintain application control.
*
* @param pAppData          : appliaction / client data Handle 
* @param width             : stream width
* @param height            : stream height
* @param frameRate         : decoder frame rate
* @param displayId         : display instance id
*
*  @return      
*
*
*/
/* ========================================================================== */

void IL_ClientInit (IL_Client **pAppData, int width, int height, int frameRate,
                    int displayId , OMX_VIDEO_CODINGTYPE coding)
{
  int i;
  extern OMX_U8 PADX;
  extern OMX_U8 PADY;

  IL_Client *pAppDataPtr;
  IL_CLIENT_INPORT_PARAMS *inPortParamsPtr;
  IL_CLIENT_OUTPORT_PARAMS *outPortParamsPtr;

  /* Allocating data structure for IL client structure / buffer management */

  pAppDataPtr = (IL_Client *) malloc (sizeof (IL_Client));
  memset (pAppDataPtr, 0x0, sizeof (IL_Client));

  /* update the user provided parameters */
  pAppDataPtr->nHeight = height;
  pAppDataPtr->nWidth = width;
  pAppDataPtr->nFrameRate = frameRate;
  pAppDataPtr->codingType = coding; 
  pAppDataPtr->displayId = displayId;

  if(CROPPED_WIDTH > width) {
   CROPPED_WIDTH = width;
  }
  
  if(CROPPED_HEIGHT > height) {
   CROPPED_HEIGHT = height;
  }
   
  /* alloacte data structure for each component used in this IL Cleint */
  pAppDataPtr->decILComp =
    (IL_CLIENT_COMP_PRIVATE *) malloc (sizeof (IL_CLIENT_COMP_PRIVATE));
  memset (pAppDataPtr->decILComp, 0x0, sizeof (IL_CLIENT_COMP_PRIVATE));

  /* these semaphores are used for tracking the callbacks received from
     component */
  pAppDataPtr->decILComp->eos = malloc (sizeof (semp_t));
  semp_init (pAppDataPtr->decILComp->eos, 0);

  pAppDataPtr->decILComp->done_sem = malloc (sizeof (semp_t));
  semp_init (pAppDataPtr->decILComp->done_sem, 0);

  pAppDataPtr->decILComp->port_sem = malloc (sizeof (semp_t));
  semp_init (pAppDataPtr->decILComp->port_sem, 0);

  /* alloacte data structure for each component used in this IL Cleint */
  pAppDataPtr->scILComp =
    (IL_CLIENT_COMP_PRIVATE *) malloc (sizeof (IL_CLIENT_COMP_PRIVATE));
  memset (pAppDataPtr->scILComp, 0x0, sizeof (IL_CLIENT_COMP_PRIVATE));

  /* these semaphores are used for tracking the callbacks received from
     component */
  pAppDataPtr->scILComp->eos = malloc (sizeof (semp_t));
  semp_init (pAppDataPtr->scILComp->eos, 0);

  pAppDataPtr->scILComp->done_sem = malloc (sizeof (semp_t));
  semp_init (pAppDataPtr->scILComp->done_sem, 0);

  pAppDataPtr->scILComp->port_sem = malloc (sizeof (semp_t));
  semp_init (pAppDataPtr->scILComp->port_sem, 0);

  /* alloacte data structure for each component used in this IL Cleint */
  pAppDataPtr->nfILComp =
    (IL_CLIENT_COMP_PRIVATE *) malloc (sizeof (IL_CLIENT_COMP_PRIVATE));
  memset (pAppDataPtr->nfILComp, 0x0, sizeof (IL_CLIENT_COMP_PRIVATE));

  /* these semaphores are used for tracking the callbacks received from
     component */
  pAppDataPtr->nfILComp->eos = malloc (sizeof (semp_t));
  semp_init (pAppDataPtr->nfILComp->eos, 0);

  pAppDataPtr->nfILComp->done_sem = malloc (sizeof (semp_t));
  semp_init (pAppDataPtr->nfILComp->done_sem, 0);

  pAppDataPtr->nfILComp->port_sem = malloc (sizeof (semp_t));
  semp_init (pAppDataPtr->nfILComp->port_sem, 0);

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

  /* number of ports for each component, which this IL cleint will handle, this 
     will be equal to number of ports supported by component or less */
  pAppDataPtr->decILComp->numInport = 1;
  pAppDataPtr->decILComp->numOutport = 1;
  pAppDataPtr->decILComp->startOutportIndex = 1;

  pAppDataPtr->scILComp->numInport = 1;
  pAppDataPtr->scILComp->numOutport = 1;

  /* VFPC OMX component support max 16 input / output ports, so o/p port index
     starts at 16 */
  pAppDataPtr->scILComp->startOutportIndex = OMX_VFPC_NUM_INPUT_PORTS;

  pAppDataPtr->nfILComp->numInport = 1;
  pAppDataPtr->nfILComp->numOutport = 1;

  /* VFPC OMX component support max 16 input / output ports, so o/p port index
     starts at 16 */
  pAppDataPtr->nfILComp->startOutportIndex = OMX_VFPC_NUM_INPUT_PORTS;

  pAppDataPtr->disILComp->numInport = 1;

  /* display does not has o/pports */
  pAppDataPtr->disILComp->numOutport = 0;
  pAppDataPtr->disILComp->startOutportIndex = 0;

  /* allocate data structure for input and output port params of IL client
     component, It is for mainitaining data structure in IL Cleint only.
     Components will have its own data structure inside omx components */
  pAppDataPtr->decILComp->inPortParams =
    malloc (sizeof (IL_CLIENT_INPORT_PARAMS) *
            pAppDataPtr->decILComp->numInport);
  memset (pAppDataPtr->decILComp->inPortParams, 0x0,
          sizeof (IL_CLIENT_INPORT_PARAMS));

  pAppDataPtr->decILComp->outPortParams =
    malloc (sizeof (IL_CLIENT_INPORT_PARAMS) *
            pAppDataPtr->decILComp->numOutport);
  memset (pAppDataPtr->decILComp->outPortParams, 0x0,
          sizeof (IL_CLIENT_OUTPORT_PARAMS));

  pAppDataPtr->scILComp->inPortParams =
    malloc (sizeof (IL_CLIENT_INPORT_PARAMS) *
            pAppDataPtr->scILComp->numInport);

  memset (pAppDataPtr->scILComp->inPortParams, 0x0,
          sizeof (IL_CLIENT_INPORT_PARAMS));

  pAppDataPtr->scILComp->outPortParams =
    malloc (sizeof (IL_CLIENT_INPORT_PARAMS) *
            pAppDataPtr->scILComp->numOutport);
  memset (pAppDataPtr->scILComp->outPortParams, 0x0,
          sizeof (IL_CLIENT_OUTPORT_PARAMS));

  pAppDataPtr->nfILComp->inPortParams =
    malloc (sizeof (IL_CLIENT_INPORT_PARAMS) *
            pAppDataPtr->nfILComp->numInport);

  memset (pAppDataPtr->nfILComp->inPortParams, 0x0,
          sizeof (IL_CLIENT_INPORT_PARAMS));

  pAppDataPtr->nfILComp->outPortParams =
    malloc (sizeof (IL_CLIENT_INPORT_PARAMS) *
            pAppDataPtr->nfILComp->numOutport);
  memset (pAppDataPtr->nfILComp->outPortParams, 0x0,
          sizeof (IL_CLIENT_OUTPORT_PARAMS));

  pAppDataPtr->disILComp->inPortParams =
    malloc (sizeof (IL_CLIENT_INPORT_PARAMS) *
            pAppDataPtr->disILComp->numInport);

  memset (pAppDataPtr->disILComp->inPortParams, 0x0,
          sizeof (IL_CLIENT_INPORT_PARAMS));

  /* specify some of the parameters, that will be used for initializing OMX
     component parameters */
  for (i = 0; i < pAppDataPtr->decILComp->numInport; i++)
  {
    inPortParamsPtr = pAppDataPtr->decILComp->inPortParams + i;
    inPortParamsPtr->nBufferCountActual = IL_CLIENT_DECODER_INPUT_BUFFER_COUNT;
    /* input buffers size for bitstream buffers, It can be smaller than this
       value , setting it for approx value */
    inPortParamsPtr->nBufferSize = pAppDataPtr->nHeight * pAppDataPtr->nWidth;
    /* this pipe is used for taking buffers from file read thread */
    pipe ((int *) inPortParamsPtr->ipBufPipe);
  }
  for (i = 0; i < pAppDataPtr->decILComp->numOutport; i++)
  {
    outPortParamsPtr = pAppDataPtr->decILComp->outPortParams + i;
    outPortParamsPtr->nBufferCountActual = IL_CLIENT_DECODER_OUTPUT_BUFFER_COUNT;
    /* H264 decoder uses padding on both sides, as well requires 128 byte
       alignment so this value is calcualtes as follows, decoder o/p is always
       YUV420 packet semi planner so * 1.5 */

    outPortParamsPtr->nBufferSize =
      (UTIL_ALIGN ((pAppDataPtr->nWidth + (2 * PADX)), 128) *
       ((((pAppDataPtr->nHeight + 15) & 0xfffffff0) + (4 * PADY))) * 3) >> 1;

    /* This pipe is used if output is directed to file write thread, in this
       example, file write is not used */
    pipe ((int *) outPortParamsPtr->opBufPipe);
  }
  /* each componet will have local pipe to take bufffes from other component or 
     its own consumed buffer, so that it can be passed to other conected
     components */
  pipe ((int *) pAppDataPtr->decILComp->localPipe);

  for (i = 0; i < pAppDataPtr->scILComp->numInport; i++)
  {
    inPortParamsPtr = pAppDataPtr->scILComp->inPortParams + i;
    inPortParamsPtr->nBufferCountActual = IL_CLIENT_SCALAR_INPUT_BUFFER_COUNT;
    /* since input of scalar is connected to output of decoder, size is same as 
       decoder o/p buffers */
    inPortParamsPtr->nBufferSize =
      (UTIL_ALIGN ((pAppDataPtr->nWidth + (2 * PADX)), 128) *
       ((pAppDataPtr->nHeight + (4 * PADY))) * 3) >> 1;

    /* this pipe will not be used in this application, as scalar does not read
       / write into file */
    pipe ((int *) inPortParamsPtr->ipBufPipe);
  }
  for (i = 0; i < pAppDataPtr->scILComp->numOutport; i++)
  {
    outPortParamsPtr = pAppDataPtr->scILComp->outPortParams + i;
    outPortParamsPtr->nBufferCountActual = IL_CLIENT_SCALAR_OUTPUT_BUFFER_COUNT;
    /* scalar is configured for YUV 422 output, so buffer size is calculated as 
       follows */
    outPortParamsPtr->nBufferSize =
      pAppDataPtr->nHeight * ((pAppDataPtr->nWidth + 15) & 0xfffffff0) * 2;

     if (1 == pAppDataPtr->displayId) {
      /* configure the buffer size to that of the display size, for custom
         display this can be used to change width and height */
      outPortParamsPtr->nBufferSize = DISPLAY_HEIGHT * DISPLAY_WIDTH * 2;      
    }
    
     if (2 == pAppDataPtr->displayId) {
      /* configure the buffer size to that of the display size, for custom
         display this can be used to change width and height */
      outPortParamsPtr->nBufferSize = SD_DISPLAY_HEIGHT * SD_DISPLAY_WIDTH * 2;      
    }

    /* this pipe will not be used in this application, as scalar does not read
       / write into file */
    pipe ((int *) outPortParamsPtr->opBufPipe);
  }

  /* each componet will have local pipe to take bufffes from other component or 
     its own consumed buffer, so that it can be passed to other conected
     components */
  pipe ((int *) pAppDataPtr->scILComp->localPipe);

  for (i = 0; i < pAppDataPtr->nfILComp->numInport; i++)
  {
    inPortParamsPtr = pAppDataPtr->nfILComp->inPortParams + i;
    inPortParamsPtr->nBufferCountActual = IL_CLIENT_NF_INPUT_BUFFER_COUNT;
    /* since input of scalar is connected to output of decoder, size is same as 
       decoder o/p buffers */
    inPortParamsPtr->nBufferSize = SD_DISPLAY_HEIGHT * SD_DISPLAY_WIDTH  * 2;

    /* this pipe will not be used in this application, as scalar does not read
       / write into file */
    pipe ((int *) inPortParamsPtr->ipBufPipe);
  }
  for (i = 0; i < pAppDataPtr->nfILComp->numOutport; i++)
  {
    outPortParamsPtr = pAppDataPtr->nfILComp->outPortParams + i;
    outPortParamsPtr->nBufferCountActual = IL_CLIENT_NF_OUTPUT_BUFFER_COUNT;
    /* scalar is configured for YUV 422 output, so buffer size is calculates as 
       follows */
    outPortParamsPtr->nBufferSize =
    (SD_DISPLAY_HEIGHT * SD_DISPLAY_WIDTH  * 3) >> 1;
    /* this pipe will not be used in this application, as scalar does not read
       / write into file */
    pipe ((int *) outPortParamsPtr->opBufPipe);
  }

  /* each componet will have local pipe to take bufffes from other component or 
     its own consumed buffer, so that it can be passed to other conected
     components */
  pipe ((int *) pAppDataPtr->nfILComp->localPipe);

  for (i = 0; i < pAppDataPtr->disILComp->numInport; i++)
  {
    inPortParamsPtr = pAppDataPtr->disILComp->inPortParams + i;
    inPortParamsPtr->nBufferCountActual = IL_CLIENT_DISPLAY_INPUT_BUFFER_COUNT;
    inPortParamsPtr->nBufferSize =
      pAppDataPtr->nHeight * pAppDataPtr->nWidth * 2;

    /* configuring for custom display parameters */  
     if (1 == pAppDataPtr->displayId) {
      inPortParamsPtr->nBufferSize = DISPLAY_HEIGHT * DISPLAY_WIDTH * 2;     
     }
     /* For SD display 420 output */
     if (2 == pAppDataPtr->displayId) {
     inPortParamsPtr->nBufferSize = (SD_DISPLAY_HEIGHT * SD_DISPLAY_WIDTH * 3) >> 1;     
     }

    /* this pipe will not be used in this application, as scalar does not read
       / write into file */
    pipe ((int *) inPortParamsPtr->ipBufPipe);
  }

  /* each componet will have local pipe to take bufffes from other component or 
     its own consumed buffer, so that it can be passed to other conected
     components */
  pipe ((int *) pAppDataPtr->disILComp->localPipe);

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
  int i;
  IL_CLIENT_INPORT_PARAMS *inPortParamsPtr;
  IL_CLIENT_OUTPORT_PARAMS *outPortParamsPtr;

  close ((int) pAppData->disILComp->localPipe);

  for (i = 0; i < pAppData->disILComp->numInport; i++)
  {
    inPortParamsPtr = pAppData->disILComp->inPortParams + i;
    /* this pipe will not be used in this application, as scalar does not read
       / write into file */
    close ((int) inPortParamsPtr->ipBufPipe);
  }

  close ((int) pAppData->scILComp->localPipe);

  for (i = 0; i < pAppData->scILComp->numInport; i++)
  {
    inPortParamsPtr = pAppData->scILComp->inPortParams + i;
    /* this pipe is not used in this application, as scalar does not read /
       write into file */
    close ((int) inPortParamsPtr->ipBufPipe);
  }
  for (i = 0; i < pAppData->scILComp->numOutport; i++)
  {
    outPortParamsPtr = pAppData->scILComp->outPortParams + i;
    /* this pipe is not used in this application, as scalar does not read /
       write into file */
    close ((int) outPortParamsPtr->opBufPipe);
  }

  close ((int) pAppData->nfILComp->localPipe);

  for (i = 0; i < pAppData->nfILComp->numInport; i++)
  {
    inPortParamsPtr = pAppData->nfILComp->inPortParams + i;
    /* this pipe is not used in this application, as scalar does not read /
       write into file */
    close ((int) inPortParamsPtr->ipBufPipe);
  }
  for (i = 0; i < pAppData->nfILComp->numOutport; i++)
  {
    outPortParamsPtr = pAppData->nfILComp->outPortParams + i;
    /* this pipe is not used in this application, as scalar does not read /
       write into file */
    close ((int) outPortParamsPtr->opBufPipe);
  }

  close ((int) pAppData->decILComp->localPipe);

  for (i = 0; i < pAppData->decILComp->numInport; i++)
  {
    inPortParamsPtr = pAppData->decILComp->inPortParams + i;
    close ((int) inPortParamsPtr->ipBufPipe);
  }
  for (i = 0; i < pAppData->decILComp->numOutport; i++)
  {
    outPortParamsPtr = pAppData->decILComp->outPortParams + i;
    /* This pipe is used if output is directed to file write thread, in this
       example, file write is not used */
    close ((int) outPortParamsPtr->opBufPipe);
  }

  free (pAppData->disILComp->inPortParams);

  free (pAppData->scILComp->inPortParams);

  free (pAppData->scILComp->outPortParams);
  
  free (pAppData->nfILComp->inPortParams);

  free (pAppData->nfILComp->outPortParams);  

  free (pAppData->decILComp->inPortParams);

  free (pAppData->decILComp->outPortParams);

  /* these semaphores are used for tracking the callbacks received from
     component */
  semp_deinit (pAppData->scILComp->eos);
  free (pAppData->scILComp->eos);

  semp_deinit (pAppData->scILComp->done_sem);
  free (pAppData->scILComp->done_sem);

  semp_deinit (pAppData->scILComp->port_sem);
  free (pAppData->scILComp->port_sem);

  semp_deinit (pAppData->nfILComp->eos);
  free (pAppData->nfILComp->eos);

  semp_deinit (pAppData->nfILComp->done_sem);
  free (pAppData->nfILComp->done_sem);

  semp_deinit (pAppData->nfILComp->port_sem);
  free (pAppData->nfILComp->port_sem);

  semp_deinit (pAppData->disILComp->eos);
  free (pAppData->disILComp->eos);

  semp_deinit (pAppData->disILComp->done_sem);
  free (pAppData->disILComp->done_sem);

  semp_deinit (pAppData->disILComp->port_sem);
  free (pAppData->disILComp->port_sem);

  semp_deinit (pAppData->decILComp->eos);
  free (pAppData->decILComp->eos);

  semp_deinit (pAppData->decILComp->done_sem);
  free (pAppData->decILComp->done_sem);

  semp_deinit (pAppData->decILComp->port_sem);
  free (pAppData->decILComp->port_sem);

  free (pAppData->decILComp);

  free (pAppData->scILComp);
  
  free (pAppData->nfILComp);

  free (pAppData->disILComp);

  if (pAppData->eCompressionFormat == OMX_VIDEO_CodingAVC)
  {
   if(pAppData->pc.readBuf)
   {
    free(pAppData->pc.readBuf);
   }
  }
  else if (pAppData->eCompressionFormat == OMX_VIDEO_CodingMPEG2)
  {
   if(pAppData->pcmpeg2.working_frame)
   {
    free(pAppData->pcmpeg2.working_frame);
   }
   if(pAppData->pcmpeg2.savedbuff)
   {
    free(pAppData->pcmpeg2.savedbuff);
   }
  }
  else if ( pAppData->eCompressionFormat == OMX_VIDEO_CodingH263   )
  {
   if(pAppData->pch263.working_frame)
   {
    free(pAppData->pch263.working_frame);
   }
   if(pAppData->pch263.savedbuff)
   {
    free(pAppData->pch263.savedbuff);
   }
  }
  else if ( pAppData->eCompressionFormat == OMX_VIDEO_CodingMPEG4   )
  {
   if(pAppData->pcmpeg4.working_frame)
   {
    free(pAppData->pcmpeg4.working_frame);
   }
   if(pAppData->pcmpeg4.savedbuff)
   {
    free(pAppData->pcmpeg4.savedbuff);
   }
  }
  else if ( pAppData->eCompressionFormat == OMX_VIDEO_CodingWMV   )
  {
   if(pAppData->pcvc1.working_frame)
   {
    free(pAppData->pcvc1.working_frame);
   }
   if(pAppData->pcvc1.savedbuff)
   {
    free(pAppData->pcvc1.savedbuff);
   }
  }

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
* IL_ClientFillBitStreamData() : Function to parse a frame and copy it to 
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

unsigned int IL_ClientFillBitStreamData (IL_Client *pAppData,
                                         OMX_BUFFERHEADERTYPE *pBuf)
{
  unsigned int dataRead = 0;
  int frameSize = 0;
  IL_CLIENT_COMP_PRIVATE *decILComp = NULL;
  decILComp = ((IL_Client *) pAppData)->decILComp;
  /* update the parser buffer, with the decoder input buffer */
 if(pAppData->codingType == OMX_VIDEO_CodingAVC)
 {
  pAppData->pc.outBuf.ptr = pBuf->pBuffer;
  pAppData->pc.outBuf.bufsize = decILComp->inPortParams->nBufferSize;
  pAppData->pc.outBuf.bufused = 0;
  frameSize = Decode_GetNextFrameSize (&pAppData->pc);
 }
 else if(pAppData->codingType == OMX_VIDEO_CodingMPEG4)
 {
  pAppData->pcmpeg4.buff_in = pBuf->pBuffer;
  frameSize = Decode_GetNextMpeg4FrameSize (&pAppData->pcmpeg4);
 }
 else if(pAppData->codingType == OMX_VIDEO_CodingH263)
 {
  pAppData->pch263.buff_in = pBuf->pBuffer;
  frameSize = Decode_GetNextH263FrameSize (&pAppData->pch263);
 }
 else if(pAppData->codingType == OMX_VIDEO_CodingWMV)
 {
  pAppData->pcvc1.buff_in = pBuf->pBuffer;
  frameSize = Decode_GetNextVC1FrameSize (&pAppData->pcvc1);
 }
 else if(pAppData->codingType == OMX_VIDEO_CodingMPEG2)
 {
  pAppData->pcmpeg2.buff_in = pBuf->pBuffer;
  frameSize = Decode_GetNextMpeg2FrameSize (&pAppData->pcmpeg2);
 }
 else {
   /* for keeping the image on display timeout */
   sleep(2);
   frameSize = 0;
 }
  /* Get the size of one frame at a time */

  dataRead = frameSize;

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
* IL_ClientConnectComponents() : This util function is to update the pipe
*                                information of other connected comonnet, so that
*                                buffers can be passed to connected component.
*
* @param handleCompPrivA   : application component data structure for producer
* @param compAPortOut      : port of producer comp
* @param handleCompPrivB   : application component data structure for consumer
* @param compBPortIn       : port number of the consumer component
*
*  @return      
*  String conversion of the OMX_ERRORTYPE
*
*/
/* ========================================================================== */

OMX_ERRORTYPE IL_ClientConnectComponents (IL_CLIENT_COMP_PRIVATE
                                            *handleCompPrivA,
                                          unsigned int compAPortOut,
                                          IL_CLIENT_COMP_PRIVATE
                                            *handleCompPrivB,
                                          unsigned int compBPortIn)
{
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  IL_CLIENT_OUTPORT_PARAMS *outPortParamPtr = NULL;
  IL_CLIENT_INPORT_PARAMS *inPortParamPtr = NULL;

  /* update the input port connect structure */
  outPortParamPtr =
    handleCompPrivA->outPortParams + compAPortOut -
    handleCompPrivA->startOutportIndex;

  inPortParamPtr = handleCompPrivB->inPortParams + compBPortIn;

  /* update input port component pipe info with connected port */
  inPortParamPtr->connInfo.remoteClient = handleCompPrivA;
  inPortParamPtr->connInfo.remotePort = compAPortOut;
  inPortParamPtr->connInfo.remotePipe[0] = handleCompPrivA->localPipe[0];
  inPortParamPtr->connInfo.remotePipe[1] = handleCompPrivA->localPipe[1];

  /* update output port component pipe info with connected port */
  outPortParamPtr->connInfo.remoteClient = handleCompPrivB;
  outPortParamPtr->connInfo.remotePort = compBPortIn;
  outPortParamPtr->connInfo.remotePipe[0] = handleCompPrivB->localPipe[0];
  outPortParamPtr->connInfo.remotePipe[1] = handleCompPrivB->localPipe[1];

  return eError;
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

OMX_ERRORTYPE IL_ClientDecUseInitialInputResources (IL_Client *pAppdata)
{

  OMX_ERRORTYPE err = OMX_ErrorNone;
  unsigned int i = 0;
  int frameSize = 0;
  IL_CLIENT_COMP_PRIVATE *decILComp = NULL;
  decILComp = ((IL_Client *) pAppdata)->decILComp;

  /* Give input buffers to component which is limited by no of input buffers
     available. Rest of the data will be read on the callback from input data
     read thread */
  for (i = 0; i < decILComp->inPortParams->nBufferCountActual; i++)
  {
     if(pAppdata->codingType == OMX_VIDEO_CodingAVC)
     {
    pAppdata->pc.outBuf.ptr = decILComp->inPortParams->pInBuff[i]->pBuffer;
    pAppdata->pc.outBuf.bufsize = decILComp->inPortParams->nBufferSize;
    pAppdata->pc.outBuf.bufused = 0;
    frameSize = Decode_GetNextFrameSize (&pAppdata->pc);
     }
     else if(pAppdata->codingType == OMX_VIDEO_CodingH263)
     {
         pAppdata->pch263.buff_in = decILComp->inPortParams->pInBuff[i]->pBuffer;
         frameSize = Decode_GetNextH263FrameSize (&pAppdata->pch263);
     }
     else if(pAppdata->codingType == OMX_VIDEO_CodingMPEG4)
     {
         pAppdata->pcmpeg4.buff_in = decILComp->inPortParams->pInBuff[i]->pBuffer;
         frameSize = Decode_GetNextMpeg4FrameSize (&pAppdata->pcmpeg4);
     }     
     else if(pAppdata->codingType == OMX_VIDEO_CodingWMV)
     {
         pAppdata->pcvc1.buff_in = decILComp->inPortParams->pInBuff[i]->pBuffer;
         frameSize = Decode_GetNextVC1FrameSize (&pAppdata->pcvc1);
     }
     else if(pAppdata->codingType == OMX_VIDEO_CodingMPEG2)
     {
         pAppdata->pcmpeg2.buff_in = decILComp->inPortParams->pInBuff[i]->pBuffer;
         frameSize = Decode_GetNextMpeg2FrameSize (&pAppdata->pcmpeg2);
     }
     else if(pAppdata->codingType == OMX_VIDEO_CodingMJPEG)
     {
      /* pass same data to all the buffers; mjpeg decode example does not have 
         parser and only single image is decoded and displayed */
      fseek(pAppdata->fIn, 0, SEEK_END);
      frameSize = ftell(pAppdata->fIn);
      fseek(pAppdata->fIn, 0 , SEEK_SET);
      printf(" reading input file of size %d bytes into input buffer \n ", frameSize); 
      fread (decILComp->inPortParams->pInBuff[i]->pBuffer, 1, frameSize, pAppdata->fIn); 
      fseek(pAppdata->fIn, 0 , SEEK_SET);
      }

    /* Exit the loop if no data available */
    if (!frameSize)
    {
      break;
    }

    decILComp->inPortParams->pInBuff[i]->nFilledLen = frameSize;
    decILComp->inPortParams->pInBuff[i]->nOffset = 0;
    decILComp->inPortParams->pInBuff[i]->nAllocLen = frameSize;
    decILComp->inPortParams->pInBuff[i]->nInputPortIndex = 0;

    /* Pass the input buffer to the component */

    err = OMX_EmptyThisBuffer (decILComp->handle,
                               decILComp->inPortParams->pInBuff[i]);

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
* IL_ClientSetDecodeParams() : Function to fill the port definition 
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
OMX_ERRORTYPE IL_ClientSetDecodeParams (IL_Client *pAppData)
{
  extern OMX_U8 PADX;
  extern OMX_U8 PADY;

  OMX_ERRORTYPE eError = OMX_ErrorNone;
  OMX_HANDLETYPE pHandle = pAppData->pDecHandle;
  OMX_PORT_PARAM_TYPE portInit;
  OMX_PARAM_PORTDEFINITIONTYPE pInPortDef, pOutPortDef;
  OMX_VIDEO_PARAM_STATICPARAMS tStaticParam;
  OMX_PARAM_COMPPORT_NOTIFYTYPE pNotifyType;
  
  if (!pHandle)
  {
    eError = OMX_ErrorBadParameter;
    goto EXIT;
  }
  printf("PADX: %d PADY: %d\n",PADX, PADY);

  OMX_INIT_PARAM (&portInit);

  portInit.nPorts = 2;
  portInit.nStartPortNumber = 0;
  eError = OMX_SetParameter (pHandle, OMX_IndexParamVideoInit, &portInit);
  if (eError != OMX_ErrorNone)
  {
    goto EXIT;
  }

  /* Set the component's OMX_PARAM_PORTDEFINITIONTYPE structure (input) */

  OMX_INIT_PARAM (&pInPortDef);

  /* populate the input port definataion structure, It is Standard OpenMax
     structure */
  /* set the port index */
  pInPortDef.nPortIndex = OMX_VIDDEC_INPUT_PORT;
  /* It is input port so direction is set as Input, Empty buffers call would be 
     accepted based on this */
  pInPortDef.eDir = OMX_DirInput;
  /* number of buffers are set here */
  pInPortDef.nBufferCountActual = IL_CLIENT_DECODER_INPUT_BUFFER_COUNT;
  /* buffer size by deafult is assumed as width * height for input bitstream
     which would suffice most of the cases */
  pInPortDef.nBufferSize = pAppData->nWidth * pAppData->nHeight;

  pInPortDef.bEnabled = OMX_TRUE;
  pInPortDef.bPopulated = OMX_FALSE;

  /* OMX_VIDEO_PORTDEFINITION values for input port */
  /* set the width and height, used for buffer size calculation */
  pInPortDef.format.video.nFrameWidth = pAppData->nWidth;
  pInPortDef.format.video.nFrameHeight = pAppData->nHeight;
  /* for bitstream buffer stride is not a valid parameter */
  pInPortDef.format.video.nStride = -1;
  /* component supports only frame based processing */

  /* bitrate does not matter for decoder */
  pInPortDef.format.video.nBitrate = 104857600;
  /* as per openmax frame rate is in Q16 format */
  pInPortDef.format.video.xFramerate = (pAppData->nFrameRate) << 16;
  /* input port would receive encoded stream */
  pInPortDef.format.video.eCompressionFormat = pAppData->codingType;
  /* this is codec setting, OMX component does not support it */
  /* color format is irrelavant */
  pInPortDef.format.video.eColorFormat = OMX_COLOR_FormatYUV420Planar;

  eError =
    OMX_SetParameter (pHandle, OMX_IndexParamPortDefinition, &pInPortDef);
  if (eError != OMX_ErrorNone)
  {
    goto EXIT;
  }

  /* Set the component's OMX_PARAM_PORTDEFINITIONTYPE structure (output) */
  OMX_INIT_PARAM (&pOutPortDef);

  /* setting the port index for output port, properties are set based on this
     index */
  pOutPortDef.nPortIndex = OMX_VIDDEC_OUTPUT_PORT;
  pOutPortDef.eDir = OMX_DirOutput;
  /* componet would expect these numbers of buffers to be allocated */
  pOutPortDef.nBufferCountActual = IL_CLIENT_DECODER_OUTPUT_BUFFER_COUNT;

  /* codec requires padded height and width and width needs to be aligned at
     128 byte boundary */

  pOutPortDef.nBufferSize =
    ((((pAppData->nWidth + (2 * PADX) +
        127) & 0xFFFFFF80) * ((((pAppData->nHeight + 15) & 0xfffffff0) + (4 * PADY))) * 3) >> 1);
  pOutPortDef.bEnabled = OMX_TRUE;
  pOutPortDef.bPopulated = OMX_FALSE;
  /* currently component alloactes contigous buffers with 128 alignment, these
     values are do't care */

  /* OMX_VIDEO_PORTDEFINITION values for output port */
  pOutPortDef.format.video.nFrameWidth = pAppData->nWidth;
  pOutPortDef.format.video.nFrameHeight = ((pAppData->nHeight + 15) & 0xfffffff0);
  /* stride is set as buffer width */
  pOutPortDef.format.video.nStride =
    UTIL_ALIGN ((pAppData->nWidth + (2 * PADX)), 128);

  /* bitrate does not matter for decoder */
  pOutPortDef.format.video.nBitrate = 25000000;
  /* as per openmax frame rate is in Q16 format */
  pOutPortDef.format.video.xFramerate = (pAppData->nFrameRate) << 16;
  /* output is raw YUV 420 SP format, It support only this */
  pOutPortDef.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
  pOutPortDef.format.video.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;

  eError =
    OMX_SetParameter (pHandle, OMX_IndexParamPortDefinition, &pOutPortDef);
  if (eError != OMX_ErrorNone)
  {
    eError = OMX_ErrorBadParameter;
    goto EXIT;
  }

  eError =
    OMX_GetParameter (pHandle, OMX_IndexParamPortDefinition, &pOutPortDef);
  if (eError != OMX_ErrorNone)
  {
    eError = OMX_ErrorBadParameter;
    goto EXIT;
  }
  pAppData->nDecStride = pOutPortDef.format.video.nStride;

  /* Make VDEC execute periodically based on fps */
  OMX_INIT_PARAM(&pNotifyType);
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

  /* Set the codec's static parameters, set it at the end, so above parameter settings
     is not disturbed */
  if (pAppData->codingType == OMX_VIDEO_CodingAVC) {
      
    OMX_INIT_PARAM (&tStaticParam);
   
    tStaticParam.nPortIndex = OMX_VIDDEC_OUTPUT_PORT;
   
    eError = OMX_GetParameter (pHandle, OMX_TI_IndexParamVideoStaticParams,
                               &tStaticParam);
    /* setting I frame interval */
    printf( " level set is %d \n", (int) tStaticParam.videoStaticParams.h264DecStaticParams.presetLevelIdc);
   
    tStaticParam.videoStaticParams.h264DecStaticParams.presetLevelIdc = IH264VDEC_LEVEL42;
                         
    eError = OMX_SetParameter (pHandle, OMX_TI_IndexParamVideoStaticParams,
                               &tStaticParam);
 }
EXIT:
  return eError;
}

/* ========================================================================== */
/**
* IL_ClientSetScalarParams() : Function to fill the port definition 
* structures and call the Set_Parameter function on to the scalar
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

OMX_ERRORTYPE IL_ClientSetScalarParams (IL_Client *pAppData)
{
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  OMX_PARAM_BUFFER_MEMORYTYPE memTypeCfg;
  OMX_PARAM_PORTDEFINITIONTYPE paramPort;
  OMX_PARAM_VFPC_NUMCHANNELPERHANDLE sNumChPerHandle;
  OMX_CONFIG_ALG_ENABLE algEnable;
  OMX_CONFIG_VIDCHANNEL_RESOLUTION chResolution;

  OMX_INIT_PARAM (&memTypeCfg);
  memTypeCfg.nPortIndex = OMX_VFPC_INPUT_PORT_START_INDEX;
  memTypeCfg.eBufMemoryType = OMX_BUFFER_MEMORY_DEFAULT;
  eError =
    OMX_SetParameter (pAppData->pScHandle, OMX_TI_IndexParamBuffMemType,
                      &memTypeCfg);

  if (eError != OMX_ErrorNone)
  {
    ERROR ("failed to set memory Type at input port\n");
  }

  /* Setting Memory type at output port to Raw Memory */
  OMX_INIT_PARAM (&memTypeCfg);
  memTypeCfg.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX;
  memTypeCfg.eBufMemoryType = OMX_BUFFER_MEMORY_DEFAULT;
  eError =
    OMX_SetParameter (pAppData->pScHandle, OMX_TI_IndexParamBuffMemType,
                      &memTypeCfg);

  if (eError != OMX_ErrorNone)
  {
    ERROR ("failed to set memory Type at output port\n");
  }

  /* set input height/width and color format */
  OMX_INIT_PARAM (&paramPort);
  paramPort.nPortIndex = OMX_VFPC_INPUT_PORT_START_INDEX;

  OMX_GetParameter (pAppData->pScHandle, OMX_IndexParamPortDefinition,
                    &paramPort);
  paramPort.nPortIndex = OMX_VFPC_INPUT_PORT_START_INDEX;
  paramPort.format.video.nFrameWidth = pAppData->nWidth;
  paramPort.format.video.nFrameHeight = pAppData->nHeight;

  /* Scalar is connceted to H264 decoder, whose stride is different than width*/
  paramPort.format.video.nStride = pAppData->nDecStride;
  paramPort.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
  paramPort.format.video.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
  paramPort.nBufferSize = (paramPort.format.video.nStride *
                           pAppData->nHeight * 3) >> 1;

  paramPort.nBufferAlignment = 0;
  paramPort.bBuffersContiguous = 0;
  paramPort.nBufferCountActual = IL_CLIENT_SCALAR_INPUT_BUFFER_COUNT;
  printf ("set input port params (width = %u, height = %u) \n",
          (unsigned int) pAppData->nWidth, (unsigned int) pAppData->nHeight);
  eError = OMX_SetParameter (pAppData->pScHandle, OMX_IndexParamPortDefinition,
                    &paramPort);
  if(eError != OMX_ErrorNone)
  {
    ERROR(" Invalid INPUT color formats for Scalar \n");
	OMX_FreeHandle (pAppData->pDisHandle);    
	return eError;
  }

  /* set output height/width and color format */
  OMX_INIT_PARAM (&paramPort);
  paramPort.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX;
  OMX_GetParameter (pAppData->pScHandle, OMX_IndexParamPortDefinition,
                    &paramPort);

  paramPort.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX;
  paramPort.format.video.nFrameWidth = pAppData->nWidth;
  paramPort.format.video.nFrameHeight = pAppData->nHeight;
  paramPort.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
  paramPort.format.video.eColorFormat = OMX_COLOR_FormatYCbYCr;
  paramPort.nBufferAlignment = 0;
  paramPort.bBuffersContiguous = 0;
  paramPort.nBufferCountActual = IL_CLIENT_SCALAR_OUTPUT_BUFFER_COUNT;
  /* scalar buffer pitch should be multiple of 16 */
  paramPort.format.video.nStride = ((pAppData->nWidth + 15) & 0xfffffff0) * 2;

    if (1 == pAppData->displayId) {
    /*For the case of On-chip HDMI as display device*/
    paramPort.format.video.nFrameWidth = DISPLAY_WIDTH;
    paramPort.format.video.nFrameHeight = DISPLAY_HEIGHT;
    paramPort.format.video.nStride = DISPLAY_HEIGHT * 2;
  }
    
    if (2 == pAppData->displayId) {
    /*For the case of On-chip HDMI as display device*/
    paramPort.format.video.nFrameWidth  = SD_DISPLAY_WIDTH;
    paramPort.format.video.nFrameHeight = SD_DISPLAY_HEIGHT;
    paramPort.format.video.nStride      = SD_DISPLAY_WIDTH * 2;
  }
  
  paramPort.nBufferSize =
    paramPort.format.video.nStride * paramPort.format.video.nFrameHeight;

  printf ("set output port params (width = %d, height = %d) \n",
          (int) paramPort.format.video.nFrameWidth,
          (int) paramPort.format.video.nFrameHeight);

  eError = OMX_SetParameter (pAppData->pScHandle, OMX_IndexParamPortDefinition,
                    &paramPort);
  if(eError != OMX_ErrorNone)
  {
    ERROR(" Invalid OUTPUT color formats for Scalar \n");
	OMX_FreeHandle (pAppData->pDisHandle);    
	return eError;
  }

  /* set number of channles */
  printf ("set number of channels \n");

  OMX_INIT_PARAM (&sNumChPerHandle);
  sNumChPerHandle.nNumChannelsPerHandle = 1;
  eError =
    OMX_SetParameter (pAppData->pScHandle,
                      (OMX_INDEXTYPE) OMX_TI_IndexParamVFPCNumChPerHandle,
                      &sNumChPerHandle);
  if (eError != OMX_ErrorNone)
  {
    ERROR ("failed to set num of channels\n");
  }

  /* set VFPC input and output resolution information */
  printf ("set input resolution \n");

  OMX_INIT_PARAM (&chResolution);
  chResolution.Frm0Width = pAppData->nWidth;
  chResolution.Frm0Height = pAppData->nHeight;
  chResolution.Frm0Pitch = pAppData->nDecStride;
  chResolution.Frm1Width = 0;
  chResolution.Frm1Height = 0;
  chResolution.Frm1Pitch = 0;
  chResolution.FrmStartX = CROP_START_X;
  chResolution.FrmStartY = CROP_START_Y;
  chResolution.FrmCropWidth = CROPPED_WIDTH;
  chResolution.FrmCropHeight = CROPPED_HEIGHT;
  chResolution.eDir = OMX_DirInput;
  chResolution.nChId = 0;

  eError =
    OMX_SetConfig (pAppData->pScHandle,
                   (OMX_INDEXTYPE) OMX_TI_IndexConfigVidChResolution,
                   &chResolution);
  if (eError != OMX_ErrorNone)
  {
    ERROR ("failed to set input channel resolution\n");
  }

  printf ("set output resolution \n");
  OMX_INIT_PARAM (&chResolution);
  chResolution.Frm0Width = CROPPED_WIDTH; 
  chResolution.Frm0Height = CROPPED_HEIGHT;
  chResolution.Frm0Pitch = ((CROPPED_WIDTH + 15) & 0xfffffff0) * 2;
  chResolution.Frm1Width = 0;
  chResolution.Frm1Height = 0;
  chResolution.Frm1Pitch = 0;
  chResolution.FrmStartX = 0;
  chResolution.FrmStartY = 0;
  chResolution.FrmCropWidth = 0;
  chResolution.FrmCropHeight = 0;
  chResolution.eDir = OMX_DirOutput;
  chResolution.nChId = 0;

   if (1 == pAppData->displayId) {
    /* on secondary display, it is scaled to display size */  
    chResolution.Frm0Width  = DISPLAY_WIDTH;
    chResolution.Frm0Height = DISPLAY_HEIGHT;
    chResolution.Frm0Pitch  = DISPLAY_WIDTH * 2;  
  }

   if (2 == pAppData->displayId) {
    /* on secondary display, it is scaled to display size */  
    chResolution.Frm0Width  = SD_DISPLAY_WIDTH; 
    chResolution.Frm0Height = SD_DISPLAY_HEIGHT;
    chResolution.Frm0Pitch  = SD_DISPLAY_WIDTH  * 2;  
  }

  eError =
    OMX_SetConfig (pAppData->pScHandle,
                   (OMX_INDEXTYPE) OMX_TI_IndexConfigVidChResolution,
                   &chResolution);
  if (eError != OMX_ErrorNone)
  {
    ERROR ("failed to set output channel resolution\n");
  }

  /* disable algo bypass mode */
  OMX_INIT_PARAM (&algEnable);
  algEnable.nPortIndex = 0;
  algEnable.nChId = 0;
  algEnable.bAlgBypass = 0;

  eError =
    OMX_SetConfig (pAppData->pScHandle,
                   (OMX_INDEXTYPE) OMX_TI_IndexConfigAlgEnable, &algEnable);
  if (eError != OMX_ErrorNone)
    ERROR ("failed to disable algo by pass mode\n");

  return (eError);
}

/* ========================================================================== */
/**
* IL_ClientSetScalarParams() : Function to fill the port definition 
* structures and call the Set_Parameter function on to the scalar
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

OMX_ERRORTYPE IL_ClientSetNfParams (IL_Client *pAppData)
{
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  OMX_PARAM_BUFFER_MEMORYTYPE memTypeCfg;
  OMX_PARAM_PORTDEFINITIONTYPE paramPort;
  OMX_PARAM_VFPC_NUMCHANNELPERHANDLE sNumChPerHandle;
  OMX_CONFIG_ALG_ENABLE algEnable;
  OMX_CONFIG_VIDCHANNEL_RESOLUTION chResolution;

  OMX_INIT_PARAM (&memTypeCfg);
  memTypeCfg.nPortIndex = OMX_VFPC_INPUT_PORT_START_INDEX;
  memTypeCfg.eBufMemoryType = OMX_BUFFER_MEMORY_DEFAULT;
  eError =
    OMX_SetParameter (pAppData->pNfHandle, OMX_TI_IndexParamBuffMemType,
                      &memTypeCfg);

  if (eError != OMX_ErrorNone)
  {
    ERROR ("failed to set memory Type at input port\n");
  }

  /* Setting Memory type at output port to Raw Memory */
  OMX_INIT_PARAM (&memTypeCfg);
  memTypeCfg.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX;
  memTypeCfg.eBufMemoryType = OMX_BUFFER_MEMORY_DEFAULT;
  eError =
    OMX_SetParameter (pAppData->pNfHandle, OMX_TI_IndexParamBuffMemType,
                      &memTypeCfg);

  if (eError != OMX_ErrorNone)
  {
    ERROR ("failed to set memory Type at output port\n");
  }

  /* set input height/width and color format */
  OMX_INIT_PARAM (&paramPort);
  paramPort.nPortIndex = OMX_VFPC_INPUT_PORT_START_INDEX;

  OMX_GetParameter (pAppData->pNfHandle, OMX_IndexParamPortDefinition,
                    &paramPort);
  paramPort.nPortIndex = OMX_VFPC_INPUT_PORT_START_INDEX;
  paramPort.format.video.nFrameWidth  = SD_DISPLAY_WIDTH;
  paramPort.format.video.nFrameHeight = SD_DISPLAY_HEIGHT;

  /* Scalar is connceted to H264 decoder, whose stride is different than width*/
  paramPort.format.video.nStride = SD_DISPLAY_WIDTH * 2;
  paramPort.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
  paramPort.format.video.eColorFormat = OMX_COLOR_FormatYCbYCr;
  paramPort.nBufferSize = (paramPort.format.video.nStride *
                           paramPort.format.video.nFrameHeight * 2);

  paramPort.nBufferAlignment = 0;
  paramPort.bBuffersContiguous = 0;
  paramPort.nBufferCountActual = IL_CLIENT_NF_INPUT_BUFFER_COUNT;
  printf ("set input port params (width = %u, height = %u) \n",
          (unsigned int) pAppData->nWidth, (unsigned int) pAppData->nHeight);
  OMX_SetParameter (pAppData->pNfHandle, OMX_IndexParamPortDefinition,
                    &paramPort);

  /* set output height/width and color format */
  OMX_INIT_PARAM (&paramPort);
  paramPort.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX;
  OMX_GetParameter (pAppData->pNfHandle, OMX_IndexParamPortDefinition,
                    &paramPort);

  paramPort.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX;
  paramPort.format.video.nFrameWidth  = SD_DISPLAY_WIDTH;
  paramPort.format.video.nFrameHeight = SD_DISPLAY_HEIGHT;
  paramPort.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
  paramPort.format.video.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
  paramPort.nBufferAlignment = 0;
  paramPort.bBuffersContiguous = 0;
  paramPort.nBufferCountActual = IL_CLIENT_NF_OUTPUT_BUFFER_COUNT;
  /* scalar buffer pitch should be multiple of 16 */
  paramPort.format.video.nStride = SD_DISPLAY_WIDTH;

  paramPort.nBufferSize =
   (paramPort.format.video.nStride * paramPort.format.video.nFrameHeight * 3 )>> 1;

  printf ("set output port params (width = %d, height = %d) \n",
          (int) paramPort.format.video.nFrameWidth,
          (int) paramPort.format.video.nFrameHeight);

  OMX_SetParameter (pAppData->pNfHandle, OMX_IndexParamPortDefinition,
                    &paramPort);

  /* set number of channles */
  printf ("set number of channels \n");

  OMX_INIT_PARAM (&sNumChPerHandle);
  sNumChPerHandle.nNumChannelsPerHandle = 1;
  eError =
    OMX_SetParameter (pAppData->pNfHandle,
                      (OMX_INDEXTYPE) OMX_TI_IndexParamVFPCNumChPerHandle,
                      &sNumChPerHandle);
  if (eError != OMX_ErrorNone)
  {
    ERROR ("failed to set num of channels\n");
  }

  /* set VFPC input and output resolution information */
  printf ("set input resolution \n");

  OMX_INIT_PARAM (&chResolution);
  chResolution.Frm0Width =  SD_DISPLAY_WIDTH;
  chResolution.Frm0Height = SD_DISPLAY_HEIGHT;
  chResolution.Frm0Pitch =  SD_DISPLAY_WIDTH * 2;
  chResolution.Frm1Width = 0;
  chResolution.Frm1Height = 0;
  chResolution.Frm1Pitch = 0;
  chResolution.FrmStartX = 0;
  chResolution.FrmStartY = 0;
  chResolution.FrmCropWidth = 0;
  chResolution.FrmCropHeight = 0;
  chResolution.eDir = OMX_DirInput;
  chResolution.nChId = 0;

  eError =
    OMX_SetConfig (pAppData->pNfHandle,
                   (OMX_INDEXTYPE) OMX_TI_IndexConfigVidChResolution,
                   &chResolution);
  if (eError != OMX_ErrorNone)
  {
    ERROR ("failed to set input channel resolution\n");
  }

  printf ("set output resolution \n");
  OMX_INIT_PARAM (&chResolution);
  chResolution.Frm0Width  = SD_DISPLAY_WIDTH;
  chResolution.Frm0Height = SD_DISPLAY_HEIGHT;
  chResolution.Frm0Pitch  = SD_DISPLAY_WIDTH;
  chResolution.Frm1Width = 0;
  chResolution.Frm1Height = 0;
  chResolution.Frm1Pitch = 0;
  chResolution.FrmStartX = 0;
  chResolution.FrmStartY = 0;
  chResolution.FrmCropWidth = 0;
  chResolution.FrmCropHeight = 0;
  chResolution.eDir = OMX_DirOutput;
  chResolution.nChId = 0;

  eError =
    OMX_SetConfig (pAppData->pNfHandle,
                   (OMX_INDEXTYPE) OMX_TI_IndexConfigVidChResolution,
                   &chResolution);
  if (eError != OMX_ErrorNone)
  {
    ERROR ("failed to set output channel resolution\n");
  }

  /* disable algo bypass mode */
  OMX_INIT_PARAM (&algEnable);
  algEnable.nPortIndex = 0;
  algEnable.nChId = 0;
  algEnable.bAlgBypass = 0;

  eError =
    OMX_SetConfig (pAppData->pNfHandle,
                   (OMX_INDEXTYPE) OMX_TI_IndexConfigAlgEnable, &algEnable);
  if (eError != OMX_ErrorNone)
    ERROR ("failed to disable algo by pass mode\n");

  return (eError);
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
  //OMX_PARAM_VFPC_NUMCHANNELPERHANDLE sNumChPerHandle;
  OMX_PARAM_VFDC_DRIVERINSTID driverId;
  OMX_PARAM_VFDC_CREATEMOSAICLAYOUT mosaicLayout;
  OMX_CONFIG_VFDC_MOSAICLAYOUT_PORT2WINMAP port2Winmap;

  OMX_INIT_PARAM (&paramPort);

  /* set input height/width and color format */
  paramPort.nPortIndex = OMX_VFDC_INPUT_PORT_START_INDEX;
  OMX_GetParameter (pAppData->pDisHandle, OMX_IndexParamPortDefinition,
                    &paramPort);
  paramPort.nPortIndex = OMX_VFDC_INPUT_PORT_START_INDEX;
  paramPort.format.video.nFrameWidth = CROPPED_WIDTH;
  paramPort.format.video.nFrameHeight = CROPPED_HEIGHT;
  paramPort.format.video.nStride = ((CROPPED_WIDTH + 15) & 0xfffffff0) * 2;
  paramPort.nBufferCountActual = IL_CLIENT_DISPLAY_INPUT_BUFFER_COUNT;
  paramPort.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
  paramPort.format.video.eColorFormat = OMX_COLOR_FormatYCbYCr;

   if (1 == pAppData->displayId) {
    /* on secondary display, it is scaled to display size */  
    paramPort.format.video.nFrameWidth = DISPLAY_WIDTH;
    paramPort.format.video.nFrameHeight = DISPLAY_HEIGHT;
    paramPort.format.video.nStride = DISPLAY_WIDTH * 2;  
  }
   
   if (2 == pAppData->displayId) {
    /* on secondary display, it is scaled to display size */  
    paramPort.format.video.nFrameWidth  = SD_WIDTH;
    paramPort.format.video.nFrameHeight = SD_DISPLAY_HEIGHT;
    paramPort.format.video.nStride      = SD_DISPLAY_WIDTH;  
    paramPort.format.video.eColorFormat = OMX_COLOR_FormatYUV420SemiPlanar;
  }
  
  paramPort.nBufferSize = paramPort.format.video.nStride * paramPort.format.video.nFrameHeight;
  
  printf ("Buffer Size computed: %d\n", (int) paramPort.nBufferSize);
  printf ("set input port params (width = %d, height = %d) \n",
          (int) pAppData->nWidth, (int) pAppData->nHeight);
  
  eError =
    OMX_SetParameter (pAppData->pDisHandle, OMX_IndexParamPortDefinition,
                    &paramPort);
  if(eError != OMX_ErrorNone)
  {
    ERROR("failed to set display params\n");
	OMX_FreeHandle (pAppData->pDisHandle);    
	return eError;
  }
  /* --------------------------------------------------------------------------*
     Supported display IDs by VFDC and DC are below The names will be renamed in
     future releases as some of the driver names & interfaces will be changed in
     future @ param OMX_VIDEO_DISPLAY_ID_HD0: 422P On-chip HDMI @ param
     OMX_VIDEO_DISPLAY_ID_HD1: 422P HDDAC component output @ param
     OMX_VIDEO_DISPLAY_ID_SD0: 420T/422T SD display (NTSC): Not supported yet.
     ------------------------------------------------------------------------ */

  /* set the parameter to the disaply component to 1080P @60 mode */
  OMX_INIT_PARAM (&driverId);
  /* Configured to use on-chip HDMI */
  driverId.nDrvInstID = OMX_VIDEO_DISPLAY_ID_HD0;
  driverId.eDispVencMode = OMX_DC_MODE_1080P_60;

  if (1 == pAppData->displayId)  {
   driverId.nDrvInstID = OMX_VIDEO_DISPLAY_ID_HD1;
   /* for custom display, it will be set differently */
   driverId.eDispVencMode = DISPLAY_VENC_MODE;
  }
  
  if (2 == pAppData->displayId)  {
   driverId.nDrvInstID = OMX_VIDEO_DISPLAY_ID_SD0;
   /* for custom display, it will be set differently */
   driverId.eDispVencMode = OMX_DC_MODE_NTSC;
  }

  eError =
    OMX_SetParameter (pAppData->pDisHandle,
                      (OMX_INDEXTYPE) OMX_TI_IndexParamVFDCDriverInstId,
                      &driverId);
  if (eError != OMX_ErrorNone)
  {
    ERROR ("failed to set driver mode to 1080P@60\n");
  }

  /* set the parameter to the disaply controller component to 1080P @60 mode */
  OMX_INIT_PARAM (&driverId);
  /* Configured to use on-chip HDMI */
  driverId.nDrvInstID = OMX_VIDEO_DISPLAY_ID_HD0;
  driverId.eDispVencMode = OMX_DC_MODE_1080P_60;

  if (1 == pAppData->displayId)  {
   driverId.nDrvInstID = OMX_VIDEO_DISPLAY_ID_HD1;
   /* for custom display, it will be set differently */
   driverId.eDispVencMode = DISPLAY_VENC_MODE;
  }

  if (2 == pAppData->displayId)  {
   driverId.nDrvInstID = OMX_VIDEO_DISPLAY_ID_SD0;
   /* for custom display, it will be set differently */
   driverId.eDispVencMode = OMX_DC_MODE_NTSC;
  }

  eError =
    OMX_SetParameter (pAppData->pctrlHandle,
                      (OMX_INDEXTYPE) OMX_TI_IndexParamVFDCDriverInstId,
                      &driverId);
  if (eError != OMX_ErrorNone)
  {
    ERROR ("failed to set driver mode to 1080P@60\n");
  }

  if (1 == pAppData->displayId) {
   IL_ClientSetSecondaryDisplayParams(pAppData);
   }
  /* set mosaic layout info */
  if (2 != pAppData->displayId) {

  OMX_INIT_PARAM (&mosaicLayout);
  /* Configuring the first (and only) window */
  /* position of window can be changed by following cordinates, keeping in
     center by default */

  mosaicLayout.sMosaicWinFmt[0].winStartX = (HD_WIDTH - CROPPED_WIDTH) / 2;
  mosaicLayout.sMosaicWinFmt[0].winStartY = (HD_HEIGHT - CROPPED_HEIGHT) / 2;
  mosaicLayout.sMosaicWinFmt[0].winWidth = CROPPED_WIDTH;
  mosaicLayout.sMosaicWinFmt[0].winHeight = CROPPED_HEIGHT;
  mosaicLayout.sMosaicWinFmt[0].pitch[VFDC_YUV_INT_ADDR_IDX] =
    ((CROPPED_WIDTH + 15) & 0xfffffff0) * 2;
  mosaicLayout.sMosaicWinFmt[0].dataFormat = VFDC_DF_YUV422I_YVYU;
  mosaicLayout.sMosaicWinFmt[0].bpp = VFDC_BPP_BITS16;
  mosaicLayout.sMosaicWinFmt[0].priority = 0;
  mosaicLayout.nDisChannelNum = 0;
  /* Only one window in this layout, hence setting it to 1 */
  mosaicLayout.nNumWindows = 1;

  if (1 == pAppData->displayId) {
    /* For secondary Display, start the window at (0,0), since it is 
       scaled to display device size */
    mosaicLayout.sMosaicWinFmt[0].winStartX = 0;
    mosaicLayout.sMosaicWinFmt[0].winStartY = 0;
    
    /*If LCD is chosen, fir the mosaic window to the size of the LCD display*/
    mosaicLayout.sMosaicWinFmt[0].winWidth = DISPLAY_WIDTH;
    mosaicLayout.sMosaicWinFmt[0].winHeight = DISPLAY_HEIGHT;
    mosaicLayout.sMosaicWinFmt[0].pitch[VFDC_YUV_INT_ADDR_IDX] = 
                                       DISPLAY_WIDTH * 2;  
  }

  eError = OMX_SetParameter (pAppData->pDisHandle, (OMX_INDEXTYPE)
                             OMX_TI_IndexParamVFDCCreateMosaicLayout,
                             &mosaicLayout);
  if (eError != OMX_ErrorNone)
  {
    ERROR ("failed to set mosaic window parameter\n");
  }

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

  return (eError);
}

/* Nothing beyond this point */
