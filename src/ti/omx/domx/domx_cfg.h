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
 *
 *          This file contains default values of all DOMX configuration
 *          parameters. They are defined only if they are NOT defined previously
 *          in the current scope. The defines are typically defined in app_cfg.h
 *          file that is used by the integrator to configure parameters when
 *          it is other than the default. 
 *
 *  @rev 1.0
 *******************************************************************************
 */

#include <OMX_Core.h>

/* Statically configurable parameters - re-define it in app_cfg.h if it need to
   be different than defined here */

/* Following Component core info table entries' default values are below */
/* Ctrl RCM Server priority */
/*NOTE!!!*/
/*
 *For Video surveillance cases wherein M3s are fully loaded with less than
 *5% idle cycles, with priority of 1, it was found that dynamic mosaic changes
 *does not happen in real time. For some of the windows, it happens quickly
 *whereas for others it happens slowly.
 *With this change, the issue got resolved.
 */
#ifdef _LOCAL_CORE_a8host_
  #define DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_GENERIC (0xE)
#else
#define DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_GENERIC (0x8)
#endif
/* Cb RCM Server priority */
/*NOTE!!!*/
/*
 *For Video surveillance cases wherein M3s are fully loaded with less than
 *5% idle cycles, with priority of 1, it was found that dynamic mosaic changes
 *does not happen in real time. For some of the windows, it happens quickly
 *whereas for others it happens slowly.
 *With this change, the issue got resolved.
 */
#ifdef _LOCAL_CORE_a8host_
  #define DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_GENERIC   (0xE)
#else
#define DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_GENERIC   (0x8)
#endif

/* Max Number of buffers */
#define DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_GENERIC   (64)


#ifdef _LOCAL_CORE_m3video_
 #ifndef DOMX_OMXRPC_MSGHEAPID_CORE0
  #define DOMX_OMXRPC_MSGHEAPID_CORE0              (0)
 #endif
 #ifndef DOMX_OMXRPC_MSGHEAPID_CORE1
  #define DOMX_OMXRPC_MSGHEAPID_CORE1              (-1)
 #endif
 #ifndef DOMX_OMXRPC_MSGHEAPID_CORE2
  #define DOMX_OMXRPC_MSGHEAPID_CORE2              (3)
 #endif
 #ifndef DOMX_OMXRPC_MSGHEAPID_CORE3
  #define DOMX_OMXRPC_MSGHEAPID_CORE3              (4)
 #endif
#endif

#ifdef _LOCAL_CORE_m3vpss_
 #ifndef DOMX_OMXRPC_MSGHEAPID_CORE0
  #define DOMX_OMXRPC_MSGHEAPID_CORE0              (1)
 #endif
 #ifndef DOMX_OMXRPC_MSGHEAPID_CORE1
  #define DOMX_OMXRPC_MSGHEAPID_CORE1              (3)
 #endif
 #ifndef DOMX_OMXRPC_MSGHEAPID_CORE2
  #define DOMX_OMXRPC_MSGHEAPID_CORE2              (-1)
 #endif
 #ifndef DOMX_OMXRPC_MSGHEAPID_CORE3
  #define DOMX_OMXRPC_MSGHEAPID_CORE3              (5)
 #endif
#endif

#ifdef _LOCAL_CORE_a8host_
 #ifndef DOMX_OMXRPC_MSGHEAPID_CORE0
  #define DOMX_OMXRPC_MSGHEAPID_CORE0              (2)
 #endif
 #ifndef DOMX_OMXRPC_MSGHEAPID_CORE1
  #define DOMX_OMXRPC_MSGHEAPID_CORE1              (4)
 #endif
 #ifndef DOMX_OMXRPC_MSGHEAPID_CORE2
  #define DOMX_OMXRPC_MSGHEAPID_CORE2              (5)
 #endif
 #ifndef DOMX_OMXRPC_MSGHEAPID_CORE3
  #define DOMX_OMXRPC_MSGHEAPID_CORE3              (-1)
 #endif
#endif

#ifdef _LOCAL_CORE_c6xdsp_
  #ifndef DOMX_OMXRPC_MSGHEAPID_CORE0
    #define DOMX_OMXRPC_MSGHEAPID_CORE0            (-1)
  #endif
  #ifndef DOMX_OMXRPC_MSGHEAPID_CORE1
    #define DOMX_OMXRPC_MSGHEAPID_CORE1            (0)
  #endif
  #ifndef DOMX_OMXRPC_MSGHEAPID_CORE2
    #define DOMX_OMXRPC_MSGHEAPID_CORE2            (1)
  #endif
  #ifndef DOMX_OMXRPC_MSGHEAPID_CORE3
    #define DOMX_OMXRPC_MSGHEAPID_CORE3            (2)
  #endif
#endif


/* Number of coreInfoTbl entries */
#ifndef DOMX_CORE_NUMCOREINFOTBLENTRIES
  #define DOMX_CORE_NUMCOREINFOTBLENTRIES          (19)
#endif

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
#define DOMX_CORE_vswmosaic_m3vpss   /* Signifies that vswmosaic runs on vpss-M3 core */

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
#define DOMX_CORE_audenc_c6xdsp   /* Signifies that aenc runs on DSP core */

/* Component 18 */
#define DOMX_CORE_vsplc_m3vpss   /* Signifies that vsplc runs on vpss-M3 core */

/* IPC is enabled on  A8 , and media controller cores */
#ifdef _LOCAL_CORE_m3video_
  #ifndef DOMX_CORE_IPCINFO_CORE0_ENABLE_SWITCH
    #define DOMX_CORE_IPCINFO_CORE0_ENABLE_SWITCH   (0)  /*DSP*/
  #endif
  #ifdef DOMX_CORE_IPCINFO_CORE1_ENABLE_SWITCH
    #undef DOMX_CORE_IPCINFO_CORE1_ENABLE_SWITCH
    #define DOMX_CORE_IPCINFO_CORE1_ENABLE_SWITCH   (0)  /*Vid-M3 (Self)*/
  #endif
  #ifndef DOMX_CORE_IPCINFO_CORE2_ENABLE_SWITCH
    #define DOMX_CORE_IPCINFO_CORE2_ENABLE_SWITCH   (0)  /*VPSS-M3*/
  #endif
  #ifndef DOMX_CORE_IPCINFO_CORE3_ENABLE_SWITCH
    #define DOMX_CORE_IPCINFO_CORE3_ENABLE_SWITCH   (1)   /*A8*/
  #endif
#endif

