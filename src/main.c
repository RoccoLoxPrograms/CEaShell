/**
 * --------------------------------------
 * 
 * CEaShell Source Code - main.c
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022
 * License: GPL-3.0
 * Last Build: December 3, 2022
 * Version: 0.79.3
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
#include "asm/apps.h"

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
    uint8_t directory = PROGRAMS_FOLDER;
    bool showAppvars = true;
    bool showApps = true;
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

    // Graphics setup
    gfx_Begin();
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
        uint8_t ceaShell[17];
        ti_Read(&ceaShell, 17, 1, slot);
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
        showApps = ceaShell[15];
        showAppvars = ceaShell[16];
        ti_Seek(17, SEEK_SET, slot);
        unsigned int scrollLoc[2];
        ti_Read(&scrollLoc, 6, 1, slot);
        fileSelected = scrollLoc[0];
        fileStartLoc = scrollLoc[1];
        ti_Seek(6, SEEK_CUR, slot);
        ti_Read(&directory, 1, 1, slot);
    } else {
        ui_NewUser();
    }

    // Restore lowercase preferences
    if (lowercase && !checkLowercase()) {
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

    // Sprites we use to save the screen
    buffer1->height = 193;
    buffer1->width = 152;
    buffer2->height = 193;
    buffer2->width = 152;

    unsigned int fileNumbers[3] = {0, 0, 0};
    util_FilesInit(fileNumbers, displayCEaShell, showHiddenProg, showApps, showAppvars); // Get number of programs and appvars

    bool infoOps[2] = {false, false}; // This will keep track of whether a program has been deleted or hidden

    uint8_t batteryStatus = boot_GetBatteryStatus();
    
    bool keyPressed = false;    // A very clever timer thingy by RoccoLox Programs
    timer_Enable(1, TIMER_32K, TIMER_NOINT, TIMER_UP);

    gfx_FillScreen(colors[0]);
    ui_StatusBar(colors[1], is24Hour, batteryStatus, "", fileNumbers[directory], showFileCount);  // Displays bar with battery and clock
    ui_BottomBar(colors[1]);
    ui_DrawAllFiles(colors, fileSelected, fileNumbers[directory], fileStartLoc, directory, displayCEaShell, showHiddenProg, showApps, showAppvars);    // This is always called after ui_StatusBar and ui_BottomBar as it will draw the program name onto the status bar
    gfx_BlitBuffer();

    timer_Set(1, 0);
    while(!kb_IsDown(kb_KeyClear)) {    // Key detection loop
        if ((timer_Get(1) >= ONE_MINUTE * apdTimer) && apdTimer) {  // Power off the calculator after specified time of inactivity
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

        // Handle keypresses
        if ((kb_Data[7] || kb_Data[6] || kb_Data[2] || kb_Data[1]) && (!keyPressed || timer_Get(1) > 1000)) { // File selecting (Probably very badly optimized)
            if (kb_IsDown(kb_KeyRight) && fileSelected + 1 < fileNumbers[directory]) {
                if (fileSelected + 2 < fileNumbers[directory]) {
                    fileSelected += 2;
                } else {
                    fileSelected += 1;
                }
                if (fileSelected - fileStartLoc > 7 && fileStartLoc + 1 < (fileNumbers[directory] + fileNumbers[directory] % 2) - 7) {
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
                if (fileSelected + 1 != fileNumbers[directory]) {
                    fileSelected += !(fileSelected % 2);
                } else if (fileSelected) {
                    fileSelected -= !(fileSelected % 2) && (fileSelected - 1 >= 0);
                }
            } else if (kb_IsDown(kb_KeyUp)) {
                fileSelected -= fileSelected % 2;
            }

            if (kb_IsDown(kb_KeyDel) && fileSelected >= 0 + ((directory == PROGRAMS_FOLDER) * (showApps + showAppvars)) + (directory != PROGRAMS_FOLDER)) {
                unsigned int filesSearched = 1; // account for folders
                uint8_t osFileType;
                char *delFileName;
                void *vatPtr = NULL;
                char appName[9] = "\0";
                unsigned int appPointer;    // Will we ever use this? 🤷‍♂️

                if (directory == PROGRAMS_FOLDER) {
                    filesSearched = (showApps + showAppvars);
                }

                if (directory == APPS_FOLDER) {
                    while (detectApp(appName, &appPointer)) {
                        if (!displayCEaShell && !strcmp(appName, "CEaShell")) { // Ignore CEaShell
                            continue;
                        }
                        if (fileSelected == filesSearched) {
                            break;
                        }
                        filesSearched++;
                    }
                } else {
                    while ((delFileName = ti_DetectAny(&vatPtr, NULL, &osFileType))) { // Suspiciously similar to the example in the docs :P
                        if (*delFileName == '!' || *delFileName == '#') {
                            continue;
                        }
                        if (directory == APPVAR_TYPE && osFileType == OS_TYPE_APPVAR) {
                            if (fileSelected == filesSearched) {
                                break;
                            }
                            filesSearched++;
                        } else if (directory == PROGRAMS_FOLDER && (osFileType == OS_TYPE_PRGM || osFileType == OS_TYPE_PROT_PRGM)) {
                            if (fileSelected == filesSearched) {
                                break;
                            }
                            filesSearched++;
                        }
                    }
                }

                if (ui_DeleteConf(colors, 56, 204)) {
                    if (directory == APPS_FOLDER) {
                        // DO NOT DELETE APPS UNTIL THE ISSUES ARE FIXED
                        // deleteApp(appName);
                    } else {
                        ti_DeleteVar(delFileName, osFileType);
                        // Move all this out of the else later when it's fixed
                        gfx_SetColor(colors[0]);
                        gfx_FillRectangle_NoClip(12, 28, 296, 164);
                        if (fileSelected >= fileNumbers[directory] - 1) {
                            fileSelected--;
                        }
                        fileNumbers[directory]--;
                        if (fileSelected + 1 >= fileNumbers[directory] && fileStartLoc) {
                            if (fileStartLoc + 7 > fileNumbers[directory]) {
                                fileStartLoc -= 2;
                            }
                        }
                        ui_DrawAllFiles(colors, fileSelected, fileNumbers[directory], fileStartLoc, directory, displayCEaShell, showHiddenProg, showApps, showAppvars);
                        gfx_BlitRectangle(gfx_buffer, 12, 28, 296, 10);
                    }
                }
                while (kb_AnyKey());
            } else if (kb_IsDown(kb_KeyYequ)) {    // Looks customization menu
                ui_StatusBar(colors[1], is24Hour, batteryStatus, "Customize", fileNumbers[directory], showFileCount);
                gfx_BlitBuffer();
                if (transitionSpeed) {  // If the user turns transitions off, this won't call at all
                    for (int8_t frame = 3; frame < 16 / transitionSpeed; frame++) {
                        shapes_RoundRectangleFill(colors[1], 15, frame * (19 * transitionSpeed), frame * (12 * transitionSpeed), 8, 231 - frame * (12 * transitionSpeed));
                        gfx_SwapDraw();
                    }
                }
                menu_Looks(colors, &fileSelected, fileNumbers[directory], fileStartLoc, &is24Hour, &transitionSpeed, directory, &displayCEaShell, showHiddenProg, showFileCount, apdTimer, &showApps, &showAppvars); // This function will store changed colors into the colors array
                timer_Set(1, 0);
                util_FilesInit(fileNumbers, displayCEaShell, showHiddenProg, showApps, showAppvars);
                gfx_FillScreen(colors[0]);
                ui_DrawAllFiles(colors, fileSelected, fileNumbers[directory], fileStartLoc, directory, displayCEaShell, showHiddenProg, showApps, showAppvars);
                ui_BottomBar(colors[1]);
                ui_StatusBar(colors[1], is24Hour, batteryStatus, "Customize", fileNumbers[directory], showFileCount);
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
                    util_WritePrefs(colors, transitionSpeed, is24Hour, displayCEaShell, getCSCHook, editArchivedProg, editLockedProg, showHiddenProg, showFileCount, hideBusyIndicator, lowercase, apdTimer, fileSelected, fileStartLoc, directory, showApps, showAppvars);   // Stores our data to the appvar before exiting
                }
                fullRedraw = true;
                gfx_BlitBuffer();
            } else if ((kb_IsDown(kb_KeyWindow) || kb_IsDown(kb_KeyZoom) || kb_IsDown(kb_KeyTrace) || kb_IsDown(kb_KeyAlpha)) && fileSelected >= 0 + ((directory == PROGRAMS_FOLDER) * (showApps + showAppvars)) + (directory != PROGRAMS_FOLDER)) {   // Info menu
                ui_StatusBar(colors[1], is24Hour, batteryStatus, "File Info", fileNumbers[directory], showFileCount);
                gfx_BlitBuffer();
                if (transitionSpeed) {
                    for (int8_t frame = 2; frame < 12 / transitionSpeed; frame++) {
                        shapes_RoundRectangleFill(colors[1], 15, 220, frame * (16 * transitionSpeed), 50, 230 - frame * (16 * transitionSpeed));
                        gfx_SwapDraw();
                    }
                }
                shapes_RoundRectangleFill(colors[1], 15, 220, 192, 50, 38);
                gfx_BlitBuffer();
                util_WritePrefs(colors, transitionSpeed, is24Hour, displayCEaShell, getCSCHook, editArchivedProg, editLockedProg, showHiddenProg, showFileCount, hideBusyIndicator, lowercase, apdTimer, fileSelected, fileStartLoc, directory, showApps, showAppvars);
                menu_Info(colors, infoOps, fileSelected - 1, fileStartLoc, fileNumbers, directory, displayCEaShell, editLockedProg, showHiddenProg, apdTimer, showApps, showAppvars); // This will store some file changes to the infoOps (Info Operations) array
                timer_Set(1, 0);
                if (infoOps[0]) {   // Takes care of deletions
                    fileNumbers[directory]--;
                    fileSelected--;
                    if (fileSelected + 1 >= fileNumbers[directory] && fileStartLoc) {
                        if (fileStartLoc + 7 > fileNumbers[directory]) {
                            fileStartLoc -= 2;
                        }
                    }
                    infoOps[0] = false;
                    while (kb_AnyKey());
                }
                gfx_FillScreen(colors[0]);
                ui_StatusBar(colors[1], is24Hour, batteryStatus, "", fileNumbers[directory], showFileCount);
                ui_DrawAllFiles(colors, fileSelected, fileNumbers[directory], fileStartLoc, directory, displayCEaShell, showHiddenProg, showApps, showAppvars);
                ui_BottomBar(colors[1]);
                ui_StatusBar(colors[1], is24Hour, batteryStatus, "File Info", fileNumbers[directory], showFileCount);
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
                ui_StatusBar(colors[1], is24Hour, batteryStatus, "Settings", fileNumbers[directory], showFileCount);
                gfx_BlitBuffer();
                if (transitionSpeed) {
                    for (int8_t frame = 3; frame < 16 / transitionSpeed; frame++) {
                        shapes_RoundRectangleFill(colors[1], 15, frame * (19 * transitionSpeed), frame * (12 * transitionSpeed), 312 - frame * (19 * transitionSpeed), 231 - frame * (12 * transitionSpeed));
                        gfx_SwapDraw();
                    }
                }
                menu_Settings(colors, &getCSCHook, &editArchivedProg, &editLockedProg, &showHiddenProg, &showFileCount, &hideBusyIndicator, &lowercase, &apdTimer);
                timer_Set(1, 0);
                util_FilesInit(fileNumbers, displayCEaShell, showHiddenProg, showApps, showAppvars);
                if (fileSelected >= fileNumbers[directory]) {
                    fileSelected = fileNumbers[directory] - 1;
                    if (fileSelected < fileStartLoc) {
                        fileStartLoc = (fileSelected - (fileSelected % 2)); 
                    }
                }
                gfx_FillScreen(colors[0]);
                ui_DrawAllFiles(colors, fileSelected, fileNumbers[directory], fileStartLoc, directory, displayCEaShell, showHiddenProg, showApps, showAppvars);
                ui_BottomBar(colors[1]);
                ui_StatusBar(colors[1], is24Hour, batteryStatus, "Settings", fileNumbers[directory], showFileCount);
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
                    util_WritePrefs(colors, transitionSpeed, is24Hour, displayCEaShell, getCSCHook, editArchivedProg, editLockedProg, showHiddenProg, showFileCount, hideBusyIndicator, lowercase, apdTimer, fileSelected, fileStartLoc, directory, showApps, showAppvars);
                }
                fullRedraw = true;
                gfx_BlitBuffer();
            } else if (kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyEnter)) {
                if (fileSelected == 0 && directory == PROGRAMS_FOLDER) {    // Toggle directories
                    if (showApps) {
                        directory = APPS_FOLDER;
                    } else if (showAppvars) {
                        directory = APPVARS_FOLDER;
                    } else {
                        util_WritePrefs(colors, transitionSpeed, is24Hour, displayCEaShell, getCSCHook, editArchivedProg, editLockedProg, showHiddenProg, showFileCount, hideBusyIndicator, lowercase, apdTimer, fileSelected, fileStartLoc, directory, showApps, showAppvars);
                        util_RunPrgm(fileSelected, editLockedProg, showApps, showAppvars);
                    }
                    fullRedraw = true;  // By updating the battery we also make a short delay so the menu won't switch back
                    fileStartLoc = 0;
                    fileSelected = 0;
                } else if (fileSelected == 1 && directory == PROGRAMS_FOLDER && showApps && showAppvars) {
                    directory = APPVARS_FOLDER;
                    fullRedraw = true;
                    fileStartLoc = 0;
                    fileSelected = 0;
                } else if (directory == PROGRAMS_FOLDER) {  // Run program
                    util_WritePrefs(colors, transitionSpeed, is24Hour, displayCEaShell, getCSCHook, editArchivedProg, editLockedProg, showHiddenProg, showFileCount, hideBusyIndicator, lowercase, apdTimer, fileSelected, fileStartLoc, directory, showApps, showAppvars);
                    util_RunPrgm(fileSelected, editLockedProg, showApps, showAppvars);
                } else if (fileSelected == 0) {
                    directory = PROGRAMS_FOLDER;
                    fullRedraw = true;
                    fileStartLoc = 0;
                    fileSelected = 0;
                } else if (directory == APPS_FOLDER) {  // Run app
                    util_WritePrefs(colors, transitionSpeed, is24Hour, displayCEaShell, getCSCHook, editArchivedProg, editLockedProg, showHiddenProg, showFileCount, hideBusyIndicator, lowercase, apdTimer, 0, 0, PROGRAMS_FOLDER, showApps, showAppvars);    // We won't return after an app
                    util_RunApp(fileSelected, displayCEaShell);
                }
            } else if (kb_IsDown(kb_KeyMode) && fileSelected >= 0 + ((directory == PROGRAMS_FOLDER) * (showApps + showAppvars)) + (directory != PROGRAMS_FOLDER)) {
                fullRedraw = true;
                char name[9] = "\0";
                uint8_t copyMenu = ui_CopyNewMenu(colors, name);
                if (copyMenu == 2) {
                    while (kb_AnyKey());
                }
                name[8] = '\0';
                if (copyMenu == 1) {    // Create new
                    if (directory == PROGRAMS_FOLDER) {
                        if (!util_CheckNameExists(name, directory)) {   // Check if the name exists before creating a new program with that name
                            uint8_t newProg = ti_OpenVar(name, "w", OS_TYPE_PRGM);
                            ti_Close(newProg);
                        }
                        gfx_End();
                        util_WritePrefs(colors, transitionSpeed, is24Hour, displayCEaShell, getCSCHook, editArchivedProg, editLockedProg, showHiddenProg, showFileCount, hideBusyIndicator, lowercase, apdTimer, fileSelected, fileStartLoc, directory, showApps, showAppvars);   // Stores our data to the appvar before exiting
                        editBasicProg(name, OS_TYPE_PRGM);
                    } else if (directory == APPVARS_FOLDER) {
                        if (!util_CheckNameExists(name, directory)) {
                            uint8_t newVar = ti_Open(name, "w");
                            const uint8_t celticHeader[5] = {CELTIC_HEADER};
                            ti_Write(celticHeader, 5, 1, newVar);
                            ti_Close(newVar);
                        }
                        gfx_End();
                        util_WritePrefs(colors, transitionSpeed, is24Hour, displayCEaShell, getCSCHook, editArchivedProg, editLockedProg, showHiddenProg, showFileCount, hideBusyIndicator, lowercase, apdTimer, fileSelected, fileStartLoc, directory, showApps, showAppvars);   // Stores our data to the appvar before exiting
                        editCelticAppvar(name);
                    }
                } else if (!copyMenu) { // Copy
                    if (util_CheckNameExists(name, directory)) {
                        break;
                    }
                    uint8_t fileType; // Different from C, ICE, ASM, etc. This is stuff like OS_TYPE_APPVAR and OS_TYPE_PRGM
                    unsigned int filesSearched = 1; // account for folders
                    char *fileName;
                    void *vatPtr = NULL;
                    if (directory == PROGRAMS_FOLDER) {
                        filesSearched = (showApps + showAppvars);
                    }
                    while ((fileName = ti_DetectAny(&vatPtr, NULL, &fileType))) { // Suspiciously similar to the example in the docs :P
                        if (*fileName == '!' || *fileName == '#') {
                            continue;
                        }
                        if ((fileType == OS_TYPE_PRGM || fileType == OS_TYPE_PROT_PRGM) && directory == PROGRAMS_FOLDER) {
                            if (fileSelected == filesSearched) {
                                break;
                            }
                            filesSearched++;
                        } else if ((fileType == OS_TYPE_APPVAR) && directory == APPVARS_FOLDER) {
                            if (fileSelected == filesSearched) {
                                break;
                            }
                            filesSearched++;
                        }
                    }
                    copyProgram(fileName, name, fileType);
                    util_FilesInit(fileNumbers, displayCEaShell, showHiddenProg, showApps, showAppvars); // Get number of programs and appvars
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
            ui_StatusBar(colors[1], is24Hour, batteryStatus, "", fileNumbers[directory], showFileCount);
            ui_BottomBar(colors[1]);
            ui_DrawAllFiles(colors, fileSelected, fileNumbers[directory], fileStartLoc, directory, displayCEaShell, showHiddenProg, showApps, showAppvars);
            gfx_BlitBuffer();
            if (!keyPressed) {
                while (timer_Get(1) < 9000 && (kb_Data[7] || kb_Data[6] || kb_Data[2] || kb_Data[1])) {
                    kb_Scan();
                }
            }
            keyPressed = true;
            timer_Set(1, 0);
            continue;
        }

        if (util_AlphaSearch(&fileSelected, &fileStartLoc, util_GetSingleKeyPress(), fileNumbers[directory], displayCEaShell, directory, showApps, showAppvars)) {
            gfx_SetColor(colors[0]);
            gfx_FillRectangle_NoClip(8, 28, 304, 203);
            ui_StatusBar(colors[1], is24Hour, batteryStatus, "", fileNumbers[directory], showFileCount);
            ui_BottomBar(colors[1]);
            ui_DrawAllFiles(colors, fileSelected, fileNumbers[directory], fileStartLoc, directory, displayCEaShell, showHiddenProg, showApps, showAppvars);
            while (kb_AnyKey());
            kb_Scan();
        }
        gfx_SetColor(colors[1]);
        gfx_FillRectangle_NoClip(15, 12, 35, 7);
        ui_Clock(is24Hour);
        gfx_BlitBuffer();
    }

    // Very long function to write all the preferences to the appvar
    util_WritePrefs(colors, transitionSpeed, is24Hour, displayCEaShell, getCSCHook, editArchivedProg,
    editLockedProg, showHiddenProg, showFileCount, hideBusyIndicator, lowercase, apdTimer, 0, 0, false,
    showApps, showAppvars);
    gfx_End();
    os_ClrHome();   // Clean screen
    exitDefrag();
    return 0;
}
