/**
 * --------------------------------------
 * 
 * CEaShell Source Code - shapes.h
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022 - 2023
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

void shapes_RoundRectangleFill(const uint8_t color, const uint8_t radius, const int width, const uint8_t height, const int x, const uint8_t y);

void shapes_PixelIndentRectangle(const uint8_t fgColor, const uint8_t bgColor, const unsigned int x, const uint8_t y, const unsigned int width, const uint8_t height);

void shapes_GetTransparentRect(gfx_sprite_t *tileSprite, const uint8_t color, int x, uint8_t y);

void shapes_DrawTransparentRect(gfx_sprite_t *tileSprite, const int x, const uint8_t y);    // Draws a semi-transparent filled rectangle with rounded corners

// Works in two parts with shapes_DrawRoundCorners. The first one gets the sprite to draw and the second draws it
void shapes_GetRoundCorners(gfx_sprite_t *corner1, const uint8_t color, const uint8_t radius, const int x, const uint8_t y);

void shapes_DrawRoundCorners(gfx_sprite_t *corner1, const uint8_t width, const uint8_t height, const int x, const uint8_t y);

void shapes_FileIcon(const uint8_t color, const uint8_t bgColor, const int x, const uint8_t y);

void shapes_Folder(const uint8_t color, const uint8_t bgColor, const int x, const uint8_t y);

#ifdef __cplusplus
}
#endif

#endif
