/**
 *******************************************************************************
 * Texas Instruments OpenMax Multimedia Sub-system                             *
 *                                                                             *
 * "OpenMax Multimedia Sub-system" is a software module developed on TI's DM   *
 * class of SOCs. These modules are based on Khronos open max 1.1.2            *
 * specification.                                                              *
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 ******************************************************************************/

/**
 *******************************************************************************
 *  @file   omx_aenc_priv.h
 *  @brief  This header file contains structure definitions, function 
 *          Headers for utilities of Audio Encoder Component.
 *
 *  @rev 1.0
 *******************************************************************************
 */

#ifndef _OMX_AUDENC_PRIV_H
#define _OMX_AUDENC_PRIV_H

#include "OMX_Core.h"
#include "OMX_Component.h"
#include "omx_base.h"
#include "omx_aenc_util.h"
#include <ti/sdo/ce/audio1/audenc1.h>

/****************************************************************
 * PUBLIC DECLARATIONS Defined here, used elsewhere

 ****************************************************************/

/** Max allowed length for the component name */
#define OMX_AUDENC_COMPONENT_NAME_MAX_LENGTH   OMX_MAX_STRINGNAME_SIZE

#define OMX_AUDENC_DEFAULT_COMPRESSION_FORMAT  OMX_AUDIO_CodingAAC

/** Number of ports for the OMX_AUDENC Comp */
#define  OMX_AUDENC_NUM_OF_PORTS     2


/** Default context Type as HOST CONTEXT(0) */
#define OMX_AUDENC_DEFAULT_CONTEXT_TYPE OMX_BASE_CONTEXT_ACTIVE

/** Default bit rate  Encoder - aac support */
#define AUDENC_DEFAULT_BITRATE                 (128000)    

/** Default sample rate  Encoder - aac support */
#define AUDENC_DEFAULT_SAMPLERATE                 (48000)    

/** Default Endianness - H264 Decoder */
#define AUDENC_DEFAULT_ENDIANNESS              XDM_BYTE

/** Minimum size for input buffer */
#define AUDENC_MIN_INPUT_BUFF_SIZE             (4096)

#define OMX_AUDENC_DEFAULT_INPUT_BUFFER_SIZE  AUDENC_MIN_INPUT_BUFF_SIZE

#define OMX_AUDENC_DEFAULT_OUTPUT_BUFFER_SIZE  (4608)  /* In bytes: 1152Samp*2Ch*2Bytes/Samp*/

/** Minimum input output buffers required - default */
#define OMX_AUDENC_MIN_BUFFER_COUNT            (1) 
#define OMX_AUDENC_DEFAULT_BUFFER_COUNT        (1)
#define OMX_AUDENC_NUM_OF_BUFFERS              (1)

#define OMX_AUDENC_MAX_INPUT_BUFFER_COUNT            (1)
#define OMX_AUDENC_MAX_OUTPUT_BUFFER_COUNT            (1)
#define OMX_AUDENC_DEFAULT_INPUT_BUFFER_COUNT     OMX_AUDENC_MAX_INPUT_BUFFER_COUNT
#define OMX_AUDENC_DEFAULT_OUTPUT_BUFFER_COUNT    OMX_AUDENC_MAX_OUTPUT_BUFFER_COUNT

#define OMX_AUDENC_DEFAULT_BUFFER_ALIGNMENT    (32)

/** Priority of the OMX_AUDENC Comp  */
#define OMX_AUDENC_TASK_PRIORITY               (10)

/** Stack Size of the OMX_AUDENC Comp*/
#define OMX_AUDENC_TASK_STACKSIZE              (100*1024)

/** Group Id of the OMX_AUDENC Comp */
#define OMX_AUDENC_GROUPID                      0

/** Defines the major version of the audenc Component */
#define OMX_AUDENC_COMP_VERSION_MAJOR 1

/**  defines the minor version of the audenc component  */
#define OMX_AUDENC_COMP_VERSION_MINOR 1

/** defiens the revision of the audenc component */
#define OMX_AUDENC_COMP_VERSION_REVISION 0

/** defines the step version of the audenc component */
#define OMX_AUDENC_COMP_VERSION_STEP 0


/** Deafult portstartnumber of audenc comp */
#define  OMX_AUDENC_DEFAULT_START_PORT_NUM 0

/** Priority of the OMX audenc Comp  */
#define OMX_AUDENC_GROUPPRIORITY 2

/** sleep duration between each data processing */
#define OMX_AUDENC_PERIODIC_TASK_SLEEP_DURATION (15)


#ifdef CODEC_AACENC
#define OMX_AUDENC_AACENC_CALLBACKTABLE_ENTRY (1)
#else
#define OMX_AUDENC_AACENC_CALLBACKTABLE_ENTRY (0)
#endif

#define OMX_AudEncCallbacksTableEntryCnt  (OMX_AUDENC_AACENC_CALLBACKTABLE_ENTRY)

typedef struct _AUDENC_BufParams
{
	OMX_U32 nBufferSize;
	OMX_U32 nBufferCountMin;
	OMX_U32 nBufferCountActual;
} AUDENC_BufParams;

