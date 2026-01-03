/**
 * --------------------------------------
 * 
 * CEaShell Source Code - settings.h
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022 - 2026
 * License: GPL-3.0
 * 
 * --------------------------------------
**/

#ifndef SETTINGS_H
#define SETTINGS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Opens the settings context.
 * 
 * @param shellPrefs Shell preferences struct.
 * @param shellContext Shell context struct.
 */
void settings_Open(struct preferences_t *shellPrefs, struct context_t *shellContext);

#ifdef __cplusplus
}
#endif

#endif
