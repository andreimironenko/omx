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
 *  @file   omx_adec_utils.h
 *  @brief  This header file contains structure definitions, function 
 *          Headers for utilities of Audio Decoder Component.
 *
 *  @rev 1.0
 *******************************************************************************
 */

#ifndef _OMX_AUDDEC_UTIL_H
#define _OMX_AUDDEC_UTIL_H

#ifdef _cplusplus
extern "C"   {
#endif /* _cplusplus */

#include "OMX_Types.h"

/* Buffer is in FQ Pool or sent to next tunneled component */
#define OMX_BUFFER_FREE 0

/* buffer has been dequed by component */
#define OMX_BUFFER_DQ   1

typedef struct _OMX_AUDDEC_buf_header {
    OMX_PTR                   pBufHeader;
    OMX_U32                   bufStatus;
} OMX_AUDDEC_buf_header;

OMX_U32 OMX_TI_AUDDEC_UTL_GetBufStatus(OMX_AUDDEC_buf_header *bufHeader, OMX_U32 cnt, OMX_PTR pBuffheader );

void OMX_TI_AUDDEC_UTL_SetBufStatus(OMX_AUDDEC_buf_header *bufHeader, OMX_U32 cnt, OMX_PTR pBuffheader, OMX_U32 status );

#ifdef _cplusplus
}
#endif /* __cplusplus */


#endif //_OMX_AUDDEC_UTIL_H
