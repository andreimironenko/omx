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
 *  @file   omx_adec_priv.h
 *  @brief  This header file contains structure definitions, function 
 *          Headers for utilities of Audio Decoder Component.
 *
 *  @rev 1.0
 *******************************************************************************
 */

#ifndef _OMX_AUDDEC_PRIV_H
#define _OMX_AUDDEC_PRIV_H

#ifdef _cplusplus
extern "C"   {
#endif /* _cplusplus */

#include "OMX_Core.h"
#include "OMX_Component.h"
#include "omx_base.h"
#include "omx_adec_util.h"
#include <ti/sdo/ce/audio1/auddec1.h>

/****************************************************************
 * PUBLIC DECLARATIONS Defined here, used elsewhere

 ****************************************************************/

/** Max allowed length for the component name */
#define OMX_AUDDEC_COMPONENT_NAME_MAX_LENGTH   OMX_MAX_STRINGNAME_SIZE

#define OMX_AUDDEC_DEFAULT_COMPRESSION_FORMAT  OMX_AUDIO_CodingAAC

/** Number of ports for the OMX_AUDDEC Comp */
#define  OMX_AUDDEC_NUM_OF_PORTS     2


/** Default context Type as HOST CONTEXT(0) */
#define OMX_AUDDEC_DEFAULT_CONTEXT_TYPE OMX_BASE_CONTEXT_ACTIVE

/** Default bit rate  Decoder - mp3 support */
#define AUDDEC_DEFAULT_BITRATE                 (128000)    

/** Default sample rate  Decoder - mp3 support */
#define AUDDEC_DEFAULT_SAMPLERATE                 (48000)    

/** Default Endianness - H264 Decoder */
#define AUDDEC_DEFAULT_ENDIANNESS              XDM_BYTE

/** Minimum size for input buffer */
#define AUDDEC_MIN_INPUT_BUFF_SIZE             (4096)

#define OMX_AUDDEC_DEFAULT_INPUT_BUFFER_SIZE  AUDDEC_MIN_INPUT_BUFF_SIZE

#define OMX_AUDDEC_DEFAULT_OUTPUT_BUFFER_SIZE  (8192)  /* In bytes: 1152Samp*2Ch*2Bytes/Samp*/

/** Minimum input output buffers required - default */
#define OMX_AUDDEC_MIN_BUFFER_COUNT            (1) 
#define OMX_AUDDEC_DEFAULT_BUFFER_COUNT        (1)
#define OMX_AUDDEC_NUM_OF_BUFFERS              (1)

#define OMX_AUDDEC_MAX_INPUT_BUFFER_COUNT            (1)
#define OMX_AUDDEC_MAX_OUTPUT_BUFFER_COUNT            (1)
#define OMX_AUDDEC_DEFAULT_INPUT_BUFFER_COUNT     OMX_AUDDEC_MAX_INPUT_BUFFER_COUNT
#define OMX_AUDDEC_DEFAULT_OUTPUT_BUFFER_COUNT    OMX_AUDDEC_MAX_OUTPUT_BUFFER_COUNT

#define OMX_AUDDEC_DEFAULT_BUFFER_ALIGNMENT    (32)

/** Priority of the OMX_AUDDEC Comp  */
#define OMX_AUDDEC_TASK_PRIORITY               (10)

/** Stack Size of the OMX_AUDDEC Comp*/
#define OMX_AUDDEC_TASK_STACKSIZE              (50*1024)

/** Group Id of the OMX_AUDDEC Comp */
#define OMX_AUDDEC_GROUPID                      0

/** Group Priority of the OMX AUDDEC Comp  */
#define OMX_AUDDEC_GROUPPRIORITY 2

/** Defines the major version of the auddec Component */
#define OMX_AUDDEC_COMP_VERSION_MAJOR 1

/**  defines the minor version of the auddec component  */
#define OMX_AUDDEC_COMP_VERSION_MINOR 1

/** defiens the revision of the auddec component */
#define OMX_AUDDEC_COMP_VERSION_REVISION 0

/** defines the step version of the auddec component */
#define OMX_AUDDEC_COMP_VERSION_STEP 0

/** Deafult portstartnumber of auddec comp */
#define  OMX_AUDDEC_DEFAULT_START_PORT_NUM 0

#ifdef CODEC_MP3DEC
#define OMX_AUDDEC_MP3DEC_CALLBACKTABLE_ENTRY (1)
#else
#define OMX_AUDDEC_MP3DEC_CALLBACKTABLE_ENTRY (0)
#endif //CODEC_MP3DEC

#ifdef CODEC_AACDEC
#define OMX_AUDDEC_AACDEC_CALLBACKTABLE_ENTRY (1)
#else
#define OMX_AUDDEC_MP3DEC_CALLBACKTABLE_ENTRY (0)
#endif //CODEC_AACDEC

#define OMX_AudDecCallbacksTableEntryCnt  (OMX_AUDDEC_MP3DEC_CALLBACKTABLE_ENTRY + OMX_AUDDEC_AACDEC_CALLBACKTABLE_ENTRY)

typedef struct _OMX_AudDecCallbacks {
	OMX_U32					compressionFormat;
	OMX_ERRORTYPE (*fpInit_Auddec)(OMX_HANDLETYPE hComponent);
	void (*fpSet_StaticParams) (OMX_HANDLETYPE hComponent, void* params); 
	void (*fpSet_DynamicParams) (void* dynamicparams); 
	void (*fpSet_Status) (void* status); 
} OMX_AudDecCallbacks;

/**
 ******************************************************************************
 *  @struct AUDDEC_InArgs
 *
 *  @brief  This structure defines the run-time input arguments for AUDDEC
 *          instance object
 *
 *  @param  auddecInArgs : XDM Base class InArgs structure  (see iauddec.h)
 ******************************************************************************
*/
typedef struct _AUDDEC_InArgs 
{
  IAUDDEC1_InArgs auddec1InArgs;
} AUDDEC_InArgs;
/**
 ******************************************************************************
 *  @struct AUDDEC_OutArgs
 *
 *  @brief  This structure defines the run time output arguments for 
 *          IMP3ADEC::process function
 *
 *  @param  auddecOutArgs : XDM Base class OutArgs structure  (see iauddec.h)
 ******************************************************************************
*/
typedef struct _AUDDEC_OutArgs 
{
  IAUDDEC1_OutArgs auddec1OutArgs;
} AUDDEC_OutArgs;

/* ========================================================================== */
/** OMX_AUDDEC_COMP_PVTTYPE - Structure for the private elements of the component
 *         This structure includes all the private elements of the Base Component
 *
 *   Define all derived Component Specifc fields over here
 */
/* ========================================================================== */
DERIVEDSTRUCT(OMX_AUDDEC_PVTTYPE, OMX_BASE_PRIVATETYPE)
#define OMX_AUDDEC_PVTTYPE_FIELDS OMX_BASE_PRIVATETYPE_FIELDS \
    OMX_AUDIO_PARAM_PORTFORMATTYPE   tAudioParams[OMX_AUDDEC_NUM_OF_PORTS];\
    Engine_Handle           ce;\
    AUDDEC1_Handle          decHandle;\
    AUDDEC1_Status          *pDecStatus;\
	void                    *pDecDynParams; \
    AUDDEC_InArgs           decInArgs[OMX_AUDDEC_MAX_INPUT_BUFFER_COUNT];\
    AUDDEC_InArgs           *decInArgsPtr;\
    AUDDEC_OutArgs          decOutArgs[OMX_AUDDEC_MAX_OUTPUT_BUFFER_COUNT];\
    AUDDEC_OutArgs          *decOutArgsPtr;\
    OMX_U32                 decOutArgsIdx;\
    OMX_U32                 decInArgsIdx;\
	void*                   pDecStaticParams; \
    XDM1_BufDesc            inputBufDesc[OMX_AUDDEC_MAX_INPUT_BUFFER_COUNT];\
    XDM1_BufDesc            *inputBufDescPtr;\
    XDM1_BufDesc            outputBufDesc[OMX_AUDDEC_MAX_OUTPUT_BUFFER_COUNT];\
    XDM1_BufDesc            *outputBufDescPtr;\
    OMX_U32                 frameCounter;\
    OMX_S32                 cmdXdm;\
    XDAS_Int8               isfirstFrame;\
    TIMM_OSAL_TRACEGRP      nTraceGroup; \
    OMX_BOOL                bCmdFlush;\
    OMX_BOOL                bEOSProcessOn;\
    OMX_BOOL                bCodecCreate;\
    OMX_BOOL                bInBufferCountChange;\
    OMX_BOOL                bOutBufferCountChange;\
    OMX_BOOL                bTaskExit;\
    OMX_PTR                 pDataNotifyMutex;\
    OMX_U32                 inBufArrayIndx; \
    OMX_U32                 outBufArrayIndx; \
    OMX_AUDDEC_buf_header   *inBufHeaderArray; \
    OMX_AUDDEC_buf_header   *outBufHeaderArray; \
	void (*fpSet_StaticParams) (OMX_HANDLETYPE hComponent, void* params); \
	void (*fpSet_DynamicParams) (void* dynamicparams); \
	void (*fpSet_Status) (void* status); \
    char  decoderName[OMX_MAX_STRINGNAME_SIZE]; 
ENDSTRUCT(OMX_AUDDEC_PVTTYPE)

/** Callback from Base to Derived to Notify Command */
static OMX_ERRORTYPE OMX_TI_AUDDEC_CommandNotify ( OMX_HANDLETYPE hComponent,
                                                   OMX_COMMANDTYPE Cmd,
                                                   OMX_U32 nParam,
                                                   OMX_PTR pCmdData );

/** Callback from Base to Derived to Notify Data */
static OMX_ERRORTYPE OMX_TI_AUDDEC_DataNotify ( OMX_HANDLETYPE hComponent );

/** Function to set the parameters of the auddec Component*/
static OMX_ERRORTYPE OMX_TI_AUDDEC_SetParameter ( OMX_HANDLETYPE hComponent,
                                                  OMX_INDEXTYPE nParamIndex,
                                                  OMX_PTR pParamStruct );

/** Function to get the parameters from the component */
static OMX_ERRORTYPE OMX_TI_AUDDEC_GetParameter ( OMX_HANDLETYPE hComponent,
                                                  OMX_INDEXTYPE nParamIndex,
                                                  OMX_PTR pParamStruct );

/** Function to set the Configurations of the Component */
static OMX_ERRORTYPE OMX_TI_AUDDEC_SetConfig ( OMX_HANDLETYPE hComponent,
                                               OMX_INDEXTYPE nIndex,
                                               OMX_PTR pConfigData );

/** Function to get the Configurations of the component */
static OMX_ERRORTYPE OMX_TI_AUDDEC_GetConfig ( OMX_HANDLETYPE hComponent,
                                               OMX_INDEXTYPE nIndex,
                                               OMX_PTR pConfigData );

/** Function to deinitalize the auddec component */
static OMX_ERRORTYPE OMX_TI_AUDDEC_ComponentDeinit ( OMX_HANDLETYPE
                                                     hComponent );

static OMX_ERRORTYPE OMX_TI_AUDDEC_SetDefault ( OMX_AUDDEC_PVTTYPE *
                                                pVidDecComPvt, OMX_AUDIO_CODINGTYPE compressionFormat );

OMX_ERRORTYPE OMX_TI_AUDDEC_ComponentInit_Common ( OMX_HANDLETYPE hComponent,
                                                   OMX_AUDIO_CODINGTYPE
                                                   compressionFormat );

OMX_ERRORTYPE OMX_TI_AUDDEC_InitCallbacks ( OMX_HANDLETYPE hComponent,
                                         OMX_U32 compressionFormat);

#ifdef _cplusplus
}
#endif /* __cplusplus */
  
#endif //_OMX_AUDDEC_PRIV_H
