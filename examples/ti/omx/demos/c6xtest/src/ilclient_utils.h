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

#ifndef __OMX_ILCLIENT_UTILS_H__
#define __OMX_ILCLIENT_UTILS_H__

#define IL_CLIENT_VLPB_OUTPUT_BUFFER_COUNT    (4)
#define IL_CLIENT_VLPB_INPUT_BUFFER_COUNT     (4)
#define IL_CLIENT_VLPB_BUFFER_WIDTH           (10)
#define IL_CLIENT_VLPB_BUFFER_HEIGHT          (10)
#define IL_CLIENT_VLPB_BUFFER_SIZE            (IL_CLIENT_VLPB_BUFFER_WIDTH * \
                                               IL_CLIENT_VLPB_BUFFER_HEIGHT)
#define IL_CLIENT_VLPB_PATTERN                (0x0DA)
#define IL_CLIENT_VLPB_MAX_FRAMES             (10)

OMX_ERRORTYPE IL_ClientSetVlpbParams (IL_Client *pAppData);

OMX_ERRORTYPE IL_ClientEncUseInitialOutputResources (IL_CLIENT_COMP_PRIVATE
                                                       *thisComp);
OMX_ERRORTYPE IL_ClientUseInitialOutputResources (IL_CLIENT_COMP_PRIVATE
                                                    *thisComp);
OMX_ERRORTYPE IL_ClientConnectComponents (IL_CLIENT_COMP_PRIVATE
                                            *handleCompPrivA,
                                          unsigned int compAPortOut,
                                          IL_CLIENT_COMP_PRIVATE
                                            *handleCompPrivB,
                                          unsigned int compBPortIn);
OMX_ERRORTYPE IL_ClientUtilGetSelfBufHeader (IL_CLIENT_COMP_PRIVATE *thisComp,
                                             OMX_U8 *pBuffer,
                                             ILCLIENT_PORT_TYPE type,
                                             OMX_U32 portIndex,
                                             OMX_BUFFERHEADERTYPE **pBufferOut);
OMX_ERRORTYPE IL_ClientUseInitialInputOutputResources (IL_CLIENT_COMP_PRIVATE
                                                         *thisComp);

#endif

/* Nothing beyond this point */
