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

/*
 * OmxIndex
 */
/* OMX_INDEXTYPE */
typedef enum OmxIndex_OMX_INDEXTYPE_e
{
    OmxIndex_OMX_IndexComponentStartUnused = 0x01000000,
    OmxIndex_OMX_IndexParamPriorityMgmt,
    OmxIndex_OMX_IndexParamAudioInit,
    OmxIndex_OMX_IndexParamImageInit,
    OmxIndex_OMX_IndexParamVideoInit,
    OmxIndex_OMX_IndexParamOtherInit,
    OmxIndex_OMX_IndexParamNumAvailableStreams,
    OmxIndex_OMX_IndexParamActiveStream,
    OmxIndex_OMX_IndexParamSuspensionPolicy,
    OmxIndex_OMX_IndexParamComponentSuspended,
    OmxIndex_OMX_IndexConfigCapturing,
    OmxIndex_OMX_IndexConfigCaptureMode,
    OmxIndex_OMX_IndexAutoPauseAfterCapture,
    OmxIndex_OMX_IndexParamContentURI,
    OmxIndex_OMX_IndexParamCustomContentPipe,
    OmxIndex_OMX_IndexParamDisableResourceConcealment,
    OmxIndex_OMX_IndexConfigMetadataItemCount,
    OmxIndex_OMX_IndexConfigContainerNodeCount,
    OmxIndex_OMX_IndexConfigMetadataItem,
    OmxIndex_OMX_IndexConfigCounterNodeID,
    OmxIndex_OMX_IndexParamMetadataFilterType,
    OmxIndex_OMX_IndexParamMetadataKeyFilter,
    OmxIndex_OMX_IndexConfigPriorityMgmt,
    OmxIndex_OMX_IndexParamStandardComponentRole,
    OmxIndex_OMX_IndexPortStartUnused = 0x02000000,
    OmxIndex_OMX_IndexParamPortDefinition,
    OmxIndex_OMX_IndexParamCompBufferSupplier,
    OmxIndex_OMX_IndexReservedStartUnused = 0x03000000,
    OmxIndex_OMX_IndexAudioStartUnused = 0x04000000,
    OmxIndex_OMX_IndexParamAudioPortFormat,
    OmxIndex_OMX_IndexParamAudioPcm,
    OmxIndex_OMX_IndexParamAudioAac,
    OmxIndex_OMX_IndexParamAudioRa,
    OmxIndex_OMX_IndexParamAudioMp3,
    OmxIndex_OMX_IndexParamAudioAdpcm,
    OmxIndex_OMX_IndexParamAudioG723,
    OmxIndex_OMX_IndexParamAudioG729,
    OmxIndex_OMX_IndexParamAudioAmr,
    OmxIndex_OMX_IndexParamAudioWma,
    OmxIndex_OMX_IndexParamAudioSbc,
    OmxIndex_OMX_IndexParamAudioMidi,
    OmxIndex_OMX_IndexParamAudioGsm_FR,
    OmxIndex_OMX_IndexParamAudioMidiLoadUserSound,
    OmxIndex_OMX_IndexParamAudioG726,
    OmxIndex_OMX_IndexParamAudioGsm_EFR,
    OmxIndex_OMX_IndexParamAudioGsm_HR,
    OmxIndex_OMX_IndexParamAudioPdc_FR,
    OmxIndex_OMX_IndexParamAudioPdc_EFR,
    OmxIndex_OMX_IndexParamAudioPdc_HR,
    OmxIndex_OMX_IndexParamAudioTdma_FR,
    OmxIndex_OMX_IndexParamAudioTdma_EFR,
    OmxIndex_OMX_IndexParamAudioQcelp8,
    OmxIndex_OMX_IndexParamAudioQcelp13,
    OmxIndex_OMX_IndexParamAudioEvrc,
    OmxIndex_OMX_IndexParamAudioSmv,
    OmxIndex_OMX_IndexParamAudioVorbis,
    OmxIndex_OMX_IndexConfigAudioMidiImmediateEvent,
    OmxIndex_OMX_IndexConfigAudioMidiControl,
    OmxIndex_OMX_IndexConfigAudioMidiSoundBankProgram,
    OmxIndex_OMX_IndexConfigAudioMidiStatus,
    OmxIndex_OMX_IndexConfigAudioMidiMetaEvent,
    OmxIndex_OMX_IndexConfigAudioMidiMetaEventData,
    OmxIndex_OMX_IndexConfigAudioVolume,
    OmxIndex_OMX_IndexConfigAudioBalance,
    OmxIndex_OMX_IndexConfigAudioChannelMute,
    OmxIndex_OMX_IndexConfigAudioMute,
    OmxIndex_OMX_IndexConfigAudioLoudness,
    OmxIndex_OMX_IndexConfigAudioEchoCancelation,
    OmxIndex_OMX_IndexConfigAudioNoiseReduction,
    OmxIndex_OMX_IndexConfigAudioBass,
    OmxIndex_OMX_IndexConfigAudioTreble,
    OmxIndex_OMX_IndexConfigAudioStereoWidening,
    OmxIndex_OMX_IndexConfigAudioChorus,
    OmxIndex_OMX_IndexConfigAudioEqualizer,
    OmxIndex_OMX_IndexConfigAudioReverberation,
    OmxIndex_OMX_IndexConfigAudioChannelVolume,
    OmxIndex_OMX_IndexImageStartUnused = 0x05000000,
    OmxIndex_OMX_IndexParamImagePortFormat,
    OmxIndex_OMX_IndexParamFlashControl,
    OmxIndex_OMX_IndexConfigFocusControl,
    OmxIndex_OMX_IndexParamQFactor,
    OmxIndex_OMX_IndexParamQuantizationTable,
    OmxIndex_OMX_IndexParamHuffmanTable,
    OmxIndex_OMX_IndexConfigFlashControl,
    OmxIndex_OMX_IndexVideoStartUnused = 0x06000000,
    OmxIndex_OMX_IndexParamVideoPortFormat,
    OmxIndex_OMX_IndexParamVideoQuantization,
    OmxIndex_OMX_IndexParamVideoFastUpdate,
    OmxIndex_OMX_IndexParamVideoBitrate,
    OmxIndex_OMX_IndexParamVideoMotionVector,
    OmxIndex_OMX_IndexParamVideoIntraRefresh,
    OmxIndex_OMX_IndexParamVideoErrorCorrection,
    OmxIndex_OMX_IndexParamVideoVBSMC,
    OmxIndex_OMX_IndexParamVideoMpeg2,
    OmxIndex_OMX_IndexParamVideoMpeg4,
    OmxIndex_OMX_IndexParamVideoWmv,
    OmxIndex_OMX_IndexParamVideoRv,
    OmxIndex_OMX_IndexParamVideoAvc,
    OmxIndex_OMX_IndexParamVideoH263,
    OmxIndex_OMX_IndexParamVideoProfileLevelQuerySupported,
    OmxIndex_OMX_IndexParamVideoProfileLevelCurrent,
    OmxIndex_OMX_IndexConfigVideoBitrate,
    OmxIndex_OMX_IndexConfigVideoFramerate,
    OmxIndex_OMX_IndexConfigVideoIntraVOPRefresh,
    OmxIndex_OMX_IndexConfigVideoIntraMBRefresh,
    OmxIndex_OMX_IndexConfigVideoMBErrorReporting,
    OmxIndex_OMX_IndexParamVideoMacroblocksPerFrame,
    OmxIndex_OMX_IndexConfigVideoMacroBlockErrorMap,
    OmxIndex_OMX_IndexParamVideoSliceFMO,
    OmxIndex_OMX_IndexConfigVideoAVCIntraPeriod,
    OmxIndex_OMX_IndexConfigVideoNalSize,
    OmxIndex_OMX_IndexCommonStartUnused = 0x07000000,
    OmxIndex_OMX_IndexParamCommonDeblocking,
    OmxIndex_OMX_IndexParamCommonSensorMode,
    OmxIndex_OMX_IndexParamCommonInterleave,
    OmxIndex_OMX_IndexConfigCommonColorFormatConversion,
    OmxIndex_OMX_IndexConfigCommonScale,
    OmxIndex_OMX_IndexConfigCommonImageFilter,
    OmxIndex_OMX_IndexConfigCommonColorEnhancement,
    OmxIndex_OMX_IndexConfigCommonColorKey,
    OmxIndex_OMX_IndexConfigCommonColorBlend,
    OmxIndex_OMX_IndexConfigCommonFrameStabilisation,
    OmxIndex_OMX_IndexConfigCommonRotate,
    OmxIndex_OMX_IndexConfigCommonMirror,
    OmxIndex_OMX_IndexConfigCommonOutputPosition,
    OmxIndex_OMX_IndexConfigCommonInputCrop,
    OmxIndex_OMX_IndexConfigCommonOutputCrop,
    OmxIndex_OMX_IndexConfigCommonDigitalZoom,
    OmxIndex_OMX_IndexConfigCommonOpticalZoom,
    OmxIndex_OMX_IndexConfigCommonWhiteBalance,
    OmxIndex_OMX_IndexConfigCommonExposure,
    OmxIndex_OMX_IndexConfigCommonContrast,
    OmxIndex_OMX_IndexConfigCommonBrightness,
    OmxIndex_OMX_IndexConfigCommonBacklight,
    OmxIndex_OMX_IndexConfigCommonGamma,
    OmxIndex_OMX_IndexConfigCommonSaturation,
    OmxIndex_OMX_IndexConfigCommonLightness,
    OmxIndex_OMX_IndexConfigCommonExclusionRect,
    OmxIndex_OMX_IndexConfigCommonDithering,
    OmxIndex_OMX_IndexConfigCommonPlaneBlend,
    OmxIndex_OMX_IndexConfigCommonExposureValue,
    OmxIndex_OMX_IndexConfigCommonOutputSize,
    OmxIndex_OMX_IndexParamCommonExtraQuantData,
    OmxIndex_OMX_IndexConfigCommonFocusRegion,
    OmxIndex_OMX_IndexConfigCommonFocusStatus,
    OmxIndex_OMX_IndexConfigCommonTransitionEffect,
    OmxIndex_OMX_IndexOtherStartUnused = 0x08000000,
    OmxIndex_OMX_IndexParamOtherPortFormat,
    OmxIndex_OMX_IndexConfigOtherPower,
    OmxIndex_OMX_IndexConfigOtherStats,
    OmxIndex_OMX_IndexTimeStartUnused = 0x09000000,
    OmxIndex_OMX_IndexConfigTimeScale,
    OmxIndex_OMX_IndexConfigTimeClockState,
    OmxIndex_OMX_IndexConfigTimeActiveRefClock,
    OmxIndex_OMX_IndexConfigTimeCurrentMediaTime,
    OmxIndex_OMX_IndexConfigTimeCurrentWallTime,
    OmxIndex_OMX_IndexConfigTimeCurrentAudioReference,
    OmxIndex_OMX_IndexConfigTimeCurrentVideoReference,
    OmxIndex_OMX_IndexConfigTimeMediaTimeRequest,
    OmxIndex_OMX_IndexConfigTimeClientStartTime,
    OmxIndex_OMX_IndexConfigTimePosition,
    OmxIndex_OMX_IndexConfigTimeSeekMode,
    OmxIndex_OMX_IndexKhronosExtensions = 0x6F000000,
    OmxIndex_OMX_IndexVendorStartUnused = 0x7F000000,
    OmxIndex_OMX_IndexMax = 0x7FFFFFFF
} OmxIndex_OMX_INDEXTYPE;

