#include "gfx/gfx.h"
#include "menu.h"
#include "shapes.h"
#include "ui.h"

#include <graphx.h>
#include <keypadc.h>

void menu_Looks(uint8_t *colors) {
    shapes_RoundRectangleFill(colors[1], 8, 304, 192, 8, 39);
    shapes_RoundRectangleFill(colors[0], 8, 140, 56, 15, 46);
    shapes_RoundRectangleFill(colors[0], 8, 140, 92, 15, 109);
    shapes_RoundRectangleFill(colors[0], 8, 140, 155, 165, 46);
    const uint8_t defaultThemes[28] = {237, 246, 236, 74, 148, 0, 128, 137, 96, 226, 228, 162, 3, 100, 2, 28, 125, 58, 210, 243, 208, 81, 114, 48, 222, 0, 181, 255};
    uint8_t color = 0;
    for (int y = 49; y < 78; y += 28) {
        for (int x = 18; x < 132; x += 28, color += 3) {
            shapes_RoundRectangleFill(defaultThemes[color], 6, 22, 22, x, y);
        }
    }
    gfx_SetColor(148);
    gfx_Line_NoClip(132, 79, 149, 96);
    gfx_FloodFill(133, 78, 0);
    if (color >= 28) {
        shapes_RoundRectangleFill(255 - colors[0], 8, 134, 86, 18, 112);
        shapes_RoundRectangleFill(255 - colors[1], 8, 61, 78, 22, 116);
        shapes_RoundRectangleFill(255 - colors[2], 8, 61, 39, 87, 116);
    } else {
        shapes_RoundRectangleFill(defaultThemes[color + 1], 8, 134, 86, 18, 112);
        shapes_RoundRectangleFill(defaultThemes[color], 8, 61, 78, 22, 116);
        shapes_RoundRectangleFill(defaultThemes[color + 2], 8, 61, 39, 87, 116);
    }
    gfx_TransparentSprite_NoClip(lArrow, 15, 208);
    gfx_SwapDraw();
    while (kb_AnyKey());
    while (!kb_IsDown(kb_KeyYequ) && !kb_IsDown(kb_KeyClear)) {
        kb_Scan();
    }
}

void menu_Info(uint8_t color) {
    shapes_RoundRectangleFill(color, 15, 220, 192, 50, 38);
    gfx_SwapDraw();
    while (kb_AnyKey());
    while (!kb_IsDown(kb_KeyWindow) && !kb_IsDown(kb_KeyZoom) && !kb_IsDown(kb_KeyTrace) && !kb_IsDown(kb_KeyClear)) {
        kb_Scan();
    }
}

void menu_Settings(uint8_t color) {
    shapes_RoundRectangleFill(color, 15, 304, 192, 8, 39);
    gfx_TransparentSprite_NoClip(rArrow, 290, 208);
    gfx_SwapDraw();
    while (kb_AnyKey());
    while (!kb_IsDown(kb_KeyGraph) && !kb_IsDown(kb_KeyClear)) {
        kb_Scan();
    }
}
