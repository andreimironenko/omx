#
# OMX
#

ifeq ($(ROOTDIR),)
  $(error Error: Environment variable ROOTDIR not set)
endif

ifeq ($(PLATFORM),)
  $(error Error: Environment variable PLATFORM not set)
endif

# Begin block - Build options
#  By the end of this block, we'd have either xdc or make or both in BUILD_OPTIONS
#  using which, we'd build either XDC packages (+run configuro) 
#  or make based components (+link) or both.
BUILD_OPTIONS = xdc make
VALID_BUILD_OPTIONS = full $(BUILD_OPTIONS)
ifeq ($(filter $(BUILD),$(VALID_BUILD_OPTIONS)),)
 ifeq ($(BUILD),)
  BUILD = full
 else
  $(error Error: Invalid value specified for BUILD. Valid values are - $(VALID_BUILD_OPTIONS))
 endif
endif
ifneq ($(BUILD),full)
 BUILD_OPTIONS = $(BUILD)
endif
# End block - Build options

include $(ROOTDIR)/makerules/build_config.mk
include $(ROOTDIR)/makerules/env.mk
include $(INTERNAL_SW_ROOT)/component.mk

omx_COMP_LIST_ALL = $(omx_COMP_LIST_c6xdsp) $(omx_COMP_LIST_m3video) $(omx_COMP_LIST_m3vpss) $(omx_COMP_LIST_a8host) $(omx_COMP_LIST_common) $(EXT_COMP_LIST_c6xdsp) $(EXT_COMP_LIST_common) $(EXT_COMP_LIST_m3video) $(EXT_COMP_LIST_m3vpss) $(EXT_COMP_LIST_a8host)
omx_PKG_LIST_ALL = $(foreach PKG,$(omx_COMP_LIST_ALL),$($(PKG)_PKG_LIST))
omx_PKG_LIST_common = $(foreach PKG,$(omx_COMP_LIST_common),$($(PKG)_PKG_LIST))
omx_PKG_LIST_c6xdsp = $(foreach PKG,$(omx_COMP_LIST_c6xdsp),$($(PKG)_PKG_LIST))
omx_PKG_LIST_m3video = $(foreach PKG,$(omx_COMP_LIST_m3video),$($(PKG)_PKG_LIST))
ALL_COMP_LIST_m3vpss = $(omx_COMP_LIST_m3vpss) $(EXT_COMP_LIST_m3vpss)
omx_PKG_LIST_m3vpss = $(foreach PKG,$(ALL_COMP_LIST_m3vpss),$($(PKG)_PKG_LIST))
omx_PKG_LIST_a8host = $(foreach PKG,$(omx_COMP_LIST_a8host),$($(PKG)_PKG_LIST))
CLEANALL_TARGETS = $(addsuffix _clean, $(omx_PKG_LIST_ALL))
CLEANALL_c6xdsp = $(addsuffix _clean, $(omx_PKG_LIST_c6xdsp))
CLEANALL_m3video = $(addsuffix _clean, $(omx_PKG_LIST_m3video))
CLEANALL_m3vpss = $(addsuffix _clean, $(omx_PKG_LIST_m3vpss))
CLEANALL_a8host = $(addsuffix _clean, $(omx_PKG_LIST_a8host))
CLEANALL_common = $(addsuffix _clean, $(omx_PKG_LIST_common))
CLEANALL_EXAMPLES = $(addsuffix _clean, $(omx_EXAMPLES_LIST))
CLEANALL_EXAMPLES_NONXDC = $(addsuffix _clean, $(omx_EXAMPLES_LIST_NONXDC))

.PHONY : $(omx_PKG_LIST_ALL) $(omx_EXAMPLES_LIST) $(omx_EXAMPLES_LIST_NONXDC)

libs: libs_host libs_dsp

libs_host: omxobjs

libs_dsp:

all: libs examples

examples: $(omx_EXAMPLES_LIST) $(omx_EXAMPLES_LIST_NONXDC)

examples_dsp: $(omx_EXAMPLES_DSP)

examples_host: $(omx_EXAMPLES_HOST)

clean: $(CLEANALL_TARGETS)

cleanall: $(CLEANALL_TARGETS) $(CLEANALL_EXAMPLES) $(CLEANALL_EXAMPLES_NONXDC)

#
# Rule to build all modules and examples
#

$(omx_PKG_LIST_common) :
	$(ECHO) \# Making $(PLATFORM):a8host:$(PROFILE_a8host):$@...
	$(MAKE) -C $($@_PATH) CORE=a8host

