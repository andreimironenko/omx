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

#ifndef __OMX_ILCLIENT_UTILS_H__
#define __OMX_ILCLIENT_UTILS_H__

#define IL_CLIENT_ENC_INPUT_BUFFER_COUNT      (4)
#define IL_CLIENT_ENC_OUTPUT_BUFFER_COUNT     (4)

typedef enum
{
  ArgID_OUTPUT_FILE = 256, /* it should not conflict with std ascii */
  ArgID_INPUT_FILE,
  ArgID_WIDTH,
  ArgID_HEIGHT,
  ArgID_FRAMERATE,
  ArgID_BITRATE,
  ArgID_CODEC,
  ArgID_NUMARGS
} ArgID;

#define MAX_FILE_NAME_SIZE      256
#define MAX_CODEC_NAME_SIZE     16


/* Arguments for app */
typedef struct Args
{
  char output_file[MAX_FILE_NAME_SIZE];
  char input_file[MAX_FILE_NAME_SIZE];
  int width;
  int height;
  int frame_rate;
  int bit_rate;
  char codec[MAX_CODEC_NAME_SIZE];
} IL_ARGS;

void usage (IL_ARGS *argsp);

void parse_args(int argc, char *argv[], IL_ARGS *argsp);

OMX_ERRORTYPE IL_ClientSetEncodeParams (IL_Client *pAppData);

OMX_ERRORTYPE IL_ClientEncUseInitialOutputResources (IL_CLIENT_COMP_PRIVATE
                                                       *thisComp);
OMX_ERRORTYPE IL_ClientEncUseInitialInputResources (IL_Client *pAppdata);

#endif

/* Nothing beyond this point */
