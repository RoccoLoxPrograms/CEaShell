/**
 * --------------------------------------
 * 
 * CEaShell Source Code - runProgram.h
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022 - 2023
 * License: GPL-3.0
 * 
 * --------------------------------------
**/

#ifndef RUNPROGRAM_H
#define RUNPROGRAM_H

#include <defines.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Runs a program.
 * 
 * @param name Name of the program.
 * @param type OS type for the program.
 * @param shellType Shell type for the program.
 * @param shellPrefs Shell preferences struct.
 */
void asm_runProgram_run(char *name, uint8_t type, uint8_t shellType, struct preferences_t *shellPrefs);

#ifdef __cplusplus
}
#endif

#endif
