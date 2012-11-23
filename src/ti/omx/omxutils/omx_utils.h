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
 *  @file  omx_utils.h
 *
 *  @rev 1.0
 */

#ifndef _OMX_UTL_UTILS_H_
#define _OMX_UTL_UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/******************************************************************
 *   INCLUDE FILES
 ******************************************************************/
//*#include <ti/sysbios/hal/Timer.h>
//#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <ti/timmosal/timm_osal_types.h>
#include <ti/timmosal/timm_osal_interfaces.h>
#include <ti/timmosal/timm_osal_trace.h>

#include "ti/omx/interfaces/openMaxv11/OMX_Types.h"
#include <ti/omx/interfaces/openMaxv11/OMX_Core.h>
#include <ti/omx/interfaces/openMaxv11/OMX_Component.h>
#include <ti/omx/interfaces/openMaxv11/OMX_IVCommon.h>

#include <xdc/runtime/Log.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Diags.h>

#ifndef OMX_UTL_TRACE_MODULE_NAME
#define OMX_UTL_TRACE_MODULE_NAME "ti.omx"
#endif

#define OMX_UTL_TRACE_FUNCTION_ENTRY() \
            Log_print3(Diags_USER1,"Module<%s> Entering<%s> @line<%d> ",(xdc_IArg)OMX_UTL_TRACE_MODULE_NAME, (xdc_IArg)__FUNCTION__, __LINE__);

#define OMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL1 OMX_UTL_TRACE_FUNCTION_ENTRY

#define OMX_UTL_TRACE_FUNCTION_EXIT(errorVal) \
            Log_print5(Diags_USER1,"Module<%s> Leaving<%s> @line<%d> with error<%d:%s>",(xdc_IArg)OMX_UTL_TRACE_MODULE_NAME, (xdc_IArg)__FUNCTION__, __LINE__,errorVal,(xdc_IArg)OMX_UTL_OmxErrorToString(errorVal));

#define OMX_UTL_TRACE_FUNCTION_EXIT_LEVEL1 OMX_UTL_TRACE_FUNCTION_EXIT

#define OMX_UTL_TRACE_FUNCTION_ENTRY_LEVEL2() \
            Log_print3(Diags_USER3,"Module<%s> Entering<%s> @line<%d> ",(xdc_IArg)OMX_UTL_TRACE_MODULE_NAME, (xdc_IArg)__FUNCTION__, __LINE__);

#define OMX_UTL_TRACE_FUNCTION_EXIT_LEVEL2(errorVal) \
            Log_print5(Diags_USER3,"Module<%s> Leaving<%s> @line<%d> with error<%d:%s>",(xdc_IArg)OMX_UTL_TRACE_MODULE_NAME, (xdc_IArg)__FUNCTION__, __LINE__,errorVal,(xdc_IArg)OMX_UTL_OmxErrorToString(errorVal));

#define OMX_UTL_TRACE_FUNCTION_MSG(printMsg) \
            Log_print4(Diags_USER1,"Module<%s> @<%s> @line<%d> msg<%s>",(xdc_IArg)OMX_UTL_TRACE_MODULE_NAME, (xdc_IArg)__FUNCTION__, __LINE__,(xdc_IArg)printMsg);

#define OMX_UTL_TRACE_FUNCTION_MSG_LEVEL1 OMX_UTL_TRACE_FUNCTION_MSG

#define OMX_UTL_TRACE_FUNCTION_MSG_LEVEL2(printMsg) \
            Log_print4(Diags_USER3,"Module<%s> @<%s> @line<%d> msg<%s>",(xdc_IArg)OMX_UTL_TRACE_MODULE_NAME, (xdc_IArg)__FUNCTION__, __LINE__,(xdc_IArg)printMsg);

#define OMX_UTL_TRACE_FUNCTION_ERROR(condition,msg) \
        if(!(condition)) { \
            Log_print3(Diags_USER1, "Module<%s>:Error in <%s> @line<%d>",(xdc_IArg)OMX_UTL_TRACE_MODULE_NAME,(xdc_IArg)__FUNCTION__, __LINE__); \
        }

#define OMX_UTL_TRACE_FUNCTION_ASSERT(condition,msg) \
        if(!(condition)) \
        { \
            Log_print4 (Diags_USER1,                                              \
                        "Module<%s>:Assert failure in <%s> @line<%d>; Error: %s", \
                        (xdc_IArg) OMX_UTL_TRACE_MODULE_NAME,                     \
                        (xdc_IArg) __FUNCTION__, __LINE__, (xdc_IArg) msg);       \
            assert (condition); \
        }

/*******************************************************************
 * EXTERNAL REFERENCE NOTE: only use if not found in header file
 *******************************************************************/
/*----------         function prototypes      ------------------- */
/*----------         data declarations        ------------------- */


/*******************************************************************
 * PUBLIC DECLARATIONS: defined here, used elsewhere
 *******************************************************************/
extern OMX_U32 g_OMX_UTL_processorID;

/*----------         function prototypes      ------------------- */

static inline void OMX_UTL_Strncpy ( OMX_STRING dststr, OMX_STRING srcstr, 
                              OMX_U32 size);

static inline OMX_ERRORTYPE  OMX_UTL_LogResAlloc (OMX_U32 size,
                         OMX_U32 segId, 
                         OMX_PTR addr,
                         OMX_PTR pResLog,
						 OMX_U32 ResType );

static inline OMX_ERRORTYPE  OMX_UTL_VerResEndStatus ( OMX_PTR pvResLog );						 
                              
/*----------         data declarations        ------------------- */

typedef struct OMX_UTL_TUNNEL_CONTEXT 
{
 OMX_HANDLETYPE hSrcComp; 
 OMX_HANDLETYPE hDstComp;
 OMX_U32 srcPortIx;
 OMX_U32 dstPortIx;
}OMX_UTL_TUNNEL_CONTEXT;

#define OMX_UTL_TABLE_MAX_SIZE            64
#define OMX_TOTAL_POOLS_PER_FQMGR         32

typedef enum resType_t
{
  OMX_UTL_UNDEFINED = 0,
  OMX_UTL_MEMORY  = 1,
  OMX_UTL_EVENT   = 2,
  OMX_UTL_SEM     = 3,
  OMX_UTL_MUTEX   = 4,
  OMX_UTL_PIPE    = 5,
  OMX_UTL_TASK    = 6
}resType_t;


typedef struct list_t
{
  resType_t resType;
  OMX_U32 uSegId;
  OMX_U32 uSize;
  OMX_PTR pAddr;
}list_t;

