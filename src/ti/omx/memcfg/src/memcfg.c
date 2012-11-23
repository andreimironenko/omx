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
 *******************************************************************************
 *   @file  memcfg.c
 *  @brief  This file contains TI's implementation of the OMX methods for memcfg
 *          component
 *
 *  @rev  1.0
 *******************************************************************************
 */

/*******************************************************************************
*                             Compilation Control Switches
*******************************************************************************/
/* None */

/*******************************************************************************
*                             INCLUDE FILES
*******************************************************************************/
/*--------------------- system and platform files ----------------------------*/
#include <stdint.h>

#include <xdc/std.h>
#include <xdc/runtime/Gate.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/IHeap.h>
#include <xdc/runtime/Timestamp.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/sdo/ipc/SharedRegion.h>
#include <ti/ipc/MultiProc.h>

#include <ti/omx/domx/domx.h>

/*-------------------------program files -------------------------------------*/
#include "memcfg.h"

#include "timm_osal_interfaces.h"

/*******************************************************************************
 * EXTERNAL REFERENCES NOTE : only use if not found in header file
*******************************************************************************/

/*------------------------ data declarations ---------------------------------*/
extern const MEMCFG_sharedRegionConfig MEMCFG_sharedRegionConfigTable;
extern MEMCFG_Module_State *MEMCFG_module;

/*----------------------- function prototypes --------------------------------*/
extern Ptr DomxCore_mapPhyAddr2UsrVirtual (UInt32 phyAddr, UInt32 len);
extern Void DomxCore_unmapPhyAddr (UInt32 phyAddr, UInt32 len);

/*******************************************************************************
 * PUBLIC DECLARATIONS Defined here, used elsewhere
 ******************************************************************************/

/*----------------------- function prototypes --------------------------------*/
/*None*/

/*------------------------ data declarations ---------------------------------*/
/*None*/

/*******************************************************************************
 * PRIVATE DECLARATIONS Defined here, used only here
 ******************************************************************************/

/*--------------------------- macros  ----------------------------------------*/
/*None*/

/*----------------------- function prototypes --------------------------------*/
static int32_t memcfg_module_init_shared_region (void);

/*============================================================================*/

