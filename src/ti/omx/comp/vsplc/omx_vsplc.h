/* ====================================================================
 *             Texas Instruments OMAP(TM) Platform Software
 * (c) Copyright Texas Instruments, Incorporated. All Rights Reserved.
 *
 * Use of this software is controlled by the terms and conditions found
 * in the license agreement under which this software has been supplied.
 * ==================================================================== */

/*!
  @file     omx_vsplc.h
  @brief    This file contains methods that Support the functionality for the
            Openmax Video Splitter component.
  @author   Yogesh Marathe
  @version  0.1 - June 6,2011
*/

/* -------------------------------------------------------------------------- */
/* =========================================================================
 *! Revision History
 *! ========================================================================
 *!06-June-2011  Yogesh Marathe: Initial Version
 * ========================================================================= */

#ifndef _OMX_VIDSPLC_COMPONENT_H
#define _OMX_VIDSPLC_COMPONENT_H

#ifdef _cplusplus
extern "C"   {
#endif /* _cplusplus */

/* User code goes here */
/* ------compilation control switches ----------------------------------------*/
/****************************************************************
 * INCLUDE FILES
 ****************************************************************/
/* ----- system and platform files ----------------------------*/
#include <xdc/std.h>
#include "omx_base.h"

/** OMX Video Encoder component's name */
#define OMX_VIDSPLC_COMPONENT_NAME              "OMX.TI.DUCATI.VIDSPLC"

 /** Maximum Number of input ports for the VIDSPLC Comp */
#define  OMX_VIDSPLC_MAX_NUM_INPUT_PORTS      (16)

 /** Maximum Number of output ports for the VIDSPLC Comp */
#define  OMX_VIDSPLC_MAX_NUM_OUTPUT_PORTS     (32)

 /** Maximum Number of ports for the VIDSPLC Comp */
#define  OMX_VIDSPLC_MAX_NUM_PORTS     (OMX_VIDSPLC_MAX_NUM_INPUT_PORTS +  \
                                       OMX_VIDSPLC_MAX_NUM_OUTPUT_PORTS)

 /** Deafult port start number of VIDSPLC comp */
#define  OMX_VIDSPLC_DEFAULT_START_PORT_NUM (0)

 /** Input port start Index for the VIDSPLC OMX Comp */
#define OMX_VIDSPLC_INPUT_PORT_START_INDEX (OMX_VIDSPLC_DEFAULT_START_PORT_NUM)

 /** Output port start Index for the VIDSPLC OMX Comp */
#define OMX_VIDSPLC_OUTPUT_PORT_START_INDEX (OMX_VIDSPLC_DEFAULT_START_PORT_NUM + \
                                            OMX_VIDSPLC_MAX_NUM_INPUT_PORTS)

 /** Maximum Number of outputport groups */
#define  OMX_VIDSPLC_MAX_NUM_OUTPUT_PORT_GROUPS     (2)

 /** Maximum Number of outputport groups */
#define  OMX_VIDSPLC_MAX_NUM_OUTPUT_PORTS_PER_GROUP (16)


/**
 *   @ struct : OMX_PARAM_VIDSPLC_PORTMAPINFO
 *   @ brief  : Provides spliter Port properties
 *
 *   @ param   nSize              	    :  Size of the structure in bytes
 *   @ param   nVersion           	    :  OMX specification version info
 *   @ param   nPortIndex               :  Input port Index
 *   @ param   nNumMappedOutputPorts    :  Mapped output ports
 *   @ param   nMappedOutputPortIndices :  output port indices
 *
 */
typedef struct OMX_PARAM_VIDSPLC_PORTMAPINFO
{
	OMX_U32         nSize;
	OMX_VERSIONTYPE nVersion;
	OMX_U32         nPortIndex;
	OMX_U8          nNumMappedOutputPorts;
	OMX_U8          nMappedOutputPortIndices[OMX_VIDSPLC_MAX_NUM_OUTPUT_PORTS];
} OMX_PARAM_VIDSPLC_PORTMAPINFO;


OMX_ERRORTYPE OMX_TI_VIDSPLC_ComponentInit(OMX_HANDLETYPE hComponent);


#ifdef _cplusplus
}
#endif /* __cplusplus */

#endif /* _OMX_VIDSPLC_COMPONENT_H */

/*@}*/ /* ingroup OMXVIDENC */