typedef struct resourceLog_t
{
  list_t  aTable [ OMX_UTL_TABLE_MAX_SIZE ];
  OMX_U32 uWriteIx;
}resourceLog_t;

#define OMX_UTL_FQ_MAX_NUM_BUFS_IN_FRAME  ( 2u  )
#define OMX_UTL_MAX_STRINGNAME_SIZE       ( 32u )
#define OMX_UTL_MAX_FQ_MAXEXTENDEDHDRFIELDS (25u )
#define OMX_UTL_MAX_FQ_HDR_SIZE           ( sizeof(OMX_UTL_FrameBufHdr ))

typedef struct OMX_UTL_FrameBufHdr {
 OMX_BUFFERHEADERTYPE sBuffHdr;
 UInt32               cMetaData[OMX_UTL_MAX_FQ_MAXEXTENDEDHDRFIELDS];
}OMX_UTL_FrameBufHdr;

typedef struct OMX_PARAM_PORTFQMGRINFO 
{
  OMX_U32  nSize;               /**< Size of the structure in bytes */
  OMX_VERSIONTYPE nVersion;     /**< OMX specification version information */
  OMX_U32  nPortIndex;          /**< Port number the structure applies to */
  OMX_BOOL bBufferSupplier;     /* Buffer supplier or Not */
  OMX_U32  uBufMemoryType;      /* */
  OMX_U32  uTotalHeaderSize;
  OMX_U32  uProcId;
  OMX_U32  uNumFrameBufsInFrame;
  OMX_U32  auFqBufferSize [ OMX_UTL_FQ_MAX_NUM_BUFS_IN_FRAME ];
}OMX_PARAM_PORTFQMGRINFO;

typedef struct OMX_CONFIG_ASSIGNFQMGR 
{
  OMX_U32  nSize;               /**< Size of the structure in bytes */
  OMX_VERSIONTYPE nVersion;     /**< OMX specification version information */
  OMX_U32  nPortIndex;          /**< Port number the structure applies to */
  OMX_S8   strFqManagerName[OMX_UTL_MAX_STRINGNAME_SIZE];
  OMX_U8   uPoolId;
}OMX_CONFIG_ASSIGNFQMGR;

typedef struct OMX_FQMGR_POOLINFO_s
{
    OMX_U32 uNumTotalFrameBufs;
    OMX_U32 uHeaderSize;
    OMX_U32 nBufferWidth;
    OMX_U32 nBufferHeight;
    OMX_U32 nBufferStride;
    OMX_U32 uNumFrameBufsInFrame;
    OMX_U32 auFqBufferSize [ OMX_UTL_FQ_MAX_NUM_BUFS_IN_FRAME ];
    OMX_U32 uBufMemoryType;
}OMX_FQMGR_POOLINFO_s;


typedef struct OMX_PARM_FQMGRCREATIONPARM
{
    OMX_U32  nSize;               /**< Size of the structure in bytes */
    OMX_VERSIONTYPE nVersion;     /**< OMX specification version information */
    OMX_U32  nPortIndex;          /**< Port number the structure applies to */

    OMX_FQMGR_POOLINFO_s sPoolInfo[OMX_TOTAL_POOLS_PER_FQMGR];
    OMX_U32  nTotalPools;
}OMX_PARM_FQMGRCREATIONPARM;

#define OMX_UTL_COMP_VERSION_MAJOR      (0x1)
#define OMX_UTL_COMP_VERSION_MINOR      (0x1)
#define OMX_UTL_COMP_VERSION_REVISION   (0x2)
#define OMX_UTL_COMP_VERSION_STEP       (0x0)

#define OMX_UTL_MEMORY_NON_TILED    ((OMX_U16)0x0)
#define OMX_UTL_MEMORY_TILED        ((OMX_U16)(OMX_UTL_MEMORY_NON_TILED + 1))

#define OMX_UTL_PROC_VIDM3              (0x1)
#define OMX_UTL_PROC_VPSM3              (OMX_UTL_PROC_VIDM3 << 1)
#define OMX_UTL_PROC_A8                 (OMX_UTL_PROC_VPSM3 << 1)

#define OMX_UTL_Trace(ARGS) TIMM_OSAL_Trace(ARGS)
#define OMX_UTL_Error(ARGS) TIMM_OSAL_Error(ARGS)


#define OMX_UTL_Malloc(_nSize_, _hHeap_) \
        TIMM_OSAL_MallocExtn(_nSize_, TIMM_OSAL_TRUE, 0, TIMMOSAL_MEM_SEGMENT_EXT, _hHeap_)

#define OMX_UTL_MALLOC_STRUCT(_pStruct_, _sName_, _hHeap_) \
    _pStruct_ = (_sName_ *)OMX_UTL_Malloc(sizeof(_sName_), _hHeap_); \
    if(_pStruct_ == NULL) { \
        OMX_UTL_Error(" Malloc failed "); \
        eError = OMX_ErrorInsufficientResources; \
        goto EXIT; \
    }

#define OMX_UTL_MALLOC_STRUCT_LOG(_pStruct_, _sName_, _hHeap_, _ptr_) \
    _pStruct_ = (_sName_ *)OMX_UTL_Malloc(sizeof(_sName_), _hHeap_); \
    if(_pStruct_ == NULL) { \
        OMX_UTL_Error(" Malloc failed "); \
        eError = OMX_ErrorInsufficientResources; \
        goto EXIT; \
    }              \
    else {         \
      eError = OMX_UTL_LogMemAlloc (sizeof(_sName_), TIMMOSAL_MEM_SEGMENT_EXT, \
                           _pStruct_, _ptr_);                         \
      if ( eError == OMX_ErrorInsufficientResources ) {               \
        goto EXIT;                                                    \
      }                                                               \
    }

#define OMX_UTL_REQUIRE OMX_UTL_PARAMCHECK
#define OMX_UTL_ASSERT  OMX_UTL_PARAMCHECK
#define OMX_UTL_ENSURE  OMX_UTL_PARAMCHECK

#define OMX_UTL_PARAMCHECK(_COND_, _ERRORCODE_) \
        if(!(_COND_)) { eError = _ERRORCODE_; \
            TIMM_OSAL_TraceFunction(" Error in %s :: line %d \n", __FUNCTION__, __LINE__); \
            Log_print2(Diags_USER1, " Error in %s :: line %d \n",(xdc_IArg)__FUNCTION__, __LINE__); \
            goto EXIT; \
        }