#ifdef _LOCAL_CORE_m3vpss_
  #ifndef DOMX_CORE_IPCINFO_CORE0_ENABLE_SWITCH
    #define DOMX_CORE_IPCINFO_CORE0_ENABLE_SWITCH   (0)  /*DSP*/
  #endif
  #ifndef DOMX_CORE_IPCINFO_CORE1_ENABLE_SWITCH
    #define DOMX_CORE_IPCINFO_CORE1_ENABLE_SWITCH   (0)  /*Vid-M3 */
  #endif
  #ifdef DOMX_CORE_IPCINFO_CORE2_ENABLE_SWITCH
    #undef DOMX_CORE_IPCINFO_CORE2_ENABLE_SWITCH
    #define DOMX_CORE_IPCINFO_CORE2_ENABLE_SWITCH   (0)  /*VPSS-M3 (Self)*/
  #endif
  #ifndef DOMX_CORE_IPCINFO_CORE3_ENABLE_SWITCH
    #define DOMX_CORE_IPCINFO_CORE3_ENABLE_SWITCH   (1)   /*A8*/
  #endif
#endif

#ifdef _LOCAL_CORE_a8host_
  #ifndef DOMX_CORE_IPCINFO_CORE0_ENABLE_SWITCH
    #define DOMX_CORE_IPCINFO_CORE0_ENABLE_SWITCH   (1)  /*DSP*/
  #endif
  #ifndef DOMX_CORE_IPCINFO_CORE1_ENABLE_SWITCH
    #define DOMX_CORE_IPCINFO_CORE1_ENABLE_SWITCH   (1)  /*Vid-M3 */
  #endif
  #ifndef DOMX_CORE_IPCINFO_CORE2_ENABLE_SWITCH
    #define DOMX_CORE_IPCINFO_CORE2_ENABLE_SWITCH   (1)  /*VPSS-M3 */
  #endif
  #ifdef DOMX_CORE_IPCINFO_CORE3_ENABLE_SWITCH
    #undef DOMX_CORE_IPCINFO_CORE3_ENABLE_SWITCH
    #define DOMX_CORE_IPCINFO_CORE3_ENABLE_SWITCH   (0)   /*A8 (self)*/
  #endif
#endif

#ifdef _LOCAL_CORE_c6xdsp_
  #ifdef DOMX_CORE_IPCINFO_CORE0_ENABLE_SWITCH
    #undef DOMX_CORE_IPCINFO_CORE0_ENABLE_SWITCH
    #define DOMX_CORE_IPCINFO_CORE0_ENABLE_SWITCH   (0)  /*DSP (Self)*/
  #endif
  #ifndef DOMX_CORE_IPCINFO_CORE1_ENABLE_SWITCH
    #define DOMX_CORE_IPCINFO_CORE1_ENABLE_SWITCH   (0)  /*Vid-M3 */
  #endif
  #ifndef DOMX_CORE_IPCINFO_CORE2_ENABLE_SWITCH
    #define DOMX_CORE_IPCINFO_CORE2_ENABLE_SWITCH   (0)  /*VPSS-M3 */
  #endif
  #ifndef DOMX_CORE_IPCINFO_CORE3_ENABLE_SWITCH
    #define DOMX_CORE_IPCINFO_CORE3_ENABLE_SWITCH   (1)   /*A8 */
  #endif
#endif

/* If proc_load (firmaware load) is done by A8 IL client, following names
   should be provided for firmaware media controller binaries */
#ifndef DOMX_CORE_SLAVECORE_M3VIDEO_EXEC_NAME
  #define DOMX_CORE_SLAVECORE_M3VIDEO_EXEC_NAME   "dm81xxbm_m3video.xem3"
#endif
#ifndef DOMX_CORE_SLAVECORE_M3VPSS_EXEC_NAME
  #define DOMX_CORE_SLAVECORE_M3VPSS_EXEC_NAME    "dm81xxbm_m3vpss.xem3"
#endif
#ifndef DOMX_CORE_SLAVECORE_C6XDSP_EXEC_NAME
  #define DOMX_CORE_SLAVECORE_C6XDSP_EXEC_NAME    "dm81xxbm_c6xdsp.xec674"
#endif

/* if A8 IL client wants to do firmware loading enable following flag */
#ifdef _LOCAL_CORE_a8host_
  #ifndef DOMX_CORE_DOPROCINIT
    #define DOMX_CORE_DOPROCINIT                     (0)
  #endif
#endif

#ifndef DOMX_OMXRPC_MAXCONFIGSTRUCTSIZE
  #define DOMX_OMXRPC_MAXCONFIGSTRUCTSIZE          (0x400)
#endif


/*
 * The following defines with format DOMX_CORE_<component-id>_<core-name>
 * have to be understood this way: These are defined by the application
 * configuration file (app_cfg.h) and it means the component identified
 * by component-id should run on the core-name. For example, when the
 * app defines DOMX_CORE_venc_m3video, the integrator tells DOMX to 
 * configure "venc" (Video Encoder OMX component) to run on the core
 * "m3video"
 *
 */
/* Entry 0 */
#ifndef DOMX_CORE_COMPINFOTBL_COREID_venc
  #ifdef DOMX_CORE_venc_m3video
    #define DOMX_CORE_COMPINFOTBL_COREID_venc (DomxTypes_coreCortexVideoM3)
  #endif
  #ifdef DOMX_CORE_venc_m3vpss
    #define DOMX_CORE_COMPINFOTBL_COREID_venc (DomxTypes_coreCortexHdvpssM3)
  #endif
  #ifdef DOMX_CORE_venc_a8host
    #define DOMX_CORE_COMPINFOTBL_COREID_venc (DomxTypes_coreCortexA8)
  #endif
#endif

#ifndef DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_venc
  #define DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_venc    (DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_venc
  #define DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_venc      (DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_venc
  #define DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_venc      (8 * DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_GENERIC)
#endif

/* Entry 1 */
#ifndef DOMX_CORE_COMPINFOTBL_COREID_vdec
  #ifdef DOMX_CORE_vdec_m3video
    #define DOMX_CORE_COMPINFOTBL_COREID_vdec (DomxTypes_coreCortexVideoM3)
  #endif
  #ifdef DOMX_CORE_vdec_m3vpss
    #define DOMX_CORE_COMPINFOTBL_COREID_vdec (DomxTypes_coreCortexHdvpssM3)
  #endif
  #ifdef DOMX_CORE_vdec_a8host
    #define DOMX_CORE_COMPINFOTBL_COREID_vdec (DomxTypes_coreCortexA8)
  #endif
#endif

#ifndef DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_vdec
  #define DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_vdec    (DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_vdec
  #define DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_vdec      (DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_vdec
  #define DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_vdec      (DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_GENERIC)
#endif

