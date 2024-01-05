/**
 * --------------------------------------
 * 
 * CEaShell Source Code - fileSystem.h
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022 - 2024
 * License: GPL-3.0
 * 
 * --------------------------------------
**/

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Alphabetically sorts the VAT.
 * 
 */
void asm_fileSystem_sortVAT(void);

/**
 * @brief Finds and caches all program VAT pointers.
 * 
 * @param programPtrs Array to cache VAT pointers in.
 * @param showHiddenPrograms Whether or not to include hidden programs when caching.
 */
void asm_fileSystem_getProgramPtrs(void **programPtrs, bool showHiddenPrograms);

/**
 * @brief Finds and caches all AppVar VAT pointers.
 * 
 * @param appvarPtrs Array to cache VAT pointers in.
 */
void asm_fileSystem_getAppVarPtrs(void **appvarPtrs);

/**
 * @brief Counts the number of programs and AppVars on the calculator.
 * 
 * @param programCount Variable to store the total number of programs in.
 * @param appVarCount Variable to store the total number of AppVars in.
 * @param showHiddenPrograms Whether or not to include hidden programs when counting.
 */
void asm_fileSystem_findAllVars(unsigned int *programCount, unsigned int *appVarCount, bool showHiddenPrograms);

#ifdef __cplusplus
}
#endif

#endif
