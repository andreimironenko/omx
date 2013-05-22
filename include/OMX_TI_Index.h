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
 *  @file  OMX_TI_Index.h
 *         This file contains the vendor(TI) specific indexes
 *
 *  @path \ti\omx\interfaces\openMaxv11\
 *
 *  @rev 1.0
 */

/*==============================================================
 *! Revision History
 *! ============================
 *! 20-Dec-2008 x0052661@ti.com, initial version
 *================================================================*/

#ifndef _OMX_TI_INDEX_H_
#define _OMX_TI_INDEX_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/******************************************************************
 *   INCLUDE FILES
 ******************************************************************/
#include <OMX_Types.h>
#include "OMX_Index.h"
#include "OMX_Core.h"


/*******************************************************************
 * EXTERNAL REFERENCE NOTE: only use if not found in header file
 *******************************************************************/
/*----------         function prototypes      ------------------- */
/*----------         data declarations        ------------------- */
/*******************************************************************
 * PUBLIC DECLARATIONS: defined here, used elsewhere
 *******************************************************************/
/*----------         function prototypes      ------------------- */
/*----------         data declarations        ------------------- */
#define OMX_TI_IndexBegin     ( (OMX_INDEXTYPE) ( OMX_IndexVendorStartUnused + 1 ) )

