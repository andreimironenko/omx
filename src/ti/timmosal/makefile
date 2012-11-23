# Makefile for TIMMOSAL

MODULE_NAME = timmosal

SRCDIR = . linux/src
INCDIR = .

# List all the external components/interfaces, whose interface header files 
#  need to be included for this component
INCLUDE_EXERNAL_INTERFACES = bios xdc

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON = 
CFLAGS_LOCAL_COMMON = -D_CIO

# Core/SoC/platform specific source files and CFLAGS
# Example: 
#   SRCS_<core/SoC/platform-name> = 
#   CFLAGS_LOCAL_<core/SoC/platform-name> =
SRCS_m3video = bios6/src/timm_osal_trace.c bios6/src/timm_osal_task.c          \
               bios6/src/timm_osal_semaphores.c bios6/src/timm_osal_pipes.c    \
               bios6/src/timm_osal_mutex.c bios6/src/timm_osal_memory.c        \
               bios6/src/timm_osal_events.c bios6/src/timm_osal.c              \
               StmLibrary/StmLibrary.c StmLibrary/STMHelper.c
SRCS_m3vpss = bios6/src/timm_osal_trace.c bios6/src/timm_osal_task.c          \
               bios6/src/timm_osal_semaphores.c bios6/src/timm_osal_pipes.c    \
               bios6/src/timm_osal_mutex.c bios6/src/timm_osal_memory.c        \
               bios6/src/timm_osal_events.c bios6/src/timm_osal.c              \
               StmLibrary/StmLibrary.c StmLibrary/STMHelper.c
SRCS_a8host = timm_osal_trace.c timm_osal_task.c timm_osal_semaphores.c \
              timm_osal_pipes.c timm_osal_mutex.c timm_osal_memory.c \
              timm_osal_events.c timm_osal.c

# Include common make files
include $(ROOTDIR)/makerules/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk 
#     and need not be explicitly specified here

# Nothing beyond this point
