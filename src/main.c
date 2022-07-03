#include "ui.h"

#include <graphx.h>
#include <keypadc.h>
#include <sys/power.h>

int main(void) {
    uint8_t colors[3] = {246, 237, 236};    // Will load colors from Appvar later, basically it's the background and two extra colors per theme

    gfx_Begin();
    gfx_SetTransparentColor(240);

    gfx_FillScreen(colors[0]);
    ui_StatusBarBG(colors[1], "");  // Displays bar with program name
    ui_Battery(boot_GetBatteryStatus(), boot_BatteryCharging(), 286, 10);   // For some reason the battery charging check isn't working
    ui_Clock(true, 15, 12);

    while(!kb_AnyKey()) {
        kb_Scan();
    }

    gfx_End();
    return 0;
}
