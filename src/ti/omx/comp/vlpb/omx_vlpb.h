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
*   @file  omx_vlpb.h
 *   This file contains API/Defines that provides the functionality for the
 *   {V}ideo {L}oo{P}{B}ack component.
 *   - A user would typically use functions and data structures defined in this
 *   module to access different VLPB functionalites.
 *
 *   @path \ti\omx\comp\vlpb\
*
*   @rev  1.0
 */

/** ===========================================================================
 *! Revision History
 *! ===========================================================================
 *!17-October-2009 : Initial Version
 * ============================================================================
 */

#ifndef _OMX_VLPB_H
#define _OMX_VLPB_H

#ifdef _cplusplus
extern "C"
{
#endif                          /* __cplusplus */

  /* -------------------compilation control switches ----------------------- */
/* none */
  /* ----------------------------------------------------------------------- */

/*****************************************************************************
 * -------------------INCLUDE FILES-------------------------------------------
 *****************************************************************************/
  /* ------------------ system and platform files -------------------------- */
/* none */
/*--------------------program files -----------------------------------------*/
#include "OMX_Types.h"

/*****************************************************************************
 * PUBLIC DECLARATIONS Defined here, used elsewhere
 *****************************************************************************/

/* Component name for VLPB. This component can be compiled for any core - A8
 * or DSP. Therefore, it has different names to uniquely identify the
 * dsp VLPB component and A8 VLPB component
 */
/* Component name on DSP */
#define OMX_VLPB_DSPCOMP_NAME ((OMX_STRING) "OMX.TI.C67X.VLPB")
/* Component name on A8  */
#define OMX_VLPB_HOSTCOMP_NAME ((OMX_STRING) "OMX.TI.A8.VLPB")
/*--------------------------Data declarations --------------------------------*/

/* ========================================================================== */
/* Macros & Typedefs                                                          */
/* ========================================================================== */
  
 /** Default port start number of VLPB comp */
#define OMX_VLPB_DEFAULT_START_PORT_NUM (0)

 /** Input port Index for the VLPB OMX Comp */
#define OMX_VLPB_INPUT_PORT_START_INDEX (OMX_VLPB_DEFAULT_START_PORT_NUM)

 /** Output port Index for the VLPB OMX Comp */
#define OMX_VLPB_OUTPUT_PORT_START_INDEX  (OMX_VLPB_DEFAULT_START_PORT_NUM +   \
                                           OMX_VLPB_NUM_INPUT_PORTS)

 /** Maximum Number of input ports for the VLPB Comp */
#define OMX_VLPB_NUM_INPUT_PORTS        (16)

 /** Maximum Number of output ports for the VLPB Component */
#define OMX_VLPB_NUM_OUTPUT_PORTS       (16)

/*******************************************************************************
* Enumerated Types
*******************************************************************************/
/* None */

/*******************************************************************************
* Strutures
*******************************************************************************/
/* None */

/*----------------------function prototypes ---------------------------------*/

/** OMX VLPB Component Init */
OMX_ERRORTYPE OMX_TI_VLPB_ComponentInit (OMX_HANDLETYPE hComponent);

#ifdef _cplusplus
}
#endif /* __cplusplus */

#endif /* _OMX_VLPB_H */

/* Nothing beyond this point */
