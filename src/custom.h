/**
 * --------------------------------------
 * 
 * CEaShell Source Code - custom.h
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022 - 2025
 * License: GPL-3.0
 * 
 * --------------------------------------
**/

#ifndef CUSTOM_H
#define CUSTOM_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Opens the customization context.
 * 
 * @param shellPrefs Shell preferences struct.
 * @param shellContext Shell context struct.
 */
void custom_Open(struct preferences_t *shellPrefs, struct context_t *shellContext);

#ifdef __cplusplus
}
#endif

#endif