/* 
 * OmxTypes
 */
 
/* OMX_U8 */
typedef xdc_UChar OmxTypes_OMX_U8;

/* OMX_S8 */
typedef xdc_Char OmxTypes_OMX_S8;

/* OMX_U16 */
typedef xdc_UInt16 OmxTypes_OMX_U16;

/* OMX_S16 */
typedef xdc_Int16 OmxTypes_OMX_S16;

/* OMX_U32 */
typedef xdc_UInt32 OmxTypes_OMX_U32;

/* OMX_S32 */
typedef xdc_Int32 OmxTypes_OMX_S32;

/* OMX_U64 */
typedef xdc_UInt64 OmxTypes_OMX_U64;

/* OMX_S64 */
typedef xdc_Int64 OmxTypes_OMX_S64;

/* OMX_BOOL */
typedef enum OmxTypes_OMX_BOOL_s
{
    OmxTypes_OMX_FALSE = 0,
    OmxTypes_OMX_TRUE = 1,
    OmxTypes_OMX_BOOL_MAX = 0x7FFFFFFF
} OmxTypes_OMX_BOOL;

/* OMX_PTR */
typedef xdc_Void* OmxTypes_OMX_PTR;

/* OMX_STRING */
typedef xdc_Char* OmxTypes_OMX_STRING;

