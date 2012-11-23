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
#include <unistd.h>
#include <xdc/std.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/knl/Thread.h>
#include <xdc/runtime/Timestamp.h>
#include <ti/omx/omxutils/omx_utils.h>
#ifdef CODEC_AACENC
#include <ti/sdo/codecs/aaclcenc/imp4aacenc.h>
#endif
/*-------------------------program files -------------------------------------*/
#include "ti/omx/interfaces/openMaxv11/OMX_Audio.h"
#include "ti/omx/interfaces/openMaxv11/OMX_Core.h"
#include "ti/omx/interfaces/openMaxv11/OMX_Component.h"
#include "omx_base_utils.h"
#include "semp.h"
#include "OMX_TI_Common.h"
#include "omx_aenc.h"
#include "ilclient_utils.h"
#include "timm_osal_interfaces.h"
#include <alsa/asoundlib.h>
/*******************************************************************************
 * EXTERNAL REFERENCES NOTE : only use if not found in header file
*******************************************************************************/

/****************************************************************
 * DEFINES
 ****************************************************************/

/** Event definition to indicate input buffer consumed */
#define AENC_ENCODER_INPUT_READY 1

/** Event definition to indicate output buffer consumed */
#define AENC_ENCODER_OUTPUT_READY   2

/** Event definition to indicate error in processing */
#define AENC_ENCODER_ERROR_EVENT 4

/** Event definition to indicate End of stream */
#define AENC_ENCODER_END_OF_STREAM 8

#define INPUT_BUF_SIZE 1024

#define MAX(a,b) (a) > (b)? (a): (b)

/****************************************************************
 * GLOBALS
 ****************************************************************/
int gOutputEventOccured = 0;

semp_t *port_sem;
semp_t *state_sem;

static OMX_BOOL bEOS_Sent;
AENC_Client *gpAppData;

/** Macro to initialize memset and initialize the OMX structure */
#define OMX_AENC_TEST_INIT_STRUCT_PTR(_s_, _name_)       \
 memset((_s_), 0x0, sizeof(_name_)); \
    (_s_)->nSize = sizeof(_name_);              \
    (_s_)->nVersion.s.nVersionMajor = 0x1;      \
    (_s_)->nVersion.s.nVersionMinor = 0x1;      \
    (_s_)->nVersion.s.nRevision  = 0x0;       \
    (_s_)->nVersion.s.nStep   = 0x0;


/*--------------------- function prototypes ----------------------------------*/
Void getDefaultHeapStats();

OMX_ERRORTYPE AENC_SetParamPortDefinition(AENC_Client * pAppData);

void SignalProcess(int sig)
{
    OMX_U32 postEvent = AENC_ENCODER_END_OF_STREAM;
    OMX_S32 retval;

    printf("STOP!!\n");
    retval = write(gpAppData->Event_Pipe[1], &postEvent, sizeof(postEvent));
    if (retval != sizeof(postEvent)) {
        printf("error : write to pipe failed\n");
        return;
    }
}

/* Handle for the PCM device */
snd_pcm_t *record_handle = NULL;

