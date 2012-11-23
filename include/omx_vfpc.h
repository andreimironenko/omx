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
 *  @file   omx_vfpc.h
 *  @brief  This file contains interfaces for the OMX methods of Video 
 *          Frame Processing Component.
 *          A user would typically use functions and data structures defined in 
 *          this module to access different VFPC functionalites
 *
 *  @rev  1.0
 *******************************************************************************
 */

#ifndef _OMX_VFPC_H
#define _OMX_VFPC_H

#ifdef _cplusplus
extern "C"
{
#endif                                                   /* _cplusplus */

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
#include "OMX_Types.h"

/*******************************************************************************
 * PUBLIC DECLARATIONS Defined here, used elsewhere
 ******************************************************************************/

/** component name for vfpc WB Scalar */
#define OMX_VFPC_INDTXSCWB_COMP_NAME          "OMX.TI.VPSSM3.VFPC.INDTXSCWB"
/** component name for vfpc DEI_M Dual scale out */
#define OMX_VFPC_DEIDUALOUT_MEDIUM_COMP_NAME  "OMX.TI.VPSSM3.VFPC.DEIMDUALOUT"
/** component name for vfpc DEI_H Dual scale out */
#define OMX_VFPC_DEIDUALOUT_HIGH_COMP_NAME    "OMX.TI.VPSSM3.VFPC.DEIHDUALOUT"
/** component name for vfpc NF */
#define OMX_VFPC_NF_COMP_NAME                 "OMX.TI.VPSSM3.VFPC.NF"

 /** Maximum Number of input ports for the VFPC Comp */
#define  OMX_VFPC_NUM_INPUT_PORTS      (16)

 /** Maximum Number of output ports for the VFPC Comp */
#define  OMX_VFPC_NUM_OUTPUT_PORTS     (16)

 /** Maximum Number of ports for the VFPC Comp */
#define  OMX_VFPC_NUM_PORTS     (OMX_VFPC_NUM_INPUT_PORTS +  \
                                   OMX_VFPC_NUM_OUTPUT_PORTS)

 /** Deafult port start number of VFPC comp */
#define  OMX_VFPC_DEFAULT_START_PORT_NUM (0)

 /** Input port start Index for the VFPC OMX Comp */
#define OMX_VFPC_INPUT_PORT_START_INDEX (OMX_VFPC_DEFAULT_START_PORT_NUM)

 /** Output port start Index for the VFPC OMX Comp */
#define OMX_VFPC_OUTPUT_PORT_START_INDEX (OMX_VFPC_DEFAULT_START_PORT_NUM + \
                                            OMX_VFPC_NUM_INPUT_PORTS)

/** Max Number of SC channels per Component Handle **/
#define OMX_VFPC_INDTXSCWB_MAX_NUM_CHANNELS           (16u)

/** Max Number of DEIM-DualSC channels per Component Handle **/
#define OMX_VFPC_DEIDUALOUT_MEDIUM_MAX_NUM_CHANNELS   (8u)

/** Max Number of DEIH-DualSC channels per Component Handle **/
#define OMX_VFPC_DEIDUALOUT_HIGH_MAX_NUM_CHANNELS     (8u)

/** Max Number of NF channels per Component Handle **/
#define OMX_VFPC_NF_MAX_NUM_CHANNELS                  (16u)


/*--------------------------Data declarations --------------------------------*/

/*******************************************************************************
*                           Enumerated Types
*******************************************************************************/

  /* ========================================================================== 
   */
  /* Macros & Typedefs */
  /* ========================================================================== 
   */

/*******************************************************************************
* Strutures
*******************************************************************************/

/** OMX_PARAM_VFPC_NUMCHANNELPERHANDLE : Configure the number of channels 
 *                                         per Handle of VFPC
 *
 *  @ param nSize      : Size of the structure in bytes
 *  @ param nVersion   : OMX specification version information 
 *  @ param nPortIndex : Index of the port
 *  @ param NumChannelsPerHandle : number of channels per Handle of VFPC
 */
typedef struct OMX_PARAM_VFPC_NUMCHANNELPERHANDLE
{
    OMX_U32 nSize;
    OMX_VERSIONTYPE nVersion;
    OMX_U32 nPortIndex;
    OMX_U32 nNumChannelsPerHandle;
} OMX_PARAM_VFPC_NUMCHANNELPERHANDLE;

/******************************************************************************/

/*----------------------function prototypes ---------------------------------*/

/** OMX VFPC Component Init */
OMX_ERRORTYPE OMX_TI_VFPC_IndTxSc_ComponentInit (OMX_HANDLETYPE hComponent);

OMX_ERRORTYPE OMX_TI_VFPC_NF_ComponentInit (OMX_HANDLETYPE hComponent);

OMX_ERRORTYPE OMX_TI_VFPC_DeiDualOutMedium_ComponentInit (OMX_HANDLETYPE
                                                            hComponent);

OMX_ERRORTYPE OMX_TI_VFPC_DeiDualOutHigh_ComponentInit (OMX_HANDLETYPE
                                                          hComponent);

#ifdef _cplusplus
}
#endif /* __cplusplus */

#endif /* _OMX_VFPC_H */

/* Nothing beyond this point */
