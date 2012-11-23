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
 *****************************************************************************
 * @file
 *	omx_adec.h														 
 *
 * @brief  
 *    This file contains methods that provides the functionality for the 
 *    Audio Decoder component
 *	
 * @rev 1.0
 *
 *****************************************************************************
 */

#ifndef _OMX_AUDDEC_H
#define _OMX_AUDDEC_H

#ifdef _cplusplus
extern "C"   {
#endif /* _cplusplus */

/****************************************************************
 * INCLUDE FILES
 ****************************************************************/

#include "ti/omx/interfaces/openMaxv11/OMX_Core.h"
#include "OMX_Types.h"

/** Input port Index for the OMX_AUDDEC Comp */
#define OMX_AUDDEC_INPUT_PORT 0

/** Output port Index for the OMX_AUDDEC Comp */
#define OMX_AUDDEC_OUTPUT_PORT 1

/** OMX Audio Decoder component's name */
#define OMX_AUDDEC_COMPONENT_NAME              "OMX.TI.DSP.AUDDEC"

/* ========================================================================== */
/**
* OMX_TI_AUDDEC_ComponentInit() : Entry point for OMX Audio Decoder Component.
* This function is called whenever the IL Client invokes a GetHandle function 
* with the name "OMX.TI.DSP.AUDDEC"
*
* @param hComponent     OMX_HANDLETYPE hComponent 
*
*  @return      
*  OMX_ErrorNone = Successful 
*
*  Other_value = Failed (Error code is returned)
*
*/
/* ========================================================================== */

OMX_ERRORTYPE OMX_TI_AUDDEC_ComponentInit(OMX_HANDLETYPE hComponent);


#ifdef _cplusplus
}
#endif /* __cplusplus */

#endif /* _OMX_AUDDEC_H */