/* Entry 2 */
#ifndef DOMX_CORE_COMPINFOTBL_COREID_vsplc
 #ifdef DOMX_CORE_vsplc_m3video
  #define DOMX_CORE_COMPINFOTBL_COREID_vsplc (DomxTypes_coreCortexVideoM3)
 #endif
 #ifdef DOMX_CORE_vsplc_m3vpss
  #define DOMX_CORE_COMPINFOTBL_COREID_vsplc (DomxTypes_coreCortexHdvpssM3)
 #endif
 #ifdef DOMX_CORE_vsplc_a8host
  #define DOMX_CORE_COMPINFOTBL_COREID_vsplc (DomxTypes_coreCortexA8)
 #endif
 #ifdef DOMX_CORE_vsplc_c6xdsp
  #define DOMX_CORE_COMPINFOTBL_COREID_vsplc (DomxTypes_coreDsp)
 #endif
#endif

#ifndef DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_vsplc
#define DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_vsplc    (DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_vsplc
#define DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_vsplc      (DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_vsplc
#define DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_vsplc      (DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_GENERIC)
#endif


#ifndef DOMX_CORE_COMPINFOTBL_COREID_vfcc
  #ifdef DOMX_CORE_vfcc_m3video
    #define DOMX_CORE_COMPINFOTBL_COREID_vfcc (DomxTypes_coreCortexVideoM3)
  #endif
  #ifdef DOMX_CORE_vfcc_m3vpss
    #define DOMX_CORE_COMPINFOTBL_COREID_vfcc (DomxTypes_coreCortexHdvpssM3)
  #endif
  #ifdef DOMX_CORE_vfcc_a8host
    #define DOMX_CORE_COMPINFOTBL_COREID_vfcc (DomxTypes_coreCortexA8)
  #endif
#endif

#ifndef DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_vfcc
  #define DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_vfcc    (DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_vfcc
  #define DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_vfcc      (DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_vfcc
  #define DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_vfcc      (DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_GENERIC)
#endif

/* Entry 3 */
#ifndef DOMX_CORE_COMPINFOTBL_COREID_vfdc
  #ifdef DOMX_CORE_vfdc_m3video
    #define DOMX_CORE_COMPINFOTBL_COREID_vfdc (DomxTypes_coreCortexVideoM3)
  #endif
  #ifdef DOMX_CORE_vfdc_m3vpss
    #define DOMX_CORE_COMPINFOTBL_COREID_vfdc (DomxTypes_coreCortexHdvpssM3)
  #endif
  #ifdef DOMX_CORE_vfdc_a8host
    #define DOMX_CORE_COMPINFOTBL_COREID_vfdc (DomxTypes_coreCortexA8)
  #endif
#endif

#ifndef DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_vfdc
  #define DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_vfdc    (DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_vfdc
  #define DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_vfdc      (DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_vfdc
  #define DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_vfdc      (2 * DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_GENERIC)
#endif

/* Entry 4 */
#ifndef DOMX_CORE_COMPINFOTBL_COREID_vfpc_deih
  #ifdef DOMX_CORE_vfpc_deihdualout_m3video
    #define DOMX_CORE_COMPINFOTBL_COREID_vfpc_deih (DomxTypes_coreCortexVideoM3)
  #endif
  #ifdef DOMX_CORE_vfpc_deihdualout_m3vpss
    #define DOMX_CORE_COMPINFOTBL_COREID_vfpc_deih (DomxTypes_coreCortexHdvpssM3)
  #endif
  #ifdef DOMX_CORE_vfpc_deihdualout_a8host
    #define DOMX_CORE_COMPINFOTBL_COREID_vfpc_deih (DomxTypes_coreCortexA8)
  #endif
#endif

#ifndef DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_vfpc_deih
  #define DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_vfpc_deih    (DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_vfpc_deih
  #define DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_vfpc_deih      (DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_vfpc_deih
  #define DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_vfpc_deih      (DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_GENERIC)
#endif

/* Entry 5 */
#ifndef DOMX_CORE_COMPINFOTBL_COREID_vfpc_deim
  #ifdef DOMX_CORE_vfpc_deimdualout_m3video
    #define DOMX_CORE_COMPINFOTBL_COREID_vfpc_deim (DomxTypes_coreCortexVideoM3)
  #endif
  #ifdef DOMX_CORE_vfpc_deimdualout_m3vpss
    #define DOMX_CORE_COMPINFOTBL_COREID_vfpc_deim (DomxTypes_coreCortexHdvpssM3)
  #endif
  #ifdef DOMX_CORE_vfpc_deimdualout_a8host
    #define DOMX_CORE_COMPINFOTBL_COREID_vfpc_deim (DomxTypes_coreCortexA8)
  #endif
#endif

#ifndef DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_vfpc_deim
  #define DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_vfpc_deim    (DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_vfpc_deim
  #define DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_vfpc_deim      (DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_vfpc_deim
  #define DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_vfpc_deim      (8 * DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_GENERIC)
#endif

/* Entry 6 */
#ifndef DOMX_CORE_COMPINFOTBL_COREID_vfpc_nf
  #ifdef DOMX_CORE_vfpc_nf_m3video
    #define DOMX_CORE_COMPINFOTBL_COREID_vfpc_nf (DomxTypes_coreCortexVideoM3)
  #endif
  #ifdef DOMX_CORE_vfpc_nf_m3vpss
    #define DOMX_CORE_COMPINFOTBL_COREID_vfpc_nf (DomxTypes_coreCortexHdvpssM3)
  #endif
  #ifdef DOMX_CORE_vfpc_nf_a8host
    #define DOMX_CORE_COMPINFOTBL_COREID_vfpc_nf (DomxTypes_coreCortexA8)
  #endif
#endif

#ifndef DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_vfpc_nf
  #define DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_vfpc_nf    (DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_vfpc_nf
  #define DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_vfpc_nf      (DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_vfpc_nf
  #define DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_vfpc_nf      (DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_GENERIC)
#endif

/* Entry 7 */
#ifndef DOMX_CORE_COMPINFOTBL_COREID_vfpc_indtxscwb
  #ifdef DOMX_CORE_vfpc_indtxscwb_m3video
    #define DOMX_CORE_COMPINFOTBL_COREID_vfpc_indtxscwb (DomxTypes_coreCortexVideoM3)
  #endif
  #ifdef DOMX_CORE_vfpc_indtxscwb_m3vpss
    #define DOMX_CORE_COMPINFOTBL_COREID_vfpc_indtxscwb (DomxTypes_coreCortexHdvpssM3)
  #endif
  #ifdef DOMX_CORE_vfpc_indtxscwb_a8host
    #define DOMX_CORE_COMPINFOTBL_COREID_vfpc_indtxscwb (DomxTypes_coreCortexA8)
  #endif
#endif

