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
 *  @file memtbl_cfg.c
 *  @brief This file contains shared region configuration required by the system
 * 
 *
 *
 *  @rev 1.0
 *******************************************************************************
 */

/*******************************************************************************
*                             Compilation Control Switches
*******************************************************************************/
#define MEMCFG_LOCAL

/*******************************************************************************
*                             INCLUDE FILES
*******************************************************************************/
/*--------------------- system and platform files ----------------------------*/
#include <stdint.h>

/*-------------------------program files -------------------------------------*/
#include "app_cfg.h"            /* Note: The order of inclusion is important */
#include "memtbl_cfg.h"         /* default values for configurable parameters
                                   come from this file */
#include "memcfg.h"

/*******************************************************************************
 * EXTERNAL REFERENCES NOTE : only use if not found in header file
*******************************************************************************/

/*------------------------ data declarations ---------------------------------*/
/* None */

/*----------------------- function prototypes --------------------------------*/
/* None */

/*******************************************************************************
 * PUBLIC DECLARATIONS Defined here, used elsewhere
 ******************************************************************************/

/*----------------------- function prototypes --------------------------------*/

/*------------------------ data declarations ---------------------------------*/
/** 
 *  MEMCFG_module_inst, is the MEMCFG_module instance. 
 *  This contains the state of the MEMCFG_module.
 *
 *
*/
MEMCFG_Module_State MEMCFG_module_inst =
{
  MEMCFG_MODSTAT_INITDONE
};

/** 
 *  MEMCFG_sharedRegionConfigTable, is the shared region configuration table. 
 *  The initial values are provided here. In the app_cfg.h, system integrator
 *  can override the macros.
 *
 *
*/
const MEMCFG_sharedRegionConfig MEMCFG_sharedRegionConfigTable =
{
  MEMCFG_NUMFQSHAREDREGIONS,    /* numFQSharedRegions */
  {
   {
    MEMCFG_SRTYPE0,             /* srType */
    MEMCFG_SRINDEX0,            /* srIndex */
    MEMCFG_SRBASE0,             /* srBase */
    MEMCFG_SRSIZE0,             /* srSize */
    MEMCFG_SRCACHEENABLE0,      /* srCacheEnable */
    MEMCFG_SRMEMTYPE0           /* memType */
   },                          /* [0] */
   {
    MEMCFG_SRTYPE1,             /* srType */
    MEMCFG_SRINDEX1,            /* srIndex */
    MEMCFG_SRBASE1,             /* srBase */
    MEMCFG_SRSIZE1,             /* srSize */
    MEMCFG_SRCACHEENABLE1,      /* srCacheEnable */
    MEMCFG_SRMEMTYPE1           /* memType */
   },                          /* [1] */
   {
    MEMCFG_SRTYPE2,             /* srType */
    MEMCFG_SRINDEX2,            /* srIndex */
    MEMCFG_SRBASE2,             /* srBase */
    MEMCFG_SRSIZE2,             /* srSize */
    MEMCFG_SRCACHEENABLE2,      /* srCacheEnable */
    MEMCFG_SRMEMTYPE2           /* memType */
   },                          /* [2] */
   {
    MEMCFG_SRTYPE3,             /* srType */
    MEMCFG_SRINDEX3,            /* srIndex */
    MEMCFG_SRBASE3,             /* srBase */
    MEMCFG_SRSIZE3,             /* srSize */
    MEMCFG_SRCACHEENABLE3,      /* srCacheEnable */
    MEMCFG_SRMEMTYPE3           /* memType */
   },                          /* [3] */
  }                            /* srInfo */
};

/** 
 *  MEMCFG_module is pointer to the MEMCFG_Module instance.
 *
 *
*/
MEMCFG_Module_State *MEMCFG_module = &MEMCFG_module_inst;

/*******************************************************************************
 * PRIVATE DECLARATIONS Defined here, used only here
 ******************************************************************************/

/*--------------------------- macros  ----------------------------------------*/
/* None*/

/*-------------------------data declarations ---------------------------------*/
/* None */

/*---------------------function prototypes -----------------------------------*/
/* None */

/* memtbl_cfg.c - EOF */
