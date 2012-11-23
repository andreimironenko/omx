# Makefile for ADEC

MODULE_NAME = adec

SRCDIR = src
INCDIR = . src

# List all the external components/interfaces, whose interface header files 
#  need to be included for this component
INCLUDE_EXERNAL_INTERFACES = bios xdc omx omxcore omxbase timmosal fc xdais \
                             ce mp3dec aaclcdec aaclcenc

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON = omx_adec.c omx_adec_util.c
CFLAGS_LOCAL_COMMON = -D_AUDIO_DSP

# Common source files that are conditionally added
ifeq ($(findstring mp3dec, $(CODECS_IN_APP)),mp3dec)
  SRCS_COMMON += omx_mp3ad.c
endif

ifeq ($(findstring aaclcdec, $(CODECS_IN_APP)),aaclcdec)
  SRCS_COMMON += omx_aacad.c
endif

# Core/SoC/platform specific source files and CFLAGS
# Example: 
#   SRCS_<core/SoC/platform-name> = 
#   CFLAGS_LOCAL_<core/SoC/platform-name> =

# Include common make files
include $(ROOTDIR)/makerules/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk 
#     and need not be explicitly specified here

# Nothing beyond this point