#define OMX_UTL_CHK_VERSION(_pStruct_, _sName_, _e_) \
    if(((_sName_ *)_pStruct_)->nSize != sizeof(_sName_)) { \
       _e_ = OMX_ErrorBadParameter; \
       goto EXIT; } \
    if((((_sName_ *)_pStruct_)->nVersion.s.nVersionMajor != 0x1) || \
       (((_sName_ *)_pStruct_)->nVersion.s.nVersionMinor != 0x1)) { \
       _e_ = OMX_ErrorVersionMismatch; \
       goto EXIT; \
    }

#define OMX_UTL_INIT_STRUCT_PTR(_pStruct_, _sName_)                            \
       {                                                                       \
        TIMM_OSAL_Memset((_pStruct_), 0x0, sizeof(_sName_));                   \
        (_pStruct_)->nSize = sizeof(_sName_);                                  \
        (_pStruct_)->nVersion.s.nVersionMajor = OMX_UTL_COMP_VERSION_MAJOR;    \
        (_pStruct_)->nVersion.s.nVersionMinor = OMX_UTL_COMP_VERSION_MINOR;    \
        (_pStruct_)->nVersion.s.nRevision     = OMX_UTL_COMP_VERSION_REVISION; \
        (_pStruct_)->nVersion.s.nStep         = OMX_UTL_COMP_VERSION_STEP;     \
       }


static inline void OMX_UTL_Strncpy ( OMX_STRING dststr, 
                              OMX_STRING srcstr, 
                                 OMX_U32 size)
{
  strncpy (dststr, srcstr, (size-1));

  dststr[size-1] = '\0';
}

static inline OMX_ERRORTYPE  OMX_UTL_LogResAlloc (OMX_U32 size,
                         OMX_U32 segId, 
                         OMX_PTR addr,
                         OMX_PTR pvResLog,
						 OMX_U32 ResType )
{
  OMX_U32 ix;
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  resourceLog_t* pResLog = (resourceLog_t*) pvResLog;
  
  
  ix = pResLog->uWriteIx;

  if (ix >= OMX_UTL_TABLE_MAX_SIZE){                      
    eError = OMX_ErrorInsufficientResources;                           
  }                                                                    
  else {
    pResLog ->aTable[ix].resType = ( resType_t ) ResType;             
    pResLog ->aTable[ix].uSegId  = segId;    
    pResLog ->aTable[ix].uSize   = size;              
    pResLog ->aTable[ix].pAddr   = addr;                    
    ix++;                                                   
    pResLog->uWriteIx = ix;
  }
  return eError;
}

static inline OMX_ERRORTYPE  OMX_UTL_ChkResStatus (OMX_PTR addr,
                         OMX_PTR pvResLog )
{
  OMX_U32 ix;
  OMX_BOOL bEntryfound = OMX_FALSE;
  OMX_ERRORTYPE eError = OMX_ErrorUndefined;
  resourceLog_t* pResLog = (resourceLog_t*) pvResLog;
  
  /* Search for the entry for the adrress you are rfreeing in the resource table*/
  for ( ix =0; ix < pResLog->uWriteIx; ix++) {
  if ( pResLog ->aTable[ix].pAddr == addr ) {
  bEntryfound = OMX_TRUE;
  pResLog ->aTable[ix].pAddr = NULL;
  pResLog ->aTable[ix].uSegId = 0;
  pResLog ->aTable[ix].uSize = 0;
  pResLog ->aTable[ix].resType = OMX_UTL_UNDEFINED;
  eError = OMX_ErrorNone;
  break;
  }
  }
  
  if (OMX_FALSE == bEntryfound) {
  eError = OMX_ErrorBadParameter;
  }
  
  return eError;
}

static inline OMX_ERRORTYPE  OMX_UTL_VerResEndStatus ( OMX_PTR pvResLog )
{
  OMX_U32 ix;
  OMX_BOOL bEntryfound = OMX_FALSE;
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  resourceLog_t* pResLog = (resourceLog_t*) pvResLog;
  
  /* Search for the entry for the adrress you are rfreeing in the resource table*/
  for ( ix =0; ix < pResLog->uWriteIx; ix++) {
  if ( NULL != pResLog ->aTable[ix].pAddr ) {
  bEntryfound = OMX_TRUE;
  printf("One or more allocated resources not freed at end point!!\n");
  printf("Resource of type %d.\n", pResLog ->aTable[ix].resType);
  break;
  }
  }
  
  if (OMX_TRUE == bEntryfound) {
  eError = OMX_ErrorBadParameter;
  }
  
  return eError;
}
extern const Bool g_ProileWindowInterruptsDisabled;

static inline OMX_ERRORTYPE  OMX_UTL_LogMemAlloc (OMX_U32 size,
                         OMX_U32 segId, 
                         OMX_PTR addr,
                         OMX_PTR pvResLog)
{
  OMX_U32 ix;
  OMX_ERRORTYPE eError = OMX_ErrorNone;
  resourceLog_t* pResLog = (resourceLog_t*) pvResLog;

  ix = pResLog->uWriteIx;

  if (ix >= OMX_UTL_TABLE_MAX_SIZE){                      
    eError = OMX_ErrorInsufficientResources;                           
  }                                                                    
  else {
    pResLog ->aTable[ix].resType = OMX_UTL_MEMORY;             
    pResLog ->aTable[ix].uSegId = segId;    
    pResLog ->aTable[ix].uSize = size;              
    pResLog ->aTable[ix].pAddr = addr;                    
    ix++;                                                   
    pResLog->uWriteIx = ix;
  }
  return eError;
}


/*----------          function prototypes      ------------------- */

Void OMX_UTL_TRACE_ProfileInit ();
Void OMX_UTL_TRACE_ProfileStart (OMX_U32 nIndex, OMX_U32 nIter);
Void OMX_UTL_TRACE_ProfileEnd (OMX_U32 nIndex, OMX_U32 nIter, OMX_U32 nFrames);
Void OMX_UTL_TRACE_ProfilePrint (OMX_U32 nIndex);
Void OMX_UTL_TRACE_PrintClose ();
Void OMX_UTL_TRACE_Enter (OMX_STRING funcName, OMX_U32 lineNum, 
                         OMX_U32 userData);
Void OMX_UTL_TRACE_Leave (OMX_STRING funcName, OMX_U32 lineNum, 
                         OMX_U32 userData);

Void OMX_UTL_TRACE_Print0 (OMX_STRING str);
Void OMX_UTL_TRACE_Print1 (OMX_STRING str, OMX_U32 data1);
Void OMX_UTL_TRACE_Print2 (OMX_STRING str, OMX_U32 data1, 
                           OMX_U32 data2);
Void OMX_UTL_TRACE_Print3 ( OMX_STRING str, OMX_U32 data1, 
                            OMX_U32 data2,  OMX_U32 data3 );
