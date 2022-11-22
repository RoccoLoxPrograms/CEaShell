/**
 * --------------------------------------
 * 
 * CEaShell Source Code - menu.h
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022
 * License: GPL-3.0
 * 
 * --------------------------------------
**/

#ifndef MENU_H
#define MENU_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void menu_Looks(uint8_t *, unsigned int *, unsigned int, const unsigned int, bool *, uint8_t *, const uint8_t, bool *, const bool, const bool, const uint8_t, bool *, bool *);

void menu_Info(uint8_t *, bool *, const unsigned int, const unsigned int, unsigned int *, const uint8_t, const bool, const bool, const bool, const uint8_t, const bool, const bool);

void menu_Settings(uint8_t *, uint8_t *, bool *, bool *, bool *, bool *, bool *, bool *, uint8_t *);

#ifdef __cplusplus
}
#endif

#endif
