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
 *  @file  ilclient_utils.c
 *  @brief This file contains aac codec specific initialization utils
 *
 *  @rev 1.0
 *******************************************************************************
 */
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <xdc/std.h>
#include <OMX_Video.h>
#include "OMX_TI_Audio.h"
#include "OMX_TI_Index.h"
#include "OMX_TI_Common.h"
#include "omx_aenc.h"
#include "OMX_Component.h"
#ifdef CODEC_AACENC
#include <ti/sdo/codecs/aaclcenc/imp4aacenc.h>
#endif
#include "ilclient_utils.h"
#include <getopt.h>

#define OMX_TEST_INIT_STRUCT_PTR(_s_, _name_)       \
 memset((_s_), 0x0, sizeof(_name_)); \
    (_s_)->nSize = sizeof(_name_);              \
    (_s_)->nVersion.s.nVersionMajor = 0x1;      \
    (_s_)->nVersion.s.nVersionMinor = 0x1;      \
    (_s_)->nVersion.s.nRevision  = 0x0;       \
    (_s_)->nVersion.s.nStep   = 0x0;

void usage(IL_ARGS * argsp)
{
    printf
        (" ./audio_encode_a8host_debug.xv5T [-i input file ] -o <output file> -c <codec> -n <channels> -b <bitrate> -s <samplerate> -f <format> \n"
         "-i | --input          input filename (optional) \n"
         "-o | --output         output filename \n"
         "-c | --codec          aaclc           \n"
         "-n | --channels       no of channels (1,2,5,6)  \n"
         "-b | --bitrate        bitrate         \n"
         "-s | --samplerate     samplerate         \n"
         "-f | --format         format (ADTS, ADIF, RAW \n");
    printf
        (" use -i option to specify input file : \n"
         "     ./audio_encode_a8host_debug.xv5T -i in.pcm -o out.aac -c aaclc -n 2 -b 192000 -s 44100 -f ADTS\n "
         " skip -i to use the alsa mic : \n"
         "      ./audio_encode_a8host_debug.xv5T -o out.aac -c aaclc -n 2 -b 192000 -s 44100 -f ADTS\n ");
    exit(1);
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

void parse_args(int argc, char *argv[], IL_ARGS * argsp)
{
    const char shortOptions[] = "i:o:c:n:b:s:f:";
    const struct option longOptions[] = {
        {"input", required_argument, NULL, ArgID_INPUT_FILE},
        {"output", required_argument, NULL, ArgID_OUTPUT_FILE},
        {"codec", required_argument, NULL, ArgID_CODEC},
        {"channels", required_argument, NULL, ArgID_CHANNELS},
        {"bitrate", required_argument, NULL, ArgID_BITRATE},
        {"samplerate", required_argument, NULL, ArgID_SAMPLERATE},
        {"format", required_argument, NULL, ArgID_FORMAT},
        {0, 0, 0, 0}
    };

    int index, infile = 0, codec = 0, outfile = 0, channels = 0, bitrate =
        0, samplerate = 0, format = 0;
    int argID;

    strncpy(argsp->input_file, "", MAX_FILE_NAME_SIZE);

    for (;;) {
        argID = getopt_long(argc, argv, shortOptions, longOptions, &index);

        if (argID == -1) {
            break;
        }

        switch (argID) {
        case ArgID_INPUT_FILE:
        case 'i':
            strncpy(argsp->input_file, optarg, MAX_FILE_NAME_SIZE);
            infile = 1;
            break;

        case ArgID_CODEC:
        case 'c':
            strncpy(argsp->codec, optarg, MAX_CODEC_NAME_SIZE);
            codec = 1;
            break;
        case ArgID_OUTPUT_FILE:
        case 'o':
            strncpy(argsp->output_file, optarg, MAX_FILE_NAME_SIZE);
            outfile = 1;
            break;
        case ArgID_CHANNELS:
        case 'n':
            argsp->no_channels = atoi(optarg);
            channels = 1;
            break;
        case ArgID_BITRATE:
        case 'b':
            argsp->bitrate = atoi(optarg);
            bitrate = 1;
            break;
        case ArgID_SAMPLERATE:
        case 's':
            argsp->samplerate = atoi(optarg);
            samplerate = 1;
            break;
        case ArgID_FORMAT:
        case 'f':
            strncpy(argsp->format, optarg, MAX_FORMAT_NAME_SIZE);
            format = 1;
            break;
        default:
            usage(argsp);
            exit(1);
        }
    }

    if (optind < argc) {
        usage(argsp);
        exit(EXIT_FAILURE);
    }

    if (!codec || !outfile || !channels || !bitrate || !format || !samplerate) {
        usage(argsp);
        exit(1);
    }

    printf("input_file: %s\n", argsp->input_file);
    printf("output_file: %s\n", argsp->output_file);
    printf("codec: %s\n", argsp->codec);
    printf("no of channnels :%d\n", argsp->no_channels);
    printf("bitrate : %d\n", argsp->bitrate);
    printf("samplerate : %d\n", argsp->samplerate);
    printf("outputformat :%s\n", argsp->format);

}

/* ========================================================================== */
/**
* AENC_SetParamPortDefinition() : Function to fill the port definition
* structures and call the Set_Parameter function on to the AAC Encoder
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
OMX_ERRORTYPE AENC_SetParamPortDefinition(AENC_Client * pAppData)
{
    OMX_ERRORTYPE eError = OMX_ErrorUndefined;
    OMX_HANDLETYPE pHandle = pAppData->pHandle;
    OMX_PORT_PARAM_TYPE portInit;
    OMX_PARAM_PORTDEFINITIONTYPE *pInPortDef, *pOutPortDef;
    OMX_AUDIO_PARAM_AACPROFILETYPE tAACProfile;
    OMX_AUDIO_PARAM_PCMMODETYPE tPCMMode;
    //OMX_AUDIO_PARAM_PORTFORMATTYPE tPortFormat;
/*    OMX_PARAM_COMPPORT_NOTIFYTYPE tNotify;*/

    if (!pHandle) {
        eError = OMX_ErrorBadParameter;
        goto EXIT;
    }
    pInPortDef = pAppData->pInPortDef;
    pOutPortDef = pAppData->pOutPortDef;
    OMX_TEST_INIT_STRUCT_PTR(&portInit, OMX_PORT_PARAM_TYPE);

    portInit.nPorts = 2;
    portInit.nStartPortNumber = 0;
    eError = OMX_SetParameter(pHandle, OMX_IndexParamAudioInit, &portInit);
    if (eError != OMX_ErrorNone) {
        goto EXIT;
    }


    /* Set the component's OMX_PARAM_PORTDEFINITIONTYPE structure (input) */

    OMX_TEST_INIT_STRUCT_PTR(pInPortDef, OMX_PARAM_PORTDEFINITIONTYPE);
    pInPortDef->nPortIndex = OMX_AUDENC_INPUT_PORT;

    OMX_GetParameter(pHandle, OMX_IndexParamPortDefinition, pInPortDef);

    pInPortDef->nBufferCountActual = NUM_OF_IN_BUFFERS;
    pInPortDef->format.audio.eEncoding = OMX_AUDIO_CodingPCM;

    eError =
        OMX_SetParameter(pHandle, OMX_IndexParamPortDefinition, pInPortDef);
    if (eError != OMX_ErrorNone) {
        goto EXIT;
    }

    /* Set the component's OMX_PARAM_PORTDEFINITIONTYPE structure (output) */
    OMX_TEST_INIT_STRUCT_PTR(pOutPortDef, OMX_PARAM_PORTDEFINITIONTYPE);
    pOutPortDef->nPortIndex = OMX_AUDENC_OUTPUT_PORT;
    OMX_GetParameter(pHandle, OMX_IndexParamPortDefinition, pOutPortDef);
    pOutPortDef->nBufferCountActual = NUM_OF_OUT_BUFFERS;
    pOutPortDef->format.audio.eEncoding = OMX_AUDIO_CodingAAC;

    eError =
        OMX_SetParameter(pHandle, OMX_IndexParamPortDefinition, pOutPortDef);
    if (eError != OMX_ErrorNone) {
        goto EXIT;
    }
    OMX_TEST_INIT_STRUCT_PTR(&tPCMMode, OMX_AUDIO_PARAM_PCMMODETYPE);
    tPCMMode.nPortIndex = OMX_AUDENC_INPUT_PORT;

    OMX_GetParameter(pHandle, OMX_IndexParamAudioPcm, &tPCMMode);

    tPCMMode.nChannels = pAppData->nChannels;
    tPCMMode.nSamplingRate = pAppData->samplerate;
    eError = OMX_SetParameter(pHandle, OMX_IndexParamAudioPcm, &tPCMMode);
    if (eError != OMX_ErrorNone) {
        goto EXIT;
    }

    OMX_TEST_INIT_STRUCT_PTR(&tAACProfile, OMX_AUDIO_PARAM_AACPROFILETYPE);
    tAACProfile.nPortIndex = OMX_AUDENC_OUTPUT_PORT;

    OMX_GetParameter(pHandle, OMX_IndexParamAudioAac, &tAACProfile);
    tAACProfile.nBitRate = pAppData->bitrate;
    tAACProfile.eAACStreamFormat = pAppData->outputformat;
    eError = OMX_SetParameter(pHandle, OMX_IndexParamAudioAac, &tAACProfile);
    if (eError != OMX_ErrorNone) {
        goto EXIT;
    }

  EXIT:
    return eError;
}
