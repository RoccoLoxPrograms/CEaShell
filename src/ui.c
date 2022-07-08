#include "ui.h"
#include "shapes.h"
#include "utility.h"
#include "gfx/gfx.h"

#include <graphx.h>
#include <string.h>
#include <sys/rtc.h>
#include <sys/power.h>

static void ui_Battery(uint8_t batteryStatus, bool isCharging) {
    gfx_TransparentSprite_NoClip(battery, 286, 10);

    gfx_SetColor((batteryStatus < 2) * 160);
    gfx_FillRectangle_NoClip(291 + (12 - batteryStatus * 3), 12, batteryStatus * 3, 6);

    if (isCharging) {
        gfx_TransparentSprite_NoClip(charging, 292, 12);  // Charging sprite instead of battery percentage
    }
}

static void ui_Clock(bool is24Hour) {    // Displays time in either 24-Hour or AM/PM
    uint8_t time[3];
    bool isAfterNoon = boot_IsAfterNoon();

    boot_GetTime(&time[0], &time[1], &time[2]);
    time[2] += 12 * (is24Hour && isAfterNoon);  // Add 12 to the hour if it's PM and 24-Hour Time, so that 1 PM becomes 13, etc.
    gfx_SetTextScale(1, 1);
    gfx_SetTextFGColor(0);
    gfx_SetTextXY(15, 12);
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

void ui_StatusBar(uint8_t color, bool is24Hour, char *fileName) {  // Draws a 308 pixel long bar with rounded edges at 6, 6
    gfx_SetColor(color);
    shapes_RoundRectangleFill(color, 8, 308, 18, 6, 6);
    
    ui_Clock(is24Hour);

    gfx_SetTextScale(2, 2);
    gfx_SetTextFGColor(0);
    int x = 160 - gfx_GetStringWidth(fileName) / 2;
    if (x == 160) { // If no filename was given
        gfx_PrintStringXY("FILENAME", 96, 8);
    } else {
        gfx_PrintStringXY(fileName, x, 8);
    }

    ui_Battery(boot_GetBatteryStatus(), boot_BatteryCharging());   // For some reason the battery charging check isn't working
}

void ui_BottomBar(uint8_t color, char *description) {
    shapes_RoundRectangleFill(color, 6, 34, 34, 8, 197);    // Background and sprite
    shapes_RoundRectangleFill(color, 15, 220, 32, 50, 198);
    shapes_RoundRectangleFill(color, 6, 34, 34, 278, 197);
    gfx_TransparentSprite_NoClip(paint, 14, 203);
    gfx_TransparentSprite_NoClip(info, 56, 204);
    gfx_TransparentSprite_NoClip(settings, 284, 203);
    
    gfx_SetTextScale(1, 1); // Description
    char lineOne[25] = "\0";
    char lineTwo[25] = "\0";
    if (strlen(description) > 24) { // Wraps text that is longer than 24, and cuts off anything longer than 48
        int8_t cut = util_SpaceSearch(description); // If there is a space it will end the line there
        int8_t descLen = strlen(description);
        strncpy(lineOne, description, cut);
        if (descLen - cut > 24) {
            strncpy(lineTwo, description + cut, 22);
        } else {
            strncpy(lineTwo, description + cut, 24);
        }
        gfx_PrintStringXY(lineOne, 82, 205);
        gfx_PrintStringXY(lineTwo, 82, 216);
        if (descLen - cut > 24) {
            gfx_PrintString("...");
        }
    } else {
        gfx_PrintStringXY(description, 82, 211);
    }
}
