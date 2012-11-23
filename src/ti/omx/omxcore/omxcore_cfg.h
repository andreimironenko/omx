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
 *  @file   domx_cfg.h
 *  @brief  This file contains default static configuration for the OMX methods
 *          of Distributed OMX.
 *          A integrator would typically use this file and re-define constants
 *          that are used to statically configure the module.
 *
 *  @rev 1.0
 *******************************************************************************
 */

#include <OMX_Core.h>

/* Component 0 */
#define DOMX_CORE_venc_m3video  /* Signifies that venc runs on video-M3 core */

/* Component 1 */
#define DOMX_CORE_vdec_m3video  /* Signifies that vdec runs on video-M3 core */

/* Component 2 */
#define DOMX_CORE_vfcc_m3vpss   /* Signifies that vfcc runs on vpss-M3 core */

/* Component 3 */
#define DOMX_CORE_vfdc_m3vpss   /* Signifies that vfdc runs on vpss-M3 core */

/* Component 4 */
#define DOMX_CORE_vfpc_deihdualout_m3vpss       /* Signifies that vfpc runs on
                                                   vpss-M3 core */

/* Component 5 */
#define DOMX_CORE_vfpc_deimdualout_m3vpss       /* Signifies that vfpc runs on
                                                   vpss-M3 core */

/* Component 6 */
#define DOMX_CORE_vfpc_nf_m3vpss        /* Signifies that vfpc runs on vpss-M3
                                           core */

/* Component 7 */
#define DOMX_CORE_vfpc_indtxscwb_m3vpss /* Signifies that vfpc runs on vpss-M3
                                           core */

/* Component 8 */
#define DOMX_CORE_ctrl_m3vpss   /* Signifies that ctrl runs on vpss-M3 core */

/* Component 9 */
#define DOMX_CORE_ctrldc_m3vpss /* Signifies that ctrl.dc runs on vpss-M3 core*/

/* Component 10 */
#define DOMX_CORE_vswmosaic_m3vpss   /* Signifies that vswmosaic runs on VPSS_m3 core */

/* Component 11 */
#define DOMX_CORE_cmux_a8host   /* Signifies that cmux runs on host-A8 core */

/* Component 12 */
#define DOMX_CORE_dmux_a8host   /* Signifies that dmux runs on host-A8 core */

/* Component 13 */
#define DOMX_CORE_rtptx_a8host  /* Signifies that rtptx runs on host-A8 core */

/* Component 14 */
#define DOMX_CORE_rtprx_a8host  /* Signifies that rtprx runs on host-A8 core */

/* Component 15 */
#define DOMX_CORE_vlpb_c6xdsp   /* Signifies that vlpb runs on DSP core */

/* Component 16 */
#define DOMX_CORE_auddec_c6xdsp   /* Signifies that adec runs on DSP core */

/* Component 17 */
#define DOMX_CORE_audenc_c6xdsp   /* Signifies that adec runs on DSP core */

/* Component 18 */
#define DOMX_CORE_vsplc_m3vpss   /* Signifies that vsplc runs on VPSS_m3 core */

/* Statically configurable parameters - re-define it in app_cfg.h if it need to
   be different than defined here */

/* Component init function pointers */
#ifdef _LOCAL_CORE_m3vpss_      /* Current iteration of the build is for m3vpss 
                                 */

 /* VFCC */
#ifndef VFCC_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_vfcc_m3vpss    /* resides on local core */
  #define VFCC_COMP_INIT_FNPTR    (OMX_TI_VFCC_ComponentInit)
 #else /* resides on the remote core */
  #define VFCC_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VFCC_ComponentInit)
 #endif
#endif

 /* VSWMOSAIC */
#ifndef VSWMOSAIC_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_vswmosaic_m3vpss    /* resides on local core */
  #define VSWMOSAIC_COMP_INIT_FNPTR    (OMX_TI_VSWMOSAIC_ComponentInit)
 #else /* resides on the remote core */
  #define VSWMOSAIC_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VSWMOSAIC_ComponentInit)
 #endif
#endif

 /* VFDC */
#ifndef VFDC_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_vfdc_m3vpss    /* resides on local core */
  #define VFDC_COMP_INIT_FNPTR    (OMX_TI_VFDC_ComponentInit)
 #else /* resides on the remote core */
  #define VFDC_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VFDC_ComponentInit)
 #endif
#endif

 /* VFPC DEI-H */
#ifndef VFPC_DEIHDUALOUT_COMP_INIT_FNPTR        /* It is not defined by the
                                                   app/integrator */
 #ifdef DOMX_CORE_vfpc_deihdualout_m3vpss        /* resides on local core */
  #define VFPC_DEIHDUALOUT_COMP_INIT_FNPTR (OMX_TI_VFPC_DeiDualOutHigh_ComponentInit)
 #else /* resides on the remote core */
  #define VFPC_DEIHDUALOUT_COMP_INIT_FNPTR (DomxProxy_OMX_TI_VFPC_DeiDualOutHigh_ComponentInit)
 #endif
#endif

 /* VFPC DEI-M */
