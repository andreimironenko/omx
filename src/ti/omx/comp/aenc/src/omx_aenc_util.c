/*******************************************************************************
 * Texas Instruments OpenMax Multimedia Sub-system                             *
 *                                                                             *
 * "OpenMax Multimedia Sub-system" is a software module developed on TI's DM   *
 * class of SOCs. These modules are based on Khronos open max 1.1.2            *
 * specification.                                                              *
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 ******************************************************************************/

/**
 *******************************************************************************
 *  @file   omx_aenc_utils.c
 *  @brief  This source file contains function definitions, 
 *          for utilities of Audio Encoder Component.
 *
 *  @rev 1.0
 *******************************************************************************
 */

#include "omx_aenc_util.h"
/**
********************************************************************************
 *  @func  OMX_TI_AUDENC_UTL_GetBufStatus
 *  @brief Initializa the ComponentPrivate Fields only for output port
 *
 *  @param bufHeader    : pointer to buffer header array
 *  @param cnt          : number of entries in buffer header array
 *  @param pBuffheader  : target buffer header
 
 *  @return 
 *  status = status of the buffer 
 *
 *
 * ============================================================================= 
 */

OMX_U32 OMX_TI_AUDENC_UTL_GetBufStatus(OMX_AUDENC_buf_header *bufHeader, 
                                       OMX_U32 cnt, OMX_PTR pBuffheader )
{
    OMX_U32 i, status;

    for(i=0; i < cnt ; i++) {
        if( bufHeader->pBufHeader == pBuffheader) {
            status = bufHeader->bufStatus;
            break;
        }
        bufHeader++;  
    }
    return(status);   
}
/**
********************************************************************************
 *  @func  OMX_TI_AUDENC_UTL_SetBufStatus
 *  @brief Initializa the ComponentPrivate Fields only for output port
 *
 *  @param bufHeader    : pointer to buffer header array
 *  @param cnt          : number of entries in buffer header array
 *  @param pBuffheader  : target buffer header
 *  @param status       : status to be set
 *
 *  @return 
 *
 * ============================================================================= 
 */

void OMX_TI_AUDENC_UTL_SetBufStatus(OMX_AUDENC_buf_header *bufHeader, OMX_U32 cnt,
                                    OMX_PTR pBuffheader, OMX_U32 status )
{
    OMX_U32 i;
  
    for(i=0; i < cnt ; i++) {
        if( bufHeader->pBufHeader == pBuffheader) {
            bufHeader->bufStatus = status;
            break;
        }
       bufHeader++;  
    }    
}