$(omx_PKG_LIST_a8host) :
	$(ECHO) \# Making $(PLATFORM):a8host:$(PROFILE_a8host):$@...
	$(MAKE) -C $($@_PATH) CORE=a8host

# DECODER NT demo/app

decode: decodehost

decodehost:
ifeq ($(findstring make,$(BUILD_OPTIONS)),make)
	$(ECHO) \# Building $(subst host,,$@) executable for core: a8host
	$(MAKE) -C $($(subst host,,$@)_EXAMPLES_PATH) CORE=a8host
endif

# Display NT example
 
display: displayhost
displayhost:
ifeq ($(findstring make,$(BUILD_OPTIONS)),make)
	$(ECHO) \# Building $(subst host,,$@) executable for core: a8host
	$(MAKE) -C $($(subst host,,$@)_EXAMPLES_PATH) CORE=a8host
endif

# DECODE DISPLAY NT demo/app

decode_display: decode_displayhost

decode_displayhost:
ifeq ($(findstring make,$(BUILD_OPTIONS)),make)
	$(ECHO) \# Building $(subst host,,$@) executable for core: a8host
	$(MAKE) -C $($(subst host,,$@)_EXAMPLES_PATH) CORE=a8host
endif

encode: encodehost
encodehost:
ifeq ($(findstring make,$(BUILD_OPTIONS)),make)
	$(ECHO) \# Building $(subst host,,$@) executable for core: a8host
	$(MAKE) -C $($(subst host,,$@)_EXAMPLES_PATH) CORE=a8host
endif


# CAPTURE->ENCODE NT demo/app

capture_encode: capture_encodehost

capture_encodehost:
ifeq ($(findstring make,$(BUILD_OPTIONS)),make)
	$(ECHO) \# Building $(subst host,,$@) executable for core: a8host
	$(MAKE) -C $($(subst host,,$@)_EXAMPLES_PATH) CORE=a8host
endif

# C6XTEST DSP NT demo/app

c6xtest: c6xtesthost

c6xtesthost:
ifeq ($(findstring make,$(BUILD_OPTIONS)),make)
	$(ECHO) \# Building $(subst host,,$@) executable for core: a8host
	$(MAKE) -C $($(subst host,,$@)_EXAMPLES_PATH) CORE=a8host
endif

# ADEC_SNT DSP NT demo/app

adec_snt: adec_snthost

adec_snthost:
ifeq ($(findstring make,$(BUILD_OPTIONS)),make)
	$(ECHO) \# Building $(subst host,,$@) executable for core: a8host
	$(MAKE) -C $($(subst host,,$@)_EXAMPLES_PATH) CORE=a8host
endif

# audio_encode demo/app
audio_encode:
ifeq ($(findstring make,$(BUILD_OPTIONS)),make)
	$(ECHO) \# Building $(subst host,,$@) executable for core: a8host
	$(MAKE) -C $($(subst host,,$@)_EXAMPLES_PATH) CORE=a8host
endif

# VSWMOSAIC demo/app

decode_mosaicdisplay: decode_mosaicdisplayhost

decode_mosaicdisplayhost:
ifeq ($(findstring make,$(BUILD_OPTIONS)),make)
	$(ECHO) \# Building $(subst host,,$@) executable for core: a8host
	$(MAKE) -C $($(subst host,,$@)_EXAMPLES_PATH) CORE=a8host
endif

# dm81xx dsp source build

dm81xxdspXdc:
ifeq ($(findstring xdc,$(BUILD_OPTIONS)),xdc)
	$(ECHO) \# Making XDC dependent packages for $(subst dspXdc,,$@):c6x
	$(MAKE) -f xdcdepmakefile _APP=$(subst dspXdc,,$@) CORE=c6xdsp
	$(ECHO) \# Configuring XDC packages for $(subst dspXdc,,$@):c6x
	$(MAKE) -C $($(subst dspXdc,,$@)_EXAMPLES_PATH) xdc_configuro CORE=c6xdsp
endif

dm81xx: dm81xxdspXdc 
ifeq ($(findstring make,$(BUILD_OPTIONS)),make)
	$(ECHO) \# Building $(subst dsp,,$@) executable for core: c6xdsp
	$(MAKE) -C $($(subst dsp,,$@)_EXAMPLES_PATH) CORE=c6xdsp
endif

