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
 * DmmDelegate
 */
#include <ti/omx/domx/domx_internal.h>

/* heapType */
typedef enum DmmDelegate_heapType_e
{
  DmmDelegate_heapFirst = 0x0000,
  DmmDelegate_heapIpcComm = DmmDelegate_heapFirst,
  DmmDelegate_heapIpcData = 0x0001,
  DmmDelegate_heapLast = DmmDelegate_heapIpcData,
  DmmDelegate_heapCount = (DmmDelegate_heapLast + 1)
} DmmDelegate_heapType;

/* ipcDataHeapConfig */
typedef struct DmmDelegate_ipcDataHeapConfig_s
{
  xdc_UInt32 maxNumDataHeaps;
  xdc_Int sharedRegionIndex;
  xdc_UInt32 sharedMemPhyBase;
  xdc_UInt32 sharedMemSize;
  xdc_UInt16 srOwnerId;
} DmmDelegate_ipcDataHeapConfig;

/* ipcHeapInfo */
typedef struct DmmDelegate_ipcHeapInfo_s
{
  xdc_String heapName;
  xdc_UInt16 remoteMultiProcId;
  DmmDelegate_heapType heapType;
} DmmDelegate_ipcHeapInfo;

IDmm_errorType DmmDelegate_mapBuffer (DomxTypes_coreType coreId,
                                      UInt8 *pBuffer,
                                      UInt32 nSizeBytes,
                                      UInt32 *mappedAddress);

IDmm_errorType DmmDelegate_unmapBuffer (DomxTypes_coreType coreId,
                                        UInt32 mappedAddress);

IDmm_errorType DmmDelegate_xltMapBuf2Ptr (UInt32 mappedAddress,
                                          UInt8 **pBuffer);

IDmm_errorType DmmDelegate_flushBuffer (DomxTypes_coreType coreId,
                                        UInt8 *pBuffer, UInt32 size);

IHeap_Handle DmmDelegate_createIpcHeap (DmmDelegate_ipcHeapInfo *heapInfo,
                                        Error_Block *eb);
/* Nothing beyond this point */
