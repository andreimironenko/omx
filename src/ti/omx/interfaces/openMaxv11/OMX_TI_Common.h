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
 *  @file  OMX_TI_Common.h
 *
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

#ifndef _OMX_TI_COMMON_H_
#define _OMX_TI_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/******************************************************************
 *   INCLUDE FILES
 ******************************************************************/
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
#define OMX_TI_COMMON_MAX_PORTS_IN_GROUP           (32u)

#define OMX_TI_COMMON_MIN_DUPD_PORTS              (0u)
#define OMX_TI_COMMON_MAX_DUPD_PORTS              (16u)

#define OMX_TI_COMMON_MAX_CHANNEL_NAME_SIZE        (128u)
#define OMX_TI_COMMON_MAX_GROUP_NAME_SIZE          (128u)

#define OMX_BUFFERHEADERFLAG_MODIFIED 0x00000100

#define OMX_TI_BUFFERFLAG_READONLY 0x00000200

#define OMX_TI_BUFFERFLAG_DETACHEDEXTRADATA 0x00000400

#define OMX_TI_BUFFERFLAG_VIDEO_FRAME_TYPE_INTERLACE (0x00010000)  

#define OMX_TI_BUFFERFLAG_VIDEO_FRAME_TYPE_INTERLACE_BOTTOM  (0x00020000)

#define OMX_TI_BUFFERFLAG_VIDEO_FRAME_TYPE_INTERLACE_TOP_FIRST  (0x00040000)


typedef struct OMX_CONFIG_COMSTRUCT {

    OMX_U32 nSize; /**< Size of the structure in bytes */
    OMX_VERSIONTYPE nVersion; /**< OMX specification version information */
    OMX_U32 nPortIndex; /**< Index of the port */

} OMX_CONFIG_COMSTRUCT;

typedef struct OMX_TI_PLATFORMPRIVATE
{
    OMX_U32 nSize;
    OMX_PTR pExtendedPlatformPrivate;
	OMX_BOOL bReadViaCPU;
	OMX_BOOL bWriteViaCPU;
	OMX_PTR pMetaDataBuffer;
	OMX_U32 nMetaDataSize;
    OMX_PTR pAuxBuf1;
    OMX_U32 pAuxBufSize1;
}OMX_TI_PLATFORMPRIVATE;

/*===============================================================*/
/** OMX_TI_PARAM_BUFFERPREANNOUNCE    : This parameter is used to enable/disable
 *                                      buffer pre announcement. Buffer pre
 *                                      announcement is enabled by default i.e.
 *                                      if buffer is being allocated by client 
 *                                      then the buffer ptrs will be made known 
 *                                      to the component at the time of 
 *                                      UseBuffer and these will not change in
 *                                      lifetime of the component. If pre
 *                                      announcement is disabled then new 
 *                                      buffers can be allocated by the client 
 *                                      at runtime and passed in ETB/FTB. This
 *                                      parameter is valid only in cases where 
 *                                      client allocates the buffer 
 *                                      (i.e. UseBuffer cases).
 *
 *  @ param nSize                     : Size of the structure.
 *  @ param nVersion                  : Version.
 *  @ param nPortIndex                : Port index on which the parameter will
 *                                      be applied.
 *  @ param bEnabled                  : Whether buffer pre announcement is 
 *                                      enabled or not. Set to TRUE (enabled)
 *                                      by default.
 */
/*===============================================================*/
typedef struct OMX_TI_PARAM_BUFFERPREANNOUNCE
{
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nPortIndex;
    OMX_BOOL bEnabled;
}OMX_TI_PARAM_BUFFERPREANNOUNCE;


