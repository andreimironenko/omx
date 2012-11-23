# Makefile for VLPB

MODULE_NAME = vlpb

SRCDIR = src
INCDIR = . src

# List all the external components/interfaces, whose interface header files 
#  need to be included for this component
INCLUDE_EXERNAL_INTERFACES = omx timmosal xdc fc

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON =

SRCS_a8host = omx_vlpb.c omx_vlpb_utils.c
SRCS_c6xdsp = omx_vlpb.c omx_vlpb_utils.c
CFLAGS_LOCAL_a8host =

# Core/SoC/platform specific source files and CFLAGS
# Example: 
#   SRCS_<core/SoC/platform-name> = 
#   CFLAGS_LOCAL_<core/SoC/platform-name> =

ifeq ($(CORE),c6xdsp)
  CFLAGS_LOCAL_c6xdsp += -D_LOCAL_CORE_c6xdsp_
endif
ifeq ($(CORE),m3vpss)
  CFLAGS_LOCAL_m3vpss += -D_LOCAL_CORE_m3vpss_
endif
ifeq ($(CORE),m3video)
  CFLAGS_LOCAL_m3video += -D_LOCAL_CORE_m3video_
endif
ifeq ($(CORE),a8host)
  CFLAGS_LOCAL_a8host += -D_LOCAL_CORE_a8host_
endif

# Include common make files
include $(ROOTDIR)/makerules/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk 
#     and need not be explicitly specified here

# Nothing beyond this point