Void OMX_UTL_TRACE_Print4 ( OMX_STRING str, OMX_U32 data1, 
                            OMX_U32 data2,  OMX_U32 data3, 
                            OMX_U32 data4  );
Void OMX_UTL_TRACE_Print5 ( OMX_STRING str, OMX_U32 data1, 
                            OMX_U32 data2,  OMX_U32 data3, 
                            OMX_U32 data4,  OMX_U32 data5  );

Void OMX_UTL_TRACE_LogEvent0 (OMX_U32 nEvtId);
Void OMX_UTL_TRACE_LogEvent1 (OMX_U32 nEvtId, OMX_U32 nEvtParam1);
Void OMX_UTL_TRACE_LogEvent2 (OMX_U32 nEvtId, OMX_U32 nEvtParam1, 
                              OMX_U32 nEvtParam2);
OMX_STRING OMX_UTL_OmxErrorToString ( OMX_ERRORTYPE eError );

/**
  \brief One time system init of memory allocator

  Should be called by application before using allocate APIs

  \return 0 on sucess, else failure
*/
Int32 OmxUtils_memInit();

/**
  \brief One time system de-init of memory allocator

  Should be called by application at system de-init

  \return 0 on sucess, else failure
*/
Int32 OmxUtils_memDeInit();
/**
  \brief Allocate memory from memory pool

  \param size   [IN] size in bytes
  \param align  [IN] alignment in bytes

  \return NULL or error, else memory pointer
*/
Ptr   OmxUtils_memAlloc(UInt32 size, UInt32 align);

/**
  \brief Free previously allocate memory pointer

  \param addr [IN] memory pointer to free
  \param size [IN] size of memory pointed to by the memory pointer

  \return 0 on sucess, else failure
*/
Int32 OmxUtils_memFree(Ptr addr, UInt32 size);

/**
  \brief Translate CPU addr to tiler addr

  \param addr [IN] cpu addr value

  \return converted tilerAddr on sucess, else 0
*/
Ptr OmxUtils_tilerAddr2CpuAddr(Ptr tilerAddr);


/**
  \brief Translate CPU addr to tiler addr

  \param addr [IN] tiler addr value

  \return converted cpuAddr on sucess, else 0
*/
Ptr OmxUtils_cpuAddr2tilerAddr(Ptr cpuAddr);
                             
OMX_S32 OmxUtils_tilerGetMaxPitchHeight(OMX_U32 cntMode, OMX_U32 *maxPitch, OMX_U32 *maxHeight);

OMX_S32 OmxUtils_tilerCopy(OMX_U32 dir, OMX_U32 tilerAddr, OMX_U32 dataWidth, OMX_U32 dataHeight,
          OMX_U8 *ddrAddr, OMX_U32 ddrPitch);

/*******************************************************************
 * PRIVATE DECLARATIONS: defined here, used only here
 *******************************************************************/
/*----------          data declarations        ------------------- */

/* Enable for Thread load Profile & Event log collection */
#define THRD_PROFILE_EVENT_LOG_ENABLE             (FALSE)

/* Enable for dumping function trace */
#define OMX_FUNCTION_TRACE (0)
#define OMX_FUNCTION_TRACE_PRINT (0)
#define OMX_FUNCTION_TRACE_MEMORYDUMP (0)

/*----------          function prototypes      ------------------- */
#if OMX_FUNCTION_TRACE_PRINT
Void OMX_UTL_TRACE_FunctionCallFlowEntryHook (const char *inParam);
Void OMX_UTL_TRACE_FunctionCallFlowExitHook (const char *inParam);
#endif
#if OMX_FUNCTION_TRACE_MEMORYDUMP
Void OMX_UTL_TRACE_FunctionCallFlowEntryHook (void (*inParam)());
Void OMX_UTL_TRACE_FunctionCallFlowExitHook (void (*inParam)());
#endif

/* Maximum number of profilers we are running simultaneously */
#define MAX_PROFILE_INDEX 10
/* Maximum number of iterations per profile */
#define MAX_ITER_INDEX    100

/* Maximum number of independent events to be logged */
#define MAX_NUM_EVENTS        250

/* Maximum number of event to be logged per Base */
#define EVENTS_PER_BASE       80

/* Maximum number of independent events to be logged */
#define MAX_NUM_EVENTCOUNT_PER_EVENT       500

/* Maximum number of event params */
#define MAX_NUM_EVENT_PARAMS  2

/* Size of profile buffer */
#define BIGOUTBUFFERSIZE 512

#define VFCC_INDEX_DRV_FVID_CALLBACK_START    (0)
#define VFCC_INDEX_DRV_FVID_CALLBACK_END      (1)
#define VFCC_INDEX_DRV_FVID_DEQUEUE_START     (2)
#define VFCC_INDEX_DRV_FVID_DEQUEUE_END       (3)
#define VFCC_INDEX_OMX_DIO_SEND_START         (4)
#define VFCC_INDEX_OMX_DIO_SEND_END           (5)
#define VFCC_INDEX_OMX_DIO_DEQUEUE_START      (6)
#define VFCC_INDEX_OMX_DIO_DEQUEUE_END        (7)
#define VFCC_INDEX_DRV_FVID_QUEUE_START       (8)
#define VFCC_INDEX_DRV_FVID_QUEUE_END         (9)
#define VFCC_INDEX_OMX_CH_SETOMXHDRINFO       (11)
#define VFCC_INDEX_OMX_CH_SETFVIDFRAMEINFO    (12)


#define VFDC_INDEX_DRV_FVID_CALLBACK_START    (13)
#define VFDC_INDEX_DRV_FVID_CALLBACK_END      (14)
#define VFDC_INDEX_DRV_FVID_DEQUEUE_START     (15)
#define VFDC_INDEX_DRV_FVID_DEQUEUE_END       (16)
#define VFDC_INDEX_OMX_DIO_SEND_START         (17)
#define VFDC_INDEX_OMX_DIO_SEND_END           (18)
#define VFDC_INDEX_OMX_DIO_DEQUEUE_START      (19)
#define VFDC_INDEX_OMX_DIO_DEQUEUE_END        (20)
#define VFDC_INDEX_DRV_FVID_QUEUE_START       (21)
#define VFDC_INDEX_DRV_FVID_QUEUE_END         (22)
#define VFDC_INDEX_OMX_DIO_DUP_START          (23)
#define VFDC_INDEX_OMX_DIO_DUP_END            (24)
#define VFDC_INDEX_OMX_DIO_DUP_FREE_START     (25)
#define VFDC_INDEX_OMX_DIO_DUP_FREE_END       (26)
#define VFDC_INDEX_OMX_CH_SETFVIDFRAMEINFO    (27)
#define VFDC_INDEX_OMX_CH_UNDERRUN_DUPEDFRM   (28)
#define VFDC_INDEX_OMX_CH_UNDERRUN_BLANKFRM   (29)