/*===============================================================*/
/** OMX_TI_CONFIG_BUFFERREFCOUNTNOTIFYTYPE : This config is used to 
 *                                           enable/disable notification when
 *                                           reference count of a buffer changes
 *                                           This happens usually when buffers 
 *                                           are locked/unlocked by codecs. By
 *                                           DEFAULT all notifications are
 *                                           DISABLED.
 *
 *  @ param nSize                          : Size of the structure.
 *  @ param nVersion                       : Version.
 *  @ param nPortIndex                     : Port index on which the config will
 *                                           be applied.
 *  @ param bNotifyOnIncrease              : Enable notification when reference 
 *                                           count is increased.
 *  @ param bNotifyOnDecrease              : Enable notification when reference 
 *                                           count is decreased.
 *  @ param nCountForNotification          : Count at which to trigger 
 *                                           notification. Count indicates no.
 *                                           of copies of the buffer in 
 *                                           circulation e.g.
 *                                           1 - Only the original buffer is in 
 *                                               circulation i.e. no buffers are
 *                                               currently locked.
 *                                           2 - There are two copies of the 
 *                                               buffer i.e. one original and 
 *                                               one copy which has been locked
 *                                               by the codec.
 *                                           And so on
 *                                           SPECIAL CASE
 *                                           0 - Indicates notify always 
 *                                               irrespective of count value.
 */
/*===============================================================*/
typedef struct OMX_TI_CONFIG_BUFFERREFCOUNTNOTIFYTYPE
{
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nPortIndex;
    OMX_BOOL bNotifyOnIncrease;
    OMX_BOOL bNotifyOnDecrease;
    OMX_U32 nCountForNotification;
}OMX_TI_CONFIG_BUFFERREFCOUNTNOTIFYTYPE;

typedef struct OMX_CONFIG_CHANNELNAME {

    OMX_U32 nSize; /**< Size of the structure in bytes */
    OMX_VERSIONTYPE nVersion; /**< OMX specification version information */
    OMX_U32 nPortIndex; /**< Index of the port */
    OMX_S8  cChannelName[OMX_TI_COMMON_MAX_CHANNEL_NAME_SIZE]; /**< Channel name  */

} OMX_CONFIG_CHANNELNAME;

/** OMX_BUFFER_MEMORY_TYPE: procssor Id in Multi processor environment, 
 *                          This Id is used to setup the Reader location of 
 *                          Tunneling component for FQ. This is required,
 *                          as based on this shread region Id is configured
 *                          while creating Buffer manager & FQ for properiatory 
 *                          tunneling.
 * 
 *  @ param OMX_PROC_ID_DSP
 *  @ param OMX_PROC_ID_VIDEO
 *  @ param OMX_PROC_ID_DSS
 *  @ param OMX_PROC_ID_HOST
 */
typedef enum OMX_PROC_ID_TYPE {
    OMX_PROC_ID_DSP    = 0x0,
    OMX_PROC_ID_VIDEO  = 0x1,
    OMX_PROC_ID_DSS    = 0x2,
    OMX_PROC_ID_HOST   = 0x3,
    OMX_PROC_ID_MAX    = 0x7FFFFFFF
}OMX_PROC_ID_TYPE;

typedef struct OMX_CONFIG_CHANNEL_READER_PROCID {

    OMX_U32 nSize; /**< Size of the structure in bytes */
    OMX_VERSIONTYPE nVersion; /**< OMX specification version information */
    OMX_U32 nPortIndex; /**< Index of the port */
    OMX_PROC_ID_TYPE nChReaderProcId; /**< Channelreader proc Id, This Id is used to 
                               *  detect the Shared region which needs to be 
                               *  used for creating FQ Buffer manager & FQ name */
} OMX_CONFIG_CHANNEL_READER_PROCID;

typedef struct OMX_CONFIG_CHANNELGROUPINFO {

    OMX_U32 nSize; /**< Size of the structure in bytes */
    OMX_VERSIONTYPE nVersion; /**< OMX specification version information */
    OMX_U32 nPortIndex; /**< Index of the port */
    OMX_S8  cChannelGroupName[OMX_TI_COMMON_MAX_GROUP_NAME_SIZE]; /**< Channel Group name  */
    OMX_U32 nNumPortsInGroup;  /**< Nnmber of ports in the group */    
    OMX_U32 auPortGroupIndex[OMX_TI_COMMON_MAX_PORTS_IN_GROUP]; /**< Index of the ports in the group */
} OMX_CONFIG_CHANNELGROUPINFO;