#ifndef DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_vfpc_indtxscwb
  #define DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_vfpc_indtxscwb    (DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_vfpc_indtxscwb
  #define DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_vfpc_indtxscwb      (DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_vfpc_indtxscwb
  #define DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_vfpc_indtxscwb      (2*DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_GENERIC)
#endif

/* Entry 8 */
#ifndef DOMX_CORE_COMPINFOTBL_COREID_vsrc
  #ifdef DOMX_CORE_vsrc_m3video
    #define DOMX_CORE_COMPINFOTBL_COREID_vsrc (DomxTypes_coreCortexVideoM3)
  #endif
  #ifdef DOMX_CORE_vsrc_m3vpss
    #define DOMX_CORE_COMPINFOTBL_COREID_vsrc (DomxTypes_coreCortexHdvpssM3)
  #endif
  #ifdef DOMX_CORE_vsrc_a8host
    #define DOMX_CORE_COMPINFOTBL_COREID_vsrc (DomxTypes_coreCortexA8)
  #endif
  #ifdef DOMX_CORE_vsrc_c6xdsp
    #define DOMX_CORE_COMPINFOTBL_COREID_vsrc (DomxTypes_coreDsp)
  #endif
#endif

#ifndef DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_vsrc
  #define DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_vsrc    (DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_vsrc
  #define DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_vsrc      (DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_vsrc
  #define DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_vsrc      (DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_GENERIC)
#endif

/* Entry 9 */
#ifndef DOMX_CORE_COMPINFOTBL_COREID_vsnk
  #ifdef DOMX_CORE_vsnk_m3video
    #define DOMX_CORE_COMPINFOTBL_COREID_vsnk (DomxTypes_coreCortexVideoM3)
  #endif
  #ifdef DOMX_CORE_vsnk_m3vpss
    #define DOMX_CORE_COMPINFOTBL_COREID_vsnk (DomxTypes_coreCortexHdvpssM3)
  #endif
  #ifdef DOMX_CORE_vsnk_a8host
    #define DOMX_CORE_COMPINFOTBL_COREID_vsnk (DomxTypes_coreCortexA8)
  #endif
  #ifdef DOMX_CORE_vsnk_c6xdsp
    #define DOMX_CORE_COMPINFOTBL_COREID_vsnk (DomxTypes_coreDsp)
  #endif
#endif

#ifndef DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_vsnk
  #define DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_vsnk    (DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_vsnk
  #define DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_vsnk      (DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_vsnk
  #define DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_vsnk      (DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_GENERIC)
#endif

/* Entry 10 */
#ifndef DOMX_CORE_COMPINFOTBL_COREID_ctrl
  #ifdef DOMX_CORE_ctrl_m3video
    #define DOMX_CORE_COMPINFOTBL_COREID_ctrl (DomxTypes_coreCortexVideoM3)
  #endif
  #ifdef DOMX_CORE_ctrl_m3vpss
    #define DOMX_CORE_COMPINFOTBL_COREID_ctrl (DomxTypes_coreCortexHdvpssM3)
  #endif
  #ifdef DOMX_CORE_ctrl_a8host
    #define DOMX_CORE_COMPINFOTBL_COREID_ctrl (DomxTypes_coreCortexA8)
  #endif
#endif

#ifndef DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_ctrl
  #define DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_ctrl    (DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_ctrl
  #define DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_ctrl      (DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_ctrl
  #define DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_ctrl      (DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_GENERIC)
#endif

/* Entry 11 */
#ifndef DOMX_CORE_COMPINFOTBL_COREID_ctrldc
  #ifdef DOMX_CORE_ctrldc_m3video
    #define DOMX_CORE_COMPINFOTBL_COREID_ctrldc (DomxTypes_coreCortexVideoM3)
  #endif
  #ifdef DOMX_CORE_ctrldc_m3vpss
    #define DOMX_CORE_COMPINFOTBL_COREID_ctrldc (DomxTypes_coreCortexHdvpssM3)
  #endif
  #ifdef DOMX_CORE_ctrldc_a8host
    #define DOMX_CORE_COMPINFOTBL_COREID_ctrldc (DomxTypes_coreCortexA8)
  #endif
#endif

#ifndef DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_ctrldc
  #define DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_ctrldc    (DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_ctrldc
  #define DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_ctrldc      (DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_ctrldc
  #define DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_ctrldc      (DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_GENERIC)
#endif

/* Entry 12 */
#ifndef DOMX_CORE_COMPINFOTBL_COREID_cmux
  #ifdef DOMX_CORE_cmux_m3video
    #define DOMX_CORE_COMPINFOTBL_COREID_cmux (DomxTypes_coreCortexVideoM3)
  #endif
  #ifdef DOMX_CORE_cmux_m3vpss
    #define DOMX_CORE_COMPINFOTBL_COREID_cmux (DomxTypes_coreCortexHdvpssM3)
  #endif
  #ifdef DOMX_CORE_cmux_a8host
    #define DOMX_CORE_COMPINFOTBL_COREID_cmux (DomxTypes_coreCortexA8)
  #endif
#endif

#ifndef DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_cmux
  #define DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_cmux    (DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_cmux
  #define DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_cmux      (DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_cmux
  #define DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_cmux      (DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_GENERIC)
#endif

/* Entry 13 */
#ifndef DOMX_CORE_COMPINFOTBL_COREID_dmux
  #ifdef DOMX_CORE_dmux_m3video
    #define DOMX_CORE_COMPINFOTBL_COREID_dmux (DomxTypes_coreCortexVideoM3)
  #endif
  #ifdef DOMX_CORE_dmux_m3vpss
    #define DOMX_CORE_COMPINFOTBL_COREID_dmux (DomxTypes_coreCortexHdvpssM3)
  #endif
  #ifdef DOMX_CORE_dmux_a8host
    #define DOMX_CORE_COMPINFOTBL_COREID_dmux (DomxTypes_coreCortexA8)
  #endif
#endif

#ifndef DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_dmux
  #define DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_dmux    (DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_dmux
  #define DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_dmux      (DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_dmux
  #define DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_dmux      (DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_GENERIC)
#endif

/* Entry 14 */
#ifndef DOMX_CORE_COMPINFOTBL_COREID_rtptx
  #ifdef DOMX_CORE_rtptx_m3video
    #define DOMX_CORE_COMPINFOTBL_COREID_rtptx (DomxTypes_coreCortexVideoM3)
  #endif
  #ifdef DOMX_CORE_rtptx_m3vpss
    #define DOMX_CORE_COMPINFOTBL_COREID_rtptx (DomxTypes_coreCortexHdvpssM3)
  #endif
  #ifdef DOMX_CORE_rtptx_a8host
    #define DOMX_CORE_COMPINFOTBL_COREID_rtptx (DomxTypes_coreCortexA8)
  #endif
