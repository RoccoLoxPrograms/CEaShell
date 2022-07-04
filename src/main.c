#include "ui.h"
#include "shapes.h"

#include <graphx.h>
#include <keypadc.h>

int main(void) {
    uint8_t colors[3] = {246, 237, 236};    // Will load colors from Appvar later, basically it's the background and two extra colors per theme

    gfx_Begin();
    gfx_SetTransparentColor(240);

    gfx_FillScreen(colors[0]);
    ui_StatusBar(colors[1], true, "");  // Displays bar with program name, clock, and battery

    shapes_RoundRectangleFill(colors[1], 6, 34, 34, 8, 197);    // This stuff will soon become the bottom bar/options
    shapes_RoundRectangleFill(colors[1], 15, 220, 32, 50, 198);
    shapes_RoundRectangleFill(colors[1], 6, 34, 34, 278, 197);

    while(!kb_AnyKey()) {
        kb_Scan();
    }

    gfx_End();
    return 0;
}
