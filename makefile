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
ICON = icon.png
DESCRIPTION = (C)  2022  RoccoLox  +  TIny_Hacker
COMPRESSED = NO
ARCHIVED = YES

CFLAGS = -Wall -Wextra -Oz
CXXFLAGS = -Wall -Wextra -Oz

# ----------------------------

include app_tools/makefile

final:
	make
	convbin -r -e zx0 -k 8xp-compressed -u -n $(NAME) -i bin/$(NAME).bin -o bin/$(NAME).8xp