#ifndef VFPC_DEIMDUALOUT_COMP_INIT_FNPTR        /* It is not defined by the
                                                   app/integrator */
 #ifdef DOMX_CORE_vfpc_deimdualout_m3vpss        /* resides on local core */
  #define VFPC_DEIMDUALOUT_COMP_INIT_FNPTR (OMX_TI_VFPC_DeiDualOutMedium_ComponentInit)
 #else /* resides on the remote core */
  #define VFPC_DEIMDUALOUT_COMP_INIT_FNPTR (DomxProxy_OMX_TI_VFPC_DeiDualOutMedium_ComponentInit)
 #endif
#endif

 /* VFPC Ind. Tx Scalar Write-back */
#ifndef VFPC_INDTXSCWB_COMP_INIT_FNPTR  /* It is not defined by the
                                           app/integrator */
 #ifdef DOMX_CORE_vfpc_indtxscwb_m3vpss  /* resides on local core */
  #define VFPC_INDTXSCWB_COMP_INIT_FNPTR    (OMX_TI_VFPC_IndTxSc_ComponentInit)
 #else /* resides on the remote core */
  #define VFPC_INDTXSCWB_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VFPC_IndTxSc_ComponentInit)
 #endif
#endif

 /* VFPC Noise Filter */
#ifndef VFPC_NF_COMP_INIT_FNPTR /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_vfpc_nf_m3vpss /* resides on local core */
  #define VFPC_NF_COMP_INIT_FNPTR    (OMX_TI_VFPC_NF_ComponentInit)
 #else /* resides on the remote core */
  #define VFPC_NF_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VFPC_NF_ComponentInit)
 #endif
#endif

 /* VSRC */
#ifndef VSRC_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_vsrc_m3vpss    /* resides on local core */
  #define VSRC_COMP_INIT_FNPTR    (OMX_TI_VSRC_ComponentInit)
 #else /* resides on the remote core */
  #define VSRC_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VSRC_ComponentInit)
 #endif
#endif

 /* VSNK */
#ifndef VSNK_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_vsnk_m3vpss    /* resides on local core */
  #define VSNK_COMP_INIT_FNPTR    (OMX_TI_VSNK_ComponentInit)
 #else /* resides on the remote core */
  #define VSNK_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VSNK_ComponentInit)
 #endif
#endif

 /* CTRL TVP */
#ifndef CTRL_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_ctrl_m3vpss    /* resides on local core */
  #define CTRL_COMP_INIT_FNPTR    (OMX_TI_CTRL_TVP_ComponentInit)
 #else /* resides on the remote core */
  #define CTRL_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_CTRL_TVP_ComponentInit)
 #endif
#endif

 /* CTRL DC */
#ifndef CTRLDC_COMP_INIT_FNPTR  /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_ctrldc_m3vpss  /* resides on local core */
  #define CTRLDC_COMP_INIT_FNPTR    (OMX_TI_CTRL_DC_ComponentInit)
 #else /* resides on the remote core */
  #define CTRLDC_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_CTRL_DC_ComponentInit)
 #endif
#endif

 /* VENC */
#ifndef VENC_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_venc_m3vpss    /* resides on local core */
  #define VENC_COMP_INIT_FNPTR    (OMX_TI_VIDENC_ComponentInit)
 #else /* resides on the remote core */
  #define VENC_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VIDENC_ComponentInit)
 #endif
#endif

 /* VDEC */
#ifndef VDEC_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_vdec_m3vpss    /* resides on local core */
  #define VDEC_COMP_INIT_FNPTR    (OMX_TI_VIDDEC_ComponentInit)
 #else /* resides on the remote core */
  #define VDEC_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VIDDEC_ComponentInit)
  #endif
 #endif
 
 /* VSPLC */
 #ifndef VSPLC_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
  #ifdef DOMX_CORE_vsplc_m3vpss               /* resides on local core */
   #define VSPLC_COMP_INIT_FNPTR    (OMX_TI_VIDSPLC_ComponentInit)
  #else                                 /* resides on the remote core */
   #define VSPLC_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VIDSPLC_ComponentInit)
 #endif
#endif

 /* CMUX */
#ifndef CMUX_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_cmux_m3vpss    /* resides on local core */
  #define CMUX_COMP_INIT_FNPTR    (OMX_TI_CMUX_ComponentInit)
 #else /* resides on the remote core */
  #define CMUX_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_CMUX_ComponentInit)
 #endif
#endif

 /* DMUX */
#ifndef DMUX_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_dmux_m3vpss    /* resides on local core */
  #define DMUX_COMP_INIT_FNPTR    (OMX_TI_DMUX_ComponentInit)
 #else /* resides on the remote core */
  #define DMUX_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_DMUX_ComponentInit)
 #endif
#endif

 /* RTPTX */
#ifndef RTPTX_COMP_INIT_FNPTR   /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_rtptx_m3vpss   /* resides on local core */
  #define RTPTX_COMP_INIT_FNPTR    (OMX_TI_RTPTX_ComponentInit)
 #else /* resides on the remote core */
  #define RTPTX_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_RTPTX_ComponentInit)
 #endif
