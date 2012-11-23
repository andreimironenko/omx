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
 *  @file   omxcore_cfg.c
 *  @brief  This file contains TI's implementation of the OMX methods
 *
 *          This file contains code that statically configures the
 *          omxcore module
 *
 *  @rev 1.0
 *******************************************************************************
 */

#include <string.h>
#include <xdc/std.h>
#include <xdc/runtime/Gate.h>

#include "app_cfg.h"
#include <omxcore_cfg.h>
#include <ti/omx/interfaces/openMaxv11/interfaces_openmaxv11_internal.h>

#include <OMX_Component.h>
#include <ti/omx/omxcore/inc/OMX_ComponentTable.h>

#include <omx_vdec.h>
#include <omx_venc.h>
#include <omx_vsplc.h>
#include <omx_vfcc.h>
#include <omx_vfdc.h>
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
#ifdef _BUILD_adec_
#include <omx_adec.h>
#endif
#ifdef _BUILD_aenc_
#include <omx_aenc.h>
#endif

#ifdef _BUILD_vswmosaic_
 #include <omx_vswmosaic.h>
#endif


#include "domx_compfn_defs.h"

OMX_COMPONENTLIST OMXComponentList[] =
{
#ifdef _BUILD_vfcc_
  {
   OMX_VFCC_COMP_NAME,
   VFCC_COMP_INIT_FNPTR,
   0,
   {{0}
   }
  }
  ,
#endif
#ifdef _BUILD_vfdc_
  {
   OMX_VFDC_COMP_NAME,
   VFDC_COMP_INIT_FNPTR,
   0,
   {{0}
   } 
  }
  ,
#endif
#ifdef _BUILD_vfpc_deihdualout_
  {
   OMX_VFPC_DEIDUALOUT_HIGH_COMP_NAME,
   VFPC_DEIHDUALOUT_COMP_INIT_FNPTR,
   0,
   {{0}
   }
  }
  ,
#endif
#ifdef _BUILD_vfpc_deimdualout_
  {
   OMX_VFPC_DEIDUALOUT_MEDIUM_COMP_NAME,
   VFPC_DEIMDUALOUT_COMP_INIT_FNPTR,
   0,
   {{0}
   }
  }
  ,
#endif
#ifdef _BUILD_vfpc_nf_
  {
   OMX_VFPC_NF_COMP_NAME,
   VFPC_NF_COMP_INIT_FNPTR,
   0,
   {{0}
   }
  }
  ,
#endif
#ifdef _BUILD_vfpc_indtxscwb_
  {
   OMX_VFPC_INDTXSCWB_COMP_NAME,
   VFPC_INDTXSCWB_COMP_INIT_FNPTR,
   0,
   {{0}
   }
  }
  ,
#endif
#ifdef _BUILD_vsrc_
  {
   OMX_VSRC_COMP_NAME,
   VSRC_COMP_INIT_FNPTR,
   0,
   {{0}
   }
  }
  ,
#endif
#ifdef _BUILD_vsnk_
  {
   OMX_VSNK_COMP_NAME,
   VSNK_COMP_INIT_FNPTR,
   0,
   {{0}
   }
  }
  ,
#endif
#ifdef _BUILD_ctrl_
  {
   OMX_CTRL_TVP_COMP_NAME,
   CTRL_COMP_INIT_FNPTR,
   0,
   {{0}
   }
  }
  ,
#endif
#ifdef _BUILD_ctrl_
  {
   OMX_CTRL_DC_COMP_NAME,
   CTRLDC_COMP_INIT_FNPTR,
   0,
   {{0}
   }
  }
  ,
#endif
#ifdef _BUILD_vswmosaic_
  {
   OMX_VSWMOSAIC_COMP_NAME,
   VSWMOSAIC_COMP_INIT_FNPTR,
   0,
   {{0}
   }
  }
  ,
#endif
#ifdef _BUILD_venc_
  {
   OMX_VIDENC_COMPONENT_NAME,
   VENC_COMP_INIT_FNPTR,
   0,
   {{0}
   }
  }
  ,
#endif
#ifdef _BUILD_vdec_
  {
   OMX_VIDDEC_COMPONENT_NAME,
   VDEC_COMP_INIT_FNPTR,
   0,
   {{0}
   }
  }
  ,
#endif
#ifdef _BUILD_vsplc_
  {
    OMX_VIDSPLC_COMPONENT_NAME,
    VSPLC_COMP_INIT_FNPTR,
    0,
    {{0}}
  }
  ,
#endif
#ifdef _BUILD_cmux_
  {
   OMX_CMUX_COMP_NAME,
   CMUX_COMP_INIT_FNPTR,
   0,
   {{0}
   }
  }
  ,
#endif
#ifdef _BUILD_dmux_
  {
   OMX_DMUX_COMP_NAME,
   DMUX_COMP_INIT_FNPTR,
   0,
   {{0}
   }
  }
  ,
#endif
#ifdef _BUILD_rtptx_
  {
   OMX_RTPTX_COMP_NAME,
   RTPTX_COMP_INIT_FNPTR,
   0,
   {{0}
   }
  }
  ,
#endif
#ifdef _BUILD_rtprx_
  {
   OMX_RTPRX_COMP_NAME,
   RTPRX_COMP_INIT_FNPTR,
   0,
   {{0}
   }
  }
  ,
#endif

#ifdef _BUILD_vlpb_
  {
   OMX_VLPB_HOSTCOMP_NAME,
   VLPB_COMP_INIT_FNPTR,
   0,
   {{0}
   }
  }
  ,
#endif
#ifdef _BUILD_vlpb_
  {
   OMX_VLPB_DSPCOMP_NAME,
   VLPB_DSPCOMP_INIT_FNPTR,
   0,
   {{0}
   }
  }
  ,
#endif
#ifdef _BUILD_adec_
  {
    OMX_AUDDEC_COMPONENT_NAME,
    AUDDEC_COMP_INIT_FNPTR,
    0,
    {{0}}
  },
#endif
#ifdef _BUILD_aenc_
  {
    OMX_AUDENC_COMPONENT_NAME,
    AUDENC_COMP_INIT_FNPTR,
    0,
    {{0}}
  },
#endif
  {
   NULL,                        /* Table Terminator . Should be last entry */
   NULL}

};

/* Nothing beyond this point */
