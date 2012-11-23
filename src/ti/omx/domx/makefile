# Makefile for DOMX

MODULE_NAME = domx

SRCDIR = .
INCDIR = .

# List all the external components/interfaces, whose interface header files 
#  need to be included for this component
INCLUDE_EXERNAL_INTERFACES = bios xdc omx omxcore omxbase timmosal fc ipc \
                             syslink uia ce xdais h264enc h264dec mpeg2dec \
                             mpeg2enc slog

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON = OmxRpcStubCommon.c OmxRpcSkelCommon.c \
              DomxTunnelMgr.c DomxCore.c domx_omxRpc_misc.c\
              OmxRpcSkel.c OmxRpcCbStub.c OmxProxy.c OmxRpcStub.c \
              OmxRpc.c OmxRpcCbSkel.c domx_util.c
CFLAGS_LOCAL_COMMON = -D_VIDEO_M3_DYNAMIC_CONFIG

SRCS_m3vpss = DomxProcMgrIf_BIOS.c
SRCS_c6xdsp = DomxProcMgrIf_BIOS.c
SRCS_m3video = DomxProcMgrIf_BIOS.c
SRCS_a8host = DomxProcMgrIf_HLOS.c IpcSupport.c

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

#Note: USE_SLOG_PRINT=0 is forced. No other value should be used.
ifeq ($(CORE),a8host)
  CFLAGS_LOCAL_a8host += -D_LOCAL_CORE_a8host_ -DUSE_SLOG_PRINT=0 -DSLOG_DOMX_Module__MID=0x8001
  CFLAGS_LOCAL_a8host += -DSLOG_DOMXDH_Module__MID=0x8002 -DSLOG_DOMXRPC_Module__MID=0x8003
  CFLAGS_LOCAL_a8host += -DSLOG_DOMXIPC_Module__MID=0x8004
endif

# Include common make files
include $(ROOTDIR)/makerules/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk 
#     and need not be explicitly specified here

# Nothing beyond this point