#endif

 /* RTPRX */
#ifndef RTPRX_COMP_INIT_FNPTR   /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_rtprx_m3vpss   /* resides on local core */
  #define RTPRX_COMP_INIT_FNPTR    (OMX_TI_RTPRX_ComponentInit)
 #else /* resides on the remote core */
  #define RTPRX_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_RTPRX_ComponentInit)
 #endif
#endif

  /* VLPB */
#ifndef VLPB_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_vlpb_m3vpss    /* resides on local core */
  #define VLPB_COMP_INIT_FNPTR    (OMX_TI_VLPB_ComponentInit)
 #else /* resides on the remote core */
  #define VLPB_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VLPB_ComponentInit)
 #endif
#endif

  /* VLPB */
#ifndef VLPB_DSPCOMP_INIT_FNPTR  /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_vlpb_m3vpss    /* resides on local core */
  #define VLPB_DSPCOMP_INIT_FNPTR (OMX_TI_VLPB_ComponentInit)
 #else /* resides on the remote core */
  #define VLPB_DSPCOMP_INIT_FNPTR (DomxProxy_OMX_TI_VLPB_ComponentInit)
 #endif
#endif

/* AUDDEC */
 #ifndef AUDDEC_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
  #ifdef DOMX_CORE_auddec_m3vpss               /* resides on local core */
   #define AUDDEC_COMP_INIT_FNPTR    (OMX_TI_AUDDEC_ComponentInit)
  #else                                 /* resides on the remote core */
   #define AUDDEC_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_AUDDEC_ComponentInit)
  #endif
 #endif

/* AUDENC */
 #ifndef AUDENC_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
  #ifdef DOMX_CORE_audenc_m3vpss               /* resides on local core */
   #define AUDENC_COMP_INIT_FNPTR    (OMX_TI_AUDENC_ComponentInit)
  #else                                 /* resides on the remote core */
   #define AUDENC_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_AUDENC_ComponentInit)
  #endif
 #endif

#endif /* _LOCAL_CORE_m3vpss_ */

#ifdef _LOCAL_CORE_m3video_     /* Current iteration of the build is for
                                   m3video */

 /* VFCC */
#ifndef VFCC_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_vfcc_m3video   /* resides on local core */
  #define VFCC_COMP_INIT_FNPTR    (OMX_TI_VFCC_ComponentInit)
 #else /* resides on the remote core */
  #define VFCC_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VFCC_ComponentInit)
 #endif
#endif

 /* VSWMOSAIC */
#ifndef VSWMOSAIC_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_vswmosaic_m3video   /* resides on local core */
  #define VSWMOSAIC_COMP_INIT_FNPTR    (OMX_TI_VSWMOSAIC_ComponentInit)
 #else /* resides on the remote core */
  #define VSWMOSAIC_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VSWMOSAIC_ComponentInit)
 #endif
#endif

 /* VFDC */
#ifndef VFDC_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_vfdc_m3video   /* resides on local core */
  #define VFDC_COMP_INIT_FNPTR    (OMX_TI_VFDC_ComponentInit)
 #else /* resides on the remote core */
  #define VFDC_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VFDC_ComponentInit)
 #endif
#endif

 /* VFPC DEI-H */
#ifndef VFPC_DEIHDUALOUT_COMP_INIT_FNPTR        /* It is not defined by the
                                                   app/integrator */
 #ifdef DOMX_CORE_vfpc_deihdualout_m3video       /* resides on local core */
  #define VFPC_DEIHDUALOUT_COMP_INIT_FNPTR (OMX_TI_VFPC_DeiDualOutHigh_ComponentInit)
 #else /* resides on the remote core */
  #define VFPC_DEIHDUALOUT_COMP_INIT_FNPTR (DomxProxy_OMX_TI_VFPC_DeiDualOutHigh_ComponentInit)
 #endif
#endif

 /* VFPC DEI-M */
#ifndef VFPC_DEIMDUALOUT_COMP_INIT_FNPTR        /* It is not defined by the
                                                   app/integrator */
 #ifdef DOMX_CORE_vfpc_deimdualout_m3video       /* resides on local core */
  #define VFPC_DEIMDUALOUT_COMP_INIT_FNPTR (OMX_TI_VFPC_DeiDualOutMedium_ComponentInit)
 #else /* resides on the remote core */
  #define VFPC_DEIMDUALOUT_COMP_INIT_FNPTR (DomxProxy_OMX_TI_VFPC_DeiDualOutMedium_ComponentInit)
 #endif
#endif

 /* VFPC Ind. Tx Scalar Write-back */
#ifndef VFPC_INDTXSCWB_COMP_INIT_FNPTR  /* It is not defined by the
                                           app/integrator */
 #ifdef DOMX_CORE_vfpc_indtxscwb_m3video /* resides on local core */
  #define VFPC_INDTXSCWB_COMP_INIT_FNPTR    (OMX_TI_VFPC_IndTxSc_ComponentInit)
 #else /* resides on the remote core */
  #define VFPC_INDTXSCWB_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VFPC_IndTxSc_ComponentInit)
 #endif