#endif

#ifndef DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_rtptx
  #define DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_rtptx   (DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_rtptx
  #define DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_rtptx      (DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_rtptx
  #define DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_rtptx      (DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_GENERIC)
#endif

/* Entry 15 */
#ifndef DOMX_CORE_COMPINFOTBL_COREID_rtprx
  #ifdef DOMX_CORE_rtprx_m3video
    #define DOMX_CORE_COMPINFOTBL_COREID_rtprx (DomxTypes_coreCortexVideoM3)
  #endif
  #ifdef DOMX_CORE_rtprx_m3vpss
    #define DOMX_CORE_COMPINFOTBL_COREID_rtprx (DomxTypes_coreCortexHdvpssM3)
  #endif
  #ifdef DOMX_CORE_rtprx_a8host
    #define DOMX_CORE_COMPINFOTBL_COREID_rtprx (DomxTypes_coreCortexA8)
  #endif
#endif

#ifndef DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_rtprx
  #define DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_rtprx    (DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_rtprx
  #define DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_rtprx      (DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_rtprx
  #define DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_rtprx      (DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_GENERIC)
#endif

/* Entry 16 */
#ifdef DOMX_VLPB_CONFIG_NEEDED /* Right now, VLPB can run only on A8 and/or DSP */
 #ifndef DOMX_CORE_COMPINFOTBL_COREID_vlpb
  #ifdef DOMX_CORE_vlpb_m3video
    #define DOMX_CORE_COMPINFOTBL_COREID_vlpb (DomxTypes_coreCortexVideoM3)
  #endif
  #ifdef DOMX_CORE_vlpb_m3vpss
    #define DOMX_CORE_COMPINFOTBL_COREID_vlpb (DomxTypes_coreCortexHdvpssM3)
  #endif
  #ifdef DOMX_CORE_vlpb_a8host
    #define DOMX_CORE_COMPINFOTBL_COREID_vlpb (DomxTypes_coreCortexA8)
  #endif
  #ifdef DOMX_CORE_vlpb_c6xdsp
    #define DOMX_CORE_COMPINFOTBL_COREID_vlpb (DomxTypes_coreDsp)
  #endif
 #endif
#endif /* DOMX_VLPB_CONFIG_NEEDED */

#ifndef DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_vlpb
  #define DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_vlpb    (DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_vlpb
  #define DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_vlpb      (DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_vlpb
  #define DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_vlpb      (DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_GENERIC)
#endif


/* Entry 17 */
#ifndef DOMX_CORE_COMPINFOTBL_COREID_auddec
 #ifdef DOMX_CORE_auddec_m3video
  #define DOMX_CORE_COMPINFOTBL_COREID_auddec (DomxTypes_coreCortexVideoM3)
 #endif
 #ifdef DOMX_CORE_auddec_m3vpss
  #define DOMX_CORE_COMPINFOTBL_COREID_auddec (DomxTypes_coreCortexHdvpssM3)
 #endif
 #ifdef DOMX_CORE_auddec_a8host
  #define DOMX_CORE_COMPINFOTBL_COREID_auddec (DomxTypes_coreCortexA8)
 #endif
 #ifdef DOMX_CORE_auddec_c6xdsp
  #define DOMX_CORE_COMPINFOTBL_COREID_auddec (DomxTypes_coreDsp)
 #endif
#endif

#ifndef DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_auddec
#define DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_auddec    (DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_auddec
#define DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_auddec      (DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_auddec
#define DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_auddec      (DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_GENERIC)
#endif

/* Entry 18 */
#ifndef DOMX_CORE_COMPINFOTBL_COREID_audenc
 #ifdef DOMX_CORE_audenc_m3video
  #define DOMX_CORE_COMPINFOTBL_COREID_audenc (DomxTypes_coreCortexVideoM3)
 #endif
 #ifdef DOMX_CORE_audenc_m3vpss
  #define DOMX_CORE_COMPINFOTBL_COREID_audenc (DomxTypes_coreCortexHdvpssM3)
 #endif
 #ifdef DOMX_CORE_audenc_a8host
  #define DOMX_CORE_COMPINFOTBL_COREID_audenc (DomxTypes_coreCortexA8)
 #endif
 #ifdef DOMX_CORE_audenc_c6xdsp
  #define DOMX_CORE_COMPINFOTBL_COREID_audenc (DomxTypes_coreDsp)
 #endif
#endif

#ifndef DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_audenc
#define DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_audenc    (DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_audenc
#define DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_audenc      (DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_audenc
#define DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_audenc      (DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_GENERIC)
#endif

/* Entry 18 */
#ifndef DOMX_CORE_COMPINFOTBL_COREID_vswmosaic
  #ifdef DOMX_CORE_vswmosaic_m3video
    #define DOMX_CORE_COMPINFOTBL_COREID_vswmosaic (DomxTypes_coreCortexVideoM3)
  #endif
  #ifdef DOMX_CORE_vswmosaic_m3vpss
    #define DOMX_CORE_COMPINFOTBL_COREID_vswmosaic (DomxTypes_coreCortexHdvpssM3)
  #endif
  #ifdef DOMX_CORE_vswmosaic_a8host
    #define DOMX_CORE_COMPINFOTBL_COREID_vswmosaic (DomxTypes_coreCortexA8)
  #endif
  #ifdef DOMX_CORE_vswmosaic_c6xdsp
    #define DOMX_CORE_COMPINFOTBL_COREID_vswmosaic (DomxTypes_coreDsp)
  #endif
#endif

#ifndef DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_vswmosaic
  #define DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_vswmosaic    (DOMX_CORE_COMPINFOTBL_CTRLRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_vswmosaic
  #define DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_vswmosaic      (DOMX_CORE_COMPINFOTBL_CBRCMSVRPRI_GENERIC)
#endif

#ifndef DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_vswmosaic
  #define DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_vswmosaic      (DOMX_CORE_COMPINFOTBL_MAXNUMBUFFS_GENERIC)
#endif

/* DomxCore IPC Info table. By default, it is disabled for all cores */
/* Core 0 */
#ifndef DOMX_CORE_IPCINFO_CORE0_ENABLE_SWITCH
  #define DOMX_CORE_IPCINFO_CORE0_ENABLE_SWITCH    (0)
#endif

/* Core 1 */
#ifndef DOMX_CORE_IPCINFO_CORE1_ENABLE_SWITCH
  #define DOMX_CORE_IPCINFO_CORE1_ENABLE_SWITCH    (0)
#endif

/* Core 2 */
#ifndef DOMX_CORE_IPCINFO_CORE2_ENABLE_SWITCH
  #define DOMX_CORE_IPCINFO_CORE2_ENABLE_SWITCH    (0)
