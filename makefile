#--------------------------------------
#
# CEaShell Source Code - makefile
# By RoccoLox Programs and TIny_Hacker
# Copyright 2022
# License: GPL-3.0
#
#--------------------------------------

NAME = CEaShell
APP_NAME = CEaShell
APP_VERSION = 5.0.0.0000
CEASHELL_VERSION = 0.76
ICON = icon.png
DESCRIPTION = (C)  2022  RoccoLox  +  TIny_Hacker
COMPRESSED = NO
ARCHIVED = YES

CFLAGS = -Wall -Wextra -Oz -DVERSION_NO=\"$(CEASHELL_VERSION)\"
CXXFLAGS = -Wall -Wextra -Oz -DVERSION_NO=\"$(CEASHELL_VERSION)\"

# ----------------------------

include app_tools/makefile