OMX_ERRORTYPE
ConfigureAudioDriver(int channels, int samplerate, int period_size)
{
    OMX_ERRORTYPE eError = OMX_ErrorUndefined;
    /* Playback stream */
    snd_pcm_stream_t stream = SND_PCM_STREAM_CAPTURE;

    /*
       This structure contains information about the hardware and can be
       used to specify the configuration to be used for
       the PCM stream.
     */
    snd_pcm_hw_params_t *hw_params;

    /*
       name of the device
     */
    static char *device = "default";

    int err, exact_rate;
    int dir, exact_period_size;
    exact_period_size = period_size;


    /*Open PCM. The last parameter of this function is the mode. */
    if ((err = snd_pcm_open(&record_handle, device, stream, 0)) < 0) {
        printf("Could not open audio device\n");
        return eError;
    }

    /* Allocate the snd_pcm_hw_params_t structure on the stack. */
    if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0) {
        fprintf(stderr, "cannot allocate hardware parameters (%s)\n",
                snd_strerror(err));
        return eError;
    }

    /* Init hwparams with full configuration space */
    if ((err = snd_pcm_hw_params_any(record_handle, hw_params)) < 0) {
        fprintf(stderr,
                "cannot initialize hardware parameter structure (%s)\n",
                snd_strerror(err));
        return eError;
    }

    /* Set access type. */
    if ((err =
         snd_pcm_hw_params_set_access(record_handle, hw_params,
                                      SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
        fprintf(stderr, "cannot set access type (%s)\n", snd_strerror(err));
        return eError;
    }

    /* Set sample format */
    if ((err =
         snd_pcm_hw_params_set_format(record_handle, hw_params,
                                      SND_PCM_FORMAT_S16_LE)) < 0) {
        fprintf(stderr, "cannot set sample format (%s)\n", snd_strerror(err));
        return eError;
    }

    /* Set sample rate. If the exact rate is not supported by the
       hardware, use nearest possible rate. */
    exact_rate = samplerate;
    if ((err =
         snd_pcm_hw_params_set_rate_near(record_handle, hw_params,
                                         (unsigned int *) &samplerate, 0)) < 0)
    {
                                        
        fprintf(stderr, "cannot set sample rate (%s)\n", snd_strerror(err));
        return eError;
    }


    if (samplerate != exact_rate) {
        fprintf(stderr, "The rate %d Hz is not supported by the hardware. \
			Using %d Hz instead.\n", samplerate, exact_rate);
    }

    /* Set number of channels */
    if ((err =
         snd_pcm_hw_params_set_channels(record_handle, hw_params,
                                        channels)) < 0) {
        fprintf(stderr, "cannot set channel count (%s)\n", snd_strerror(err));
        return eError;
    }

    /* Set period size */
    if ((err =
         snd_pcm_hw_params_set_period_size_near(record_handle, hw_params,
                                                (snd_pcm_uframes_t *) &period_size,
                                                &dir)) < 0)
    {
        fprintf(stderr, "cannot set periodsize (%s)\n", snd_strerror(err));
        return eError;
    }
    if (period_size != exact_period_size) {
        fprintf(stderr, "using periodsize %d instead of %d \n", period_size,
                exact_period_size);
    }



    /* Apply HW parameter settings to PCM device and prepare device. */
    if ((err = snd_pcm_hw_params(record_handle, hw_params)) < 0) {
        fprintf(stderr, "cannot set parameters (%s)\n", snd_strerror(err));
        return eError;
    }

    snd_pcm_hw_params_free(hw_params);

    if ((err = snd_pcm_prepare(record_handle)) < 0) {
        fprintf(stderr, "cannot prepare audio interface for use (%s)\n",
                snd_strerror(err));
        return eError;
    }

    eError = OMX_ErrorNone;

    return eError;
}

/* ========================================================================== */
/**
* AENC_AllocateResources() : Allocates the resources required for Audio
* Encoder.
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
static OMX_ERRORTYPE AENC_AllocateResources(AENC_Client * pAppData)
{
    OMX_U32 retval;
    OMX_ERRORTYPE eError = OMX_ErrorNone;

    pAppData->pCb = (OMX_CALLBACKTYPE *) malloc(sizeof(OMX_CALLBACKTYPE));
    if (!pAppData->pCb) {
        eError = OMX_ErrorInsufficientResources;
        goto EXIT;
    }

    pAppData->pInPortDef = (OMX_PARAM_PORTDEFINITIONTYPE *)
        malloc(sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
    if (!pAppData->pInPortDef) {
        eError = OMX_ErrorInsufficientResources;
        goto EXIT;
    }

    pAppData->pOutPortDef = (OMX_PARAM_PORTDEFINITIONTYPE *)
        malloc(sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
    if (!pAppData->pOutPortDef) {
        eError = OMX_ErrorInsufficientResources;
        goto EXIT;
    }

    /* Create a pipes for Input and Output Buffers.. used to queue data from the callback. */
    retval = pipe((int *) pAppData->IpBuf_Pipe);
    if (retval == -1) {
        eError = OMX_ErrorContentPipeCreationFailed;
        goto EXIT;
    }
    retval = pipe((int *) pAppData->OpBuf_Pipe);
    if (retval == -1) {
        eError = OMX_ErrorContentPipeCreationFailed;
        goto EXIT;
    }
    retval = pipe((int *) pAppData->Event_Pipe);
    if (retval == -1) {
        eError = OMX_ErrorContentPipeCreationFailed;
        goto EXIT;
    }

  EXIT:
    return eError;
}

