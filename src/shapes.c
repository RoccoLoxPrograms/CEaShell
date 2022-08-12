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

void shapes_TransparentRect(uint8_t color, int width, uint8_t height, int x, uint8_t y) {
    gfx_SetColor(color);
    for (uint8_t pixelXOffset = 0; pixelXOffset <= width + 1; pixelXOffset += 2) {
        for (uint8_t pixelYOffset = 0; pixelYOffset <= height; pixelYOffset += 2) {
            gfx_SetPixel(x + pixelXOffset, y + pixelYOffset);
            gfx_SetPixel(x + pixelXOffset + 1, y + pixelYOffset + 1);
        }
    }
}

void shapes_RoundRectangle(bool buffer, uint8_t color, uint8_t radius, uint8_t thickness, int width, uint8_t height, int x, uint8_t y) {
    gfx_UninitedSprite(corner1, radius, radius);
    gfx_UninitedSprite(corner2, radius, radius);
    corner1->width = radius;
    corner1->height = radius;
    corner2->width = radius;
    corner2->height = radius;
    gfx_SetColor(240);	// Transparent color
    gfx_SetDrawBuffer();
    gfx_FillRectangle_NoClip(x, y, radius, radius);
    gfx_SetColor(color);
    gfx_FillCircle_NoClip(x + radius, y + radius, radius);
    gfx_SetColor(240);
    gfx_FillCircle_NoClip(x + radius, y + radius, radius - thickness);
    gfx_SetColor(color);
    gfx_GetSprite_NoClip(corner1, x, y);
    gfx_BlitScreen();
    gfx_TransparentSprite_NoClip(corner1, x, y);
    gfx_RotateSpriteC(corner1, corner2);
    gfx_TransparentSprite_NoClip(corner2, x + width - radius, y);
    gfx_RotateSpriteC(corner2, corner1);
    gfx_TransparentSprite_NoClip(corner1, x + width - radius, y + height - radius);
    gfx_RotateSpriteC(corner1, corner2);
    gfx_TransparentSprite_NoClip(corner2, x, y + height - radius);
    gfx_FillRectangle_NoClip(x + radius, y, width - radius * 2, thickness);
    gfx_FillRectangle_NoClip(x, y + radius, thickness, height - radius * 2);
    gfx_FillRectangle_NoClip(x + width - thickness, y + radius, thickness, height - radius * 2);
    gfx_FillRectangle_NoClip(x + radius, y + height - thickness, width - radius * 2, thickness);
    if (!buffer) {
        gfx_SwapDraw();
    }
}

void shapes_RoundCorners(bool buffer, uint8_t bg_color, uint8_t radius, int width, uint8_t height, int x, uint8_t y) {
    gfx_UninitedSprite(corner1, radius, radius);
    gfx_UninitedSprite(corner2, radius, radius);
    corner1->width = radius;
    corner1->height = radius;
    corner2->width = radius;
    corner2->height = radius;
    gfx_SetDrawBuffer();
    gfx_SetColor(bg_color);
    gfx_FillRectangle_NoClip(x, y, radius, radius);
    gfx_SetColor(240);
    gfx_FillCircle_NoClip(x + radius, y + radius, radius);
    gfx_GetSprite(corner1, x, y);
    gfx_BlitScreen();
    gfx_TransparentSprite_NoClip(corner1, x, y);
    gfx_RotateSpriteC(corner1, corner2);
    gfx_TransparentSprite_NoClip(corner2, x + width - radius, y);
    gfx_RotateSpriteC(corner2, corner1);
    gfx_TransparentSprite_NoClip(corner1, x + width - radius, y + height - radius);
    gfx_RotateSpriteC(corner1, corner2);
    gfx_TransparentSprite_NoClip(corner2, x, y + height - radius);
    if (!buffer) {
        gfx_SwapDraw();
    }
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
