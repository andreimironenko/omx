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
 *  @file  DmmDelegate.c
 *         DOMX DmmDelegate that implements SharedMemory based address translation
 *
 *  @path \avmecomp\ti\omx\domx\delegates\shmem\DmmDelegate.c
 *
 *  @rev 1.0
 */

 /******************************************************************
 *   INCLUDE FILES
 ******************************************************************/

/* xdctools header files */
#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/IHeap.h>

/* syslink header files */

#include <ti/ipc/SharedRegion.h>
#include <ti/omx/interfaces/openMaxv11/interfaces_openmaxv11_internal.h>
#include <ti/omx/interfaces/openMaxv11/OMX_Core.h>
#include <ti/omx/domx/interfaces/domx_interfaces_internal.h>
#include <ti/omx/domx/domx_internal.h>
#include <ti/omx/domx/domx_internalcfg.h>
#include "domx_util.h"

/**
 * @name DmmDelegate_mapBuffer
 * @brief  DmmDelegate ShMem function to translate localPtr into SharedRegionPtr
 * @param  coreId           : Remote coreID
 * @param  pBuffer          : Pointer to Buffer
 * @param  nSizeBytes       : Size of buffer in bytes
 * @param  mappedAddress    : Pointer to mapped buffer address
 * @return IDmm_errorNone   : SUCCESS
 */
IDmm_errorType DmmDelegate_mapBuffer (DomxTypes_coreType coreId,
                                      UInt8 *pBuffer, UInt32 nSizeBytes,
                                      UInt32 *mappedAddress)
{
  Int sharedRegionIndex;
  IDmm_errorType retVal = IDmm_errorNone;
  SharedRegion_SRPtr srPtr;

  (Void) (coreId);
  (Void) (nSizeBytes);          /* Kill Warnings */
  if (IDmm_errorNone == retVal)
  {
    sharedRegionIndex = SharedRegion_getId (pBuffer);
    if (SharedRegion_INVALIDREGIONID == sharedRegionIndex)
    {
      retVal = IDmm_errorInvalidMapAddress;
    }
  }
  if (IDmm_errorNone == retVal)
  {
    srPtr = SharedRegion_getSRPtr (pBuffer, sharedRegionIndex);
    if (SharedRegion_invalidSRPtr () == srPtr)
    {
      retVal = IDmm_errorInvalidMapAddress;
    }
  }
  if (IDmm_errorNone == retVal)
  {
    *mappedAddress = (UInt32) srPtr;
  }
  return (retVal);
}

/**
 * @name   DmmDelegate_xltMapBuf2Ptr
 * @brief  DmmDelegate ShMem function to translate SharedRegionPtr to localPtr
 * @param  mappedAddress    : Mapped buffer address value (SrPtr)
 * @param  pBuffer          : Pointer to LocalBuffer address to be set
 * @return IDmm_errorNone   : SUCCESS
 */
IDmm_errorType DmmDelegate_xltMapBuf2Ptr (UInt32 mappedAddress, UInt8 **pBuffer)
{
  IDmm_errorType retVal = IDmm_errorNone;
  Void *ptr;

  if (IDmm_errorNone == retVal)
  {
    ptr = SharedRegion_getPtr ((SharedRegion_SRPtr) mappedAddress);
    *pBuffer = ptr;
  }
  return retVal;
}

/**
 * @name   DmmDelegate_unmapBuffer
 * @brief  DmmDelegate ShMem function to unmap buffer from remote core
 * @param  coreId           : Remote coreID 
 * @param  mappedAddress    : Mapped buffer address value 
 * @return IDmm_errorNone   : SUCCESS
 */
IDmm_errorType DmmDelegate_unmapBuffer (DomxTypes_coreType coreId,
                                        UInt32 mappedAddress)
{

  (Void) (coreId);
  (Void) (mappedAddress);
  return IDmm_errorNone;
}

/**
 * @name   DmmDelegate_flushBuffer
 * @brief  DmmDelegate ShMem function to flush buffer
 * @param  coreId           : Remote coreID 
 * @param  pBuffer          : Pointer to buffer 
 * @param  size             : Size of buffer in bytes
 * @return IDmm_errorNone   : SUCCESS
 */
IDmm_errorType DmmDelegate_flushBuffer (DomxTypes_coreType coreId,
                                        UInt8 *pBuffer, UInt32 size)
{
  (Void) (coreId);
  (Void) (pBuffer);
  (Void) (size);
  return IDmm_errorNone;
}

/* Nothing beyond this point */
