/**
 * --------------------------------------
 * 
 * CEaShell Source Code - fileOps.h
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022
 * License: GPL-3.0
 * 
 * --------------------------------------
**/

#ifndef FILEOPS_H
#define FILEOPS_H

#include <stdint.h>
#include <graphx.h>

#ifdef __cplusplus
extern "C" {
#endif

uint8_t getPrgmType(char *, uint8_t);   // Gets the program's actual type (ASM, C, ICE, etc)

void hidePrgm(char *, uint8_t); // Toggles a program's hidden status

void lockPrgm(char *, uint8_t); // Toggles a program's protected status

bool getDescASM(char *, uint8_t, uint8_t, char *);  // Gets description from C, ICE, or ASM program

bool getDescBASIC(char *, uint8_t, char *); // Gets a description from a BASIC program

bool getIconASM(char *, uint8_t, uint8_t, gfx_sprite_t *);  // Gets an icon from a C, ICE, or ASM program and returns false if it does not have one

bool getIconDCS(char *, uint8_t, gfx_sprite_t *);   // Gets a DCS format icon

int getProgSize(char *, uint8_t);   // Gets the actual size of a program (ti_GetSize is slightly off)

int getASMPrgmSize(void); 

#ifdef __cplusplus
}
#endif

#endif