typedef struct OMX_PARAM_DUPEPORTSINFO {

    OMX_U32 nSize; /**< Size of the structure in bytes */
    OMX_VERSIONTYPE nVersion; /**< OMX specification version information */
    OMX_U32 nPortIndex; /**< Index of the port */
    OMX_U32 nNumDupedPorts;  /**< Nnmber of ports in the group */    
    OMX_U32 auDupedPortsIndex[OMX_TI_COMMON_MAX_DUPD_PORTS]; /**< Index of the ports in the duped ports */
} OMX_PARAM_DUPEPORTSINFO;


/** OMX_CONFIG_VIDCHANNEL_RESOLUTION   : Componemt port resolution
 *  Assumption - Any driver channel of VFPC module will have a max of 
 *  two input ports and a max of two output ports.
 */
typedef struct OMX_CONFIG_VIDCHANNEL_RESOLUTION {
    OMX_U32 nSize; /**< Size of the structure in bytes */
    OMX_VERSIONTYPE nVersion; /**< OMX specification version information */
    OMX_U32 nPortIndex; /**< Index of the port */
    OMX_DIRTYPE eDir;     /**< OMX_DirInput - Input, OMX_DirOutput - Output */
    OMX_U32 nChId;        /**< Channel ID */
    OMX_U32 Frm0Width;    /**< Width of first Frame */
    OMX_U32 Frm0Height;   /**< Height of first Frame */
    OMX_U32 Frm0Pitch;    /**< Pitch of first Frame */
    OMX_U32 Frm1Width;    /**< Width of Second Frame */
    OMX_U32 Frm1Height;   /**< Height of Second Frame */
    OMX_U32 Frm1Pitch;    /**< Pitch of Second Frame */
    OMX_U32 FrmStartX;    /**< Horizontal start offset */
    OMX_U32 FrmStartY;    /**< Vertical start offset */
    OMX_U32 FrmCropWidth; /**< Crop Width */
    OMX_U32 FrmCropHeight;/**< Crop Height */
  } OMX_CONFIG_VIDCHANNEL_RESOLUTION;

  
/** OMX_BUFFER_MEMORY_TYPE: Buffer allocation type. New types has 
 *                          been added required for tiled support.
 *
 *  @ param OMX_BUFFER_MEMORY_DEFAULT    : Default Normal(Non-tiled) 1D Memory
 *  @ param OMX_BUFFER_MEMORY_TILED_8BIT : 8-bit Tiled memory
 *  @ param OMX_BUFFER_MEMORY_TILED_16BIT: 16-bit Tiled memory
 *  @ param OMX_BUFFER_MEMORY_TILED_32BIT: 32-bit Tiled memory
 *  @ param OMX_BUFFER_MEMORY_TILED_PAGE : Page Tiled memory
 *  @ param OMX_BUFFER_MEMORY_CUSTOM     : Custom buffer allocation which will
 *                                         be specified by derived component
 */
typedef enum OMX_BUFFER_MEMORY_TYPE {
    OMX_BUFFER_MEMORY_DEFAULT           = 0x1,
    OMX_BUFFER_MEMORY_TILED_8BIT        = 0x2,
    OMX_BUFFER_MEMORY_TILED_16BIT       = 0x3,
    OMX_BUFFER_MEMORY_TILED_32BIT       = 0x4,
    OMX_BUFFER_MEMORY_TILED_PAGE        = 0x5,
    OMX_BUFFER_MEMORY_CUSTOM            = 0xA,
    OMX_BUFFER_MEMORY_32_BIT            = 0x10000
}OMX_BUFFER_MEMORY_TYPE;

/** OMX_PARAM_BUFFER_MEMORYTYPE   : Type of Memory from where the component
 *  buffers are getting allocated. By default it will be normal non tiled
 *  memory
 */
typedef struct OMX_PARAM_BUFFER_MEMORYTYPE {
    OMX_U32 nSize; /**< Size of the structure in bytes */
    OMX_VERSIONTYPE nVersion; /**< OMX specification version information */
    OMX_U32 nPortIndex; /**< Index of the port */
    OMX_BUFFER_MEMORY_TYPE eBufMemoryType; /**< Type of the Memory to
                                                be allocated */
} OMX_PARAM_BUFFER_MEMORYTYPE;

