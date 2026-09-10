#--------------------------------------
#
# CEaShell Source Code - makefile
# By RoccoLox Programs and TIny_Hacker
# Copyright 2022 - 2026
# License: GPL-3.0
#
#--------------------------------------

NAME = CEaShell
ICON = icon.png
APPVAR_PREFIX = CEaShel
APPLICATION = YES
APPLICATION_DESCRIPTION = "(C)  2022-2026  RoccoLox  &  TIny_Hacker"
CEASHELL_VERSION = 2.0.3
APPVAR_VERSION = 0
COMPRESSED = NO
ARCHIVED = YES
LANGUAGE = EN

CFLAGS = -Wall -Wextra -Oz -DVERSION_NO=\"$(CEASHELL_VERSION)\" -DAPPVAR_VERSION=$(APPVAR_VERSION) -D$(LANGUAGE)=true
CXXFLAGS = -Wall -Wextra -Oz -DVERSION_NO=\"$(CEASHELL_VERSION)\" -DAPPVAR_VERSION=$(APPVAR_VERSION) -D$(LANGUAGE)=true

# ----------------------------

include $(shell cedev-config --makefile)

appvar:
	@make
	@convbin --iformat 8ek --input bin/$(NAME).8ek --oformat 8xv-split --maxvarsize 65200 --output bin/$(APPVAR_PREFIX).8xv --name $(APPVAR_PREFIX)
