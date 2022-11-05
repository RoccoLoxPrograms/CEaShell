/**
 * --------------------------------------
 * 
 * CEaShell Source Code - main.c
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022
 * License: GPL-3.0
 * Last Build: November 5, 2022
 * Version: 0.76
 * 
 * --------------------------------------
**/

#include "ui.h"
#include "shapes.h"
#include "menu.h"
#include "utility.h"
#include "asm/invert.h"
#include "asm/sortVat.h"
#include "asm/fileOps.h"
#include "gfx/gfx.h"
#include "asm/hooks.h"
#include "asm/lowercase.h"
#include "asm/runProgram.h"

#include <graphx.h>
#include <keypadc.h>
#include <fileioc.h>
#include <string.h>
#include <sys/timers.h>
#include <sys/power.h>
#include <ti/getcsc.h>
#include <ti/screen.h>

#define ONE_SECOND      32768
#define ONE_MINUTE      (ONE_SECOND * 60)

gfx_UninitedSprite(buffer1, 152, 193);  // These preserve the background to make redrawing faster
gfx_UninitedSprite(buffer2, 152, 193);

int main(void) {
    removeStopHook();
    installMenuHook();
    removeExecuteHookInstalled();
    while (kb_AnyKey());

    // Default settings
    uint8_t colors[4] = {246, 237, 236, 0};    // If the appvar contains no theme it defaults to these settings
    uint8_t transitionSpeed = 2;    // 1 is slow, 2 is normal, 3 is fast, and 0 has no transitions
    bool is24Hour = true;
    bool appvars = false;   // Whether the appvars are being displayed
    bool displayCEaShell = false;   // Whether we display CEaShell
    uint8_t getCSCHook = BOTH;
    bool editArchivedProg = true;
    bool editLockedProg = true;
    bool showHiddenProg = true;
    bool showFileCount = false;
    bool hideBusyIndicator = false;
    bool lowercase = false;
    uint8_t apdTimer = 3;
    unsigned int fileSelected = 0;
    unsigned int fileStartLoc = 0;

    bool fullRedraw = false;

    gfx_Begin();
    ti_SetGCBehavior(&gfx_End, &reloadApp);
    if (colors[3]) {
        invertPalette();
    }
    gfx_SetTransparentColor(240);
    gfx_SetTextFGColor(255 * !(colors[1] > 131 && colors[1] % 8 > 3));
    gfx_SetTextBGColor(240);
    gfx_SetTextTransparentColor(240);

    gfx_SetDrawBuffer();
    // Restore preferences from appvar, if it exists
    uint8_t slot = ti_Open("CEaShell", "r");
    if (slot) { // If the appvar doesn't exist now, we'll just write the defaults into it later
        uint8_t ceaShell[15];
        ti_Read(&ceaShell, 15, 1, slot);
        colors[0] = ceaShell[0];
        colors[1] = ceaShell[1];
        colors[2] = ceaShell[2];
        colors[3] = ceaShell[3];
        transitionSpeed = ceaShell[4];
        is24Hour = ceaShell[5];
        displayCEaShell = ceaShell[6];
        getCSCHook = ceaShell[7];
        editArchivedProg = ceaShell[8];
        editLockedProg = ceaShell[9];
        showHiddenProg = ceaShell[10];
        showFileCount = ceaShell[11];
        hideBusyIndicator = ceaShell[12];
        lowercase = ceaShell[13];
        apdTimer = ceaShell[14];
        ti_Seek(15, SEEK_SET, slot);
        unsigned int scrollLoc[2];
        ti_Read(&scrollLoc, 6, 1, slot);
        fileSelected = scrollLoc[0];
        fileStartLoc = scrollLoc[1];
    } else {
        ui_NewUser();
    }

    if (lowercase && !checkLowercase()) {   // Restore lowercase preferences
        toggleLowercase(true);
    } else if (checkLowercase() && !lowercase) {
        lowercase = true;
    }

    // Restore hooks
    if (getCSCHook) {
        installGetCSCHook(getCSCHook);
    }

    if (editArchivedProg) {
        installMenuHook();
        installHomescreenHook();
    } else if (!editArchivedProg) {
        if (checkMenuHookInstalled()) {
            removeMenuHook();
        }
        if (checkHomescreenHookInstalled()) {
            removeHomescreenHook();
        }
    }

    buffer1->height = 193;
    buffer1->width = 152;
    buffer2->height = 193;
    buffer2->width = 152;

    unsigned int fileNumbers[2] = {0, 0};
    util_FilesInit(fileNumbers, displayCEaShell, showHiddenProg); // Get number of programs and appvars

    bool infoOps[2] = {false, false}; // This will keep track of whether a program has been deleted or hidden

    uint8_t batteryStatus = boot_GetBatteryStatus();
    
    bool keyPressed = false;    // A very clever timer thingy by RoccoLox Programs
    timer_Enable(1, TIMER_32K, TIMER_NOINT, TIMER_UP);

    gfx_FillScreen(colors[0]);
    ui_StatusBar(colors[1], is24Hour, batteryStatus, "", fileNumbers[appvars], showFileCount);  // Displays bar with battery and clock
    ui_BottomBar(colors[1]);
    ui_DrawAllFiles(colors, fileSelected, fileNumbers[appvars], fileStartLoc, appvars, displayCEaShell, showHiddenProg);    // This is always called after ui_StatusBar and ui_BottomBar as it will draw the program name onto the status bar
    gfx_BlitBuffer();

    timer_Set(1, 0);
    while(!kb_IsDown(kb_KeyClear)) {    // Key detection loop
        if ((timer_Get(1) >= ONE_MINUTE * apdTimer) && apdTimer) {
            gfx_End();
            triggerAPD();
            timer_Set(1, 0);
        }
        kb_Scan();
        if (!kb_AnyKey() && keyPressed) {
            keyPressed = false;
            timer_Set(1, 0);
        }
        if (kb_AnyKey() && !keyPressed) {
            timer_Set(1, 0);
        }
        if ((kb_Data[7] || kb_Data[6] || kb_Data[2] || kb_Data[1]) && (!keyPressed || timer_Get(1) > 1000)) { // File selecting (Probably very badly optimized)
            if (kb_IsDown(kb_KeyRight) && fileSelected + 1 < fileNumbers[appvars]) {
                if (fileSelected + 2 < fileNumbers[appvars]) {
                    fileSelected += 2;
                } else {
                    fileSelected += 1;
                }
                if (fileSelected - fileStartLoc > 7 && fileStartLoc + 1 < (fileNumbers[appvars] + fileNumbers[appvars] % 2) - 7) {
                    fileStartLoc += 2;
                }
            } else if (kb_IsDown(kb_KeyLeft) && fileSelected != 0) {
                if (fileSelected - fileStartLoc < 2 && fileStartLoc) {
                    fileStartLoc -= 2;
                }
                if (fileSelected != 1) {
                    fileSelected -= 2;
                } else if (fileSelected == 1) {
                    fileSelected -= 1;
                }
            }
            if (kb_IsDown(kb_KeyDown)) {
                if (fileSelected + 1 != fileNumbers[appvars]) {
                    fileSelected += !(fileSelected % 2);
                } else if (fileSelected) {
                    fileSelected -= !(fileSelected % 2) && (fileSelected - 1 >= 0);
                }
            } else if (kb_IsDown(kb_KeyUp)) {
                fileSelected -= fileSelected % 2;
            }
            if (kb_IsDown(kb_KeyDel) && fileSelected) {
                unsigned int filesSearched = 0;
                uint8_t osFileType;
                char *delFileName;
                void *vatPtr = NULL;
                while ((delFileName = ti_DetectAny(&vatPtr, NULL, &osFileType))) { // Suspiciously similar to the example in the docs :P
                    if (*delFileName == '!' || *delFileName == '#') {
                        continue;
                    }
                    if (!displayCEaShell && !strcmp(delFileName, "CEASHELL")) {
                        continue;
                    }
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
                    ui_DrawAllFiles(colors, fileSelected, fileNumbers[appvars], fileStartLoc, appvars, displayCEaShell, showHiddenProg);
                    gfx_BlitRectangle(gfx_buffer, 12, 28, 296, 10);
                }
                while (kb_AnyKey());
            } else if (kb_IsDown(kb_KeyYequ)) {    // Looks customization menu
                ui_StatusBar(colors[1], is24Hour, batteryStatus, "Customize", fileNumbers[appvars], showFileCount);
                gfx_BlitBuffer();
                if (transitionSpeed) {  // If the user turns transitions off, this won't call at all
                    for (int8_t frame = 3; frame < 16 / transitionSpeed; frame++) {
                        shapes_RoundRectangleFill(colors[1], 15, frame * (19 * transitionSpeed), frame * (12 * transitionSpeed), 8, 231 - frame * (12 * transitionSpeed));
                        gfx_SwapDraw();
                    }
                }
                menu_Looks(colors, &fileSelected, fileNumbers[appvars], fileStartLoc, &is24Hour, &transitionSpeed, appvars, &displayCEaShell, showHiddenProg, showFileCount, apdTimer); // This function will store changed colors into the colors array
                timer_Set(1, 0);
                util_FilesInit(fileNumbers, displayCEaShell, showHiddenProg);
                gfx_FillScreen(colors[0]);
                ui_DrawAllFiles(colors, fileSelected, fileNumbers[appvars], fileStartLoc, appvars, displayCEaShell, showHiddenProg);
                ui_BottomBar(colors[1]);
                ui_StatusBar(colors[1], is24Hour, batteryStatus, "Customize", fileNumbers[appvars], showFileCount);
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
                    util_WritePrefs(colors, transitionSpeed, is24Hour, displayCEaShell, getCSCHook, editArchivedProg, editLockedProg, showHiddenProg, showFileCount, hideBusyIndicator, lowercase, apdTimer, fileSelected, fileStartLoc);   // Stores our data to the appvar before exiting
                }
                fullRedraw = true;
                gfx_BlitBuffer();
            } else if ((kb_IsDown(kb_KeyWindow) || kb_IsDown(kb_KeyZoom) || kb_IsDown(kb_KeyTrace) || kb_IsDown(kb_KeyAlpha)) && fileSelected != 0) {   // Info menu
                ui_StatusBar(colors[1], is24Hour, batteryStatus, "File Info", fileNumbers[appvars], showFileCount);
                gfx_BlitBuffer();
                if (transitionSpeed) {
                    for (int8_t frame = 2; frame < 12 / transitionSpeed; frame++) {
                        shapes_RoundRectangleFill(colors[1], 15, 220, frame * (16 * transitionSpeed), 50, 230 - frame * (16 * transitionSpeed));
                        gfx_SwapDraw();
                    }
                }
                util_WritePrefs(colors, transitionSpeed, is24Hour, displayCEaShell, getCSCHook, editArchivedProg, editLockedProg, showHiddenProg, showFileCount, hideBusyIndicator, lowercase, apdTimer, fileSelected, fileStartLoc);
                menu_Info(colors, infoOps, fileSelected - 1, fileStartLoc, fileNumbers, appvars, displayCEaShell, editLockedProg, showHiddenProg, apdTimer); // This will store some file changes to the infoOps (Info Operations) array
                timer_Set(1, 0);
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
                gfx_FillScreen(colors[0]);
                ui_StatusBar(colors[1], is24Hour, batteryStatus, "", fileNumbers[appvars], showFileCount);
                ui_DrawAllFiles(colors, fileSelected, fileNumbers[appvars], fileStartLoc, appvars, displayCEaShell, showHiddenProg);
                ui_BottomBar(colors[1]);
                ui_StatusBar(colors[1], is24Hour, batteryStatus, "File Info", fileNumbers[appvars], showFileCount);
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
                ui_StatusBar(colors[1], is24Hour, batteryStatus, "Settings", fileNumbers[appvars], showFileCount);
                gfx_BlitBuffer();
                if (transitionSpeed) {
                    for (int8_t frame = 3; frame < 16 / transitionSpeed; frame++) {
                        shapes_RoundRectangleFill(colors[1], 15, frame * (19 * transitionSpeed), frame * (12 * transitionSpeed), 312 - frame * (19 * transitionSpeed), 231 - frame * (12 * transitionSpeed));
                        gfx_SwapDraw();
                    }
                }
                menu_Settings(colors, &getCSCHook, &editArchivedProg, &editLockedProg, &showHiddenProg, &showFileCount, &hideBusyIndicator, &lowercase, &apdTimer);
                timer_Set(1, 0);
                util_FilesInit(fileNumbers, displayCEaShell, showHiddenProg);
                if (fileSelected >= fileNumbers[appvars]) {
                    fileSelected = fileNumbers[appvars] - 1;
                    if (fileSelected < fileStartLoc) {
                        fileStartLoc = (fileSelected - (fileSelected % 2)); 
                    }
                }
                gfx_FillScreen(colors[0]);
                ui_DrawAllFiles(colors, fileSelected, fileNumbers[appvars], fileStartLoc, appvars, displayCEaShell, showHiddenProg);
                ui_BottomBar(colors[1]);
                ui_StatusBar(colors[1], is24Hour, batteryStatus, "Settings", fileNumbers[appvars], showFileCount);
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
                if (kb_IsDown(kb_KeyClear)) {
                    continue;
                } else {    // Same as Customize menu
                    util_WritePrefs(colors, transitionSpeed, is24Hour, displayCEaShell, getCSCHook, editArchivedProg, editLockedProg, showHiddenProg, showFileCount, hideBusyIndicator, lowercase, apdTimer, fileSelected, fileStartLoc);
                }
                fullRedraw = true;
                gfx_BlitBuffer();
            } else if (kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyEnter)) {
                if (fileSelected == 0) {
                    appvars = !appvars;
                    fullRedraw = true;  // By updating the battery we also make a short delay so the menu won't switch back
                } else {
                    util_WritePrefs(colors, transitionSpeed, is24Hour, displayCEaShell, getCSCHook, editArchivedProg, editLockedProg, showHiddenProg, showFileCount, hideBusyIndicator, lowercase, apdTimer, fileSelected, fileStartLoc);
                    util_RunPrgm(fileSelected, displayCEaShell, editLockedProg);
                }
            } else if (kb_IsDown(kb_KeyMode) && fileSelected) {
                fullRedraw = true;
                char name[9] = "\0";
                uint8_t copyMenu = ui_CopyNewMenu(colors, name);
                if (copyMenu == 2) {
                    while (kb_AnyKey());
                }
                name[8] = '\0';
                if (copyMenu == 1) {
                    if (!util_CheckNameExists(name, appvars)) {
                        uint8_t newProg = ti_OpenVar(name, "w", OS_TYPE_PRGM);
                        ti_Close(newProg);
                    }
                    gfx_End();
                    util_WritePrefs(colors, transitionSpeed, is24Hour, displayCEaShell, getCSCHook, editArchivedProg, editLockedProg, showHiddenProg, showFileCount, hideBusyIndicator, lowercase, apdTimer, fileSelected, fileStartLoc);   // Stores our data to the appvar before exiting
                    editBasicProg(name, OS_TYPE_PRGM);
                } else if (!copyMenu) {
                    if (util_CheckNameExists(name, appvars)) {
                        break;
                    }
                    uint8_t fileType; // Different from C, ICE, ASM, etc. This is stuff like OS_TYPE_APPVAR and OS_TYPE_PRGM
                    unsigned int filesSearched = 0;
                    char *fileName;
                    void *vatPtr = NULL;
                    while ((fileName = ti_DetectAny(&vatPtr, NULL, &fileType))) { // Suspiciously similar to the example in the docs :P
                        if (*fileName == '!' || *fileName == '#') {
                            continue;
                        }
                        if (!displayCEaShell && !strcmp(fileName, "CEASHELL")) {
                            continue;
                        }
                        if ((fileType == OS_TYPE_PRGM || fileType == OS_TYPE_PROT_PRGM) && !appvars) {
                            if (fileSelected - 1 == filesSearched) {
                                break;
                            }
                            filesSearched++;
                        } else if ((fileType == OS_TYPE_APPVAR) && appvars) {
                            if (fileSelected - 1 == filesSearched) {
                                break;
                            }
                            filesSearched++;
                        }
                    }
                    copyProgram(fileName, name, fileType);
                    util_FilesInit(fileNumbers, displayCEaShell, showHiddenProg); // Get number of programs and appvars
                }
            }
            if (kb_IsDown(kb_KeyClear)) {
                continue;
            }
            if (fullRedraw) {
                gfx_FillScreen(colors[0]);
                batteryStatus = boot_GetBatteryStatus();
                fullRedraw = false;
            } else {
                gfx_SetColor(colors[0]);
                gfx_FillRectangle_NoClip(8, 28, 304, 203);
            }
            ui_StatusBar(colors[1], is24Hour, batteryStatus, "", fileNumbers[appvars], showFileCount);
            ui_BottomBar(colors[1]);
            ui_DrawAllFiles(colors, fileSelected, fileNumbers[appvars], fileStartLoc, appvars, displayCEaShell, showHiddenProg);
            gfx_BlitBuffer();
            if (!keyPressed) {
                while (timer_Get(1) < 9000 && (kb_Data[7] || kb_Data[6] || kb_Data[2] || kb_Data[1])) {
                    kb_Scan();
                }
            }
            keyPressed = true;
            timer_Set(1, 0);
            continue;
        } else if (kb_AnyKey()) {
            keyPressed = true;
            asm("ei");
            if (util_AlphaSearch(&fileSelected, &fileStartLoc, os_GetCSC(), fileNumbers[appvars], appvars, displayCEaShell)) {
                gfx_SetColor(colors[0]);
                gfx_FillRectangle_NoClip(8, 28, 304, 203);
                ui_StatusBar(colors[1], is24Hour, batteryStatus, "", fileNumbers[appvars], showFileCount);
                ui_BottomBar(colors[1]);
                ui_DrawAllFiles(colors, fileSelected, fileNumbers[appvars], fileStartLoc, appvars, displayCEaShell, showHiddenProg);
            }
            asm("di");
            kb_Scan();
        }
        gfx_SetColor(colors[1]);
        gfx_FillRectangle_NoClip(15, 12, 35, 7);
        ui_Clock(is24Hour);
        gfx_BlitBuffer();
    }

    util_WritePrefs(colors, transitionSpeed, is24Hour, displayCEaShell, getCSCHook, editArchivedProg, editLockedProg, showHiddenProg, showFileCount, hideBusyIndicator, lowercase, apdTimer, 0, 0);
    gfx_End();
    os_ClrHome();   // Clean screen
    return 0;
}
