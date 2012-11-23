# Makefile for omxobjs app

APP_NAME = omxobjs

SRCDIR = src
INCDIR = . src

# List all the external components/interfaces, whose interface header files 
#  need to be included for this component
INCLUDE_EXERNAL_INTERFACES = xdc omx fc timmosal

# List of all external libraries that need to be linked
EXTLIBS_m3video =
EXTLIBS_m3vpss =
EXTLIBS_a8host = $(osal_PATH)/packages/linuxdist/build/lib/osal.a \
                 $(osal_PATH)/packages/linuxdist/cstubs/lib/cstubs.a \
                 $(fc_PATH)/packages/ti/sdo/rcm/lib/debug/rcm_syslink.av5T \
                 $(fc_PATH)/packages/ti/sdo/fc/memutils/lib/release/memutils.av5T \
                 $(osal_PATH)/packages/ti/sdo/xdcruntime/linux/lib/debug/osal_linux_470.av5T \
                 $(fc_PATH)/packages/ti/sdo/fc/global/lib/debug/fcsettings.av5T \
                 $(syslink_PATH)/packages/ti/syslink/lib/syslink.a_debug \
                 $(linuxutils_PATH)/packages/ti/sdo/linuxutils/cmem/lib/cmem.a470MV \
                 $(uia_PATH)/packages/ti/uia/linux/lib/servicemgr.a
#                 $(slog_PATH)/lib/slog.a \
#                 $(slog_PATH)/lib/slog_modtbl.a


# List all the components required by the application
COMP_LIST_m3video = omxbase venc vdec server scheduler domx omxcore memcfg ldrmemcfg
COMP_LIST_m3vpss = omxbase vfdc vfcc vfpc domx omxcore ctrl hdvpss memcfg ldrmemcfg vswmosaic
COMP_LIST_a8host = domx omxcore memcfg timmosal ldrmemcfg
COMP_LIST_c6xdsp = memcfg omxbase domx omxcore vlpb adec aenc

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON = 
CFLAGS_LOCAL_COMMON = -DGlobal_appTerminate=0

SRCS_a8host = main.c 
CFLAGS_LOCAL_a8host = -DGlobal_GrpxDssMsgHeapId=6 -DGlobal_TilerHeapId=7

# Core/SoC/platform specific source files and CFLAGS
# Example: 
#   SRCS_<core/SoC/platform-name> = 
#   CFLAGS_LOCAL_<core/SoC/platform-name> =

# Include common make files
include $(ROOTDIR)/makerules/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk 
#     and need not be explicitly specified here

# Nothing beyond this point
