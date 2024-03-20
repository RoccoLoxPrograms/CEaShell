/**
 * --------------------------------------
 * 
 * CEaShell Source Code - shapes.c
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022 - 2024
 * License: GPL-3.0
 * 
 * --------------------------------------
**/

#include "defines.h"

#include "shapes.h"

#include "gfx/gfx.h"

void shapes_RoundRectangleFill(uint8_t radius, int x, uint8_t y, int width, uint8_t height) {
    gfx_FillCircle_NoClip(x + radius, y + radius, radius);
    gfx_FillCircle_NoClip(x + radius, y + height - radius - 1, radius);
    gfx_FillCircle_NoClip(x + width - radius - 1, y + radius, radius);
    gfx_FillCircle_NoClip(x + width - radius - 1, y + height - radius - 1, radius);
    gfx_FillRectangle_NoClip(x, y + radius, width, height - radius * 2);
    gfx_FillRectangle_NoClip(x + radius, y, width - radius * 2, height);
}

void shapes_PixelIndentRectangle(unsigned int x, uint8_t y, unsigned int width, uint8_t height) {
    gfx_FillRectangle(x + 1, y, width - 2, height);
    gfx_Rectangle(x, y + 1, width, height - 2);
}

void shapes_GetRoundCorners(gfx_sprite_t *corner1, uint8_t color, uint8_t radius, int x, uint8_t y) {
    gfx_UninitedSprite(cornerBackup, 25, 25);
    cornerBackup->width = radius * 2 + 1;
    cornerBackup->height = radius * 2 + 1;
    gfx_GetSprite_NoClip(cornerBackup, x, y);
    gfx_SetColor(color);
    gfx_FillRectangle_NoClip(x, y, radius, radius);
    gfx_SetColor(240);
    gfx_FillCircle_NoClip(x + radius, y + radius, radius);
    gfx_GetSprite_NoClip(corner1, x, y);
    gfx_Sprite_NoClip(cornerBackup, x, y);
}

void shapes_DrawRoundCorners(gfx_sprite_t *corner1, uint8_t width, uint8_t height, int x, uint8_t y) {
    gfx_UninitedSprite(corner2, 12, 12);
    corner2->width = corner1->width;
    corner2->height = corner1->height;

    gfx_TransparentSprite_NoClip(corner1, x, y);
    gfx_RotateSpriteC(corner1, corner2);
    gfx_TransparentSprite_NoClip(corner2, x + width - corner1->width, y);
    gfx_RotateSpriteC(corner2, corner1);
    gfx_TransparentSprite_NoClip(corner1, x + width - corner1->width, y + height - corner1->width);
    gfx_RotateSpriteC(corner1, corner2);
    gfx_TransparentSprite_NoClip(corner2, x, y + height - corner1->width);
}

void shapes_FileIcon(uint8_t scale, uint8_t shellType, int x, uint8_t y) {
    static const gfx_sprite_t *fileTypeIcons[20] = {
        typeASM, typeC, typeBasic, typeICE, typeSrc, NULL, typeAppVar, NULL, typeCelticVar, typeApp,
        typeASMAlt, typeCAlt, typeBasicAlt, typeICEAlt, typeSrcAlt, NULL, typeAppVarAlt, NULL, typeCelticVarAlt, typeAppAlt
    };

    uint8_t bgColor = gfx_GetPixel(x, y);
    uint8_t fgColor = 0 + 255 * !(gfx_GetPixel(x, y) > 131 && gfx_GetPixel(x, y) % 8 > 3);
    gfx_SetColor(0 + 255 * (fgColor != 255));
    gfx_FillRectangle_NoClip(x + scale, y + scale, 6 * scale, 8 * scale);
    gfx_SetColor(fgColor);

    for (int i = 0; i < scale; i++) {
        gfx_Rectangle_NoClip(x + i, y + i, 8 * scale - i * 2, 10 * scale - i * 2);
    }

    gfx_FillRectangle_NoClip(x + 4 * scale, y, 4 * scale, 4 * scale);

    unsigned int fileTypeX = 0; // Minimize function calls this way
    uint8_t fileTypeY = 0;

    switch (scale) {
        case SCALE_SMALLEST:
            fileTypeX = x - 2;
            fileTypeY = y + 6;
            break;
        case SCALE_SMALL:
            fileTypeX = x + 2;
            fileTypeY = y + 11;
            break;
        case SCALE_MEDIUM:
            fileTypeX = x + 6;
            fileTypeY = y + 18;
            break;
        case SCALE_LARGE:
            fileTypeX = x + 5;
            fileTypeY = y + 21;
            break;
        default:
            break;
    }

    gfx_FillRectangle_NoClip(fileTypeX - (scale + (scale == 4)), fileTypeY - (1 + (scale == 4)), scale * 2 + 12 * (1 + 1 * (scale == 4)), 7 * (1 + 1 * (scale == 4)));
    gfx_ScaledTransparentSprite_NoClip(fileTypeIcons[shellType + 10 * (fgColor == 255)], fileTypeX + (4 * (shellType == C_TYPE)) * (1 + 1 * (scale == 4)), fileTypeY, 1 + (scale == 4), 1 + (scale == 4));

    gfx_SetColor(bgColor);
    gfx_FillTriangle_NoClip(x + 5 * scale, y, x + 8 * scale, y, x + 8 * scale, y + 3 * scale);

    if (scale > 1) {
        gfx_SetPixel(x, y);
        gfx_SetPixel(x, y + 10 * scale - 1);
        gfx_SetPixel(x + 8 * scale - 1, y + 10 * scale - 1);
    }
}

void shapes_Folder(uint8_t scale, int x, uint8_t y) {
    uint8_t bgColor = gfx_GetPixel(x, y);
    uint8_t fgColor = 0 + 255 * !(bgColor > 131 && bgColor % 8 > 3);
    gfx_SetColor(0 + 255 * (fgColor != 255));
    gfx_FillRectangle_NoClip(x + scale, y + scale * 2, 8 * scale, 5 * scale);
    gfx_SetColor(fgColor);

    for (int i = 0; i < scale; i++) {
        gfx_Rectangle_NoClip(x + i, y + scale + i, 10 * scale - i * 2, 7 * scale - i * 2);
    }

    gfx_FillRectangle_NoClip(x, y, 5 * scale, scale);

    if (scale > 1) {
        gfx_SetColor(bgColor);
        gfx_SetPixel(x, y);
        gfx_SetPixel(x, y + 8 * scale - 1);
        gfx_SetPixel(x + 5 * scale - 1, y);
        gfx_SetPixel(x + 10 * scale - 1, y + scale);
        gfx_SetPixel(x + 10 * scale - 1, y + 8 * scale - 1);
    }
}
