#include "main.h"
#include "ui.h"
#include "shapes.h"
#include "menu.h"
#include "utility.h"
#include "asm/invert.h"
#include "asm/sortVat.h"
#include "asm/fileOps.h"

#include <graphx.h>
#include <keypadc.h>
#include <fileioc.h>
#include <sys/timers.h>
#include <sys/power.h>
#include <ti/getcsc.h>

gfx_UninitedSprite(buffer1, 152, 193);  // These preserve the background to make redrawing faster
gfx_UninitedSprite(buffer2, 152, 193);

int main(void) {
    shellMain(0, 0);
    return 0;
}

int shellMain(unsigned int fileSelected, unsigned int fileStartLoc) {
    uint8_t colors[4] = {246, 237, 236, 0};    // If the appvar contains no theme it defaults to these settings
    uint8_t transitionSpeed = 2;    // 1 is slow, 2 is normal, 3 is fast, and 0 has no transitions
    bool is24Hour = true;
    bool appvars = false;   // Whether the appvars are being displayed

    uint8_t redraw = 0; // 0 = Clock Redraw, 1 = Screen Redraw, 2 = Full Redraw w/ Battery Update

    uint8_t slot = ti_Open("CEaShell", "r");
    if (slot) { // If the appvar doesn't exist now, we'll just write the defaults into it later
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
    util_FilesInit(fileNumbers); // Get number of programs and appvars

    bool infoOps[2] = {false, false}; // This will keep track of whether a program has been deleted or hidden

    uint8_t batteryStatus = boot_GetBatteryStatus();
    
    bool keyPressed = false;    // A very clever timer thingy by RoccoLox Programs
    timer_Enable(1, TIMER_32K, TIMER_NOINT, TIMER_UP);

    ti_SetGCBehavior(&gfx_End, &gfx_Begin);
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
    ui_StatusBar(colors[1], is24Hour, batteryStatus, "");  // Displays bar with battery and clock
    ui_BottomBar(colors[1]);
    ui_DrawAllFiles(colors, fileSelected, fileNumbers[appvars], fileStartLoc, appvars);    // This is always called after ui_StatusBar and ui_BottomBar as it will draw the program name onto the status bar
    gfx_BlitBuffer();

    while(!kb_IsDown(kb_KeyClear)) {    // Key detection loop
        kb_Scan();
        if (!kb_AnyKey()) {
            keyPressed = false;
            timer_Set(1, 0);
        }
        if ((kb_Data[7] || kb_Data[6] || kb_Data[2] || kb_Data[1]) && (!keyPressed || timer_Get(1) > 3000)) { // File selecting (Probably very badly optimized)
            if (kb_IsDown(kb_KeyRight) && fileSelected + 1 < fileNumbers[appvars]) {
                if (fileSelected + 2 < fileNumbers[appvars]) {
                    fileSelected += 2;
                } else {
                    fileSelected += 1;
                }
                if (fileSelected - fileStartLoc > 7 && fileStartLoc + 1 < (fileNumbers[appvars] + fileNumbers[appvars] % 2) - 7) {
                    fileStartLoc += 2;
                }
                redraw = 1;
            } else if (kb_IsDown(kb_KeyLeft) && fileSelected != 0) {
                if (fileSelected - fileStartLoc < 2 && fileStartLoc) {
                    fileStartLoc -= 2;
                }
                if (fileSelected != 1) {
                    fileSelected -= 2;
                } else if (fileSelected == 1) {
                    fileSelected -= 1;
                }
                redraw = 1;
            }
            if (kb_IsDown(kb_KeyDown)) {
                if (fileSelected + 1 != fileNumbers[appvars]) {
                    fileSelected += !(fileSelected % 2);
                } else if (fileSelected) {
                    fileSelected -= !(fileSelected % 2) && (fileSelected - 1 >= 0);
                }
                redraw = 1;
            } else if (kb_IsDown(kb_KeyUp)) {
                fileSelected -= fileSelected % 2;
                redraw = 1;
            }
            if (kb_IsDown(kb_KeyDel) && fileSelected) {
                uint8_t filesSearched = 0;
                uint8_t osFileType;
                char *delFileName;
                void *vatPtr = NULL;
                while ((delFileName = ti_DetectAny(&vatPtr, NULL, &osFileType))) { // Suspiciously similar to the example in the docs :P
                    if (appvars && osFileType == OS_TYPE_APPVAR) {
                        if (fileSelected - 1 == filesSearched) {
                            break;
                        }
                        filesSearched++;
                    } else if (!appvars && (osFileType == OS_TYPE_PRGM || osFileType == OS_TYPE_PROT_PRGM)) {
                        if (fileSelected - 1 == filesSearched) {
                            break;
                        }
                        filesSearched++;
                    }
                }
                if (ui_DeleteConf(colors, 56, 204)) {
                    ti_DeleteVar(delFileName, osFileType);
                    gfx_SetColor(colors[0]);
                    gfx_FillRectangle_NoClip(12, 28, 296, 164);
                    if (fileSelected >= fileNumbers[appvars] - 1) {
                        fileSelected--;
                    }
                    fileNumbers[appvars]--;
                    if (fileSelected + 1 >= fileNumbers[appvars] && fileStartLoc) {
                        if (fileStartLoc + 7 > fileNumbers[appvars]) {
                            fileStartLoc -= 2;
                        }
                    }
                    ui_DrawAllFiles(colors, fileSelected, fileNumbers[appvars], fileStartLoc, appvars);
                    gfx_BlitRectangle(gfx_buffer, 12, 28, 296, 10);
                }
                while (kb_AnyKey());
                redraw = 1;
            } else if (kb_IsDown(kb_KeyYequ)) {    // Looks customization menu
                ui_StatusBar(colors[1], is24Hour, batteryStatus, "Customize");
                gfx_BlitBuffer();
                if (transitionSpeed) {  // If the user turns transitions off, this won't call at all
                    for (int8_t frame = 3; frame < 16 / transitionSpeed; frame++) {
                        shapes_RoundRectangleFill(colors[1], 15, frame * (19 * transitionSpeed), frame * (12 * transitionSpeed), 8, 231 - frame * (12 * transitionSpeed));
                        gfx_SwapDraw();
                    }
                }
                menu_Looks(colors, fileSelected, fileNumbers[appvars], fileStartLoc, is24Hour, appvars); // This function will store changed colors into the colors array
                gfx_FillScreen(colors[0]);
                ui_DrawAllFiles(colors, fileSelected, fileNumbers[appvars], fileStartLoc, appvars);
                ui_BottomBar(colors[1]);
                ui_StatusBar(colors[1], is24Hour, batteryStatus, "Customize");
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
                if (kb_IsDown(kb_KeyClear)) {
                    continue;
                } else {    // We write the preferences before exiting, so this is fine
                    util_WritePrefs(colors, transitionSpeed, is24Hour);   // Stores our data to the appvar before exiting
                }
                redraw = 2;
                gfx_BlitBuffer();
            } else if ((kb_IsDown(kb_KeyWindow) || kb_IsDown(kb_KeyZoom) || kb_IsDown(kb_KeyTrace) || kb_IsDown(kb_KeyAlpha)) && fileSelected != 0) {   // Info menu
                ui_StatusBar(colors[1], is24Hour, batteryStatus, "File Info");
                gfx_BlitBuffer();
                if (transitionSpeed) {
                    for (int8_t frame = 2; frame < 12 / transitionSpeed; frame++) {
                        shapes_RoundRectangleFill(colors[1], 15, 220, frame * (16 * transitionSpeed), 50, 230 - frame * (16 * transitionSpeed));
                        gfx_SwapDraw();
                    }
                }
                menu_Info(colors, infoOps, fileSelected - 1, fileStartLoc, fileNumbers, appvars); // This will store some file changes to the infoOps (Info Operations) array
                if (infoOps[0]) {   // Takes care of deletions
                    fileNumbers[appvars]--;
                    fileSelected--;
                    if (fileSelected + 1 >= fileNumbers[appvars] && fileStartLoc) {
                        if (fileStartLoc + 7 > fileNumbers[appvars]) {
                            fileStartLoc -= 2;
                        }
                    }
                    infoOps[0] = false;
                    while (kb_AnyKey());
                }
                redraw = 1;

                gfx_FillScreen(colors[0]);
                ui_StatusBar(colors[1], is24Hour, batteryStatus, "");
                ui_DrawAllFiles(colors, fileSelected, fileNumbers[appvars], fileStartLoc, appvars);
                ui_BottomBar(colors[1]);
                ui_StatusBar(colors[1], is24Hour, batteryStatus, "File Info");
                if (infoOps[1]) {
                    gfx_BlitScreen();
                    infoOps[1] = false;
                }
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
                gfx_BlitBuffer();
            } else if (kb_IsDown(kb_KeyGraph)) {   // Settings menu
                ui_StatusBar(colors[1], is24Hour, batteryStatus, "Settings");
                gfx_BlitBuffer();
                if (transitionSpeed) {
                    for (int8_t frame = 3; frame < 16 / transitionSpeed; frame++) {
                        shapes_RoundRectangleFill(colors[1], 15, frame * (19 * transitionSpeed), frame * (12 * transitionSpeed), 312 - frame * (19 * transitionSpeed), 231 - frame * (12 * transitionSpeed));
                        gfx_SwapDraw();
                    }
                }
                menu_Settings(colors[1]);   // Nothing here yet
                gfx_FillScreen(colors[0]);
                ui_DrawAllFiles(colors, fileSelected, fileNumbers[appvars], fileStartLoc, appvars);
                ui_BottomBar(colors[1]);
                ui_StatusBar(colors[1], is24Hour, batteryStatus, "Settings");
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
            } else if (kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyEnter)) {
                if (fileSelected == 0) {
                    appvars = !appvars;
                    redraw = 2; // By updating the battery we also make a short delay so the menu won't switch back
                } else {
                    util_RunPrgm(fileSelected, fileStartLoc);
                }
            }
            if (!keyPressed) {
                while (timer_Get(1) < 9000 && (kb_Data[7] || kb_Data[1])) {
                    kb_Scan();
                }
            }
            keyPressed = true;
            timer_Set(1,0);
        } else if (kb_AnyKey()) {
            os_GetCSC();    // Running this twice because interrupts
            if (util_AlphaSearch(&fileSelected, &fileStartLoc, os_GetCSC(), fileNumbers[appvars], appvars)) {
                redraw = 1;
            }
        }
        if (kb_IsDown(kb_KeyClear)) {
            continue;
        }
        if (redraw) {
            if (redraw == 2) {
                gfx_FillScreen(colors[0]);
                batteryStatus = boot_GetBatteryStatus();
            } else {
                gfx_SetColor(colors[0]);
                gfx_FillRectangle_NoClip(8, 28, 304, 203);
            }
            ui_StatusBar(colors[1], is24Hour, batteryStatus, "");
            ui_BottomBar(colors[1]);
            ui_DrawAllFiles(colors, fileSelected, fileNumbers[appvars], fileStartLoc, appvars);
            redraw = false;
        } else {
            gfx_SetColor(colors[1]);
            gfx_FillRectangle_NoClip(15, 12, 35, 7);
            ui_Clock(is24Hour);
        }
        gfx_BlitBuffer();
    }

    util_WritePrefs(colors, transitionSpeed, is24Hour);
    gfx_End();
    return 0;
}
