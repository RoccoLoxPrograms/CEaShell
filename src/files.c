/**
 * --------------------------------------
 * 
 * CEaShell Source Code - files.c
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022 - 2026
 * License: GPL-3.0
 * 
 * --------------------------------------
**/

#include "defines.h"

#include "custom.h"
#include "files.h"
#include "info.h"
#include "menu.h"
#include "settings.h"
#include "shapes.h"
#include "ui.h"
#include "utility.h"

#include "asm/apps.h"
#include "asm/fileOps.h"
#include "asm/spi.h"
#include "asm/utils.h"

#include <string.h>

#include <sys/power.h>
#include <sys/util.h>

static void files_Redraw(uint8_t rows, uint8_t columns, unsigned int fileCount, struct preferences_t *shellPrefs, struct context_t *shellContext) {
    static struct file_t fileInfo;
    util_GetFileInfo(shellContext->fileSelected, &fileInfo, shellPrefs, shellContext);

    fileInfo.icon = NULL;

    gfx_FillScreen(shellPrefs->bgColor);

    if (!fileCount) {
        memcpy(fileInfo.name, &rodata_appName, 9);
        gfx_SetTextFGColor(shellPrefs->textColor);
        gfx_PrintStringXY("No Files", 130, 106);
    }

    ui_DrawStatusBar(shellPrefs, shellContext, fileInfo.name);
    ui_DrawFiles(shellPrefs, shellContext);
    ui_DrawBottomBar(shellPrefs, fileInfo.description);

    if (fileCount > rows * columns) {
        unsigned int dummyFiles = fileCount % rows; // Correctly draw scrollbar

        if (dummyFiles) {
            dummyFiles = rows - dummyFiles;
        }

        uint8_t scrollY = 191;

        switch (shellPrefs->uiScale) {
            case SCALE_SMALLEST:
                scrollY = 186;
                break;
            case SCALE_SMALL:
                scrollY = 181;
                break;
            case SCALE_MEDIUM:
                scrollY = 177;
                break;
        }

        ui_ScrollBar(shellPrefs, 20, scrollY, 280, fileCount + dummyFiles, shellContext->fileStartLoc, rows * columns, true);
    }

    gfx_SetColor(shellPrefs->fgColor);
}

