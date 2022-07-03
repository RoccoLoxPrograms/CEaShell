#include "ui.h"
#include "gfx/gfx.h"

#include <graphx.h>
#include <sys/rtc.h>

void ui_StatusBarBG(uint8_t color, char fileName[9]) {  // Draws a 308 pixel long bar with rounded edges at 6, 6
    gfx_SetColor(color);
    gfx_FillCircle_NoClip(15, 14, 8); // I had to draw two circles because you can't have a circle 18 pixels tall
    gfx_FillCircle_NoClip(15, 15, 8);
    gfx_FillRectangle_NoClip(15, 6, 290, 18);
    gfx_FillCircle_NoClip(305, 14, 8);
    gfx_FillCircle_NoClip(305, 15, 8);

    gfx_SetTextScale(2, 2);
    gfx_SetTextFGColor(0);
    int x = 160 - gfx_GetStringWidth(fileName) / 2;
    if (x == 160) { // If no filename was given
        gfx_PrintStringXY("FILENAME", 96, 8);
    } else {
        gfx_PrintStringXY(fileName, x, 8);
    }
}

void ui_Battery(uint8_t batteryStatus, bool isCharging, int x, uint8_t y) {
    gfx_TransparentSprite_NoClip(battery, x, y);

    if (isCharging) {
        gfx_Sprite_NoClip(charging, x + 6, y + 2);  // Charging sprite instead of battery percentage
    } else if (batteryStatus > 1) {   // There has got to be a better way to do this
        gfx_SetColor(0);
        if (batteryStatus == 4) {   // Various levels of charge
            gfx_FillRectangle_NoClip(x + 5, y + 2, 12, 6);
        } else if (batteryStatus == 3) {
            gfx_FillRectangle_NoClip(x + 8, y + 2, 9, 6);
        } else {
            gfx_FillRectangle_NoClip(x + 11, y + 2, 6, 6);
        }
    } else {    // If it's very low, then the battery shows up red instead of black
        gfx_SetColor(160);
        gfx_FillRectangle_NoClip(x + 14, y + 2, 3, 6);
    }
}

void ui_Clock(bool is24Hour, int x, uint8_t y) {    // Displays time in either 24-Hour or AM/PM
    uint8_t time[3];
    bool isAfterNoon = boot_IsAfterNoon();

    boot_GetTime(&time[0], &time[1], &time[2]);
    if (is24Hour && isAfterNoon) {  // Add 12 to the hour if it's PM and 24-Hour Time, so that 1 PM becomes 13, etc.
        time[2] += 12;
    }
    gfx_SetTextScale(1, 1);
    gfx_SetTextFGColor(0);
    gfx_SetTextXY(x, y);
    gfx_PrintUInt(time[2], 1);
    gfx_PrintString(":");
    gfx_PrintUInt(time[1], 2);
    if (!is24Hour) {    // This draws the AM/PM if 24-Hour Time is not enabled
        if (isAfterNoon) {
            gfx_PrintString(" PM");
        } else {
            gfx_PrintString(" AM");
        }
    }
}
