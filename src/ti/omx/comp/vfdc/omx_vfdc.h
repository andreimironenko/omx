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
*   @file  omx_vfdc.h
 *  @brief  This file contains interfaces for the OMX methods of Video Frame 
 *          Display Component (VFDC).
 *          A user would typically use functions and data structures defined in 
 *          this module to access different VFDC functionalites
 *
 *   @rev  1.0
 *******************************************************************************
 */


#ifndef _OMX_VFDC_H
#  define _OMX_VFDC_H

#  ifdef _cplusplus
extern "C"
{
#  endif                                                   /* _cplusplus */

/*******************************************************************************
*                             Compilation Control Switches
*******************************************************************************/
/* None */

/*******************************************************************************
*                             INCLUDE FILES
*******************************************************************************/
/*--------------------- system and platform files ----------------------------*/
/* None */

/*-------------------------program files -------------------------------------*/
#include <OMX_Types.h>

/*******************************************************************************
 * PUBLIC DECLARATIONS Defined here, used elsewhere
 ******************************************************************************/

/*--------------------------- macros  ----------------------------------------*/

/** 
 *  OMX_VFDC_COMP_NAME - Name of the component. This define is used when IL 
 *  client makes getHandle call to the omx core.
 *
 */

#  define OMX_VFDC_COMP_NAME "OMX.TI.VPSSM3.VFDC"

/** Deafult port start number of VFDC comp */
#  define  OMX_VFDC_DEFAULT_START_PORT_NUM (0)

/** Input port start Index for the VFDC OMX Comp */
#  define OMX_VFDC_INPUT_PORT_START_INDEX (OMX_VFDC_DEFAULT_START_PORT_NUM)

/**
 *  \brief This macro determines the maximum number of planes/address used to
 *  represent a video buffer per field.
 *
 *  Currently this is set to 3 to support the maximum pointers required for
 *  YUV planar format - Y, Cb and Cr.
 */
#define VFDC_MAX_PLANES                 (3u)

/** \brief Index for YUV444/YUV422 interleaved formats. */
#define VFDC_YUV_INT_ADDR_IDX           (0u)

/** \brief Y Index for YUV semi planar formats. */
#define VFDC_YUV_SP_Y_ADDR_IDX          (0u)

/** \brief CB Index for semi planar formats. */
#define VFDC_YUV_SP_CBCR_ADDR_IDX       (1u)

 /** Maximum Number of input ports for the VFDC Comp */
#  define  OMX_VFDC_NUM_INPUT_PORTS      (20)

 /** Maximum Number of output ports for the VFDC Comp */
#  define  OMX_VFDC_NUM_OUTPUT_PORTS     (0)

/** \brief Maximum number of mosaic windows supported in a single VFDC instance */
#define OMX_VFDC_MAX_NUM_DISPLAY_WINDOWS    (20u)

/*--------------------------Data declarations --------------------------------*/

/*******************************************************************************
*                           Enumerated Types
*******************************************************************************/
/**
 *  @brief OMX_VIDEO_DISPLAY_ID : Defines the supported display IDs by VFDC
 *         The below names will be renamed in future releases as some of the 
 *         driver names & interfaces will be changed in future
 *         
 *  @ param OMX_VIDEO_DISPLAY_ID_HD0: 422P Private path 0 display (HD0)
 *  @ param OMX_VIDEO_DISPLAY_ID_HD1: 422P Private path 1 display (HD1)
 *  @ param OMX_VIDEO_DISPLAY_ID_SD0: 420T/422T path SD display (NTSC)
 *
 */
typedef enum OMX_VIDEO_DISPLAY_ID
{
    OMX_VIDEO_DISPLAY_ID_HD0,
    OMX_VIDEO_DISPLAY_ID_HD1,
    OMX_VIDEO_DISPLAY_ID_SD0,
    OMX_VIDEO_DISPLAY_ID_Max = 0x7FFFFFFF
} OMX_VIDEO_DISPLAY_ID;

/**
 * \brief enum Vps_DcModeId
 *  Enum defining ID of the standard Modes. Standard timinig parameters
 *  will be used if the standard mode id is used for configuring mode
 *  in the venc.
 */
typedef enum
{
    OMX_DC_MODE_NTSC = 0,
    /**< Mode Id for NTSC, currently NOT supported */
    OMX_DC_MODE_PAL,
    /**< Mode Id for PAL, currently NOT supported */
    OMX_DC_MODE_1080P_60,
    /**< Mode Id for 1080p at 60fps mode */
    OMX_DC_MODE_720P_60,
    /**< Mode Id for 720p at 60fps mode */
    OMX_DC_MODE_1080I_60,
    /**< Mode Id for 1080I at 60fps mode */
    OMX_DC_MODE_1080P_30,
    /**< Mode Id for 1080P at 30fps mode */
    OMX_DC_MODE_CUSTOM,
    /**< Mode Id for Custom mode */  
    OMX_DC_MODE_Max = 0x7FFFFFFF
    /**< This should be the last mode id */
} Omx_DcModeId;

/**
 * \brief enum Omx_DcScanformat
 *  Enum defining ID of the scan formats.
 */
typedef enum
{
    OMX_DC_SF_INTERLACED = 0,
    /**< Interlaced mode. */
    OMX_SF_PROGRESSIVE,
    /**< Progressive mode. */
    OMX_SF_Max = 0x7FFFFFFF
    /**< This should be the last mode id */
} Omx_DcScanFormat;
/*******************************************************************************
* Strutures
*******************************************************************************/

/** OMX_PARAM_VFDC_DRIVERINSTID : Configure the supported displays by VFDC
 *
 *  @ param nSize      : Size of the structure in bytes
 *  @ param nVersion   : OMX specification version information 
 *  @ param nPortIndex : Index of the port
 *  @ param nDrvInstID : Id Determine which Display driver to be selected
 *  @ param eVencDispMode : Venc display mode (1080p60, 720p60, 1080i60 etc)
 */
typedef struct OMX_PARAM_VFDC_DRIVERINSTID
{
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nPortIndex;
    OMX_VIDEO_DISPLAY_ID nDrvInstID;
    Omx_DcModeId eDispVencMode;
} OMX_PARAM_VFDC_DRIVERINSTID;

/** OMX_CONFIG_VFDC_MOSAICLAYOUT_PORT2WINMAP : Mapping OMX port to window
 *
 *  @ param nSize      : Size of the structure in bytes
 *  @ param nVersion   : OMX specification version information 
 *  @ param nPortIndex : Index of the port
 *  @ param nlayoutId  : ID of the mosaic layout
 *  @ param numWindows : Number of display windows in the mosaic layout
 *  @ param omxPortList: OMX port list in which the OMX ports are listed 
 *                       in the same order as the dipslay windows order
 */
typedef struct OMX_CONFIG_VFDC_MOSAICLAYOUT_PORT2WINMAP
{
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nPortIndex;
    OMX_U32 nLayoutId;
    OMX_U32 numWindows;
    OMX_U32 omxPortList[OMX_VFDC_MAX_NUM_DISPLAY_WINDOWS];
} OMX_CONFIG_VFDC_MOSAICLAYOUT_PORT2WINMAP;

/**
 *  \brief Data format.
 */
typedef enum
{
    VFDC_DF_YUV422I_UYVY = 0x0000,
    /**< YUV 422 Interleaved format - UYVY. */
    VFDC_DF_YUV422I_YUYV,
    /**< YUV 422 Interleaved format - YUYV. */
    VFDC_DF_YUV422I_YVYU,
    /**< YUV 422 Interleaved format - YVYU. */
    VFDC_DF_YUV422I_VYUY,
    /**< YUV 422 Interleaved format - VYUY. */
    VFDC_DF_YUV422SP_UV,
    /**< YUV 422 Semi-Planar - Y separate, UV interleaved. */
    VFDC_DF_YUV422SP_VU,
    /**< YUV 422 Semi-Planar - Y separate, VU interleaved. */
    VFDC_DF_YUV422P,
    /**< YUV 422 Planar - Y, U and V separate. */
    VFDC_DF_YUV420SP_UV,
    /**< YUV 420 Semi-Planar - Y separate, UV interleaved. */
    VFDC_DF_YUV420SP_VU,
    /**< YUV 420 Semi-Planar - Y separate, VU interleaved. */
    VFDC_DF_YUV420P,
    /**< YUV 420 Planar - Y, U and V separate. */
    VFDC_DF_YUV444P,
    /**< YUV 444 Planar - Y, U and V separate. */
    VFDC_DF_YUV444I,
    /**< YUV 444 interleaved - YUVYUV... */
    VFDC_DF_RGB16_565 = 0x1000,
    /**< RGB565 16-bit - 5-bits R, 6-bits G, 5-bits B. */
    VFDC_DF_ARGB16_1555,
    /**< ARGB1555 16-bit - 5-bits R, 5-bits G, 5-bits B, 1-bit Alpha (MSB). */
    VFDC_DF_RGBA16_5551,
    /**< RGBA5551 16-bit - 5-bits R, 5-bits G, 5-bits B, 1-bit Alpha (LSB). */
    VFDC_DF_ARGB16_4444,
    /**< ARGB4444 16-bit - 4-bits R, 4-bits G, 4-bits B, 4-bit Alpha (MSB). */
    VFDC_DF_RGBA16_4444,
    /**< RGBA4444 16-bit - 4-bits R, 4-bits G, 4-bits B, 4-bit Alpha (LSB). */
    VFDC_DF_ARGB24_6666,
    /**< ARGB4444 24-bit - 6-bits R, 6-bits G, 6-bits B, 6-bit Alpha (MSB). */
    VFDC_DF_RGBA24_6666,
    /**< RGBA4444 24-bit - 6-bits R, 6-bits G, 6-bits B, 6-bit Alpha (LSB). */
    VFDC_DF_RGB24_888,
    /**< RGB24 24-bit - 8-bits R, 8-bits G, 8-bits B. */
    VFDC_DF_ARGB32_8888,
    /**< ARGB32 32-bit - 8-bits R, 8-bits G, 8-bits B, 8-bit Alpha (MSB). */
    VFDC_DF_RGBA32_8888,
    /**< RGBA32 32-bit - 8-bits R, 8-bits G, 8-bits B, 8-bit Alpha (LSB). */
    VFDC_DF_BITMAP8 = 0x2000,
    /**< BITMAP 8bpp. */
    VFDC_DF_BITMAP4_LOWER,
    /**< BITMAP 4bpp lower address in CLUT. */
    VFDC_DF_BITMAP4_UPPER,
    /**< BITMAP 4bpp upper address in CLUT. */
    VFDC_DF_BITMAP2_OFFSET0,
    /**< BITMAP 2bpp offset 0 in CLUT. */
    VFDC_DF_BITMAP2_OFFSET1,
    /**< BITMAP 2bpp offset 1 in CLUT. */
    VFDC_DF_BITMAP2_OFFSET2,
    /**< BITMAP 2bpp offset 2 in CLUT. */
    VFDC_DF_BITMAP2_OFFSET3,
    /**< BITMAP 2bpp offset 3 in CLUT. */
    VFDC_DF_BITMAP1_OFFSET0,
    /**< BITMAP 1bpp offset 0 in CLUT. */
    VFDC_DF_BITMAP1_OFFSET1,
    /**< BITMAP 1bpp offset 1 in CLUT. */
    VFDC_DF_BITMAP1_OFFSET2,
    /**< BITMAP 1bpp offset 2 in CLUT. */
    VFDC_DF_BITMAP1_OFFSET3,
    /**< BITMAP 1bpp offset 3 in CLUT. */
    VFDC_DF_BITMAP1_OFFSET4,
    /**< BITMAP 1bpp offset 4 in CLUT. */
    VFDC_DF_BITMAP1_OFFSET5,
    /**< BITMAP 1bpp offset 5 in CLUT. */
    VFDC_DF_BITMAP1_OFFSET6,
    /**< BITMAP 1bpp offset 6 in CLUT. */
    VFDC_DF_BITMAP1_OFFSET7,
    /**< BITMAP 1bpp offset 7 in CLUT. */
    VFDC_DF_BAYER_RAW = 0x3000,
    /**< Bayer pattern. */
    VFDC_DF_RAW_VBI,
    /**< Raw VBI data. */
    VFDC_DF_RAW,
    /**< Raw data - Format not interpreted. */
    VFDC_DF_MISC,
    /**< For future purpose. */
    VFDC_DF_INVALID,
    /**< Last enum - Invalid data format. */
    VFDC_DF_32_BIT = 0x7FFFFFFF
    /**< Ensure that enum takes 32 bit in M3 */
} VFDC_DataFormat;

/**
 *  \brief Bits per pixel.
 */
typedef enum
{
    VFDC_BPP_BITS1 = 0,
    /**< 1 Bits per Pixel. */
    VFDC_BPP_BITS2,
    /**< 2 Bits per Pixel. */
    VFDC_BPP_BITS4,
    /**< 4 Bits per Pixel. */
    VFDC_BPP_BITS8,
    /**< 8 Bits per Pixel. */
    VFDC_BPP_BITS12,
    /**< 12 Bits per Pixel - used for YUV420 format. */
    VFDC_BPP_BITS16,
    /**< 16 Bits per Pixel. */
    VFDC_BPP_BITS24,
    /**< 24 Bits per Pixel. */
    VFDC_BPP_BITS32,
    /**< 32 Bits per Pixel. */
    VFDC_BPP_32_BIT = 0x7FFFFFFF
    /**< Ensure that enum in M3 takes 32 bit */
} VFDC_BitsPerPixel;

/**
 *  struct OMX_VFDC_MultipleWindowFmt
 *  \brief Structure for setting the mosaic or region based graphic
 *  window for each of the window.
 *  Note: This structure is bound to change. Kindly treat this as work
 *  in progress.
 *
 *  @ param winStartX : Horizontal start
 *  @ param winStartY : Vertical start
 *  @ param winWidth  : Width in pixels
 *  @ param winHeight : Number of lines in a window. For interlaced mode, this
 *                      should be set to the frame size and not the field size
 *  @ param pitch     : Pitch in bytes for each of the sub-window buffers. This
 *                      represents the difference between two consecutive line
 *                      address. This is irrespective of whether the video is
 *                      interlaced or progressive and whether the fields are
 *                      merged or separated for interlaced video
 *  @ param dataFormat: Data format for each window
 *  @ param bpp       : Bits per pixels for each window
 *  @ param priority  : In case of overlapping windows (as in PIP), priority
 *                      could be used to choose the window to be displayed in
 *                      the overlapped region. 0 is highest priority, 1 is next
 *                      and so on...Note that keeping same priority for all
 *                      windows specifies that there are no overlapping windows
 */
typedef struct OMX_VFDC_MultipleWindowFmt
{
    OMX_U32                  winStartX;
    OMX_U32                  winStartY;
    OMX_U32                  winWidth;
    OMX_U32                  winHeight;
    OMX_U32                  pitch[VFDC_MAX_PLANES];
    OMX_U32                  dataFormat;
    OMX_U32                  bpp;
    OMX_U32                  priority;
} OMX_VFDC_MultipleWindowFmt;

/** OMX_PARAM_VFDC_CREATEMOSAICLAYOUT: Structure containing a specific  
 *                                      Mosaic layout info
 * 
 *  @ param nSize         : Size of the structure in bytes
 *  @ param nVersion      : OMX specification version information 
 *  @ param nPortIndex    : Index of the port
 *  @ param nDisChannelNum: Channel number to which this this config belongs.
 *                          This is used in case of multiple buffers queuing/
 *                          deqeuing using a single call. If only one display
 *                          channel is supported, then this should be set to 0
 *  @ param nlayoutId     : ID of the mosaic layout
 *  @ param  nNumWindows   : Specifies the number windows that would require to
 *                          be displayed,MosaicWinFmt should point to a array
 *                          that has atleast numWindows of entries
 *  @ param  sMosaicWinFmt : FVID2 display driver Mosaic window format
 */
typedef struct OMX_PARAM_VFDC_CREATEMOSAICLAYOUT
{
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nPortIndex;
    OMX_U32               nDisChannelNum;
    OMX_U32               nLayoutId;
    OMX_U32               nNumWindows;
    OMX_VFDC_MultipleWindowFmt sMosaicWinFmt[OMX_VFDC_MAX_NUM_DISPLAY_WINDOWS];
}  OMX_PARAM_VFDC_CREATEMOSAICLAYOUT;

/** OMX_CONFIG_VFDC_MODIFYMOSAICLAYOUT: Structure containing a specific  
 *                                      Mosaic layout info
 * 
 *  @ param nSize         : Size of the structure in bytes
 *  @ param nVersion      : OMX specification version information 
 *  @ param nPortIndex    : Index of the port
 *  @ param nDisChannelNum: Channel number to which this this config belongs.
 *                          This is used in case of multiple buffers queuing/
 *                          deqeuing using a single call. If only one display
 *                          channel is supported, then this should be set to 0
 *  @ param nlayoutId     : ID of the mosaic layout
 *  @ param  nNumWindows   : Specifies the number windows that would require to
 *                          be displayed,MosaicWinFmt should point to a array
 *                          that has atleast numWindows of entries
 *  @ param  sMosaicWinFmt : FVID2 display driver Mosaic window format
 */
typedef struct OMX_CONFIG_VFDC_MODIFYMOSAICLAYOUT
{
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nPortIndex;
    OMX_U32               nDisChannelNum;
    OMX_U32               nLayoutId;
    OMX_U32               nNumWindows;
    OMX_VFDC_MultipleWindowFmt sMosaicWinFmt[OMX_VFDC_MAX_NUM_DISPLAY_WINDOWS];
}  OMX_CONFIG_VFDC_MODIFYMOSAICLAYOUT;

/** OMX_PARAM_CTRL_VENC_CUSTOM_MODE_INFO : Configure the Display Controller 
    if custom mode is chosen with this structure
 *
 *  @ param nSize          : Size of the structure in bytes
 *  @ param nVersion       : OMX specification version information 
 *  @ param nPortIndex     : Index of the port, not applicable here
 *  @ param width          : Active video frame width in pixels.
 *  @ param height         : Active video frame height in lines
 *  @ param scanFormat     : Scan format of standard. For valid values see 
 *                           #Omx_DcScanFormat
 *  @ param pixelClock     : Pixel clock of standard in KHz
 *  @ param fps            : Frames per second. Not Used
 *  @ param hFrontPorch    : Horizontal front porch. Same for both fields 
 *                           in case of interlaced display
 *  @ param hBackPorch     : Horizontal back porch  
 *  @ param hSyncLen       : Horizontal sync length. Same for both fields in 
 *                           case of interlaced display
 *  @ param vFrontPorch    : Vertical front porch for each field or frame 
 *  @ param vBackPorch     : Vertical back porch for each field or frame
 *  @ param vSyncLen       : Vertical sync length for each field
 */
typedef struct OMX_PARAM_DC_CUSTOM_MODE_INFO
{
  OMX_U32 nSize;
  OMX_VERSIONTYPE nVersion;
  OMX_U32 nPortIndex;
  OMX_U32 width;
  OMX_U32 height;
  Omx_DcScanFormat scanFormat;
  OMX_U32 pixelClock;
  OMX_U32 fps;
  OMX_U32 hFrontPorch;
  OMX_U32 hBackPorch;
  OMX_U32 hSyncLen;
  OMX_U32 vFrontPorch;
  OMX_U32 vBackPorch;
  OMX_U32 vSyncLen;
}OMX_PARAM_DC_CUSTOM_MODE_INFO;


/** OMX_PARAM_VFDC_UPSAMPLE_INFO : Maintain upsampleing info for input port of VFDC
 *
 *  @ param nSize      : Size of the structure in bytes
 *  @ param nVersion   : OMX specification version information 
 *  @ param nPortIndex : Index of the port
 *  @ param bUpSampleInput  : Boolean which says whether to upsample at the Port
                              Upsampling by a factor of 2 happens when bUpSampleInput
                              is set to OMX_TRUE. 
 *
 */
typedef struct OMX_PARAM_VFDC_UPSAMPLE_INFO
{
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nPortIndex;
    OMX_BOOL bUpSampleInput;
} OMX_PARAM_VFDC_UPSAMPLE_INFO;

/** OMX_PARAM_VFDC_FIELD_MERGE_INFO : Maintain upsampleing info for input port of VFDC
 *
 *  @ param nSize      : Size of the structure in bytes
 *  @ param nVersion   : OMX specification version information 
 *  @ param fieldMergeMode  : Boolean which says whether at input to display 
 *                            both fileds are interleaved or not
 *
 */
typedef struct OMX_PARAM_VFDC_FIELD_MERGE_INFO
{
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_BOOL fieldMergeMode;
} OMX_PARAM_VFDC_FIELD_MERGE_INFO;

/*-----------------------function prototypes ---------------------------------*/

/** OMX VFDC Component Init */
  OMX_ERRORTYPE OMX_TI_VFDC_ComponentInit ( OMX_HANDLETYPE hComponent );


#  ifdef _cplusplus
}
#  endif                                                   /* __cplusplus */

#endif /* _OMX_VFDC_H */

/* omx_VFDC.h - EOF */
