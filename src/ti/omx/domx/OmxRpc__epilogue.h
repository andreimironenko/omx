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
 *  ======== OmxRpc__epilogue.h ========
 *
 */

#ifndef OMXRPC__EPILOGUE_H_
#define OMXRPC__EPILOGUE_H_

#define OmxRpc_genCoreRcmServerName(buf, coreId)                               \
          snprintf (buf, OMX_MAX_STRINGNAME_SIZE, "%s_%d",                     \
                    OmxRpc_rcmServerNamePrefix, coreId)

#define OmxRpc_genComponentCtrlRcmServerName(buf, componentName,coreId,instID) \
          snprintf (buf, OMX_MAX_STRINGNAME_SIZE, "%s_%s_Ctrl_%d_%d",          \
                    OmxRpc_rcmServerNamePrefix, componentName, coreId, instID)

#define OmxRpc_genComponentCbRcmServerName(buf, componentName,coreId,instID)   \
          snprintf (buf, OMX_MAX_STRINGNAME_SIZE, "%s_%s_Cb_%d_%d",            \
                    OmxRpc_rcmServerNamePrefix, componentName, coreId, instID)

#define OmxRpc_genCoreMsgqHeapName(buf,remoteCoreId,localCoreId)               \
          if (remoteCoreId < localCoreId)                                      \
          {                                                                    \
            snprintf (buf, OMX_MAX_STRINGNAME_SIZE, "%s_%s_%d_%d",             \
                      OmxRpc_rcmServerNamePrefix, "MsgQ", remoteCoreId,        \
                      localCoreId);                                            \
          }                                                                    \
          else                                                                 \
          {                                                                    \
            snprintf (buf, OMX_MAX_STRINGNAME_SIZE, "%s_%s_%d_%d",             \
                      OmxRpc_rcmServerNamePrefix, "MsgQ", localCoreId,         \
                      remoteCoreId);                                           \
          }

/*
 * TODO See if these function prototypes need to be moved out of xdc file and
 * moved into a header file.
 * This issue is bcos omx xdc spec doesnt exactly match the Omx standard header
 * files
 */
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

OMX_ERRORTYPE OmxRpc_stubCbEventHandler (OMX_HANDLETYPE hComponent,
                                         OMX_PTR pAppData,
                                         OMX_EVENTTYPE eEvent,
                                         OMX_U32 nData1,
                                         OMX_U32 nData2, OMX_PTR pEventData);
OMX_ERRORTYPE OmxRpc_stubCbEmptyBufferDone (OMX_HANDLETYPE hComponent,
                                            OMX_PTR pAppData,
                                            OMX_BUFFERHEADERTYPE *pBufHdr);
OMX_ERRORTYPE OmxRpc_stubCbFillBufferDone (OMX_HANDLETYPE hComponent,
                                           OMX_PTR pAppData,
                                           OMX_BUFFERHEADERTYPE *pBufHdr);

#include <xdc/std.h>
#include <ti/sdo/rcm/RcmClient.h>

/* Internal Functions */
OmxRpc_errorType OmxRpc_rcmClientCreate (RcmClient_Handle *rcmHndlPtr,
                                         String rcmServerName,
                                         UInt16 rcmMsgHeapId);

OmxRpc_errorType OmxRpc_rcmClientRemoteFxnLocate (RcmClient_Handle rcmHndl,
                                                  OmxRpc_remoteFxnCategory
                                                    regFxnCategory,
                                                  OmxRpc_remoteFxnEntryTbl
                                                    *pFxnsTbl);
OmxRpc_errorType OmxRpc_rcmMsgAlloc (RcmClient_Handle rcmHndl,
                                     RcmClient_Message **rcmMsgPtr,
                                     String fileName, Int line_num);

OmxRpc_errorType OmxRpc_rcmExec (RcmClient_Handle rcmHndl,
                                 RcmClient_Message *rcmMsg, UInt32 fxnIdx);

OmxRpc_errorType OmxRpc_moduleDeinitClient (DomxTypes_coreType remoteCoreId);

OmxRpc_errorType OmxRpc_moduleInitClient (DomxTypes_coreType remoteCoreId);

#endif /* OMXRPC__EPILOGUE_H_ */