#define OMX_INDEX_DIO_EVENT_FIRST             (30)
#define OMX_INDEX_DIO_FQALLOCV_START          (OMX_INDEX_DIO_EVENT_FIRST)
#define OMX_INDEX_DIO_FQALLOCV_END            (31)
#define OMX_INDEX_DIO_FQPUTV_START            (32)
#define OMX_INDEX_DIO_FQPUTV_END              (33)
#define OMX_INDEX_DIO_FQGETV_START            (34)
#define OMX_INDEX_DIO_FQGETV_END              (35)
#define OMX_INDEX_DIO_FQFREEV_START           (36)
#define OMX_INDEX_DIO_FQFREEV_END             (37)
#define OMX_INDEX_DIO_FQALLOC_START           (38)
#define OMX_INDEX_DIO_FQALLOC_END             (39)
#define OMX_INDEX_DIO_FQPUT_START             (40)
#define OMX_INDEX_DIO_FQPUT_END               (41)
#define OMX_INDEX_DIO_FQGET_START             (42)
#define OMX_INDEX_DIO_FQGET_END               (43)
#define OMX_INDEX_DIO_FQFREE_START            (44)
#define OMX_INDEX_DIO_FQFREE_END              (45)
#define OMX_INDEX_DIO_FQGETQUEUECNT_START     (46)
#define OMX_INDEX_DIO_FQGETQUEUECNT_END       (47)
#define OMX_INDEX_DIO_FQGETFREECNT_START      (48)
#define OMX_INDEX_DIO_FQGETFREECNT_END        (49)
#define OMX_INDEX_DIO_FQGETQUEUECNTV_START    (50)
#define OMX_INDEX_DIO_FQGETQUEUECNTV_END      (51)
#define OMX_INDEX_DIO_FQGETFREECNTV_START     (52)
#define OMX_INDEX_DIO_FQGETFREECNTV_END       (53)
#define OMX_INDEX_DIO_EVENT_LAST              (OMX_INDEX_DIO_FQGETFREECNTV_END)



#define VFPC_INDEX_EVENT_START                     (OMX_INDEX_DIO_EVENT_LAST + 1)
#define VFPC_INDEX_OMX_PRDTASKSCHEDULED            (VFPC_INDEX_EVENT_START +  1)
#define VFPC_INDEX_OMX_GETREADYCHANNEL_START       (VFPC_INDEX_EVENT_START +  2)
#define VFPC_INDEX_OMX_GETREADYCHANNEL_END         (VFPC_INDEX_EVENT_START +  3)
#define VFPC_INDEX_OMX_GENCHANNELFRAMELIST_START   (VFPC_INDEX_EVENT_START +  4)
#define VFPC_INDEX_OMX_GENCHANNELFRAMELIST_END     (VFPC_INDEX_EVENT_START +  5)
#define VFPC_INDEX_OMX_DEQUEUEINPUT_START          (VFPC_INDEX_EVENT_START +  6)
#define VFPC_INDEX_OMX_DEQUEUEINPUT_END            (VFPC_INDEX_EVENT_START +  7)
#define VFPC_INDEX_OMX_DEQUEUEOUTPUT_START         (VFPC_INDEX_EVENT_START +  8)
#define VFPC_INDEX_OMX_DEQUEUEOUTPUT_END           (VFPC_INDEX_EVENT_START +  9)
#define VFPC_INDEX_OMX_ACQUIREHISTORY_START        (VFPC_INDEX_EVENT_START + 10)
#define VFPC_INDEX_OMX_ACQUIREHISTORY_END          (VFPC_INDEX_EVENT_START + 11)
#define VFPC_INDEX_OMX_MAPOMX2FVIDPROCLIST_START   (VFPC_INDEX_EVENT_START + 12)
#define VFPC_INDEX_OMX_MAPOMX2FVIDPROCLIST_END     (VFPC_INDEX_EVENT_START + 13)
#define VFPC_INDEX_OMX_COPYHISTORYPIPE_START       (VFPC_INDEX_EVENT_START + 14)
#define VFPC_INDEX_OMX_COPYHISTORYPIPE_END         (VFPC_INDEX_EVENT_START + 15)
#define VFPC_INDEX_OMX_OUTPUTSUBSAMPLED            (VFPC_INDEX_EVENT_START + 16)
#define VFPC_INDEX_DRV_FRAMELIST_CREATED           (VFPC_INDEX_EVENT_START + 17)
#define VFPC_INDEX_DRV_FVID_DO_PROCESS_START       (VFPC_INDEX_EVENT_START + 18)
#define VFPC_INDEX_DRV_FVID_DO_PROCESS_END         (VFPC_INDEX_EVENT_START + 19)
#define VFPC_INDEX_DRV_FVID_PROCESS_DONE_CB        (VFPC_INDEX_EVENT_START + 20)
#define VFPC_INDEX_OMX_DIO_SEND_INPORTS_START      (VFPC_INDEX_EVENT_START + 21)
#define VFPC_INDEX_OMX_DIO_SEND_INPORTS_END        (VFPC_INDEX_EVENT_START + 22)
#define VFPC_INDEX_OMX_DIO_SEND_OUTPORTS_START     (VFPC_INDEX_EVENT_START + 23)
#define VFPC_INDEX_OMX_DIO_SEND_OUTPORTS_END       (VFPC_INDEX_EVENT_START + 24)
#define VFPC_INDEX_OMX_DIO_SEND_IOPORTS_START      (VFPC_INDEX_EVENT_START + 25)
#define VFPC_INDEX_OMX_DIO_SEND_IOPORTS_END        (VFPC_INDEX_EVENT_START + 26)
#define VFPC_INDEX_OMX_FREE_HISTORYBUF_START       (VFPC_INDEX_EVENT_START + 27)
#define VFPC_INDEX_OMX_FREE_HISTORYBUF_END         (VFPC_INDEX_EVENT_START + 28)
#define VFPC_INDEX_EVENT_END                       (VFPC_INDEX_OMX_FREE_HISTORYBUF_END)
#define VFPC_INDEX_EVENT_COUNT                    ((VFPC_INDEX_EVENT_END -     \
                                                   VFPC_INDEX_EVENT_START) + 1)