omxobjs: 
ifeq ($(findstring make,$(BUILD_OPTIONS)),make)
	$(ECHO) \# Building $(subst host,,$@) executable for core: a8host
	$(MAKE) -C $($(subst host,,$@)_EXAMPLES_PATH) CORE=a8host
	$(CP) $(DEST_ROOT)/omxobjs/obj/$(PLATFORM)/a8host/$(PROFILE_a8host)/*.ov5T $(DEST_ROOT)
	$(RM) $(DEST_ROOT)/main.ov5T
	$(MAKE) -C $($(subst host,,$@)_EXAMPLES_PATH) archive_spl CORE=a8host AR_SPL_PATH=$(DEST_ROOT) AR_SPL_NAME=omxcfg.av5T
	$(RM) $(DEST_ROOT)/*.ov5T
endif

# Clean targets

$(CLEANALL_common) : 
	$(ECHO) \# Cleaning $(subst _clean,,$@)...
	$(MAKE) -C $($(subst _clean,,$@)_PATH) clean CORE=c6xdsp
	$(MAKE) -C $($(subst _clean,,$@)_PATH) clean CORE=m3video
	$(MAKE) -C $($(subst _clean,,$@)_PATH) clean CORE=m3vpss
	$(MAKE) -C $($(subst _clean,,$@)_PATH) clean CORE=a8host

$(CLEANALL_c6xdsp) : 
	$(ECHO) \# Cleaning $(subst _clean,,$@)...
	$(MAKE) -C $($(subst _clean,,$@)_PATH) clean CORE=c6xdsp

$(CLEANALL_a8host) : 
	$(ECHO) \# Cleaning $(subst _clean,,$@)...
	$(MAKE) -C $($(subst _clean,,$@)_PATH) clean CORE=a8host

$(CLEANALL_EXAMPLES): 
	$(ECHO) \# Cleaning $(subst _clean,,$@)...
ifeq ($(findstring xdc,$(BUILD_OPTIONS)),xdc)
	$(MAKE) -f xdcdepmakefile xdcdepclean _APP=$(subst _clean,,$@) CORE=c6xdsp	
endif
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean CORE=c6xdsp
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean CORE=a8host

$(CLEANALL_EXAMPLES_NONXDC): 
	$(ECHO) \# Cleaning $(subst _clean,,$@)...
	$(MAKE) -C $($(subst _clean,,$@)_EXAMPLES_PATH) clean CORE=a8host
 
# SDK Related targets

#
# ==== Standard Variables ====
#


#
# Where to install/stage the packages
# Typically this would point to the devkit location
#

DESTDIR ?= $(HOME)/target-root
libdir ?= /lib
includedir ?= /include
ifeq ($(docdir),)
    docdir := /share/ti/omx/doc
endif

ifeq ($(prefix),)
    prefix := /usr
    xdcprefix := /
endif

#
# == Helper variables ==
#

LIBSINPACKAGES = $(shell cd src && find . -name '*.ae674' -or -name '*.av5T' -type f)
LIBSINBIN = $(shell cd bin && find . -name '*.ae674' -or -name '*.av5T' -type f)
LIBSINLIB = $(shell cd lib && find ti -name '*.ae674' -or -name '*.av5T' -type f)
HEADERS = $(shell cd src && find . -name '*.h' -type f -not \( -name "package.defs.h" -o -name "_*.h" \))
DOCS = $(wildcard *ReleaseNotes.PDF) $(wildcard *UserGuide.pdf)

LIBDIRSDK = $(DESTDIR)/$(prefix)/$(libdir)
INCDIRSDK = $(DESTDIR)/$(prefix)/$(includedir)
DOCDIR = $(DESTDIR)/$(prefix)/$(docdir)
PACKAGEDIR = $(DESTDIR)/$(xdcprefix)/$(packagedir)

#
# ==== Helper functions ====
#

purge = @find $(1) -name $(2) -exec rm -rf {} \;

all_packages = $(XDCPKG) packages | sed -e 's:packages/::' -e 's:/:.:g'

# 
# ==== Targets ====
#

install:
	@echo "Installing libraries.."
	@mkdir -p $(LIBDIRSDK)
ifneq ($(LIBSINPACKAGES),)
	@cd packages && cp --parents $(LIBSINPACKAGES) $(LIBDIRSDK)
endif
ifneq ($(LIBSINBIN),)
	@cd bin && cp --parents $(LIBSINBIN) $(LIBDIRSDK)
endif
ifneq ($(LIBSINLIB),)
	@cd lib && cp --parents $(LIBSINLIB) $(LIBDIRSDK)
endif
	@echo "Installing header files.."
	@mkdir -p $(INCDIRSDK)
	@cd packages && cp --parents $(HEADERS) $(INCDIRSDK)
	@echo "Installing documentation.."
	@mkdir -p $(DOCDIR)
	@cp $(DOCS) $(DOCDIR)

# Nothing beyond this point