/* ========================================================================== */
/**
* AENC_FreeResources() : Free the resources allocated for Audio
* Encoder.
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
static void AENC_FreeResources(AENC_Client * pAppData)
{



    if (pAppData->pCb)
        free(pAppData->pCb);

    if (pAppData->pInPortDef)
        free(pAppData->pInPortDef);

    if (pAppData->pOutPortDef)
        free(pAppData->pOutPortDef);

    close((int) pAppData->IpBuf_Pipe);

    close((int) pAppData->OpBuf_Pipe);

    close((int) pAppData->Event_Pipe);

    return;
}

/* ========================================================================== */
/**
* AENC_GetEncoderErrorString() : Function to map the OMX error enum to string
*
* @param error   : OMX Error type
*
*  @return
*  String conversion of the OMX_ERRORTYPE
*
*/
/* ========================================================================== */
#ifdef ILC_AENC_GETENCERRSTR_IN_BUILD
static OMX_STRING AENC_GetEncoderErrorString(OMX_ERRORTYPE error)
{
    OMX_STRING errorString;

    switch (error) {
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
#endif /* ILC_AENC_GETENCERRSTR_IN_BUILD */

/* ========================================================================== */
/**
* AAC_Read_InputData() : Function to fill the input buffer with data.
* This function currently reads the entire file into one single memory chunk.
* May require modification to support bigger file sizes.
*
*
* @param pAppData   : Pointer to the application data
* @param pBuf       : Pointer to the input buffer
*
*  @return
*  OMX_U32 nRead   
*
*
*/
/* ========================================================================== */
static OMX_U32
AAC_Read_InputData(AENC_Client * pAppData, OMX_BUFFERHEADERTYPE * pBufHdr)
{
    OMX_U32 nRead = 0;
    OMX_U32 toRead = pAppData->pInPortDef->nBufferSize;

    int tFrames;

    if (pAppData->fIn != NULL) {
    nRead = fread(pBufHdr->pBuffer, sizeof(char), toRead, pAppData->fIn);
    } else {
        tFrames = toRead / (pAppData->nChannels * 2);

        nRead = snd_pcm_readi(record_handle, pBufHdr->pBuffer, tFrames);

        if (nRead == -EPIPE) {
            /* EPIPE means overrun */
            fprintf(stderr, "overrun occurred\n");
            snd_pcm_prepare(record_handle);
        } else if (nRead < 0) {
            fprintf(stderr, "error from read: %s\n", snd_strerror(nRead));
        } else if (nRead != tFrames) {
            fprintf(stderr, "short read, read %d frames\n", (int) nRead);
        }
    }
    pBufHdr->nFilledLen = toRead;
    pBufHdr->nOffset = 0;

    return nRead;

}

/* ========================================================================== */
/**
* AENC_FillData() : Function to fill the input buffer with data.
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
static OMX_U32
AENC_FillData(AENC_Client * pAppData, OMX_BUFFERHEADERTYPE * pBuf)
{
    OMX_U32 nRead = 0;

    nRead = AAC_Read_InputData(pAppData, pBuf);

    return nRead;
}

/* ========================================================================== */
/**
* AENC_ChangePortSettings() : This method will
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
static OMX_ERRORTYPE AENC_ChangePortSettings(AENC_Client * pAppData)
{

    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_U32 i;


    eError =
        OMX_SendCommand(pAppData->pHandle, OMX_CommandPortDisable,
                        pAppData->pOutPortDef->nPortIndex, NULL);
    if (eError != OMX_ErrorNone) {
        goto EXIT;
    }

    for (i = 0; i < pAppData->pOutPortDef->nBufferCountActual; i++) {
        eError =
            OMX_FreeBuffer(pAppData->pHandle, pAppData->pOutPortDef->nPortIndex,
                           pAppData->pOutBuff[i]);
        if (eError != OMX_ErrorNone) {
            goto EXIT;
        }
    }

    eError =
        OMX_GetParameter(pAppData->pHandle, OMX_IndexParamPortDefinition,
                         pAppData->pOutPortDef);
    if (eError != OMX_ErrorNone) {
        goto EXIT;
    }

    eError =
        OMX_SendCommand(pAppData->pHandle, OMX_CommandPortEnable,
                        pAppData->pOutPortDef->nPortIndex, NULL);
    if (eError != OMX_ErrorNone) {
        goto EXIT;
    }

    for (i = 0; i < pAppData->pOutPortDef->nBufferCountActual; i++) {
        eError =
            OMX_AllocateBuffer(pAppData->pHandle, &pAppData->pOutBuff[i],
                               pAppData->pOutPortDef->nPortIndex, pAppData,
                               pAppData->pOutPortDef->nBufferSize);
        if (eError != OMX_ErrorNone) {
            goto EXIT;
        }
    }

  EXIT:
    return eError;

}

/* ========================================================================== */
/**
* AENC_EventHandler() : This method is the event handler implementation to
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
static OMX_ERRORTYPE
AENC_EventHandler(OMX_HANDLETYPE hComponent,
                                       OMX_PTR ptrAppData,
                                       OMX_EVENTTYPE eEvent, OMX_U32 nData1,
                                       OMX_U32 nData2, OMX_PTR pEventData)
{
    AENC_Client *pAppData = ptrAppData;
    /*OMX_STATETYPE state; */
    OMX_S32 retval;
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_U32 postEvent;

    switch (eEvent) {
    case OMX_EventCmdComplete:
        if (nData1 == OMX_CommandStateSet) {
            semp_post(state_sem);
        }
        if (nData1 == OMX_CommandPortEnable || nData1 == OMX_CommandPortDisable) {
            semp_post(port_sem);
        }
        break;
    case OMX_EventError:
        postEvent = AENC_ENCODER_ERROR_EVENT;
        retval = write(pAppData->Event_Pipe[1], &postEvent, sizeof(postEvent));
        if (retval != sizeof(postEvent)) {
            eError = OMX_ErrorNotReady;
            return eError;
        }
        eError = OMX_GetState(pAppData->pHandle, &pAppData->eState);
        /*For create errors: */
        if (pAppData->eState == OMX_StateLoaded) {
            semp_post(state_sem);
        }
        break;
    case OMX_EventMark:

        break;
    case OMX_EventPortSettingsChanged:

        /* In case of change in output buffer sizes re-allocate the buffers */
        eError = AENC_ChangePortSettings(pAppData);

        break;
    case OMX_EventBufferFlag:
        postEvent = AENC_ENCODER_END_OF_STREAM;
        retval = write(pAppData->Event_Pipe[1], &postEvent, sizeof(postEvent));
        if (retval != sizeof(postEvent)) {
            eError = OMX_ErrorNotReady;
            return eError;
        }
        /* EOS here nData1-> port....  nData2->OMX_BUFFERFLAG_EOS */
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
    case OMX_EventVendorStartUnused:
        break;
    case OMX_EventKhronosExtensions:
        break;
    }                           /* end of switch */

    return eError;
}

/* ========================================================================== */
/**
* AENC_FillBufferDone() : This method handles the fill buffer done event
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
int framesEncoded = 0;
static OMX_ERRORTYPE
AENC_FillBufferDone(OMX_HANDLETYPE hComponent,
                    OMX_PTR ptrAppData, OMX_BUFFERHEADERTYPE * pBuffer)
{
    AENC_Client *pAppData = ptrAppData;
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_S32 retval = 0;
    gOutputEventOccured = 0;

    {
        retval = write(pAppData->OpBuf_Pipe[1], &pBuffer, sizeof(pBuffer));

        if (retval != sizeof(pBuffer)) {
            eError = OMX_ErrorNotReady;
            return eError;
        }
        framesEncoded++;

    }

    return eError;
}

/* ========================================================================== */
/**
* AENC_EmptyBufferDone() : This method handles the Empty buffer done event
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
int gEOF = 0, gbytesInInputBuffer = INPUT_BUF_SIZE;
static OMX_ERRORTYPE
AENC_EmptyBufferDone(OMX_HANDLETYPE hComponent,
                     OMX_PTR ptrAppData, OMX_BUFFERHEADERTYPE * pBuffer)
{
    AENC_Client *pAppData = ptrAppData;
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_S32 retval = 0;

#ifdef AENC_LINUX_CLIENT
#ifdef SRCHANGES
    pBuffer->pBuffer = SharedRegion_getPtr(pBuffer->pBuffer);
#endif
#endif

    retval = write(pAppData->IpBuf_Pipe[1], &pBuffer, sizeof(pBuffer));

    if (retval != sizeof(pBuffer)) {
        printf("EBD error: %d / %d\n", (int) retval, sizeof(pBuffer));
        eError = OMX_ErrorNotReady;
        return eError;
    }


    return eError;
}


/******************************************************************************/
/* Main entrypoint into the Test */
/******************************************************************************/
Int
OMX_Audio_Encode_Test(char *inFileName, char *outFileName, char *format,
                          int nChannels, int bitrate, int samplerate,
                          char *outputFormat)
{
    AENC_Client *pAppData = NULL;
    OMX_HANDLETYPE pHandle;
    OMX_ERRORTYPE eError = OMX_ErrorNone;
    OMX_BUFFERHEADERTYPE *pBufferIn = NULL;
    OMX_BUFFERHEADERTYPE *pBufferOut = NULL;
    OMX_U32 i;
    OMX_U32 nRead = 0;
    OMX_CALLBACKTYPE appCallbacks;

    OMX_U32 pRetrievedEvents;
    OMX_U32 frames_encoded;
    int frameLen;
    struct timeval timeout;

    OMX_S32 nfds = 0;
    fd_set rd, wr, er;
    FD_ZERO(&wr);
    FD_ZERO(&er);

    appCallbacks.EventHandler = AENC_EventHandler;
    appCallbacks.EmptyBufferDone = AENC_EmptyBufferDone;
    appCallbacks.FillBufferDone = AENC_FillBufferDone;

    frames_encoded = 0;

/*	printf("Connect to CCS now : waiting f/r char\n");
	getchar();
*/
    bEOS_Sent = OMX_FALSE;

    port_sem = (semp_t *) malloc(sizeof(semp_t));
    semp_init(port_sem, 0);

    state_sem = (semp_t *) malloc(sizeof(semp_t));
    semp_init(state_sem, 0);

    pAppData = (AENC_Client *) malloc(sizeof(AENC_Client));
    if (!pAppData) {
        eError = OMX_ErrorInsufficientResources;
        goto EXIT;
    }
    memset(pAppData, 0x00, sizeof(AENC_Client));
    gpAppData = pAppData;

    pAppData->fOut = fopen(outFileName, "wb");
    if (pAppData->fOut == NULL) {
        printf("Error: failed to open the file <%s> for writing\n",
               outFileName);
        goto EXIT;
    }
    if (strcmp(format, "aaclc") == 0) {
        pAppData->eCompressionFormat = OMX_AUDIO_CodingAAC;
    }

    pAppData->nChannels = nChannels;
    pAppData->bitrate = bitrate;
    pAppData->samplerate = samplerate;
    if (!strcmp(outputFormat, "RAW")) {
        pAppData->outputformat = OMX_AUDIO_AACStreamFormatRAW;
    } else if (!strcmp(outputFormat, "ADTS")) {
        pAppData->outputformat = OMX_AUDIO_AACStreamFormatMP4ADTS;
    } else if (!strcmp(outputFormat, "ADIF")) {
        pAppData->outputformat = OMX_AUDIO_AACStreamFormatADIF;
    }

    /* Allocate memory for the structure fields present in the pAppData(AENC_Client) */
    eError = AENC_AllocateResources(pAppData);
    if (eError != OMX_ErrorNone) {
        eError = OMX_ErrorInsufficientResources;
        goto EXIT;
    }

    pAppData->eState = OMX_StateInvalid;
    *pAppData->pCb = appCallbacks;


    /* Load the AENC Component */
    eError = OMX_GetHandle(&pHandle, (OMX_STRING) "OMX.TI.DSP.AUDENC"
                           /*StrAENCEncoder */ , pAppData, pAppData->pCb);
    if ((eError != OMX_ErrorNone) || (pHandle == NULL)) {
        printf("Couldn't get a handle\n");
        goto EXIT;
    }


    pAppData->pHandle = pHandle;
    pAppData->pComponent = (OMX_COMPONENTTYPE *) pHandle;

    AENC_SetParamPortDefinition(pAppData);

    fflush(stdout);

    OMX_SendCommand(pAppData->pHandle, OMX_CommandPortEnable,
                    OMX_AUDENC_INPUT_PORT, NULL);


    /* Wait for initialization to complete.. Wait for port enable of component  */
    semp_pend(port_sem);

    OMX_SendCommand(pAppData->pHandle, OMX_CommandPortEnable,
                    OMX_AUDENC_OUTPUT_PORT, NULL);
    /* Wait for initialization to complete.. Wait for port enable of component  */
    semp_pend(port_sem);

    if (eError != OMX_ErrorNone) {
        goto EXIT;
    }

    eError =
        OMX_GetParameter(pHandle, OMX_IndexParamPortDefinition,
                         pAppData->pInPortDef);
    if (eError != OMX_ErrorNone) {
        goto EXIT;
    }


    eError =
        OMX_GetParameter(pHandle, OMX_IndexParamPortDefinition,
                         pAppData->pOutPortDef);
    if (eError != OMX_ErrorNone) {
        goto EXIT;
    }

    /* OMX_SendCommand expecting OMX_StateIdle */
    eError = OMX_SendCommand(pHandle, OMX_CommandStateSet, OMX_StateIdle, NULL);

    if (eError != OMX_ErrorNone) {
        goto EXIT;
    }

    /* Allocate I/O Buffers */

    for (i = 0; i < pAppData->pInPortDef->nBufferCountActual; i++) {
        eError = OMX_AllocateBuffer(pHandle,
                                    &pAppData->pInBuff[i],
                                    pAppData->pInPortDef->nPortIndex, pAppData,
                                    pAppData->pInPortDef->nBufferSize);
    }

    for (i = 0; i < pAppData->pOutPortDef->nBufferCountActual; i++) {
        eError = OMX_AllocateBuffer(pHandle,
                                    &pAppData->pOutBuff[i],
                                    pAppData->pOutPortDef->nPortIndex,
                                    pAppData,
                                    pAppData->pOutPortDef->nBufferSize);
    }

    /* Wait for initialization to complete.. Wait for Idle state of component  */
    semp_pend(state_sem);
    /*check for error event */
    FD_ZERO(&rd);
    FD_SET(pAppData->Event_Pipe[0], &rd);
    //nfds = MAX (nfds, pAppData->Event_Pipe[0]);
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    select(pAppData->Event_Pipe[0] + 1, &rd, &wr, &er, &timeout);

    if (FD_ISSET(pAppData->Event_Pipe[0], &rd)) {
        read(pAppData->Event_Pipe[0], &pRetrievedEvents,
             sizeof(pRetrievedEvents));
        if (pRetrievedEvents == AENC_ENCODER_ERROR_EVENT) {
            printf
                ("Encoder returned an error while creating. Check your input parameters\n");
            goto FREE_HANDLE;
        }
    }

    eError =
        OMX_SendCommand(pHandle, OMX_CommandStateSet, OMX_StateExecuting, NULL);
    if (eError != OMX_ErrorNone) {
        goto FREE_HANDLE;
    }

    semp_pend(state_sem);


    printf("Component transitioned from Idle to Execute\n");

    if (strcmp(inFileName, "") == 0) {
        frameLen = (pAppData->pInPortDef->nBufferSize) / (nChannels * 2);
        eError = ConfigureAudioDriver(nChannels, samplerate, frameLen);
        if (eError != OMX_ErrorNone) {
            printf("Audio driver configuration failed \n");
            goto EXIT1;
        }
    } else {
        pAppData->fIn = fopen(inFileName, "rb");
        if (pAppData->fIn == NULL) {
            printf("Error : failed to open file <%s> for reading\n",
                   inFileName);
            goto EXIT;
        } else {
            fseek(pAppData->fIn, 0, SEEK_END);
            pAppData->InFileSize = ftell(pAppData->fIn);
            fseek(pAppData->fIn, 0, SEEK_SET);
            printf("FileSize : %d\n", (int) pAppData->InFileSize);
        }
    }

    for (i = 0; i < pAppData->pInPortDef->nBufferCountActual; i++) {
        nRead = AENC_FillData(pAppData, pAppData->pInBuff[i]);
        if (!nRead) {
            printf("filldata %d break\n", (int) nRead);
            break;
        }
        if (pAppData->fIn != NULL) {
            pAppData->InFileSize -= nRead;
        }

        eError =
            pAppData->pComponent->EmptyThisBuffer(pHandle,
                                                  pAppData->pInBuff[i]);
        printf("etb called\n");
        if (eError != OMX_ErrorNone) {
            goto EXIT1;
        }
    }

    for (i = 0; i < pAppData->pOutPortDef->nBufferCountActual; i++) {
        eError =
            pAppData->pComponent->FillThisBuffer(pHandle,
                                                 pAppData->pOutBuff[i]);
        if (eError != OMX_ErrorNone) {
            goto EXIT1;
        }
    }

    eError = OMX_GetState(pHandle, &pAppData->eState);

    /* Initialize the number of encoded frames to zero */
    pAppData->nEncodedFrms = 0;
    printf("encoded: 000000");
    while ((eError == OMX_ErrorNone) && (pAppData->eState != OMX_StateIdle)) {
        FD_ZERO(&rd);
        FD_SET(pAppData->OpBuf_Pipe[0], &rd);
        nfds = MAX(nfds, pAppData->OpBuf_Pipe[0]);
        FD_SET(pAppData->IpBuf_Pipe[0], &rd);
        nfds = MAX(nfds, pAppData->IpBuf_Pipe[0]);
        FD_SET(pAppData->Event_Pipe[0], &rd);
        nfds = MAX(nfds, pAppData->Event_Pipe[0]);

        select(nfds + 1, &rd, &wr, &er, NULL);

        if (FD_ISSET(pAppData->Event_Pipe[0], &rd)) {

            read(pAppData->Event_Pipe[0], &pRetrievedEvents,
                 sizeof(pRetrievedEvents));
            if (pRetrievedEvents == AENC_ENCODER_END_OF_STREAM) {
                printf("\nEnd of stream processed");
                break;
            } else if (pRetrievedEvents & AENC_ENCODER_ERROR_EVENT) {
                eError = OMX_ErrorUndefined;
                break;
            }

        }
        if ((FD_ISSET(pAppData->OpBuf_Pipe[0], &rd))) {
            gOutputEventOccured = 1;
            /*read from the pipe */
            read(pAppData->OpBuf_Pipe[0], &pBufferOut, sizeof(pBufferOut));
/*                printf("FTB %x\n",pBufferOut->nFlags);	*/
            if (pBufferOut != NULL) {
                fwrite(pBufferOut->pBuffer, 1, pBufferOut->nFilledLen,
                       pAppData->fOut);
                fflush(pAppData->fOut);
                pBufferOut->nFilledLen = 0;
            }

            if ((pBufferOut->nFlags & OMX_BUFFERFLAG_EOS)) {
                printf("\nReceived End of stream");
                break;
            }
            eError = pAppData->pComponent->FillThisBuffer(pHandle, pBufferOut);
            if (eError != OMX_ErrorNone) {
                printf("error from ftb %x \n", eError);
                goto EXIT1;
            }
            pAppData->nEncodedFrms++;
            printf("\b\b\b\b\b\b%06d", (int) pAppData->nEncodedFrms);
            fflush(stdout);
        }

        if ((FD_ISSET(pAppData->IpBuf_Pipe[0], &rd))) {
            /*read from the pipe */
            read(pAppData->IpBuf_Pipe[0], &pBufferIn, sizeof(pBufferIn));

            if (pBufferIn != NULL) {
                nRead = AENC_FillData(pAppData, pBufferIn);
                if (pAppData->fIn != NULL) {
                pAppData->InFileSize -= nRead;
                if (pAppData->InFileSize <= 0) {
                    pBufferIn->nFlags = OMX_BUFFERFLAG_EOS;
                }
                }
                eError =
                    pAppData->pComponent->EmptyThisBuffer(pHandle, pBufferIn);
            }
        }

        eError = OMX_GetState(pHandle, &pAppData->eState);
    }

  EXIT1:
    printf("\nGoing to Idle\n");
    eError = OMX_SendCommand(pHandle, OMX_CommandStateSet, OMX_StateIdle, NULL);
    if (eError != OMX_ErrorNone) {
        goto EXIT;
    }

    semp_pend(state_sem);

    printf("Going to Loaded\n");
    eError =
        OMX_SendCommand(pHandle, OMX_CommandStateSet, OMX_StateLoaded, NULL);

    printf("Freeing buffers\n");
    for (i = 0; i < pAppData->pInPortDef->nBufferCountActual; i++) {
        eError =
            OMX_FreeBuffer(pHandle, pAppData->pInPortDef->nPortIndex,
                           pAppData->pInBuff[i]);
        if (eError != OMX_ErrorNone) {
            goto EXIT;
        }
    }

    for (i = 0; i < pAppData->pOutPortDef->nBufferCountActual; i++) {
        eError =
            OMX_FreeBuffer(pHandle, pAppData->pOutPortDef->nPortIndex,
                           pAppData->pOutBuff[i]);
        if (eError != OMX_ErrorNone) {
            goto EXIT;
        }
    }

    semp_pend(state_sem);


    printf("Component transitioned from Idle to Loaded\n");

  FREE_HANDLE:
    /* UnLoad the Encoder Component */
    eError = OMX_FreeHandle(pHandle);
    if ((eError != OMX_ErrorNone)) {
        goto EXIT;
    }

    semp_deinit(port_sem);
    free(port_sem);
    port_sem = NULL;

    semp_deinit(state_sem);
    free(state_sem);
    state_sem = NULL;



  EXIT:
    if (pAppData) {

        if (pAppData->fOut)
            fclose(pAppData->fOut);

        AENC_FreeResources(pAppData);

        free(pAppData);
    }

    printf("Audio Encode Test End\n");

  return (0);
}                               /* OMX_Audio_Encode_Test */

/* ilclient.c - EOF */
