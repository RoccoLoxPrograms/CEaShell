/**
 * --------------------------------------
 * 
 * CEaShell Source Code - files.h
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022 - 2026
 * License: GPL-3.0
 * 
 * --------------------------------------
**/

#ifndef FILES_H
#define FILES_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Opens the file manager context (Also the main context for CEaShell).
 * 
 * @param shellPrefs Shell preferences struct.
 * @param shellContext Shell context struct.
 */
void files_Main(struct preferences_t *shellPrefs, struct context_t *shellContext);

#ifdef __cplusplus
}
#endif

#endif