typedef struct _OMX_AudEncCallbacks {
	OMX_U32					compressionFormat;
	OMX_ERRORTYPE (*fpInit_Audenc)(OMX_HANDLETYPE hComponent);
	void (*fpSet_StaticParams) (OMX_HANDLETYPE hComponent, void* params); 
	void (*fpSet_DynamicParams) (void* dynamicparams); 
	void (*fpSet_Status) (void* status); 
	AUDENC_BufParams (*fpCalculate_oubuffdetails) (void *, OMX_U32 portIndex); 
} OMX_AudEncCallbacks;


/**
 ******************************************************************************
 *  @struct AUDENC_InArgs
 *
 *  @brief  This structure defines the run-time input arguments for AUDENC
 *          instance object
 *
 *  @param  audencInArgs : XDM Base class InArgs structure  (see iaudenc.h)
 ******************************************************************************
*/
typedef struct _AUDENC_InArgs 
{
  IAUDENC1_InArgs audenc1InArgs;
} AUDENC_InArgs;
/**
 ******************************************************************************
 *  @struct AUDENC_OutArgs
 *
 *  @brief  This structure defines the run time output arguments for 
 *          IMP4AACENC::process function
 *
 *  @param  audencOutArgs : XDM Base class OutArgs structure  (see iaudenc.h)
 ******************************************************************************
*/
typedef struct _AUDENC_OutArgs 
{
  IAUDENC1_OutArgs audenc1OutArgs;
} AUDENC_OutArgs;
#if 0
/**
 ******************************************************************************
 *  @struct AUDENC_Params
 *
 *  @brief  This structure defines the creation time parameters for 
 *          IMP4AACENC::process function
 *
 *  @param  audencParams : XDM Base class Params structure  (see iaudenc.h)
 ******************************************************************************
*/
typedef struct _AUDENC_Params
{
 IAUDENC1_Params audenc1Params;
}AUDENC_Params;
/**
 ******************************************************************************
 *  @struct AUDENC_Status
 *
 *  @brief  This structure defines the status parameters for 
 *          IMP4AACENC::process function
 *
 *  @param  audencStatus : XDM Base class Params structure  (see iaudenc.h)
 ******************************************************************************
*/
typedef struct _AUDENC_Status
{
 IAUDENC1_Status audenc1Status;
}AUDENC_Status;
/**
 ******************************************************************************
 *  @struct AUDENC_Handle
 *
 *  @brief      Opaque handle to an IAUDENC objects for 
 *          IMP4AACENC::process function
 *
 *  @param  audencHandle : XDM Base class Params structure  (see iaudenc.h)
 ******************************************************************************
*/
typedef struct _AUDENC_Handle
{
 IAUDENC1_Handle audenc1Handle;
}AUDENC_Handle;
#endif
/* ========================================================================== */
/** OMX_AUDENC_COMP_PVTTYPE - Structure for the private elements of the component
 *         This structure includes all the private elements of the Base Component
 *
 *   Define all derived Component Specifc fields over here
 */
/* ========================================================================== */
DERIVEDSTRUCT(OMX_AUDENC_PVTTYPE, OMX_BASE_PRIVATETYPE)
#define OMX_AUDENC_PVTTYPE_FIELDS OMX_BASE_PRIVATETYPE_FIELDS \
    OMX_PTR                 pIndataPipe;\
    OMX_PTR                 pOutdataPipe;\
    Engine_Handle           ce;\
    AUDENC1_Handle          encHandle;\
    AUDENC1_Params         *pEncParams;\
    AUDENC1_Status          *pEncStatus;\
	void                    *pEncDynParams; \
    AUDENC1_InArgs           encInArgs[OMX_AUDENC_MAX_INPUT_BUFFER_COUNT];\
    AUDENC1_InArgs           *encInArgsPtr;\
    AUDENC1_OutArgs          encOutArgs[OMX_AUDENC_MAX_OUTPUT_BUFFER_COUNT];\
    AUDENC1_OutArgs          *encOutArgsPtr;\
    OMX_U32                 encOutArgsIdx;\
    OMX_U32                 encInArgsIdx;\
	void*                   pEncStaticParams; \
    XDM1_BufDesc            inputBufDesc[OMX_AUDENC_MAX_INPUT_BUFFER_COUNT];\
    XDM1_BufDesc            *inputBufDescPtr;\
    XDM1_BufDesc            outputBufDesc[OMX_AUDENC_MAX_OUTPUT_BUFFER_COUNT];\
    XDM1_BufDesc            *outputBufDescPtr;\
    OMX_U32                 numBytesConsumed;\
    OMX_U32                 frameCounter;\
    OMX_S32                 cmdXdm;\
    XDAS_Int8               isfirstFrame;\
    XDAS_Int16              cmdStatus;\
    TIMM_OSAL_TRACEGRP      nTraceGroup; \
	OMX_BOOL                bFileMode; \
    OMX_BOOL                bCmdFlush;\
    OMX_BOOL                bEOSProcessOn;\
    OMX_BOOL                bCodecCreate;\
    OMX_BOOL                bInBufferCountChange;\
    OMX_BOOL                bOutBufferCountChange;\
    OMX_BOOL                bTaskExit;\
    OMX_PTR                 pDataNotifyMutex;\
    OMX_U32                 inBufArrayIndx; \
    OMX_U32                 outBufArrayIndx; \
    OMX_AUDENC_buf_header   *inBufHeaderArray; \
    OMX_AUDENC_buf_header   *outBufHeaderArray; \
	void (*fpSet_StaticParams) (OMX_HANDLETYPE hComponent, void* params); \
	void (*fpSet_DynamicParams) (void* dynamicparams); \
	void (*fpSet_Status) (void* status); \
	AUDENC_BufParams (*fpCalculate_oubuffdetails) (void *,OMX_U32 portIndex); \
    char  encoderName[OMX_MAX_STRINGNAME_SIZE]; 
