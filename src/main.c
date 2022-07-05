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
    ui_BottomBar(colors[1], "Description");

    while(!kb_AnyKey()) {
        kb_Scan();
    }

    gfx_End();
    return 0;
}