#define OMX_INDEX_VIDEOM3_EVENT_START               (0)
#define OMX_INDEX_IVAHD0_PROCESS_START              (OMX_INDEX_VIDEOM3_EVENT_START)
#define OMX_INDEX_IVAHD0_PROCESS_END                (OMX_INDEX_VIDEOM3_EVENT_START + 1)
#define OMX_INDEX_IVAHD1_PROCESS_START              (OMX_INDEX_VIDEOM3_EVENT_START + 2)
#define OMX_INDEX_IVAHD1_PROCESS_END                (OMX_INDEX_VIDEOM3_EVENT_START + 3)
#define OMX_INDEX_IVAHD2_PROCESS_START              (OMX_INDEX_VIDEOM3_EVENT_START + 4)
#define OMX_INDEX_IVAHD2_PROCESS_END                (OMX_INDEX_VIDEOM3_EVENT_START + 5)

#define OMX_INDEX_IVAHD_DONE_ENCODE_START           (OMX_INDEX_VIDEOM3_EVENT_START + 6)
#define OMX_INDEX_IVAHD_DONE_ENCODE_END             (OMX_INDEX_VIDEOM3_EVENT_START + 7)
#define OMX_INDEX_IVAHD_DONE_DECODE_START           (OMX_INDEX_VIDEOM3_EVENT_START + 8)
#define OMX_INDEX_IVAHD_DONE_DECODE_END             (OMX_INDEX_VIDEOM3_EVENT_START + 9)

#define OMX_INDEX_IVASCHEDULER_PUTQ                 (OMX_INDEX_VIDEOM3_EVENT_START + 10)
#define OMX_INDEX_IVASCHEDULER_GETQ                 (OMX_INDEX_VIDEOM3_EVENT_START + 11)
#define OMX_INDEX_IVASCHEDULER_GETDONE              (OMX_INDEX_VIDEOM3_EVENT_START + 12)
#define OMX_INDEX_IVASCHEDULER_PROCESSDONE          (OMX_INDEX_VIDEOM3_EVENT_START + 13)

#define OMX_INDEX_IVAHD0_HDVICPWAIT_START           (OMX_INDEX_VIDEOM3_EVENT_START + 14)
#define OMX_INDEX_IVAHD0_HDVICPWAIT_END             (OMX_INDEX_VIDEOM3_EVENT_START + 15)
#define OMX_INDEX_IVAHD1_HDVICPWAIT_START           (OMX_INDEX_VIDEOM3_EVENT_START + 16)
#define OMX_INDEX_IVAHD1_HDVICPWAIT_END             (OMX_INDEX_VIDEOM3_EVENT_START + 17)
#define OMX_INDEX_IVAHD2_HDVICPWAIT_START           (OMX_INDEX_VIDEOM3_EVENT_START + 18)
#define OMX_INDEX_IVAHD2_HDVICPWAIT_END             (OMX_INDEX_VIDEOM3_EVENT_START + 19)

#define OMX_INDEX_IVAHD0_HDVICPACQUIRE_START        (OMX_INDEX_VIDEOM3_EVENT_START + 20)
#define OMX_INDEX_IVAHD0_HDVICPACQUIRE_END          (OMX_INDEX_VIDEOM3_EVENT_START + 21)
#define OMX_INDEX_IVAHD1_HDVICPACQUIRE_START        (OMX_INDEX_VIDEOM3_EVENT_START + 22)
#define OMX_INDEX_IVAHD1_HDVICPACQUIRE_END          (OMX_INDEX_VIDEOM3_EVENT_START + 23)
#define OMX_INDEX_IVAHD2_HDVICPACQUIRE_START        (OMX_INDEX_VIDEOM3_EVENT_START + 24)
#define OMX_INDEX_IVAHD2_HDVICPACQUIRE_END          (OMX_INDEX_VIDEOM3_EVENT_START + 25)

#define OMX_INDEX_IVAHD0_SWIPOST_START              (OMX_INDEX_VIDEOM3_EVENT_START + 26)
#define OMX_INDEX_IVAHD0_SWIPOST_END                (OMX_INDEX_VIDEOM3_EVENT_START + 27)
#define OMX_INDEX_IVAHD1_SWIPOST_START              (OMX_INDEX_VIDEOM3_EVENT_START + 28)
#define OMX_INDEX_IVAHD1_SWIPOST_END                (OMX_INDEX_VIDEOM3_EVENT_START + 29)
#define OMX_INDEX_IVAHD2_SWIPOST_START              (OMX_INDEX_VIDEOM3_EVENT_START + 30)
#define OMX_INDEX_IVAHD2_SWIPOST_END                (OMX_INDEX_VIDEOM3_EVENT_START + 31)

#define OMX_INDEX_IVAHD0_SWI_START                  (OMX_INDEX_DIO_EVENT_LAST + 1) /* 54 */
#define OMX_INDEX_IVAHD0_SWI_END                    (OMX_INDEX_IVAHD0_SWI_START + 1)
#define OMX_INDEX_IVAHD1_SWI_START                  (OMX_INDEX_IVAHD0_SWI_START + 2)
#define OMX_INDEX_IVAHD1_SWI_END                    (OMX_INDEX_IVAHD0_SWI_START + 3)
#define OMX_INDEX_IVAHD2_SWI_START                  (OMX_INDEX_IVAHD0_SWI_START + 4)
#define OMX_INDEX_IVAHD2_SWI_END                    (OMX_INDEX_IVAHD0_SWI_START + 5)
#define OMX_INDEX_IVAHD_SWI_EVENT_LAST              (OMX_INDEX_IVAHD2_SWI_END)

#define OMX_INDEX_VENC_EVENT_START                 (OMX_INDEX_IVAHD_SWI_EVENT_LAST + 1) /* 60 */
#define OMX_INDEX_VENC_CHANNELREADY                (OMX_INDEX_VENC_EVENT_START + 0)
#define OMX_INDEX_VENC_INBUF_DEQUEUE_START         (OMX_INDEX_VENC_EVENT_START + 1)
#define OMX_INDEX_VENC_INBUF_DEQUEUE_END           (OMX_INDEX_VENC_EVENT_START + 2)
#define OMX_INDEX_VENC_OUTBUF_DEQUEUE_START        (OMX_INDEX_VENC_EVENT_START + 3)
#define OMX_INDEX_VENC_OUTBUF_DEQUEUE_END          (OMX_INDEX_VENC_EVENT_START + 4)
#define OMX_INDEX_VENC_IVASERVERPUT                (OMX_INDEX_VENC_EVENT_START + 5)
#define OMX_INDEX_VENC_INBUF_FREE_START            (OMX_INDEX_VENC_EVENT_START + 6)
#define OMX_INDEX_VENC_INBUF_FREE_END              (OMX_INDEX_VENC_EVENT_START + 7)
#define OMX_INDEX_VENC_OUTBUF_SEND_START           (OMX_INDEX_VENC_EVENT_START + 8)
#define OMX_INDEX_VENC_OUTBUF_SEND_END             (OMX_INDEX_VENC_EVENT_START + 9)
#define OMX_INDEX_VENC_EVENT_LAST                  (OMX_INDEX_VENC_OUTBUF_SEND_END)

