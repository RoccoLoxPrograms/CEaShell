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

bool getIconASM(char *, uint8_t, uint8_t, gfx_sprite_t *);  // Gets an icon from a C, ICE, or ASM program and returns false if it does not have one

#ifdef __cplusplus
}
#endif

#endif