typedef enum OMX_TI_INDEXTYPE {

    OMX_IndexConfigAutoPauseAfterCapture = OMX_IndexAutoPauseAfterCapture,

    /* Vendor specific area for storing indices */
    OMX_TI_IndexConfigChannelName = OMX_TI_IndexBegin, /**< reference: OMX_CONFIG_CHANNELNAME */

    OMX_TI_IndexParamJPEGUncompressedMode,      /**< reference: OMX_JPEG_PARAM_UNCOMPRESSEDMODETYPE */
    OMX_TI_IndexParamJPEGCompressedMode,        /**< reference: OMX_JPEG_PARAM_COMPRESSEDMODETYPE */
    OMX_TI_IndexParamDecodeSubregion,           /**< reference: OMX_IMAGE_PARAM_DECODE_SUBREGION */

    /* H264 Encoder Indices*/
  OMX_TI_IndexParamVideoDataSyncMode, //!< Refer to OMX_VIDEO_PARAM_DATASYNCMODETYPE structure 	
  OMX_TI_IndexParamVideoBitStreamFormatSelect,	//!< use OMX_VIDEO_PARAM_AVCBITSTREAMFORMATTYPE to specify the stream format type
  OMX_TI_IndexParamVideoNALUsettings,	//!< use OMX_VIDEO_PARAM_AVCNALUCONTROLTYPE to configure the type os NALU to send along with the Different Frame Types	
  OMX_TI_IndexParamVideoMEBlockSize,	//!< use OMX_VIDEO_PARAM_MEBLOCKSIZETYPE to specify the minimum block size used for motion estimation
  OMX_TI_IndexParamVideoIntraPredictionSettings,	//!< use OMX_VIDEO_PARAM_INTRAPREDTYPE to configure the intra prediction modes used for different block sizes
  OMX_TI_IndexParamVideoEncoderPreset,	//!< use OMX_VIDEO_PARAM_ENCODER_PRESETTYPE to select the encoding mode & rate control preset
  OMX_TI_IndexParamVideoFrameDataContentSettings,	//!< use OMX_VIDEO_PARAM_FRAMEDATACONTENTTYPE to configure the data content tpye
  OMX_TI_IndexParamVideoTransformBlockSize,	//!< use OMX_VIDEO_PARAM_TRANSFORM_BLOCKSIZETYPE to specify the block size used for ttransformation	
  OMX_TI_IndexParamVideoVUIsettings, //!use OMX_VIDEO_PARAM_VUIINFOTYPE
  OMX_TI_IndexParamVideoAdvancedFMO,
  OMX_TI_IndexConfigVideoPixelInfo,	//!<  Use OMX_VIDEO_CONFIG_PIXELINFOTYPE structure to know the pixel aspectratio & pixel range
  OMX_TI_IndexConfigVideoMESearchRange,	//!< use OMX_VIDEO_CONFIG_MESEARCHRANGETYPE to specify the ME Search settings
  OMX_TI_IndexConfigVideoQPSettings,	//!< use OMX_TI_VIDEO_CONFIG_QPSETTINGS to specify the ME Search settings
  OMX_TI_IndexConfigSliceSettings,		//!<use OMX_VIDEO_CONFIG_SLICECODINGTYPE to specify the ME Search settings
    
  /* Custom (non-standard) video parameters and configuration */
  OMX_TI_IndexParamVideoStaticParams,  /**< reference: OMX_VIDEO_PARAM_STATICPARAMS */
  OMX_TI_IndexParamVideoDynamicParams,               /**< reference: OMX_VIDEO_PARAM_DYNAMICPARAMS */
  OMX_TI_IndexParamVideoStatus,                      /**< reference: OMX_VIDEO_PARAM_STATUS */
  OMX_TI_IndexConfigVideoStaticParams  = OMX_TI_IndexParamVideoStaticParams,  /**< reference: OMX_VIDEO_CONFIG_STATICPARAMS */
  OMX_TI_IndexConfigVideoDynamicParams = OMX_TI_IndexParamVideoDynamicParams, /**< reference: OMX_VIDEO_CONFIG_DYNAMICPARAMS */
  OMX_TI_IndexConfigVideoStatus = OMX_TI_IndexParamVideoStatus,               /**< reference: OMX_VIDEO_CONFIG_STATUS */
  OMX_TI_IndexVideoEncoderSet,                        /**< reference: OMX_VIDEO_ENCODER_SET */


  /* Camera Indices */
  OMX_IndexParamSensorSelect,                 /**< reference: OMX_CONFIG_SENSORSELECTTYPE */
  OMX_IndexConfigFlickerCancel,               /**< reference: OMX_CONFIG_FLICKERCANCELTYPE */
  OMX_IndexConfigSensorCal,                   /**< reference: OMX_CONFIG_SENSORCALTYPE */
  OMX_IndexConfigISOSetting, /**< reference: OMX_CONFIG_ISOSETTINGTYPE */
  OMX_IndexParamSceneMode,                    /**< reference: OMX_CONFIG_SCENEMODETYPE */

  OMX_IndexConfigDigitalZoomSpeed,            /**< reference: OMX_CONFIG_DIGITALZOOMSPEEDTYPE */
  OMX_IndexConfigDigitalZoomTarget,           /**< reference: OMX_CONFIG_DIGITALZOOMTARGETTYPE */

  OMX_IndexConfigCommonScaleQuality,          /**< reference: OMX_CONFIG_SCALEQUALITYTYPE */

  OMX_IndexConfigCommonDigitalZoomQuality,    /**< reference: OMX_CONFIG_SCALEQUALITYTYPE */

  OMX_IndexConfigOpticalZoomSpeed,            /**< reference: OMX_CONFIG_DIGITALZOOMSPEEDTYPE */
  OMX_IndexConfigOpticalZoomTarget,           /**< reference: OMX_CONFIG_DIGITALZOOMTARGETTYPE */

  OMX_IndexConfigSmoothZoom,                  /**< reference: OMX_CONFIG_SMOOTHZOOMTYPE */

  OMX_IndexConfigBlemish,                     /**< reference: OMX_CONFIG_BLEMISHTYPE */

  OMX_IndexConfigExtCaptureMode,              /**< reference: OMX_CONFIG_EXTCAPTUREMODETYPE */
  OMX_IndexConfigExtPrepareCapturing, /**< reference : OMX_CONFIG_BOOLEANTYPE */
  OMX_IndexConfigExtCapturing, /**< reference : OMX_CONFIG_EXTCAPTURING */

  OMX_IndexCameraOperatingMode, /**<  OMX_CONFIG_CAMOPERATINGMODETYPE */
  OMX_IndexConfigDigitalFlash,                /**< reference: OMX_CONFIG_BOOLEANTYPE */
  OMX_IndexConfigPrivacyIndicator,            /**< reference: OMX_CONFIG_BOOLEANTYPE */

  OMX_IndexConfigTorchMode,                   /**< reference: OMX_CONFIG_TORCHMODETYPE */

  OMX_IndexConfigSlowSync,                    /**< reference: OMX_CONFIG_BOOLEANTYPE */

  OMX_IndexConfigExtFocusRegion, /**< reference : OMX_CONFIG_EXTFOCUSREGIONTYPE */ 
  OMX_IndexConfigFocusAssist,                 /**< reference: OMX_CONFIG_BOOLEANTYPE */

  OMX_IndexConfigImageFocusLock,              /**< reference: OMX_IMAGE_CONFIG_LOCKTYPE */
  OMX_IndexConfigImageWhiteBalanceLock,       /**< reference: OMX_IMAGE_CONFIG_LOCKTYPE */
  OMX_IndexConfigImageExposureLock,           /**< reference: OMX_IMAGE_CONFIG_LOCKTYPE */
  OMX_IndexConfigImageAllLock,                /**< reference: OMX_IMAGE_CONFIG_LOCKTYPE */

  OMX_IndexConfigImageDeNoiseLevel,           /**< reference: OMX_IMAGE_CONFIG_PROCESSINGLEVELTYPE */
  OMX_IndexConfigSharpeningLevel,             /**< reference: OMX_IMAGE_CONFIG_PROCESSINGLEVELTYPE */
  OMX_IndexConfigDeBlurringLevel,             /**< reference: OMX_IMAGE_CONFIG_PROCESSINGLEVELTYPE */
  OMX_IndexConfigChromaCorrection,            /**< reference: OMX_IMAGE_CONFIG_PROCESSINGLEVELTYPE */
  OMX_IndexConfigDeMosaicingLevel,            /**< reference: OMX_IMAGE_CONFIG_PROCESSINGLEVELTYPE */

  OMX_IndexConfigCommonWhiteBalanceGain,      /**< reference: OMX_CONFIG_WHITEBALGAINTYPE */

  OMX_IndexConfigCommonRGB2RGB,               /**< reference: OMX_CONFIG_COLORCONVERSIONTYPE_II */
  OMX_IndexConfigCommonRGB2YUV,               /**< reference: OMX_CONFIG_COLORCONVERSIONTYPE_II */
  OMX_IndexConfigCommonYUV2RGB, /**< reference : OMX_CONFIG_EXT_COLORCONVERSIONTYPE */

  OMX_IndexConfigCommonGammaTable,            /**< reference: OMX_CONFIG_GAMMATABLETYPE */

  OMX_IndexConfigImageFaceDetection,          /**< reference: OMX_CONFIG_OBJDETECTIONTYPE */
  OMX_IndexConfigImageBarcodeDetection,       /**< reference: OMX_CONFIG_EXTRADATATYPE */
  OMX_IndexConfigImageSmileDetection,         /**< reference: OMX_CONFIG_OBJDETECTIONTYPE */
  OMX_IndexConfigImageBlinkDetection,         /**< reference: OMX_CONFIG_OBJDETECTIONTYPE */
  OMX_IndexConfigImageFrontObjectDetection,   /**< reference: OMX_CONFIG_EXTRADATATYPE */
  OMX_IndexConfigHistogramMeasurement,        /**< reference: OMX_CONFIG_HISTOGRAMTYPE */
  OMX_IndexConfigDistanceMeasurement,         /**< reference: OMX_CONFIG_EXTRADATATYPE */

  OMX_IndexConfigSnapshotToPreview,           /**< reference: OMX_CONFIG_BOOLEANTYPE */

  OMX_IndexConfigJpegHeaderType , /**< reference : OMX_CONFIG_JPEGHEEADERTYPE */
  OMX_IndexParamJpegMaxSize,                  /**< reference: OMX_IMAGE_JPEGMAXSIZE */

  OMX_IndexConfigRestartMarker,               /**< reference: OMX_CONFIG_BOOLEANTYPE */

  OMX_IndexParamImageStampOverlay,            /**< reference: OMX_PARAM_IMAGESTAMPOVERLAYTYPE */
  OMX_IndexParamThumbnail,                    /**< reference: OMX_PARAM_THUMBNAILTYPE */
  OMX_IndexConfigImageStabilization,          /**< reference: OMX_CONFIG_BOOLEANTYPE */
  OMX_IndexConfigMotionTriggeredImageStabilisation, /**< reference : OMX_CONFIG_BOOLEANTYPE */
  OMX_IndexConfigRedEyeRemoval,               /**< reference: OMX_CONFIG_REDEYEREMOVALTYPE */
  OMX_IndexParamHighISONoiseFiler,            /**< reference: OMX_CONFIG_BOOLEANTYPE */
  OMX_IndexParamLensDistortionCorrection,     /**< reference: OMX_CONFIG_BOOLEANTYPE */
  OMX_IndexParamLocalBrightnessAndContrast,   /**< reference: OMX_CONFIG_BOOLEANTYPE */
  OMX_IndexConfigChromaticAberrationCorrection, /**< reference: OMX_CONFIG_BOOLEANTYPE */
  OMX_IndexParamVideoCaptureYUVRange,         /**< reference: OMX_PARAM_VIDEOYUVRANGETYPE */

  OMX_IndexConfigFocusRegion,                 /**< reference: OMX_CONFIG_EXTFOCUSREGIONTYPE */
  OMX_IndexConfigImageMotionEstimation,       /**< reference: OMX_CONFIG_OBJDETECTIONTYPE */
  OMX_IndexParamProcessingOrder,              /**< reference: OMX_CONFIGPROCESSINGORDERTYPE */
  OMX_IndexParamFrameStabilisation,           /**< reference: OMX_CONFIG_BOOLEANTYPE */
  OMX_IndexParamVideoNoiseFilter,             /**< reference: OMX_PARAM_VIDEONOISEFILTERTYPE */

  OMX_TI_IndexParamVFDCDriverInstId,          /**< reference: OMX_PARAM_VFDC_DRIVERINSTID */
  OMX_TI_IndexParamVFDCCustomModeInfo,        /**< reference: OMX_PARAM_DC_CUSTOM_MODE_INFO */
  OMX_TI_IndexParamVFDCUpsampleInfo,          /**< reference: OMX_PARAM_VFDC_UPSAMPLE_INFO */  
  OMX_TI_IndexConfigChannelGroupInfo,         /**< reference: OMX_CONFIG_CHANNELGROUPINFO */
  OMX_TI_IndexConfigCallbackfxnInfo,			/**< reference: OMX_TI_IndexConfigCallbackfxnInfo*/
  OMX_TI_IndexConfigGetDomxCompInfo,          /**< reference: OMX_CONFIG_DOMXPROXYCOMPINFO */
  OMX_TI_IndexConfigOtherExtraDataControl,    /**< reference: OMX_CONFIG_EXTRADATATYPE */
  OMX_TI_IndexConfigVidChResolution,          /**< reference: OMX_CONFIG_VIDCHANNEL_RESOLUTION */
  OMX_TI_IndexParamBuffMemType,               /**< reference: OMX_PARAM_BUFFER_MEMORYTYPE */
  OMX_TI_IndexParamCompPortNotifyType,        /**< reference: OMX_PARAM_COMPPORT_NOTIFYTYPE */
  OMX_TI_IndexConfigVFDCMosaicPort2WinMap,    /**< reference: OMX_CONFIG_VFDC_MOSAICLAYOUT_PORT2WINMAP */
  OMX_TI_IndexParamVFDCCreateMosaicLayout,    /**< reference: OMX_PARAM_VFDC_CREATEMOSAICLAYOUT */
  OMX_TI_IndexParamVFPCNumChPerHandle,        /**< reference: OMX_PARAM_VFPC_NUMCHANNELPERHANDLE */
  OMX_TI_IndexConfigAlgEnable,                /**< reference: OMX_CONFIG_ALG_ENABLE */
  OMX_TI_IndexConfigSubSamplingFactor,        /**< reference: OMX_CONFIG_SUBSAMPLING_FACTOR */
  OMX_TI_IndexParamFileModeType,              /**< reference: OMX_PARAM_FILEMODE_TYPE */ 
  OMX_TI_IndexParamVFCCHwPortID,              /**< reference: OMX_PARAM_VFCC_HWPORT_ID */
  OMX_TI_IndexParamVFCCHwPortProperties,      /**< reference: OMX_PARAM_VFCC_HWPORT_PROPERTIES */
  OMX_TI_IndexConfigVFCCFrameSkip,            /**< reference: OMX_CONFIG_VFCC_FRAMESKIP_INFO */
  OMX_TI_IndexParamCTRLVidDecInfo,             /**< reference: OMX_PARAM_CTRL_VIDDECODER_INFO */
  OMX_IndexConfigVFCCPortStats,                /**< reference: OMX_PARAM_VIDEO_CAPTURE_STATS */
  OMX_IndexConfigVFCCPortResetStats,          /**< reference: OMX_CONFIG_VIDEO_CAPTURE_RESET_STATS */
  OMX_IndexParamCTRLVidDecCfg,                /**< reference: OMX_VIDEO_PARAM_CTRL_VIDDEC_CONFIG */
  OMX_TI_IndexConfigVSWMOSAICCreateMosaicLayout,/**< reference: OMX_PARAM_VSWMOSAIC_CREATEMOSAICLAYOUT */
  OMX_TI_IndexConfigVSWMOSAICSetBackgroundColor,/**< reference: OMX_PARAM_VSWMOSAIC_SETBACKGROUNDCOLOR */
  OMX_TI_IndexParamVSWMOSAICMosaicPeriodicity, /**< reference: OMX_CONFIG_VSWMOSAIC_MOSAIC_PERIODICITY */
  OMX_TI_IndexParamVSPLCPortMapInfo,            /**< reference: OMX_PARAM_VIDSPLC_PORTMAP_INFO */
  OMX_IndexParamFrameQBufferManagerInfo,      /**< reference: OMX_PARAM_FQMGR */
  OMX_TI_IndexParamAssignFrameQBufferManager, /**< reference: OMX_CONFIG_ASSIGNFQMGR */
  OMX_IndexParamDupPorts,                     /**< reference: OMX_PARAM_DUPEDPORTS */
  OMX_TI_IndexParamVFDCFieldMergeMode,        /**< reference: OMX_TI_IndexParamVFDCFieldMergeMode */
  OMX_TI_IndexParamResolutionChangeType,      /**< reference: OMX_PARAM_RESOLUTIONCHANGE_TYPE */
  OMX_TI_IndexConfigResolutionChangeType,     /**< reference: OMX_CONFIG_RESOLUTIONCHANGE_TYPE */
  OMX_TI_IndexConfigFrameRateControlType,     /**< reference: OMX_CONFIG_FRAMERATECONTROL_TYPE */
  OMX_TI_IndexConfigVFDCModifyMosaicLayout,   /**< reference: OMX_CONFIG_VFDC_MODIFYMOSAICLAYOUT */
  OMX_TI_IndexConfigTvpPortStatus,
  OMX_TI_IndexConfigGDRSettings,		//!<use OMX_VIDEO_CONFIG_GDRINFOTYPE to specify the GDR parameters
  OMX_TI_IndexMAX  = 0x7F0FFFFF               /**< Last Index Used by TI */
} OMX_TI_INDEXTYPE;



