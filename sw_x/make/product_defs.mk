## This file is generated by product_defs.py, DO NOT EDIT!

PRODUCT_VARS := PLATFORM ARCH CONFIG_ES KVERS HW_PLAT

# Arguments: <product type> <param variable name prefix>
# Calling this function causes the variables <prefix><param>, for each product parameter,
#   to be defined with values corresponding to the product type.  E.g. after
#   $(eval $(call setParamsFromProduct,linux,FOO_)) the variable FOO_PLATFORM is linux.
define setParamsFromProduct
ifeq ($(1),android)
$(2)PLATFORM := android
$(2)ARCH := arm
$(2)CONFIG_ES := ul
$(2)KVERS := na
$(2)HW_PLAT := na
endif
ifeq ($(1),cloudnode)
$(2)PLATFORM := cloudnode
$(2)ARCH := arm
$(2)CONFIG_ES := ul
$(2)KVERS := na
$(2)HW_PLAT := na
endif
ifeq ($(1),ios)
$(2)PLATFORM := ios
$(2)ARCH := multi
$(2)CONFIG_ES := ul
$(2)KVERS := na
$(2)HW_PLAT := na
endif
ifeq ($(1),linux)
$(2)PLATFORM := linux
$(2)ARCH := x86
$(2)CONFIG_ES := ul
$(2)KVERS := na
$(2)HW_PLAT := na
endif
ifeq ($(1),linux_emb)
$(2)PLATFORM := linux_emb
$(2)ARCH := mips
$(2)CONFIG_ES := ul
$(2)KVERS := na
$(2)HW_PLAT := na
endif
ifeq ($(1),linux_ol)
$(2)PLATFORM := linux
$(2)ARCH := x86
$(2)CONFIG_ES := ul
$(2)KVERS := na
$(2)HW_PLAT := na
endif
ifeq ($(1),msvc)
$(2)PLATFORM := msvc
$(2)ARCH := multi
$(2)CONFIG_ES := ul
$(2)KVERS := na
$(2)HW_PLAT := na
endif
ifeq ($(1),winrt)
$(2)PLATFORM := msvc
$(2)ARCH := multi
$(2)CONFIG_ES := ul
$(2)KVERS := na
$(2)HW_PLAT := na
endif
ifeq ($(1),woa)
$(2)PLATFORM := msvc
$(2)ARCH := multi
$(2)CONFIG_ES := ul
$(2)KVERS := na
$(2)HW_PLAT := na
endif
ifndef $(2)PLATFORM
define ENDL


endef
$$(error Invalid product type "$(1)".$$(ENDL)Please set environment variable "PRODUCT" to be one of:$$(ENDL)  android$$(ENDL)  cloudnode$$(ENDL)  ios$$(ENDL)  linux$$(ENDL)  linux_emb$$(ENDL)  linux_ol$$(ENDL)  msvc$$(ENDL)  winrt$$(ENDL)  woa$$(ENDL))
endif
endef
