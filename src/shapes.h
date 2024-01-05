/**
 * --------------------------------------
 * 
 * CEaShell Source Code - shapes.h
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022 - 2024
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

/**
 * @brief Draws a filled rectangle with rounded corners of a user-specified radius.
 * 
 * @param radius Radius of the rounded corners.
 * @param x X coordinate to draw the rectangle at.
 * @param y Y coordinate to draw the rectangle at.
 * @param width Width of the rectangle.
 * @param height Height of the rectangle.
 */
void shapes_RoundRectangleFill(const uint8_t radius, const int x, const uint8_t y, const int width, const uint8_t height);

/**
 * @brief Draws a filled rectangle with a pixel indent in each of the corners. (Destroys current GFX color)
 * 
 * @param x X coordinate to draw the rectangle at.
 * @param y Y coordinate to draw the rectangle at.
 * @param width Width of the rectangle.
 * @param height Height of the rectangle.
 */
void shapes_PixelIndentRectangle(const unsigned int x, const uint8_t y, const unsigned int width, const uint8_t height);

/**
 * @brief Sets up a semi-transparent tile in tileSprite to be used later with shapes_DrawTransparentRect.
 * 
 * @param tileSprite Sprite to store semi-transparent tile in.
 * @param color Color of the non-transparent pixels in the tile.
 * @param x X coordinate to use for setting up the tile.
 * @param y Y coordinate to use for setting up the tile.
 */
void shapes_GetTransparentRect(gfx_sprite_t *tileSprite, const uint8_t color, int x, uint8_t y);

/**
 * @brief Draws a semi-transparent rectangle with rounded corners, used for the hidden file effect. The height and width of the rectangle are both 64 pixels.
 * 
 * @param tileSprite Semi-transparent tile created using shapes_GetTransparentRect.
 * @param scale UI scale.
 * @param x X coordinate to begin drawing the rectangle at.
 * @param y Y coordinate to begin drawing the rectangle at.
 */
//void shapes_DrawTransparentRect(gfx_sprite_t *tileSprite, uint8_t scale, const int x, const uint8_t y);

/**
 * @brief Sets up a corner tile in corner1 to be used later with shapes_DrawRoundCorners.
 * 
 * @param corner1 Sprite to store corner tile in.
 * @param color Color of the outside of the corner tile (The non-transparent part).
 * @param radius Radius of the corner.
 * @param x X coordinate to use for setting up the tile.
 * @param y Y coordinate to use for setting up the tile.
 */
void shapes_GetRoundCorners(gfx_sprite_t *corner1, const uint8_t color, const uint8_t radius, const int x, const uint8_t y);

/**
 * @brief Draws inverted corners around an object to give the impression of "rounding" it.
 * 
 * @param corner1 Corner tile sprite created using shapes_GetRoundCorners.
 * @param width Width of the rectangle being "rounded".
 * @param height Height of the rectangle being "rounded".
 * @param x X coordinate to begin drawing at.
 * @param y Y coordinate to begin drawing at.
 */
void shapes_DrawRoundCorners(gfx_sprite_t *corner1, const uint8_t width, const uint8_t height, const int x, const uint8_t y);

/**
 * @brief Draws the default file icon.
 * 
 * @param scale UI icon scale.
 * @param shellType Shell file type.
 * @param x X coordinate to begin drawing the icon at.
 * @param y Y coordinate to begin drawing the icon at.
 */
void shapes_FileIcon(uint8_t scale, uint8_t shellType, const int x, const uint8_t y);

/**
 * @brief Draws the default folder icon/
 * 
 * @param scale UI icon scale.
 * @param x X coordinate to begin drawing the icon at.
 * @param y Y coordinate to begin drawing the icon at.
 */
void shapes_Folder(uint8_t scale, const int x, const uint8_t y);

#ifdef __cplusplus
}
#endif

#endif