#endif

 /* VFPC Noise Filter */
#ifndef VFPC_NF_COMP_INIT_FNPTR /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_vfpc_nf_m3video        /* resides on local core */
  #define VFPC_NF_COMP_INIT_FNPTR    (OMX_TI_VFPC_NF_ComponentInit)
 #else /* resides on the remote core */
  #define VFPC_NF_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VFPC_NF_ComponentInit)
 #endif
#endif

 /* VSRC */
#ifndef VSRC_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_vsrc_m3video   /* resides on local core */
  #define VSRC_COMP_INIT_FNPTR    (OMX_TI_VSRC_ComponentInit)
 #else /* resides on the remote core */
  #define VSRC_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VSRC_ComponentInit)
 #endif
#endif

 /* VSNK */
#ifndef VSNK_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_vsnk_m3video   /* resides on local core */
  #define VSNK_COMP_INIT_FNPTR    (OMX_TI_VSNK_ComponentInit)
 #else /* resides on the remote core */
  #define VSNK_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VSNK_ComponentInit)
 #endif
#endif

 /* CTRL TVP */
#ifndef CTRL_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_ctrl_m3video   /* resides on local core */
  #define CTRL_COMP_INIT_FNPTR    (OMX_TI_CTRL_TVP_ComponentInit)
 #else /* resides on the remote core */
  #define CTRL_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_CTRL_TVP_ComponentInit)
 #endif
#endif

 /* CTRL DC */
#ifndef CTRLDC_COMP_INIT_FNPTR  /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_ctrldc_m3video /* resides on local core */
  #define CTRLDC_COMP_INIT_FNPTR    (OMX_TI_CTRL_DC_ComponentInit)
 #else /* resides on the remote core */
  #define CTRLDC_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_CTRL_DC_ComponentInit)
 #endif
#endif

 /* VENC */
#ifndef VENC_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_venc_m3video   /* resides on local core */
  #define VENC_COMP_INIT_FNPTR    (OMX_TI_VIDENC_ComponentInit)
 #else /* resides on the remote core */
  #define VENC_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VIDENC_ComponentInit)
 #endif
#endif

 /* VDEC */
#ifndef VDEC_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_vdec_m3video   /* resides on local core */
  #define VDEC_COMP_INIT_FNPTR    (OMX_TI_VIDDEC_ComponentInit)
 #else /* resides on the remote core */
  #define VDEC_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VIDDEC_ComponentInit)
 #endif
#endif
 
 /* VSPLC */
 #ifndef VSPLC_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
  #ifdef DOMX_CORE_vsplc_m3video               /* resides on local core */
   #define VSPLC_COMP_INIT_FNPTR    (OMX_TI_VIDSPLC_ComponentInit)
  #else                                 /* resides on the remote core */
   #define VSPLC_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VIDSPLC_ComponentInit)
  #endif
 #endif

 /* CMUX */
#ifndef CMUX_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_cmux_m3video   /* resides on local core */
  #define CMUX_COMP_INIT_FNPTR    (OMX_TI_CMUX_ComponentInit)
 #else /* resides on the remote core */
  #define CMUX_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_CMUX_ComponentInit)
 #endif
#endif

 /* DMUX */
#ifndef DMUX_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_dmux_m3video   /* resides on local core */
  #define DMUX_COMP_INIT_FNPTR    (OMX_TI_DMUX_ComponentInit)
 #else /* resides on the remote core */
  #define DMUX_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_DMUX_ComponentInit)
 #endif
#endif

 /* RTPTX */
#ifndef RTPTX_COMP_INIT_FNPTR   /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_rtptx_m3video  /* resides on local core */
  #define RTPTX_COMP_INIT_FNPTR    (OMX_TI_RTPTX_ComponentInit)
 #else /* resides on the remote core */
  #define RTPTX_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_RTPTX_ComponentInit)
 #endif
#endif

 /* RTPRX */
#ifndef RTPRX_COMP_INIT_FNPTR   /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_rtprx_m3video  /* resides on local core */
  #define RTPRX_COMP_INIT_FNPTR    (OMX_TI_RTPRX_ComponentInit)
 #else /* resides on the remote core */
  #define RTPRX_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_RTPRX_ComponentInit)
 #endif
#endif
 /* VLPB */
#ifndef VLPB_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_vlpb_m3video   /* resides on local core */
  #define VLPB_COMP_INIT_FNPTR    (OMX_TI_VLPB_ComponentInit)
 #else /* resides on the remote core */
  #define VLPB_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VLPB_ComponentInit)
 #endif
#endif
 /* VLPB */
#ifndef VLPB_DSPCOMP_INIT_FNPTR  /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_vlpb_m3video   /* resides on local core */
  #define VLPB_DSPCOMP_INIT_FNPTR (OMX_TI_VLPB_ComponentInit)
 #else /* resides on the remote core */
  #define VLPB_DSPCOMP_INIT_FNPTR (DomxProxy_OMX_TI_VLPB_ComponentInit)
 #endif
