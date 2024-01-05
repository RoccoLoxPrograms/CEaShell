/**
 * --------------------------------------
 * 
 * CEaShell Source Code - settings.h
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022 - 2024
 * License: GPL-3.0
 * 
 * --------------------------------------
**/

#ifndef SETTINGS_H
#define SETTINGS_H

#include "defines.h"

/**
 * @brief Opens the settings context.
 * 
 * @param shellPrefs Shell preferences struct.
 * @param shellContext Shell context struct.
 */
void settings_Open(struct preferences_t *shellPrefs, struct context_t *shellContext);

/**
 * @brief Screen displaying credits and other info about CEaShell.
 * 
 * @param shellPrefs Shell perferences struct.
 * @param shellContext Shell context struct.
 */
void menu_AboutScreen(struct preferences_t *shellPrefs, struct context_t *shellContext);

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif
