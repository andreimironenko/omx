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
#ifndef __OMX_AENC_H__
#define __OMX_AENC_H__

/******************************************************************************\
 *      Includes
\******************************************************************************/

/******************************************************************************/

#ifdef __cplusplus              /* required for headers that might */
extern "C" {                    /* be compiled by a C++ compiler */
#endif

    typedef enum {
        ArgID_INPUT_FILE,
  ArgID_OUTPUT_FILE,
  ArgID_CODEC,
  ArgID_CHANNELS,
  ArgID_BITRATE,
  ArgID_SAMPLERATE,
  ArgID_FORMAT,
  ArgID_NUMARGS,
} ArgID;

#define MAX_FILE_NAME_SIZE      256
#define MAX_CODEC_NAME_SIZE     16
#define MAX_FORMAT_NAME_SIZE 16

#define NUM_OF_IN_BUFFERS 1
#define NUM_OF_OUT_BUFFERS 1

/* Arguments for app */
    typedef struct Args {
  char input_file[MAX_FILE_NAME_SIZE];
  char output_file[MAX_FILE_NAME_SIZE];
  char codec[MAX_CODEC_NAME_SIZE];
  int no_channels;
  int bitrate;
  int samplerate;
  char format[MAX_FORMAT_NAME_SIZE];
} IL_ARGS;

void usage (IL_ARGS *argsp);

void parse_args (int argc, char *argv[], IL_ARGS *argsp);

/* ========================================================================== */
/** AENC_Client is the structure definition for the AENC Encoder IL Client
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
* @param fInFrmSz              File pointer of Frame Size file (unused)
* @param fOut                  Output file pointer
* @param ColorFormat           Input color format
* @param nWidth                Width of the input vector
* @param nHeight               Height of the input vector
* @param nEncodedFrm           Total number of encoded frames
*/
/* ========================================================================== */
typedef struct AENC_Client {
    OMX_HANDLETYPE pHandle;
    OMX_COMPONENTTYPE *pComponent;
    OMX_CALLBACKTYPE *pCb;
    OMX_STATETYPE eState;
    OMX_PARAM_PORTDEFINITIONTYPE *pInPortDef;
    OMX_PARAM_PORTDEFINITIONTYPE *pOutPortDef;
    OMX_U8 eCompressionFormat;
    OMX_BUFFERHEADERTYPE *pInBuff[NUM_OF_IN_BUFFERS];
    OMX_BUFFERHEADERTYPE *pOutBuff[NUM_OF_OUT_BUFFERS];
    OMX_S32 IpBuf_Pipe[2];
    OMX_S32 OpBuf_Pipe[2];
    OMX_S32 Event_Pipe[2];

    FILE *fOut;
        FILE *fIn;
	OMX_U32 nChannels;
	OMX_U32 bitrate;
	OMX_U32 samplerate;
	OMX_AUDIO_AACSTREAMFORMATTYPE outputformat;

        OMX_S64 InFileSize;
    OMX_U32 nEncodedFrms;
	OMX_U32 InputDataReadSize;

} AENC_Client;

#endif
