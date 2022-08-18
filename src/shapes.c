#include "shapes.h"

#include <graphx.h>

void shapes_RoundRectangleFill(uint8_t color, uint8_t radius, int width, uint8_t height, int x, uint8_t y) {
    gfx_SetColor(color);
    gfx_FillCircle_NoClip(x + radius, y + radius, radius);
    gfx_FillCircle_NoClip(x + radius, y + height - radius - 1, radius);
    gfx_FillCircle_NoClip(x + width - radius - 1, y + radius, radius);
    gfx_FillCircle_NoClip(x + width - radius - 1, y + height - radius - 1, radius);
    gfx_FillRectangle_NoClip(x, y + radius, width, height - radius * 2);
    gfx_FillRectangle_NoClip(x + radius, y, width - radius * 2, height);
}

void shapes_GetTransparentRect(gfx_sprite_t *tileSprite, uint8_t color, int x, uint8_t y) {
    x += 2; // Make sure it's hidden behind whatever we draws
    y += 2;
    gfx_SetColor(240);
    gfx_FillRectangle_NoClip(x, y, 16, 16);
    gfx_SetColor(color);
    for (uint8_t pixelXOffset = 0; pixelXOffset < 18; pixelXOffset += 2) {
        for (uint8_t pixelYOffset = 0; pixelYOffset < 17; pixelYOffset += 2) {
            gfx_SetPixel(x + pixelXOffset, y + pixelYOffset);
            gfx_SetPixel(x + pixelXOffset + 1, y + pixelYOffset + 1);
        }
    }
    gfx_GetSprite_NoClip(tileSprite, x, y);
}

void shapes_DrawTransparentRect(gfx_sprite_t *tileSprite, int x, uint8_t y) {
    for (uint8_t pixelXOffset = 0; pixelXOffset < 64; pixelXOffset += 16) {
        for (uint8_t pixelYOffset = 0; pixelYOffset < 64; pixelYOffset += 16) {
            gfx_TransparentSprite_NoClip(tileSprite, x + pixelXOffset, y + pixelYOffset);
        }
    }
}

void shapes_GetRoundCorners(gfx_sprite_t *corner1, uint8_t color, uint8_t radius, int x, uint8_t y) {
    gfx_SetColor(color);
    gfx_FillRectangle_NoClip(x, y, radius, radius);
    gfx_SetColor(240);
    gfx_FillCircle_NoClip(x + radius, y + radius, radius);
    gfx_GetSprite(corner1, x, y);
}

void shapes_DrawRoundCorners(gfx_sprite_t *corner1, uint8_t width, uint8_t height, int x, uint8_t y) {
    gfx_sprite_t *corner2 = gfx_MallocSprite(corner1->width, corner1->height);
    gfx_TransparentSprite_NoClip(corner1, x, y);
    gfx_RotateSpriteC(corner1, corner2);
    gfx_TransparentSprite_NoClip(corner2, x + width - corner1->width, y);
    gfx_RotateSpriteC(corner2, corner1);
    gfx_TransparentSprite_NoClip(corner1, x + width - corner1->width, y + height - corner1->width);
    gfx_RotateSpriteC(corner1, corner2);
    gfx_TransparentSprite_NoClip(corner2, x, y + height - corner1->width);
    
    free (corner2);
}

void shapes_FileIcon(uint8_t color, uint8_t bgColor, int x, uint8_t y) {
    gfx_SetColor(color);
    for (int i = 0; i < 3; i++) {
        gfx_Rectangle_NoClip(x + i, y + i, 32 - 2 * i, 42 - 2 * i);
    }
    gfx_FillRectangle_NoClip(x + 17, y, 15, 15);
    gfx_SetColor(bgColor);
    gfx_FillTriangle_NoClip(x + 20, y, x + 32, y, x + 32, y + 12);
    gfx_SetPixel(x, y);
    gfx_SetPixel(x, y + 41);
    gfx_SetPixel(x + 31, y + 41);
}
