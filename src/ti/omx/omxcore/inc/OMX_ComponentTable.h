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
 *  @file  OMX_ComponentTable.h
 *  This file contains the component table definition
 *
 *  @path \ti\omx\omxcore\inc
 *
 *  @rev 1.0
 */

/*==============================================================
 *! Revision History
 *! ============================
 *! 20-Dec-2008 x0052661@ti.com, initial version
 *================================================================*/

#ifndef _OMX_COMPONENT_TABLE_H_
#define _OMX_COMPONENT_TABLE_H_

#ifdef __cplusplus
extern "C"
{
#endif                          /* __cplusplus */

/******************************************************************
 *   INCLUDE FILES
 ******************************************************************/
#include "ti/omx/interfaces/openMaxv11/OMX_Core.h"

/*******************************************************************
 * EXTERNAL REFERENCE NOTE: only use if not found in header file
 *******************************************************************/
/*----------         function prototypes      ------------------- */
/*----------         data declarations        ------------------- */
/*******************************************************************
 * PUBLIC DECLARATIONS: defined here, used elsewhere
 *******************************************************************/
/*----------         function prototypes      ------------------- */
/*----------         data declarations        ------------------- */

/*----------         defines                  ------------------- */
#define OMX_MAX_COMP_ROLES 10

/* ==========================================================================*/
/*
 * OMX_COMPONENTLIST Structure maintain the table for component names and
 * function pointer for the respective component init functions.
 * Component name in the table must be passed by the application when it calls
 * the OMX_GetHandle function.
 *
 */
/* ==========================================================================*/
typedef struct OMX_COMPONENTLIST
{

  /* pointer to store the component name. The component name must be null
     terminated and follow the follwing format:
     OMX.<VENDOR_ID>.<COMPONENT_TYPE>.<COMPONENT_NAME> where: <VENDOR_ID> - 2 
     or 3 letter vendor id. Example: TI, PSW <COMPONENT_TYPE> - Type of the
     component. Example: AUDIO, VIDEO <COMPONENT_NAME> - Name of the
     componet. Example: DSP.MIXER example: "OMX.TI.AUDIO.DSP.MIXER\0" */
  OMX_STRING cComponentName;

  /* The exported ComponentInit function of the component */
    OMX_ERRORTYPE (*pComponentInit) (OMX_HANDLETYPE hComponent);

  /* Number of Roles Supported by a Component */
  OMX_U32 nNumRoles;

  /* List of Roles that each Component Supports */
  OMX_U8 roleArray[OMX_MAX_COMP_ROLES][OMX_MAX_STRINGNAME_SIZE];
} OMX_COMPONENTLIST;

/* external definition for the ComponentTable */
extern OMX_COMPONENTLIST OMXComponentList[];

/*******************************************************************
 * PRIVATE DECLARATIONS: defined here, used only here
 *******************************************************************/
/*----------          data declarations        ------------------- */
/*----------          function prototypes      ------------------- */

#ifdef __cplusplus
}
#endif                          /* __cplusplus */

#endif                          /* _OMX_COMPONENT_TABLE_H_ */

/* Nothing beyond this point */
