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
 *  @file  OmxProxy.c
 *         DOMX Proxy component functionality
 *
 *  @path \avmecomp\ti\omx\domx\OmxProxy.c
 *
 *  @rev 1.0
 */

 /******************************************************************
 *   INCLUDE FILES
 ******************************************************************/
#include <xdc/std.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <xdc/runtime/Memory.h>
#include <xdc/runtime/IHeap.h>
#include <ti/ipc/SharedRegion.h>

#ifdef __ti__
//#define OMXPROXY_MEMLOG
#endif

#ifndef _LOCAL_CORE_a8host_
  #include <xdc/runtime/Log.h>
  #include <xdc/runtime/Assert.h>
  #include <xdc/runtime/Diags.h>
#else
  /* Use Slog at A8 side */
  #if (USE_SLOG_PRINT == 1)
    #include <Log.h>
    #include <LoggerSys.h>

    /* Every module needs to define unique module id */
    #undef Module__MID
    #define Module__MID SLOG_DOMX_Module__MID
  #else
    #include <xdc/runtime/Log.h>
    #include <xdc/runtime/Assert.h>
    #include <xdc/runtime/Diags.h>
  #endif /* End Of #if (USE_SLOG_PRINT == 1) */
#endif /* End of #else */
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

#include <ti/omx/interfaces/openMaxv11/OMX_TI_Index.h>
#include <ti/omx/interfaces/openMaxv11/interfaces_openmaxv11_internal.h>
#include <timm_osal_memory.h>
#include "domx_internal.h"
#include "domx_internalcfg.h"
#include "OmxProxy__epilogue.h"
#include "domx_util.h"

#ifdef OMXPROXY_MEMLOG
/**
 * @name   omxproxy_memstats_getfreestats
 * @brief  Function to get free memory stats from all the heaps in the system
 * @param  freeSize                 : Pointer to freeSize array to be populated
 * @param  nMaxSize                 : Max size of free size array
 * @return Number of heaps in the system
 */
static Int omxproxy_memstats_getfreestats (OMX_U32 freeSize[], OMX_U32 nMaxSize)
{
  SharedRegion_Entry srEntry;
  Int i;
  Int heapIndex = 0;
  Memory_Stats memstats;

  Memory_getStats (NULL, &(memstats));
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((nMaxSize > heapIndex),
                                  "heapIndex >= maxSize");
  freeSize[heapIndex++] = memstats.totalFreeSize;
  for (i = 0; i < SharedRegion_getNumRegions (); i++)
  {
    SharedRegion_getEntry (i, &srEntry);
    if (TRUE == srEntry.isValid)
    {
      if (TRUE == srEntry.createHeap)
      {
        IHeap_Handle srHeap = SharedRegion_getHeap (i);
        DOMX_UTL_TRACE_FUNCTION_ASSERT ((nMaxSize > heapIndex),
                                        "heapIndex >= maxSize");
        Memory_getStats (srHeap, &(memstats));
        freeSize[heapIndex++] = memstats.totalFreeSize;
      }
    }
  }
  return heapIndex;
}

/**
 * @name   omxproxy_memstats_getusedsize
 * @brief  Function to get used memory stats from all the heaps in the system
 * @param  initialfreeSize          : Pointer to initial free size array
 * @param  usedSize                 : Pointer to used size array to be populated
 * @param  numHeaps                 : Number of heaps in the system
 * @return Number of heaps in the system
 */
static Void omxproxy_memstats_getusedsize (OMX_U32 initialfreeSize[],
                                           OMX_U32 usedSize[], OMX_U32 numHeaps)
{
  Int i;
  OMX_U32 currentfreeSize[DOMX_MEMUSAGE_MAX_HEAPS_PER_PHASE];
  OMX_U32 numHeapsEnd;

  DOMX_UTL_TRACE_FUNCTION_ASSERT ((numHeaps <=
                                   DOMX_MEMUSAGE_MAX_HEAPS_PER_PHASE),
                                  "numHeaps > maxHeaps");
  numHeapsEnd =
    omxproxy_memstats_getfreestats (&(currentfreeSize[0]),
                                    DOMX_MEMUSAGE_MAX_HEAPS_PER_PHASE);
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((numHeapsEnd == numHeaps),
                                  "numHeaps incorrect");

  for (i = 0; i < numHeaps; i++)
  {
    DOMX_UTL_TRACE_FUNCTION_ASSERT ((initialfreeSize[i] >= currentfreeSize[i]),
                                    "Current free size > initial free size");
    usedSize[i] = initialfreeSize[i] - currentfreeSize[i];
  }
}

/**
 * @name   omxproxy_memstats_getfreedsize
 * @brief  Function to get freed memory stats from all the heaps in the system
 * @param  initialfreeSize          : Pointer to initial free size array
 * @param  freedSize                 : Pointer to freed size array to be populated
 * @param  numHeaps                 : Number of heaps in the system
 * @return Number of heaps in the system
 */
static Void omxproxy_memstats_getfreedsize (OMX_U32 initialfreeSize[],
                                            OMX_U32 freedSize[],
                                            OMX_U32 numHeaps)
{
  Int i;
  OMX_U32 currentfreeSize[DOMX_MEMUSAGE_MAX_HEAPS_PER_PHASE];
  OMX_U32 numHeapsEnd;

  DOMX_UTL_TRACE_FUNCTION_ASSERT ((numHeaps <=
                                   DOMX_MEMUSAGE_MAX_HEAPS_PER_PHASE),
                                  "numHeaps > maxHeaps");
  numHeapsEnd =
    omxproxy_memstats_getfreestats (&(currentfreeSize[0]),
                                    DOMX_MEMUSAGE_MAX_HEAPS_PER_PHASE);
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((numHeapsEnd == numHeaps),
                                  "numHeaps incorrect");

  for (i = 0; i < numHeaps; i++)
  {
    DOMX_UTL_TRACE_FUNCTION_ASSERT ((currentfreeSize[i] >= initialfreeSize[i]),
                                    "initial free size > Current free size");
    freedSize[i] = currentfreeSize[i] - initialfreeSize[i];
  }
}

/**
 * @name   omxproxy_memstats_checkleak
 * @brief  Function to check if memory leak occured
 * @param  allocSize                 : Pointer to alloc size array
 * @param  freedSize                 : Pointer to freed size array
 * @param  numHeaps                  : Number of heaps in the system
 * @param  id                        : ID used to identify stage when printing leaks. 
 * @return Number of heaps in the system
 */
static Void omxproxy_memstats_checkleak (OMX_U32 allocSize[],
                                         OMX_U32 freedSize[], OMX_U32 numHeaps,
                                         OMX_U32 id)
{
  Int i;
  for (i = 0; i < numHeaps; i++)
  {
    if (allocSize[i] != freedSize[i])
    {
      Log_print4 (Diags_INFO, "\nDOMXPROXY:MemoryLeak:STAGE:%d\tHEAPNUM:%d\t"
                     "ALLOC=%d\tFREED=%d", id, i, allocSize[i], freedSize[i]);
  //    System_flush ();
    }
  }
}
#endif

#ifdef OMXPROXY_MEMLOG
#define OMXPROXY_MEMLOG_USED_START()                                           \
          do                                                                   \
          {                                                                    \
            OMX_U32 nNumHeaps =      0;                                        \
            OMX_U32 freeMemStart[DOMX_MEMUSAGE_MAX_HEAPS_PER_PHASE];           \
                                                                               \
            nNumHeaps = omxproxy_memstats_getfreestats (freeMemStart,          \
                                              DOMX_MEMUSAGE_MAX_HEAPS_PER_PHASE)
#else
#define OMXPROXY_MEMLOG_USED_START()
#endif

#ifdef OMXPROXY_MEMLOG
#define OMXPROXY_MEMLOG_USED_END(pUsedSize)                                    \
            omxproxy_memstats_getusedsize (freeMemStart, pUsedSize, nNumHeaps);\
          } while (0)
#else
#define OMXPROXY_MEMLOG_USED_END(pUsedSize)
#endif

#ifdef OMXPROXY_MEMLOG
#define OMXPROXY_MEMLOG_FREE_START()                                           \
          do                                                                   \
          {                                                                    \
            OMX_U32 nNumHeaps =      0;                                        \
            OMX_U32 freeMemStart[DOMX_MEMUSAGE_MAX_HEAPS_PER_PHASE];           \
            OMX_U32 freedSize[DOMX_MEMUSAGE_MAX_HEAPS_PER_PHASE];              \
                                                                               \
            nNumHeaps = omxproxy_memstats_getfreestats (freeMemStart,          \
                                             DOMX_MEMUSAGE_MAX_HEAPS_PER_PHASE)
#else
#define OMXPROXY_MEMLOG_FREE_START()
#endif

#ifdef OMXPROXY_MEMLOG
#define OMXPROXY_MEMLOG_FREE_END(pAllocSize,id)                                \
            omxproxy_memstats_getfreedsize (freeMemStart, freedSize,           \
                                            nNumHeaps);                        \
                                                                               \
            omxproxy_memstats_checkleak (pAllocSize, freedSize, nNumHeaps, id);\
          } while (0)
#else
#define OMXPROXY_MEMLOG_FREE_END(pAllocSize,id)
#endif

/*!
 *  @struct PROXY_COMPONENT_PRIVATE
 *  @brief  Structure defining proxy private instance structure
 */
typedef struct PROXY_COMPONENT_PRIVATE
{
  OMX_CALLBACKTYPE cbInfo;                /**< IL Client callback structrue */
  OMX_HANDLETYPE pRemoteComponentHandle;  /**< Pointer to remote component handle */
  OMX_STATETYPE eState;                   /**< Component state */
  OMX_PTR pApplicationPrivate;            /**< Pointer to app private */
  Char szComponentName[OMX_MAX_STRINGNAME_SIZE + 1];  /**< Name of component */
  OmxRpc_Handle rpcHandle;                            /**< Handle to RPC instance */
  OmxTypes_OMX_U32 nRpcRemoteHandle;                  /**< Remote RPC handle value */
  Char szComponentRcmServerName[OMX_MAX_STRINGNAME_SIZE + 1]; /**< RcmServerName of component        */
  DomxTypes_coreType realCoreId;                              /**< COmponent native coreID           */
  OMX_U32 maxNumBuffers;                                      /**< Max number of  buffers            */
  OmxProxy_proxyBufferInfo *bufferList;                       /**< Pointer to buffers list           */
  OMX_U32 numOfBuffers;                                       /**< Buffer count                      */
  OmxProxy_configParamStructInfo *customParamStructInfo;      /**< Pointer to customerParamStructure */
    OMX_U32 memoryUsed[DOMXPROXY_MEMUSAGEPHASE_NUM_PHASES][DOMX_MEMUSAGE_MAX_HEAPS_PER_PHASE];
                                                                                        /**< Array storing memory used from each heap in different stages */
} PROXY_COMPONENT_PRIVATE;

/*!
 *  @brief  Array mapping standard OMX parameter indexes to corresponding structure sizes
 */
