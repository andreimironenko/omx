
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
*   @file  omx_ctrl.h
 *   This file contains API/Defines that provides the functionality for the 
 *   video frame capture component.
 *   - A user would typically use functions and data structures defined in this
 *   module to access different CTRL functionalites.
 *
 *   @path \ti\omx\comp\ctrl\inc
*
*   @rev  1.0
 *   @Release version 04.00.00.02
 *   @{
 */

/** ===========================================================================
 *! Revision History
 *! ===========================================================================
 *!17-October-2009 : Initial Version
 * ============================================================================
 */

#ifndef _OMX_CTRL_H
#  define _OMX_CTRL_H

#  ifdef _cplusplus
extern "C"
{
#  endif                                                   /* _cplusplus */

  /* -------------------compilation control switches ----------------------- */

  /* ----------------------------------------------------------------------- */

/*****************************************************************************
 * -------------------INCLUDE FILES-------------------------------------------
 *****************************************************************************/
  /* ------------------ system and platform files -------------------------- */

/*--------------------program files -----------------------------------------*/
#include "OMX_Types.h"

/*****************************************************************************
 * PUBLIC DECLARATIONS Defined here, used elsewhere
 *****************************************************************************/

 /** @ingroup    HDVPSSCTRL */
  /* @{ */

  /* ========================================================================== 
   */
  /* Macros & Typedefs */
  /* ========================================================================== 
   */

/** component name for ctrl */
/** component name for TVP video decoder */
#  define OMX_CTRL_TVP_COMP_NAME        "OMX.TI.VPSSM3.CTRL.TVP"
/** component name for display controller */
#  define OMX_CTRL_DC_COMP_NAME         "OMX.TI.VPSSM3.CTRL.DC"

/*******************************************************************************
*                           Enumerated Types
*******************************************************************************/
/**
 *  @brief OMX_CTRL_VID_DECODER_ID : Defines the supported video decoders
 *         The below names will be renamed in future releases as some of the 
 *         driver names & interfaces will be changed in future
 *         
 *  @ param OMX_VID_DEC_SII9135_DRV: SIL video decoder for 1080P60 capture
 *  @ param OMX_VID_DEC_TVP7002_DRV: TVP7002 video decoder for 1080i60 capture
 *  @ param OMX_VID_DEC_TVP5158_DRV: TVP5158 video decoder for muxed capture
 *
 */
typedef enum OMX_CTRL_VID_DECODER_ID {
  OMX_VID_DEC_Unused = 0x00000000,
  OMX_VID_DEC_SII9135_DRV,
  OMX_VID_DEC_TVP7002_DRV,
  OMX_VID_DEC_TVP5158_DRV,
  OMX_VID_DEC_TIExtensions = 0x6F000000,
  OMX_VID_DEC_VendorStartUnused  = 0x7F000000,
  OMX_VID_DEC_Max = 0x7FFFFFFF
} OMX_CTRL_VID_DECODER_ID;

/******************************************************************************/
/**
 *  @brief OMX_CTRL_VIDEO_DECODER_STD : Defines the supported video decoder
 *                                      standards
 *         The below names will be renamed in future releases as some of the 
 *         driver names & interfaces will be changed in future
 *         
 *  @ param OMX_VIDEO_DECODER_STD_AUTO_DETECT: Auto detect the standard
 *  @ param OMX_VIDEO_DECODER_STD_MUX_4CH_D1 : 4 ch D1 capture
 *  @ param OMX_VIDEO_DECODER_STD_1080P_60 : 1 ch 1080P60 capture, to be used
 *                                           when capturing via TVP 7002
 *  @ param OMX_VIDEO_DECODER_STD_1080I_60 : 1 ch 1080I60 capture, to be used 
 *                                           when capturing via TVP_7002
 *
 */
typedef enum OMX_CTRL_VIDEO_DECODER_STD {
  OMX_VIDEO_DECODER_STD_Unused = 0x00000000,
  OMX_VIDEO_DECODER_STD_AUTO_DETECT,
  OMX_VIDEO_DECODER_STD_1080P_60,
  OMX_VIDEO_DECODER_STD_1080I_60,
  OMX_VIDEO_DECODER_STD_1080I_50,
  OMX_VIDEO_DECODER_STD_MUX_4CH_D1,
  OMX_VIDEO_DECODER_STD_720P_60,
  OMX_VIDEO_DECODER_STD_NTSC,
  OMX_VIDEO_DECODER_STD_PAL,
  OMX_VIDEO_DECODER_STD_1CH_D1,
  OMX_VIDEO_DECODER_STD_TIExtensions = 0x6F000000,
  OMX_VIDEO_DECODER_STD_VendorStartUnused  = 0x7F000000,
  OMX_VIDEO_DECODER_STD_Max = 0x7FFFFFFF
} OMX_CTRL_VIDEO_DECODER_STD;

/******************************************************************************/
/**
 *  @brief OMX_CTRL_VIDEO_DECODER_SYSTEM_STD : Defines the supported video 
 *         decoder system standards
 *         The below names will be renamed in future releases as some of the 
 *         driver names & interfaces will be changed in future
 *         
 *  @ param OMX_VIDEO_DECODER_VIDEO_SYSTEM_NTSC: 
                      Interlaced, NTSC system, 720x240 per field
 *  @ param OMX_VIDEO_DECODER_VIDEO_SYSTEM_PAL: 
                      Interlaced, PAL system, 720x288 per field
 *  @ param OMX_VIDEO_DECODER_VIDEO_SYSTEM_AUTO_DETECT: 
                      Auto-detect NTSC or PAL system
 */
typedef enum OMX_CTRL_VIDEO_DECODER_SYSTEM_STD {
  OMX_VIDEO_DECODER_VIDEO_SYSTEM_NTSC = 0x00000000,
  OMX_VIDEO_DECODER_VIDEO_SYSTEM_PAL,
  OMX_VIDEO_DECODER_VIDEO_SYSTEM_AUTO_DETECT,
  OMX_VIDEO_DECODER_VIDEO_SYSTEM_Max = 0x7FFFFFFF
} OMX_CTRL_VIDEO_DECODER_SYSTEM_STD;

/*******************************************************************************
* Strutures
*******************************************************************************/

/** OMX_PARAM_CTRL_VIDDECODER_INFO : Configure the video decoder params
 *
 *  @ param nSize      : Size of the structure in bytes
 *  @ param nVersion   : OMX specification version information 
 *  @ param nPortIndex : Index of the port
 *  @ param videoStandard : supported video decoder standards
 *  @ param videoDecoderId: supported video decoders
 */
typedef struct OMX_PARAM_CTRL_VIDDECODER_INFO {
  OMX_U32 nSize;
  OMX_VERSIONTYPE nVersion;
  OMX_U32 nPortIndex;
  OMX_CTRL_VIDEO_DECODER_STD videoStandard;
  OMX_CTRL_VID_DECODER_ID videoDecoderId;
  OMX_CTRL_VIDEO_DECODER_SYSTEM_STD videoSystemId;
} OMX_PARAM_CTRL_VIDDECODER_INFO;


/*----------------------function prototypes ---------------------------------*/

/** OMX CTRL Component Init */
OMX_ERRORTYPE OMX_TI_CTRL_TVP_ComponentInit ( OMX_HANDLETYPE hComponent );

OMX_ERRORTYPE OMX_TI_CTRL_DC_ComponentInit ( OMX_HANDLETYPE hComponent );


#  ifdef _cplusplus
}
#  endif                                                   /* __cplusplus */

#endif                                                     /* _OMX_CTRL_H 
                                                            */

/* Nothing beyond this point */
