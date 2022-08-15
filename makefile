# ----------------------------
# Makefile Options
# ----------------------------

NAME = CEASHELL
ICON = icon.png
DESCRIPTION = "CEaShell v0.39"
COMPRESSED = YES
ARCHIVED = YES

CFLAGS = -Wall -Wextra -Oz
CXXFLAGS = -Wall -Wextra -Oz

# ----------------------------

include $(shell cedev-config --makefile)

final:
	make
	convbin -r -e zx0 -k 8xp-compressed -u -n $(NAME) -i bin/$(NAME).bin -o bin/$(NAME).8xp
