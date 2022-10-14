/**
 * --------------------------------------
 * 
 * CEaShell Source Code - hooks.h
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022
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

void installHomescreenHook(void);

void removeHomescreenHook(void);

void installMenuHook(void);

void removeMenuHook(void);

bool checkMenuHookInstalled(void);

void installGetCSCHook(void);

void removeGetCSCHook(void);

bool checkGetCSCHookInstalled(void);

void installStopHook(void);

void removeStopHook(void);

#ifdef __cplusplus
}
#endif

#endif