ENDSTRUCT(OMX_AUDENC_PVTTYPE)


/** Callback from Base to Derived to Notify Command */
static OMX_ERRORTYPE OMX_TI_AUDENC_CommandNotify ( OMX_HANDLETYPE hComponent,
                                                   OMX_COMMANDTYPE Cmd,
                                                   OMX_U32 nParam,
                                                   OMX_PTR pCmdData );

/** Callback from Base to Derived to Notify Data */
static OMX_ERRORTYPE OMX_TI_AUDENC_DataNotify ( OMX_HANDLETYPE hComponent );

/** Function to set the parameters of the audenc Component*/
static OMX_ERRORTYPE OMX_TI_AUDENC_SetParameter ( OMX_HANDLETYPE hComponent,
                                                  OMX_INDEXTYPE nParamIndex,
                                                  OMX_PTR pParamStruct );

/** Function to get the parameters from the component */
static OMX_ERRORTYPE OMX_TI_AUDENC_GetParameter ( OMX_HANDLETYPE hComponent,
                                                  OMX_INDEXTYPE nParamIndex,
                                                  OMX_PTR pParamStruct );

/** Function to set the Configurations of the Component */
static OMX_ERRORTYPE OMX_TI_AUDENC_SetConfig ( OMX_HANDLETYPE hComponent,
                                               OMX_INDEXTYPE nIndex,
                                               OMX_PTR pConfigData );

/** Function to get the Configurations of the component */
static OMX_ERRORTYPE OMX_TI_AUDENC_GetConfig ( OMX_HANDLETYPE hComponent,
                                               OMX_INDEXTYPE nIndex,
                                               OMX_PTR pConfigData );

/** Function to deinitalize the audenc component */
static OMX_ERRORTYPE OMX_TI_AUDENC_ComponentDeinit ( OMX_HANDLETYPE
                                                     hComponent );

static OMX_ERRORTYPE OMX_TI_AUDENC_SetDefault ( OMX_AUDENC_PVTTYPE *
                                                pVidEncComPvt, OMX_AUDIO_CODINGTYPE compressionFormat );

static OMX_ERRORTYPE OMX_TI_AUDENC_AllocateBuffer ( OMX_IN OMX_HANDLETYPE
                                                    hComponent,
                                                    OMX_INOUT
                                                    OMX_BUFFERHEADERTYPE **
                                                    ppBuffer,
                                                    OMX_IN OMX_U32 nPortIndex,
                                                    OMX_IN OMX_PTR pAppPrivate,
                                                    OMX_IN OMX_U32 nSizeBytes );

static OMX_ERRORTYPE OMX_TI_AUDENC_UseBuffer ( OMX_IN OMX_HANDLETYPE hComponent,
                                               OMX_INOUT OMX_BUFFERHEADERTYPE **
                                               ppBufferHdr,
                                               OMX_IN OMX_U32 nPortIndex,
                                               OMX_IN OMX_PTR pAppPrivate,
                                               OMX_IN OMX_U32 nSizeBytes,
                                               OMX_IN OMX_U8 * pBuffer );

static OMX_ERRORTYPE OMX_TI_AUDENC_FreeBuffer ( OMX_IN OMX_HANDLETYPE
                                                hComponent,
                                                OMX_IN OMX_U32 nPortIndex,
                                                OMX_IN OMX_BUFFERHEADERTYPE *
                                                pBuffer );

static OMX_ERRORTYPE _OMX_TI_AUDENC_InitialzeComponentExtraDataInfo ( OMX_HANDLETYPE
                                                                      hComponent );
                                                                      
OMX_ERRORTYPE OMX_TI_AUDENC_Process (void *hComponent, int16_t ctrlStatus );

OMX_ERRORTYPE OMX_TI_AUDENC_ComponentInit_Common ( OMX_HANDLETYPE hComponent,
                                                   OMX_AUDIO_CODINGTYPE
                                                   compressionFormat );

OMX_ERRORTYPE OMX_TI_AUDENC_InitCallbacks ( OMX_HANDLETYPE hComponent,
                                         OMX_U32 compressionFormat);

OMX_U32 OMX_TI_AUDENC_SetChannel(OMX_HANDLETYPE hComponent);										

                                                                      
#endif
