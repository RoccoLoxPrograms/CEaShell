/**
 * --------------------------------------
 * 
 * CEaShell Source Code - hooks.h
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022 - 2023
 * License: GPL-3.0
 * 
 * --------------------------------------
**/

#ifndef HOOKS_H
#define HOOKS_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// GetCSC hook codes
#define NONE            0
#define BOTH            1
#define ICON_HOOK       2
#define ON_SHORTS_HOOK  3

void installHomescreenHook(void);

void removeHomescreenHook(void);

bool checkHomescreenHookInstalled(void);

void installMenuHook(void);

void removeMenuHook(void);

bool checkMenuHookInstalled(void);

void installGetCSCHook(uint8_t hook);

void removeGetCSCHook(void);

bool checkGetCSCHookInstalled(uint8_t hook);

void installStopHook(void);

void removeStopHook(void);

void triggerAPD(void);

#ifdef __cplusplus
}
#endif

#endif
