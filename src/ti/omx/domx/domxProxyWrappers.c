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
 *  @file  domxProxyWrappers.c
 *         This file contains methods that has the proxy wrappers for
 *         the each of the openMax components
 *
 *  @path \avmecomp\ti\omx\domx\domxProxyWrappers.c
 *
 *  @rev 1.0
 */

/******************************************************************
 *   INCLUDE FILES
 ******************************************************************/
#include <xdc/std.h>
#include <string.h>
#include <OMX_Component.h>
#include <OMX_Video.h>

#include <omx_vdec.h>
#include <omx_venc.h>
#include <omx_vfcc.h>
#include <omx_vfdc.h>
#include <omx_vsplc.h>
#include <omx_vfpc.h>
#ifdef _BUILD_vsnk_
 #include <omx_vsnk.h>
#endif
#ifdef _BUILD_vsrc_
 #include <omx_vsrc.h>
#endif
#include <omx_ctrl.h>
#ifdef _BUILD_cmux_
 #include <omx_cmux.h>
#endif
#ifdef _BUILD_dmux_
 #include <omx_dmux.h>
#endif
#ifdef _BUILD_rtptx_
 #include <omx_rtptx.h>
#endif
#ifdef _BUILD_rtprx_
 #include <omx_rtprx.h>
#endif
#ifdef _BUILD_vlpb_
 #include <omx_vlpb.h>
#endif

#ifdef _BUILD_vswmosaic_
 #include <omx_vswmosaic.h>
#endif

#ifdef _BUILD_adec_
 #include <omx_adec.h>
#endif
#ifdef _BUILD_aenc_
 #include <omx_aenc.h>
#endif
/** ============================================================================
 *  Extern declarations
 *  ============================================================================
 */
OMX_ERRORTYPE OmxProxy_commonInit (OMX_HANDLETYPE hComponent,
                                   OMX_STRING cComponentName);