/* OMX_BYTE */
typedef xdc_UChar* OmxTypes_OMX_BYTE;

/* OMX_UUIDTYPE */
typedef xdc_UChar OmxTypes_OMX_UUIDTYPE[128];

/* OMX_DIRTYPE */
typedef enum OmxTypes_OMX_DIRTYPE_s
{
    OmxTypes_OMX_DirInput,
    OmxTypes_OMX_DirOutput,
    OmxTypes_OMX_DirMax = 0x7FFFFFFF
} OmxTypes_OMX_DIRTYPE;

/* OMX_ENDIANTYPE */
typedef enum OmxTypes_OMX_ENDIANTYPE_s
{
    OmxTypes_OMX_EndianBig,
    OmxTypes_OMX_EndianLittle,
    OmxTypes_OMX_EndianMax = 0x7FFFFFFF
} OmxTypes_OMX_ENDIANTYPE;

/* OMX_NUMERICALDATATYPE */
typedef enum OmxTypes_OMX_NUMERICALDATATYPE_e
{
    OmxTypes_OMX_NumericalDataSigned,
    OmxTypes_OMX_NumericalDataUnsigned,
    OmxTypes_OMX_NumercialDataMax = 0x7FFFFFFF
} OmxTypes_OMX_NUMERICALDATATYPE;