/** OMX_NOTIFY_TYPE: Notification Type 
 *
 *  @ param OMX_NOTIFY_TYPE_NONE   : Notify Never
 *  @ param OMX_NOTIFY_TYPE_ALWAYS : Always Notify
 *  @ param OMX_NOTIFY_TYPE_ONCE   : Notify Once, This is not implemented yet
 */
typedef enum OMX_NOTIFY_TYPE {
    OMX_NOTIFY_TYPE_NONE   = 0x0,
    OMX_NOTIFY_TYPE_ALWAYS = 0x1,
    OMX_NOTIFY_TYPE_ONCE   = 0x2,
    OMX_NOTIFY_TYPE_32_BIT = 0x10000
}OMX_NOTIFY_TYPE;

/** OMX_RESOLUTIONCHANGE_TYPE: Resolution change detection Type 
 *
 *  @ param OMX_RESOLUTIONCHANGE_TYPE_AUTODETECT   : SetConfig is not required
 *  @ param OMX_RESOLUTIONCHANGE_TYPE_CONFIG : SetConfig is required
 */
typedef enum OMX_RESOLUTIONCHANGE_TYPE {
    OMX_RESOLUTIONCHANGE_TYPE_AUTODETECT   = 0x0,
    OMX_RESOLUTIONCHANGE_TYPE_CONFIG = 0x1,
    OMX_RESOLUTIONCHANGE_TYPE_32_BIT = 0x10000
}OMX_RESOLUTIONCHANGE_TYPE;

/** OMX_FRAMERATECONTROL_TYPE: Frame rate control type. This config enables a
 *  component to skip extra frames without processing and return them to the 
 *  supplier component
 *
 *  @ param OMX_FRAMERATECONTROL_TYPE_ENABLED   : Frame Rate Control is Enabled
 *  @ param OMX_FRAMERATECONTROL_TYPE_DISABLED  : Frame Rate Control is Disabled
 */
typedef enum OMX_FRAMERATECONTROL_TYPE {
    OMX_FRAMERATECONTROL_TYPE_DISABLED   = 0x0,
    OMX_FRAMERATECONTROL_TYPE_ENABLED    = 0x1,
    OMX_FRAMERATECONTROL_TYPE_32_BIT = 0x10000
}OMX_FRAMERATECONTROL_TYPE;

/** OMX_PARAM_COMPPORT_NOTIFYTYPE   : The OMX component's ports could
 *  be configured with various notification types. This will decide the 
 *  component port latency, where the data processing is based
 *  on DataNotify rather than the periodic task. This will be a per port 
 *  configuration and the Dafault configuration is Notify Never.
 */
typedef struct OMX_PARAM_COMPPORT_NOTIFYTYPE {
    OMX_U32 nSize; /**< Size of the structure in bytes */
    OMX_VERSIONTYPE nVersion; /**< OMX specification version information */
    OMX_U32 nPortIndex; /**< Index of the port */
    OMX_NOTIFY_TYPE eNotifyType; /**< Notify type */
} OMX_PARAM_COMPPORT_NOTIFYTYPE;

/** OMX_PARAM_RESOLUTIONCHANGE_TYPE   : The OMX component may detect an
 *  updated resolution from either its own config or from the incoming buffer
 *  header. This parameter configures this:
 *  AUTODETECT: implies resolution change is detected from input buffer header
 *  CONFIG: implies resolution change is detected from SetConfig call
 */
typedef struct OMX_PARAM_RESOLUTIONCHANGE_TYPE {
    OMX_U32 nSize; /**< Size of the structure in bytes */
    OMX_VERSIONTYPE nVersion; /**< OMX specification version information */
    OMX_U32 nPortIndex; /**< Index of the port */
    OMX_U32 nChId;        /**< Channel ID */
    OMX_RESOLUTIONCHANGE_TYPE eResolutionChangeType; /**< Resolution change type */
} OMX_PARAM_RESOLUTIONCHANGE_TYPE;

