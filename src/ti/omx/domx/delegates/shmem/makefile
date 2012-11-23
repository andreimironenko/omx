# Makefile for DOMX Delegates Shmem

MODULE_NAME = domx_delegates_shmem

SRCDIR = .
INCDIR = .

# List all the external components/interfaces, whose interface header files 
#  need to be included for this component
INCLUDE_EXERNAL_INTERFACES = bios xdc ipc fc omx omxcore

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON = DmmDelegate.c DmmDelegateHeap.c
CFLAGS_LOCAL_COMMON =

# Core/SoC/platform specific source files and CFLAGS
# Example: 
#   SRCS_<core/SoC/platform-name> = 
#   CFLAGS_LOCAL_<core/SoC/platform-name> =

# Include common make files
include $(ROOTDIR)/makerules/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk 
#     and need not be explicitly specified here

# Nothing beyond this point