/* OMX_BU32 */
typedef struct OmxTypes_OMX_BU32_s
{
    OmxTypes_OMX_U32 nValue;
    OmxTypes_OMX_U32 nMin;
    OmxTypes_OMX_U32 nMax;
} OmxTypes_OMX_BU32;

/* OMX_BS32 */
typedef struct OmxTypes_OMX_BS32_s
{
    OmxTypes_OMX_S32 nValue;
    OmxTypes_OMX_S32 nMin;
    OmxTypes_OMX_S32 nMax;
} OmxTypes_OMX_BS32;

/* OMX_TICKS */
typedef OmxTypes_OMX_S64 OmxTypes_OMX_TICKS;

/* OMX_HANDLETYPE */
typedef xdc_Void* OmxTypes_OMX_HANDLETYPE;

/* OMX_MARKTYPE */
typedef struct OmxTypes_OMX_MARKTYPE_s
{
    OmxTypes_OMX_HANDLETYPE hMarkTargetComponent;
    OmxTypes_OMX_PTR pMarkData;
} OmxTypes_OMX_MARKTYPE;

/* OMX_NATIVE_DEVICETYPE */
typedef xdc_Void* OmxTypes_OMX_NATIVE_DEVICETYPE;

/* OMX_NATIVE_WINDOWTYPE */
typedef xdc_Void* OmxTypes_OMX_NATIVE_WINDOWTYPE;

/* __struct__1 */
typedef struct OmxTypes___struct__1_s
{
    OmxTypes_OMX_U8 nVersionMajor;
    OmxTypes_OMX_U8 nVersionMinor;
    OmxTypes_OMX_U8 nRevision;
    OmxTypes_OMX_U8 nStep;
} OmxTypes___struct__1;

/* OMX_VERSIONTYPE */
typedef union OmxTypes_OMX_VERSIONTYPE_u
{
    OmxTypes___struct__1 s;
    OmxTypes_OMX_U32 nVersion;
} OmxTypes_OMX_VERSIONTYPE;

/*
 * OmxCore
 */

/* OMX_COMMANDTYPE */
typedef enum OmxCore_OMX_COMMANDTYPE_e
{
    OmxCore_OMX_CommandStateSet,
    OmxCore_OMX_CommandFlush,
    OmxCore_OMX_CommandPortDisable,
    OmxCore_OMX_CommandPortEnable,
    OmxCore_OMX_CommandMarkBuffer,
    OmxCore_OMX_CommandKhronosExtensions = 0x6F000000,
    OmxCore_OMX_CommandVendorStartUnused = 0x7F000000,
    OmxCore_OMX_CommandMax = 0X7FFFFFFF
} OmxCore_OMX_COMMANDTYPE;

/* OMX_STATETYPE */
typedef enum OmxCore_OMX_STATETYPE_e
{
    OmxCore_OMX_StateInvalid,
    OmxCore_OMX_StateLoaded,
    OmxCore_OMX_StateIdle,
    OmxCore_OMX_StateExecuting,
    OmxCore_OMX_StatePause,
    OmxCore_OMX_StateWaitForResources,
    OmxCore_OMX_StateKhronosExtensions = 0x6F000000,
    OmxCore_OMX_StateVendorStartUnused = 0x7F000000,
    OmxCore_OMX_StateMax = 0X7FFFFFFF
} OmxCore_OMX_STATETYPE;