#define OMX_INDEX_VDEC_EVENT_START                 (OMX_INDEX_VENC_EVENT_LAST + 1) /* 70 */
#define OMX_INDEX_VDEC_CHANNELREADY                (OMX_INDEX_VDEC_EVENT_START + 0)
#define OMX_INDEX_VDEC_INBUF_DEQUEUE_START         (OMX_INDEX_VDEC_EVENT_START + 1)
#define OMX_INDEX_VDEC_INBUF_DEQUEUE_END           (OMX_INDEX_VDEC_EVENT_START + 2)
#define OMX_INDEX_VDEC_OUTBUF_DEQUEUE_START        (OMX_INDEX_VDEC_EVENT_START + 3)
#define OMX_INDEX_VDEC_OUTBUF_DEQUEUE_END          (OMX_INDEX_VDEC_EVENT_START + 4)
#define OMX_INDEX_VDEC_IVASERVERPUT                (OMX_INDEX_VDEC_EVENT_START + 5)
#define OMX_INDEX_VDEC_DUP_START                   (OMX_INDEX_VDEC_EVENT_START + 6)
#define OMX_INDEX_VDEC_DUP_END                     (OMX_INDEX_VDEC_EVENT_START + 7)
#define OMX_INDEX_VDEC_OUTBUF_SEND_START           (OMX_INDEX_VDEC_EVENT_START + 8)
#define OMX_INDEX_VDEC_OUTBUF_SEND_END             (OMX_INDEX_VDEC_EVENT_START + 9)
#define OMX_INDEX_VDEC_INBUF_FREE_START            (OMX_INDEX_VDEC_EVENT_START + 10)
#define OMX_INDEX_VDEC_INBUF_FREE_END              (OMX_INDEX_VDEC_EVENT_START + 11)
#define OMX_INDEX_VDEC_EVENT_LAST                  (OMX_INDEX_VDEC_INBUF_FREE_END)

#define OMX_INDEX_VIDSPLC_EVENT_START              (OMX_INDEX_VDEC_EVENT_LAST + 1)    /* 81 */
#define OMX_INDEX_VIDSPLC_CHANNELREADY             (OMX_INDEX_VIDSPLC_EVENT_START + 0)
#define OMX_INDEX_VIDSPLC_INBUF_DEQUEUE_START      (OMX_INDEX_VIDSPLC_EVENT_START + 1)
#define OMX_INDEX_VIDSPLC_INBUF_DEQUEUE_END        (OMX_INDEX_VIDSPLC_EVENT_START + 2)
#define OMX_INDEX_VIDSPLC_OUTBUF_DEQUEUE_START     (OMX_INDEX_VIDSPLC_EVENT_START + 3)
#define OMX_INDEX_VIDSPLC_OUTBUF_DEQUEUE_END       (OMX_INDEX_VIDSPLC_EVENT_START + 4)
#define OMX_INDEX_VIDSPLC_IVASERVERPUT             (OMX_INDEX_VIDSPLC_EVENT_START + 5)
#define OMX_INDEX_VIDSPLC_DUP_START                (OMX_INDEX_VIDSPLC_EVENT_START + 6)
#define OMX_INDEX_VIDSPLC_DUP_END                  (OMX_INDEX_VIDSPLC_EVENT_START + 7)
#define OMX_INDEX_VIDSPLC_OUTBUF_SEND_START        (OMX_INDEX_VIDSPLC_EVENT_START + 8)
#define OMX_INDEX_VIDSPLC_OUTBUF_SEND_END          (OMX_INDEX_VIDSPLC_EVENT_START + 9)
#define OMX_INDEX_VIDSPLC_INBUF_FREE_START         (OMX_INDEX_VIDSPLC_EVENT_START + 10)
#define OMX_INDEX_VIDSPLC_INBUF_FREE_END           (OMX_INDEX_VIDSPLC_EVENT_START + 11)
#define OMX_INDEX_VIDSPLC_EVENT_LAST               (OMX_INDEX_VIDSPLC_INBUF_FREE_END)


#define OMX_INDEX_IVASERVER_GETPROCESSEDCHANNEL_START (OMX_INDEX_VIDSPLC_EVENT_LAST + 1) /* 92 */
#define OMX_INDEX_IVASERVER_GETPROCESSEDCHANNEL_END   (OMX_INDEX_IVASERVER_GETPROCESSEDCHANNEL_START + 1)
#define OMX_INDEX_IVASERVER_EVENT_LAST                (OMX_INDEX_IVASERVER_GETPROCESSEDCHANNEL_END)

#define OMX_INDEX_VSWMOSAIC_EDMACALLBACK_START (OMX_INDEX_IVASERVER_EVENT_LAST + 1) /* 84 */
#define OMX_INDEX_VSWMOSAIC_EDMACALLBACK_END   (OMX_INDEX_IVASERVER_EVENT_LAST + 2) /* 85 */
#define OMX_INDEX_VSWMOSAIC_EVENT_LAST         (OMX_INDEX_VSWMOSAIC_EDMACALLBACK_END)

/*----------          function prototypes      ------------------- */
OMX_ERRORTYPE OMX_UTL_CreateFQBufferManager (OMX_IN OMX_U32 uTunnelCnt,
                                  OMX_IN OMX_UTL_TUNNEL_CONTEXT asTunnelCtxt [],
                                  OMX_IN OMX_STRING fQBufMgrName);

OMX_ERRORTYPE OMX_UTL_CreateFQBufferManagerWithPoolInfo(OMX_IN
                                OMX_PARM_FQMGRCREATIONPARM *pFQBufMgrInfo,
                                  OMX_IN OMX_STRING fQBufMgrName);

OMX_ERRORTYPE OMX_UTL_AssignFQBufferManager (OMX_IN OMX_U32 uTunnelCnt,
                                  OMX_IN OMX_UTL_TUNNEL_CONTEXT asTunnelCtxt [],
                                  OMX_IN OMX_U8 auPooId[],
                                  OMX_IN OMX_STRING fQBufMgrName);

