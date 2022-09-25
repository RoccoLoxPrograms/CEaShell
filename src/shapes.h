/**
 * --------------------------------------
 * 
 * CEaShell Source Code - shapes.h
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022
 * License: GPL-3.0
 * 
 * --------------------------------------
**/

#ifndef SHAPES_H
#define SHAPES_H

#include <stdint.h>
#include <stdbool.h>
#include <graphx.h>

#ifdef __cplusplus
extern "C" {
#endif

void shapes_RoundRectangleFill(const uint8_t, const uint8_t, const int, const uint8_t, const int, const uint8_t);	// Draws a filled rectangle with rounded corners

void shapes_PixelIndentRectangle(const uint8_t, const uint8_t, const unsigned int, const uint8_t, const unsigned int, const uint8_t);

void shapes_GetTransparentRect(gfx_sprite_t *, const uint8_t, int, uint8_t);

void shapes_DrawTransparentRect(gfx_sprite_t *, const int, const uint8_t);   // Draws a semi-transparent filled rectangle with rounded corners

// Works in two parts with shapes_DrawRoundCorners. The first one gets the sprite to draw and the second draws it
void shapes_GetRoundCorners(gfx_sprite_t *, const uint8_t, const uint8_t, const int, const uint8_t);

void shapes_DrawRoundCorners(gfx_sprite_t *, const uint8_t, const uint8_t, const int, const uint8_t);

void shapes_FileIcon(const uint8_t, const uint8_t, const int, const uint8_t);

void shapes_Folder(const uint8_t, const uint8_t, const int, const uint8_t);

#ifdef __cplusplus
}
#endif

#endif