/* OMX_ERRORTYPE */
typedef enum OmxCore_OMX_ERRORTYPE
{
    OmxCore_OMX_ErrorNone = 0,
    OmxCore_OMX_ErrorInsufficientResources = 0x80001000,
    OmxCore_OMX_ErrorUndefined = 0x80001001,
    OmxCore_OMX_ErrorInvalidComponentName = 0x80001002,
    OmxCore_OMX_ErrorComponentNotFound = 0x80001003,
    OmxCore_OMX_ErrorInvalidComponent = 0x80001004,
    OmxCore_OMX_ErrorBadParameter = 0x80001005,
    OmxCore_OMX_ErrorNotImplemented = 0x80001006,
    OmxCore_OMX_ErrorUnderflow = 0x80001007,
    OmxCore_OMX_ErrorOverflow = 0x80001008,
    OmxCore_OMX_ErrorHardware = 0x80001009,
    OmxCore_OMX_ErrorInvalidState = 0x8000100A,
    OmxCore_OMX_ErrorStreamCorrupt = 0x8000100B,
    OmxCore_OMX_ErrorPortsNotCompatible = 0x8000100C,
    OmxCore_OMX_ErrorResourcesLost = 0x8000100D,
    OmxCore_OMX_ErrorNoMore = 0x8000100E,
    OmxCore_OMX_ErrorVersionMismatch = 0x8000100F,
    OmxCore_OMX_ErrorNotReady = 0x80001010,
    OmxCore_OMX_ErrorTimeout = 0x80001011,
    OmxCore_OMX_ErrorSameState = 0x80001012,
    OmxCore_OMX_ErrorResourcesPreempted = 0x80001013,
    OmxCore_OMX_ErrorPortUnresponsiveDuringAllocation = 0x80001014,
    OmxCore_OMX_ErrorPortUnresponsiveDuringDeallocation = 0x80001015,
    OmxCore_OMX_ErrorPortUnresponsiveDuringStop = 0x80001016,
    OmxCore_OMX_ErrorIncorrectStateTransition = 0x80001017,
    OmxCore_OMX_ErrorIncorrectStateOperation = 0x80001018,
    OmxCore_OMX_ErrorUnsupportedSetting = 0x80001019,
    OmxCore_OMX_ErrorUnsupportedIndex = 0x8000101A,
    OmxCore_OMX_ErrorBadPortIndex = 0x8000101B,
    OmxCore_OMX_ErrorPortUnpopulated = 0x8000101C,
    OmxCore_OMX_ErrorComponentSuspended = 0x8000101D,
    OmxCore_OMX_ErrorDynamicResourcesUnavailable = 0x8000101E,
    OmxCore_OMX_ErrorMbErrorsInFrame = 0x8000101F,
    OmxCore_OMX_ErrorFormatNotDetected = 0x80001020,
    OmxCore_OMX_ErrorContentPipeOpenFailed = 0x80001021,
    OmxCore_OMX_ErrorContentPipeCreationFailed = 0x80001022,
    OmxCore_OMX_ErrorSeperateTablesUsed = 0x80001023,
    OmxCore_OMX_ErrorTunnelingUnsupported = 0x80001024,
    OmxCore_OMX_ErrorKhronosExtensions = 0x8F000000,
    OmxCore_OMX_ErrorVendorStartUnused = 0x90000000,
    OmxCore_OMX_ErrorMax = 0x7FFFFFFF
} OmxCore_OMX_ERRORTYPE;

/* OMX_COMPONENTINITTYPE */
typedef OmxCore_OMX_ERRORTYPE (*OmxCore_OMX_COMPONENTINITTYPE)(OmxTypes_OMX_HANDLETYPE);

/* OMX_COMPONENTREGISTERTYPE */
typedef struct OmxCore_OMX_COMPONENTREGISTERTYPE_s
{
    const xdc_Char *pName;
    OmxCore_OMX_COMPONENTINITTYPE *pInitialize;
} OmxCore_OMX_COMPONENTREGISTERTYPE;

