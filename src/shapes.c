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
