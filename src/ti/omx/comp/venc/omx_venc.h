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


/*!
  @file 	OMX_H264VideoEncoder.h
  @brief	This file contains methods that Support the functionality for the
            Openmax Video Encoder component.
  @author	
  @version	0.1 - April 03,2009
*/

/* -------------------------------------------------------------------------- */
/* =========================================================================
 *! Revision History
 *! ========================================================================
 *!20-Nov-2008 	Navneet: Initial Version
 *!25-Aug-2009 Radha Purnima  radhapurnima@ti.com 
 * ========================================================================= */

#ifndef _OMX_VIDENC_COMPONENT_H
#define _OMX_VIDENC_COMPONENT_H

#ifdef _cplusplus
extern "C"   {
#endif /* _cplusplus */

/* User code goes here */
/* ------compilation control switches ----------------------------------------*/
/****************************************************************
 * INCLUDE FILES
 ****************************************************************/
/* ----- system and platform files ----------------------------*/

/** OMX Video Encoder component's name */
#define OMX_VIDENC_COMPONENT_NAME              "OMX.TI.DUCATI.VIDENC"

/** Input port Index for the VIDENC OMX Comp */
#define OMX_VIDENC_INPUT_PORT  (0)

/** Output port Index for the VIDENC OMX Comp */
#define OMX_VIDENC_OUTPUT_PORT (1)

OMX_ERRORTYPE OMX_TI_VIDENC_ComponentInit (OMX_HANDLETYPE hComponent);

#ifdef _cplusplus
}
#endif /* __cplusplus */

#endif /* _OMX_VIDENC_COMPONENT_H */

/* Nothing beyond this point */