/**
 * @name DomxProxy_OMX_TI_VFCC_ComponentInit()
 * @brief DomxProxy component init function for VFCC
 * @param hComponent  : Component handle
 * @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE DomxProxy_OMX_TI_VFCC_ComponentInit (OMX_HANDLETYPE hComponent)
{
  return OmxProxy_commonInit (hComponent, OMX_VFCC_COMP_NAME);

}

/**
 * @name DomxProxy_OMX_TI_VFDC_ComponentInit()
 * @brief DomxProxy component init function for VFDC
 * @param hComponent  : Component handle
 * @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE DomxProxy_OMX_TI_VFDC_ComponentInit (OMX_HANDLETYPE hComponent)
{
  return OmxProxy_commonInit (hComponent, OMX_VFDC_COMP_NAME);
}

/**
 * @name DomxProxy_OMX_TI_VFPC_DeiDualOutHigh_ComponentInit()
 * @brief DomxProxy component init function for DEIDualOut HighQuality
 * @param hComponent  : Component handle
 * @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE DomxProxy_OMX_TI_VFPC_DeiDualOutHigh_ComponentInit (OMX_HANDLETYPE
                                                                    hComponent)
{
  return OmxProxy_commonInit (hComponent, OMX_VFPC_DEIDUALOUT_HIGH_COMP_NAME);
}

/**
 * @name DomxProxy_OMX_TI_VFPC_DeiDualOutMedium_ComponentInit()
 * @brief DomxProxy component init function for DEIDualOut Medium Quality
 * @param hComponent  : Component handle
 * @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE DomxProxy_OMX_TI_VFPC_DeiDualOutMedium_ComponentInit
                (OMX_HANDLETYPE hComponent)
{
  return OmxProxy_commonInit (hComponent, OMX_VFPC_DEIDUALOUT_MEDIUM_COMP_NAME);
}

/**
 * @name DomxProxy_OMX_TI_VFPC_NF_ComponentInit()
 * @brief DomxProxy component init function for Noise filter
 * @param hComponent  : Component handle
 * @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE DomxProxy_OMX_TI_VFPC_NF_ComponentInit (OMX_HANDLETYPE hComponent)
{
  return OmxProxy_commonInit (hComponent, OMX_VFPC_NF_COMP_NAME);
}

/**
 * @name DomxProxy_OMX_TI_VFPC_IndTxSc_ComponentInit()
 * @brief DomxProxy component init function for IndTx Scalar
 * @param hComponent  : Component handle
 * @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE DomxProxy_OMX_TI_VFPC_IndTxSc_ComponentInit (OMX_HANDLETYPE
                                                             hComponent)
{
  return OmxProxy_commonInit (hComponent, OMX_VFPC_INDTXSCWB_COMP_NAME);
}

/**
 * @name DomxProxy_OMX_TI_CTRL_TVP_ComponentInit()
 * @brief DomxProxy component init function for TVP Control component
 * @param hComponent  : Component handle
 * @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE DomxProxy_OMX_TI_CTRL_TVP_ComponentInit (OMX_HANDLETYPE
                                                         hComponent)
{
  return OmxProxy_commonInit (hComponent, OMX_CTRL_TVP_COMP_NAME);
}

/**
 * @name DomxProxy_OMX_TI_CTRL_DC_ComponentInit()
 * @brief DomxProxy component init function for Display controller component
 * @param hComponent  : Component handle
 * @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE DomxProxy_OMX_TI_CTRL_DC_ComponentInit (OMX_HANDLETYPE hComponent)
{
  return OmxProxy_commonInit (hComponent, OMX_CTRL_DC_COMP_NAME);
}

/**
 * @name DomxProxy_OMX_TI_VIDENC_ComponentInit()
 * @brief DomxProxy component init function for VideoEncoder component
 * @param hComponent  : Component handle
 * @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE DomxProxy_OMX_TI_VIDENC_ComponentInit (OMX_HANDLETYPE hComponent)
{
  return OmxProxy_commonInit (hComponent, OMX_VIDENC_COMPONENT_NAME);
}

/**
 * @name DomxProxy_OMX_TI_VIDDEC_ComponentInit()
 * @brief DomxProxy component init function for VideoDecoder component
 * @param hComponent  : Component handle
 * @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE DomxProxy_OMX_TI_VIDDEC_ComponentInit (OMX_HANDLETYPE hComponent)
{
  return OmxProxy_commonInit (hComponent, OMX_VIDDEC_COMPONENT_NAME);
}

/**
 * @name DomxProxy_OMX_TI_VIDSPLC_ComponentInit()
 * @brief DomxProxy component init function for VideoSplitter component
 * @param hComponent  : Component handle
 * @return OMX_ERRORTYPE
 */
OMX_ERRORTYPE DomxProxy_OMX_TI_VIDSPLC_ComponentInit (OMX_HANDLETYPE hComponent)
{
    return OmxProxy_commonInit(hComponent, OMX_VIDSPLC_COMPONENT_NAME);
}

/**
 * @name DomxProxy_OMX_TI_VSRC_ComponentInit()
 * @brief DomxProxy component init function for VSRC component
 * @param hComponent  : Component handle
 * @return OMX_ERRORTYPE
 */
#ifdef _BUILD_vsrc_
OMX_ERRORTYPE DomxProxy_OMX_TI_VSRC_ComponentInit (OMX_HANDLETYPE hComponent)
{
  return OmxProxy_commonInit (hComponent, OMX_VSRC_COMP_NAME);
}
#endif

/**
 * @name DomxProxy_OMX_TI_VSNK_ComponentInit()
 * @brief DomxProxy component init function for VSNK component
 * @param hComponent  : Component handle
 * @return OMX_ERRORTYPE
 */
#ifdef _BUILD_vsnk_
OMX_ERRORTYPE DomxProxy_OMX_TI_VSNK_ComponentInit (OMX_HANDLETYPE hComponent)
{
  return OmxProxy_commonInit (hComponent, OMX_VSNK_COMP_NAME);
}
#endif

/**
 * @name DomxProxy_OMX_TI_CMUX_ComponentInit()
 * @brief DomxProxy component init function for CMUX component
 * @param hComponent  : Component handle
 * @return OMX_ERRORTYPE
 */
