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
#ifndef __OMX_ADEC_H__
#define __OMX_ADEC_H__

/******************************************************************************\
 *      Includes
\******************************************************************************/

/******************************************************************************/

#ifdef __cplusplus              /* required for headers that might */
extern "C"
{                               /* be compiled by a C++ compiler */
#endif

typedef enum
{
  ArgID_INPUT_FILE = 256,       /* it should not conflict with std ascii */
  ArgID_OUTPUT_FILE,
  ArgID_CODEC,
  ArgID_RAW_FORMAT,
  ArgID_SAMPLE_RATE,
  ArgID_NUMARGS,
} ArgID;

#define MAX_FILE_NAME_SIZE      256
#define MAX_CODEC_NAME_SIZE     16
#define MAX_RAW_FORMAT_SIZE     16
#define MAX_SAMPLE_RATE_SIZE    16

/* Arguments for app */
typedef struct Args
{
  char input_file[MAX_FILE_NAME_SIZE];
  char output_file[MAX_FILE_NAME_SIZE];
  char codec[MAX_CODEC_NAME_SIZE];
  char rawFormat[MAX_RAW_FORMAT_SIZE];
  char sampleRate[MAX_SAMPLE_RATE_SIZE];
} IL_ARGS;

void usage (IL_ARGS *argsp);

void parse_args (int argc, char *argv[], IL_ARGS *argsp);

#endif
