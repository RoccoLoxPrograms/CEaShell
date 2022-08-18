#ifndef FILEOPS_H
#define FILEOPS_H

#include <stdint.h>
#include <graphx.h>

#ifdef __cplusplus
extern "C" {
#endif

uint8_t getPrgmType(char *, uint8_t);

void hidePrgm(char *, uint8_t);

void lockPrgm(char *, uint8_t);

bool getIconASM(char *, uint8_t, uint8_t, gfx_sprite_t *);

#ifdef __cplusplus
}
#endif

#endif
