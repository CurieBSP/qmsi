#
# Copyright (c) 2015, Intel Corporation
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 3. Neither the name of the Intel Corporation nor the names of its
#    contributors may be used to endorse or promote products derived from this
#    software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

THIS_DIR          := $(shell dirname $(abspath $(lastword $(MAKEFILE_LIST))))
OUT               := $(abspath $(THIS_DIR))

IAMCU_TOOLCHAIN_DIR = $(dir $(CROSS_COMPILE))
BUILD = release
DEFAULT_SOC = quark_se
SUPPORTED_SOCS = quark_se \
                 quark_d2000

SOC ?= $(DEFAULT_SOC)

ifeq ($(filter $(SOC),$(SUPPORTED_SOCS)),)
$(error SOC=$(SOC) is not supported. Run 'make help' for help)
endif

BASE_DIR = .

include $(BASE_DIR)/base.mk
include $(BASE_DIR)/soc/$(SOC)/$(SOC).mk
include $(BASE_DIR)/soc/$(SOC)/rom/rom.mk
include $(BASE_DIR)/drivers/libqmsi.mk

.PHONY: help

help:
	$(info )
	$(info List of build targets. By default all targets are built.)
	$(info rom      - Build the ROM firmware)
	$(info libqmsi  - Build the libqmsi package)
	$(info )
	$(info By default SOC=$(DEFAULT_SOC).)
	$(info List of supported values for SOC: $(SUPPORTED_SOCS))

#all: libqmsi rom

LIB = $(OUT)/qmsi1.a

$(LIB): $(OUT)/lib/libqmsi.a
	$(AT)cp $< $@

$(OUT)/lib/libqmsi.a:
	$(AT)cp -a $(THIS_DIR)/* $(OUT)
	$(AT)BUILD=$(BUILD) \
	     IAMCU_TOOLCHAIN_DIR=$(IAMCU_TOOLCHAIN_DIR) \
	     SOC=$(SOC) \
		$(MAKE) -C $(OUT) libqmsi
	$(AT)cp -rf $(OUT)/drivers/include/* $(OUT)/include/
	$(AT)cp -rf $(OUT)/soc/quark_se/include/* $(OUT)/include/
	$(AT)install -CD $(OUT)/build/$(BUILD)/$(SOC)/libqmsi/lib/libqmsi.a $@

lib: $(LIB)

all: lib