#endif

/* Core 3 */
#ifndef DOMX_CORE_IPCINFO_CORE3_ENABLE_SWITCH
  #define DOMX_CORE_IPCINFO_CORE3_ENABLE_SWITCH    (0)
#endif

#ifndef DOMX_CORE_DOPROCINIT
  #define DOMX_CORE_DOPROCINIT                     (0)
#endif

#ifndef DOMX_CORE_SLAVECORE_M3VIDEO_EXEC_NAME
  #define DOMX_CORE_SLAVECORE_M3VIDEO_EXEC_NAME   "null.xem3"
#endif

#ifndef DOMX_CORE_SLAVECORE_M3VPSS_EXEC_NAME
  #define DOMX_CORE_SLAVECORE_M3VPSS_EXEC_NAME    "null.xem3"
#endif

#ifndef DOMX_CORE_SLAVECORE_C6XDSP_EXEC_NAME
  #define DOMX_CORE_SLAVECORE_C6XDSP_EXEC_NAME    "null.xec674"
#endif

/*
 * DOMX TunnelMgr
 */
#ifndef DOMX_TUNNELMGR_MAXNUMCOMPS
  #define DOMX_TUNNELMGR_MAXNUMCOMPS               (0x80)
#endif

#ifndef DOMX_TUNNELMGR_MODSTATE_INITDONE
  #define DOMX_TUNNELMGR_MODSTATE_INITDONE         (0)
#endif

#ifndef DOMX_TUNNELMGR_MODSTATE_NUMACTIVECOMPS
  #define DOMX_TUNNELMGR_MODSTATE_NUMACTIVECOMPS   (0)
#endif

/* 
 * DOMX omxRpc
 */

#ifndef DOMX_OMXRPC_CREATEMSGHEAPS
  #define DOMX_OMXRPC_CREATEMSGHEAPS               (1)
#endif

#ifndef DOMX_OMXRPC_RCMSVRNAMEPREFIX
  #define DOMX_OMXRPC_RCMSVRNAMEPREFIX             "OmxRpcRcmServer"
#endif

#ifndef DOMX_OMXRPC_MAXQUEUEDBUFLEN
  #define DOMX_OMXRPC_MAXQUEUEDBUFLEN              (0x40)
#endif

/*NOTE!!!*/
/*
 *For Video surveillance cases wherein M3s are fully loaded with less than
 *5% idle cycles, with priority of 1, it was found that dynamic mosaic changes
 *does not happen in real time. For some of the windows, it happens quickly
 *whereas for others it happens slowly.
 *With this change, the issue got resolved.
 */
#ifndef DOMX_OMXRPC_RCMCORESVRPRI
  #ifdef _LOCAL_CORE_a8host_
    #define DOMX_OMXRPC_RCMCORESVRPRI                (0xE)
  #else
  #define DOMX_OMXRPC_RCMCORESVRPRI                (0x8)
#endif
#endif

/* OmxProxy */
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

/* If DOMX_OMXPROXY_<NAME> is defined, it means the component by <NAME> runs in
   a remote core w.r.t. the current core for which build iteration is in 
   progress */
#ifdef _LOCAL_CORE_m3video_     /* Current build iteration is for m3video */
  #ifdef _BUILD_vfcc_
    #ifndef DOMX_CORE_vfcc_m3video
      #define DOMX_OMXPROXY_VFCC
    #endif
  #endif

  #ifdef _BUILD_vfdc_
    #ifndef DOMX_CORE_vfdc_m3video
      #define DOMX_OMXPROXY_VFDC
  #endif
 #endif
 
 #ifdef _BUILD_vsplc_
  #ifndef DOMX_CORE_vsplc_m3video
   #define DOMX_OMXPROXY_VSPLC
    #endif
  #endif

  #ifdef _BUILD_vfpc_deihdualout_
    #ifndef DOMX_CORE_vfpc_deihdualout_m3video
      #define DOMX_OMXPROXY_VFPC_DEIH
    #endif
  #endif

  #ifdef _BUILD_vfpc_deimdualout_
    #ifndef DOMX_CORE_vfpc_deimdualout_m3video
      #define DOMX_OMXPROXY_VFPC_DEIM
    #endif
  #endif

  #ifdef _BUILD_vfpc_nf_
    #ifndef DOMX_CORE_vfpc_nf_m3video
      #define DOMX_OMXPROXY_VFPC_NF
    #endif
  #endif

  #ifdef _BUILD_vfpc_indtxscwb_
    #ifndef DOMX_CORE_vfpc_indtxscwb_m3video
      #define DOMX_OMXPROXY_VFPC_INDTXSCWB
    #endif
  #endif

  #ifdef _BUILD_vsrc_
    #ifndef DOMX_CORE_vsrc_m3video
      #define DOMX_OMXPROXY_VSRC
    #endif
  #endif

  #ifdef _BUILD_vsnk_
    #ifndef DOMX_CORE_vsnk_m3video
      #define DOMX_OMXPROXY_VSNK
    #endif
  #endif

  #ifdef _BUILD_ctrl_
    #ifndef DOMX_CORE_ctrl_m3video
      #define DOMX_OMXPROXY_CTRL
    #endif
  #endif

  #ifdef _BUILD_ctrl_
    #ifndef DOMX_CORE_ctrldc_m3video
      #define DOMX_OMXPROXY_CTRLDC
    #endif
  #endif

  #ifdef _BUILD_venc_
    #ifndef DOMX_CORE_venc_m3video
      #define DOMX_OMXPROXY_VENC
    #endif
  #endif

  #ifdef _BUILD_vdec_
    #ifndef DOMX_CORE_vdec_m3video
      #define DOMX_OMXPROXY_VDEC
    #endif
  #endif

  #ifdef _BUILD_cmux_
    #ifndef DOMX_CORE_cmux_m3video
      #define DOMX_OMXPROXY_CMUX
    #endif
  #endif

  #ifdef _BUILD_dmux_
    #ifndef DOMX_CORE_dmux_m3video
      #define DOMX_OMXPROXY_DMUX
    #endif
  #endif

  #ifdef _BUILD_rtptx_
    #ifndef DOMX_CORE_rtptx_m3video
      #define DOMX_OMXPROXY_RTPTX
    #endif
  #endif
  
  #ifdef _BUILD_rtprx_
    #ifndef DOMX_CORE_rtprx_m3video
      #define DOMX_OMXPROXY_RTPRX
    #endif
  #endif

  #ifdef _BUILD_vlpb_
    #ifndef DOMX_CORE_vlpb_m3video
      #define DOMX_OMXPROXY_VLPB
    #endif
  #endif

  #ifdef _BUILD_adec_
    #ifndef DOMX_CORE_auddec_m3video
      #define DOMX_OMXPROXY_AUDDEC
    #endif
  #endif

  #ifdef _BUILD_aenc_
    #ifndef DOMX_CORE_audenc_m3video
      #define DOMX_OMXPROXY_AUDENC
    #endif
  #endif

  #ifdef _BUILD_vswmosaic_
    #ifndef DOMX_CORE_vswmosaic_m3video
      #define DOMX_OMXPROXY_VSWMOSAIC
    #endif
  #endif

