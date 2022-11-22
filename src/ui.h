/**
 * --------------------------------------
 * 
 * CEaShell Source Code - ui.h
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022
 * License: GPL-3.0
 * 
 * --------------------------------------
**/

#ifndef UI_H
#define UI_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UI_BATTERY	    0
#define UI_CHARGING	    1
#define UI_PAINT	    2
#define UI_INFO		    3
#define UI_SETTINGS	    4
#define UI_LARROW	    5
#define UI_RARROW	    6
#define UI_DARROW       7
#define UI_CHECK        8
#define UI_CURSOR_A     9
#define UI_CURSOR_1     10

void ui_DrawUISprite(const uint8_t, const uint8_t, const int, const uint8_t);

void ui_DrawFile(const bool, const bool, const bool, const bool, uint8_t *, char *, const uint8_t, const uint8_t, const int, const uint8_t);

void ui_CheckBox(const uint8_t, const uint8_t, const bool, const int, const uint8_t);

void ui_Clock(const bool);

void ui_Battery(const uint8_t, const uint8_t, const bool);

void ui_StatusBar(const uint8_t, const bool, const uint8_t, const char *, const unsigned int, const bool);

void ui_DescriptionWrap(const char *, const uint8_t, const int, const uint8_t);

void ui_BottomBar(const uint8_t);

bool ui_DeleteConf(uint8_t *, const int, const uint8_t);

bool ui_RenameBox(uint8_t *, char *);

void ui_DrawAllFiles(uint8_t *, const uint8_t, const uint8_t, const unsigned int, const uint8_t, const bool, const bool, const bool, const bool);

void ui_MiniCursor(uint8_t, int, uint8_t);

void ui_DrawMenuItem(const char *, const int, const uint8_t, const bool);

void ui_DrawDoubleMenuItem(const char *, const char *, const int, const uint8_t, const bool);

void ui_AboutScreen(uint8_t *);

void ui_NewUser(void);

uint8_t ui_CopyNewMenu(uint8_t *, char *);

#ifdef __cplusplus
}
#endif

#endif