#endif

/* AUDDEC */
 #ifndef AUDDEC_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
  #ifdef DOMX_CORE_auddec_m3video               /* resides on local core */
   #define AUDDEC_COMP_INIT_FNPTR    (OMX_TI_AUDDEC_ComponentInit)
  #else                                 /* resides on the remote core */
   #define AUDDEC_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_AUDDEC_ComponentInit)
  #endif
 #endif

/* AUDENC */
 #ifndef AUDENC_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
  #ifdef DOMX_CORE_audenc_m3video               /* resides on local core */
   #define AUDENC_COMP_INIT_FNPTR    (OMX_TI_AUDENC_ComponentInit)
  #else                                 /* resides on the remote core */
   #define AUDENC_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_AUDENC_ComponentInit)
  #endif
 #endif

#endif /* _LOCAL_CORE_m3video_ */

#ifdef _LOCAL_CORE_a8host_      /* Current iteration of the build is for a8host 
                                 */

 /* VFCC */
#ifndef VFCC_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_vfcc_a8host    /* resides on local core */
  #define VFCC_COMP_INIT_FNPTR    (OMX_TI_VFCC_ComponentInit)
 #else /* resides on the remote core */
  #define VFCC_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VFCC_ComponentInit)
 #endif
#endif

 /* VSWMOSAIC */
#ifndef VSWMOSAIC_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_vswmosaic_a8host    /* resides on local core */
  #define VSWMOSAIC_COMP_INIT_FNPTR    (OMX_TI_VSWMOSAIC_ComponentInit)
 #else /* resides on the remote core */
  #define VSWMOSAIC_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VSWMOSAIC_ComponentInit)
 #endif
#endif

 /* VFDC */
#ifndef VFDC_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_vfdc_a8host    /* resides on local core */
  #define VFDC_COMP_INIT_FNPTR    (OMX_TI_VFDC_ComponentInit)
 #else /* resides on the remote core */
  #define VFDC_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VFDC_ComponentInit)
 #endif
#endif

 /* VFPC DEI-H */
#ifndef VFPC_DEIHDUALOUT_COMP_INIT_FNPTR        /* It is not defined by the
                                                   app/integrator */
 #ifdef DOMX_CORE_vfpc_deihdualout_a8host        /* resides on local core */
  #define VFPC_DEIHDUALOUT_COMP_INIT_FNPTR (OMX_TI_VFPC_DeiDualOutHigh_ComponentInit)
 #else /* resides on the remote core */
  #define VFPC_DEIHDUALOUT_COMP_INIT_FNPTR (DomxProxy_OMX_TI_VFPC_DeiDualOutHigh_ComponentInit)
 #endif
#endif

 /* VFPC DEI-M */
#ifndef VFPC_DEIMDUALOUT_COMP_INIT_FNPTR        /* It is not defined by the
                                                   app/integrator */
 #ifdef DOMX_CORE_vfpc_deimdualout_a8host        /* resides on local core */
  #define VFPC_DEIMDUALOUT_COMP_INIT_FNPTR (OMX_TI_VFPC_DeiDualOutMedium_ComponentInit)
 #else /* resides on the remote core */
  #define VFPC_DEIMDUALOUT_COMP_INIT_FNPTR (DomxProxy_OMX_TI_VFPC_DeiDualOutMedium_ComponentInit)
 #endif
#endif

 /* VFPC Ind. Tx Scalar Write-back */
#ifndef VFPC_INDTXSCWB_COMP_INIT_FNPTR  /* It is not defined by the
                                           app/integrator */
 #ifdef DOMX_CORE_vfpc_indtxscwb_a8host  /* resides on local core */
  #define VFPC_INDTXSCWB_COMP_INIT_FNPTR    (OMX_TI_VFPC_IndTxSc_ComponentInit)
 #else /* resides on the remote core */
  #define VFPC_INDTXSCWB_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VFPC_IndTxSc_ComponentInit)
 #endif
#endif

 /* VFPC Noise Filter */
#ifndef VFPC_NF_COMP_INIT_FNPTR /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_vfpc_nf_a8host /* resides on local core */
  #define VFPC_NF_COMP_INIT_FNPTR    (OMX_TI_VFPC_NF_ComponentInit)
 #else /* resides on the remote core */
  #define VFPC_NF_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VFPC_NF_ComponentInit)
 #endif
#endif

 /* VSRC */
#ifndef VSRC_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_vsrc_a8host    /* resides on local core */
  #define VSRC_COMP_INIT_FNPTR    (OMX_TI_VSRC_ComponentInit)
 #else /* resides on the remote core */
  #define VSRC_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VSRC_ComponentInit)
 #endif
#endif

 /* VSNK */
#ifndef VSNK_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_vsnk_a8host    /* resides on local core */
  #define VSNK_COMP_INIT_FNPTR    (OMX_TI_VSNK_ComponentInit)
 #else /* resides on the remote core */
  #define VSNK_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VSNK_ComponentInit)
 #endif
#endif

 /* CTRL TVP */