#ifdef _BUILD_cmux_
OMX_ERRORTYPE DomxProxy_OMX_TI_CMUX_ComponentInit (OMX_HANDLETYPE hComponent)
{
  return OmxProxy_commonInit (hComponent, OMX_CMUX_COMP_NAME);
}
#endif

/**
 * @name DomxProxy_OMX_TI_DMUX_ComponentInit()
 * @brief DomxProxy component init function for DMUX component
 * @param hComponent  : Component handle
 * @return OMX_ERRORTYPE
 */
#ifdef _BUILD_dmux_
OMX_ERRORTYPE DomxProxy_OMX_TI_DMUX_ComponentInit (OMX_HANDLETYPE hComponent)
{
  return OmxProxy_commonInit (hComponent, OMX_DMUX_COMP_NAME);
}
#endif

/**
 * @name DomxProxy_OMX_TI_RTPTX_ComponentInit()
 * @brief DomxProxy component init function for RTPTX component
 * @param hComponent  : Component handle
 * @return OMX_ERRORTYPE
 */
#ifdef _BUILD_rtptx_
OMX_ERRORTYPE DomxProxy_OMX_TI_RTPTX_ComponentInit (OMX_HANDLETYPE hComponent)
{
  return OmxProxy_commonInit (hComponent, OMX_RTPTX_COMP_NAME);
}
#endif

/**
 * @name DomxProxy_OMX_TI_RTPRX_ComponentInit()
 * @brief DomxProxy component init function for RTPRX component
 * @param hComponent  : Component handle
 * @return OMX_ERRORTYPE
 */
#ifdef _BUILD_rtprx_
OMX_ERRORTYPE DomxProxy_OMX_TI_RTPRX_ComponentInit (OMX_HANDLETYPE hComponent)
{
  return OmxProxy_commonInit (hComponent, OMX_RTPRX_COMP_NAME);
}
#endif

/**
 * @name DomxProxy_OMX_TI_VLPB_ComponentInit()
 * @brief DomxProxy component init function for VLPB component
 * @param hComponent  : Component handle
 * @return OMX_ERRORTYPE
 */

#ifdef _BUILD_vlpb_
OMX_ERRORTYPE DomxProxy_OMX_TI_VLPB_ComponentInit (OMX_HANDLETYPE hComponent)
{
  return OmxProxy_commonInit (hComponent, OMX_VLPB_DSPCOMP_NAME);
}
#endif

/**
 * @name DomxProxy_OMX_TI_AUDDEC_ComponentInit()
 * @brief DomxProxy component init function for ADEC component
 * @param hComponent  : Component handle
 * @return OMX_ERRORTYPE
 */

#ifdef _BUILD_adec_
OMX_ERRORTYPE DomxProxy_OMX_TI_AUDDEC_ComponentInit (OMX_HANDLETYPE hComponent)
{
  return OmxProxy_commonInit(hComponent, OMX_AUDDEC_COMPONENT_NAME);
}
#endif

/**
* @name DomxProxy_OMX_TI_AUDENC_ComponentInit()
* @brief DomxProxy component init function for AENC component
* @param hComponent  : Component handle
* @return OMX_ERRORTYPE
*/

#ifdef _BUILD_aenc_
OMX_ERRORTYPE DomxProxy_OMX_TI_AUDENC_ComponentInit (OMX_HANDLETYPE hComponent)
{
  return OmxProxy_commonInit(hComponent, OMX_AUDENC_COMPONENT_NAME);
}
#endif

/**
 * @name DomxProxy_OMX_TI_VSWMOSAIC_ComponentInit()
 * @brief DomxProxy component init function for VSWMOSAIC component
 * @param hComponent  : Component handle
 * @return OMX_ERRORTYPE
 */

#ifdef _BUILD_vswmosaic_
OMX_ERRORTYPE DomxProxy_OMX_TI_VSWMOSAIC_ComponentInit (OMX_HANDLETYPE hComponent)
{
  return OmxProxy_commonInit (hComponent, OMX_VSWMOSAIC_COMP_NAME);
}
#endif

/* Nothing beyond this point */