/* OMX_PRIORITYMGMTTYPE */
typedef struct OmxCore_OMX_PRIORITYMGMTTYPE_s
{
    OmxTypes_OMX_U32 nSize;
    OmxTypes_OMX_VERSIONTYPE nVersion;
    OmxTypes_OMX_U32 nGroupPriority;
    OmxTypes_OMX_U32 nGroupID;
} OmxCore_OMX_PRIORITYMGMTTYPE;

/* OMX_PARAM_COMPONENTROLETYPE */
typedef struct OmxCore_OMX_PARAM_COMPONENTROLETYPE_s
{
    OmxTypes_OMX_U32 nSize;
    OmxTypes_OMX_VERSIONTYPE nVersion;
    OmxTypes_OMX_U8 cRole[128];
} OmxCore_OMX_PARAM_COMPONENTROLETYPE;

/* OMX_BUFFERFLAG_EOS */
#define OmxCore_OMX_BUFFERFLAG_EOS (0x00000001)

/* OMX_BUFFERFLAG_STARTTIME */
#define OmxCore_OMX_BUFFERFLAG_STARTTIME (0x00000002)

/* OMX_BUFFERFLAG_DECODEONLY */
#define OmxCore_OMX_BUFFERFLAG_DECODEONLY (0x00000004)

/* OMX_BUFFERFLAG_DATACORRUPT */
#define OmxCore_OMX_BUFFERFLAG_DATACORRUPT (0x00000008)

/* OMX_BUFFERFLAG_ENDOFFRAME */
#define OmxCore_OMX_BUFFERFLAG_ENDOFFRAME (0x00000010)

/* OMX_BUFFERFLAG_SYNCFRAME */
#define OmxCore_OMX_BUFFERFLAG_SYNCFRAME (0x00000020)

/* OMX_BUFFERFLAG_EXTRADATA */
#define OmxCore_OMX_BUFFERFLAG_EXTRADATA (0x00000040)

/* OMX_BUFFERFLAG_CODECCONFIG */
#define OmxCore_OMX_BUFFERFLAG_CODECCONFIG (0x00000080)

/* OMX_BUFFERHEADERTYPE */
typedef struct OmxCore_OMX_BUFFERHEADERTYPE_s
{
    OmxTypes_OMX_U32 nSize;
    OmxTypes_OMX_VERSIONTYPE nVersion;
    OmxTypes_OMX_U8* pBuffer;
    OmxTypes_OMX_U32 nAllocLen;
    OmxTypes_OMX_U32 nFilledLen;
    OmxTypes_OMX_U32 nOffset;
    OmxTypes_OMX_PTR pAppPrivate;
    OmxTypes_OMX_PTR pPlatformPrivate;
    OmxTypes_OMX_PTR pInputPortPrivate;
    OmxTypes_OMX_PTR pOutputPortPrivate;
    OmxTypes_OMX_HANDLETYPE hMarkTargetComponent;
    OmxTypes_OMX_PTR pMarkData;
    OmxTypes_OMX_U32 nTickCount;
    OmxTypes_OMX_TICKS nTimeStamp;
    OmxTypes_OMX_U32 nFlags;
    OmxTypes_OMX_U32 nOutputPortIndex;
    OmxTypes_OMX_U32 nInputPortIndex;
} OmxCore_OMX_BUFFERHEADERTYPE;

/* OMX_EXTRADATATYPE */
typedef enum OmxCore_OMX_EXTRADATATYPE_s
{
    OmxCore_OMX_ExtraDataNone = 0,
    OmxCore_OMX_ExtraDataQuantization,
    OmxCore_OMX_ExtraDataKhronosExtensions = 0x6F000000,
    OmxCore_OMX_ExtraDataVendorStartUnused = 0x7F000000,
    OmxCore_OMX_ExtraDataMax = 0x7FFFFFFF
} OmxCore_OMX_EXTRADATATYPE;

/* OMX_OTHER_EXTRADATATYPE */
typedef struct OmxCore_OMX_OTHER_EXTRADATATYPE_s
{
    OmxTypes_OMX_U32 nSize;
    OmxTypes_OMX_VERSIONTYPE nVersion;
    OmxTypes_OMX_U32 nPortIndex;
    OmxCore_OMX_EXTRADATATYPE eType;
    OmxTypes_OMX_U32 nDataSize;
    OmxTypes_OMX_U8 data[1];
} OmxCore_OMX_OTHER_EXTRADATATYPE;