/** 
********************************************************************************
 *  @func     memcfg_module_init_shared_region
 *  @brief  This function performs initialization of shared regions.
 *
 *          This functions gets a virtual address by calling 
 *          DomxCore_mapPhyAddr2UsrVirtual(). Checks if the address already 
 *          available the table. If not it creates an entry and sets the entry
 *          by calling SharedRegion_setEntry.
 *
 *  @param[in ]  None  : None
 * 
 *  @returns MEMCFG_E_FAIL 
 *           MEMCFG_S_SUCCESS
********************************************************************************
*/
static int32_t memcfg_module_init_shared_region (void)
{
  int32_t i, j;
  int32_t nSR_retval;
  SharedRegion_Entry srEntry;
  uint16_t srOwnerProcId;
  uint16_t srIndex;
  int32_t createHeap;
  int32_t srStatus = SharedRegion_S_SUCCESS;
  void *srBaseVirtual = NULL;
  int32_t memcfg_retval = MEMCFG_S_SUCCESS;
  int32_t aSRIndices[MEMCFG_MAXNUMSHAREDREGIONS];
  int16_t sridx_local = 0;
  int16_t k;

  /*--------------------------------------------------------------------------*/
  /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
  /* Following are the initialization steps for each of the shared regions: */
  /* 1. Convert the base address into user space virtual */
  /* 2. Derived to Base params with Actual values */
  /* 3. Get entry for a specific shared region index */
  /* 4. If the shared region is not valid, then set the shared region */
  /* 5. If the shared region is already there, then check if the values are */
  /* correct.  */
  /* */
  /*--------------------------------------------------------------------------*/

  /* This needs to be fixed. SR1 is in cacheable region for M3, but the heap is 
     created by the master A8 which updates the Heap attributes in SR1, thus
     writing to it. If MEMCFGModule_init on M3 runs before A8, then the memory
     corresponding to the Heap attrs gets cached by M3 and thus
     SharedRegion_setEntry keeps on failing despite A8 successfully creating
     the heap. IPC probably needs to communicate heap attrs via SR0? The fix
     below ensures SR1 is created by A8 before M3 checks for it, as M3 waits
     for SR2 creation before SR1 and SR2 is in non-cacheable region. In short, 
     on A8, SRs which does not require cache operations are created first and
     then those which does needs cache operations. On M3, reverse happens. */
  if (DomxTypes_coreCortexA8 == MultiProc_self ())
  {                             /* Master/SR Owner */
    /* On master, create the order such that all the SRs that does not need
       cache operations go first... */
    for (k = 0; k < MEMCFG_sharedRegionConfigTable.nNumSharedRegions; k++)
    {
      if (!MEMCFG_sharedRegionConfigTable.aSRInfo[k].bCacheEnable)
      {                         /* if the SR does NOT need cache operation, put 
                                   that in */
        aSRIndices[sridx_local++] =
          MEMCFG_sharedRegionConfigTable.aSRInfo[k].uIndex;
      }                         /* if */
    }                           /* for */
    /* ... and then all SRs that need cache operation go next */
    for (k = 0; k < MEMCFG_sharedRegionConfigTable.nNumSharedRegions; k++)
    {
      if (MEMCFG_sharedRegionConfigTable.aSRInfo[k].bCacheEnable)
      {                         /* if the SR needs cache operation, put that in 
                                 */
        aSRIndices[sridx_local++] =
          MEMCFG_sharedRegionConfigTable.aSRInfo[k].uIndex;
      }                         /* if */
    }                           /* for */
  }                             /* if (DomxTypes_coreCortexA8...) */
  else
  {                             /* Slaves */
    /* On slaves, create the order such that all the SRs that needs cache
       operations go first... */
    for (k = 0; k < MEMCFG_sharedRegionConfigTable.nNumSharedRegions; k++)
    {
      if (MEMCFG_sharedRegionConfigTable.aSRInfo[k].bCacheEnable)
      {                         /* if the SR is NOT cached, put that in */
        aSRIndices[sridx_local++] =
          MEMCFG_sharedRegionConfigTable.aSRInfo[k].uIndex;
      }                         /* if */
    }                           /* for */
    /* ... and then all SRs does not need cache operation go next */
    for (k = 0; k < MEMCFG_sharedRegionConfigTable.nNumSharedRegions; k++)
    {
      if (!MEMCFG_sharedRegionConfigTable.aSRInfo[k].bCacheEnable)
      {                         /* if the SR is cached, put that in */
        aSRIndices[sridx_local++] =
          MEMCFG_sharedRegionConfigTable.aSRInfo[k].uIndex;
      }                         /* if */
    }                           /* for */
  }                             /* if (DomxTypes_coreCortexA8) else ... */

  for (j = 0; j < MEMCFG_sharedRegionConfigTable.nNumSharedRegions; j++)
  {
    i = aSRIndices[j];
    srBaseVirtual = NULL;
    /*------------------------------------------------------------------------*/
    /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
    /* Convert the physical address to virtual address.  */
    /* */
    /*------------------------------------------------------------------------*/
    srBaseVirtual =
      DomxCore_mapPhyAddr2UsrVirtual (MEMCFG_sharedRegionConfigTable.aSRInfo[i].
                                      uBase,
                                      MEMCFG_sharedRegionConfigTable.aSRInfo[i].
                                      uSize);
    /*------------------------------------------------------------------------*/
    /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
    /* Get the entry for the specific shared region index.  */
    /* */
    /*------------------------------------------------------------------------*/
    nSR_retval =
      SharedRegion_getEntry (MEMCFG_sharedRegionConfigTable.aSRInfo[i].uIndex,
                             &srEntry);
    if (nSR_retval == SharedRegion_S_SUCCESS)
    {
      if (FALSE == srEntry.isValid)
      {
        /*--------------------------------------------------------------------*/
        /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
        /* If the shared region is not included, then set a new entry.  */
        /* */
        /*--------------------------------------------------------------------*/
        SharedRegion_entryInit (&srEntry);
        MEMCFG_module->bSrAddEntryDone[i] = TRUE;
        if (MEMCFG_sharedRegionTypeMaster ==
            MEMCFG_sharedRegionConfigTable.aSRInfo[i].srType)
        {
          srOwnerProcId = MultiProc_self ();
          createHeap = TRUE;
        }
        else
        {
          srOwnerProcId = SharedRegion_DEFAULTOWNERID;
          createHeap = TRUE;
        }
        srEntry.base = srBaseVirtual;
        srIndex = MEMCFG_sharedRegionConfigTable.aSRInfo[i].uIndex;
        srEntry.len = MEMCFG_sharedRegionConfigTable.aSRInfo[i].uSize;
        srEntry.cacheEnable =
          MEMCFG_sharedRegionConfigTable.aSRInfo[i].bCacheEnable;
        srEntry.createHeap = createHeap;
        srEntry.isValid = TRUE;
        srEntry.ownerProcId = srOwnerProcId;

        Log_print2 (Diags_USER1, "Calling SharedRegion_setEntry (%d, 0x%x)",
                    srIndex, (xdc_IArg) & srEntry);
        Log_print3 (Diags_USER1, "base : 0x%x, len: 0x%x, ownerProcId: %d\n",
                    (xdc_IArg) srEntry.base, srEntry.len, srEntry.ownerProcId);

        do
        {
          /*------------------------------------------------------------------*/
          /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
          /* If the shared region is not included, then set a new entry.  */
          /* */
          /*------------------------------------------------------------------*/
          srStatus = SharedRegion_setEntry (srIndex, &srEntry);

          if (srStatus != SharedRegion_S_SUCCESS)
          {
            /*----------------------------------------------------------------*/
            /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
            /* If the shared region is not successful, retry for */
            /* MEMCFG_MAX_NUM_TRIALS */
            /* */
            /*----------------------------------------------------------------*/
            Log_print3 (Diags_USER1,
                        "SharedRegion_setEntry (%d, 0x%x) Failed, Status %d",
                        srIndex, (xdc_IArg) & srEntry, srStatus);
          }
        }
        while (srStatus != SharedRegion_S_SUCCESS);
      }                         /* if (FALSE == srEntry..isValid) { */
      else
      {
        /*--------------------------------------------------------------------*/
        /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
        /* There is already an entry. Check it has valid information.  */
        /* */
        /*--------------------------------------------------------------------*/
        Assert_isTrue (((srEntry.base == srBaseVirtual) &&
                        (srEntry.len ==
                         MEMCFG_sharedRegionConfigTable.aSRInfo[i].uSize)),
                       Assert_E_assertFailed);
        MEMCFG_module->bSrAddEntryDone[i] = FALSE;
      }
    }                           /* End of IF block: if (nSR_retval ==
                                   SharedRegion_S_SUCCESS) */
    else
    {
      /* if (nSR_retval != SharedRegion_S_SUCCESS) */
      memcfg_retval = MEMCFG_E_FAIL;
      goto EXIT;
    }
  }                             /* End of FOR block: for (i = 0; i <
                                   MEMCFG_sharedRegionConfigTable.nNumSharedRegions; 
                                   i++) */

EXIT:
  return memcfg_retval;
}                               /* memcfg_module_init_shared_region */

