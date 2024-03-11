/**
 * --------------------------------------
 * 
 * CEaShell Source Code - apps.h
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022 - 2024
 * License: GPL-3.0
 * 
 * --------------------------------------
**/

#ifndef APPS_H
#define APPS_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Finds and caches all app pointers.
 * 
 * @param appvarPtrs Array to cache app pointers in.
 * @param showCEaShell If CEaShell should be skipped over.
*/
void asm_apps_getAppPtrs(void **appPointers, bool showCEaShell);

/**
 * @brief Finds an app name and copies it.
 * 
 * @param appPointer Pointer to the app.
 * @param appName Pointer to copy the name string into.
*/
void asm_apps_getAppName(void *appPointer, char *appName);

/**
 * @brief Finds the total number of apps.
 * 
 * @return uint8_t Total number of apps.
*/
uint8_t asm_apps_findAllApps(void);

/**
 * @brief Get the size of an app, given a name.
 * 
 * @param appPointer Pointer to the app.
 * @return unsigned int Size of the app.
 */
unsigned int asm_apps_getAppSize(void *appPointer);

/**
 * @brief Gets the Minimum OS Version of an app, given a name.
 * 
 * @param appPointer Pointer to the app.
 * @return char* Pointer to the Minimum OS Version string of the app.
 */
char *asm_apps_getAppMinOSVersion(void *appPointer);

/**
 * @brief Finds an app copyright string and copies it.
 * 
 * @param appPointer Pointer to the app.
 * @param copyright Pointer to copy the copyright string into. (First byte is \0 if no copyright)
 */
void asm_apps_getAppCopyrightInfo(void *appPointer, char *copyright);

/**
 * @brief Checks and returns the icon info of an app.
 * 
 * @param appPointer Pointer to the app.
 * @param icon Pointer to return the icon sprite in.
 * @return true The app has an icon.
 * @return false The app does not have an icon.
*/
bool asm_apps_getAppIcon(void *appPointer, void *icon);

/**
 * @brief Executes an app.
 * 
 * @param appPointer Pointer to the app.
 */
void asm_apps_executeApp(void *appPointer);

/**
 * @brief Deletes an app.
 * 
 * @param appPointer Pointer to the app.
 */
void asm_apps_deleteApp(void *appPointer);

/**
 * @brief Exits and ensures defragmenting is done if necessary.
 * 
 */
void asm_apps_exitDefrag(void);

#ifdef __cplusplus
}
#endif

#endif