typedef enum OMX_TI_ERRORTYPE
{
    /* Vendor specific area for storing indices */
    
    /*Control attribute is pending - Dio_Dequeue will not work until attribute 
    is retreived*/
    OMX_TI_WarningAttributePending = (OMX_S32)((OMX_ERRORTYPE)OMX_ErrorVendorStartUnused + 1),
    /*Attribute buffer size is insufficient - reallocate the attribute buffer*/
    OMX_TI_WarningInsufficientAttributeSize,
    /*EOS buffer has been received*/
    OMX_TI_WarningEosReceived,
    
    OMX_TI_ErrorPortIsAlreadyDisabled,    
    OMX_TI_ErrorPortIsAlreadyEnabled,
    OMX_TI_Warning_Max = 0x7FFFFFFF
}OMX_TI_ERRORTYPE;

typedef enum OMX_TI_EVENTTYPE
{
    /* Vendor specific area for storing indices */
    /*Reference count for the buffer has changed. In the callback, nData1 will
    pBufferHeader, nData2 will be present count*/
    OMX_TI_EventBufferRefCount = (OMX_S32)((OMX_EVENTTYPE)OMX_EventVendorStartUnused + 1)
}OMX_TI_EVENTTYPE;

/*******************************************************************
 * PRIVATE DECLARATIONS: defined here, used only here
 *******************************************************************/
/*----------          data declarations        ------------------- */
/*----------          function prototypes      ------------------- */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _OMX_TI_INDEX_H_ */