void files_Main(struct preferences_t *shellPrefs, struct context_t *shellContext) {
    static const uint8_t leftBracket[8] = {0xF0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xF0, 0x00};
    static const uint8_t thetaChar[8] = {0x7C, 0xC6, 0xC6, 0xFE, 0xC6, 0xC6, 0x7C, 0x00};

    util_SetGFXChar('[', thetaChar, 8);

    bool updateBattery = false;

    uint8_t rows = 6 / shellPrefs->uiScale + (shellPrefs->uiScale == 4);
    uint8_t columns = 4 - (shellPrefs->uiScale == 1);
    unsigned int fileCount = *(&(shellContext->programCount) + shellContext->directory); // Number of files in the current directory

    shellContext->batteryLevel = boot_GetBatteryStatus();
    shellContext->searchString[0] = '\0';

    files_Redraw(rows, columns, fileCount, shellPrefs, shellContext);
    gfx_BlitBuffer();

    bool keyPressed = false;
    clock_t clockOffset = clock(); // Keep track of an offset for timer stuff

    static struct file_t fileInfo;

    // Debounce keypresses when entering the shell.
    while (kb_AnyKey());

    while(!kb_IsDown(kb_KeyClear)) {
        kb_Scan();
        util_UpdateKeyTimer(shellPrefs, shellContext, &clockOffset, &keyPressed);

        if (updateBattery) {
            updateBattery = false;
            shellContext->batteryLevel = boot_GetBatteryStatus();
        }

        if (kb_AnyKey() && !kb_IsDown(kb_KeyClear) && (!keyPressed || clock() - clockOffset > CLOCKS_PER_SEC / 32)) {
            if (!keyPressed) {
                clockOffset = clock();
            }

            if (fileCount) {
                if (kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyEnter)) {
                    while (kb_AnyKey());
                    util_GetFileInfo(shellContext->fileSelected, &fileInfo, shellPrefs, shellContext);

                    if (fileInfo.shellType == DIR_TYPE) {
                        shellContext->fileSelected = 0;

                        #ifdef FR
                        if (fileInfo.name[3] == 'l') { // Applis
                        #else
                        if (fileInfo.name[3] == 's') { // Apps
                        #endif
                            shellContext->directory = APPS_FOLDER;
                            fileCount = shellContext->appCount;
                        #ifdef FR
                        } else if (fileInfo.name[3] == 'V') { // AppVars
                        #else
                        } else if (fileInfo.name[3] == 'V') { // AppVars
                        #endif
                            shellContext->directory = APPVARS_FOLDER;
                            fileCount = shellContext->appVarCount;
                        #ifdef FR
                        } else { // Prgms
                        #else
                        } else { // Programs
                        #endif
                            shellContext->directory = PROGRAMS_FOLDER;
                            fileCount = shellContext->programCount;
                        }

                        shellContext->searchString[0] = '\0';
                        util_WritePrefs(shellPrefs, shellContext, true);
                    } else if (shellContext->directory == APPS_FOLDER) {
                        unsigned int app = shellContext->fileSelected; // Preserve this
                        shellContext->fileStartLoc = 0;
                        shellContext->fileSelected = 0;
                        shellContext->directory = PROGRAMS_FOLDER; // Reset this because Apps do not return to CEaShell
                        util_WritePrefs(shellPrefs, shellContext, false);
                        gfx_End();
                        asm_apps_executeApp(shellContext->appPtrs[app - 1]);
                    } else if (shellContext->directory == PROGRAMS_FOLDER) {
                        util_SearchToMain(shellPrefs, shellContext);
                        asm_runProgram_run(fileInfo.name, fileInfo.type, fileInfo.shellType, shellPrefs);
                    }

                    updateBattery = false;
                } else if (kb_IsDown(kb_KeyVars) || kb_IsDown(kb_KeyDel) || kb_IsDown(kb_KeyMode) || (kb_IsDown(kb_KeyStat) && shellContext->directory != APPS_FOLDER)) {
                    static struct file_t fileInfo;
                    util_GetFileInfo(shellContext->fileSelected, &fileInfo, shellPrefs, shellContext);

                    if (kb_IsDown(kb_KeyVars) && (fileInfo.shellType == BASIC_TYPE || fileInfo.shellType == ICE_SRC_TYPE || fileInfo.shellType == CELTIC_TYPE)) {
                        util_SearchToMain(shellPrefs, shellContext);
                        asm_editProgram_edit(fileInfo.name, fileInfo.shellType == CELTIC_TYPE, shellPrefs);
                    } else if (kb_IsDown(kb_KeyDel)) {
                        menu_DeleteFile(shellPrefs, shellContext, &fileInfo);
                    } else if (kb_IsDown(kb_KeyMode) && shellContext->directory != APPS_FOLDER) {
                        menu_CopyFile(shellPrefs, shellContext, &fileInfo);
                    } else if (kb_IsDown(kb_KeyStat)) {
                        gfx_SetColor(shellPrefs->bgColor);
                        shapes_RoundRectangleFill(9, 56, 205, 208, 20);
                        #ifdef FR
                        gfx_PrintStringXY("Chercher:", 60, 212);
                        #else
                        gfx_PrintStringXY("Search:", 60, 212);
                        #endif
                        char *tempSearch = ui_StringInput(shellPrefs, shellContext, 111, 208, true);

                        if (tempSearch) {
                            memcpy(shellContext->searchString, tempSearch, 9);
                            shellContext->fileStartLoc = 0;
                            shellContext->fileSelected = 0;
                        }
                    }

                    util_CorrectCursorRemove(shellPrefs, shellContext);
                }

                fileCount = *(&(shellContext->programCount) + shellContext->directory);

                if (kb_IsDown(kb_KeyUp)) {
                    if(shellContext->fileSelected) {
                        if (shellContext->fileSelected == shellContext->fileStartLoc) {
                            shellContext->fileStartLoc -= rows;
                        }

                        shellContext->fileSelected--;
                    }
                } else if (kb_IsDown(kb_KeyDown)) {
                    if ((shellContext->fileSelected + 1) % rows) {
                        if (shellContext->fileSelected + 1 < fileCount) {
                            shellContext->fileSelected++;
                        } else if (shellContext->fileSelected > rows) {
                            shellContext->fileSelected = fileCount - 1 - (shellContext->fileSelected + 1) % rows;
                        }
                    } else if (shellContext->fileSelected + 1 != fileCount) {
                        shellContext->fileSelected++;

                        if (shellContext->fileSelected >= shellContext->fileStartLoc + rows * columns) {
                            shellContext->fileStartLoc += rows;
                        }
                    }
                }

                if (kb_IsDown(kb_KeyLeft)) {
                    if (shellContext->fileSelected - shellContext->fileStartLoc >= rows) {
                        shellContext->fileSelected -= rows;
                    } else if (shellContext->fileSelected - shellContext->fileStartLoc < rows) {
                        if (shellContext->fileStartLoc) {
                            shellContext->fileStartLoc -= rows;
                            shellContext->fileSelected -= rows;
                        } else if (shellContext->fileSelected) {
                            shellContext->fileSelected = 0;
                        }
                    }
                } else if (kb_IsDown(kb_KeyRight)) {
                    if (shellContext->fileSelected - shellContext->fileStartLoc < rows * (columns - 1) && shellContext->fileSelected + rows < fileCount) {
                        shellContext->fileSelected += rows;
                    } else if (shellContext->fileSelected - shellContext->fileStartLoc >= rows * (columns - 1)) {
                        if (shellContext->fileSelected + rows < fileCount) {
                            shellContext->fileStartLoc += rows;
                            shellContext->fileSelected += rows;
                        } else if (shellContext->fileStartLoc + rows * columns < fileCount) {
                            shellContext->fileStartLoc += rows;
                            shellContext->fileSelected = fileCount - 1;
                        } else {
                            shellContext->fileSelected = fileCount - 1;
                        }
                    } else {
                        shellContext->fileSelected = fileCount - 1;
                    }
                }
            } else {
                shellContext->fileSelected = 0;
            }

            if (kb_IsDown(kb_KeyYequ)) {
                custom_Open(shellPrefs, shellContext);

                // Don't need to do this if we're exiting since we'll do it again anyway
                if (!kb_IsDown(kb_KeyClear)) {
                    util_WritePrefs(shellPrefs, shellContext, true);
                }

                rows = 6 / shellPrefs->uiScale + (shellPrefs->uiScale == 4);
                columns = 4 - (shellPrefs->uiScale == 1);
                fileCount = *(&(shellContext->programCount) + shellContext->directory);
                files_Redraw(rows, columns, fileCount, shellPrefs, shellContext);
                gfx_SetDrawScreen();

                if (shellPrefs->transitionSpeed) {
                    for (uint8_t frame = 16 / shellPrefs->transitionSpeed; frame > 2; frame--) {
                        if (!(frame & 1)) { // Skip every other frame because of speed issue with SPI stuff I didn't fix
                            ui_TransitionDrawFrame(6, 8, 232 - frame * 12 * shellPrefs->transitionSpeed, frame * (19 * shellPrefs->transitionSpeed), frame * (12 * shellPrefs->transitionSpeed));
                        }
                    }
                }

                updateBattery = true;
            } else if (fileCount && (kb_IsDown(kb_KeyWindow) || kb_IsDown(kb_KeyZoom) || kb_IsDown(kb_KeyTrace) || kb_IsDown(kb_KeyAlpha))) {
                info_Open(shellPrefs, shellContext, &fileCount);

                if (!kb_IsDown(kb_KeyClear)) {
                    util_CorrectCursorRemove(shellPrefs, shellContext);
                    fileCount = *(&(shellContext->programCount) + shellContext->directory);
                }

                files_Redraw(rows, columns, fileCount, shellPrefs, shellContext);
                gfx_SetDrawScreen();

                if (shellPrefs->transitionSpeed) {
                    for (int8_t frame = 16 / shellPrefs->transitionSpeed; frame > 2; frame--) {
                        if (!(frame & 1)) {
                            ui_TransitionDrawFrame(15, 50, 231 - frame * 12 * shellPrefs->transitionSpeed, 220, frame * 12 * shellPrefs->transitionSpeed);
                        }
                    }
                }

                updateBattery = true;
            } else if (kb_IsDown(kb_KeyGraph)) {
                util_SetGFXChar('[', leftBracket, 5);
                settings_Open(shellPrefs, shellContext);

                if (!kb_IsDown(kb_KeyClear)) {
                    util_WritePrefs(shellPrefs, shellContext, true);
                }

                rows = 6 / shellPrefs->uiScale + (shellPrefs->uiScale == 4);
                columns = 4 - (shellPrefs->uiScale == 1);
                fileCount = *(&(shellContext->programCount) + shellContext->directory);
                util_SetGFXChar('[', thetaChar, 8);
                files_Redraw(rows, columns, fileCount, shellPrefs, shellContext);
                gfx_SetDrawScreen();

                if (shellPrefs->transitionSpeed) {
                    for (int8_t frame = 16 / shellPrefs->transitionSpeed; frame > 3; frame--) {
                        if (!(frame & 1)) {
                            ui_TransitionDrawFrame(6, 312 - frame * 19 * shellPrefs->transitionSpeed, 232 - frame * 12 * shellPrefs->transitionSpeed, frame * (19 * shellPrefs->transitionSpeed), frame * (12 * shellPrefs->transitionSpeed));
                        }
                    }
                }

                updateBattery = true;
            }

            if (kb_IsDown(kb_KeyClear)) {
                continue;
            }

            if (!keyPressed && asm_utils_getCharFromKey(INPUT_UPPER)) {
                util_AlphaSearch(shellPrefs, shellContext, asm_utils_getCharFromKey(INPUT_UPPER));
            }

            gfx_SetDrawBuffer();
            files_Redraw(rows, columns, fileCount, shellPrefs, shellContext);
            gfx_BlitBuffer();

            util_WaitBeforeKeypress(&clockOffset, &keyPressed);
        } else if (fileCount == 1 && !strcmp(asm_utils_getFileName(shellContext->programPtrs[0]), "CAESHELL")) {
            uint8_t color = randInt(0, 255);
            uint8_t drawY = 240;

            while (!kb_AnyKey()) {
                for (unsigned int drawX = 0; drawX < 340; drawX += 20) {
                    gfx_FillCircle(drawX, drawY, randInt(5, 15));
                    gfx_SetColor(color);
                    color += 32;
                }

                gfx_BlitBuffer();
                drawY -= 16;
            }
        }
    }

    util_WaitBeforeKeypress(&clockOffset, &keyPressed);
}