#ifndef CTRL_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_ctrl_a8host    /* resides on local core */
  #define CTRL_COMP_INIT_FNPTR    (OMX_TI_CTRL_TVP_ComponentInit)
 #else /* resides on the remote core */
  #define CTRL_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_CTRL_TVP_ComponentInit)
 #endif
#endif

 /* CTRL DC */
#ifndef CTRLDC_COMP_INIT_FNPTR  /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_ctrldc_a8host  /* resides on local core */
  #define CTRLDC_COMP_INIT_FNPTR    (OMX_TI_CTRL_DC_ComponentInit)
 #else /* resides on the remote core */
  #define CTRLDC_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_CTRL_DC_ComponentInit)
 #endif
#endif

 /* VENC */
#ifndef VENC_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_venc_a8host    /* resides on local core */
  #define VENC_COMP_INIT_FNPTR    (OMX_TI_VIDENC_ComponentInit)
 #else /* resides on the remote core */
  #define VENC_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VIDENC_ComponentInit)
 #endif
#endif

 /* VDEC */
#ifndef VDEC_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_vdec_a8host    /* resides on local core */
  #define VDEC_COMP_INIT_FNPTR    (OMX_TI_VIDDEC_ComponentInit)
 #else /* resides on the remote core */
  #define VDEC_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VIDDEC_ComponentInit)
  #endif
 #endif
 
 /* VSPLC */
 #ifndef VSPLC_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
  #ifdef DOMX_CORE_vsplc_a8host               /* resides on local core */
   #define VSPLC_COMP_INIT_FNPTR    (OMX_TI_VIDSPLC_ComponentInit)
  #else                                 /* resides on the remote core */
   #define VSPLC_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VIDSPLC_ComponentInit)
 #endif
#endif

 /* CMUX */
#ifndef CMUX_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_cmux_a8host    /* resides on local core */
  #define CMUX_COMP_INIT_FNPTR    (OMX_TI_CMUX_ComponentInit)
 #else /* resides on the remote core */
  #define CMUX_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_CMUX_ComponentInit)
 #endif
#endif

 /* DMUX */
#ifndef DMUX_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_dmux_a8host    /* resides on local core */
  #define DMUX_COMP_INIT_FNPTR    (OMX_TI_DMUX_ComponentInit)
 #else /* resides on the remote core */
  #define DMUX_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_DMUX_ComponentInit)
 #endif
#endif

 /* RTPTX */
#ifndef RTPTX_COMP_INIT_FNPTR   /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_rtptx_a8host   /* resides on local core */
  #define RTPTX_COMP_INIT_FNPTR    (OMX_TI_RTPTX_ComponentInit)
 #else /* resides on the remote core */
  #define RTPTX_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_RTPTX_ComponentInit)
 #endif
#endif

 /* RTPRX */
#ifndef RTPRX_COMP_INIT_FNPTR   /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_rtprx_a8host   /* resides on local core */
  #define RTPRX_COMP_INIT_FNPTR    (OMX_TI_RTPRX_ComponentInit)
 #else /* resides on the remote core */
  #define RTPRX_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_RTPRX_ComponentInit)
 #endif
#endif
 /* VLPB */
#ifndef VLPB_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_vlpb_a8host    /* resides on local core */
  #define VLPB_COMP_INIT_FNPTR    (OMX_TI_VLPB_ComponentInit)
 #else /* resides on the remote core */
  #define VLPB_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VLPB_ComponentInit)
 #endif
#endif
 /* VLPB */
#ifndef VLPB_DSPCOMP_INIT_FNPTR  /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_vlpb_c6xdsp    /* resides on local core */
  #define VLPB_DSPCOMP_INIT_FNPTR (DomxProxy_OMX_TI_VLPB_ComponentInit)
 #else /* resides on the remote core */
  #define VLPB_DSPCOMP_INIT_FNPTR (OMX_TI_VLPB_ComponentInit)
 #endif
#endif

/* AUDDEC */
 #ifndef AUDDEC_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
  #ifdef DOMX_CORE_auddec_a8host               /* resides on local core */
   #define AUDDEC_COMP_INIT_FNPTR    (OMX_TI_AUDDEC_ComponentInit)
  #else                                 /* resides on the remote core */
   #define AUDDEC_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_AUDDEC_ComponentInit)
  #endif
 #endif

/* AUDENC */
 #ifndef AUDENC_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
  #ifdef DOMX_CORE_audenc_a8host               /* resides on local core */
   #define AUDENC_COMP_INIT_FNPTR    (OMX_TI_AUDENC_ComponentInit)
  #else                                 /* resides on the remote core */
   #define AUDENC_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_AUDENC_ComponentInit)
  #endif
 #endif

#endif /* _LOCAL_CORE_a8host_ */

#ifdef _LOCAL_CORE_c6xdsp_      /* Current iteration of the build is for c6xdsp 
                                 */

 /* VFCC */
#ifndef VFCC_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_vfcc_c6xdsp    /* resides on local core */
  #define VFCC_COMP_INIT_FNPTR    (OMX_TI_VFCC_ComponentInit)
 #else /* resides on the remote core */
  #define VFCC_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VFCC_ComponentInit)
 #endif
