# File: env.mk. This file contains all the paths and other ENV variables

#
# Module paths
#

# Directory where all internal software packages are located; typically 
#  those that are checked into version controlled repository. In this case all
#  the OMX components and SDK/OMX demo.
INTERNAL_SW_ROOT = $(ROOTDIR)/src

# Directory where all external (imported) software packages are located; typically 
#  those that are NOT checked into version controlled repository. In this case,
#  compiler tool chains, BIOS, XDC, Syslink, IPC, FC, CE, drivers, codecs, etc.
EXTERNAL_SW_ROOT = /opt/tools

# Destination root directory.
#   - specify the directory where you want to place the object, archive/library,
#     binary and other generated files in a different location than source tree
#   - or leave it blank to place then in the same tree as the source
DEST_ROOT = $(INTERNAL_SW_ROOT)/../lib

# Directory where example-apps/demos are located. By default, it resides along
#  with other source code. This can be over-ridden by specifying it in the 
#  command line.
EXAMPLES_ROOT = $(ROOTDIR)/examples

# Utilities directory. This is required only if the build machine is Windows.
#   - specify the installation directory of utility which supports POSIX commands
#     (eg: Cygwin installation or MSYS installation).
UTILS_INSTALL_DIR = c:/cygwin

# Set path separator, etc based on the OS
ifeq ($(OS),Windows_NT)
  PATH_SEPARATOR = ;
  UTILSPATH = $(UTILS_INSTALL_DIR)/bin/
else 
  # else, assume it is linux
  PATH_SEPARATOR = :
endif

# BIOS

bios_PATH = $(EXTERNAL_SW_ROOT)/bios_6_33_05_46
bios_INCLUDE = $(bios_PATH)/packages

# FC - Framework Components
fc_PATH = $(EXTERNAL_SW_ROOT)/framework_components_3_22_01_07
fc_INCLUDE = $(fc_PATH)/packages

# CE - Codec Engine
ce_PATH =$(EXTERNAL_SW_ROOT)/codec_engine_3_22_01_06
ce_INCLUDE = $(ce_PATH)/packages

# H264ENC - H.264 Encoder
h264enc_PATH = $(EXTERNAL_SW_ROOT)/ivahd_h264enc_02_00_02_02_production
h264enc_INCLUDE = $(h264enc_PATH)/packages

# H264DEC - H.264 Decoder
h264dec_PATH = $(EXTERNAL_SW_ROOT)/ivahd_h264vdec_02_00_07_00_production
h264dec_INCLUDE = $(h264dec_PATH)/packages

# MPEG2DEC - MPEG2 Decoder
mpeg2dec_PATH = $(EXTERNAL_SW_ROOT)/ivahd_mpeg2vdec_01_00_05_00_production
mpeg2dec_INCLUDE = $(mpeg2dec_PATH)/packages

# MP3DEC - MP3 Decoder
mp3dec_PATH = $(EXTERNAL_SW_ROOT)/c674x_mp3dec_01_41_00_00_elf
mp3dec_INCLUDE = $(mp3dec_PATH)/packages

# AACDEC - AAC Decoder
aaclcdec_PATH = $(EXTERNAL_SW_ROOT)/c674x_aaclcdec_01_41_00_00_elf
aaclcdec_INCLUDE = $(aaclcdec_PATH)/packages

# AACENC - AAC Encoder
aaclcenc_PATH = $(EXTERNAL_SW_ROOT)/c674x_aaclcenc_01_00_01_00_elf
aaclcenc_INCLUDE = $(aaclcenc_PATH)/packages

# MPEG2ENC - MPEG2 Encoder
mpeg2enc_PATH = $(EXTERNAL_SW_ROOT)/codecs-dm816x_5_00_00_00
mpeg2enc_INCLUDE = $(mpeg2enc_PATH)/packages

# MPEG4DEC- mpeg4 Decoder
mpeg4dec_PATH = $(EXTERNAL_SW_ROOT)/ivahd_mpeg4vdec_01_00_04_00_production
mpeg4dec_INCLUDE = $(mpeg4dec_PATH)/packages

# MPEG4ENC - MPEG4 Encoder
mpeg4enc_PATH = $(EXTERNAL_SW_ROOT)/ivahd_mpeg4enc_01_00_00_05_production
mpeg4enc_INCLUDE = $(mpeg4enc_PATH)/packages

# VC1DEC
vc1dec_PATH = $(EXTERNAL_SW_ROOT)/ivahd_vc1vdec_01_00_00_07_production
vc1dec_INCLUDE = $(vc1dec_PATH)/packages

# MJPEGDEC
mjpegdec_PATH = $(EXTERNAL_SW_ROOT)/ivahd_jpegvdec_01_00_04_00_production
mjpegdec_INCLUDE = $(mjpegdec_PATH)/packages

# HDVICP20API - HDVICP 2.0 API module
hdvicp20api_PATH = $(EXTERNAL_SW_ROOT)/ivahd_hdvicp20api_01_00_00_19_production
hdvicp20api_INCLUDE = $(hdvicp20api_PATH)/packages

# IPC
ipc_PATH = $(EXTERNAL_SW_ROOT)/ipc_1_24_03_32
ipc_INCLUDE = $(ipc_PATH)/packages

# OSAL
osal_PATH = $(EXTERNAL_SW_ROOT)/osal_1_22_01_09
osal_INCLUDE = $(osal_PATH)/packages

# XDAIS
xdais_PATH = $(EXTERNAL_SW_ROOT)/xdais_7_22_00_03
xdais_INCLUDE = $(xdais_PATH)/packages

