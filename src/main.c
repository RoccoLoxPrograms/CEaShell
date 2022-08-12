#include "ui.h"
#include "shapes.h"
#include "menu.h"
#include "utility.h"
#include "asm/invert.h"
#include "asm/sortVat.h"
#include "asm/getPrgmType.h"

#include <graphx.h>
#include <keypadc.h>
#include <fileioc.h>
#include <sys/timers.h>
#include <sys/power.h>

gfx_UninitedSprite(buffer1, 152, 193);  // These preserve the background to make redrawing faster
gfx_UninitedSprite(buffer2, 152, 193);

int main(void) {
    uint8_t colors[4] = {246, 237, 236, 0};    // If the appvar contains no theme it defaults to these settings
    uint8_t transitionSpeed = 2;    // 1 is slow, 2 is normal, 3 is fast, and 0 has no transitions
    bool is24Hour = true;

    uint8_t redraw = 0; // 0 = Clock Redraw, 1 = Screen Redraw, 2 = Full Redraw w/ Battery Update

    uint8_t slot = ti_Open("CEaShell", "r");
    if (slot) {
        uint8_t ceaShell[6];
        ti_Read(&ceaShell, 6, 1, slot);
        colors[0] = ceaShell[0];
        colors[1] = ceaShell[1];
        colors[2] = ceaShell[2];
        colors[3] = ceaShell[3];
        transitionSpeed = ceaShell[4];
        is24Hour = ceaShell[5];
    }
    buffer1->height = 193;
    buffer1->width = 152;
    buffer2->height = 193;
    buffer2->width = 152;

    uint8_t fileNumbers[2] = {0, 0};
    uint8_t *newNumbers = util_FilesInit(fileNumbers);
    NOPROGS = newNumbers[0];        // Stores the number of programs in fileNumbers[0]
    NOAPPVARS = newNumbers[1];      // Stores the number of appvars in fileNumbers[1]
    uint8_t fileStartLoc = 0;
    uint8_t fileSelected = 0;    // Which of the slots the cursor is selecting

    uint8_t batteryStatus = boot_GetBatteryStatus();
    
    bool keyPressed = false;    // A very clever timer thingy by RoccoLox Programs
    timer_Enable(1, TIMER_32K, TIMER_NOINT, TIMER_UP);

    gfx_Begin();
    if (colors[3]) {
        invertPalette();
    }
    gfx_SetTransparentColor(240);
    gfx_SetTextFGColor(255 * !(colors[1] > 131 && colors[1] % 8 > 3));
    gfx_SetTextBGColor(240);
    gfx_SetTextTransparentColor(240);

    gfx_SetDrawBuffer();
    gfx_FillScreen(colors[0]);
    ui_StatusBar(colors[1], is24Hour, batteryStatus, "");  // Displays bar with program name and clock
    ui_DrawAllFiles(colors, fileSelected, NOPROGS, fileStartLoc, false);
    ui_BottomBar(colors[1], "By TIny_Hacker + RoccoLox Programs");
    gfx_BlitBuffer();

    while(!kb_IsDown(kb_KeyClear)) {    // Looks menu
        kb_Scan();
        if (!kb_AnyKey()) {
            keyPressed = false;
            timer_Set(1, 0);
        }
        if ((kb_Data[7] || kb_Data[2] || kb_Data[1]) && (!keyPressed || timer_Get(1) > 3000)) {
            if (kb_IsDown(kb_KeyRight) && fileSelected + 1 < NOPROGS) {
                if (fileSelected + 2 < NOPROGS) {
                    fileSelected += 2;
                } else {
                    fileSelected += 1;
                }
                if (fileSelected - fileStartLoc > 7 && fileStartLoc + 1 < (NOPROGS + NOPROGS % 2) - 7) {
                    fileStartLoc += 2;
                }
                redraw = 1;
            } else if (kb_IsDown(kb_KeyLeft) && fileSelected != 0) {
                if (fileSelected - 2 >= 0) {
                    fileSelected -= 2;
                } else {
                    fileSelected -= 1;
                }
                if (fileSelected - fileStartLoc < 0 && fileStartLoc) {
                    fileStartLoc -= 2;
                }
                redraw = 1;
            }
            if (kb_IsDown(kb_KeyDown)) {
                if (fileSelected + 1 != NOPROGS) {
                    fileSelected += !(fileSelected % 2);
                } else if (fileSelected) {
                    fileSelected -= !(fileSelected % 2) && (fileSelected - 1 >= 0);
                }
                redraw = 1;
            } else if (kb_IsDown(kb_KeyUp)) {
                fileSelected -= fileSelected % 2;
                redraw = 1;
            }
            if (kb_IsDown(kb_KeyYequ)) {
                ui_StatusBar(colors[1], is24Hour, batteryStatus, "Customize");
                gfx_BlitBuffer();
                if (transitionSpeed) {
                    for (int8_t frame = 3; frame < 16 / transitionSpeed; frame++) {
                        shapes_RoundRectangleFill(colors[1], 15, frame * (19 * transitionSpeed), frame * (12 * transitionSpeed), 8, 231 - frame * (12 * transitionSpeed));
                        gfx_SwapDraw();
                    }
                }
                uint8_t *newColors = menu_Looks(colors, fileSelected, NOPROGS, fileStartLoc, is24Hour);
                for (uint8_t byte = 0; byte < 4; byte++) {
                    colors[byte] = newColors[byte];
                }
                gfx_FillScreen(colors[0]);
                ui_DrawAllFiles(colors, fileSelected, NOPROGS, fileStartLoc, false);
                ui_StatusBar(colors[1], is24Hour, batteryStatus, "Customize");
                ui_BottomBar(colors[1], "By TIny_Hacker + RoccoLox Programs");
                if (transitionSpeed) {
                    gfx_GetSprite_NoClip(buffer1, 8, 38);   // For redrawing the background
                    gfx_GetSprite_NoClip(buffer2, 160, 38);
                    for (uint8_t frame = 16 / transitionSpeed; frame > 2; frame--) {
                        gfx_Sprite_NoClip(buffer1, 8, 38);
                        gfx_Sprite_NoClip(buffer2, 160, 38);
                        shapes_RoundRectangleFill(colors[1], 15, frame * (19 * transitionSpeed), frame * (12 * transitionSpeed), 8, 231 - frame * (12 * transitionSpeed));
                        gfx_SwapDraw();
                    }
                    gfx_Sprite_NoClip(buffer1, 8, 38);
                    gfx_Sprite_NoClip(buffer2, 160, 38);
                }
                redraw = 2;
                gfx_BlitBuffer();
            }
            if (kb_IsDown(kb_KeyWindow) || kb_IsDown(kb_KeyZoom) || kb_IsDown(kb_KeyTrace) || kb_IsDown(kb_KeyAlpha)) {   // Info menu
                ui_StatusBar(colors[1], is24Hour, batteryStatus, "File Info");
                gfx_BlitBuffer();
                if (transitionSpeed) {
                    for (int8_t frame = 2; frame < 12 / transitionSpeed; frame++) {
                        shapes_RoundRectangleFill(colors[1], 15, 220, frame * (16 * transitionSpeed), 50, 230 - frame * (16 * transitionSpeed));
                        gfx_SwapDraw();
                    }
                }
                menu_Info(colors, fileSelected, false);
                gfx_FillScreen(colors[0]);
                ui_StatusBar(colors[1], is24Hour, batteryStatus, "");
                ui_DrawAllFiles(colors, fileSelected, NOPROGS, fileStartLoc, false);
                ui_StatusBar(colors[1], is24Hour, batteryStatus, "File Info");
                ui_BottomBar(colors[1], "By TIny_Hacker + RoccoLox Programs");
                if (transitionSpeed) {
                    gfx_GetSprite_NoClip(buffer1, 8, 38);   // For redrawing the background
                    gfx_GetSprite_NoClip(buffer2, 160, 38);
                    for (uint8_t frame = 12 / transitionSpeed; frame > 1; frame--) {
                        gfx_Sprite_NoClip(buffer1, 8, 38);
                        gfx_Sprite_NoClip(buffer2, 160, 38);
                        shapes_RoundRectangleFill(colors[1], 15, 220, frame * (16 * transitionSpeed), 50, 230 - frame * (16 * transitionSpeed));
                        gfx_SwapDraw();
                    }
                    gfx_Sprite_NoClip(buffer1, 8, 38);
                    gfx_Sprite_NoClip(buffer2, 160, 38);
                }
                redraw = 2;
                gfx_BlitBuffer();
            }
            if (kb_IsDown(kb_KeyGraph)) {   // Settings menu
                ui_StatusBar(colors[1], is24Hour, batteryStatus, "Settings");
                gfx_BlitBuffer();
                if (transitionSpeed) {
                    for (int8_t frame = 3; frame < 16 / transitionSpeed; frame++) {
                        shapes_RoundRectangleFill(colors[1], 15, frame * (19 * transitionSpeed), frame * (12 * transitionSpeed), 312 - frame * (19 * transitionSpeed), 231 - frame * (12 * transitionSpeed));
                        gfx_SwapDraw();
                    }
                }
                menu_Settings(colors[1]);
                gfx_FillScreen(colors[0]);
                ui_DrawAllFiles(colors, fileSelected, NOPROGS, fileStartLoc, false);
                ui_StatusBar(colors[1], is24Hour, batteryStatus, "Settings");
                ui_BottomBar(colors[1], "By TIny_Hacker + RoccoLox Programs");
                if (transitionSpeed) {
                    gfx_GetSprite_NoClip(buffer1, 8, 38);   // For redrawing the background
                    gfx_GetSprite_NoClip(buffer2, 160, 38);
                    for (uint8_t frame = 16 / transitionSpeed; frame > 2; frame--) {
                        gfx_Sprite_NoClip(buffer1, 8, 38);
                        gfx_Sprite_NoClip(buffer2, 160, 38);
                        shapes_RoundRectangleFill(colors[1], 15, frame * (19 * transitionSpeed), frame * (12 * transitionSpeed), 312 - frame * (19 * transitionSpeed), 231 - frame * (12 * transitionSpeed));
                        gfx_SwapDraw();
                    }
                    gfx_Sprite_NoClip(buffer1, 8, 38);
                    gfx_Sprite_NoClip(buffer2, 160, 38);
                }
                redraw = 2;
                gfx_BlitBuffer();
            }
            if (!keyPressed) {
                while (timer_Get(1) < 9000 && (kb_Data[7] || kb_Data[1])) {
                    kb_Scan();
                }
            }
            keyPressed = true;
            timer_Set(1,0);
        }
        if (redraw) {
            if (redraw == 2) {
                gfx_FillScreen(colors[0]);
                batteryStatus = boot_GetBatteryStatus();
                ui_StatusBar(colors[1], is24Hour, batteryStatus, "");
            } else {
                gfx_SetColor(colors[0]);
                gfx_FillRectangle_NoClip(12, 28, 296, 164);
                gfx_SetColor(colors[1]);
                gfx_FillRectangle_NoClip(15, 12, 35, 7);
                gfx_FillRectangle_NoClip(96, 8, 128, 16);
                ui_Clock(is24Hour);
            }
            ui_DrawAllFiles(colors, fileSelected, NOPROGS, fileStartLoc, false);
            ui_BottomBar(colors[1], "By TIny_Hacker + RoccoLox Programs");
            redraw = false;
        } else {
            gfx_SetColor(colors[1]);
            gfx_FillRectangle_NoClip(15, 12, 35, 7);
            ui_Clock(is24Hour);
        }
        gfx_BlitBuffer();
    }

    gfx_End();
    util_Exit(colors, transitionSpeed, is24Hour);
    return 0;
}