#endif

 /* VSWMOSAIC */
#ifndef VSWMOSAIC_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_vswmosaic_c6xdsp    /* resides on local core */
  #define VSWMOSAIC_COMP_INIT_FNPTR    (OMX_TI_VSWMOSAIC_ComponentInit)
 #else /* resides on the remote core */
  #define VSWMOSAIC_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VSWMOSAIC_ComponentInit)
 #endif
#endif

 /* VFDC */
#ifndef VFDC_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_vfdc_c6xdsp    /* resides on local core */
  #define VFDC_COMP_INIT_FNPTR    (OMX_TI_VFDC_ComponentInit)
 #else /* resides on the remote core */
  #define VFDC_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VFDC_ComponentInit)
 #endif
#endif

 /* VFPC DEI-H */
#ifndef VFPC_DEIHDUALOUT_COMP_INIT_FNPTR        /* It is not defined by the
                                                   app/integrator */
 #ifdef DOMX_CORE_vfpc_deihdualout_c6xdsp        /* resides on local core */
  #define VFPC_DEIHDUALOUT_COMP_INIT_FNPTR (OMX_TI_VFPC_DeiDualOutHigh_ComponentInit)
 #else /* resides on the remote core */
  #define VFPC_DEIHDUALOUT_COMP_INIT_FNPTR (DomxProxy_OMX_TI_VFPC_DeiDualOutHigh_ComponentInit)
 #endif
#endif

 /* VFPC DEI-M */
#ifndef VFPC_DEIMDUALOUT_COMP_INIT_FNPTR        /* It is not defined by the
                                                   app/integrator */
 #ifdef DOMX_CORE_vfpc_deimdualout_c6xdsp        /* resides on local core */
  #define VFPC_DEIMDUALOUT_COMP_INIT_FNPTR (OMX_TI_VFPC_DeiDualOutMedium_ComponentInit)
 #else /* resides on the remote core */
  #define VFPC_DEIMDUALOUT_COMP_INIT_FNPTR (DomxProxy_OMX_TI_VFPC_DeiDualOutMedium_ComponentInit)
 #endif
#endif

 /* VFPC Ind. Tx Scalar Write-back */
#ifndef VFPC_INDTXSCWB_COMP_INIT_FNPTR  /* It is not defined by the
                                           app/integrator */
 #ifdef DOMX_CORE_vfpc_indtxscwb_c6xdsp  /* resides on local core */
  #define VFPC_INDTXSCWB_COMP_INIT_FNPTR    (OMX_TI_VFPC_IndTxSc_ComponentInit)
 #else /* resides on the remote core */
  #define VFPC_INDTXSCWB_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VFPC_IndTxSc_ComponentInit)
 #endif
#endif

 /* VFPC Noise Filter */
#ifndef VFPC_NF_COMP_INIT_FNPTR /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_vfpc_nf_c6xdsp /* resides on local core */
  #define VFPC_NF_COMP_INIT_FNPTR    (OMX_TI_VFPC_NF_ComponentInit)
 #else /* resides on the remote core */
  #define VFPC_NF_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VFPC_NF_ComponentInit)
 #endif
#endif

 /* VSRC */
#ifndef VSRC_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_vsrc_c6xdsp    /* resides on local core */
  #define VSRC_COMP_INIT_FNPTR    (OMX_TI_VSRC_ComponentInit)
 #else /* resides on the remote core */
  #define VSRC_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VSRC_ComponentInit)
 #endif
#endif

 /* VSNK */
#ifndef VSNK_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_vsnk_c6xdsp    /* resides on local core */
  #define VSNK_COMP_INIT_FNPTR    (OMX_TI_VSNK_ComponentInit)
 #else /* resides on the remote core */
  #define VSNK_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VSNK_ComponentInit)
 #endif
#endif

 /* CTRL TVP */
#ifndef CTRL_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_ctrl_c6xdsp    /* resides on local core */
  #define CTRL_COMP_INIT_FNPTR    (OMX_TI_CTRL_TVP_ComponentInit)
 #else /* resides on the remote core */
  #define CTRL_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_CTRL_TVP_ComponentInit)
 #endif
#endif

 /* CTRL DC */
#ifndef CTRLDC_COMP_INIT_FNPTR  /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_ctrldc_c6xdsp  /* resides on local core */
  #define CTRLDC_COMP_INIT_FNPTR    (OMX_TI_CTRL_DC_ComponentInit)
 #else /* resides on the remote core */
  #define CTRLDC_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_CTRL_DC_ComponentInit)
 #endif
#endif

 /* VENC */
#ifndef VENC_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_venc_c6xdsp    /* resides on local core */
  #define VENC_COMP_INIT_FNPTR    (OMX_TI_VIDENC_ComponentInit)
 #else /* resides on the remote core */
  #define VENC_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VIDENC_ComponentInit)
 #endif
#endif

 /* VDEC */
