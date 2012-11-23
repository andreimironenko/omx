/* ====================================================================
 *             Texas Instruments OMAP(TM) Platform Software
 * (c) Copyright Texas Instruments, Incorporated. All Rights Reserved.
 *
 * Use of this software is controlled by the terms and conditions found
 * in the license agreement under which this software has been supplied.
 * ==================================================================== */

/* -------------------------------------------------------------------------- */
/*
 * @file:Omx_ti_audio.h
 * This header defines the structures specific to the param or config indices of Openmax Audio Component.
 *
 * @path:
 * \ti\omx\interfaces\openMaxv11\
 * -------------------------------------------------------------------------- */


#ifndef OMX_TI_AUDIO_H
#define OMX_TI_AUDIO_H

#include <ti/omx/interfaces/openMaxv11/OMX_Core.h>

#include <ti/sdo/ce/audio1/auddec1.h>
#include <ti/sdo/ce/audio1/audenc1.h>

#ifdef CODEC_MP3DEC
#include <ti/sdo/codecs/mp3dec/imp3dec.h>
#endif

#ifdef CODEC_AACDEC
#include <ti/sdo/codecs/aaclcdec/iaacdec.h>
#endif

#ifdef CODEC_AACENC
#include <ti/sdo/codecs/aaclcenc/imp4aacenc.h>
#endif

/**
 * Custom Param for Codec Static Params (Create-time parameters)
 * NOTE: This will be used in OMX_SetParameter
 * STRUCT MEMBERS:
 *  nSize      : Size of the structure in bytes
 *  nVersion   : OMX specification version information
 *  nPortIndex : Port that this structure applies to
 *  audioStaticParams : Specifies the create time parameters for the codecs
 */
typedef union audioStaticParams {
#ifdef CODEC_MP3DEC
    IMP3DEC_Params mp3DecStaticParams;
#endif
#ifdef CODEC_AACDEC
    IAACDEC_Params aacDecStaticParams;
#endif
#ifdef CODEC_AACENC
    IMP4AACENC_Params aacEncStaticParams;
#endif
} uAudioStaticParams;
typedef struct OMX_AUDIO_PARAM_STATICPARAMS {
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nPortIndex;
    uAudioStaticParams audioStaticParams;
} OMX_AUDIO_PARAM_STATICPARAMS;
/* To allow CONFIG/PARAM to be used interchangeably */
#define OMX_AUDIO_CONFIG_STATICPARAMS OMX_AUDIO_PARAM_STATICPARAMS

/**
 * Custom Param for Codec Dynamic Params (Run-time parameters)
 * NOTE: This will be used in OMX_SetParameter
 * STRUCT MEMBERS:
 *  nSize      : Size of the structure in bytes
 *  nVersion   : OMX specification version information
 *  nPortIndex : Port that this structure applies to
 *  audioDynamicParams : Specifies the run time parameters for the codecs
 */
typedef union audioDynamicParams {
#ifdef CODEC_MP3DEC
    IMP3DEC_DynamicParams mp3DecDynamicParams;
#endif
#ifdef CODEC_AACDEC
    IAACDEC_DynamicParams aacDecDynamicParams;
#endif
#ifdef CODEC_AACENC
    IMP4AACENC_DynamicParams aacEncDynamicParams;
#endif
} uAudioDynamicParams;
typedef struct OMX_AUDIO_PARAM_DYNAMICPARAMS {
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nPortIndex;
    uAudioDynamicParams audioDynamicParams;
} OMX_AUDIO_PARAM_DYNAMICPARAMS;
/* To allow CONFIG/PARAM to be used interchangeably */
#define OMX_AUDIO_CONFIG_DYNAMICPARAMS OMX_AUDIO_PARAM_DYNAMICPARAMS

/**
 * Custom Param for Codec Status Params (query execution parameters)
 * NOTE: This will be used in OMX_GetParameter
 * STRUCT MEMBERS:
 *  nSize      : Size of the structure in bytes
 *  nVersion   : OMX specification version information
 *  nPortIndex : Port that this structure applies to
 *  audioStatus : Used for querying run time parameters for the codecs
 */
typedef union audioStatus {
#ifdef CODEC_MP3DEC
    IMP3DEC_Status mp3DecStatus;
#endif
#ifdef CODEC_AACDEC
    IAACDEC_Status aacDecStatus;
#endif
#ifdef CODEC_AACENC
    IMP4AACENC_Status aacEncStatus;
#endif
} uAudioStatus;
typedef struct OMX_AUDIO_PARAM_STATUS {
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nPortIndex;
    uAudioStatus AudioStatus;
} OMX_AUDIO_PARAM_STATUS;
/* To allow CONFIG/PARAM to be used interchangeably */
#define OMX_AUDIO_CONFIG_STATUS OMX_AUDIO_PARAM_STATUS


#endif /* OMX_TI_AUDIO_H */


