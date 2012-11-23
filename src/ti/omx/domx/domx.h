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
 *  @file  domx.h
 *  @brief This file is the interface header file for DOMX and has common
 *         type defines and other defines that are used across the modules
 *
 *  @rev 1.0
 */

#ifndef _DOMX_H_
#define _DOMX_H_

#ifdef __cplusplus
extern "C"
{
#endif                          /* __cplusplus */

/******************************************************************
 *   INCLUDE FILES
 ******************************************************************/

/******************************************************************
 *   Types/enums
 ******************************************************************/

/* DomxTypes_coreType - This enum is used to identify the Core ID number of
   various cores within the SoC. */
typedef enum DomxTypes_coreType_e
{
  DomxTypes_coreFirst = 0x0000,
  DomxTypes_coreDsp = DomxTypes_coreFirst,
  DomxTypes_coreCortexVideoM3 = 0x0001,
  DomxTypes_coreCortexHdvpssM3 = 0x0002,
  DomxTypes_coreCortexA8 = 0x0003,
  DomxTypes_coreLast = DomxTypes_coreCortexA8,
  DomxTypes_coreCount = (DomxTypes_coreLast + 1),
  DomxTypes_coreInvalid = 0xFFFFFFFF
} DomxTypes_coreType;

#ifdef __cplusplus
}
#endif                          /* __cplusplus */

#endif                          /* _DOMX_H_ */
