# Makefile for dm81xx app

APP_NAME = dm81xx

SRCDIR = src ../graphics ../common
INCDIR = . src ../common

# List all the external components/interfaces, whose interface header files 
#  need to be included for this component
INCLUDE_EXERNAL_INTERFACES = bios xdc omx timmosal ipc fc xdais osal syslink \
                             ce uia slog

# List of all external libraries that need to be linked
EXTLIBS_c6xdsp = $(omx_PATH)/ti/omx/omxutils/lib/debug/omxutils.ae674 \
                 $(omx_PATH)/ti/omx/comp/omxbase/lib/c6xdsp/debug/omxbase.ae674 \
                 $(omx_PATH)/ti/omx/comp/omxbase/lib/c6xdsp/debug/omxbase_cfg.oe674
EXTLIBS_a8host = $(osal_PATH)/packages/linuxdist/build/lib/osal.a \
                 $(osal_PATH)/packages/linuxdist/cstubs/lib/cstubs.a \
                 $(fc_PATH)/packages/ti/sdo/rcm/lib/debug/rcm_syslink.av5T \
                 $(fc_PATH)/packages/ti/sdo/fc/memutils/lib/release/memutils.av5T \
                 $(osal_PATH)/packages/ti/sdo/xdcruntime/linux/lib/debug/osal_linux_470.av5T \
                 $(fc_PATH)/packages/ti/sdo/fc/global/lib/debug/fcsettings.av5T \
                 $(syslink_PATH)/packages/ti/syslink/lib/syslink.a_debug \
                 $(linuxutils_PATH)/packages/ti/sdo/linuxutils/cmem/lib/cmem.a470MV \
                 $(uia_PATH)/packages/ti/uia/linux/lib/servicemgr.a
#                $(slog_PATH)/lib/slog.a \

# List all the components required by the application
COMP_LIST_a8host = omxcore memcfg domx timmosal
COMP_LIST_c6xdsp = memcfg domx omxcore vlpb adec aenc

# XDC CFG File
XDC_CFG_FILE_c6xdsp = DspAppMain.cfg

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON =
CFLAGS_LOCAL_COMMON = -DGlobal_appTerminate=0

SRCS_c6xdsp = main_c674.c platform_c674.c configureDspUiaLogger.c
CFLAGS_LOCAL_c6xdsp = -DGlobal_GrpxDssMsgHeapId=6

# Core/SoC/platform specific source files and CFLAGS
# Example: 
#   SRCS_<core/SoC/platform-name> = 
#   CFLAGS_LOCAL_<core/SoC/platform-name> =
ifeq ($(CORE),c6xdsp)
  CFLAGS_LOCAL_c6xdsp += -D_LOCAL_CORE_c6xdsp_
endif
ifeq ($(CORE),a8host)
  CFLAGS_LOCAL_a8host += -D_LOCAL_CORE_a8host_ -DUSE_SLOG_PRINT=1
endif

# Include common make files
include $(ROOTDIR)/makerules/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk 
#     and need not be explicitly specified here

# Nothing beyond this point