/** OMX_CONFIG_RESOLUTIONCHANGE_TYPE   : The OMX component may detect an
 *  updated resolution from either its own config or from the incoming buffer
 *  header. This parameter configures this:
 *  AUTODETECT: implies resolution change is detected from input buffer header
 *  CONFIG: implies resolution change is detected from SetConfig call
 */
typedef struct OMX_CONFIG_RESOLUTIONCHANGE_TYPE {
    OMX_U32 nSize; /**< Size of the structure in bytes */
    OMX_VERSIONTYPE nVersion; /**< OMX specification version information */
    OMX_U32 nPortIndex; /**< Index of the port */
    OMX_U32 nChId;        /**< Channel ID */
    OMX_RESOLUTIONCHANGE_TYPE eResolutionChangeType; /**< Resolution change type */
} OMX_CONFIG_RESOLUTIONCHANGE_TYPE;

/** OMX_CONFIG_FRAMERATECONTROL_TYPE   : This config enables a
 *  component to skip extra frames without processing and return them to the 
 *  supplier component. It also allows setting up a threshold beyond which
 *  frames would be treated as extra frames
 */
typedef struct OMX_CONFIG_FRAMERATECONTROL_TYPE {
    OMX_U32 nSize; /**< Size of the structure in bytes */
    OMX_VERSIONTYPE nVersion; /**< OMX specification version information */
    OMX_U32 nPortIndex; /**< Index of the port */
    OMX_U32 nThreshold;
    OMX_FRAMERATECONTROL_TYPE eFrameRateControlType; /**< Frame rate control type */
} OMX_CONFIG_FRAMERATECONTROL_TYPE;

/** OMX_CONFIG_ALG_ENABLE: Flag to enable/disable the Algoritm/IP
 */
typedef struct OMX_CONFIG_ALG_ENABLE {
    OMX_U32 nSize; /**< Size of the structure in bytes */
    OMX_VERSIONTYPE nVersion; /**< OMX specification version information */
    OMX_U32 nPortIndex; /**< Index of the port */
    OMX_U32 nChId;        /**< Channel ID */
    OMX_BOOL bAlgBypass; /**< Algoritm/IP enable/disable flag */
} OMX_CONFIG_ALG_ENABLE;

/** OMX_CONFIG_SUBSAMPLING_FACTOR: Video frame rate sub sampling factor
 *  configuration parameter
 */
typedef struct OMX_CONFIG_SUBSAMPLING_FACTOR {
    OMX_U32 nSize; /**< Size of the structure in bytes */
    OMX_VERSIONTYPE nVersion; /**< OMX specification version information */
    OMX_U32 nPortIndex; /**< Index of the port */
    OMX_U32 nSubSamplingFactor; /**< Video frame rate sub sampling factor */
} OMX_CONFIG_SUBSAMPLING_FACTOR;

/** OMX_PARAM_FILEMODE_TYPE   : Enable/Disable file mode operation for component
 *  COmponents such as video decoder can operate either in a single file mode
 *  where the component keeps track of bytes consumed across frames or in
 *  frame mode where the frames are chunked before feeding to decode component
 *  
 */
typedef struct OMX_PARAM_FILEMODE_TYPE {
    OMX_U32 nSize; /**< Size of the structure in bytes */
    OMX_VERSIONTYPE nVersion; /**< OMX specification version information */
    OMX_U32 nPortIndex; /**< Index of the port */
    OMX_BOOL bEnableFileMode; /**< Flag to enable/disable file mode operation */
} OMX_PARAM_FILEMODE_TYPE;

	
typedef struct OMX_PARAM_FRAMEQBUFMANAGER
{
  OMX_U32 nSize;
  OMX_VERSIONTYPE nVersion;
  OMX_U32 nPortIndex;
  OMX_PTR hFrameQMgr;
} OMX_PARAM_FRAMEQBUFMANAGER;

/*******************************************************************
 * PRIVATE DECLARATIONS: defined here, used only here
 *******************************************************************/
/*----------          data declarations        ------------------- */
/*----------          function prototypes      ------------------- */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _OMX_TI_COMMON_H_ */
