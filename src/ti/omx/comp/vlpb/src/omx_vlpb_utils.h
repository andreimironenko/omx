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

/* ! @file omx_vlpb_utils.h @brief This file contains methods that Support the
 * functionality for the Openmax VLPB component @author */

#ifndef _OMX_VLPB_UTILS_H_
#  define _OMX_VLPB_UTILS_H_

/******************************************************************************
* Includes
******************************************************************************/
/* ----- system and platform files ---------------------------- */
#  include <string.h>

/*-------program files ----------------------------------------*/
#  include <ti/omx/interfaces/openMaxv11/OMX_Core.h>
#  include <ti/omx/interfaces/openMaxv11/OMX_Component.h>

#  include <ti/omx/comp/omxbase/omx_base.h>
#  include <ti/omx/comp/omxbase/omx_base_utils.h>

#  include <ti/timmosal/timm_osal_interfaces.h>

#  include <xdc/std.h>
#  include "OMX_TI_Index.h"
#  include "OMX_TI_Video.h"


/**
 *  @defgroup   HDVPSSVLPB OMXVLPB_TI
 *
 *              The OMXVLPB_TI interface enables Video Encoding at OMX level
 *
 */

 /** @ingroup    HDVPSSVLPB */
 /* @{ */

/** some inline functions*/
/* Set the Codec Chroma Format */
/* TODO: To be implemented */
#  define GET_OMX_COLORFORMAT(_pCompPvtStruct_, _e_) ()

/* TODO: To be implemented */
#  define GET_OMX_DATACONTENTTYPE(_pCompPvtStruct_, _pParamStruct_, _e_) ()

/* Function to set the DerToBaseParameters */
OMX_ERRORTYPE _OMX_VLPB_SetDerToBaseParameters (OMX_HANDLETYPE hComponent);

/* Function to Initialize the ComponentPrivate fields */
OMX_ERRORTYPE _OMX_VLPB_InitialzeComponentPrivateParams (OMX_HANDLETYPE
                                                         hComponent);

/* Function to config the extra buf header fields */
OMX_ERRORTYPE _OMX_VLPB_InitialzeComponentExtraDataInfo (OMX_HANDLETYPE
                                                         hComponent);

OMX_ERRORTYPE _OMX_VLPBFlushBuffer (OMX_HANDLETYPE hComponent);

OMX_ERRORTYPE _OMX_VLPBDisableChannel (OMX_HANDLETYPE hComponent,
                                       OMX_U32 nParam);

Void _OMX_VLPBDataProcPrdTaskFcn (TIMM_OSAL_U32 argc, TIMM_OSAL_PTR argv);

OMX_ERRORTYPE _OMX_VLPBEnablePeriodicDataProcess (OMX_HANDLETYPE hComponent);

OMX_ERRORTYPE OMX_VLPBFlushChannelBuf (OMX_HANDLETYPE hComponent);

/* @} *//* ingroup
   * *
   * HDVPSSVLPB
 */

#endif /* _OMX_VLPB_UTILS_H_ */

/* Nothing beyond this point */
