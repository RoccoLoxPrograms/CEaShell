#include "menu.h"
#include "shapes.h"
#include "ui.h"

#include <graphx.h>
#include <keypadc.h>

void menu_Looks(uint8_t color) {
    shapes_RoundRectangleFill(color, 15, 304, 192, 8, 39);
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
    gfx_SwapDraw();
    while (kb_AnyKey());
    while (!kb_IsDown(kb_KeyGraph) && !kb_IsDown(kb_KeyClear)) {
        kb_Scan();
    }
}