#ifndef VDEC_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_vdec_c6xdsp    /* resides on local core */
  #define VDEC_COMP_INIT_FNPTR    (OMX_TI_VIDDEC_ComponentInit)
 #else /* resides on the remote core */
  #define VDEC_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VIDDEC_ComponentInit)
 #endif
#endif

 /* VSPLC */
#ifndef VSPLC_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_vsplc_c6xdsp    /* resides on local core */
  #define VSPLC_COMP_INIT_FNPTR    (OMX_TI_VIDSPLC_ComponentInit)
 #else /* resides on the remote core */
  #define VSPLC_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VIDSPLC_ComponentInit)
 #endif
#endif

 /* CMUX */
#ifndef CMUX_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_cmux_c6xdsp    /* resides on local core */
  #define CMUX_COMP_INIT_FNPTR    (OMX_TI_CMUX_ComponentInit)
 #else /* resides on the remote core */
  #define CMUX_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_CMUX_ComponentInit)
 #endif
#endif

 /* DMUX */
#ifndef DMUX_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_dmux_c6xdsp    /* resides on local core */
  #define DMUX_COMP_INIT_FNPTR    (OMX_TI_DMUX_ComponentInit)
 #else /* resides on the remote core */
  #define DMUX_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_DMUX_ComponentInit)
 #endif
#endif

 /* RTPTX */
#ifndef RTPTX_COMP_INIT_FNPTR   /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_rtptx_c6xdsp   /* resides on local core */
  #define RTPTX_COMP_INIT_FNPTR    (OMX_TI_RTPTX_ComponentInit)
 #else /* resides on the remote core */
  #define RTPTX_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_RTPTX_ComponentInit)
 #endif
#endif

 /* RTPRX */
#ifndef RTPRX_COMP_INIT_FNPTR   /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_rtprx_c6xdsp   /* resides on local core */
  #define RTPRX_COMP_INIT_FNPTR    (OMX_TI_RTPRX_ComponentInit)
 #else /* resides on the remote core */
  #define RTPRX_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_RTPRX_ComponentInit)
 #endif
#endif

  /* VLPB */
#ifndef VLPB_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_vlpb_c6xdsp    /* resides on local core */
  #define VLPB_COMP_INIT_FNPTR    (OMX_TI_VLPB_ComponentInit)
 #else /* resides on the remote core */
  #define VLPB_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_VLPB_ComponentInit)
 #endif
#endif

  /* VLPB */
#ifndef VLPB_DSPCOMP_INIT_FNPTR  /* It is not defined by the app/integrator */
 #ifdef DOMX_CORE_vlpb_c6xdsp    /* resides on local core */
  #define VLPB_DSPCOMP_INIT_FNPTR (OMX_TI_VLPB_ComponentInit)
 #else /* resides on the remote core */
  #define VLPB_DSPCOMP_INIT_FNPTR (DomxProxy_OMX_TI_VLPB_ComponentInit)
 #endif
#endif

/* AUDDEC */
 #ifndef AUDDEC_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
  #ifdef DOMX_CORE_auddec_c6xdsp               /* resides on local core */
   #define AUDDEC_COMP_INIT_FNPTR    (OMX_TI_AUDDEC_ComponentInit)
  #else                                 /* resides on the remote core */
   #define AUDDEC_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_AUDDEC_ComponentInit)
 #endif
#endif

/* AUDENC */
 #ifndef AUDENC_COMP_INIT_FNPTR    /* It is not defined by the app/integrator */
  #ifdef DOMX_CORE_audenc_c6xdsp               /* resides on local core */
   #define AUDENC_COMP_INIT_FNPTR    (OMX_TI_AUDENC_ComponentInit)
  #else                                 /* resides on the remote core */
   #define AUDENC_COMP_INIT_FNPTR    (DomxProxy_OMX_TI_AUDENC_ComponentInit)
 #endif
#endif

#endif /* _LOCAL_CORE_c6xdsp_ */

/* By default all the VFPC's sub-modules would be included in the library. 
   If app_cfg.h defines VFPC_BUILD_SUBSET, then only those sub-modules that are
   defined by it would be included */
#ifdef _BUILD_vfpc_             /* If VFPC is in the build */
 #ifdef VFPC_BUILD_SUBSET        /* If app_cfg.h defines it, define only those
                                   that are requested */
  #ifdef VFPC_BUILD_DEIHDUALOUT
   #define _BUILD_vfpc_deihdualout_
  #endif
  #ifdef VFPC_BUILD_DEIMDUALOUT
   #define _BUILD_vfpc_deimdualout_
  #endif
  #ifdef VFPC_BUILD_INDTXSCWB
   #define _BUILD_vfpc_indtxscwb_
  #endif
  #ifdef VFPC_BUILD_NF
   #define _BUILD_vfpc_nf_
  #endif
 #else /* If app_cfg.h doesn't define it, it means all sub-modules of VFPC has
         to be included in the build */
  #define _BUILD_vfpc_deihdualout_
  #define _BUILD_vfpc_deimdualout_
  #define _BUILD_vfpc_indtxscwb_
  #define _BUILD_vfpc_nf_
 #endif
#endif

/* Nothing beyond this point */
