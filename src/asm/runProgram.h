/**
 * --------------------------------------
 * 
 * CEaShell Source Code - runProgram.h
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022
 * License: GPL-3.0
 * 
 * --------------------------------------
**/

#ifndef RUNPROGRAM_H
#define RUNPROGRAM_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void runProgram(char *name, uint8_t type, uint8_t shellType, bool editLockedPrgms);

void removeExecuteHookInstalled(void);

void reloadApp(void);

#ifdef __cplusplus
}
#endif

#endif
