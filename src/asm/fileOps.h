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

void unlockBasic(char *name, uint8_t type);  // Edit locked BASIC programs

void editBasicProg(char *name, uint8_t type);    // Edit a program

void editCelticAppvar(char *name);  // Edit a celtic appvar

uint8_t getPrgmType(char *name, uint8_t type);   // Gets the program's actual type (ASM, C, ICE, etc)

uint8_t getAppvarType(char *name);  // Checks if an Appvar is a Celtic Appvar

void hidePrgm(char *name, uint8_t type); // Toggles a program's hidden status

void lockPrgm(char *name, uint8_t type); // Toggles a program's protected status

bool getDescASM(char *name, uint8_t type, uint8_t shellType, char *outputDesc);  // Gets description from C, ICE, or ASM program

bool getDescBASIC(char *name, uint8_t type, char *outputDesc); // Gets a description from a BASIC program

bool getIconASM(char *name, uint8_t type, uint8_t shellType, gfx_sprite_t *outputIcon);  // Gets an icon from a C, ICE, or ASM program and returns false if it does not have one

bool getIconDCS(char *name, uint8_t type, gfx_sprite_t *outputIcon);   // Gets a DCS format icon

int getProgSize(char *name, uint8_t type);   // Gets the actual size of a program (ti_GetSize is slightly off)

int getASMPrgmSize(void); 

void copyProgram(char *name, char *copy, uint8_t);

#ifdef __cplusplus
}
#endif

#endif
