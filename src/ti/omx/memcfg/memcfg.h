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
 *  @file   memcfg.h
 *  @brief  This interface header file contains structure definitions, function 
 *          Headers for the memory configuration module.
 *
 *  @rev 1.0
 *******************************************************************************
 */

#ifndef _MEMCFG_H
#define  _MEMCFG_H

#ifdef _cplusplus
extern "C"
{
#endif                          /* _cplusplus */

/*******************************************************************************
*                             Compilation Control Switches
*******************************************************************************/

/*******************************************************************************
*                             INCLUDE FILES
*******************************************************************************/
/*--------------------- system and platform files ----------------------------*/
#include <stdint.h>

/*-------------------------program files -------------------------------------*/
/*None*/

/*******************************************************************************
 * PUBLIC DECLARATIONS Defined here, used elsewhere
 ******************************************************************************/

/*--------------------------Data declarations --------------------------------*/

/*--------------------------Enumerated Types  --------------------------------*/

/* MEMCFG_sharedRegionType_e - This enum is used to distinguish between master
   and slaves of Shared Regions */
typedef enum MEMCFG_sharedRegionType_e
{
  MEMCFG_sharedRegionTypeMaster,
  MEMCFG_sharedRegionTypeSlave
} MEMCFG_sharedRegionType_e;

/* MEMCFG_sharedRegionMemoryType_e - This enum is used to specify the way the
   Shared Regions are going to be used - to share data or to share control
   information */
typedef enum MEMCFG_sharedRegionMemoryType_e
{
  MEMCFG_sharedRegionMemoryTypeControl,
  MEMCFG_sharedRegionMemoryTypeData
} MEMCFG_sharedRegionMemoryType_e;

/*--------------------------- macros  ----------------------------------------*/
/** 
 *  MEMCFG_MAXNUMSHAREDREGIONS, provides the Max. no of shared regions 
 *  possible in the system.
 */
#define MEMCFG_MAXNUMSHAREDREGIONS       ((uint32_t)4)

#define MEMCFG_S_SUCCESS  (((uint32_t)0))
#define MEMCFG_E_FAIL     (((uint32_t)1))

#define MEMCFG_MODSTAT_INITIALIZED         ((uint32_t)1)
#define MEMCFG_MODSTAT_NOTINITIALIZED      ((uint32_t)0)

/*------------------------- Strutures ----------------------------------------*/
/**
 *******************************************************************************
 *  @struct  MEMCFG_sharedRegionInfo_s - 
 *  @brief   Structure Per Shared region
 *
 *  @ param srType          : Shared region type
 *  @ param uIndex          : Index of the shared region
 *  @ param uBase           : Base address of the shared region
 *  @ param uSize           : Size of the shared in MAU(bytes)
 *  @ param bCacheEnable    : Bool specifying if cache is enabled
 *  @ param memType         : Shared region memory type
 *******************************************************************************
 */
typedef struct MEMCFG_sharedRegionInfo_s
{
  MEMCFG_sharedRegionType_e srType;
  uint32_t uIndex;
  uint32_t uBase;
  uint32_t uSize;
  uint32_t bCacheEnable;
  MEMCFG_sharedRegionMemoryType_e memType;
} MEMCFG_sharedRegionInfo;

/**
 *******************************************************************************
 *  @struct  MEMCFG_sharedRegionConfig_s - 
 *  @brief   Structure specifying shared region of the whole system
 *
 *  @ param nNumSharedRegions  : Total no. of shared regions
 *  @ param aSRInfo            : Array of shared region informations
 *******************************************************************************
 */
typedef struct MEMCFG_sharedRegionConfig_s
{
  int32_t nNumSharedRegions;
  MEMCFG_sharedRegionInfo aSRInfo[MEMCFG_MAXNUMSHAREDREGIONS];
} MEMCFG_sharedRegionConfig;

/**
 *******************************************************************************
 *  @struct  MEMCFG_Module_State_s - 
 *  @brief   Structure specifying MEMCFG module state
 *
 *  @ param uInitState  : Bool value specifying intialized or not
 *  @ param bSrAddEntryDone : Bool specifying if SrEntry was added already or not
 *******************************************************************************
 */
typedef struct MEMCFG_Module_State_s
{
  uint32_t uInitState;
  uint32_t bSrAddEntryDone[MEMCFG_MAXNUMSHAREDREGIONS];
} MEMCFG_Module_State;

/*--------------------------Data declarations --------------------------------*/
#ifndef MEMCFG_LOCAL
  extern const MEMCFG_sharedRegionConfig MEMCFG_sharedRegionConfigTable;
#endif

/*----------------------function prototypes ---------------------------------*/
uint32_t MEMCFG_ModuleInit (void);
uint32_t MEMCFG_ModuleDeInit (void);

#ifdef _cplusplus
}
#endif                          /* __cplusplus */

#endif                          /* #define _MEMCFG_H */

/* memcfg.h - EOF */
