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
 *  @file   memtbl_cfg.h
 *  @brief  This header file contains default values of configuration parameters
 *          of MEMCFG module
 *
 *  @rev 1.0
 *******************************************************************************
 */

#ifndef _MEMTBL_CFG_H
#define  _MEMTBL_CFG_H

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

/** 
 *  MEMCFG_NUMFQSHAREDREGIONS, used for providing the actual no of shared regions 
 *  used in the system.
 */
#ifndef MEMCFG_NUMFQSHAREDREGIONS
  #define MEMCFG_NUMFQSHAREDREGIONS       ((int32_t) 3)
#endif

/** 
 *  MEMCFG_SRTYPE0, MEMCFG_SRINDEX0, MEMCFG_SRBASE0, MEMCFG_SRSIZE0 
 *  MEMCFG_SRCACHEENABLE0 MEMCFG_SRMEMTYPE0 provide the default values of shared 
 *  region0
 */
#ifndef MEMCFG_SRTYPE0
  #define MEMCFG_SRTYPE0                  (MEMCFG_sharedRegionTypeSlave)
#endif

#ifndef MEMCFG_SRINDEX0
  #define MEMCFG_SRINDEX0                 ((uint32_t)0x0)
#endif

#ifndef MEMCFG_SRBASE0
  #define MEMCFG_SRBASE0                  ((uint32_t)0x9F700000)
#endif

#ifndef MEMCFG_SRSIZE0
  #define MEMCFG_SRSIZE0                  ((uint32_t)0x00200000)
#endif

#ifndef MEMCFG_SRCACHEENABLE0
  #define MEMCFG_SRCACHEENABLE0           ((uint32_t)1)
#endif

#ifndef MEMCFG_SRMEMTYPE0
  #define MEMCFG_SRMEMTYPE0               (MEMCFG_sharedRegionMemoryTypeControl)
#endif

/** 
 *  MEMCFG_SRTYPE1, MEMCFG_SRINDEX1, MEMCFG_SRBASE1, MEMCFG_SRSIZE1 
 *  MEMCFG_SRCACHEENABLE1 MEMCFG_SRMEMTYPE1 provide the default values of shared 
 *  region1
 */
#ifndef MEMCFG_SRTYPE1
  #define MEMCFG_SRTYPE1                  (MEMCFG_sharedRegionTypeSlave)
#endif

#ifndef MEMCFG_SRINDEX1
  #define MEMCFG_SRINDEX1                 ((uint32_t)0x1)
#endif

#ifndef MEMCFG_SRBASE1
  #define MEMCFG_SRBASE1                  ((uint32_t)0x9A100000)
#endif

#ifndef MEMCFG_SRSIZE1
  #define MEMCFG_SRSIZE1                  ((uint32_t)0x00100000)
#endif

#ifndef MEMCFG_SRCACHEENABLE1
  #define MEMCFG_SRCACHEENABLE1           ((uint32_t)0)
#endif

#ifndef MEMCFG_SRMEMTYPE1
  #define MEMCFG_SRMEMTYPE1               (MEMCFG_sharedRegionMemoryTypeControl)
#endif

/** 
 *  MEMCFG_SRTYPE2, MEMCFG_SRINDEX2, MEMCFG_SRBASE2, MEMCFG_SRSIZE2 
 *  MEMCFG_SRCACHEENABLE2 MEMCFG_SRMEMTYPE2 provide the default values of shared 
 *  region2
 */
#ifndef MEMCFG_SRTYPE2
  #define MEMCFG_SRTYPE2                  (MEMCFG_sharedRegionTypeSlave)
#endif

#ifndef MEMCFG_SRINDEX2
  #define MEMCFG_SRINDEX2                 ((uint32_t)0x2)
#endif

#ifndef MEMCFG_SRBASE2
  #define MEMCFG_SRBASE2                  ((uint32_t)0xB3D00000)
#endif

#ifndef MEMCFG_SRSIZE2
  #define MEMCFG_SRSIZE2                  ((uint32_t)0x0BC00000)
#endif

#ifndef MEMCFG_SRCACHEENABLE2
  #define MEMCFG_SRCACHEENABLE2           ((uint32_t)0)
#endif

#ifndef MEMCFG_SRMEMTYPE2
  #define MEMCFG_SRMEMTYPE2               (MEMCFG_sharedRegionMemoryTypeData)
#endif

/** 
 *  MEMCFG_SRTYPE3, MEMCFG_SRINDEX3, MEMCFG_SRBASE3, MEMCFG_SRSIZE3 
 *  MEMCFG_SRCACHEENABLE3 MEMCFG_SRMEMTYPE3 provide the default values of shared 
 *  region3
 */
#ifndef MEMCFG_SRTYPE3
  #define MEMCFG_SRTYPE3                  (MEMCFG_sharedRegionTypeSlave)
#endif

#ifndef MEMCFG_SRINDEX3
  #define MEMCFG_SRINDEX3                 ((uint32_t)0x3)
#endif

#ifndef MEMCFG_SRBASE3
  #define MEMCFG_SRBASE3                  ((uint32_t)0x0)
#endif

#ifndef MEMCFG_SRSIZE3
  #define MEMCFG_SRSIZE3                  ((uint32_t)0x0)
#endif

#ifndef MEMCFG_SRCACHEENABLE3
  #define MEMCFG_SRCACHEENABLE3           ((uint32_t)0)
#endif

#ifndef MEMCFG_SRMEMTYPE3
  #define MEMCFG_SRMEMTYPE3               (MEMCFG_sharedRegionMemoryTypeControl)
#endif

#ifndef MEMCFG_MODSTAT_INITDONE
  #define MEMCFG_MODSTAT_INITDONE MEMCFG_MODSTAT_NOTINITIALIZED
#endif

#ifndef MEMCFG_MODSTAT_CURSRREGIONBUF
  #define MEMCFG_MODSTAT_CURSRREGIONBUF   ((uint32_t)0)
#endif

#ifndef MEMCFG_MODSTAT_CURSRREGIONCTRLCACHEON
  #define MEMCFG_MODSTAT_CURSRREGIONCTRLCACHEON  ((uint32_t)0)
#endif

#ifndef MEMCFG_MODSTAT_CURSRREGIONCTRLCACHEOFF
  #define MEMCFG_MODSTAT_CURSRREGIONCTRLCACHEOFF  ((uint32_t)0)
#endif

#ifdef _cplusplus
}
#endif                          /* __cplusplus */

#endif                          /* #define _MEMTBL_CFG_H */

/* memtbl_cfg.h - EOF */
