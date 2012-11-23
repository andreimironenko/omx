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

/*
 * This file contains externs of signatures of all OMX components' init APIs
 * that are local to the core as well as proxy APIs
 */

#include <OMX_Component.h>

extern OMX_ERRORTYPE OMX_TI_VFCC_ComponentInit (OMX_HANDLETYPE hComponent);
extern OMX_ERRORTYPE OMX_TI_VFDC_ComponentInit (OMX_HANDLETYPE hComponent);
extern OMX_ERRORTYPE OMX_TI_VFPC_DeiDualOutHigh_ComponentInit (OMX_HANDLETYPE
                                                               hComponent);
extern OMX_ERRORTYPE OMX_TI_VFPC_DeiDualOutMedium_ComponentInit (OMX_HANDLETYPE
                                                                 hComponent);
extern OMX_ERRORTYPE OMX_TI_VFPC_NF_ComponentInit (OMX_HANDLETYPE hComponent);
extern OMX_ERRORTYPE OMX_TI_VFPC_IndTxSc_ComponentInit (OMX_HANDLETYPE
                                                        hComponent);
extern OMX_ERRORTYPE OMX_TI_CTRL_TVP_ComponentInit (OMX_HANDLETYPE hComponent);
extern OMX_ERRORTYPE OMX_TI_CTRL_DC_ComponentInit (OMX_HANDLETYPE hComponent);
extern OMX_ERRORTYPE OMX_TI_VIDENC_ComponentInit (OMX_HANDLETYPE hComponent);
extern OMX_ERRORTYPE OMX_TI_VIDSPLC_ComponentInit (OMX_HANDLETYPE hComponent);
extern OMX_ERRORTYPE OMX_TI_VIDDEC_ComponentInit (OMX_HANDLETYPE hComponent);
extern OMX_ERRORTYPE OMX_TI_VSRC_ComponentInit (OMX_HANDLETYPE hComponent);
extern OMX_ERRORTYPE OMX_TI_VSNK_ComponentInit (OMX_HANDLETYPE hComponent);
extern OMX_ERRORTYPE OMX_TI_CMUX_ComponentInit (OMX_HANDLETYPE hComponent);
extern OMX_ERRORTYPE OMX_TI_DMUX_ComponentInit (OMX_HANDLETYPE hComponent);
extern OMX_ERRORTYPE OMX_TI_RTPTX_ComponentInit (OMX_HANDLETYPE hComponent);
extern OMX_ERRORTYPE OMX_TI_RTPRX_ComponentInit (OMX_HANDLETYPE hComponent);
extern OMX_ERRORTYPE OMX_TI_VLPB_ComponentInit (OMX_HANDLETYPE hComponent);
extern OMX_ERRORTYPE OMX_TI_AUDDEC_ComponentInit(OMX_HANDLETYPE hComponent);  
extern OMX_ERRORTYPE OMX_TI_VSWMOSAIC_ComponentInit (OMX_HANDLETYPE hComponent);

extern OMX_ERRORTYPE DomxProxy_OMX_TI_VFCC_ComponentInit (OMX_HANDLETYPE
                                                          hComponent);
extern OMX_ERRORTYPE DomxProxy_OMX_TI_VFDC_ComponentInit (OMX_HANDLETYPE
                                                          hComponent);
extern OMX_ERRORTYPE
 DomxProxy_OMX_TI_VFPC_DeiDualOutHigh_ComponentInit (OMX_HANDLETYPE hComponent);
extern OMX_ERRORTYPE
 DomxProxy_OMX_TI_VFPC_DeiDualOutMedium_ComponentInit (OMX_HANDLETYPE
                                                       hComponent);
extern OMX_ERRORTYPE DomxProxy_OMX_TI_VFPC_NF_ComponentInit (OMX_HANDLETYPE
                                                             hComponent);
extern OMX_ERRORTYPE DomxProxy_OMX_TI_VFPC_IndTxSc_ComponentInit (OMX_HANDLETYPE
                                                                  hComponent);
extern OMX_ERRORTYPE DomxProxy_OMX_TI_CTRL_TVP_ComponentInit (OMX_HANDLETYPE
                                                              hComponent);
extern OMX_ERRORTYPE DomxProxy_OMX_TI_CTRL_DC_ComponentInit (OMX_HANDLETYPE
                                                             hComponent);
extern OMX_ERRORTYPE DomxProxy_OMX_TI_VIDENC_ComponentInit (OMX_HANDLETYPE
                                                            hComponent);
extern OMX_ERRORTYPE DomxProxy_OMX_TI_VIDDEC_ComponentInit (OMX_HANDLETYPE
                                                            hComponent);
extern OMX_ERRORTYPE DomxProxy_OMX_TI_VSRC_ComponentInit (OMX_HANDLETYPE
                                                          hComponent);
extern OMX_ERRORTYPE DomxProxy_OMX_TI_VSNK_ComponentInit (OMX_HANDLETYPE
                                                          hComponent);
extern OMX_ERRORTYPE DomxProxy_OMX_TI_CMUX_ComponentInit (OMX_HANDLETYPE
                                                          hComponent);
extern OMX_ERRORTYPE DomxProxy_OMX_TI_DMUX_ComponentInit (OMX_HANDLETYPE
                                                          hComponent);
extern OMX_ERRORTYPE DomxProxy_OMX_TI_RTPTX_ComponentInit (OMX_HANDLETYPE
                                                           hComponent);
extern OMX_ERRORTYPE DomxProxy_OMX_TI_RTPRX_ComponentInit (OMX_HANDLETYPE
                                                           hComponent);
extern OMX_ERRORTYPE DomxProxy_OMX_TI_VLPB_ComponentInit (OMX_HANDLETYPE
                                                           hComponent);
extern OMX_ERRORTYPE DomxProxy_OMX_TI_AUDDEC_ComponentInit (OMX_HANDLETYPE
                                                          hComponent);
extern OMX_ERRORTYPE DomxProxy_OMX_TI_AUDENC_ComponentInit (OMX_HANDLETYPE
                                                          hComponent);
extern OMX_ERRORTYPE DomxProxy_OMX_TI_VSWMOSAIC_ComponentInit (OMX_HANDLETYPE
                                                          hComponent);
extern OMX_ERRORTYPE DomxProxy_OMX_TI_VIDSPLC_ComponentInit (OMX_HANDLETYPE hComponent);

/* Nothing beyond this point */