#endif /* _LOCAL_CORE_m3video_ */

#ifdef _LOCAL_CORE_m3vpss_      /* Current build iteration is for m3vpss */

  #ifdef _BUILD_vfcc_
    #ifndef DOMX_CORE_vfcc_m3vpss
      #define DOMX_OMXPROXY_VFCC
    #endif
  #endif

  #ifdef _BUILD_vfdc_
    #ifndef DOMX_CORE_vfdc_m3vpss
      #define DOMX_OMXPROXY_VFDC
  #endif
 #endif
 
 #ifdef _BUILD_vsplc_
  #ifndef DOMX_CORE_vsplc_m3vpss
   #define DOMX_OMXPROXY_VSPLC
    #endif
  #endif

  #ifdef _BUILD_vfpc_deihdualout_
    #ifndef DOMX_CORE_vfpc_deihdualout_m3vpss
      #define DOMX_OMXPROXY_VFPC_DEIH
    #endif
  #endif

  #ifdef _BUILD_vfpc_deimdualout_
    #ifndef DOMX_CORE_vfpc_deimdualout_m3vpss
      #define DOMX_OMXPROXY_VFPC_DEIM
    #endif
  #endif

  #ifdef _BUILD_vfpc_nf_
    #ifndef DOMX_CORE_vfpc_nf_m3vpss
      #define DOMX_OMXPROXY_VFPC_NF
    #endif
  #endif

  #ifdef _BUILD_vfpc_indtxscwb_
    #ifndef DOMX_CORE_vfpc_indtxscwb_m3vpss
      #define DOMX_OMXPROXY_VFPC_INDTXSCWB
    #endif
  #endif

  #ifdef _BUILD_vswmosaic_
    #ifndef DOMX_CORE_vswmosaic_m3vpss
      #define DOMX_OMXPROXY_VSWMOSAIC
    #endif
  #endif

  #ifdef _BUILD_vsrc_
    #ifndef DOMX_CORE_vsrc_m3vpss
      #define DOMX_OMXPROXY_VSRC
    #endif
  #endif

  #ifdef _BUILD_vsnk_
    #ifndef DOMX_CORE_vsnk_m3vpss
      #define DOMX_OMXPROXY_VSNK
    #endif
  #endif

  #ifdef _BUILD_ctrl_
    #ifndef DOMX_CORE_ctrl_m3vpss
      #define DOMX_OMXPROXY_CTRL
    #endif
  #endif

  #ifdef _BUILD_ctrl_
    #ifndef DOMX_CORE_ctrldc_m3vpss
      #define DOMX_OMXPROXY_CTRLDC
    #endif
  #endif

  #ifdef _BUILD_venc_
    #ifndef DOMX_CORE_venc_m3vpss
      #define DOMX_OMXPROXY_VENC
    #endif
  #endif

  #ifdef _BUILD_vdec_
    #ifndef DOMX_CORE_vdec_m3vpss
      #define DOMX_OMXPROXY_VDEC
    #endif
  #endif

  #ifdef _BUILD_cmux_
    #ifndef DOMX_CORE_cmux_m3vpss
      #define DOMX_OMXPROXY_CMUX
    #endif
  #endif

  #ifdef _BUILD_dmux_
    #ifndef DOMX_CORE_dmux_m3vpss
      #define DOMX_OMXPROXY_DMUX
    #endif
  #endif

  #ifdef _BUILD_rtptx_
    #ifndef DOMX_CORE_rtptx_m3vpss
      #define DOMX_OMXPROXY_RTPTX
    #endif
  #endif

  #ifdef _BUILD_rtprx_
    #ifndef DOMX_CORE_rtprx_m3vpss
      #define DOMX_OMXPROXY_RTPRX
    #endif
  #endif

  #ifdef _BUILD_vlpb_
    #ifndef DOMX_CORE_vlpb_m3vpss
      #define DOMX_OMXPROXY_VLPB
    #endif
  #endif

  #ifdef _BUILD_adec_
    #ifndef DOMX_CORE_auddec_m3vpss
     #define DOMX_OMXPROXY_AUDDEC
    #endif
  #endif

  #ifdef _BUILD_aenc_
    #ifndef DOMX_CORE_audenc_m3vpss
     #define DOMX_OMXPROXY_AUDENC
    #endif
  #endif

#endif /* _LOCAL_CORE_m3vpss_ */

