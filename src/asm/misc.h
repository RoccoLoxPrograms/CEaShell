/**
 * --------------------------------------
 * 
 * CEaShell Source Code - misc.h
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022
 * License: GPL-3.0
 * 
 * --------------------------------------
**/

#ifndef MISC_H
#define MISC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

bool willGarbageCollect(char *name, uint8_t type);  // Check if archiving a file will garbage collect (since the toolchain function doesn't work)

void invertPalette(void);

#ifdef __cplusplus
}
#endif

#endif
