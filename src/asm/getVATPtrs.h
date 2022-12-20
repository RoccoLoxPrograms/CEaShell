/**
 * --------------------------------------
 * 
 * CEaShell Source Code - getVATPtrs.h
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022
 * License: GPL-3.0
 * 
 * --------------------------------------
**/

#ifndef GETVATPTRS_H
#define GETVATPTRS_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void getProgramPtrs(void **programPtrs, bool hideHiddenPrograms);

void getAppVarPtrs(void **appvarPtrs);

#ifdef __cplusplus
}
#endif

#endif
