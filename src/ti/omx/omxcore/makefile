# Makefile for OMXCORE

MODULE_NAME = omxcore

SRCDIR = src
INCDIR = . inc

# List all the external components/interfaces, whose interface header files 
#  need to be included for this component
INCLUDE_EXERNAL_INTERFACES = bios xdc ipc omx timmosal fc slog

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON = omx_core.c
CFLAGS_LOCAL_COMMON =

# Core/SoC/platform specific source files and CFLAGS
# Example: 
#   SRCS_<core/SoC/platform-name> = 
#   CFLAGS_LOCAL_<core/SoC/platform-name> =
ifeq ($(CORE),a8host)
  CFLAGS_LOCAL_a8host += -D_LOCAL_CORE_a8host_
endif

# Include common make files
include $(ROOTDIR)/makerules/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk 
#     and need not be explicitly specified here

# Nothing beyond this point
