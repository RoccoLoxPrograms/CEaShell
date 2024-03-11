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
 * @brief Finds spots in RAM for the AppVar and app pointer arrays.
 * 
 * @param programCount Number of programs.
 * @param appVarCount Number of AppVars.
 * @param appCount Number of apps.
 * @param appvarPtrs Pointer to AppVar pointers pointer.
 * @param appPtrs Pointer to app pointers pointer.
 */
void asm_fileSystem_initPtrArrays(unsigned int programCount, unsigned int appVarCount, unsigned int appCount, void ***appvarPtrs, void ***appPtrs);

/**
 * @brief Finds and caches all program VAT pointers.
 * 
 * @param programPtrs Array to cache VAT pointers in.
 * @param showHiddenPrograms Whether or not to include hidden programs when caching.
 * @param searchString Search string value, or NULL if no search.
 */
void asm_fileSystem_getProgramPtrs(void **programPtrs, bool showHiddenPrograms, char *searchString);

/**
 * @brief Finds and caches all AppVar VAT pointers.
 * 
 * @param appvarPtrs Array to cache VAT pointers in.
 * @param searchString Search string value, or NULL if no search.
 */
void asm_fileSystem_getAppVarPtrs(void **appvarPtrs, char *searchString);

/**
 * @brief Counts the number of programs and AppVars on the calculator.
 * 
 * @param programCount Variable to store the total number of programs in.
 * @param appVarCount Variable to store the total number of AppVars in.
 * @param showHiddenPrograms Whether or not to include hidden programs when counting.
 * @param searchString Search string value, or NULL if no search.
 */
void asm_fileSystem_findAllVars(unsigned int *programCount, unsigned int *appVarCount, bool showHiddenPrograms, char *searchString);

/**
 * @brief Finds the offset at which ptr occurs in the array ptrs.
 * 
 * @param ptr Pointer to search for.
 * @param ptrs Array of pointers to search.
 * @return unsigned int Offset at which ptr occurs.
 */
unsigned int asm_fileSystem_findArrayOffset(void *ptr, void **ptrs);

#ifdef __cplusplus
}
#endif

#endif