static OmxProxy_configParamStructInfo omxproxy_omx_std_struct_info[] =
{
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamPriorityMgmt,
   sizeof (OMX_PRIORITYMGMTTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamAudioInit,
   sizeof (OMX_PORT_PARAM_TYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamImageInit,
   sizeof (OMX_PORT_PARAM_TYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamVideoInit,
   sizeof (OMX_PORT_PARAM_TYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamOtherInit,
   sizeof (OMX_PORT_PARAM_TYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamNumAvailableStreams,
   sizeof (OMX_PARAM_U32TYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamActiveStream,
   sizeof (OMX_PARAM_U32TYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamSuspensionPolicy,
   sizeof (OMX_PARAM_SUSPENSIONPOLICYTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamComponentSuspended,
   sizeof (OMX_PARAM_SUSPENSIONTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigCapturing,
   sizeof (OMX_CONFIG_BOOLEANTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigCaptureMode,
   sizeof (OMX_CONFIG_CAPTUREMODETYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexAutoPauseAfterCapture,
   sizeof (OMX_CONFIG_BOOLEANTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamContentURI,
   sizeof (OMX_PARAM_CONTENTURITYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamCustomContentPipe,
   sizeof (OMX_PARAM_CONTENTPIPETYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamDisableResourceConcealment,
   sizeof (OMX_RESOURCECONCEALMENTTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigMetadataItemCount,
   sizeof (OMX_CONFIG_METADATAITEMCOUNTTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamMetadataKeyFilter,
   sizeof (OMX_PARAM_METADATAFILTERTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigPriorityMgmt,
   sizeof (OMX_PRIORITYMGMTTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamStandardComponentRole,
   sizeof (OMX_PARAM_COMPONENTROLETYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamPortDefinition,
   sizeof (OMX_PARAM_PORTDEFINITIONTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamCompBufferSupplier,
   sizeof (OMX_PARAM_BUFFERSUPPLIERTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamAudioPortFormat,
   sizeof (OMX_AUDIO_PARAM_PORTFORMATTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamAudioPcm,
   sizeof (OMX_AUDIO_PARAM_PCMMODETYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamAudioAac,
   sizeof (OMX_AUDIO_PARAM_AACPROFILETYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamAudioRa,
   sizeof (OMX_AUDIO_PARAM_RATYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamAudioMp3,
   sizeof (OMX_AUDIO_PARAM_MP3TYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamAudioAdpcm,
   sizeof (OMX_AUDIO_PARAM_ADPCMTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamAudioG723,
   sizeof (OMX_AUDIO_PARAM_G723TYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamAudioG729,
   sizeof (OMX_AUDIO_PARAM_G729TYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamAudioAmr,
   sizeof (OMX_AUDIO_PARAM_AMRTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamAudioWma,
   sizeof (OMX_AUDIO_PARAM_WMATYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamAudioSbc,
   sizeof (OMX_AUDIO_PARAM_SBCTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamAudioMidi,
   sizeof (OMX_AUDIO_PARAM_MIDITYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamAudioMidiLoadUserSound,
   sizeof (OMX_AUDIO_PARAM_MIDILOADUSERSOUNDTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamAudioG726,
   sizeof (OMX_AUDIO_PARAM_G726TYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamAudioGsm_EFR,
   sizeof (OMX_AUDIO_PARAM_GSMEFRTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamAudioGsm_HR,
   sizeof (OMX_AUDIO_PARAM_GSMHRTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamAudioPdc_FR,
   sizeof (OMX_AUDIO_PARAM_PDCFRTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamAudioPdc_EFR,
   sizeof (OMX_AUDIO_PARAM_PDCEFRTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamAudioPdc_HR,
   sizeof (OMX_AUDIO_PARAM_PDCHRTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamAudioTdma_FR,
   sizeof (OMX_AUDIO_PARAM_TDMAFRTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamAudioTdma_EFR,
   sizeof (OMX_AUDIO_PARAM_TDMAEFRTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamAudioQcelp8,
   sizeof (OMX_AUDIO_PARAM_QCELP8TYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamAudioQcelp13,
   sizeof (OMX_AUDIO_PARAM_QCELP13TYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamAudioEvrc,
   sizeof (OMX_AUDIO_PARAM_EVRCTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamAudioSmv,
   sizeof (OMX_AUDIO_PARAM_SMVTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamAudioVorbis,
   sizeof (OMX_AUDIO_PARAM_VORBISTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigAudioMidiImmediateEvent,
   sizeof (OMX_AUDIO_CONFIG_MIDIIMMEDIATEEVENTTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigAudioMidiControl,
   sizeof (OMX_AUDIO_CONFIG_MIDICONTROLTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigAudioMidiSoundBankProgram,
   sizeof (OMX_AUDIO_CONFIG_MIDISOUNDBANKPROGRAMTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigAudioMidiStatus,
   sizeof (OMX_AUDIO_CONFIG_MIDISTATUSTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigAudioMidiMetaEvent,
   sizeof (OMX_AUDIO_CONFIG_MIDIMETAEVENTTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigAudioVolume,
   sizeof (OMX_AUDIO_CONFIG_VOLUMETYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigAudioBalance,
   sizeof (OMX_AUDIO_CONFIG_BALANCETYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigAudioChannelMute,
   sizeof (OMX_AUDIO_CONFIG_CHANNELMUTETYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigAudioMute,
   sizeof (OMX_AUDIO_CONFIG_MUTETYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigAudioLoudness,
   sizeof (OMX_AUDIO_CONFIG_LOUDNESSTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigAudioEchoCancelation,
   sizeof (OMX_AUDIO_CONFIG_ECHOCANCELATIONTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigAudioNoiseReduction,
   sizeof (OMX_AUDIO_CONFIG_NOISEREDUCTIONTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigAudioBass,
   sizeof (OMX_AUDIO_CONFIG_BASSTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigAudioTreble,
   sizeof (OMX_AUDIO_CONFIG_TREBLETYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigAudioStereoWidening,
   sizeof (OMX_AUDIO_CONFIG_STEREOWIDENINGTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigAudioChorus,
   sizeof (OMX_AUDIO_CONFIG_CHORUSTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigAudioEqualizer,
   sizeof (OMX_AUDIO_CONFIG_EQUALIZERTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigAudioReverberation,
   sizeof (OMX_AUDIO_CONFIG_REVERBERATIONTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigAudioChannelVolume,
   sizeof (OMX_AUDIO_CONFIG_CHANNELVOLUMETYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamImagePortFormat,
   sizeof (OMX_IMAGE_PARAM_PORTFORMATTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamFlashControl,
   sizeof (OMX_IMAGE_PARAM_FLASHCONTROLTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigFocusControl,
   sizeof (OMX_IMAGE_CONFIG_FOCUSCONTROLTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamQFactor,
   sizeof (OMX_IMAGE_PARAM_QFACTORTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamQuantizationTable,
   sizeof (OMX_IMAGE_PARAM_QUANTIZATIONTABLETYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamHuffmanTable,
   sizeof (OMX_IMAGE_PARAM_HUFFMANTTABLETYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigFlashControl,
   sizeof (OMX_IMAGE_PARAM_FLASHCONTROLTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamVideoPortFormat,
   sizeof (OMX_VIDEO_PARAM_PORTFORMATTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamVideoFastUpdate,
   sizeof (OMX_VIDEO_PARAM_VIDEOFASTUPDATETYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamVideoBitrate,
   sizeof (OMX_VIDEO_PARAM_BITRATETYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamVideoMotionVector,
   sizeof (OMX_VIDEO_PARAM_MOTIONVECTORTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamVideoIntraRefresh,
   sizeof (OMX_VIDEO_PARAM_INTRAREFRESHTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamVideoErrorCorrection,
   sizeof (OMX_VIDEO_PARAM_ERRORCORRECTIONTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamVideoVBSMC,
   sizeof (OMX_VIDEO_PARAM_VBSMCTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamVideoMpeg2,
   sizeof (OMX_VIDEO_PARAM_MPEG2TYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamVideoMpeg4,
   sizeof (OMX_VIDEO_PARAM_MPEG4TYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamVideoWmv,
   sizeof (OMX_VIDEO_PARAM_WMVTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamVideoRv,
   sizeof (OMX_VIDEO_PARAM_RVTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamVideoAvc,
   sizeof (OMX_VIDEO_PARAM_AVCTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamVideoH263,
   sizeof (OMX_VIDEO_PARAM_H263TYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigVideoBitrate,
   sizeof (OMX_VIDEO_CONFIG_BITRATETYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigVideoIntraVOPRefresh,
   sizeof (OMX_CONFIG_INTRAREFRESHVOPTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigVideoIntraMBRefresh,
   sizeof (OMX_CONFIG_MACROBLOCKERRORMAPTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigVideoMBErrorReporting,
   sizeof (OMX_CONFIG_MBERRORREPORTINGTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamVideoMacroblocksPerFrame,
   sizeof (OMX_PARAM_MACROBLOCKSTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigVideoMacroBlockErrorMap,
   sizeof (OMX_CONFIG_MACROBLOCKERRORMAPTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamVideoSliceFMO,
   sizeof (OMX_VIDEO_PARAM_AVCSLICEFMO)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigVideoAVCIntraPeriod,
   sizeof (OMX_VIDEO_CONFIG_AVCINTRAPERIOD)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigVideoNalSize,
   sizeof (OMX_VIDEO_CONFIG_NALSIZE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamCommonDeblocking,
   sizeof (OMX_PARAM_DEBLOCKINGTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamCommonSensorMode,
   sizeof (OMX_PARAM_SENSORMODETYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamCommonInterleave,
   sizeof (OMX_PARAM_INTERLEAVETYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigCommonColorFormatConversion,
   sizeof (OMX_CONFIG_COLORCONVERSIONTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigCommonScale,
   sizeof (OMX_CONFIG_SCALEFACTORTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigCommonImageFilter,
   sizeof (OMX_CONFIG_IMAGEFILTERTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigCommonColorEnhancement,
   sizeof (OMX_CONFIG_COLORENHANCEMENTTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigCommonColorKey,
   sizeof (OMX_CONFIG_COLORKEYTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigCommonColorBlend,
   sizeof (OMX_CONFIG_COLORBLENDTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigCommonFrameStabilisation,
   sizeof (OMX_CONFIG_FRAMESTABTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigCommonRotate,
   sizeof (OMX_CONFIG_ROTATIONTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigCommonMirror,
   sizeof (OMX_CONFIG_MIRRORTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigCommonOutputPosition,
   sizeof (OMX_CONFIG_POINTTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigCommonInputCrop,
   sizeof (OMX_CONFIG_RECTTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigCommonOutputCrop,
   sizeof (OMX_CONFIG_RECTTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigCommonDigitalZoom,
   sizeof (OMX_CONFIG_SCALEFACTORTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigCommonOpticalZoom,
   sizeof (OMX_CONFIG_SCALEFACTORTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigCommonWhiteBalance,
   sizeof (OMX_CONFIG_WHITEBALCONTROLTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigCommonExposure,
   sizeof (OMX_CONFIG_EXPOSURECONTROLTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigCommonContrast,
   sizeof (OMX_CONFIG_CONTRASTTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigCommonBrightness,
   sizeof (OMX_CONFIG_BRIGHTNESSTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigCommonBacklight,
   sizeof (OMX_CONFIG_BACKLIGHTTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigCommonGamma,
   sizeof (OMX_CONFIG_GAMMATYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigCommonSaturation,
   sizeof (OMX_CONFIG_SATURATIONTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigCommonLightness,
   sizeof (OMX_CONFIG_LIGHTNESSTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigCommonExclusionRect,
   sizeof (OMX_CONFIG_RECTTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigCommonPlaneBlend,
   sizeof (OMX_CONFIG_PLANEBLENDTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigCommonExposureValue,
   sizeof (OMX_CONFIG_EXPOSUREVALUETYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigCommonOutputSize,
   sizeof (OMX_FRAMESIZETYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamCommonExtraQuantData,
   sizeof (OMX_OTHER_EXTRADATATYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexParamOtherPortFormat,
   sizeof (OMX_OTHER_PARAM_PORTFORMATTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigOtherPower,
   sizeof (OMX_OTHER_CONFIG_POWERTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigOtherStats,
   sizeof (OMX_OTHER_CONFIG_STATSTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigTimeScale,
   sizeof (OMX_TIME_CONFIG_SCALETYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigTimeClockState,
   sizeof (OMX_TIME_CONFIG_CLOCKSTATETYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigTimeActiveRefClock,
   sizeof (OMX_TIME_CONFIG_ACTIVEREFCLOCKTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigTimeCurrentMediaTime,
   sizeof (OMX_TIME_CONFIG_TIMESTAMPTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigTimeCurrentWallTime,
   sizeof (OMX_TIME_CONFIG_TIMESTAMPTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigTimeCurrentAudioReference,
   sizeof (OMX_TIME_CONFIG_TIMESTAMPTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigTimeCurrentVideoReference,
   sizeof (OMX_TIME_CONFIG_TIMESTAMPTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigTimeMediaTimeRequest,
   sizeof (OMX_TIME_CONFIG_MEDIATIMEREQUESTTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigTimeClientStartTime,
   sizeof (OMX_TIME_CONFIG_TIMESTAMPTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigTimePosition,
   sizeof (OMX_TIME_CONFIG_TIMESTAMPTYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexConfigTimeSeekMode,
   sizeof (OMX_TIME_CONFIG_SEEKMODETYPE)}
  ,
  {(OmxIndex_OMX_INDEXTYPE) OMX_IndexMax, 0}
};

/* 
 *************************************************************************
 *                       Static functions
 *************************************************************************
 */
/**
 * @name omxproxy_map_component_name2info
 * @brief Map component name to DOMX Component COre Info Entry
 * @param  component_name  : OMX Component name
 * @param  info            : Info entry to be populated
 * @return OMX_ErrorNone : SUCCESS
 */
static OMX_ERRORTYPE
  omxproxy_map_component_name2info (String component_name,
                                    DomxCore_componentCoreInfoEntry **info)
{
  Int i;
  OMX_ERRORTYPE status = OMX_ErrorNone;
  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();

  *info = NULL;
  for (i = 0; i < DomxCore_componentCoreInfoTbl.length; i++)
  {
    if (strcmp ((DomxCore_componentCoreInfoTbl.elem[i][0]).name,
                component_name) == 0)
    {
      *info = &(DomxCore_componentCoreInfoTbl.elem[i][0]);
      break;
    }
  }
  if (i == DomxCore_componentCoreInfoTbl.length)
  {
    status = OMX_ErrorComponentNotFound;
  }

  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (status);

  return status;
}

/**
 * @name omxproxy_get_component_custom_config_info
 * @brief Map component name to COmponent custom config info structure
 * @param  component_name  : OMX Component name
 * @param  info            : Info entry to be populated
 * @return OMX_ErrorNone : SUCCESS
 */
static OMX_ERRORTYPE
  omxproxy_get_component_custom_config_info (String component_name,
                                             OmxProxy_configParamStructInfo
                                               **configInfo)
{
  OMX_ERRORTYPE status = OMX_ErrorNone;
  OmxProxy_configParamStructInfoMap *tbl_entry =
    OmxProxy_omxCustomStructInfoMap;
  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();

  *configInfo = NULL;
  while (tbl_entry->omxComponentName != NULL)
  {
    if (strcmp (tbl_entry->omxComponentName, component_name) == 0)
    {
      *configInfo = tbl_entry->configParamStructInfoTbl;
      break;
    }
    tbl_entry++;
  }
  if (tbl_entry->omxComponentName == NULL)
  {
    status = OMX_ErrorComponentNotFound;
  }

  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (status);
  return status;
}

/**
 * @name omxproxy_get_omx_custom_struct_size
 * @brief Map OMX param/config index to corresponding structure size for custom config index
 * @param  nParamIndex  : Param/Config index to be mapped
 * @param  nSize        : Pointer to structure size to be populated
 * @param  customStructInfo : COmponents custom config info structure
 * @return OMX_ErrorNone : SUCCESS
 */
static OMX_ERRORTYPE 
  omxproxy_get_omx_custom_struct_size (OMX_INDEXTYPE nParamIndex,
                                       SizeT *nSize,
                                       OmxProxy_configParamStructInfo
                                         *customStructInfo)
{
  int count = 0;
  OMX_ERRORTYPE status = OMX_ErrorNone;

  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();

  if (nSize == NULL)
    return OMX_ErrorBadParameter;

  *nSize = 0;
  while (customStructInfo[count].nParamIndex != OMX_IndexMax)
  {
    if (customStructInfo[count].nParamIndex == nParamIndex)
    {
      *nSize = customStructInfo[count].nParamStructSizeof;
      status = OMX_ErrorNone;
      break;
    }
    count++;
  }

  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (status);
  return status;
}

/**
 * @name omxproxy_get_omx_std_struct_size
 * @brief Map OMX param/config index to corresponding structure size for standard config index
 * @param  nParamIndex  : Param/Config index to be mapped
 * @param  nSize        : Pointer to structure size to be populated
 * @return OMX_ErrorNone : SUCCESS
 */
static OMX_ERRORTYPE
  omxproxy_get_omx_std_struct_size (OMX_INDEXTYPE nParamIndex, SizeT * nSize)
{
  int count = 0;
  OMX_ERRORTYPE status = OMX_ErrorNone;

  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();

  if (nSize == NULL)
    return OMX_ErrorBadParameter;

  *nSize = 0;

  while (omxproxy_omx_std_struct_info[count].nParamIndex != OMX_IndexMax)
  {
    if (omxproxy_omx_std_struct_info[count].nParamIndex == nParamIndex)
    {
      *nSize = omxproxy_omx_std_struct_info[count].nParamStructSizeof;
      status = OMX_ErrorNone;
      break;
    }
    count++;
  }

  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (status);
  return status;
}

/**
 * @name omxproxy_get_omx_struct_size
 * @brief Map OMX param/config index to corresponding structure size
 * @param  nParamIndex  : Param/Config index to be mapped
 * @param  nSize        : Pointer to structure size to be populated
 * @param  customStructInfo : COmponents custom config info structure
 * @param  pComponentParameterStructure: Pointer to config/param structure  
 * @return OMX_ErrorNone : SUCCESS
 */
static OMX_ERRORTYPE
  omxproxy_get_omx_struct_size (OMX_INDEXTYPE nConfigParamIndex,
                                SizeT *nSize,
                                OmxProxy_configParamStructInfo
                                  *customStructInfo,
                                OMX_PTR pComponentParameterStructure)
{
  OMX_ERRORTYPE status = OMX_ErrorNone;

  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();

  *nSize = 0;
  if (((UInt32) nConfigParamIndex) < ((UInt32) OMX_IndexVendorStartUnused))
  {
    /* Retrieve size based on the nConfigIndex in order to copy the struct into 
     * the RPCMsg argStruct union */
    status = omxproxy_get_omx_std_struct_size (nConfigParamIndex, nSize);
  }
  else
  {
    Log_print1 (Diags_USER1, "CustomIndexSize(%d) Lookup..", nConfigParamIndex);
    status =
      omxproxy_get_omx_custom_struct_size (nConfigParamIndex, nSize,
                                           customStructInfo);
  }
  if (OMX_ErrorNone == status)
  {
    if (0 == *nSize)
    {
      *nSize = ((OmxProxy_configStructCommonHeader *)
                pComponentParameterStructure)->nSize;
    }
  }
  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (status);
  return status;
}

/**
 * @name omxproxy_empty_buffer_done
 * @brief Proxy implementation of Empty Buffer Done
 * @param  hRealOmxComponent  : Handle to real component
 * @param  cbContext          : Callback context
 * @param  remoteBufHdr       : Remote OMX buf header
 * @param  pBufHdrInfo        : Pointer to OMX buf header
 * @return OMX_ErrorNone : SUCCESS
 */
static OmxCore_OMX_ERRORTYPE
  omxproxy_empty_buffer_done (OmxTypes_OMX_HANDLETYPE hRealOmxComponent,
                              OmxTypes_OMX_PTR cbContext,
                              OmxCore_OMX_BUFFERHEADERTYPE *remoteBufHdr,
                              OmxCore_OMX_BUFFERHEADERTYPE *pBufHdrInfo)
{
  OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *) cbContext;
  PROXY_COMPONENT_PRIVATE *pComponentPrivate =
    (PROXY_COMPONENT_PRIVATE *) pHandle->pComponentPrivate;
  OMX_BUFFERHEADERTYPE *pBufHdr = NULL;
  int count;
  OMX_ERRORTYPE eError = OMX_ErrorNone;

  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL2 ();
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((pComponentPrivate->pRemoteComponentHandle ==
                                   hRealOmxComponent),
                                  "Remote comp handle != omx comp handle");
  for (count = 0; count < pComponentPrivate->numOfBuffers; ++count)
  {
    if (pComponentPrivate->bufferList[count].pBufHeaderRemote ==
        (OMX_BUFFERHEADERTYPE *) remoteBufHdr)
    {
      pBufHdr = pComponentPrivate->bufferList[count].pBufHeader;
      pBufHdr->nFilledLen = pBufHdrInfo->nFilledLen;
      pBufHdr->nOffset = pBufHdrInfo->nOffset;
      pBufHdr->nFlags = pBufHdrInfo->nFlags;
      pBufHdr->pBuffer = pComponentPrivate->bufferList[count].pBufferActual;
      eError =
        pComponentPrivate->cbInfo.EmptyBufferDone (pHandle,
                                                   pHandle->
                                                   pApplicationPrivate,
                                                   pBufHdr);
      break;
    }
  }
  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL2 (eError);
  return (OmxCore_OMX_ERRORTYPE) eError;
}

/**
 * @name omxproxy_event_handler
 * @brief Proxy implementation of Empty Buffer Done
 * @param  hRealOmxComponent  : Handle to real component
 * @param  cbContext          : Callback context
 * @param  eEvent             : Callback event enum
 * @param  nData1             : data1 associated with event
 * @param  nData2             : data2 associated with event 
 * @param  pEventData         : Pointer to event data
 * @return OMX_ErrorNone : SUCCESS
 */
static OmxCore_OMX_ERRORTYPE omxproxy_event_handler (OmxTypes_OMX_HANDLETYPE
                                                       hRealOmxComponent,
                                                     OmxTypes_OMX_PTR
                                                       cbContext,
                                                     OmxCore_OMX_EVENTTYPE
                                                       eEvent,
                                                     OmxTypes_OMX_U32 nData1,
                                                     OmxTypes_OMX_U32 nData2,
                                                     OmxTypes_OMX_PTR
                                                       pEventData)
{
  OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *) cbContext;
  PROXY_COMPONENT_PRIVATE *pComponentPrivate =
    (PROXY_COMPONENT_PRIVATE *) pHandle->pComponentPrivate;
  OMX_ERRORTYPE eError = OMX_ErrorNone;

  Log_print5 (Diags_ENTRY, "Entered %s (0x%x, 0x%x, %d, 0x%x,",
              (xdc_IArg) __FUNCTION__, (xdc_IArg) hRealOmxComponent,
              (xdc_IArg) cbContext, (xdc_IArg) eEvent, (xdc_IArg) nData1);
  Log_print2 (Diags_ENTRY, "0x%x, 0x%x)", (xdc_IArg) nData2,
              (xdc_IArg) pEventData);

  DOMX_UTL_TRACE_FUNCTION_ASSERT ((pComponentPrivate->pRemoteComponentHandle ==
                                   hRealOmxComponent),
                                  "Remote comp handle != omx comp handle");

  eError =
    pComponentPrivate->cbInfo.EventHandler (pHandle,
                                            pHandle->pApplicationPrivate,
                                            (OMX_EVENTTYPE) eEvent, nData1,
                                            nData2, pEventData);
  Log_print2 (Diags_EXIT, "Exiting: %s retVal %d", (xdc_IArg) __FUNCTION__,
              eError);
  return (OmxCore_OMX_ERRORTYPE) eError;
}

/**
 * @name omxproxy_fill_buffer_done
 * @brief Proxy implementation of fill buffer done
 * @param  hRealOmxComponent  : Handle to real component
 * @param  cbContext          : Callback context
 * @param  remoteBufHdr       : Remote buffer header
 * @param  pBufHdrInfo        : Pointer to local OMX header
 * @return OMX_ErrorNone : SUCCESS
 */
static OmxCore_OMX_ERRORTYPE
  omxproxy_fill_buffer_done (OmxTypes_OMX_HANDLETYPE hRealOmxComponent,
                             OmxTypes_OMX_PTR cbContext,
                             OmxCore_OMX_BUFFERHEADERTYPE
                               *remoteBufHdr,
                             OmxCore_OMX_BUFFERHEADERTYPE *pBufHdrInfo)
{
  OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *) cbContext;
  PROXY_COMPONENT_PRIVATE *pComponentPrivate =
    (PROXY_COMPONENT_PRIVATE *) pHandle->pComponentPrivate;
  OMX_BUFFERHEADERTYPE *pBufHdr = NULL;
  int count;
  OMX_ERRORTYPE eError = OMX_ErrorNone;

  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL2 ();
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((pComponentPrivate->pRemoteComponentHandle ==
                                   hRealOmxComponent),
                                  "Remote comp handle != omx comp handle");

  for (count = 0; count < pComponentPrivate->numOfBuffers; ++count)
  {
    if (pComponentPrivate->bufferList[count].pBufHeaderRemote ==
        (OMX_BUFFERHEADERTYPE *) remoteBufHdr)
    {
      pBufHdr = pComponentPrivate->bufferList[count].pBufHeader;
      pBufHdr->nFilledLen = pBufHdrInfo->nFilledLen;
      pBufHdr->nOffset = pBufHdrInfo->nOffset;
      pBufHdr->nFlags = pBufHdrInfo->nFlags;
      pBufHdr->nTimeStamp = pBufHdrInfo->nTimeStamp;
      pBufHdr->pBuffer = pComponentPrivate->bufferList[count].pBufferActual;
      eError =
        pComponentPrivate->cbInfo.FillBufferDone (pHandle,
                                                  pHandle->
                                                  pApplicationPrivate, pBufHdr);
      break;
    }
  }
  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL2 (eError);

  return (OmxCore_OMX_ERRORTYPE) eError;
}

/**
 * @def   OMXPROXY_RSRC_PROXYINIT_NONE 
 * @def   OMXPROXY_RSRC_PROXYINIT_COMPPRIVATE_ALLOC
 * @def   OMXPROXY_RSRC_PROXYINIT_BUFFERLIST_ALLOC
 * @def   OMXPROXY_RSRC_PROXYINIT_OMXRPC_CREATE
 * @def   OMXPROXY_RSRC_PROXYINIT_OMXRPC_GETHANDLE
 * @def   OMXPROXY_RSRC_PROXYINIT_ALL
 * @brief Defines of resources allocated at ProxyInit time
 */
#define OMXPROXY_RSRC_PROXYINIT_NONE              (0)
#define OMXPROXY_RSRC_PROXYINIT_COMPPRIVATE_ALLOC (1 << 0)
#define OMXPROXY_RSRC_PROXYINIT_BUFFERLIST_ALLOC  (1 << 1)
#define OMXPROXY_RSRC_PROXYINIT_OMXRPC_CREATE     (1 << 2)
#define OMXPROXY_RSRC_PROXYINIT_OMXRPC_GETHANDLE  (1 << 3)
#define OMXPROXY_RSRC_PROXYINIT_ALL                                            \
                           (OMXPROXY_RSRC_PROXYINIT_COMPPRIVATE_ALLOC |        \
                           OMXPROXY_RSRC_PROXYINIT_BUFFERLIST_ALLOC   |        \
                           OMXPROXY_RSRC_PROXYINIT_OMXRPC_CREATE      |        \
                           OMXPROXY_RSRC_PROXYINIT_OMXRPC_GETHANDLE)

/**
 * @name omxproxy_free_proxyinit_rsrc
 * @brief Free ProxyInit allocated resources
 * @param  pHandle            : Pointer to proxy component handle
 * @param  rsrcMask           : bit mask indicating resources allocated
 * @return OMX_ErrorNone : SUCCESS
 */
static OMX_ERRORTYPE omxproxy_free_proxyinit_rsrc (OMX_COMPONENTTYPE *pHandle,
                                                   Int rsrcMask)
{
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  PROXY_COMPONENT_PRIVATE *pComponentPrivate;
  OmxRpc_errorType rpcCmdStatus;

  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();

  pComponentPrivate = pHandle->pComponentPrivate;
  if (OMXPROXY_RSRC_PROXYINIT_OMXRPC_GETHANDLE & rsrcMask)
  {
    OMXPROXY_MEMLOG_FREE_START ();
    rpcCmdStatus = OmxRpc_stubFreeHandle (pComponentPrivate->rpcHandle,
                                          (OmxCore_OMX_ERRORTYPE *) & eError);
    OMXPROXY_MEMLOG_FREE_END (&
                              (pComponentPrivate->
                               memoryUsed[DOMXPROXY_MEMUSAGEPHASE_RPCGETHANDLE]
                               [0]), DOMXPROXY_MEMUSAGEPHASE_RPCGETHANDLE);
    if (OmxRpc_errorNone != rpcCmdStatus)
    {
      DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (OMX_ErrorUndefined);
      /* If free Handle failed, return error */
      return OMX_ErrorUndefined;
    }
  }
  if (OMXPROXY_RSRC_PROXYINIT_OMXRPC_CREATE & rsrcMask)
  {
    OMXPROXY_MEMLOG_FREE_START ();
    OmxRpc_delete (&pComponentPrivate->rpcHandle);
    OMXPROXY_MEMLOG_FREE_END (&
                              (pComponentPrivate->
                               memoryUsed[DOMXPROXY_MEMUSAGEPHASE_RPCSTUBCREATE]
                               [0]), DOMXPROXY_MEMUSAGEPHASE_RPCSTUBCREATE);
  }
  if (OMXPROXY_RSRC_PROXYINIT_BUFFERLIST_ALLOC & rsrcMask)
  {
    Memory_free (OmxProxy_Module_heap (), pComponentPrivate->bufferList,
                 (sizeof (OmxProxy_proxyBufferInfo) *
                  pComponentPrivate->maxNumBuffers));
  }
  if (OMXPROXY_RSRC_PROXYINIT_COMPPRIVATE_ALLOC & rsrcMask)
  {
    Memory_free (OmxProxy_Module_heap (), pHandle->pComponentPrivate,
                 sizeof (PROXY_COMPONENT_PRIVATE));
  }
  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (eError);
  return eError;
}

/**
 * @name omxproxy_component_deinit
 * @brief  DOMX proxy implementation of component deInit
 * @param  hComponent         : Handle to the component
 * @return OMX_ErrorNone : SUCCESS
 */
static OMX_ERRORTYPE omxproxy_component_deinit (OMX_HANDLETYPE hComponent)
{
  OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *) hComponent;
  PROXY_COMPONENT_PRIVATE *pComponentPrivate =
    (PROXY_COMPONENT_PRIVATE *) pHandle->pComponentPrivate;
  unsigned int count;
  OmxRpc_errorType rpcCmdStatus;
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  OMX_U32 proxyAllocSize[DOMX_MEMUSAGE_MAX_HEAPS_PER_PHASE];

  Log_print1 (Diags_ENTRY, "%s Entered", (xdc_IArg) __FUNCTION__);

  memcpy (proxyAllocSize,
          &(pComponentPrivate->
            memoryUsed[DOMXPROXY_MEMUSAGEPHASE_PROXYCREATE][0]),
          sizeof (pComponentPrivate->
                  memoryUsed[DOMXPROXY_MEMUSAGEPHASE_PROXYCREATE][0]) *
          DOMX_MEMUSAGE_MAX_HEAPS_PER_PHASE);
  OMXPROXY_MEMLOG_FREE_START ();
  /* Unmap any buffers previously mapped */
  /* TODO : Buffer unmap & buffer header free is done before free Handle. Check 
     if this is okay */
  for (count = 0; count < pComponentPrivate->maxNumBuffers; ++count)
  {
    Log_print1 (Diags_USER1, "%s Cleaning up Buffers", (xdc_IArg) __FUNCTION__);
    if (pComponentPrivate->bufferList[count].pBufferMapped)
    {
      rpcCmdStatus = OmxRpc_stubUnMapBuffer (pComponentPrivate->rpcHandle,
                                             (UInt32) pComponentPrivate->
                                             bufferList[count].pBufferMapped);
      if (OmxRpc_errorNone != rpcCmdStatus)
      {
        Log_print4 (Diags_USER1, "%s:%d:: OmxRpc_stubUnmapBuffer failed"
                    "MappedAddr = %d, RpcErrorCode = %d",
                    (xdc_IArg) __FUNCTION__, __LINE__,
                    (xdc_IArg) pComponentPrivate->bufferList[count].
                    pBufferMapped, rpcCmdStatus);
        eError = OMX_ErrorUndefined;
      }
    }
    if (pComponentPrivate->bufferList[count].pBufHeader)
    {
      Memory_free (OmxProxy_Module_heap (),
                   pComponentPrivate->bufferList[count].pBufHeader,
                   sizeof (OMX_BUFFERHEADERTYPE));
      pComponentPrivate->bufferList[count].pBufHeader = NULL;
    }
  }
  if (OMX_ErrorNone == eError)
  {

    eError = omxproxy_free_proxyinit_rsrc (pHandle,
                                           OMXPROXY_RSRC_PROXYINIT_ALL);
    DOMX_UTL_TRACE_FUNCTION_ASSERT ((OmxRpc_errorNone == eError),
                                    "omxproxy_free_proxyinit_rsrc returned error");
  }
  OMXPROXY_MEMLOG_FREE_END (proxyAllocSize,
                            DOMXPROXY_MEMUSAGEPHASE_PROXYCREATE);
  return eError;
}

/**
 * @name   omxproxy_setcallbacks
 * @brief  DOMX proxy implementation of component setCallback
 * @param  pComponent         : Handle to the component
 * @param  pCallBacks         : pointer to callback context
 * @param  pAppData           : pointer to app data 
 * @return OMX_ErrorNone : SUCCESS
 */
static OMX_ERRORTYPE omxproxy_setcallbacks (OMX_HANDLETYPE pComponent,
                                            OMX_CALLBACKTYPE *pCallBacks,
                                            OMX_PTR pAppData)
{
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *) pComponent;
  PROXY_COMPONENT_PRIVATE *pComponentPrivate =
    (PROXY_COMPONENT_PRIVATE *) pHandle->pComponentPrivate;

  Log_print1 (Diags_ENTRY, "Entered : %s", (xdc_IArg) __FUNCTION__);

  if (pCallBacks == NULL)
  {
    eError = OMX_ErrorBadParameter;
    Log_print1 (Diags_USER1,
                "%d :: Received the empty callbacks from the application",
                __LINE__);
  }
  if (pAppData == NULL)
  {
    Log_print1 (Diags_USER1,
                "%d :: Received the empty Private data from the application\n",
                __LINE__);
    eError = OMX_ErrorBadParameter;
  }
  if (OMX_ErrorNone == eError)
  {
    /* Copy the callbacks of the application to the component private */
    memcpy (&(pComponentPrivate->cbInfo), pCallBacks,
            sizeof (OMX_CALLBACKTYPE));

    /* copy the application private data to component memory */
    pHandle->pApplicationPrivate = pAppData;

    /* May not be required to maintain the state */
    pComponentPrivate->eState = OMX_StateLoaded;
  }
  return eError;
}

/**
 * @name   omxproxy_use_buffer
 * @brief  DOMX proxy implementation of component use buffer 
 * @param  hComponent         : Handle to the component
 * @param  ppBufferHdr        : Pointer to pointer to OMX buf header
 * @param  nPortIndex         : Port index 
 * @param  pAppPrivate        : app private pointer to be passed in callback  
 * @param  nSizeBytes         : size of buffer in bytes
 * @param  pBuffer            : Pointer to data buffer
 * @return OMX_ErrorNone : SUCCESS
 */
static OMX_ERRORTYPE omxproxy_use_buffer (OMX_IN OMX_HANDLETYPE hComponent,
                                          OMX_INOUT OMX_BUFFERHEADERTYPE
                                            **ppBufferHdr,
                                          OMX_IN OMX_U32 nPortIndex,
                                          OMX_IN OMX_PTR pAppPrivate,
                                          OMX_IN OMX_U32 nSizeBytes,
                                          OMX_IN OMX_U8 *pBuffer)
{
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *) hComponent;
  PROXY_COMPONENT_PRIVATE *pComponentPrivate =
    (PROXY_COMPONENT_PRIVATE *) pHandle->pComponentPrivate;
  OMX_BUFFERHEADERTYPE *pBufferHeader = NULL;
  OMX_BUFFERHEADERTYPE *pBufHeaderRemote = NULL;
  OMX_U8 *pBufferMapped = NULL;
  OmxRpc_errorType rpcError = OmxRpc_errorNone;
  UInt32 currentBuffer = pComponentPrivate->numOfBuffers;
  Error_Block eb;
  int count = 0;
  Bool bufHdrAlloced = FALSE;
  Bool isMatchFound = FALSE;

  Log_print1 (Diags_ENTRY, "Entered function :%s", (xdc_IArg) __FUNCTION__);
  Error_init (&eb);

  DOMX_UTL_TRACE_FUNCTION_ASSERT ((currentBuffer < 
                                   pComponentPrivate->maxNumBuffers),
                                  "curBuf >= maxNumBuf");
  /* if max number of proxy buffers exceeded, return error */
  if (currentBuffer >= pComponentPrivate->maxNumBuffers)
  {
    return OMX_ErrorInsufficientResources;
  }
  pBufferHeader =
    Memory_calloc (OmxProxy_Module_heap (),
                   sizeof (OMX_BUFFERHEADERTYPE), sizeof (Ptr), &eb);
  if (Error_check (&eb))
  {
    Log_print3 (Diags_USER1,
                "%s:%d::Memory alloc failed for ProxyComponentPrivate."
                "Alloc Size = (%d)", (xdc_IArg) __FUNCTION__,
                (xdc_IArg) __LINE__,
                (xdc_IArg) sizeof (PROXY_COMPONENT_PRIVATE));
    eError = OMX_ErrorInsufficientResources;
  }
  else
  {
    bufHdrAlloced = TRUE;
  }
  if (OMX_ErrorNone == eError)
  {
    rpcError = OmxRpc_stubUseBuffer (pComponentPrivate->rpcHandle,
                                     (OmxCore_OMX_BUFFERHEADERTYPE *)
                                     pBufferHeader, nPortIndex, pAppPrivate,
                                     nSizeBytes, pBuffer,
                                     (OmxTypes_OMX_U32 *) &pBufferMapped,
                                     (OmxCore_OMX_BUFFERHEADERTYPE **)
                                     &pBufHeaderRemote,
                                     (OmxCore_OMX_ERRORTYPE *) & eError);
    if (OmxRpc_errorNone != rpcError)
    {
      Log_print3 (Diags_USER1, "%s:%d::OmxRpc_stubUseBuffer failed."
                  "ErrorId: %d", (xdc_IArg) __FILE__, __LINE__, rpcError);
      eError = OMX_ErrorUndefined;
    }
    else
    {
      if (OMX_ErrorNone == eError)
      {
        Log_print3 (Diags_USER1,
                    "%s:Use Buffer Successful"
                    "Value of pBufHeaderRemote: 0x%x"
                    "LocalBufferHdr :0x%x",
                    (xdc_IArg) __FUNCTION__,
                    (xdc_IArg) pBufHeaderRemote, (xdc_IArg) pBufferHeader);
      }
    }                           /* end of else of if (OmxRpc_errorNone !=
                                   rpcError) */
  }                             /* end of OmxRpc_stubUseBuffer invocation
                                   block */
  if (OMX_ErrorNone == eError)
  {
    for (count = 0; count < pComponentPrivate->maxNumBuffers; count++)
    {
      /* TODO: Check if original check is still reuired
         (pComponentPrivate->bufferList[count].pBufferMapped == pBufferMapped) */
      if (pComponentPrivate->bufferList[count].pBufHeader == NULL)
      {
        Log_print2 (Diags_USER1, "%s: Buffer Index of Match %d \n",
                    (xdc_IArg) __FUNCTION__, count);
        isMatchFound = TRUE;
        break;
      }
    }
    if (!isMatchFound)
    {
      Log_print1 (Diags_USER1,
                  "%s: Could not find a free slot in bufferList",
                  (xdc_IArg) __FUNCTION__);
      eError = OMX_ErrorBadParameter;
    }
    if (OMX_ErrorNone == eError) {
    /* Storing details of pBufferHeader/Mapped/Actual buffer address locally. */
    pComponentPrivate->bufferList[count].pBufHeader = pBufferHeader;
    pComponentPrivate->bufferList[count].pBufHeaderRemote =
      pBufHeaderRemote;
    pComponentPrivate->bufferList[count].pBufferMapped = pBufferMapped;
    pComponentPrivate->bufferList[count].pBufferActual = pBuffer;

    /* replacing the pBuffer with local memory address space buffer */
    pBufferHeader->pBuffer = pBuffer;

    /* keeping track of number of Buffers */
    pComponentPrivate->numOfBuffers++;
    *ppBufferHdr = pBufferHeader;
    }
  }
  if ((OMX_ErrorNone != eError) && (TRUE == bufHdrAlloced))
  {
    Memory_free (OmxProxy_Module_heap (), pBufferHeader,
                 sizeof (OMX_BUFFERHEADERTYPE));
  }
  return eError;
}

/**
 * @name   omxproxy_alloc_buffer
 * @brief  DOMX proxy implementation of component alloc buffer 
 * @param  hComponent         : Handle to the component
 * @param  ppBufferHdr        : Pointer to pointer to OMX buf header
 * @param  nPortIndex         : Port index 
 * @param  pAppPrivate        : app private pointer to be passed in callback  
 * @param  nSizeBytes         : size of buffer in bytes
 * @return OMX_ErrorNone : SUCCESS
 */
static OMX_ERRORTYPE omxproxy_alloc_buffer (OMX_IN OMX_HANDLETYPE hComponent,
                                            OMX_INOUT OMX_BUFFERHEADERTYPE
                                              **ppBufferHdr,
                                            OMX_IN OMX_U32 nPortIndex,
                                            OMX_IN OMX_PTR pAppPrivate,
                                            OMX_IN OMX_U32 nSizeBytes)
{
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *) hComponent;
  PROXY_COMPONENT_PRIVATE *pComponentPrivate =
    (PROXY_COMPONENT_PRIVATE *) pHandle->pComponentPrivate;
  OMX_BUFFERHEADERTYPE *pBufferHeader = NULL;
  OMX_BUFFERHEADERTYPE *pBufHeaderRemote = NULL;
  OMX_U8 *pBufferMapped = NULL;
  OMX_U8 *pBuffer = NULL;
  OmxRpc_errorType rpcError = OmxRpc_errorNone;
  UInt32 currentBuffer = pComponentPrivate->numOfBuffers;
  Error_Block eb;
  int count = 0;
  Bool bufHdrAlloced = FALSE;
  Bool isMatchFound = FALSE;

  Log_print1 (Diags_ENTRY, "Entered function :%s", (xdc_IArg) __FUNCTION__);
  Error_init (&eb);

  DOMX_UTL_TRACE_FUNCTION_ASSERT ((currentBuffer <
                                   pComponentPrivate->maxNumBuffers),
                                  "curBuf >= maxNumBuf");
  /* if max number of proxy buffers exceeded, retrun error */
  if (currentBuffer >= pComponentPrivate->maxNumBuffers)
  {
    return OMX_ErrorInsufficientResources;
  }
  pBufferHeader =
    Memory_calloc (OmxProxy_Module_heap (),
                   sizeof (OMX_BUFFERHEADERTYPE), sizeof (Ptr), &eb);
  if (Error_check (&eb))
  {
    Log_print3 (Diags_USER1,
                "%s:%d::Memory alloc failed for ProxyComponentPrivate."
                "Alloc Size = (%d)", (xdc_IArg) __FUNCTION__,
                (xdc_IArg) __LINE__,
                (xdc_IArg) sizeof (PROXY_COMPONENT_PRIVATE));
    eError = OMX_ErrorInsufficientResources;
  }
  else
  {
    bufHdrAlloced = TRUE;
  }
  if (OMX_ErrorNone == eError)
  {
    rpcError = OmxRpc_stubAllocBuffer (pComponentPrivate->rpcHandle,
                                       (OmxCore_OMX_BUFFERHEADERTYPE *)
                                       pBufferHeader, nPortIndex, pAppPrivate,
                                       nSizeBytes,
                                       (OmxTypes_OMX_U8 **) & pBuffer,
                                       (OmxTypes_OMX_U32 *) & pBufferMapped,
                                       (OmxCore_OMX_BUFFERHEADERTYPE **) &
                                       pBufHeaderRemote,
                                       (OmxCore_OMX_ERRORTYPE *) & eError);
    if (OmxRpc_errorNone != rpcError)
    {
      Log_print3 (Diags_USER1, "%s:%d::OmxRpc_stubAllocBuffer failed."
                  "ErrorId: %d", (xdc_IArg) __FILE__, __LINE__, rpcError);
      eError = OMX_ErrorUndefined;
    }
    else
    {
      if (OMX_ErrorNone == eError)
      {
        Log_print5 (Diags_USER1,
                    "%s:Alloc Buffer Successful"
                    "Value of pBufHeaderRemote: 0x%x"
                    "LocalBufferHdr :0x%x"
                    "Remote Buf Ptr :0x%x"
                    "Local  Buf Ptr :0x%x",
                    (xdc_IArg) __FUNCTION__,
                    (xdc_IArg) pBufHeaderRemote,
                    (xdc_IArg) pBufferHeader,
                    (xdc_IArg) pBufferMapped, (xdc_IArg) pBuffer);
      }
    }                           /* end of else of if (OmxRpc_errorNone !=
                                   rpcError) */
  }                             /* end of OmxRpc_stubUseBuffer invocation
                                   block */
  if (OMX_ErrorNone == eError)
  {
    for (count = 0; count < pComponentPrivate->maxNumBuffers; count++)
    {
      /* TODO: Check if original check is still reuired
         (pComponentPrivate->bufferList[count].pBufferMapped == pBufferMapped) */
      if (pComponentPrivate->bufferList[count].pBufHeader == NULL)
      {
        Log_print2 (Diags_USER1, "%s: Buffer Index of Match %d \n",
                    (xdc_IArg) __FUNCTION__, count);
        isMatchFound = TRUE;
        break;
      }
    }
    if (!isMatchFound)
    {
      Log_print1 (Diags_USER1,
                  "%s: Could not find a free slot in bufferList",
                  (xdc_IArg) __FUNCTION__);
      eError = OMX_ErrorBadParameter;
    }
    if (OMX_ErrorNone == eError) {
    /* Storing details of pBufferHeader/Mapped/Actual buffer address locally. */
    pComponentPrivate->bufferList[count].pBufHeader = pBufferHeader;
    pComponentPrivate->bufferList[count].pBufHeaderRemote =
      pBufHeaderRemote;
    pComponentPrivate->bufferList[count].pBufferMapped = pBufferMapped;
    pComponentPrivate->bufferList[count].pBufferActual = pBuffer;

    /* replacing the pBuffer with local memory address space buffer */
    pBufferHeader->pBuffer = pBuffer;

    /* keeping track of number of Buffers */
    pComponentPrivate->numOfBuffers++;
    *ppBufferHdr = pBufferHeader;
    }
  }
  if ((OMX_ErrorNone != eError) && (TRUE == bufHdrAlloced))
  {
    Memory_free (OmxProxy_Module_heap (), pBufferHeader,
                 sizeof (OMX_BUFFERHEADERTYPE));
  }
  return eError;
}

/**
 * @name   omxproxy_etb
 * @brief  DOMX proxy implementation of component empty this buffer
 * @param  hComponent         : Handle to the component
 * @param  ppBufferHdr        : Pointer to OMX buf header
 * @param  cmdId              : OmxRpc command ID
 * @return OMX_ErrorNone : SUCCESS
 */
static OMX_ERRORTYPE omxproxy_etb (OMX_HANDLETYPE hComponent,
                                   OMX_BUFFERHEADERTYPE *pBufferHdr,
                                   OmxRpc_cmd cmdId)
{
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  OmxRpc_errorType rpcError = OmxRpc_errorNone;
  OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *) hComponent;
  PROXY_COMPONENT_PRIVATE *pComponentPrivate =
    (PROXY_COMPONENT_PRIVATE *) pHandle->pComponentPrivate;
  unsigned int count = 0;
  Bool isMatchFound = FALSE;

  /* This is check, but Ideally we should skip this.
   * These buffer calls should be independent of these loops in order to
   * decrease latency */
  for (count = 0; count < pComponentPrivate->numOfBuffers; count++)
  {
    /* TODO: Check if original check is still reuired
       (pComponentPrivate->bufferList[count].pBufferMapped == pBufferMapped) */
    if (pComponentPrivate->bufferList[count].pBufHeader == pBufferHdr)
    {
      Log_print2 (Diags_USER1, "%s: Buffer Index of Match %d",
                  (xdc_IArg) __FUNCTION__, count);
      isMatchFound = TRUE;
      break;
    }
  }
  if (!isMatchFound)
  {
    Log_print2 (Diags_USER1, "%s: Could not find the mapped address:0x%x",
                (xdc_IArg) __FUNCTION__, (xdc_IArg) pBufferHdr);
    return OMX_ErrorBadParameter;
  }

  Log_print3 (Diags_USER1,
              "%s:  pBufferHdr->pBuffer : 0x%x, pBufferHdr->nFilledLen : %d ",
              (xdc_IArg) __FUNCTION__, (xdc_IArg) pBufferHdr->pBuffer,
              pBufferHdr->nFilledLen);
  /* Flushing the buffer */
  rpcError =
    OmxRpc_stubFlushBuffer (pComponentPrivate->rpcHandle,
                            pBufferHdr->pBuffer, pBufferHdr->nAllocLen);
  if (OmxRpc_errorNone != rpcError)
  {
    Log_print3 (Diags_USER1, "%s:%d::OmxRpc_stubFlushBuffer failed."
                "ErrorId: %d", (xdc_IArg) __FILE__, __LINE__, rpcError);
    return OMX_ErrorUndefined;
  }
  /* Changing the local buffer address to remote mapped buffer address */
  pBufferHdr->pBuffer = pComponentPrivate->bufferList[count].pBufferMapped;
  rpcError =
    OmxRpc_stubETB (pComponentPrivate->rpcHandle,
                    (OmxCore_OMX_BUFFERHEADERTYPE *) pBufferHdr,
                    (OmxCore_OMX_BUFFERHEADERTYPE *) pComponentPrivate->
                    bufferList[count].pBufHeaderRemote, cmdId,
                    (OmxCore_OMX_ERRORTYPE *) & eError);
  if (OmxRpc_errorNone != rpcError)
  {
    Log_print3 (Diags_USER1, "%s:%d::OmxRpc_stubETBAndFTB failed."
                "ErrorId: %d", (xdc_IArg) __FILE__, __LINE__, rpcError);
    eError = OMX_ErrorUndefined;
  }
  /* changing back the local buffer address */
  pBufferHdr->pBuffer = pComponentPrivate->bufferList[count].pBufferActual;

  Log_print1 (Diags_EXIT, "%s: Exiting: ", (xdc_IArg) __FUNCTION__);
  return eError;
}

/**
 * @name   omxproxy_ftb
 * @brief  DOMX proxy implementation of component fill this buffer
 * @param  hComponent         : Handle to the component
 * @param  ppBufferHdr        : Pointer to OMX buf header
 * @param  cmdId              : OmxRpc command ID
 * @return OMX_ErrorNone : SUCCESS
 */
static OMX_ERRORTYPE omxproxy_ftb (OMX_HANDLETYPE hComponent,
                                   OMX_BUFFERHEADERTYPE *pBufferHdr,
                                   OmxRpc_cmd cmdId)
{
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  OmxRpc_errorType rpcError = OmxRpc_errorNone;
  OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *) hComponent;
  PROXY_COMPONENT_PRIVATE *pComponentPrivate =
    (PROXY_COMPONENT_PRIVATE *) pHandle->pComponentPrivate;
  unsigned int count = 0;
  Bool isMatchFound = FALSE;

  /* This is check, but Ideally we should skip this. 
     These buffer calls should be independent of these loops in order to
     decrease latency */
  for (count = 0; count < pComponentPrivate->numOfBuffers; count++)
  {
    /* TODO: Check if original check is still reuired
       (pComponentPrivate->bufferList[count].pBufferMapped == pBufferMapped) */
    if (pComponentPrivate->bufferList[count].pBufHeader == pBufferHdr)
    {
      Log_print2 (Diags_USER1, "%s: Buffer Index of Match %d",
                  (xdc_IArg) __FUNCTION__, count);
      isMatchFound = TRUE;
      break;
    }
  }
  if (!isMatchFound)
  {
    Log_print2 (Diags_USER1, "%s: Could not find the mapped address:0x%x",
                (xdc_IArg) __FUNCTION__, (xdc_IArg) pBufferHdr);
    return OMX_ErrorBadParameter;
  }

  Log_print3 (Diags_USER1,
              "%s:  pBufferHdr->pBuffer : 0x%x, pBufferHdr->nFilledLen : %d ",
              (xdc_IArg) __FUNCTION__, (xdc_IArg) pBufferHdr->pBuffer,
              pBufferHdr->nFilledLen);
  /* Flushing the buffer */
  rpcError =
    OmxRpc_stubFlushBuffer (pComponentPrivate->rpcHandle,
                            pBufferHdr->pBuffer, pBufferHdr->nAllocLen);
  if (OmxRpc_errorNone != rpcError)
  {
    Log_print3 (Diags_USER1, "%s:%d::OmxRpc_stubFlushBuffer failed."
                "ErrorId: %d", (xdc_IArg) __FILE__, __LINE__, rpcError);
    return OMX_ErrorUndefined;
  }
  /* Changing the local buffer address to remote mapped buffer address */
  pBufferHdr->pBuffer = pComponentPrivate->bufferList[count].pBufferMapped;
  rpcError =
    OmxRpc_stubFTB (pComponentPrivate->rpcHandle,
                    (OmxCore_OMX_BUFFERHEADERTYPE *) pBufferHdr,
                    (OmxCore_OMX_BUFFERHEADERTYPE *) pComponentPrivate->
                    bufferList[count].pBufHeaderRemote, cmdId,
                    (OmxCore_OMX_ERRORTYPE *) & eError);
  if (OmxRpc_errorNone != rpcError)
  {
    Log_print3 (Diags_USER1, "%s:%d::OmxRpc_stubETBAndFTB failed."
                "ErrorId: %d", (xdc_IArg) __FILE__, __LINE__, rpcError);
    eError = OMX_ErrorUndefined;
  }
  /* changing back the local buffer address */
  pBufferHdr->pBuffer = pComponentPrivate->bufferList[count].pBufferActual;

  Log_print1 (Diags_EXIT, "%s: Exiting: ", (xdc_IArg) __FUNCTION__);
  return eError;
}

/**
 * @name   omxproxy_common_ftb_etb
 * @brief  DOMX proxy common implementation of component etb & ftb
 * @param  hComponent         : Handle to the component
 * @param  ppBufferHdr        : Pointer to OMX buf header
 * @param  cmdId              : OmxRpc command ID
 * @return OMX_ErrorNone : SUCCESS
 */
#ifdef DOMX_OMXPROXY_INCLUDE_COMMON_FTB_ETB_FUNCTION
static OMX_ERRORTYPE omxproxy_common_ftb_etb (OMX_HANDLETYPE hComponent,
                                              OMX_BUFFERHEADERTYPE *pBufferHdr,
                                              OmxRpc_cmd cmdId)
{
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  OmxRpc_errorType rpcError = OmxRpc_errorNone;
  OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *) hComponent;
  PROXY_COMPONENT_PRIVATE *pComponentPrivate =
    (PROXY_COMPONENT_PRIVATE *) pHandle->pComponentPrivate;
  unsigned int count = 0;
  Bool isMatchFound = FALSE;

  /* This is check, but Ideally we should skip this.
     These buffer calls should be independent of these loops in order to
     decrease latency */
  for (count = 0; count < pComponentPrivate->numOfBuffers; count++)
  {
    /* TODO: Check if original check is still reuired
       (pComponentPrivate->bufferList[count].pBufferMapped == pBufferMapped) */
    if (pComponentPrivate->bufferList[count].pBufHeader == pBufferHdr)
    {
      Log_print2 (Diags_USER1, "%s: Buffer Index of Match %d",
                  (xdc_IArg) __FUNCTION__, count);
      isMatchFound = TRUE;
      break;
    }
  }
  if (!isMatchFound)
  {
    Log_print2 (Diags_USER1, "%s: Could not find the mapped address:0x%x",
                (xdc_IArg) __FUNCTION__, (xdc_IArg) pBufferHdr);
    return OMX_ErrorBadParameter;
  }

  Log_print3 (Diags_USER1,
              "%s:  pBufferHdr->pBuffer : 0x%x, pBufferHdr->nFilledLen : %d ",
              (xdc_IArg) __FUNCTION__, (xdc_IArg) pBufferHdr->pBuffer,
              pBufferHdr->nFilledLen);
  /* Flushing the buffer */
  rpcError =
    OmxRpc_stubFlushBuffer (pComponentPrivate->rpcHandle,
                            pBufferHdr->pBuffer, pBufferHdr->nAllocLen);
  if (OmxRpc_errorNone != rpcError)
  {
    Log_print3 (Diags_USER1, "%s:%d::OmxRpc_stubFlushBuffer failed."
                "ErrorId: %d", (xdc_IArg) __FILE__, __LINE__, rpcError);
    return OMX_ErrorUndefined;
  }
  /* Changing the local buffer address to remote mapped buffer address */
  pBufferHdr->pBuffer = pComponentPrivate->bufferList[count].pBufferMapped;
  rpcError =
    OmxRpc_stubETBAndFTB (pComponentPrivate->rpcHandle,
                          (OmxCore_OMX_BUFFERHEADERTYPE *) pBufferHdr,
                          (OmxCore_OMX_BUFFERHEADERTYPE *)
                          pComponentPrivate->bufferList[count].
                          pBufHeaderRemote, cmdId,
                          (OmxCore_OMX_ERRORTYPE *) & eError);
  if (OmxRpc_errorNone != rpcError)
  {
    Log_print3 (Diags_USER1, "%s:%d::OmxRpc_stubETBAndFTB failed."
                "ErrorId: %d", (xdc_IArg) __FILE__, __LINE__, rpcError);
    eError = OMX_ErrorUndefined;
  }
  /* changing back the local buffer address */
  pBufferHdr->pBuffer = pComponentPrivate->bufferList[count].pBufferActual;

  Log_print1 (Diags_EXIT, "%s: Exiting: ", (xdc_IArg) __FUNCTION__);
  return eError;
}
#endif

/**
 * @name   omxproxy_fill_this_buffer
 * @brief  DOMX proxy common implementation of component fill this buffer
 * @param  hComponent         : Handle to the component
 * @param  ppBufferHdr        : Pointer to OMX buf header
 * @return OMX_ErrorNone : SUCCESS
 */
static OMX_ERRORTYPE omxproxy_fill_this_buffer (OMX_HANDLETYPE hComponent,
                                                OMX_BUFFERHEADERTYPE 
                                                  *pBufferHdr)
{
  Log_print1 (Diags_ENTRY, "%s:Entered..", (xdc_IArg) __FUNCTION__);
  return omxproxy_ftb (hComponent, pBufferHdr, OmxRpc_cmdFillThisBuffer);
}

/**
 * @name   omxproxy_empty_this_buffer
 * @brief  DOMX proxy common implementation of component empty this buffer
 * @param  hComponent         : Handle to the component
 * @param  ppBufferHdr        : Pointer to OMX buf header
 * @return OMX_ErrorNone : SUCCESS
 */
static OMX_ERRORTYPE omxproxy_empty_this_buffer (OMX_HANDLETYPE hComponent,
                                                 OMX_BUFFERHEADERTYPE 
                                                   *pBufferHdr)
{
  Log_print1 (Diags_ENTRY, "%s:Entered..", (xdc_IArg) __FUNCTION__);
  return omxproxy_etb (hComponent, pBufferHdr, OmxRpc_cmdEmptyThisBuffer);
}

/**
 * @name   omxproxy_free_buffer
 * @brief  DOMX proxy component free buffer implementation
 * @param  hComponent         : Handle to the component
 * @param  nPortIndex         : PortIndex
 * @param  ppBufferHdr        : Pointer to OMX buf header
 * @return OMX_ErrorNone : SUCCESS
 */
static OMX_ERRORTYPE omxproxy_free_buffer (OMX_IN OMX_HANDLETYPE hComponent,
                                           OMX_IN OMX_U32 nPortIndex,
                                           OMX_IN OMX_BUFFERHEADERTYPE
                                             *pBufferHdr)
{
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  OmxRpc_errorType rpcError = OmxRpc_errorNone;
  PROXY_COMPONENT_PRIVATE *pComponentPrivate = NULL;
  OMX_COMPONENTTYPE *pHandle = hComponent;
  int count = 0;
  Bool isMatchFound = FALSE;

  Log_print1 (Diags_ENTRY, "Entered: %s\n", (xdc_IArg) __FUNCTION__);
  pComponentPrivate = (PROXY_COMPONENT_PRIVATE *) pHandle->pComponentPrivate;
  /* This is check, but Ideally we should skip this.
     These buffer calls should be independent of these loops in order to
     decrease latency */
  for (count = 0; count < pComponentPrivate->maxNumBuffers; count++)
  {
    /* TODO: Check if original check is still reuired
       (pComponentPrivate->bufferList[count].pBufferMapped == pBufferMapped) */
    if (pComponentPrivate->bufferList[count].pBufHeader == pBufferHdr)
    {
      Log_print2 (Diags_USER1, "%s: Buffer Index of Match %d \n",
                  (xdc_IArg) __FUNCTION__, count);
      isMatchFound = TRUE;
      break;
    }
  }
  if (!isMatchFound)
  {
    Log_print2 (Diags_USER1,
                "%s: Could not find the mapped Buffer for bufHdr:0x%x",
                (xdc_IArg) __FUNCTION__, (xdc_IArg) pBufferHdr);
    return OMX_ErrorBadParameter;
  }
  rpcError = OmxRpc_stubFreeBuffer (pComponentPrivate->rpcHandle,
                                    nPortIndex,
                                    (OmxCore_OMX_BUFFERHEADERTYPE *)
                                    pComponentPrivate->bufferList[count].
                                    pBufHeaderRemote,
                                    (OmxCore_OMX_ERRORTYPE *) & eError);
  if (rpcError == OmxRpc_errorNone)
  {
    Log_print2 (Diags_USER1, "%s:Free Buffer Successful.BufHdr = 0x%x",
                (xdc_IArg) __FUNCTION__, (xdc_IArg) pBufferHdr);
    /* Unmap any buffers previously mapped */
    /* TODO : Buffer unmap & buffer header free is done before free Handle. Check 
       if this is okay */
    {
      OmxRpc_errorType rpcCmdStatus;
      Log_print1 (Diags_USER1, "%s Cleaning up Buffers", (xdc_IArg) __FUNCTION__);
      if (pComponentPrivate->bufferList[count].pBufferMapped)
      {
        rpcCmdStatus = OmxRpc_stubUnMapBuffer (pComponentPrivate->rpcHandle,
                                               (UInt32) pComponentPrivate->
                                               bufferList[count].pBufferMapped);
        if (OmxRpc_errorNone != rpcCmdStatus)
        {
          Log_print4 (Diags_USER1, "%s:%d:: OmxRpc_stubUnmapBuffer failed"
                      "MappedAddr = %d, RpcErrorCode = %d",
                      (xdc_IArg) __FUNCTION__, __LINE__,
                      (xdc_IArg) pComponentPrivate->bufferList[count].
                      pBufferMapped, rpcCmdStatus);
          eError = OMX_ErrorUndefined;
        }
        if(eError == OMX_ErrorNone) {
        pComponentPrivate->bufferList[count].pBufferMapped = NULL;
        }
      }
      if (pComponentPrivate->bufferList[count].pBufHeader)
      {
        Memory_free (OmxProxy_Module_heap (),
                     pComponentPrivate->bufferList[count].pBufHeader,
                     sizeof (OMX_BUFFERHEADERTYPE));
        pComponentPrivate->bufferList[count].pBufHeader = NULL;
      }
    }
    pComponentPrivate->bufferList[count].pBufHeaderRemote = NULL;
    pComponentPrivate->bufferList[count].pBufferActual = NULL;
    /* keeping track of number of Buffers */
    pComponentPrivate->numOfBuffers--;
  }
  else
  {
    Log_print3 (Diags_USER1, "%s:%d::OmxRpc_stubFreeBuffer failed."
                "ErrorId: %d", (xdc_IArg) __FILE__, __LINE__, rpcError);
    eError = OMX_ErrorUndefined;
  }
  return eError;
}

/**
 * @name   omxproxy_set_parameter
 * @brief  DOMX proxy component set parameter implementation
 * @param  hComponent         : Handle to the component
 * @param  nParamIndex        : Parameter index 
 * @param  pComponentParameterStructure : Pointer to parameter structure
 * @return OMX_ErrorNone : SUCCESS
 */
static OMX_ERRORTYPE omxproxy_set_parameter (OMX_IN OMX_HANDLETYPE hComponent,
                                             OMX_IN OMX_INDEXTYPE nParamIndex,
                                             OMX_IN OMX_PTR
                                               pComponentParameterStructure)
{
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  PROXY_COMPONENT_PRIVATE *pComponentPrivate = NULL;
  OMX_COMPONENTTYPE *pHandle = hComponent;
  OmxRpc_errorType rpcError;
  SizeT sizeofParamStruct;

  Log_print4 (Diags_ENTRY, "Entered: %s (0x%x, %d, 0x%x)",
              (xdc_IArg) __FUNCTION__, (xdc_IArg) hComponent, nParamIndex,
              (xdc_IArg) pComponentParameterStructure);
  /* Checking the pointers */
  if (pComponentParameterStructure == NULL)
  {
    eError = OMX_ErrorBadParameter;
    goto EXIT;
  }

  pComponentPrivate = (PROXY_COMPONENT_PRIVATE *) pHandle->pComponentPrivate;
  sizeofParamStruct = 0;
  eError = omxproxy_get_omx_struct_size (nParamIndex, &sizeofParamStruct,
                                         pComponentPrivate->
                                         customParamStructInfo,
                                         pComponentParameterStructure);
  if (OMX_ErrorNone != eError)
  {
    eError = eError;
    goto EXIT;
  }
  rpcError = OmxRpc_stubSetParameter (pComponentPrivate->rpcHandle,
                                      (OmxIndex_OMX_INDEXTYPE) nParamIndex,
                                      pComponentParameterStructure,
                                      sizeofParamStruct,
                                      (OmxCore_OMX_ERRORTYPE *) & eError);
  if (rpcError == OmxRpc_errorNone)
  {
    Log_print2 (Diags_USER1,
                "%s:OmxRpc_stubSetParameter Successful.nParamIndex = %d",
                (xdc_IArg) __FUNCTION__, nParamIndex);
  }
  else
  {
    Log_print3 (Diags_USER1, "%s:%d::OmxRpc_stubSetParameter failed."
                "ErrorId: %d", (xdc_IArg) __FILE__, __LINE__, rpcError);
    eError = OMX_ErrorUndefined;
  }

EXIT:
  Log_print2 (Diags_EXIT, "Leaving %s returns %s\n",
              (xdc_IArg) __FUNCTION__,
              (xdc_IArg) DOMX_UTL_OmxErrorToString (eError));

  return eError;
}

/**
 * @name   omxproxy_get_parameter
 * @brief  DOMX proxy component get parameter implementation
 * @param  hComponent         : Handle to the component
 * @param  nParamIndex        : Parameter index 
 * @param  pComponentParameterStructure : Pointer to parameter structure
 * @return OMX_ErrorNone : SUCCESS
 */
static OMX_ERRORTYPE omxproxy_get_parameter (OMX_IN OMX_HANDLETYPE hComponent,
                                             OMX_IN OMX_INDEXTYPE nParamIndex,
                                             OMX_INOUT OMX_PTR
                                               pComponentParameterStructure)
{
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  PROXY_COMPONENT_PRIVATE *pComponentPrivate = NULL;
  OMX_COMPONENTTYPE *pHandle = hComponent;
  OmxRpc_errorType rpcError = OmxRpc_errorNone;
  SizeT sizeofParamStruct;

  Log_print1 (Diags_ENTRY, "Entered: %s", (xdc_IArg) __FUNCTION__);
  /* Checking the pointers */
  if (pComponentParameterStructure == NULL)
    return OMX_ErrorBadParameter;

  pComponentPrivate = (PROXY_COMPONENT_PRIVATE *) pHandle->pComponentPrivate;
  sizeofParamStruct = 0;
  eError = omxproxy_get_omx_struct_size (nParamIndex, &sizeofParamStruct,
                                         pComponentPrivate->
                                         customParamStructInfo,
                                         pComponentParameterStructure);
  if (OMX_ErrorNone != eError)
  {
    return eError;
  }
  rpcError = OmxRpc_stubGetParameter (pComponentPrivate->rpcHandle,
                                      (OmxIndex_OMX_INDEXTYPE) nParamIndex,
                                      pComponentParameterStructure,
                                      sizeofParamStruct,
                                      (OmxCore_OMX_ERRORTYPE *) & eError);

  if (rpcError == OmxRpc_errorNone)
  {
    Log_print2 (Diags_USER1,
                "%s:OmxRpc_stubGetParameter Successful.nParamIndex = %d",
                (xdc_IArg) __FUNCTION__, nParamIndex);
  }
  else
  {
    Log_print3 (Diags_USER1, "%s:%d::OmxRpc_stubGetParameter failed."
                "ErrorId: %d", (xdc_IArg) __FILE__, __LINE__, rpcError);
    eError = OMX_ErrorUndefined;
  }
  return eError;
}

/**
 * @name   omxproxy_get_config
 * @brief  DOMX proxy component get config implementation
 * @param  hComponent         : Handle to the component
 * @param  nParamIndex        : Config index 
 * @param  pComponentParameterStructure : Pointer to config structure
 * @return OMX_ErrorNone : SUCCESS
 */
static OMX_ERRORTYPE omxproxy_get_config (OMX_HANDLETYPE hComponent,
                                          OMX_INDEXTYPE nConfigIndex,
                                          OMX_PTR pComponentConfigStructure)
{
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  PROXY_COMPONENT_PRIVATE *pComponentPrivate = NULL;
  OMX_COMPONENTTYPE *pHandle = hComponent;
  OmxRpc_errorType rpcError = OmxRpc_errorNone;
  SizeT sizeofConfigStruct;

  Log_print4 (Diags_ENTRY, "Entered: %s(0x%x, %d, 0x%x)",
              (xdc_IArg) __FUNCTION__, (xdc_IArg) hComponent, nConfigIndex,
              (xdc_IArg) pComponentConfigStructure);
  /* Checking the pointers */
  if (pComponentConfigStructure == NULL)
    return OMX_ErrorBadParameter;

  switch (nConfigIndex)
  {
    case OMX_TI_IndexConfigGetDomxCompInfo:
      {
        OMX_CONFIG_DOMXPROXYCOMPINFO *pDomxCfgInfo = pComponentConfigStructure;

        TIMM_OSAL_Memset (pDomxCfgInfo, 0x0,
                          sizeof (OMX_CONFIG_DOMXPROXYCOMPINFO));
        pDomxCfgInfo->nSize = sizeof (OMX_CONFIG_DOMXPROXYCOMPINFO);
        pDomxCfgInfo->nVersion.s.nVersionMajor = OMX_VERSION_MAJOR;
        pDomxCfgInfo->nVersion.s.nVersionMinor = OMX_VERSION_MINOR;
        pDomxCfgInfo->nVersion.s.nRevision = OMX_VERSION_REVISION;
        pDomxCfgInfo->nVersion.s.nStep = OMX_VERSION_STEP;
        pComponentPrivate =
          (PROXY_COMPONENT_PRIVATE *) pHandle->pComponentPrivate;
        pDomxCfgInfo->pRpcStubHandle = pComponentPrivate->rpcHandle;
        pDomxCfgInfo->nRpcSkelPtr = pComponentPrivate->nRpcRemoteHandle;
        pDomxCfgInfo->hCompRealHandle =
          pComponentPrivate->pRemoteComponentHandle;
        strncpy ((char *) pDomxCfgInfo->cComponentName,
                 pComponentPrivate->szComponentName,
                 sizeof (pComponentPrivate));
        strncpy ((char *) pDomxCfgInfo->cComponentRcmSvrName,
                 pComponentPrivate->szComponentRcmServerName,
                 sizeof (pDomxCfgInfo->cComponentRcmSvrName));
        break;
      }
    default:
      pComponentPrivate =
        (PROXY_COMPONENT_PRIVATE *) pHandle->pComponentPrivate;
      sizeofConfigStruct = 0;
      eError = omxproxy_get_omx_struct_size (nConfigIndex, &sizeofConfigStruct,
                                             pComponentPrivate->
                                             customParamStructInfo,
                                             pComponentConfigStructure);
      if (OMX_ErrorNone != eError)
      {
        return eError;
      }
      rpcError = OmxRpc_stubGetConfig (pComponentPrivate->rpcHandle,
                                       (OmxIndex_OMX_INDEXTYPE) nConfigIndex,
                                       pComponentConfigStructure,
                                       sizeofConfigStruct,
                                       (OmxCore_OMX_ERRORTYPE *) & eError);
      if (rpcError == OmxRpc_errorNone)
      {
        Log_print2 (Diags_USER1,
                    "%s:OmxRpc_stubGetConfig Successful.nParamIndex = %d",
                    (xdc_IArg) __FUNCTION__, nConfigIndex);
      }
      else
      {
        Log_print3 (Diags_USER1, "%s:%d::OmxRpc_stubGetConfig failed."
                    "ErrorId: %d", (xdc_IArg) __FILE__, __LINE__, rpcError);
        eError = OMX_ErrorUndefined;
      }
  }                             /* end of switch (nConfigIndex) */

  Log_print2 (Diags_EXIT, "Leaving: %s, retVal: %d", (xdc_IArg) __FUNCTION__,
              eError);
  return eError;
}

/**
 * @name   omxproxy_set_config
 * @brief  DOMX proxy component set config implementation
 * @param  hComponent         : Handle to the component
 * @param  nParamIndex        : Config index 
 * @param  pComponentParameterStructure : Pointer to config structure
 * @return OMX_ErrorNone : SUCCESS
 */
static OMX_ERRORTYPE omxproxy_set_config (OMX_IN OMX_HANDLETYPE hComponent,
                                          OMX_IN OMX_INDEXTYPE nConfigIndex,
                                          OMX_IN OMX_PTR
                                            pComponentConfigStructure)
{
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  PROXY_COMPONENT_PRIVATE *pComponentPrivate = NULL;
  OMX_COMPONENTTYPE *pHandle = hComponent;
  OmxRpc_errorType rpcError = OmxRpc_errorNone;
  SizeT sizeofConfigStruct;

  Log_print4 (Diags_ENTRY, "Entered: %s (0x%x, %d, 0x%x)",
              (xdc_IArg) __FUNCTION__, (xdc_IArg) hComponent, nConfigIndex,
              (xdc_IArg) pComponentConfigStructure);

  /* Checking the pointers */
  if (pComponentConfigStructure == NULL)
  {
    eError = OMX_ErrorBadParameter;
    goto EXIT;
  }

  pComponentPrivate = (PROXY_COMPONENT_PRIVATE *) pHandle->pComponentPrivate;
  sizeofConfigStruct = 0;
  eError = omxproxy_get_omx_struct_size (nConfigIndex, &sizeofConfigStruct,
                                         pComponentPrivate->
                                         customParamStructInfo,
                                         pComponentConfigStructure);
  if (OMX_ErrorNone != eError)
  {
    goto EXIT;
  }
  rpcError = OmxRpc_stubSetConfig (pComponentPrivate->rpcHandle,
                                   (OmxIndex_OMX_INDEXTYPE) nConfigIndex,
                                   pComponentConfigStructure,
                                   sizeofConfigStruct,
                                   (OmxCore_OMX_ERRORTYPE *) & eError);
  if (rpcError == OmxRpc_errorNone)
  {
    Log_print2 (Diags_USER1,
                "%s:OmxRpc_stubGetConfig Successful.nParamIndex = %d",
                (xdc_IArg) __FUNCTION__, nConfigIndex);
  }
  else
  {
    Log_print3 (Diags_USER1, "%s:%d::OmxRpc_stubGetConfig failed."
                "ErrorId: %d", (xdc_IArg) __FILE__, __LINE__, rpcError);
    eError = OMX_ErrorUndefined;
  }

EXIT:
  Log_print2 (Diags_EXIT, "Leaving %s returns %s\n",
              (xdc_IArg) __FUNCTION__,
              (xdc_IArg) DOMX_UTL_OmxErrorToString (eError));
  return eError;
}

/**
 * @name   omxproxy_get_state
 * @brief  DOMX proxy component get state implementation
 * @param  hComponent         : Handle to the component
 * @param  pState             : Pointer to state enum to be set
 * @return OMX_ErrorNone : SUCCESS
 */
static OMX_ERRORTYPE omxproxy_get_state (OMX_IN OMX_HANDLETYPE hComponent,
                                         OMX_OUT OMX_STATETYPE *pState)
{
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  OmxRpc_errorType rpcError = OmxRpc_errorNone;
  OMX_COMPONENTTYPE *pHandle = hComponent;
  PROXY_COMPONENT_PRIVATE *pComponentPrivate =
    (PROXY_COMPONENT_PRIVATE *) pHandle->pComponentPrivate;

  Log_print3 (Diags_ENTRY, "Entered: %s (0x%x, 0x%x)",
              (xdc_IArg) __FUNCTION__, (xdc_IArg) hComponent,
              (xdc_IArg) pState);
  /* Checking the pointers */
  if (pState == NULL)
    return OMX_ErrorBadParameter;

  rpcError = OmxRpc_stubGetState (pComponentPrivate->rpcHandle,
                                  (OmxCore_OMX_STATETYPE *) pState,
                                  (OmxCore_OMX_ERRORTYPE *) & eError);
  Log_print1 (Diags_USER1, "%s: Returned from OmxRpc_stubGetState",
              (xdc_IArg) __FUNCTION__);
  if (rpcError == OmxRpc_errorNone)
  {
    Log_print1 (Diags_USER1, "%s:OmxRpc_stubGetState Successful.nParamIndex",
                (xdc_IArg) __FUNCTION__);
  }
  else
  {
    Log_print3 (Diags_USER1, "%s:%d::OmxRpc_stubGetState failed."
                "ErrorId: %d", (xdc_IArg) __FILE__, __LINE__, rpcError);
    eError = OMX_ErrorUndefined;
  }

  Log_print2 (Diags_EXIT, "Leaving: %s return %s\n",
              (xdc_IArg) __FUNCTION__,
              (xdc_IArg) DOMX_UTL_OmxErrorToString (eError));
  return eError;
}

/**
 * @name   omxproxy_send_command
 * @brief  DOMX proxy component send command implementation
 * @param  hComponent         : Handle to the component
 * @param  eCmd               : COmmand to be sent
 * @param  nParam             : Command param
 * @param  pCmdData           : Pointer to command data
 * @return OMX_ErrorNone : SUCCESS
 */
static OMX_ERRORTYPE omxproxy_send_command (OMX_IN OMX_HANDLETYPE hComponent,
                                            OMX_IN OMX_COMMANDTYPE eCmd,
                                            OMX_IN OMX_U32 nParam, 
                                            OMX_IN OMX_PTR pCmdData)
{
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  OmxRpc_errorType rpcError = OmxRpc_errorNone;
  PROXY_COMPONENT_PRIVATE *pComponentPrivate = NULL;
  OMX_COMPONENTTYPE *pHandle = hComponent;

  Log_print5 (Diags_ENTRY, "Entered: %s (0x%x, %d, %d, 0x%x)",
              (xdc_IArg) __FUNCTION__, (xdc_IArg) hComponent, eCmd, nParam,
              (xdc_IArg) pCmdData);
  pComponentPrivate = (PROXY_COMPONENT_PRIVATE *) pHandle->pComponentPrivate;
  rpcError = OmxRpc_stubSendCommand (pComponentPrivate->rpcHandle,
                                     (OmxCore_OMX_COMMANDTYPE) eCmd,
                                     nParam,
                                     pCmdData,
                                     (OmxCore_OMX_ERRORTYPE *) & eError);

  if (rpcError == OmxRpc_errorNone)
  {
    Log_print2 (Diags_USER1, "%s:OmxRpc_stubSendCommand Successful.Cmd = %d",
                (xdc_IArg) __FUNCTION__, eCmd);
  }
  else
  {
    Log_print3 (Diags_USER1, "%s:%d::OmxRpc_stubSendCommand failed."
                "ErrorId: %d", (xdc_IArg) __FILE__, __LINE__, rpcError);
    eError = OMX_ErrorUndefined;
  }

  Log_print2 (Diags_EXIT, "Leaving: %s return %s\n",
              (xdc_IArg) __FUNCTION__,
              (xdc_IArg) DOMX_UTL_OmxErrorToString (eError));

  return eError;
}

/**
 * @name   omxproxy_get_component_version
 * @brief  DOMX proxy component get component version implementation
 * @param  hComponent         : Handle to the component
 * @param  pComponentName     : Component name
 * @param  pComponentVersion  : Ptr to component version structure
 * @param  pSpecVersion       : Ptr to component version structure
 * @param  pComponentUUID     : Ptr to component uuid structure
 * @return OMX_ErrorNone : SUCCESS
 */
static OMX_ERRORTYPE omxproxy_get_component_version (OMX_IN OMX_HANDLETYPE
                                                       hComponent,
                                                     OMX_OUT OMX_STRING
                                                       pComponentName,
                                                     OMX_OUT OMX_VERSIONTYPE
                                                       *pComponentVersion,
                                                     OMX_OUT OMX_VERSIONTYPE
                                                       *pSpecVersion,
                                                     OMX_OUT OMX_UUIDTYPE 
                                                       *pComponentUUID)
{
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  PROXY_COMPONENT_PRIVATE *pComponentPrivate = NULL;
  OMX_COMPONENTTYPE *pHandle = hComponent;
  OmxRpc_errorType rpcError;

  Log_print1 (Diags_ENTRY, "Entered: %s", (xdc_IArg) __FUNCTION__);
  pComponentPrivate = (PROXY_COMPONENT_PRIVATE *) pHandle->pComponentPrivate;

 /***call RPC****/
  rpcError = OmxRpc_stubGetComponentVersion (pComponentPrivate->rpcHandle,
                                             pComponentName,
                                             (OmxTypes_OMX_VERSIONTYPE *)
                                             pComponentVersion,
                                             (OmxTypes_OMX_VERSIONTYPE *)
                                             pSpecVersion, pComponentUUID,
                                             (OmxCore_OMX_ERRORTYPE *) &
                                             eError);
  if (rpcError == OmxRpc_errorNone)
  {
    Log_print2 (Diags_USER1,
                "%s:OmxRpc_stubGetComponentVersion Successful.ComponentName = %s",
                (xdc_IArg) __FUNCTION__, (xdc_IArg) pComponentName);
  }
  else
  {
    Log_print3 (Diags_USER1, "%s:%d::OmxRpc_stubGetComponentVersion failed."
                "ErrorId: %d", (xdc_IArg) __FILE__, __LINE__, rpcError);
    eError = OMX_ErrorUndefined;
  }
  return eError;
}

/**
 * @name   omxproxy_get_extension_index
 * @brief  DOMX proxy component get extension index implementation
 * @param  hComponent         : Handle to the component
 * @param  cParameterName     : Parameter name
 * @param  pIndexType         : Ptr to index type
 * @return OMX_ErrorNone : SUCCESS
 */
static OMX_ERRORTYPE omxproxy_get_extension_index (OMX_IN OMX_HANDLETYPE
                                                     hComponent,
                                                   OMX_IN OMX_STRING
                                                     cParameterName,
                                                   OMX_OUT OMX_INDEXTYPE
                                                     *pIndexType)
{
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  PROXY_COMPONENT_PRIVATE *pComponentPrivate = NULL;
  OMX_COMPONENTTYPE *pHandle = hComponent;
  OmxRpc_errorType rpcError;

  Log_print1 (Diags_ENTRY, "Entered: %s", (xdc_IArg) __FUNCTION__);
  /* Checking the pointers */
  if (pIndexType == NULL)
    return OMX_ErrorBadParameter;
  pComponentPrivate = (PROXY_COMPONENT_PRIVATE *) pHandle->pComponentPrivate;

 /***call RPC****/
  rpcError = OmxRpc_stubGetExtensionIndex (pComponentPrivate->rpcHandle,
                                           cParameterName,
                                           (OmxIndex_OMX_INDEXTYPE *)
                                           pIndexType,
                                           (OmxCore_OMX_ERRORTYPE *) & eError);

  if (rpcError == OmxRpc_errorNone)
  {
    Log_print2 (Diags_USER1,
                "%s:OmxRpc_stubGetExtensionIndex Successful.ParamName = %s",
                (xdc_IArg) __FUNCTION__, (xdc_IArg) cParameterName);
  }
  else
  {
    Log_print3 (Diags_USER1, "%s:%d::OmxRpc_stubGetExtensionIndex failed."
                "ErrorId: %d", (xdc_IArg) __FILE__, __LINE__, rpcError);
    eError = OMX_ErrorUndefined;
  }
  return eError;
}

/**
 * @name   omxproxy_component_role_enum
 * @brief  DOMX proxy component get component role enum implementation
 * @param  hComponent         : Handle to the component
 * @param  cRole              : role name
 * @param  nIndex             : 
 * @return OMX_ErrorNone : SUCCESS
 */
static OMX_ERRORTYPE omxproxy_component_role_enum (OMX_IN OMX_HANDLETYPE
                                                     hComponent,
                                                   OMX_OUT OMX_U8 *cRole,
                                                   OMX_IN OMX_U32 nIndex)
{
  OMX_ERRORTYPE eError = OMX_ErrorNone;

  (Void) hComponent;            /* Kill warnings */
  (Void) cRole;                 /* Kill warnings */
  (Void) nIndex;                /* Kill warnings */
  return eError;
}

/**
 * @name   omxproxy_component_tunnel_request
 * @brief  DOMX proxy component component tunnel request implementation
 * @param  hComponent         : Handle to the component
 * @param  nPort              : Port index to tunnel
 * @param  hTunneledComp      : Handle to the tunneled component
 * @param  nTunneledPort      : Port index of tunneled component
 * @param  pTunnelSetup       : Pointer to tunnel setup structure
 * @return OMX_ErrorNone : SUCCESS
 */
static OMX_ERRORTYPE
  omxproxy_component_tunnel_request (OMX_IN OMX_HANDLETYPE hComponent,
                                     OMX_IN OMX_U32 nPort,
                                     OMX_IN OMX_HANDLETYPE hTunneledComp,
                                     OMX_IN OMX_U32 nTunneledPort,
                                     OMX_INOUT OMX_TUNNELSETUPTYPE 
                                       *pTunnelSetup)
{
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  PROXY_COMPONENT_PRIVATE *pComponentPrivate = NULL;
  OMX_COMPONENTTYPE *pHandle = hComponent;
  OmxRpc_errorType rpcError;
  OmxCore_OMX_TUNNELSETUPTYPE sRpcTunnelSetup;
  OmxCore_OMX_TUNNELSETUPTYPE *pRpcTunnelSetup;

  Log_print6 (Diags_ENTRY, "Entered: %s (0x%x, %d, 0x%x, %d, 0x%x",
              (xdc_IArg) __FUNCTION__, (xdc_IArg) hComponent, nPort,
              (xdc_IArg) hTunneledComp, nTunneledPort, (xdc_IArg) pTunnelSetup);
  /* Checking the pointers */
  if (hComponent == NULL)
    return OMX_ErrorBadParameter;
  pComponentPrivate = (PROXY_COMPONENT_PRIVATE *) pHandle->pComponentPrivate;

  /***call RPC****/

  if (pTunnelSetup != NULL)
  {
    sRpcTunnelSetup.nTunnelFlags = pTunnelSetup->nTunnelFlags;
    sRpcTunnelSetup.eSupplier =
      (OmxCore_OMX_BUFFERSUPPLIERTYPE) pTunnelSetup->eSupplier;
    pRpcTunnelSetup = &sRpcTunnelSetup;
  }
  else
  {
    pRpcTunnelSetup = NULL;
  }
  rpcError = OmxRpc_stubComponentTunnelRequest (pComponentPrivate->rpcHandle,
                                                nPort,
                                                hTunneledComp,
                                                nTunneledPort,
                                                pRpcTunnelSetup,
                                                (OmxCore_OMX_ERRORTYPE *) &
                                                eError);

  if (rpcError == OmxRpc_errorNone)
  {
    Log_print2 (Diags_USER1,
                "%s:OmxRpc_stubComponentTunnelRequest Successful.TunneledPtr = %p",
                (xdc_IArg) __FUNCTION__, (xdc_IArg) hTunneledComp);
    if (NULL != pTunnelSetup)
    {
      pTunnelSetup->nTunnelFlags = pRpcTunnelSetup->nTunnelFlags;
      pTunnelSetup->eSupplier =
        (OMX_BUFFERSUPPLIERTYPE) pRpcTunnelSetup->eSupplier;
    }
  }
  else
  {
    Log_print3 (Diags_USER1, "%s:%d::OmxRpc_stubGetExtensionIndex failed."
                "ErrorId: %d", (xdc_IArg) __FILE__, __LINE__, rpcError);
    eError = OMX_ErrorUndefined;
  }
  return eError;
}

/**
 * @name   OmxProxy_commonInit
 * @brief  DOMX proxy component init implementation
 * @param  hComponent         : Handle to the component
 * @param  cComponentName     : Component name
 * @return OMX_ErrorNone : SUCCESS
 */
OMX_ERRORTYPE OmxProxy_commonInit (OMX_HANDLETYPE hComponent,
                                   OMX_STRING cComponentName)
{
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  OmxRpc_errorType rpcCmdStatus = OmxRpc_errorNone;
  OMX_COMPONENTTYPE *pHandle = NULL;
  PROXY_COMPONENT_PRIVATE *pComponentPrivate;
  OmxRpc_Params omxRpcParams;
  Error_Block eb;
  OmxRpc_Handle rpcStubHandle = NULL;
  DomxCore_componentCoreInfoEntry *compInfo = NULL;
  OmxRpc_callBack rpcCb = { omxproxy_event_handler,
    omxproxy_empty_buffer_done,
    omxproxy_fill_buffer_done
  };
  Int rsrcMask = OMXPROXY_RSRC_PROXYINIT_NONE;

  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();
  DOMX_UTL_TRACE_FUNCTION_MSG (cComponentName);

  OMXPROXY_MEMLOG_USED_START ();
  pHandle = (OMX_COMPONENTTYPE *) hComponent;
  eError = omxproxy_map_component_name2info (cComponentName, &compInfo);
  DOMX_UTL_TRACE_FUNCTION_ASSERT ((OMX_ErrorNone == eError),
                                  "Component not found");

  /* Must initialize before use */
  Error_init (&eb);

  pHandle->pComponentPrivate =
    (PROXY_COMPONENT_PRIVATE *) Memory_alloc (OmxProxy_Module_heap (),
                                              sizeof
                                              (PROXY_COMPONENT_PRIVATE),
                                              sizeof (Ptr), &eb);
  if (Error_check (&eb))
  {
    Log_print3 (Diags_USER1,
                "%s:%d::Memory alloc failed for ProxyComponentPrivate."
                "Alloc Size = (%d)", (xdc_IArg) __FUNCTION__,
                (xdc_IArg) __LINE__,
                (xdc_IArg) sizeof (PROXY_COMPONENT_PRIVATE));
    eError = OMX_ErrorInsufficientResources;
  }

  DOMX_UTL_TRACE_FUNCTION_ASSERT ((eError == OMX_ErrorNone),
                                  "Memory alloc failed for ProxyCompPrivate");

  if (OMX_ErrorNone == eError)
  {
    rsrcMask |= OMXPROXY_RSRC_PROXYINIT_COMPPRIVATE_ALLOC;
    pComponentPrivate = (PROXY_COMPONENT_PRIVATE *) pHandle->pComponentPrivate;
    /* Setting its core information in private data */
    pComponentPrivate->realCoreId = compInfo->coreId;
    /* Copy component name into private data */
    strncpy (pComponentPrivate->szComponentName, cComponentName,
             OMX_MAX_STRINGNAME_SIZE);
    pComponentPrivate->numOfBuffers = 0;
    pComponentPrivate->maxNumBuffers = compInfo->maxNumBuffers;
    pComponentPrivate->bufferList =
      (OmxProxy_proxyBufferInfo *) Memory_alloc (OmxProxy_Module_heap (),
                                                 (sizeof
                                                  (OmxProxy_proxyBufferInfo) *
                                                  pComponentPrivate->
                                                  maxNumBuffers), sizeof (Ptr),
                                                 &eb);
    if (Error_check (&eb))
    {
      Log_print3 (Diags_USER1, "%s:%d::Memory alloc failed for bufferList."
                  "Alloc Size = (%d)",
                  (xdc_IArg) __FUNCTION__,
                  (xdc_IArg) __LINE__,
                  (xdc_IArg) sizeof (OmxProxy_proxyBufferInfo) *
                  pComponentPrivate->maxNumBuffers);
      eError = OMX_ErrorInsufficientResources;
      DOMX_UTL_TRACE_FUNCTION_ASSERT ((eError == OMX_ErrorNone),
                                      "Memory alloc failed for bufferList");
    }
    else
    {
      TIMM_OSAL_Memset (pComponentPrivate->bufferList, 0x0,
                        (sizeof(OmxProxy_proxyBufferInfo) *
                         pComponentPrivate->
                         maxNumBuffers));
      rsrcMask |= OMXPROXY_RSRC_PROXYINIT_BUFFERLIST_ALLOC;
      eError = omxproxy_get_component_custom_config_info (cComponentName,
                                                          &pComponentPrivate->
                                                          customParamStructInfo);
      DOMX_UTL_TRACE_FUNCTION_ASSERT
        ((eError == OMX_ErrorNone),
         "omxproxy_get_component_custom_config_info failed");

      DOMX_UTL_TRACE_FUNCTION_MSG ("Before OmxRpc_Params_init");
      OMXPROXY_MEMLOG_USED_START ();
      OmxRpc_Params_init (&omxRpcParams);

      DOMX_UTL_TRACE_FUNCTION_MSG ("After OmxRpc_Params_init");

      omxRpcParams.componentName = cComponentName;
      omxRpcParams.remoteCoreId = compInfo->coreId;
      omxRpcParams.rcmEventCbServerPriority = compInfo->ctrlRcmServerPriority;
      omxRpcParams.rcmOmxComponentServerPriority =
        compInfo->cbRcmServerPriority;
      /* Create an OmxRpc Stub instance */
      omxRpcParams.layerId = OmxRpc_layerTypeStub;

      DOMX_UTL_TRACE_FUNCTION_MSG ("Before OmxRpc_create");
      rpcStubHandle = OmxRpc_create (&omxRpcParams, &eb);
      if (Error_check (&eb))
      {
        Log_print1 (Diags_USER1, "OmxRpc Skel create failed.Staus = %d",
                    Error_getId (&eb));
        eError = OMX_ErrorInsufficientResources;
      }
      DOMX_UTL_TRACE_FUNCTION_ASSERT ((OMX_ErrorNone == eError),
                                      "OmxRpc_create failed");
      DOMX_UTL_TRACE_FUNCTION_MSG ("After OmxRpc_create");
      OMXPROXY_MEMLOG_USED_END (&
                                (pComponentPrivate->
                                 memoryUsed
                                 [DOMXPROXY_MEMUSAGEPHASE_RPCSTUBCREATE][0]));
    }
  }                             /* end of if component Private Memory_alloc
                                   success */
  if (OMX_ErrorNone == eError)
  {
    rsrcMask |= OMXPROXY_RSRC_PROXYINIT_OMXRPC_CREATE;
    /* The Proxy component does a get RPC_GetHandle to create corresponding
       remote components */
    /* Here we send the proxy components local handle for pAppData, this is
       useful in maintaining a handle Match in Facilitator/OMXRPC Remote side
       In case of multile tunnel requests for the same remote components, this
       {proxy handle , real remote handle} pair list would be used to check if
       we have already created the tunneled component. Also during call backs
       to 'Facilitator' on remote side, pAppData would be returned by the real
       component to 'Facilitator' based on this handle matching pair we can
       know who is calling the call back functions and replicate the call back
       from respective proxy components on the Local core. */

    OMXPROXY_MEMLOG_USED_START ();

    rpcCmdStatus = OmxRpc_stubGetHandle (rpcStubHandle, /* rpc skel layer
                                                           handle */
                                         &rpcCb,        /* Cb Function Tbl */
                                         (OMX_PTR) hComponent,  /* Cb Context */
                                         (OmxCore_OMX_ERRORTYPE *) & eError,    /* OMXApi 
                                                                                 * return 
                                                                                 * value 
                                                                                 */
                                         &pComponentPrivate->
                                         pRemoteComponentHandle,
                                         &pComponentPrivate->nRpcRemoteHandle,
                                         pComponentPrivate->
                                         szComponentRcmServerName,
                                         sizeof (pComponentPrivate->
                                                 szComponentRcmServerName));
    OMXPROXY_MEMLOG_USED_END (&
                              (pComponentPrivate->
                               memoryUsed[DOMXPROXY_MEMUSAGEPHASE_RPCGETHANDLE]
                               [0]));

    DOMX_UTL_TRACE_FUNCTION_ERROR (((OmxRpc_errorNone == rpcCmdStatus) &&
                                    (OMX_ErrorNone == eError)),
                                   "OmxRpc_stubGetHandle failed");

    if ((OmxRpc_errorNone == rpcCmdStatus) && (OMX_ErrorNone == eError))
    {
      rsrcMask |= OMXPROXY_RSRC_PROXYINIT_OMXRPC_GETHANDLE;
      Log_print2 (Diags_USER1,
                  "Remote component creation success for (%s).CoreId = (%d)",
                  (xdc_IArg) cComponentName, compInfo->coreId);
      pComponentPrivate->rpcHandle = rpcStubHandle;
    }
    else
    {
      if (OMX_ErrorNone == eError)
      {
        /* If GetHandle failed due to RPC error, set OMX error code as
           OMX_ErrorInsufficientResources */
        eError = OMX_ErrorInsufficientResources;
        Log_print1 (Diags_USER1, "OmxRpc_stubGetHandle failed.ErrorId:%d",
                    rpcCmdStatus);
      }                         /* end of if OmxRpc error occured */
    }                           /* end of else -- OmxRpc_stubgetHandle failed
                                   case) */
  }                             /* end of if (OmxRpc create success */
  if (OMX_ErrorNone == eError)
  {
    pHandle->SetCallbacks = omxproxy_setcallbacks;
    pHandle->ComponentDeInit = omxproxy_component_deinit;
    pHandle->UseBuffer = omxproxy_use_buffer;
    pHandle->AllocateBuffer = omxproxy_alloc_buffer;
    pHandle->GetParameter = omxproxy_get_parameter;
    pHandle->SetParameter = omxproxy_set_parameter;
    pHandle->EmptyThisBuffer = omxproxy_empty_this_buffer;
    pHandle->FillThisBuffer = omxproxy_fill_this_buffer;
    pHandle->GetComponentVersion = omxproxy_get_component_version;
    pHandle->SendCommand = omxproxy_send_command;
    pHandle->GetConfig = omxproxy_get_config;
    pHandle->SetConfig = omxproxy_set_config;
    pHandle->GetState = omxproxy_get_state;
    pHandle->GetExtensionIndex = omxproxy_get_extension_index;
    pHandle->FreeBuffer = omxproxy_free_buffer;

    pHandle->ComponentRoleEnum = omxproxy_component_role_enum;

    pHandle->ComponentTunnelRequest = omxproxy_component_tunnel_request;
  }
  else
  {                             /* GetHandle failed. Do rollback by freeing
                                   allocated resource */
    OMX_ERRORTYPE freeRsrcError;

    freeRsrcError = omxproxy_free_proxyinit_rsrc (pHandle, rsrcMask);
    DOMX_UTL_TRACE_FUNCTION_ASSERT ((OmxRpc_errorNone == freeRsrcError),
                                    "omxproxy_free_proxyinit_rsrc failed");
  }

  OMXPROXY_MEMLOG_USED_END (&
                            (pComponentPrivate->
                             memoryUsed[DOMXPROXY_MEMUSAGEPHASE_PROXYCREATE]
                             [0]));
  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (eError);

  return eError;
}

/* Bit values for each of the create types */

/* Value 0x0 - binary value: 0000 0000 */
#define OMXPROXY_RSRC_PROXYLITE_CREATE_NONE                                  (0)

/* Value 0x1 - binary value: 0000 0001 */
#define OMXPROXY_RSRC_PROXYLITE_CREATE_OMXRPC_GETHANDLE                 (1 << 0)

/* Value 0x2 - binary value: 0000 0010 */
#define OMXPROXY_RSRC_PROXYLITE_CREATE_OMXRPC_CREATE                    (1 << 1)

/* Value 0x4 - binary value: 0000 0100 */
#define OMXPROXY_RSRC_PROXYLITE_CREATE_BUFFERLIST_ALLOC                 (1 << 2)

/* Value 0x8 - binary value: 0000 1000 */
#define OMXPROXY_RSRC_PROXYLITE_CREATE_COMPPRIVATE_ALLOC                (1 << 3)

/* Value 0x10 - binary value: 0001 0000 */
#define OMXPROXY_RSRC_PROXYLITE_CREATE_COMPOBJ_ALLOC                    (1 << 4)

/* Value 0x1F - binary value: 0001 1111 */
#define OMXPROXY_RSRC_PROXYLITE_CREATE_ALL                              (      \
                    OMXPROXY_RSRC_PROXYLITE_CREATE_OMXRPC_GETHANDLE          | \
                    OMXPROXY_RSRC_PROXYLITE_CREATE_OMXRPC_CREATE             | \
                    OMXPROXY_RSRC_PROXYLITE_CREATE_BUFFERLIST_ALLOC          | \
                    OMXPROXY_RSRC_PROXYLITE_CREATE_COMPPRIVATE_ALLOC         | \
                    OMXPROXY_RSRC_PROXYLITE_CREATE_COMPOBJ_ALLOC)

/**
 * @name   omxproxy_free_proxylitecreate_rsrc
 * @brief  DOMX proxy free resources allocated at proxy lite creation time
 * @param  pHandle            : Pointer to handle
 * @param  rsrcMask           : Resource mask bitfield
 * @return OMX_ErrorNone : SUCCESS
 */
static Void omxproxy_free_proxylitecreate_rsrc (OMX_COMPONENTTYPE *pHandle,
                                                Int rsrcMask)
{
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  PROXY_COMPONENT_PRIVATE *pComponentPrivate;
  OmxRpc_errorType rpcCmdStatus;

  pComponentPrivate = pHandle->pComponentPrivate;
  if (OMXPROXY_RSRC_PROXYLITE_CREATE_OMXRPC_GETHANDLE & rsrcMask)
  {
    OMXPROXY_MEMLOG_FREE_START ();
    rpcCmdStatus = OmxRpc_stubLiteFreeHandle (pComponentPrivate->rpcHandle,
                                              (OmxCore_OMX_ERRORTYPE *) &
                                              eError);
    DOMX_UTL_TRACE_FUNCTION_ASSERT ((OmxRpc_errorNone == rpcCmdStatus),
                                    "stubLiteFreeHandle returned error");
    OMXPROXY_MEMLOG_FREE_END (&
                              (pComponentPrivate->
                               memoryUsed
                               [DOMXPROXY_MEMUSAGEPHASE_RPCLITEGETHANDLE][0]),
                              DOMXPROXY_MEMUSAGEPHASE_RPCLITEGETHANDLE);
  }
  if (OMXPROXY_RSRC_PROXYLITE_CREATE_OMXRPC_CREATE & rsrcMask)
  {
    OMXPROXY_MEMLOG_FREE_START ();
    OmxRpc_delete (&pComponentPrivate->rpcHandle);
    OMXPROXY_MEMLOG_FREE_END (&
                              (pComponentPrivate->
                               memoryUsed
                               [DOMXPROXY_MEMUSAGEPHASE_STUBLITECREATE][0]),
                              DOMXPROXY_MEMUSAGEPHASE_STUBLITECREATE);
  }
  if (OMXPROXY_RSRC_PROXYLITE_CREATE_BUFFERLIST_ALLOC & rsrcMask)
  {
    Memory_free (OmxProxy_Module_heap (), pComponentPrivate->bufferList,
                 (sizeof (OmxProxy_proxyBufferInfo) *
                  pComponentPrivate->maxNumBuffers));
  }
  if (OMXPROXY_RSRC_PROXYLITE_CREATE_COMPPRIVATE_ALLOC & rsrcMask)
  {
    Memory_free (OmxProxy_Module_heap (), pHandle->pComponentPrivate,
                 sizeof (PROXY_COMPONENT_PRIVATE));
  }
  if (OMXPROXY_RSRC_PROXYLITE_CREATE_COMPOBJ_ALLOC & rsrcMask)
  {
    Memory_free (OmxProxy_Module_heap (), pHandle, sizeof (OMX_COMPONENTTYPE));
  }
}

/**
 * @name   omxproxy_component_deinit_proxylite
 * @brief  DOMX proxy lite component deinit
 * @param  hComponent         : Handle to proxy lite component
 * @return OMX_ErrorNone : SUCCESS
 */
static OMX_ERRORTYPE omxproxy_component_deinit_proxylite (OMX_HANDLETYPE
                                                            hComponent)
{
  OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *) hComponent;
  PROXY_COMPONENT_PRIVATE *pComponentPrivate =
    (PROXY_COMPONENT_PRIVATE *) pHandle->pComponentPrivate;
  OMX_U32 proxyAllocSize[DOMX_MEMUSAGE_MAX_HEAPS_PER_PHASE];

  memcpy (proxyAllocSize,
          &(pComponentPrivate->
            memoryUsed[DOMXPROXY_MEMUSAGEPHASE_PROXYLITECREATE][0]),
          sizeof (pComponentPrivate->
                  memoryUsed[DOMXPROXY_MEMUSAGEPHASE_PROXYLITECREATE][0]) *
          DOMX_MEMUSAGE_MAX_HEAPS_PER_PHASE);
  OMXPROXY_MEMLOG_FREE_START ();

  omxproxy_free_proxylitecreate_rsrc (pHandle,
                                      OMXPROXY_RSRC_PROXYLITE_CREATE_ALL);

  OMXPROXY_MEMLOG_FREE_END (proxyAllocSize,
                            DOMXPROXY_MEMUSAGEPHASE_PROXYLITECREATE);
  return OMX_ErrorNone;

}

/**
 * @name   OmxProxy_createLite
 * @brief  DOMX create proxy lite create
 * @param  pProxyLiteHandlePtr: Pointer to proxylite instance to be created
 * @param  cp                 : ProxyLite create params
 * @return OMX_ErrorNone : SUCCESS
 */
OMX_ERRORTYPE OmxProxy_createLite (OMX_HANDLETYPE *pProxyLiteHandlePtr,
                                   OmxProxy_proxyLiteCreateParams *cp)
{
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  OmxRpc_errorType rpcCmdStatus = OmxRpc_errorNone;
  OMX_COMPONENTTYPE *pHandle = NULL;
  PROXY_COMPONENT_PRIVATE *pComponentPrivate;
  OmxRpc_Params omxRpcParams;
  Error_Block eb;
  OmxRpc_Handle rpcStubHandle = NULL;
  DomxCore_componentCoreInfoEntry *compInfo = NULL;
  OmxRpc_callBack rpcCb = { omxproxy_event_handler,
    omxproxy_empty_buffer_done,
    omxproxy_fill_buffer_done
  };
  Int rsrcMask = OMXPROXY_RSRC_PROXYLITE_CREATE_NONE;

  *pProxyLiteHandlePtr = NULL;
  /* Must initialize before use */
  Error_init (&eb);
  OMXPROXY_MEMLOG_USED_START ();
  pHandle = (OMX_COMPONENTTYPE *) Memory_alloc (OmxProxy_Module_heap (),
                                                sizeof (OMX_COMPONENTTYPE),
                                                sizeof (Ptr), &eb);
  if (Error_check (&eb))
  {
    Log_print3 (Diags_USER1,
                "%s:%d::Memory alloc failed for ProxyComponentHandle."
                "Alloc Size = (%d)", (xdc_IArg) __FUNCTION__,
                (xdc_IArg) __LINE__, (xdc_IArg) sizeof (OMX_COMPONENTTYPE));
    eError = OMX_ErrorInsufficientResources;
  }
  if (OMX_ErrorNone == eError)
  {
    rsrcMask |= OMXPROXY_RSRC_PROXYLITE_CREATE_COMPOBJ_ALLOC;
    eError = omxproxy_map_component_name2info (cp->cComponentName, &compInfo);
    DOMX_UTL_TRACE_FUNCTION_ASSERT ((OMX_ErrorNone == eError),
                                    "Comp not found");
    pHandle->pComponentPrivate =
      (PROXY_COMPONENT_PRIVATE *) Memory_alloc (OmxProxy_Module_heap (),
                                                sizeof
                                                (PROXY_COMPONENT_PRIVATE),
                                                sizeof (Ptr), &eb);
    if (Error_check (&eb))
    {
      Log_print3 (Diags_USER1,
                  "%s:%d::Memory alloc failed for ProxyComponentPrivate."
                  "Alloc Size = (%d)", (xdc_IArg) __FUNCTION__,
                  (xdc_IArg) __LINE__,
                  (xdc_IArg) sizeof (PROXY_COMPONENT_PRIVATE));
      eError = OMX_ErrorInsufficientResources;
    }
  }
  if (OMX_ErrorNone == eError)
  {
    rsrcMask |= OMXPROXY_RSRC_PROXYLITE_CREATE_COMPPRIVATE_ALLOC;
    pComponentPrivate = (PROXY_COMPONENT_PRIVATE *) pHandle->pComponentPrivate;
    /* Setting its core information in private data */
    pComponentPrivate->realCoreId = compInfo->coreId;
    /* Copy component name into private data */
    strncpy (pComponentPrivate->szComponentName, cp->cComponentName,
             OMX_MAX_STRINGNAME_SIZE);
    strncpy (pComponentPrivate->szComponentRcmServerName,
             cp->cComponentRcmServerName, OMX_MAX_STRINGNAME_SIZE);
    pComponentPrivate->nRpcRemoteHandle = cp->nRpcRemoteHandle;
    pComponentPrivate->numOfBuffers = 0;
    pComponentPrivate->maxNumBuffers = compInfo->maxNumBuffers;
    pComponentPrivate->bufferList =
      (OmxProxy_proxyBufferInfo *) Memory_alloc (OmxProxy_Module_heap (),
                                                 (sizeof
                                                  (OmxProxy_proxyBufferInfo) *
                                                  pComponentPrivate->
                                                  maxNumBuffers), sizeof (Ptr),
                                                 &eb);
    if (Error_check (&eb))
    {
      Log_print3 (Diags_USER1, "%s:%d::Memory alloc failed for bufferList."
                  "Alloc Size = (%d)",
                  (xdc_IArg) __FUNCTION__,
                  (xdc_IArg) __LINE__,
                  (xdc_IArg) sizeof (OmxProxy_proxyBufferInfo) *
                  pComponentPrivate->maxNumBuffers);
      eError = OMX_ErrorInsufficientResources;
    }
    else
    {
      TIMM_OSAL_Memset (pComponentPrivate->bufferList, 0x0,
                        (sizeof(OmxProxy_proxyBufferInfo) *
                         pComponentPrivate->
                         maxNumBuffers));
      rsrcMask |= OMXPROXY_RSRC_PROXYLITE_CREATE_BUFFERLIST_ALLOC;
      eError = omxproxy_get_component_custom_config_info (cp->cComponentName,
                                                          &pComponentPrivate->
                                                          customParamStructInfo);
      DOMX_UTL_TRACE_FUNCTION_ASSERT ((OMX_ErrorNone == eError),
                                      "Unable to get config info");
      OMXPROXY_MEMLOG_USED_START ();
      OmxRpc_Params_init (&omxRpcParams);
      omxRpcParams.componentName = cp->cComponentName;
      omxRpcParams.remoteCoreId = compInfo->coreId;
      omxRpcParams.rcmEventCbServerPriority = compInfo->ctrlRcmServerPriority;
      omxRpcParams.rcmOmxComponentServerPriority =
        compInfo->cbRcmServerPriority;
      /* Create an OmxRpc Stub Lite instance */
      omxRpcParams.layerId = OmxRpc_layerTypeStubLite;
      rpcStubHandle = OmxRpc_create (&omxRpcParams, &eb);
      if (Error_check (&eb))
      {
        Log_print1 (Diags_USER1, "OmxRpc Skel create failed.Staus = %d",
                    Error_getId (&eb));
        eError = OMX_ErrorInsufficientResources;
      }
      OMXPROXY_MEMLOG_USED_END (&
                                (pComponentPrivate->
                                 memoryUsed
                                 [DOMXPROXY_MEMUSAGEPHASE_STUBLITECREATE][0]));
    }
  }                             /* end of if component Private Memory_alloc
                                   success */
  if (OMX_ErrorNone == eError)
  {
    rsrcMask |= OMXPROXY_RSRC_PROXYLITE_CREATE_OMXRPC_CREATE;
    OMXPROXY_MEMLOG_USED_START ();
    rpcCmdStatus = OmxRpc_stubLiteGetHandle (rpcStubHandle,     /* rpc stublite 
                                                                 * layer handle 
                                                                 */
                                             &rpcCb,    /* Cb Function Tbl */
                                             (OMX_PTR) pHandle, /* Cb Context */
                                             (OmxCore_OMX_ERRORTYPE *) & eError,        /* OMXApi 
                                                                                         * return 
                                                                                         * value 
                                                                                         */
                                             cp->nRpcRemoteHandle,
                                             cp->cComponentRcmServerName);
    OMXPROXY_MEMLOG_USED_END (&
                              (pComponentPrivate->
                               memoryUsed
                               [DOMXPROXY_MEMUSAGEPHASE_RPCLITEGETHANDLE][0]));
    if ((OmxRpc_errorNone == rpcCmdStatus) && (OMX_ErrorNone == eError))
    {
      rsrcMask |= OMXPROXY_RSRC_PROXYLITE_CREATE_OMXRPC_GETHANDLE;
      Log_print2 (Diags_USER1,
                  "Remote component creation success for (%s).CoreId = (%d)",
                  (xdc_IArg) cp->cComponentName, compInfo->coreId);
      pComponentPrivate->rpcHandle = rpcStubHandle;
      pComponentPrivate->pRemoteComponentHandle = cp->pOmxComponentHandle;
    }
    else
    {
      if (OMX_ErrorNone == eError)
      {
        /* If GetHandle failed due to RPC error, set OMX error code as
           OMX_ErrorInsufficientResources */
        eError = OMX_ErrorInsufficientResources;
        Log_print1 (Diags_USER1, "OmxRpc_stubGetHandle failed.ErrorId:%d",
                    rpcCmdStatus);
      }                         /* end of if OmxRpc error occured */
    }                           /* end of else -- OmxRpc_stubgetHandle failed
                                   case) */
  }                             /* end of if (OmxRpc create success */
  if (OMX_ErrorNone == eError)
  {
    pHandle->SetCallbacks = omxproxy_setcallbacks;
    pHandle->ComponentDeInit = omxproxy_component_deinit_proxylite;
    pHandle->UseBuffer = omxproxy_use_buffer;
    pHandle->AllocateBuffer = omxproxy_alloc_buffer;
    pHandle->GetParameter = omxproxy_get_parameter;
    pHandle->SetParameter = omxproxy_set_parameter;
    pHandle->EmptyThisBuffer = omxproxy_empty_this_buffer;
    pHandle->FillThisBuffer = omxproxy_fill_this_buffer;
    pHandle->GetComponentVersion = omxproxy_get_component_version;
    pHandle->SendCommand = omxproxy_send_command;
    pHandle->GetConfig = omxproxy_get_config;
    pHandle->SetConfig = omxproxy_set_config;
    pHandle->GetState = omxproxy_get_state;
    pHandle->GetExtensionIndex = omxproxy_get_extension_index;
    pHandle->FreeBuffer = omxproxy_free_buffer;
    pHandle->ComponentRoleEnum = omxproxy_component_role_enum;
    pHandle->ComponentTunnelRequest = omxproxy_component_tunnel_request;
    *pProxyLiteHandlePtr = pHandle;
  }
  else
  { /* GetHandle failed. Do rollback by freeing
       allocated resource */
    omxproxy_free_proxylitecreate_rsrc (pHandle, rsrcMask);
  }
  OMXPROXY_MEMLOG_USED_END (&
                            (pComponentPrivate->
                             memoryUsed[DOMXPROXY_MEMUSAGEPHASE_PROXYLITECREATE]
                             [0]));
  return eError;
}

/**
 * @name   OmxProxy_deleteLite
 * @brief  DOMX delete proxy lite create
 * @param  pProxyLiteHandlePtr: Pointer to proxylite instance to be created
 * @return OMX_ErrorNone : SUCCESS
 */

OMX_ERRORTYPE OmxProxy_deleteLite (OMX_HANDLETYPE *pProxyLiteHandlePtr)
{
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  OMX_COMPONENTTYPE *pHandle = NULL;
  Int rsrcMask = OMXPROXY_RSRC_PROXYLITE_CREATE_ALL;
  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();

  pHandle = (OMX_COMPONENTTYPE *) * pProxyLiteHandlePtr;

  omxproxy_free_proxylitecreate_rsrc (pHandle, rsrcMask);
  *pProxyLiteHandlePtr = NULL;

  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (eError);
  return eError;
}

/**
 * @name   Omxproxy_get_heap_mem_stats
 * @brief  DOMX proxy free resources allocated at proxy lite creation time
 * @param  rsrcMask           : Resource mask bitfield
 * @return OMX_ErrorNone : SUCCESS
 */

OMX_ERRORTYPE OMX_GetHeapMemStats (OMX_IN OMX_U32 *totalFreeSize)
{
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  OmxRpc_errorType rpcError = OmxRpc_errorNone;

  DOMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 ();

  rpcError =
    OmxRpc_stubGetHeapMemStats ((OmxTypes_OMX_U32 *) totalFreeSize,
                                (OmxCore_OMX_ERRORTYPE *) & eError);

  if (rpcError == OmxRpc_errorNone)
  {
    Log_print1 (Diags_USER1, "%s:OmxRpc_stubGetHeapMemStats Successful.",
                (xdc_IArg) __FUNCTION__);
  }
  else
  {
    Log_print3 (Diags_USER1, "%s:%d::OmxRpc_stubGetHeapMemStats failed."
                "ErrorId: %d", (xdc_IArg) __FILE__, __LINE__, rpcError);
    eError = OMX_ErrorUndefined;
  }

  DOMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 (eError);

  return eError;
}

/* Nothing beyond this point */
