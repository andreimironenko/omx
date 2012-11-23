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

/*
 *  ======== OmxProxy__epilogue.h ========
 *
 */

#ifndef OMXPROXY__EPILOGUE_H_
#define OMXPROXY__EPILOGUE_H_

#include <ti/omx/interfaces/openMaxv11/OMX_Types.h>
#include <ti/omx/interfaces/openMaxv11/OMX_Index.h>
#include <ti/omx/interfaces/openMaxv11/OMX_Core.h>
#include <ti/omx/interfaces/openMaxv11/OMX_Component.h>
#include <ti/omx/interfaces/openMaxv11/OMX_Audio.h>
#include <ti/omx/interfaces/openMaxv11/OMX_IVCommon.h>
#include <ti/omx/interfaces/openMaxv11/OMX_Image.h>
#include <ti/omx/interfaces/openMaxv11/OMX_Video.h>
#include <ti/omx/interfaces/openMaxv11/OMX_Other.h>
#include <ti/omx/interfaces/openMaxv11/OMX_ContentPipe.h>

OMX_ERRORTYPE OmxProxy_commonInit (OMX_HANDLETYPE hComponent,
                                   OMX_STRING cComponentName);

typedef struct OMX_CONFIG_DOMXPROXYCOMPINFO
{
  OMX_U32 nSize;   /**< Size of the structure in bytes */
  OMX_VERSIONTYPE nVersion;   /**< OMX specification version information */
  OMX_HANDLETYPE hCompRealHandle;                              /**< Real Component handle - valid only on remote core */
  OMX_PTR pRpcStubHandle;                                      /**< Rpc Stub Handle for the OmxProxy */
  OMX_U32 nRpcSkelPtr;                                         /**< Rpc Skel Handle for the OmxProxy - Valid only on remote core */
  OMX_S8 cComponentName[OMX_MAX_STRINGNAME_SIZE];              /**< Component name  */
  OMX_S8 cComponentRcmSvrName[OMX_MAX_STRINGNAME_SIZE];        /**< Component rcmsvr name  */
} OMX_CONFIG_DOMXPROXYCOMPINFO;

OMX_ERRORTYPE OmxProxy_createLite (OMX_HANDLETYPE *pProxyLiteHandlePtr,
                                   OmxProxy_proxyLiteCreateParams *cp);

extern OmxProxy_configParamStructInfoMap *OmxProxy_omxCustomStructInfoMap;
#endif /* OMXPROXY__EPILOGUE_H_ */
