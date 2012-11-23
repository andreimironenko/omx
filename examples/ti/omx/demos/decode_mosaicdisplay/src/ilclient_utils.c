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
#include <omx_vswmosaic.h>
#include <OMX_TI_Index.h>
#include "OMX_TI_Video.h"


/*--------------------------- defines ----------------------------------------*/
/* Align address "a" at "b" boundary */
#define UTIL_ALIGN(a,b)  ((((uint32_t)(a)) + (b)-1) & (~((uint32_t)((b)-1))))

void usage (IL_ARGS *argsp)
{
  printf ("decode_mosaicdisplay -w <image_width> -h <image_height> -f <frame_rate> "
          "-i -j -k -l <input_files>  -g <gfx on/off> -d <0/1> -p <1-4>\n"
          "-i -j -k -l | --input           input filenames \n"
          "-w | --width           image width \n"
          "-h | --height          image height \n"
          "-f | --framerate       decode frame rate - max 60 \n"
          "-g | --gfx             gfx - 0 - off, 1 - on \n"
          "-p | --pip             number of decodes in the mosaic( 1-4) \n"
          "-d | --display_id      0 - for on-chip HDMI, 1 for LCD \n");
  printf(" This examples assumes all 4 files are of same resolution \n");        
  printf(" example : ./decode_mosaicdisplay_a8host_debug.xv5T -i sample1.h264 -j sample2.h264 -k sample3.h264 -l sample4.h264 -w 1920 -h 1080 -f 30 -g 0 -p 0 -d 0 \n");
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
  const char shortOptions[] = "i:j:k:l:w:h:f:g:p:d:";
  const struct option longOptions[] =
  {
    {"input1", required_argument, NULL, ArgID_INPUT_FILE},
    {"input2", required_argument, NULL, ArgID_INPUT_SEC_FILE},
    {"input3", required_argument, NULL, ArgID_INPUT_THIRD_FILE},
    {"input4", required_argument, NULL, ArgID_INPUT_FOURTH_FILE},    
    {"width", required_argument, NULL, ArgID_WIDTH},
    {"height", required_argument, NULL, ArgID_HEIGHT},
    {"framerate", required_argument, NULL, ArgID_FRAMERATE},
    {"gfx", required_argument, NULL, ArgID_GFX},
    {"pip", required_argument, NULL, ArgID_PIP},
    {"display_id", required_argument, NULL, ArgID_DISPLAYID},
    {0, 0, 0, 0}
  };
  char *gfxen = "fbdev enable";
  char *gfxdis = "fbdev disable";
  char *gfxoption = "fbdev disable";

  char *pipen = "pip enable";
  char *pipdis = "pip disable";
  char *pipoption = gfxdis;

  int index, infile = 0, width = 0, height = 0, framerate = 0, gfx = 0, pip = 0;
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
        strncpy (argsp->input_file[0], optarg, MAX_FILE_NAME_SIZE);
        infile = 1;
        break;
      case ArgID_INPUT_SEC_FILE:
      case 'j':
        strncpy (argsp->input_file[1], optarg, MAX_FILE_NAME_SIZE);
        infile = 1;
        break;
      case ArgID_INPUT_THIRD_FILE:
      case 'k':
        strncpy (argsp->input_file[2], optarg, MAX_FILE_NAME_SIZE);
        infile = 1;
        break;
      case ArgID_INPUT_FOURTH_FILE:
      case 'l':
        strncpy (argsp->input_file[3], optarg, MAX_FILE_NAME_SIZE);
        infile = 1;
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

      case ArgID_PIP:
      case 'p':
        argsp->pip = atoi (optarg);
        pip = 1;
        if (argsp->pip)
        {
          pipoption = pipen;
        }
        else
        {
          pipoption = pipdis;
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

  if (!infile || !framerate || !width || !height || !gfx || !pip || !display_id)
  {
    usage (argsp);
    exit (1);
  }

  printf ("input file1: %s\n", argsp->input_file[0]);
  printf ("input file2: %s\n", argsp->input_file[1]);
  printf ("input file3: %s\n", argsp->input_file[2]);
  printf ("input file4: %s\n", argsp->input_file[3]);
  printf ("width: %d\n", argsp->width);
  printf ("height: %d\n", argsp->height);
  printf ("frame_rate: %d\n", argsp->frame_rate);
  printf ("gfx: %s\n", gfxoption);
  printf ("pip: %s\n", pipoption);
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
int g_max_decode = 1;
void IL_ClientInit (IL_Client **pAppData, IL_ARGS *args)
{
  int i,j;
  IL_Client *pAppDataPtr;
  IL_CLIENT_INPORT_PARAMS *inPortParamsPtr;
  IL_CLIENT_OUTPORT_PARAMS *outPortParamsPtr;
  OMX_BOOL bUseSwMosaic;

  int width = args->width;
  int height = args->height;
  int frameRate = args->frame_rate;
  int displayId = args->display_id;
  bUseSwMosaic = (args->pip) ? OMX_TRUE: OMX_FALSE;
  
  if(bUseSwMosaic == OMX_TRUE) {
   g_max_decode = args->pip;
  }               
  
  /* Allocating data structure for IL client structure / buffer management */

  pAppDataPtr = (IL_Client *) malloc (sizeof (IL_Client));
  memset (pAppDataPtr, 0x0, sizeof (IL_Client));

  /* update the user provided parameters */
  pAppDataPtr->nHeight = height;
  pAppDataPtr->nWidth = width;
  pAppDataPtr->nFrameRate = frameRate;
  pAppDataPtr->displayId = displayId;
  
  for (i = 0; i < g_max_decode; i++) {
   /* alloacte data structure for each component used in this IL Cleint */
   pAppDataPtr->decILComp[i] =
     (IL_CLIENT_COMP_PRIVATE *) malloc (sizeof (IL_CLIENT_COMP_PRIVATE));
   memset (pAppDataPtr->decILComp[i], 0x0, sizeof (IL_CLIENT_COMP_PRIVATE));

   /* these semaphores are used for tracking the callbacks received from
      component */
   pAppDataPtr->decILComp[i]->eos = malloc (sizeof (semp_t));
   semp_init (pAppDataPtr->decILComp[i]->eos, 0);

   pAppDataPtr->decILComp[i]->done_sem = malloc (sizeof (semp_t));
   semp_init (pAppDataPtr->decILComp[i]->done_sem, 0);

   pAppDataPtr->decILComp[i]->port_sem = malloc (sizeof (semp_t));
   semp_init (pAppDataPtr->decILComp[i]->port_sem, 0);

   /* alloacte data structure for each component used in this IL Cleint */
   pAppDataPtr->scILComp[i] =
     (IL_CLIENT_COMP_PRIVATE *) malloc (sizeof (IL_CLIENT_COMP_PRIVATE));
   memset (pAppDataPtr->scILComp[i], 0x0, sizeof (IL_CLIENT_COMP_PRIVATE));

   /* these semaphores are used for tracking the callbacks received from
      component */
   pAppDataPtr->scILComp[i]->eos = malloc (sizeof (semp_t));
   semp_init (pAppDataPtr->scILComp[i]->eos, 0);

   pAppDataPtr->scILComp[i]->done_sem = malloc (sizeof (semp_t));
   semp_init (pAppDataPtr->scILComp[i]->done_sem, 0);

   pAppDataPtr->scILComp[i]->port_sem = malloc (sizeof (semp_t));
   semp_init (pAppDataPtr->scILComp[i]->port_sem, 0);
 }
  /* alloacte data structure for each component used in this IL Cleint */
  pAppDataPtr->vswmosaicILComp =
    (IL_CLIENT_COMP_PRIVATE *) malloc (sizeof (IL_CLIENT_COMP_PRIVATE));
  memset (pAppDataPtr->vswmosaicILComp, 0x0, sizeof (IL_CLIENT_COMP_PRIVATE));

  /* these semaphores are used for tracking the callbacks received from
     component */
  pAppDataPtr->vswmosaicILComp->eos = malloc (sizeof (semp_t));
  semp_init (pAppDataPtr->vswmosaicILComp->eos, 0);

  pAppDataPtr->vswmosaicILComp->done_sem = malloc (sizeof (semp_t));
  semp_init (pAppDataPtr->vswmosaicILComp->done_sem, 0);

  pAppDataPtr->vswmosaicILComp->port_sem = malloc (sizeof (semp_t));
  semp_init (pAppDataPtr->vswmosaicILComp->port_sem, 0);

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
  for (i = 0; i < g_max_decode; i++) {
   pAppDataPtr->decILComp[i]->numInport = 1;
   pAppDataPtr->decILComp[i]->numOutport = 1;
   pAppDataPtr->decILComp[i]->startOutportIndex = 1;

   pAppDataPtr->scILComp[i]->numInport = 1;
   pAppDataPtr->scILComp[i]->numOutport = 1;

   /* VFPC OMX component support max 16 input / output ports, so o/p port index
      starts at 16 */
   pAppDataPtr->scILComp[i]->startOutportIndex = OMX_VFPC_NUM_INPUT_PORTS;
  }
  
  pAppDataPtr->vswmosaicILComp->numInport  =  g_max_decode;
  pAppDataPtr->vswmosaicILComp->numOutport = 1;

  /* VSWMOSAIC OMX component support max 16 input / output ports, so o/p port index
     starts at 16 */
  pAppDataPtr->vswmosaicILComp->startOutportIndex = 
                                          OMX_VSWMOSAIC_OUTPUT_PORT_START_INDEX;

  pAppDataPtr->disILComp->numInport = 1;

  /* display does not has o/pports */
  pAppDataPtr->disILComp->numOutport = 0;
  pAppDataPtr->disILComp->startOutportIndex = 0;

  /* allocate data structure for input and output port params of IL client
     component, It is for mainitaining data structure in IL Cleint only.
     Components will have its own data structure inside omx components */
  for (i = 0; i < g_max_decode; i++) {
   pAppDataPtr->decILComp[i]->inPortParams =
     malloc (sizeof (IL_CLIENT_INPORT_PARAMS) *
             pAppDataPtr->decILComp[i]->numInport);
   memset (pAppDataPtr->decILComp[i]->inPortParams, 0x0,
           sizeof (IL_CLIENT_INPORT_PARAMS));

   pAppDataPtr->decILComp[i]->outPortParams =
     malloc (sizeof (IL_CLIENT_INPORT_PARAMS) *
             pAppDataPtr->decILComp[i]->numOutport);
   memset (pAppDataPtr->decILComp[i]->outPortParams, 0x0,
           sizeof (IL_CLIENT_OUTPORT_PARAMS));

   pAppDataPtr->scILComp[i]->inPortParams =
     malloc (sizeof (IL_CLIENT_INPORT_PARAMS) *
             pAppDataPtr->scILComp[i]->numInport);

   memset (pAppDataPtr->scILComp[i]->inPortParams, 0x0,
           sizeof (IL_CLIENT_INPORT_PARAMS));

   pAppDataPtr->scILComp[i]->outPortParams =
     malloc (sizeof (IL_CLIENT_INPORT_PARAMS) *
             pAppDataPtr->scILComp[i]->numOutport);
   memset (pAppDataPtr->scILComp[i]->outPortParams, 0x0,
           sizeof (IL_CLIENT_OUTPORT_PARAMS));
  }
  pAppDataPtr->vswmosaicILComp->inPortParams =
    malloc (sizeof (IL_CLIENT_INPORT_PARAMS) *
            pAppDataPtr->vswmosaicILComp->numInport);

  memset (pAppDataPtr->vswmosaicILComp->inPortParams, 0x0,
          sizeof (IL_CLIENT_INPORT_PARAMS) *
          pAppDataPtr->vswmosaicILComp->numInport);

  pAppDataPtr->vswmosaicILComp->outPortParams =
    malloc (sizeof (IL_CLIENT_INPORT_PARAMS) *
            pAppDataPtr->vswmosaicILComp->numOutport);
  memset (pAppDataPtr->vswmosaicILComp->outPortParams, 0x0,
          sizeof (IL_CLIENT_OUTPORT_PARAMS) *
            pAppDataPtr->vswmosaicILComp->numOutport);

  pAppDataPtr->disILComp->inPortParams =
    malloc (sizeof (IL_CLIENT_INPORT_PARAMS) *
            pAppDataPtr->disILComp->numInport);

  memset (pAppDataPtr->disILComp->inPortParams, 0x0,
          sizeof (IL_CLIENT_INPORT_PARAMS));

  /* specify some of the parameters, that will be used for initializing OMX
     component parameters */
  for (i = 0; i < g_max_decode; i++) {
     
   for (j = 0; j < pAppDataPtr->decILComp[i]->numInport; j++)
   {
     inPortParamsPtr = pAppDataPtr->decILComp[i]->inPortParams + j;
     inPortParamsPtr->nBufferCountActual = IL_CLIENT_DECODER_INPUT_BUFFER_COUNT;
     /* input buffers size for bitstream buffers, It can be smaller than this
        value , setting it for approx value */
     inPortParamsPtr->nBufferSize = pAppDataPtr->nHeight * pAppDataPtr->nWidth;
     /* this pipe is used for taking buffers from file read thread */
     pipe ((int *) inPortParamsPtr->ipBufPipe);
   }
   for (j = 0; j < pAppDataPtr->decILComp[i]->numOutport; j++)
   {
     outPortParamsPtr = pAppDataPtr->decILComp[i]->outPortParams + j;
     outPortParamsPtr->nBufferCountActual = IL_CLIENT_DECODER_OUTPUT_BUFFER_COUNT;
     /* H264 decoder uses padding on both sides, as well requires 128 byte
        alignment so this value is calcualtes as follows, decoder o/p is always
        YUV420 packet semi planner so * 1.5 */

     outPortParamsPtr->nBufferSize =
       (UTIL_ALIGN ((pAppDataPtr->nWidth + (2 * H264_PADX)), 128) *
        ((((pAppDataPtr->nHeight + 15) & 0xfffffff0) + (4 * H264_PADY))) * 3) >> 1;

     /* This pipe is used if output is directed to file write thread, in this
        example, file write is not used */
     pipe ((int *) outPortParamsPtr->opBufPipe);
   }
   /* each componet will have local pipe to take bufffes from other component or 
      its own consumed buffer, so that it can be passed to other conected
      components */
   pipe ((int *) pAppDataPtr->decILComp[i]->localPipe);

   for (j = 0; j < pAppDataPtr->scILComp[i]->numInport; j++)
   {
     inPortParamsPtr = pAppDataPtr->scILComp[i]->inPortParams + j;
     inPortParamsPtr->nBufferCountActual = IL_CLIENT_SCALAR_INPUT_BUFFER_COUNT;
     /* since input of scalar is connected to output of decoder, size is same as 
        decoder o/p buffers */
     inPortParamsPtr->nBufferSize =
       (UTIL_ALIGN ((pAppDataPtr->nWidth + (2 * H264_PADX)), 128) *
        ((pAppDataPtr->nHeight + (4 * H264_PADY))) * 3) >> 1;

     /* this pipe will not be used in this application, as scalar does not read
        / write into file */
     pipe ((int *) inPortParamsPtr->ipBufPipe);
   }
   for (j = 0; j < pAppDataPtr->scILComp[i]->numOutport; j++)
   {
     outPortParamsPtr = pAppDataPtr->scILComp[i]->outPortParams + j;
     outPortParamsPtr->nBufferCountActual = IL_CLIENT_SCALAR_OUTPUT_BUFFER_COUNT;
     /* scalar is configured for YUV 422 output, so buffer size is calculates as 
        follows */
     outPortParamsPtr->nBufferSize =
       SWMOSAIC_WINDOW_WIDTH * SWMOSAIC_WINDOW_HEIGHT * 2;


      if (1 == pAppDataPtr->displayId) {
       /* configure the buffer size to that of the display size, for custom
          display this can be used to change width and height */
       outPortParamsPtr->nBufferSize = (DISPLAY_HEIGHT/2) * (DISPLAY_WIDTH/2) * 2;      
     }

     /* this pipe will not be used in this application, as scalar does not read
        / write into file */
     pipe ((int *) outPortParamsPtr->opBufPipe);
   }

   /* each componet will have local pipe to take bufffes from other component or 
      its own consumed buffer, so that it can be passed to other conected
      components */
   pipe ((int *) pAppDataPtr->scILComp[i]->localPipe);
 }
 
  for (i = 0; i < pAppDataPtr->vswmosaicILComp->numInport; i++)
  {
    inPortParamsPtr = pAppDataPtr->vswmosaicILComp->inPortParams + i;
    inPortParamsPtr->nBufferCountActual = IL_CLIENT_VSWMOSAIC_INPUT_BUFFER_COUNT;
    /* since input of vswmosaic is connected to output of scalar, size is same as 
       scalar o/p buffers */
    inPortParamsPtr->nBufferSize = UTIL_ALIGN (SWMOSAIC_WINDOW_HEIGHT * 
                                               SWMOSAIC_WINDOW_WIDTH * 2, 32);
      if (1 == pAppDataPtr->displayId) {
       /* configure the buffer size to that of the display size (2x2), for custom
          display this can be used to change width and height */
       inPortParamsPtr->nBufferSize = (DISPLAY_HEIGHT/2) * (DISPLAY_WIDTH/2) * 2;      
     }
                                               

    /* this pipe will not be used in this application, as vswmosaic does not read/
       write into file */
    pipe ((int *) inPortParamsPtr->ipBufPipe);
  }
  for (i = 0; i < pAppDataPtr->vswmosaicILComp->numOutport; i++)
  {
    outPortParamsPtr = pAppDataPtr->vswmosaicILComp->outPortParams + i;
    outPortParamsPtr->nBufferCountActual = IL_CLIENT_SCALAR_OUTPUT_BUFFER_COUNT;
    /* vswmosaic is configured for YUV 422 output, so buffer size is calculates as 
       follows */
    outPortParamsPtr->nBufferSize =
      UTIL_ALIGN (HD_WIDTH * HD_HEIGHT * 2, 128);

     if (1 == pAppDataPtr->displayId) {
      /* configure the buffer size to that of the display size, for custom
         display this can be used to change width and height */
      outPortParamsPtr->nBufferSize = UTIL_ALIGN (DISPLAY_HEIGHT * 
                                                  DISPLAY_WIDTH * 2, 128);
    }
    /* this pipe will not be used in this application, as vswmosaic does not read
       / write into file */
    pipe ((int *) outPortParamsPtr->opBufPipe);
  }
  /* each componet will have local pipe to take bufffes from other component or 
     its own consumed buffer, so that it can be passed to other conected
     components */
  pipe ((int *) pAppDataPtr->vswmosaicILComp->localPipe);

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
  int i, j;
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

  close ((int) pAppData->vswmosaicILComp->localPipe);

  for (i = 0; i < pAppData->vswmosaicILComp->numInport; i++)
  {
    inPortParamsPtr = pAppData->vswmosaicILComp->inPortParams + i;
    /* this pipe will not be used in this application, as scalar does not read
       / write into file */
    close ((int) inPortParamsPtr->ipBufPipe);
  }
  for (i = 0; i < pAppData->vswmosaicILComp->numOutport; i++)
  {
    outPortParamsPtr = pAppData->vswmosaicILComp->outPortParams + i;
    /* this pipe is not used in this application, as vswmosaic does not read /
       write into file */
    close ((int) outPortParamsPtr->opBufPipe);
  }
  for (i = 0; i < g_max_decode; i++) {

   close ((int) pAppData->scILComp[i]->localPipe);

   for (j = 0; j < pAppData->scILComp[i]->numInport; j++)
   {
     inPortParamsPtr = pAppData->scILComp[i]->inPortParams + j;
     /* this pipe is not used in this application, as scalar does not read /
        write into file */
     close ((int) inPortParamsPtr->ipBufPipe);
   }
   for (j = 0; j < pAppData->scILComp[i]->numOutport; j++)
   {
     outPortParamsPtr = pAppData->scILComp[i]->outPortParams + j;
     /* this pipe is not used in this application, as scalar does not read /
        write into file */
     close ((int) outPortParamsPtr->opBufPipe);
   }

   close ((int) pAppData->decILComp[i]->localPipe);

   for (j = 0; j < pAppData->decILComp[i]->numInport; j++)
   {
     inPortParamsPtr = pAppData->decILComp[i]->inPortParams + j;
     close ((int) inPortParamsPtr->ipBufPipe);
   }
   for (j = 0; j < pAppData->decILComp[i]->numOutport; j++)
   {
     outPortParamsPtr = pAppData->decILComp[i]->outPortParams + j;
     /* This pipe is used if output is directed to file write thread, in this
        example, file write is not used */
     close ((int) outPortParamsPtr->opBufPipe);
   }

   free (pAppData->scILComp[i]->inPortParams);

   free (pAppData->scILComp[i]->outPortParams);

   free (pAppData->decILComp[i]->inPortParams);

   free (pAppData->decILComp[i]->outPortParams);
  } 
  
  free (pAppData->disILComp->inPortParams);

  free (pAppData->vswmosaicILComp->inPortParams);

  free (pAppData->vswmosaicILComp->outPortParams);

  /* these semaphores are used for tracking the callbacks received from
     component */
  semp_deinit (pAppData->vswmosaicILComp->eos);
  free (pAppData->vswmosaicILComp->eos);

  semp_deinit (pAppData->vswmosaicILComp->done_sem);
  free (pAppData->vswmosaicILComp->done_sem);

  semp_deinit (pAppData->vswmosaicILComp->port_sem);
  free (pAppData->vswmosaicILComp->port_sem);

  /* these semaphores are used for tracking the callbacks received from
     component */
  for (i = 0; i < g_max_decode; i++) {
   semp_deinit (pAppData->scILComp[i]->eos);
   free (pAppData->scILComp[i]->eos);

   semp_deinit (pAppData->scILComp[i]->done_sem);
   free (pAppData->scILComp[i]->done_sem);

   semp_deinit (pAppData->scILComp[i]->port_sem);
   free (pAppData->scILComp[i]->port_sem);


   semp_deinit (pAppData->decILComp[i]->eos);
   free (pAppData->decILComp[i]->eos);

   semp_deinit (pAppData->decILComp[i]->done_sem);
   free (pAppData->decILComp[i]->done_sem);

   semp_deinit (pAppData->decILComp[i]->port_sem);
   free (pAppData->decILComp[i]->port_sem);

   free (pAppData->decILComp[i]);

   free (pAppData->scILComp[i]);
 }
  semp_deinit (pAppData->disILComp->eos);
  free (pAppData->disILComp->eos);

  semp_deinit (pAppData->disILComp->done_sem);
  free (pAppData->disILComp->done_sem);

  semp_deinit (pAppData->disILComp->port_sem);
  free (pAppData->disILComp->port_sem);

  free (pAppData->vswmosaicILComp);

  free (pAppData->disILComp);

  for (i = 0; i < g_max_decode; i++) {
   if (pAppData->eCompressionFormat == OMX_VIDEO_CodingAVC) {
    if(pAppData->pc[i].readBuf) {
     free(pAppData->pc[i].readBuf);
    }
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

unsigned int IL_ClientFillBitStreamData (IL_CLIENT_DEC_THREAD_ARGS *pDecArgs,
                                         OMX_BUFFERHEADERTYPE *pBuf)
{
  unsigned int dataRead = 0;
  int frameSize = 0;
  IL_CLIENT_COMP_PRIVATE *decILComp = NULL;
  decILComp = ((IL_Client *) (pDecArgs->ptrAppData))->decILComp[pDecArgs->instId];
  /* update the parser buffer, with the decoder input buffer */
  pDecArgs->ptrAppData->pc[pDecArgs->instId].outBuf.ptr = pBuf->pBuffer;
  pDecArgs->ptrAppData->pc[pDecArgs->instId].outBuf.bufsize = decILComp->inPortParams->nBufferSize;
  pDecArgs->ptrAppData->pc[pDecArgs->instId].outBuf.bufused = 0;

  /* Get the size of one frame at a time */
  frameSize = Decode_GetNextFrameSize (&(pDecArgs->ptrAppData->pc[pDecArgs->instId]));


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

OMX_ERRORTYPE IL_ClientDecUseInitialInputResources (IL_CLIENT_DEC_THREAD_ARGS *pDecArgs)
{

  OMX_ERRORTYPE err = OMX_ErrorNone;
  unsigned int i = 0;
  int frameSize = 0;
  IL_CLIENT_COMP_PRIVATE *decILComp = NULL;
  decILComp = ((IL_Client *) (pDecArgs->ptrAppData))->decILComp[pDecArgs->instId];

  /* Give input buffers to component which is limited by no of input buffers
     available. Rest of the data will be read on the callback from input data
     read thread */
  for (i = 0; i < decILComp->inPortParams->nBufferCountActual; i++)
  {
    pDecArgs->ptrAppData->pc[pDecArgs->instId].outBuf.ptr     = decILComp->inPortParams->pInBuff[i]->pBuffer;
    pDecArgs->ptrAppData->pc[pDecArgs->instId].outBuf.bufsize = decILComp->inPortParams->nBufferSize;
    pDecArgs->ptrAppData->pc[pDecArgs->instId].outBuf.bufused = 0;

    /* Get the size of one frame at a time */
    frameSize = Decode_GetNextFrameSize (&(pDecArgs->ptrAppData->pc[pDecArgs->instId]));

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
* IL_ClientPipWindowDataUpdate() : This function gives initially all
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

OMX_ERRORTYPE IL_ClientPipUseInitialInputResources (IL_Client *pAppdata)
{

  OMX_ERRORTYPE err = OMX_ErrorNone;
  IL_CLIENT_COMP_PRIVATE *vswmosaicILComp = NULL;
  vswmosaicILComp = ((IL_Client *) pAppdata)->vswmosaicILComp;
  int line[1280];
  int cols, rows, i;
  char *dst;
  IL_CLIENT_INPORT_PARAMS *inPortParamsPtr = pAppdata->vswmosaicILComp->
                                             inPortParams + 
                                             IL_CLIENT_VSWMOSAIC_PORT1;

  /* Give input buffers to component which is limited by no of input buffers
     available. Rest of the data will be read on the callback from input data
     read thread */

  inPortParamsPtr->pInBuff[0]->nFilledLen = SWMOSAIC_WINDOW_WIDTH*2*SWMOSAIC_WINDOW_HEIGHT;
  inPortParamsPtr->pInBuff[0]->nOffset = 0;
  inPortParamsPtr->pInBuff[0]->nAllocLen = SWMOSAIC_WINDOW_WIDTH*2*SWMOSAIC_WINDOW_HEIGHT;
  inPortParamsPtr->pInBuff[0]->nInputPortIndex = IL_CLIENT_VSWMOSAIC_PORT1;

  /* Pass the input buffer to the component */
  cols = SWMOSAIC_WINDOW_WIDTH;
  for (i=0; i<(cols/2); i++){
    line [i] = 0x80EF80EF;
  }
  rows = SWMOSAIC_WINDOW_HEIGHT;
  for (i=0; i<rows; i++){
    dst = (char *) inPortParamsPtr->pInBuff[0]->pBuffer + ( SWMOSAIC_WINDOW_WIDTH * 2 * i );
    memcpy ( dst, line, sizeof(line));
  }
  
  err = OMX_EmptyThisBuffer (vswmosaicILComp->handle,
                             inPortParamsPtr->pInBuff[0]);

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
OMX_ERRORTYPE IL_ClientSetDecodeParams (IL_Client *pAppData, unsigned int instId)
{
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  OMX_HANDLETYPE pHandle = pAppData->pDecHandle[instId];
  OMX_PORT_PARAM_TYPE portInit;
  OMX_PARAM_PORTDEFINITIONTYPE pInPortDef, pOutPortDef;
  OMX_VIDEO_PARAM_STATICPARAMS tStaticParam;
  OMX_PARAM_COMPPORT_NOTIFYTYPE pNotifyType;
  
  if (!pHandle)
  {
    eError = OMX_ErrorBadParameter;
    goto EXIT;
  }


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
  pInPortDef.format.video.cMIMEType = "H264";
  /* set the width and height, used for buffer size calculation */
  pInPortDef.format.video.nFrameWidth = pAppData->nWidth;
  pInPortDef.format.video.nFrameHeight = pAppData->nHeight;
  /* for bitstream buffer stride is not a valid parameter */
  pInPortDef.format.video.nStride = -1;
  /* component supports only frame based processing */

  /* bitrate does not matter for decoder */
  /* as per openmax frame rate is in Q16 format */
  pInPortDef.format.video.xFramerate = (pAppData->nFrameRate) << 16;
  /* input port would receive H264 stream */
  pInPortDef.format.video.eCompressionFormat = OMX_VIDEO_CodingAVC;
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
    ((((pAppData->nWidth + (2 * H264_PADX) +
        127) & 0xFFFFFF80) * ((((pAppData->nHeight + 15) & 0xfffffff0) + (4 * H264_PADY))) * 3) >> 1);
  pOutPortDef.bEnabled = OMX_TRUE;
  pOutPortDef.bPopulated = OMX_FALSE;
  /* currently component alloactes contigous buffers with 128 alignment, these
     values are do't care */

  /* OMX_VIDEO_PORTDEFINITION values for output port */
  pOutPortDef.format.video.cMIMEType = "H264";
  pOutPortDef.format.video.nFrameWidth = pAppData->nWidth;
  pOutPortDef.format.video.nFrameHeight = ((pAppData->nHeight + 15) & 0xfffffff0);
  /* stride is set as buffer width */
  pOutPortDef.format.video.nStride =
    UTIL_ALIGN ((pAppData->nWidth + (2 * H264_PADX)), 128);

  /* bitrate does not matter for decoder */
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
  OMX_INIT_PARAM (&tStaticParam);
  
  tStaticParam.nPortIndex = OMX_VIDDEC_OUTPUT_PORT;
  
  eError = OMX_GetParameter (pHandle, OMX_TI_IndexParamVideoStaticParams,
                             &tStaticParam);
  /* setting I frame interval */
  printf( "level set is %d \n", (int) tStaticParam.videoStaticParams.h264DecStaticParams.presetLevelIdc);
  
  tStaticParam.videoStaticParams.h264DecStaticParams.presetLevelIdc = IH264VDEC_LEVEL42;
                         
  eError = OMX_SetParameter (pHandle, OMX_TI_IndexParamVideoStaticParams,
                             &tStaticParam);

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

OMX_ERRORTYPE IL_ClientSetScalarParams (IL_Client *pAppData, unsigned int instId)
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
    OMX_SetParameter (pAppData->pScHandle[instId], OMX_TI_IndexParamBuffMemType,
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
    OMX_SetParameter (pAppData->pScHandle[instId], OMX_TI_IndexParamBuffMemType,
                      &memTypeCfg);

  if (eError != OMX_ErrorNone)
  {
    ERROR ("failed to set memory Type at output port\n");
  }

  /* set input height/width and color format */
  OMX_INIT_PARAM (&paramPort);
  paramPort.nPortIndex = OMX_VFPC_INPUT_PORT_START_INDEX;

  OMX_GetParameter (pAppData->pScHandle[instId], OMX_IndexParamPortDefinition,
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
  OMX_SetParameter (pAppData->pScHandle[instId], OMX_IndexParamPortDefinition,
                    &paramPort);

  /* set output height/width and color format */
  OMX_INIT_PARAM (&paramPort);
  paramPort.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX;
  OMX_GetParameter (pAppData->pScHandle[instId], OMX_IndexParamPortDefinition,
                    &paramPort);

  paramPort.nPortIndex = OMX_VFPC_OUTPUT_PORT_START_INDEX;
  paramPort.format.video.nFrameWidth = SWMOSAIC_WINDOW_WIDTH; 
  paramPort.format.video.nFrameHeight = SWMOSAIC_WINDOW_HEIGHT;
  paramPort.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
  paramPort.format.video.eColorFormat = OMX_COLOR_FormatYCbYCr;
  paramPort.nBufferAlignment = 0;
  paramPort.bBuffersContiguous = 0;
  paramPort.nBufferCountActual = IL_CLIENT_SCALAR_OUTPUT_BUFFER_COUNT;
  paramPort.format.video.nStride = SWMOSAIC_WINDOW_WIDTH * 2;

    if (1 == pAppData->displayId) {
    /*For the case of On-chip HDMI as display device*/
    paramPort.format.video.nFrameWidth = DISPLAY_WIDTH / 2;
    paramPort.format.video.nFrameHeight = DISPLAY_HEIGHT / 2;
    paramPort.format.video.nStride = (DISPLAY_WIDTH / 2) * 2;
  }
  
  paramPort.nBufferSize =
    paramPort.format.video.nStride * paramPort.format.video.nFrameHeight;

  printf ("set output port params (width = %d, height = %d) \n",
          (int) paramPort.format.video.nFrameWidth, (int) paramPort.format.video.nFrameHeight);

  OMX_SetParameter (pAppData->pScHandle[instId], OMX_IndexParamPortDefinition,
                    &paramPort);

  /* set number of channles */
  printf ("set number of channels \n");

  OMX_INIT_PARAM (&sNumChPerHandle);
  sNumChPerHandle.nNumChannelsPerHandle = 1;
  eError =
    OMX_SetParameter (pAppData->pScHandle[instId],
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
  chResolution.FrmStartX = 0;
  chResolution.FrmStartY = 0;
  chResolution.FrmCropWidth = pAppData->nWidth;
  chResolution.FrmCropHeight = pAppData->nHeight;
  chResolution.eDir = OMX_DirInput;
  chResolution.nChId = 0;

  eError =
    OMX_SetConfig (pAppData->pScHandle[instId],
                   (OMX_INDEXTYPE) OMX_TI_IndexConfigVidChResolution,
                   &chResolution);
  if (eError != OMX_ErrorNone)
  {
    ERROR ("failed to set input channel resolution\n");
  }

  printf ("set output resolution \n");
  OMX_INIT_PARAM (&chResolution);
  chResolution.Frm0Width = SWMOSAIC_WINDOW_WIDTH;
  chResolution.Frm0Height = SWMOSAIC_WINDOW_HEIGHT;
  chResolution.Frm0Pitch = SWMOSAIC_WINDOW_WIDTH * 2;
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
    /* on secondary display, it is scaled to display size / 4 */  
    chResolution.Frm0Width = DISPLAY_WIDTH / 2;
    chResolution.Frm0Height = DISPLAY_HEIGHT / 2;
    chResolution.Frm0Pitch = (DISPLAY_WIDTH / 2) * 2;  
  }

  eError =
    OMX_SetConfig (pAppData->pScHandle[instId],
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
    OMX_SetConfig (pAppData->pScHandle[instId],
                   (OMX_INDEXTYPE) OMX_TI_IndexConfigAlgEnable, &algEnable);
  if (eError != OMX_ErrorNone)
    ERROR ("failed to disable algo by pass mode\n");

  return (eError);
}

/* ========================================================================== */
/**
* IL_ClientSetSwMosaicParams() : Function to fill the port definition 
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

OMX_ERRORTYPE IL_ClientSetSwMosaicParams ( IL_Client *pAppData )
{
  int i;
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  OMX_PARAM_BUFFER_MEMORYTYPE memTypeCfg;
  OMX_PARAM_PORTDEFINITIONTYPE paramPort;
  OMX_CONFIG_VSWMOSAIC_CREATEMOSAICLAYOUT  sMosaic;

  OMX_INIT_PARAM (&memTypeCfg);
  memTypeCfg.eBufMemoryType = OMX_BUFFER_MEMORY_DEFAULT;

  for ( i = 0; i < g_max_decode; i++) {
   memTypeCfg.nPortIndex = OMX_VSWMOSAIC_INPUT_PORT_START_INDEX + i;
   
   eError =
     OMX_SetParameter (pAppData->pVswmosaicHandle, OMX_TI_IndexParamBuffMemType,
                       &memTypeCfg);
   if (eError != OMX_ErrorNone)
   {
     ERROR ("failed to set memory Type at input port\n");
   }
 }

  /* Setting Memory type at output port to Raw Memory */
  OMX_INIT_PARAM (&memTypeCfg);
  memTypeCfg.nPortIndex = OMX_VSWMOSAIC_OUTPUT_PORT_START_INDEX;
  memTypeCfg.eBufMemoryType = OMX_BUFFER_MEMORY_DEFAULT;
  eError =
    OMX_SetParameter (pAppData->pVswmosaicHandle, OMX_TI_IndexParamBuffMemType,
                      &memTypeCfg);

  if (eError != OMX_ErrorNone)
  {
    ERROR ("failed to set memory Type at output port\n");
  }

  for ( i = 0; i < g_max_decode; i++) {
   /* set input height/width and color format */
   OMX_INIT_PARAM (&paramPort);
   paramPort.nPortIndex = OMX_VSWMOSAIC_INPUT_PORT_START_INDEX + i;

   OMX_GetParameter (pAppData->pVswmosaicHandle, OMX_IndexParamPortDefinition,
                     &paramPort);
   paramPort.nPortIndex = OMX_VFPC_INPUT_PORT_START_INDEX + i;
   paramPort.format.video.nFrameWidth  = SWMOSAIC_WINDOW_WIDTH; 
   paramPort.format.video.nFrameHeight = SWMOSAIC_WINDOW_HEIGHT;
   /* swmosaic is connceted to scalar, whose stride is different than width*/
   paramPort.format.video.nStride = SWMOSAIC_WINDOW_WIDTH * 2;

   if (1 == pAppData->displayId) {
     /*For the case of On-chip HDMI as display device*/
     paramPort.format.video.nFrameWidth = DISPLAY_WIDTH / 2;
     paramPort.format.video.nFrameHeight = DISPLAY_HEIGHT / 2;
     paramPort.format.video.nStride = (DISPLAY_WIDTH / 2) * 2;
   }

   paramPort.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
   paramPort.format.video.eColorFormat = OMX_COLOR_FormatYCbYCr;
   paramPort.nBufferSize = (paramPort.format.video.nStride *
                            paramPort.format.video.nFrameHeight);

   paramPort.nBufferAlignment = 0;
   paramPort.bBuffersContiguous = 0;
   paramPort.nBufferCountActual = IL_CLIENT_VSWMOSAIC_INPUT_BUFFER_COUNT;
   printf ("vswmosaic: set input port params (width = %u, height = %u) \n",
           (unsigned int)paramPort.format.video.nFrameWidth, 
           (unsigned int)paramPort.format.video.nFrameHeight);
   OMX_SetParameter (pAppData->pVswmosaicHandle, OMX_IndexParamPortDefinition,
                     &paramPort);
 }
  
  /* set output height/width and color format */
  OMX_INIT_PARAM (&paramPort);
  paramPort.nPortIndex = OMX_VSWMOSAIC_OUTPUT_PORT_START_INDEX;
  OMX_GetParameter (pAppData->pVswmosaicHandle, OMX_IndexParamPortDefinition,
                    &paramPort);

  paramPort.nPortIndex = OMX_VSWMOSAIC_OUTPUT_PORT_START_INDEX;
  paramPort.format.video.nFrameWidth  = HD_WIDTH;
  paramPort.format.video.nFrameHeight = HD_HEIGHT;
  paramPort.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
  paramPort.format.video.eColorFormat = OMX_COLOR_FormatYCbYCr;
  paramPort.nBufferAlignment = 0;
  paramPort.bBuffersContiguous = 0;
  paramPort.nBufferCountActual = IL_CLIENT_SCALAR_OUTPUT_BUFFER_COUNT;
  paramPort.format.video.nStride = HD_WIDTH * 2;

  if (1 == pAppData->displayId) {
    /*For the case of On-chip HDMI as display device*/
    paramPort.format.video.nFrameWidth = DISPLAY_WIDTH;
    paramPort.format.video.nFrameHeight = DISPLAY_HEIGHT;
    paramPort.format.video.nStride = DISPLAY_WIDTH * 2;
  }

  paramPort.nBufferSize =
    paramPort.format.video.nStride * paramPort.format.video.nFrameHeight;

  printf ("vswmosaic: set output port params (width = %d, height = %d) \n",
          (int) paramPort.format.video.nFrameWidth, (int) paramPort.format.video.nFrameHeight);
      

  OMX_SetParameter (pAppData->pVswmosaicHandle, OMX_IndexParamPortDefinition,
                    &paramPort);

  OMX_INIT_PARAM (&sMosaic);
  sMosaic.nPortIndex  = OMX_VSWMOSAIC_OUTPUT_PORT_START_INDEX;
  
  sMosaic.nOpWidth    = HD_WIDTH;  /* Width in pixels  */
  sMosaic.nOpHeight   = HD_HEIGHT; /* Height in pixels */
  sMosaic.nOpPitch    = HD_WIDTH*2; /* Pitch in bytes   */

  if (1 == pAppData->displayId) {
    /* on secondary display, it is scaled to display size */  
    sMosaic.nOpWidth = DISPLAY_WIDTH;
    sMosaic.nOpHeight= DISPLAY_HEIGHT;
    sMosaic.nOpPitch = DISPLAY_WIDTH*2;  
  }
  printf ("vswmosaic: Width=%d, Height=%d, pitch=%d\n", (int) sMosaic.nOpWidth,
          (int) sMosaic.nOpHeight, (int) sMosaic.nOpPitch);

  sMosaic.nNumWindows = g_max_decode;
  
  for ( i = 0; i < g_max_decode; i++) {
    sMosaic.sMosaicWinFmt[i].dataFormat = OMX_COLOR_FormatYCbYCr;
    sMosaic.sMosaicWinFmt[i].nPortIndex = i;
    sMosaic.sMosaicWinFmt[i].pitch[0]   = SWMOSAIC_WINDOW_WIDTH*2;
    sMosaic.sMosaicWinFmt[i].winStartX  = (i%2) * SWMOSAIC_WINDOW_WIDTH;
    sMosaic.sMosaicWinFmt[i].winStartY  = (i/2) * SWMOSAIC_WINDOW_HEIGHT;
    sMosaic.sMosaicWinFmt[i].winWidth   = SWMOSAIC_WINDOW_WIDTH;
    sMosaic.sMosaicWinFmt[i].winHeight  = SWMOSAIC_WINDOW_HEIGHT;

     if (1 == pAppData->displayId) {
      /* on secondary display, it is scaled to display size */  
      sMosaic.sMosaicWinFmt[i].pitch[0]  = (DISPLAY_WIDTH/2) * 2;
      sMosaic.sMosaicWinFmt[i].winHeight = DISPLAY_HEIGHT / 2;
      sMosaic.sMosaicWinFmt[i].winWidth  = DISPLAY_WIDTH / 2;  
      sMosaic.sMosaicWinFmt[i].winStartX = (i%2) * (DISPLAY_WIDTH / 2);
      sMosaic.sMosaicWinFmt[i].winStartY = (i/2) * (DISPLAY_HEIGHT / 2);
      
    }
   }

  eError = OMX_SetConfig (pAppData->pVswmosaicHandle, 
                          OMX_TI_IndexConfigVSWMOSAICCreateMosaicLayout, 
                          &sMosaic);
  if (eError != OMX_ErrorNone)
  {
    ERROR ("Failed to set OMX_TI_IndexConfigVSWMOSAICCreateMosaicLayout for output \n");
  }
  else {
    printf ("Mosaic layout configuration:: Successful \n");
  }

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
  OMX_PARAM_VFDC_DRIVERINSTID driverId;
  OMX_PARAM_VFDC_CREATEMOSAICLAYOUT mosaicLayout;
  OMX_CONFIG_VFDC_MOSAICLAYOUT_PORT2WINMAP port2Winmap;

  OMX_INIT_PARAM (&paramPort);

  /* set input height/width and color format */
  paramPort.nPortIndex = OMX_VFDC_INPUT_PORT_START_INDEX;
  OMX_GetParameter (pAppData->pDisHandle, OMX_IndexParamPortDefinition,
                    &paramPort);
  paramPort.nPortIndex = OMX_VFDC_INPUT_PORT_START_INDEX;
  paramPort.format.video.nFrameWidth = HD_WIDTH; 
  paramPort.format.video.nFrameHeight = HD_HEIGHT;
  paramPort.format.video.nStride = HD_WIDTH * 2 ; 
  paramPort.nBufferCountActual = IL_CLIENT_DISPLAY_INPUT_BUFFER_COUNT;
  paramPort.format.video.eCompressionFormat = OMX_VIDEO_CodingUnused;
  paramPort.format.video.eColorFormat = OMX_COLOR_FormatYCbYCr;
  
   if (1 == pAppData->displayId) {
    /* on secondary display, it is scaled to display size */  
    paramPort.format.video.nFrameWidth = DISPLAY_WIDTH;
    paramPort.format.video.nFrameHeight = DISPLAY_HEIGHT;
    paramPort.format.video.nStride = DISPLAY_WIDTH * 2;  
  }
  paramPort.nBufferSize = paramPort.format.video.nStride * pAppData->nHeight;
  
  printf ("Buffer Size computed: %d\n", (int) paramPort.nBufferSize);
  printf ("set input port params (width = %d, height = %d) \n",
          (int) pAppData->nWidth, (int) pAppData->nHeight);
  OMX_SetParameter (pAppData->pDisHandle, OMX_IndexParamPortDefinition,
                    &paramPort);
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

  OMX_INIT_PARAM (&mosaicLayout);
  /* Configuring the first (and only) window */
  /* position of window can be changed by following cordinates, keeping in
     center by default */

  mosaicLayout.sMosaicWinFmt[0].winStartX = 0;
  mosaicLayout.sMosaicWinFmt[0].winStartY = 0;
  mosaicLayout.sMosaicWinFmt[0].winWidth = HD_WIDTH;
  mosaicLayout.sMosaicWinFmt[0].winHeight = HD_HEIGHT;
  mosaicLayout.sMosaicWinFmt[0].pitch[VFDC_YUV_INT_ADDR_IDX] = HD_WIDTH * 2;
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