/** 
********************************************************************************
 *  @func   memcfg_module_deinit_shared_region
 *  @brief  This function performs de-initialization of shared regions.
 *
 *          This functions removes the sharedRegions added by the MemCfg module
 *          and also unmaps the corresponding physical memory from usr space.
 *
 *  @param[in ]  None  : None
 * 
 *  @returns MEMCFG_E_FAIL 
 *           MEMCFG_S_SUCCESS
********************************************************************************
*/
static int32_t memcfg_module_deinit_shared_region (void)
{
  int32_t srStatus = SharedRegion_S_SUCCESS;
  int32_t i;
  int32_t memcfg_retval = MEMCFG_S_SUCCESS;

  for (i = 0; i < MEMCFG_sharedRegionConfigTable.nNumSharedRegions; i++)
  {
    if (MEMCFG_module->bSrAddEntryDone[i])
    {
      srStatus =
        SharedRegion_clearEntry (MEMCFG_sharedRegionConfigTable.aSRInfo[i].
                                 uIndex);
      if (SharedRegion_S_SUCCESS == srStatus)
      {
        DomxCore_unmapPhyAddr (MEMCFG_sharedRegionConfigTable.aSRInfo[i].uBase,
                               MEMCFG_sharedRegionConfigTable.aSRInfo[i].uSize);
        MEMCFG_module->bSrAddEntryDone[i] = FALSE;
      }
      else
      {
        memcfg_retval = MEMCFG_E_FAIL;
      }
    }
  }
  return memcfg_retval;
}

/** 
********************************************************************************
 *  @func     MEMCFG_ModuleInit
 *  @brief  This function performs initialization of shared regions and set the
 *          Module instance appropriately.
 *
 *  @param[in ]  None  : None
 * 
 *  @returns MEMCFG_MODSTAT_INITIALIZED 
 *           MEMCFG_MODSTAT_NOTINITIALIZED
********************************************************************************
*/
uint32_t MEMCFG_ModuleInit (void)
{
  IArg key;
  int32_t memcfg_retval;

  key = Gate_enterModule ();

  if (MEMCFG_MODSTAT_NOTINITIALIZED == MEMCFG_module->uInitState)
  {
    memcfg_retval = memcfg_module_init_shared_region ();

    if (MEMCFG_S_SUCCESS == memcfg_retval)
    {
      MEMCFG_module->uInitState = MEMCFG_MODSTAT_INITIALIZED;
    }

  }                             /* End of IF: if (MEMCFG_MODSTAT_NOTINITIALIZED 
                                   == uInitState) */

  Gate_leaveModule (key);

  return (MEMCFG_module->uInitState);

}                               /* MEMCFG_ModuleInit */

/** 
********************************************************************************
 *  @func     MEMCFG_ModuleDeInit
 *  @brief  This function performs de-initialization of shared regions and set the
 *          Module instance appropriately.
 *
 *  @param[in ]  None  : None
 * 
 *  @returns MEMCFG_MODSTAT_INITIALIZED 
 *           MEMCFG_MODSTAT_NOTINITIALIZED
********************************************************************************
*/
uint32_t MEMCFG_ModuleDeInit (void)
{
  IArg key;
  int32_t memcfg_retval;

  key = Gate_enterModule ();

  if (MEMCFG_MODSTAT_INITIALIZED == MEMCFG_module->uInitState)
  {
    memcfg_retval = memcfg_module_deinit_shared_region ();

    if (MEMCFG_S_SUCCESS == memcfg_retval)
    {
      MEMCFG_module->uInitState = MEMCFG_MODSTAT_NOTINITIALIZED;
    }

  }                             /* End of IF: if (MEMCFG_MODSTAT_NOTINITIALIZED 
                                   == uInitState) */

  Gate_leaveModule (key);

  return (MEMCFG_module->uInitState);

}                               /* MEMCFG_ModuleInit */

/* memcfg.c - EOF */
