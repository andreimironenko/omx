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
 *  @file  domx_utils.h
 *
 *  @rev 1.0
 */

#ifndef _DOMX_UTL_UTILS_H_
#define _DOMX_UTL_UTILS_H_

#ifdef __cplusplus
extern "C"
{
#endif                          /* __cplusplus */

/******************************************************************
 *   INCLUDE FILES
 ******************************************************************/
#include <string.h>
#include <assert.h>
#include <OMX_Types.h>
#include <OMX_Core.h>
#include <xdc/std.h>

#ifndef DOMX_UTL_TRACE_MODULE_NAME
  #define DOMX_UTL_TRACE_MODULE_NAME "ti.omx"
#endif

#define DOMX_UTL_TRACE_FUNCTION_ENTRY()                                        \
          Log_print3 (Diags_ENTRY, "Module<%s> Entering<%s> @line<%d> ",        \
                      (xdc_IArg) DOMX_UTL_TRACE_MODULE_NAME,                   \
                      (xdc_IArg) __FUNCTION__, __LINE__);

#define DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 DOMX_UTL_TRACE_FUNCTION_ENTRY

#define DOMX_UTL_TRACE_FUNCTION_EXIT(errorVal)                                 \
          Log_print5 (Diags_EXIT,                                             \
                      "Module<%s> Leaving<%s> @line<%d> with error<%d:%s>",    \
                      (xdc_IArg) DOMX_UTL_TRACE_MODULE_NAME,                   \
                      (xdc_IArg) __FUNCTION__, __LINE__, errorVal,             \
                      (xdc_IArg) DOMX_UTL_OmxErrorToString ((OMX_ERRORTYPE) errorVal));

#define DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 DOMX_UTL_TRACE_FUNCTION_EXIT

#define DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL2()                                 \
          Log_print3 (Diags_ENTRY, "Module<%s> Entering<%s> @line<%d> ",       \
          (xdc_IArg) DOMX_UTL_TRACE_MODULE_NAME, (xdc_IArg) __FUNCTION__,      \
          __LINE__);

#define DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL2(errorVal)                          \
          Log_print5 (Diags_EXIT,                                             \
          "Module<%s> Leaving<%s> @line<%d> with error<%d:%s>",                \
          (xdc_IArg) DOMX_UTL_TRACE_MODULE_NAME, (xdc_IArg) __FUNCTION__,      \
          __LINE__, errorVal, (xdc_IArg) DOMX_UTL_OmxErrorToString ((OMX_ERRORTYPE) errorVal));

#define DOMX_UTL_TRACE_FUNCTION_MSG(printMsg)                                  \
          Log_print4 (Diags_USER1, "Module<%s> @<%s> @line<%d> msg<%s>",       \
          (xdc_IArg) DOMX_UTL_TRACE_MODULE_NAME, (xdc_IArg) __FUNCTION__,      \
          __LINE__, (xdc_IArg) printMsg);

#define DOMX_UTL_TRACE_FUNCTION_MSG_LEVEL1 DOMX_UTL_TRACE_FUNCTION_MSG

#define DOMX_UTL_TRACE_FUNCTION_MSG_LEVEL2(printMsg)                           \
          Log_print4 (Diags_USER3, "Module<%s> @<%s> @line<%d> msg<%s>",       \
          (xdc_IArg) DOMX_UTL_TRACE_MODULE_NAME, (xdc_IArg) __FUNCTION__,      \
          __LINE__, (xdc_IArg) printMsg);

#define DOMX_UTL_TRACE_FUNCTION_ERROR(condition,msg)                           \
          if (!(condition))                                                    \
          {                                                                    \
            Log_print3 (Diags_USER1, "Module<%s>:Error in <%s> @line<%d>",     \
                        (xdc_IArg) DOMX_UTL_TRACE_MODULE_NAME,                 \
                        (xdc_IArg) __FUNCTION__, __LINE__);                    \
          }

#define DOMX_UTL_TRACE_FUNCTION_ASSERT(condition,msg)                          \
          if(!(condition))                                                     \
          {                                                                    \
            Log_print4 (Diags_USER1, "Module<%s>:Assert failure in <%s> @line<%d>; Error: %s", \
                        (xdc_IArg) DOMX_UTL_TRACE_MODULE_NAME,                 \
                        (xdc_IArg) __FUNCTION__, __LINE__, (xdc_IArg) msg);    \
            assert(condition);                                                 \
          }

static inline void DOMX_UTL_Strncpy (OMX_STRING dststr, OMX_STRING srcstr,
                                     OMX_U32 size);

OMX_STRING DOMX_UTL_OmxErrorToString (OMX_ERRORTYPE eError);

static inline void DOMX_UTL_Strncpy (OMX_STRING dststr,
                                     OMX_STRING srcstr, OMX_U32 size)
{
  strncpy (dststr, srcstr, (size - 1));

  dststr[size - 1] = '\0';
}

#ifndef __ti__
//#define ENABLE_LINUX_LOGGING
#ifdef ENABLE_LINUX_LOGGING
#define Log_print0(x,y) printf(y);printf("\n")
#define Log_print1(x,y,z1) printf(y,z1);printf("\n")
#define Log_print2(x,y,z1,z2) printf(y,z1,z2);printf("\n")
#define Log_print3(x,y,z1,z2,z3) printf(y,z1,z2,z3);printf("\n")
#define Log_print4(x,y,z1,z2,z3,z4) printf(y,z1,z2,z3,z4);printf("\n")
#define Log_print5(x,y,z1,z2,z3,z4,z5) printf(y,z1,z2,z3,z4,z5);printf("\n")
#define Log_print6(x,y,z1,z2,z3,z4,z5,z6) printf(y,z1,z2,z3,z4,z5,z6);printf("\n")
#endif
#endif

#ifdef __cplusplus
}
#endif                          /* __cplusplus */

#endif                          /* _DOMX_UTL_UTILS_H_ */