OMX_ERRORTYPE OMX_UTL_DeleteFQBufferManager (OMX_IN OMX_STRING fQBufMgrName);

OMX_ERRORTYPE OMX_UTL_InitFQBufferManagerCreateUtility ();

OMX_ERRORTYPE omx_util_map_chromafmt2bufsize(
                                       OMX_PARAM_PORTDEFINITIONTYPE* pPortDef,
                                       OMX_U32 nMaxNumFrameBufsInFrame,
                                       OMX_U32* pNumFrameBufsInFrame,
                                       OMX_U32 pBufferSize[]);

/******************************************************************
 *                          UIA MACROS & PROTOTYPES
 ******************************************************************/
extern Ptr  g_IntHeapMemory_Handle_0; 
extern Ptr  g_IntHeapMemory_Handle_1; 
extern Ptr  g_IntHeapMemory_Handle_2; 
extern Ptr  gOmxUtils_heapMemFrameHandle;

UInt32 OmxUtils_getHeapMemStats(Ptr heapMemHandle);
UInt32 DucatiLocalMemMgr_GetHeapMemStats();
void OmxUtils_getAllVideoM3HeapMemStats(Uint32 * heapSize);
void OmxUtils_getAllVpssM3HeapMemStats(Uint32 * heapSize);
OMX_ERRORTYPE OMX_GetHeapMemStats(OMX_IN  OMX_U32 * totalFreeSize);

#ifndef _LOCAL_CORE_a8host_
#define OMX_UTL_MONITOR_ALL_VIDEOM3_HEAPS() \
    Log_write5(UIAStatistic_freeBytes, (IArg)__FILE__, (IArg)__LINE__, 0, OmxUtils_getHeapMemStats((Ptr)NULL), (IArg)hComponent); \
    if(g_IntHeapMemory_Handle_0 != NULL) Log_write5(UIAStatistic_freeBytes, (IArg)__FILE__, (IArg)__LINE__, 1, OmxUtils_getHeapMemStats((Ptr)g_IntHeapMemory_Handle_0), (IArg)hComponent); \
    if(g_IntHeapMemory_Handle_1 != NULL) Log_write5(UIAStatistic_freeBytes, (IArg)__FILE__, (IArg)__LINE__, 2, OmxUtils_getHeapMemStats((Ptr)g_IntHeapMemory_Handle_1), (IArg)hComponent); \
    Log_write5(UIAStatistic_freeBytes, (IArg)__FILE__, (IArg)__LINE__, 3, DucatiLocalMemMgr_GetHeapMemStats(), (IArg)hComponent); \
    if(gOmxUtils_heapMemFrameHandle != NULL) Log_write5(UIAStatistic_freeBytes, (IArg)__FILE__, (IArg)__LINE__, 4, OmxUtils_getHeapMemStats((Ptr)gOmxUtils_heapMemFrameHandle), (IArg)hComponent)

#define OMX_UTL_MONITOR_ALL_VPSSM3_HEAPS() \
    Log_write5(UIAStatistic_freeBytes, (IArg)__FILE__, (IArg)__LINE__, 0, OmxUtils_getHeapMemStats((Ptr)NULL), (IArg)hComponent); \
    if(g_IntHeapMemory_Handle_2 != NULL) Log_write5(UIAStatistic_freeBytes, (IArg)__FILE__, (IArg)__LINE__, 1, OmxUtils_getHeapMemStats((Ptr)g_IntHeapMemory_Handle_2), (IArg)hComponent); \
    if(gOmxUtils_heapMemFrameHandle != NULL) Log_write5(UIAStatistic_freeBytes, (IArg)__FILE__, (IArg)__LINE__, 2, OmxUtils_getHeapMemStats((Ptr)gOmxUtils_heapMemFrameHandle), (IArg)hComponent)

#define UIA_LOGGING

#ifdef UIA_LOGGING
#define OMX_UTL_PRINTHEAPSTATS(compName,msg,handle,heapId,freeSize) Log_print5(Diags_USER4,\
"M3 Heap Stats -> Component %s Handle 0x%x  Execution@ %s HeapId %d FreeSize(bytes) %d",\
(IArg)compName,(IArg)handle,(IArg)msg,heapId,freeSize)
#else
#define OMX_UTL_PRINTHEAPSTATS(compName,msg,handle,heapId,freeSize) System_printf(\
"M3 Heap Stats -> Component %s Handle 0x%x  Execution@ %s HeapId %d FreeSize(bytes) %d\n",\
(IArg)compName,(IArg)handle,(IArg)msg,heapId,freeSize)
#endif

#define OMX_UTL_PRINTSTATS_ALL_VIDEOM3_HEAPS(compName,msg) \
    OMX_UTL_PRINTHEAPSTATS(compName,msg,hComponent,0,OmxUtils_getHeapMemStats((Ptr)NULL)); \
    if(g_IntHeapMemory_Handle_0 != NULL) OMX_UTL_PRINTHEAPSTATS(compName,msg,hComponent,1,OmxUtils_getHeapMemStats((Ptr)g_IntHeapMemory_Handle_0)); \
    if(g_IntHeapMemory_Handle_1 != NULL) OMX_UTL_PRINTHEAPSTATS(compName,msg,hComponent,2,OmxUtils_getHeapMemStats((Ptr)g_IntHeapMemory_Handle_1)); \
    OMX_UTL_PRINTHEAPSTATS(compName,msg,hComponent,3,DucatiLocalMemMgr_GetHeapMemStats()); \
    if(gOmxUtils_heapMemFrameHandle != NULL) OMX_UTL_PRINTHEAPSTATS(compName,msg,hComponent,4,OmxUtils_getHeapMemStats((Ptr)gOmxUtils_heapMemFrameHandle))

#define OMX_UTL_PRINTSTATS_ALL_VPSSM3_HEAPS(compName,msg) \
    OMX_UTL_PRINTHEAPSTATS(compName,msg,hComponent,0,OmxUtils_getHeapMemStats((Ptr)NULL)); \
    if(g_IntHeapMemory_Handle_2 != NULL) OMX_UTL_PRINTHEAPSTATS(compName,msg,hComponent,1,OmxUtils_getHeapMemStats((Ptr)g_IntHeapMemory_Handle_2)); \
    if(gOmxUtils_heapMemFrameHandle != NULL)OMX_UTL_PRINTHEAPSTATS(compName,msg,hComponent,2,OmxUtils_getHeapMemStats((Ptr)gOmxUtils_heapMemFrameHandle))

#endif /*#ifndef _LOCAL_CORE_a8host_*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

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

#endif /* _OMX_UTL_UTILS_H_ */
