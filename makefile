#--------------------------------------
#
# CEaShell Source Code - makefile
# By RoccoLox Programs and TIny_Hacker
# Copyright 2022 - 2024
# License: GPL-3.0
#
#--------------------------------------

NAME = CEaShell
APP_NAME = CEaShell
APP_INST_NAME = CEASHELL
APP_VERSION = 5.0.0.0000
CEASHELL_VERSION = 2.0.0-beta.5
APPVAR_VERSION = 0
DESCRIPTION = "(C)  2022-2024  RoccoLox  &  TIny_Hacker"
COMPRESSED = NO
ARCHIVED = YES
LANGUAGE = EN

CFLAGS = -Wall -Wextra -Oz -DVERSION_NO=\"$(CEASHELL_VERSION)\" -DAPPVAR_VERSION=$(APPVAR_VERSION) -D$(LANGUAGE)=true
CXXFLAGS = -Wall -Wextra -Oz -DVERSION_NO=\"$(CEASHELL_VERSION)\" -DAPPVAR_VERSION=$(APPVAR_VERSION) -D$(LANGUAGE)=true

# ----------------------------

include app_tools/makefile
