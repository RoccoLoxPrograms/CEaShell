#ifndef SHAPES_H
#define SHAPES_H

#include <stdint.h>
#include <stdbool.h>
#include <graphx.h>

#ifdef __cplusplus
extern "C" {
#endif

void shapes_RoundRectangleFill(uint8_t, uint8_t, int, uint8_t, int, uint8_t);	// Draws a filled rectangle with rounded corners

void shapes_TransparentRect(uint8_t, int, uint8_t, int, uint8_t);   // Draws a semi-transparent filled rectangle with rounded corners

void shapes_GetRoundCorners(gfx_sprite_t *, uint8_t, uint8_t, int, uint8_t);

void shapes_DrawRoundCorners(gfx_sprite_t *, uint8_t, uint8_t, int, uint8_t);

void shapes_FileIcon(uint8_t, uint8_t, int, uint8_t);

#ifdef __cplusplus
}
#endif

#endif
