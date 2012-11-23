# Makefile for audio_encode app

APP_NAME = audio_encode

SRCDIR = src ../common
INCDIR = . src ../common \
			$(lindevkit_PATH)/include

# List all the external components/interfaces, whose interface header files 
#  need to be included for this component
INCLUDE_EXERNAL_INTERFACES = bios xdc omx timmosal ipc fc xdais osal syslink \
                             ce uia slog aaclcenc

# List of all external libraries that need to be linked
EXTLIBS_m3video =
EXTLIBS_m3vpss =
EXTLIBS_a8host = $(omx_LIBPATH)/../lib/omxcore.av5T        \
                 $(omx_LIBPATH)/../lib/memcfg.av5T         \
                 $(omx_LIBPATH)/../lib/domx.av5T           \
                 $(omx_LIBPATH)/../lib/domx_delegates_shmem.av5T       \
                 $(omx_LIBPATH)/../lib/timmosal.av5T \
                 $(omx_LIBPATH)/../lib/omxcfg.av5T \
                 $(osal_PATH)/packages/linuxdist/build/lib/osal.a \
                 $(osal_PATH)/packages/linuxdist/cstubs/lib/cstubs.a \
                 $(fc_PATH)/packages/ti/sdo/rcm/lib/debug/rcm_syslink.av5T \
                 $(fc_PATH)/packages/ti/sdo/fc/memutils/lib/release/memutils.av5T \
                 $(osal_PATH)/packages/ti/sdo/xdcruntime/linux/lib/debug/osal_linux_470.av5T \
                 $(fc_PATH)/packages/ti/sdo/fc/global/lib/debug/fcsettings.av5T \
                 $(syslink_PATH)/packages/ti/syslink/lib/syslink.a_debug \
                 $(linuxutils_PATH)/packages/ti/sdo/linuxutils/cmem/lib/cmem.a470MV \
		 $(uia_PATH)/packages/ti/uia/linux/lib/servicemgr.a \
		 $(lindevkit_PATH)/lib/libasound.so
#                $(slog_PATH)/lib/slog.a
#                 /opt/tools/linux-devkit/arm-none-linux-gnueabi/usr/lib/ti/sdo/linuxutils/cmem/lib/cmem.a470MV \
#				 $(linuxutils_PATH)/packages/ti/sdo/linuxutils/cmem/lib/cmem.a470MV \

# List all the components required by the application
COMP_LIST_a8host = ldrmemcfg
COMP_LIST_c6xdsp = memcfg omxbase domx omxcore aenc

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON =
CFLAGS_LOCAL_COMMON = -DGlobal_appTerminate=0

SRCS_a8host = main.c msgq.c \
              ilclient_utils.c ilclient.c semp.c
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