/* OMX_PORT_PARAM_TYPE */
typedef struct OmxCore_OMX_PORT_PARAM_TYPE_s
{
    OmxTypes_OMX_U32 nSize;
    OmxTypes_OMX_VERSIONTYPE nVersion;
    OmxTypes_OMX_U32 nPorts;
    OmxTypes_OMX_U32 nStartPortNumber;
} OmxCore_OMX_PORT_PARAM_TYPE;

/* OMX_EVENTTYPE */
typedef enum OmxCore_OMX_EVENTTYPE_e
{
    OmxCore_OMX_EventCmdComplete,
    OmxCore_OMX_EventError,
    OmxCore_OMX_EventMark,
    OmxCore_OMX_EventPortSettingsChanged,
    OmxCore_OMX_EventBufferFlag,
    OmxCore_OMX_EventResourcesAcquired,
    OmxCore_OMX_EventComponentResumed,
    OmxCore_OMX_EventDynamicResourcesAvailable,
    OmxCore_OMX_EventPortFormatDetected,
    OmxCore_OMX_EventKhronosExtensions = 0x6F000000,
    OmxCore_OMX_EventVendorStartUnused = 0x7F000000,
    OmxCore_OMX_EventMax = 0x7FFFFFFF
} OmxCore_OMX_EVENTTYPE;

/* EventHandlerFcnPtr */
typedef OmxCore_OMX_ERRORTYPE (*OmxCore_EventHandlerFcnPtr)(OmxTypes_OMX_HANDLETYPE, OmxTypes_OMX_PTR, OmxCore_OMX_EVENTTYPE, OmxTypes_OMX_U32, OmxTypes_OMX_U32, OmxTypes_OMX_PTR);

/* EmptyBufferDoneFcnPtr */
typedef OmxCore_OMX_ERRORTYPE (*OmxCore_EmptyBufferDoneFcnPtr)(OmxTypes_OMX_HANDLETYPE, OmxTypes_OMX_PTR, OmxCore_OMX_BUFFERHEADERTYPE*);

/* FillBufferDoneFcnPtr */
typedef OmxCore_OMX_ERRORTYPE (*OmxCore_FillBufferDoneFcnPtr)(OmxTypes_OMX_HANDLETYPE, OmxTypes_OMX_PTR, OmxCore_OMX_BUFFERHEADERTYPE*);

/* OMX_CALLBACKTYPE */
typedef struct OmxCore_OMX_CALLBACKTYPE_s
{
    OmxCore_EventHandlerFcnPtr EventHandler;
    OmxCore_EmptyBufferDoneFcnPtr EmptyBufferDone;
} OmxCore_OMX_CALLBACKTYPE;

/* OMX_BUFFERSUPPLIERTYPE */
typedef enum OmxCore_OMX_BUFFERSUPPLIERTYPE_e
{
    OmxCore_OMX_BufferSupplyUnspecified = 0x0,
    OmxCore_OMX_BufferSupplyInput,
    OmxCore_OMX_BufferSupplyOutput,
    OmxCore_OMX_BufferSupplyKhronosExtensions = 0x6F000000,
    OmxCore_OMX_BufferSupplyVendorStartUnused = 0x7F000000,
    OmxCore_OMX_BufferSupplyMax = 0x7FFFFFFF
} OmxCore_OMX_BUFFERSUPPLIERTYPE;

/* OMX_PARAM_BUFFERSUPPLIERTYPE */
typedef struct OmxCore_OMX_PARAM_BUFFERSUPPLIERTYPE_s
{
    OmxTypes_OMX_U32 nSize;
    OmxTypes_OMX_VERSIONTYPE nVersion;
    OmxTypes_OMX_U32 nPortIndex;
    OmxCore_OMX_BUFFERSUPPLIERTYPE eBufferSupplier;
} OmxCore_OMX_PARAM_BUFFERSUPPLIERTYPE;

/* OMX_PORTTUNNELFLAG_READONLY */
#define OmxCore_OMX_PORTTUNNELFLAG_READONLY (0x00000001)

/* OMX_TUNNELSETUPTYPE */
typedef struct OmxCore_OMX_TUNNELSETUPTYPE_s
{
    OmxTypes_OMX_U32 nTunnelFlags;
    OmxCore_OMX_BUFFERSUPPLIERTYPE eSupplier;
} OmxCore_OMX_TUNNELSETUPTYPE;

 
