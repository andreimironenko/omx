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
 *  @file  omx_base_utils.h
 *
 *  @path \ti\omx\comp\omxbase\
 *
 *  @rev 1.0
 */

/*==============================================================
 *! Revision History
 *! ============================
 *! 17-April-2008 Rabbani Patan rabbani@ti.com: Initial Version
 *================================================================*/

#ifndef _OMX_BASE_UTILS_H_
#define _OMX_BASE_UTILS_H_

#ifdef __cplusplus
extern "C"
{
#endif                          /* __cplusplus */

/******************************************************************
 *   INCLUDE FILES
 ******************************************************************/
#include <ti/omx/omxutils/omx_utils.h>

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

#define OMX_BASE_Error(ARGS) OMX_UTL_Error(ARGS)
#define OMX_BASE_Trace(ARGS) OMX_UTL_Trace(ARGS)


#define OMX_BASE_Malloc(_nSize_, _hHeap_) OMX_UTL_Malloc(_nSize_, _hHeap_)

#define OMX_BASE_MALLOC_STRUCT(_pStruct_, _sName_, _hHeap_) \
        OMX_UTL_MALLOC_STRUCT(_pStruct_, _sName_, _hHeap_)

#define OMX_BASE_REQUIRE OMX_BASE_PARAMCHECK
#define OMX_BASE_ENSURE  OMX_BASE_PARAMCHECK

#define OMX_BASE_ASSERT(_COND_, _ERRORCODE_) \
        if(!(_COND_))                                                          \
        {                                                                      \
          eError = _ERRORCODE_;                                                \
          Log_print2(Diags_USER1, " OMX Error in %s :: line %d \n",(xdc_IArg)__FUNCTION__, __LINE__); \
          goto EXIT;                                                           \
        }

#define OMX_BASE_PARAMCHECK(_COND_, _ERRORCODE_) \
        OMX_UTL_PARAMCHECK(_COND_, _ERRORCODE_)

#define OMX_BASE_CHK_VERSION(_pStruct_, _sName_, _e_) \
        OMX_UTL_CHK_VERSION(_pStruct_, _sName_, _e_)

#define OMX_BASE_INIT_STRUCT_PTR(_pStruct_, _sName_) \
        OMX_UTL_INIT_STRUCT_PTR(_pStruct_, _sName_)

#define OMXBASE_UTL_TRACE_FUNCTION_ASSERT(condition,msg)                       \
          if(!(condition))                                                     \
          {                                                                    \
            Log_print4 (Diags_USER1, "Module<%s>:Assert failure in <%s> @line<%d>; Error: %s", \
                        (xdc_IArg) "ti.omx",                                   \
                        (xdc_IArg) __FUNCTION__, __LINE__, (xdc_IArg) msg);    \
            assert(condition);                                                 \
          }

/*******************************************************************
 * PRIVATE DECLARATIONS: defined here, used only here
 *******************************************************************/
/*----------          data declarations        ------------------- */
/*----------          function prototypes      ------------------- */

#ifdef __cplusplus
}
#endif                          /* __cplusplus */

#endif                          /* _OMX_BASE_UTILS_H_ */