#ifdef _LOCAL_CORE_a8host_      /* Current build iteration is for a8host */

  #ifdef _BUILD_vfcc_
    #ifndef DOMX_CORE_vfcc_a8host
      #define DOMX_OMXPROXY_VFCC
    #endif
  #endif

  #ifdef _BUILD_vfdc_
    #ifndef DOMX_CORE_vfdc_a8host
      #define DOMX_OMXPROXY_VFDC
  #endif
 #endif
 
 #ifdef _BUILD_vsplc_
  #ifndef DOMX_CORE_vsplc_a8host
   #define DOMX_OMXPROXY_VSPLC
    #endif
  #endif

  #ifdef _BUILD_vfpc_deihdualout_
    #ifndef DOMX_CORE_vfpc_deihdualout_a8host
      #define DOMX_OMXPROXY_VFPC_DEIH
    #endif
  #endif

  #ifdef _BUILD_vfpc_deimdualout_
    #ifndef DOMX_CORE_vfpc_deimdualout_a8host
      #define DOMX_OMXPROXY_VFPC_DEIM
    #endif
  #endif

  #ifdef _BUILD_vfpc_nf_
    #ifndef DOMX_CORE_vfpc_nf_a8host
      #define DOMX_OMXPROXY_VFPC_NF
    #endif
  #endif

  #ifdef _BUILD_vfpc_indtxscwb_
    #ifndef DOMX_CORE_vfpc_indtxscwb_a8host
      #define DOMX_OMXPROXY_VFPC_INDTXSCWB
    #endif
  #endif

  #ifdef _BUILD_vsrc_
    #ifndef DOMX_CORE_vsrc_a8host
      #define DOMX_OMXPROXY_VSRC
    #endif
  #endif

  #ifdef _BUILD_vsnk_
    #ifndef DOMX_CORE_vsnk_a8host
      #define DOMX_OMXPROXY_VSNK
    #endif
  #endif

  #ifdef _BUILD_ctrl_
    #ifndef DOMX_CORE_ctrl_a8host
      #define DOMX_OMXPROXY_CTRL
    #endif
  #endif

  #ifdef _BUILD_ctrl_
    #ifndef DOMX_CORE_ctrldc_a8host
      #define DOMX_OMXPROXY_CTRLDC
    #endif
  #endif

  #ifdef _BUILD_venc_
    #ifndef DOMX_CORE_venc_a8host
      #define DOMX_OMXPROXY_VENC
    #endif
  #endif

  #ifdef _BUILD_vdec_
    #ifndef DOMX_CORE_vdec_a8host
      #define DOMX_OMXPROXY_VDEC
    #endif
  #endif

  #ifdef _BUILD_cmux_
    #ifndef DOMX_CORE_cmux_a8host
      #define DOMX_OMXPROXY_CMUX
    #endif
  #endif

  #ifdef _BUILD_dmux_
    #ifndef DOMX_CORE_dmux_a8host
      #define DOMX_OMXPROXY_DMUX
    #endif
  #endif

  #ifdef _BUILD_rtptx_
    #ifndef DOMX_CORE_rtptx_a8host
      #define DOMX_OMXPROXY_RTPTX
    #endif
  #endif

  #ifdef _BUILD_rtprx_
    #ifndef DOMX_CORE_rtprx_a8host
      #define DOMX_OMXPROXY_RTPRX
    #endif
  #endif

  #ifdef _BUILD_vlpb_
    #ifndef DOMX_CORE_vlpb_a8host
      #define DOMX_OMXPROXY_VLPB
    #endif
  #endif

  #ifdef _BUILD_adec_
    #ifndef DOMX_CORE_auddec_a8host
     #define DOMX_OMXPROXY_AUDDEC
    #endif
  #endif

  #ifdef _BUILD_vswmosaic_
    #ifndef DOMX_CORE_vswmosaic_a8host
      #define DOMX_OMXPROXY_VSWMOSAIC
    #endif
  #endif

  #ifdef _BUILD_aenc_
    #ifndef DOMX_CORE_audenc_a8host
     #define DOMX_OMXPROXY_AUDENC
    #endif
  #endif

#endif /* _LOCAL_CORE_a8host_ */

#ifdef _LOCAL_CORE_c6xdsp_      /* Current build iteration is for c6xdsp */

  #ifdef _BUILD_vfcc_
    #ifndef DOMX_CORE_vfcc_c6xdsp
      #define DOMX_OMXPROXY_VFCC
    #endif
  #endif

  #ifdef _BUILD_vfdc_
    #ifndef DOMX_CORE_vfdc_c6xdsp
      #define DOMX_OMXPROXY_VFDC
    #endif
  #endif

 #ifdef _BUILD_vsplc_
  #ifndef DOMX_CORE_vsplc_c6xdsp
   #define DOMX_OMXPROXY_VSPLC
    #endif
  #endif

  #ifdef _BUILD_vfpc_deihdualout_
    #ifndef DOMX_CORE_vfpc_deihdualout_c6xdsp
      #define DOMX_OMXPROXY_VFPC_DEIH
    #endif
  #endif

  #ifdef _BUILD_vfpc_deimdualout_
    #ifndef DOMX_CORE_vfpc_deimdualout_c6xdsp
      #define DOMX_OMXPROXY_VFPC_DEIM
    #endif
  #endif

  #ifdef _BUILD_vfpc_nf_
    #ifndef DOMX_CORE_vfpc_nf_c6xdsp
      #define DOMX_OMXPROXY_VFPC_NF
    #endif
  #endif

  #ifdef _BUILD_vfpc_indtxscwb_
    #ifndef DOMX_CORE_vfpc_indtxscwb_c6xdsp
      #define DOMX_OMXPROXY_VFPC_INDTXSCWB
    #endif
  #endif

  #ifdef _BUILD_vsrc_
    #ifndef DOMX_CORE_vsrc_c6xdsp
      #define DOMX_OMXPROXY_VSRC
    #endif
  #endif

  #ifdef _BUILD_vsnk_
    #ifndef DOMX_CORE_vsnk_c6xdsp
      #define DOMX_OMXPROXY_VSNK
    #endif
  #endif

  #ifdef _BUILD_ctrl_
    #ifndef DOMX_CORE_ctrl_c6xdsp
      #define DOMX_OMXPROXY_CTRL
    #endif
  #endif

  #ifdef _BUILD_ctrl_
    #ifndef DOMX_CORE_ctrldc_c6xdsp
      #define DOMX_OMXPROXY_CTRLDC
    #endif
  #endif

  #ifdef _BUILD_venc_
    #ifndef DOMX_CORE_venc_c6xdsp
      #define DOMX_OMXPROXY_VENC
    #endif
  #endif

  #ifdef _BUILD_vdec_
    #ifndef DOMX_CORE_vdec_c6xdsp
      #define DOMX_OMXPROXY_VDEC
    #endif
  #endif

  #ifdef _BUILD_cmux_
    #ifndef DOMX_CORE_cmux_c6xdsp
      #define DOMX_OMXPROXY_CMUX
    #endif
  #endif

  #ifdef _BUILD_dmux_
    #ifndef DOMX_CORE_dmux_c6xdsp
      #define DOMX_OMXPROXY_DMUX
    #endif
  #endif

  #ifdef _BUILD_rtptx_
    #ifndef DOMX_CORE_rtptx_c6xdsp
      #define DOMX_OMXPROXY_RTPTX
    #endif
  #endif

  #ifdef _BUILD_rtprx_
    #ifndef DOMX_CORE_rtprx_c6xdsp
      #define DOMX_OMXPROXY_RTPRX
    #endif
  #endif

  #ifdef _BUILD_vlpb_
    #ifndef DOMX_CORE_vlpb_c6xdsp
      #define DOMX_OMXPROXY_VLPB
    #endif
  #endif

  #ifdef _BUILD_adec_
    #ifndef DOMX_CORE_auddec_c6xdsp
     #define DOMX_OMXPROXY_AUDDEC
    #endif
  #endif
  #ifdef _BUILD_aenc_
    #ifndef DOMX_CORE_audenc_c6xdsp
     #define DOMX_OMXPROXY_AUDENC
    #endif
  #endif

  #ifdef _BUILD_vswmosaic_
    #ifndef DOMX_CORE_vswmosaic_c6xdsp
      #define DOMX_OMXPROXY_VSWMOSAIC
    #endif
  #endif

#endif /* _LOCAL_CORE_c6xdsp_ */

/* Nothing beyond this point */