# LINUXUTILS
linuxutils_PATH = $(EXTERNAL_SW_ROOT)/linuxutils_3_22_00_02
linuxutils_INCLUDE = $(linuxutils_PATH)/packages

# SYSLINK 
syslink_PATH = $(EXTERNAL_SW_ROOT)/syslink_2_20_00_14
syslink_INCLUDE = $(syslink_PATH)/packages

# XDC
xdc_PATH = $(EXTERNAL_SW_ROOT)/xdctools_3_23_03_53
xdc_INCLUDE = $(xdc_PATH)/packages

# UIA
uia_PATH = $(EXTERNAL_SW_ROOT)/uia_1_01_01_14
uia_INCLUDE = $(uia_PATH)/packages

# Linux Dev Kit
lindevkit_PATH = $(EXTERNAL_SW_ROOT)/linux-devkit/arm-none-linux-gnueabi/usr
lindevkit_INCLUDE = $(lindevkit_PATH)/include

# PSP
ifeq ($(PLATFORM),ti816x-evm)
kernel_PATH = $(EXTERNAL_SW_ROOT)/TI816X-LINUX-PSP-04.04.00.01
else
kernel_PATH = $(EXTERNAL_SW_ROOT)/TI814X-LINUX-PSP-04.04.00.01
endif
kernel_INCLUDE = $(kernel_PATH)/include $(kernel_PATH)/arch/arm/include

# Audio (ALSA)
audio_INCLUDE = $(lindevkit_INCLUDE)

# OMX Top-level package directory
omx_PATH = $(INTERNAL_SW_ROOT)
omx_LIBPATH = $(omx_PATH)
include $(omx_PATH)/component.mk

# SLog
slog_PATH = $(EXTERNAL_SW_ROOT)/Slog_04_00_00_02
slog_INCLUDE = $(slog_PATH)

#
# Tools paths
#
# Cortex-M3

CODEGEN_PATH_M3 = $(EXTERNAL_SW_ROOT)/cgt470_4_9_2

# Cortex-A8
CODESOURCERY_PATH = $(EXTERNAL_SW_ROOT)/CodeSourcery/2009q1-203
CODEGEN_PATH_A8 = $(CODESOURCERY_PATH)

# DSP - Since same toolchain does not support COFF and ELF, there are two entries
#        This would go away when one version supports both formats
CODEGEN_PATH_DSP = $(EXTERNAL_SW_ROOT)/cgt6x_7_3_1
CODEGEN_PATH_DSPELF = $(EXTERNAL_SW_ROOT)/cgt6x_7_3_1

# Commands commonly used within the make files

RM = $(UTILSPATH)rm
RMDIR = $(UTILSPATH)rm -rf
MKDIR = $(UTILSPATH)mkdir
ECHO = @echo
# MAKE = $(UTILSPATH)make
EGREP = $(UTILSPATH)egrep
CP = $(UTILSPATH)cp
CHMOD = $(UTILSPATH)chmod

#
# XDC specific ENV variables
#

# XDC Config.bld file (required for configuro) ; Derives from top-level omx_PATH
CONFIG_BLD_XDC_c674 = $(omx_PATH)/ti/omx/build/config_dsp.bld
CONFIG_BLD_XDC_c64p = $(omx_PATH)/ti/omx/build/config_dsp.bld
CONFIG_BLD_XDC_m3 = $(omx_PATH)/ti/omx/build/config.bld
CONFIG_BLD_XDC_a8 = $(omx_PATH)/ti/omx/build/config_ca8.bld

XDCPATH = $(bios_PATH)/packages;$(fc_PATH)/packages;$(ipc_PATH)/packages;$(syslink_PATH)/packages;$(xdc_PATH)/packages;$(ce_PATH)/packages;$(omx_PATH);$(omx_PATH)/ti/omx/interfaces/openMaxv11;$(h264enc_PATH)/packages;$(h264dec_PATH)/packages;$(mpeg2dec_PATH)/packages;$(mp3dec_PATH)/packages;$(aaclcdec_PATH)/packages;$(mpeg2enc_PATH)/packages;$(omx_PATH)/ti/omx/comp/omxbase;$(omx_PATH)/ti/omx/omxcore;$(hdvicp20api_PATH)/packages;$(osal_PATH)/packages;$(xdais_PATH)/packages;$(linuxutils_PATH)/packages;$(uia_PATH)/packages;$(mpeg4enc_PATH)/packages;$(mpeg4dec_PATH)/packages;$(vc1dec_PATH)/packages;$(mjpegdec_PATH)/packages;$(aaclcenc_PATH)/packages;
export XDCPATH

XDCROOT = $(xdc_PATH)
XDCTOOLS = $(xdc_PATH)
export XDCROOT
export XDCTOOLS

TMS470CGTOOLPATH = $(CODEGEN_PATH_M3)
CGTOOLS = $(CODEGEN_PATH_DSP)
CGTOOLS_ELF = $(CODEGEN_PATH_DSPELF)
C674CODEGENTOOL = $(CODEGEN_PATH_DSPELF) 

export TMS470CGTOOLPATH
export CGTOOLS
export CGTOOLS_ELF
export C674CODEGENTOOL

CODESOURCERYCGTOOLS = $(CODEGEN_PATH_A8)
export CODESOURCERYCGTOOLS

PATH += $(PATH_SEPARATOR)$(xdc_PATH)$(PATH_SEPARATOR)$(CODEGEN_PATH_DSPELF)/bin$(PATH_SEPARATOR)$(CODEGEN_PATH_M3)/bin
export PATH

# Nothing beyond this point
