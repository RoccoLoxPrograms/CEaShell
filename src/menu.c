#include "gfx/gfx.h"
#include "menu.h"
#include "shapes.h"
#include "ui.h"
#include "utility.h"
#include "asm/invert.h"
#include "asm/fileOps.h"
#include "asm/sortVat.h"

#include <graphx.h>
#include <keypadc.h>
#include <fileioc.h>
#include <sys/timers.h>
#include <sys/power.h>
#include <string.h>

static void menu_ThemePreview(const uint8_t color, uint8_t *colors, const uint8_t *defaultThemes) {   // Draws the theme preview box. Basically a bunch of rectangles
    if (color == 27) {
        shapes_RoundRectangleFill(255 - colors[0], 7, 134, 86, 18, 112);
        shapes_RoundRectangleFill(255 - colors[1], 6, 61, 78, 22, 116);
        shapes_RoundRectangleFill(255 - colors[2], 6, 61, 39, 87, 116);
    } else {
        shapes_RoundRectangleFill(defaultThemes[color + 1], 7, 134, 86, 18, 112);
        shapes_RoundRectangleFill(defaultThemes[color], 6, 61, 78, 22, 116);
        shapes_RoundRectangleFill(defaultThemes[color + 2], 6, 61, 39, 87, 116);
    }
}

static void menu_LooksRefresh(const uint8_t color, uint8_t *colors, const uint8_t *defaultThemes, const int cursorX, const uint8_t cursorY, const bool is24Hour, const uint8_t transitionSpeed, const bool displayCEaShell, const bool themePicker, const uint8_t option) {
    shapes_RoundRectangleFill(colors[0], 8, 140, 56, 15, 46);

    if (!themePicker) {
        shapes_RoundRectangleFill(colors[0], 8, 140, 155, 165, 46);
        switch (option) {
            case 0:
                shapes_PixelIndentRectangle(colors[2], colors[0], 170, 51, 130, 9);
                break;
            case 1:
                shapes_PixelIndentRectangle(colors[2], colors[0], 170, 68, 130, 9);
                break;
            case 2:
                shapes_PixelIndentRectangle(colors[2], colors[0], 170, 85, 130, 21);
                break;
            case 3:
                shapes_PixelIndentRectangle(colors[2], colors[0], 170, 114, 130, 21);
                break;
            case 4:
                shapes_PixelIndentRectangle(colors[2], colors[0], 170, 143, 130, 9);
                break;
            default:
                break;
        }

        gfx_SetTextScale(1, 1); // Customizing options
        gfx_PrintStringXY("Clock:", 171, 52);
        gfx_PrintStringXY("<", 235, 52);
        gfx_PrintStringXY(">", 294, 52);
        if (is24Hour) {
            gfx_PrintStringXY("24 Hour", 242, 52);
        } else {
            gfx_PrintStringXY("AM/PM", 247, 52);
        }
        gfx_PrintStringXY("Transitions:", 171, 69);
        gfx_PrintStringXY("<", 264, 69);
        gfx_PrintStringXY(">", 294, 69);
        gfx_PrintStringXY("Transition", 171, 86);
        gfx_PrintStringXY("Speed:", 171, 98);
        gfx_PrintStringXY("<", 243, 98);
        gfx_PrintStringXY(">", 294, 98);
        gfx_PrintStringXY("Hide", 171, 115);
        gfx_PrintStringXY("CEaShell:", 171, 127);
        gfx_PrintStringXY("<", 264, 127);
        gfx_PrintStringXY(">", 294, 127);
        gfx_PrintStringXY("Change Theme", 171, 144);
        gfx_PrintStringXY("Nothing", 59, 144);
        gfx_PrintStringXY("Selected", 55, 157);
        if (transitionSpeed) {
            gfx_PrintStringXY("On", 274, 69);
            if (transitionSpeed == 1) {
                gfx_PrintStringXY("Slow", 257, 98);
            } else if (transitionSpeed == 2) {
                gfx_PrintStringXY("Normal", 249, 98);
            } else if (transitionSpeed == 3) {
                gfx_PrintStringXY("Fast", 256, 98);
            }
        } else {
            gfx_PrintStringXY("Off", 270, 69);
            gfx_PrintStringXY("Off", 259, 98);
        }
        if (!displayCEaShell) {
            gfx_PrintStringXY("On", 274, 127);
        } else {
            gfx_PrintStringXY("Off", 270, 127);
        }
    } else {
        shapes_RoundRectangleFill(colors[2], 8, 26, 26, cursorX, cursorY);
        menu_ThemePreview(color, colors, defaultThemes);
    }
    uint8_t drawBox = 0;    // Theme selector
    for (uint8_t y = 49; y < 78; y += 28) {
        for (int x = 18; x < 132; x += 28, drawBox += 3) {
            shapes_RoundRectangleFill(defaultThemes[drawBox], 6, 22, 22, x, y);
        }
    }
    gfx_TransparentSprite_NoClip(invSwitch, 130, 77);   // Invert color icon
    ui_DrawUISprite(colors[1], UI_LARROW, 15, 208);
}

void menu_Looks(uint8_t *colors, unsigned int *fileSelected, const unsigned int fileCount, const unsigned int fileStartLoc, bool *is24Hour, uint8_t *transitionSpeed, const bool appvars, bool *displayCEaShell) {
    const uint8_t defaultThemes[28] = {237, 246, 236, 74, 148, 0, 128, 137, 96, 226, 228, 162, 3, 100, 2, 28, 125, 58, 210, 243, 208, 81, 114, 48, 222, 255, 181, 222};
    shapes_RoundRectangleFill(colors[1], 8, 304, 192, 8, 39);   // Background
    shapes_RoundRectangleFill(colors[0], 8, 140, 92, 15, 109);
    menu_LooksRefresh(0, colors, defaultThemes, 16, 47, *is24Hour, *transitionSpeed, *displayCEaShell, false, 0);
    gfx_BlitBuffer();

    uint8_t color = 0;
    uint8_t cursorY = 47;
    uint8_t prevCursorY = 47;
    int cursorX = 16;
    int prevCursorX = 16;
    uint8_t pColor = 0;
    uint8_t option = 0;

    uint8_t batteryStatus = boot_GetBatteryStatus();

    bool themePicker = false;
    bool keyPressed = false;
    
    while (kb_AnyKey());
    while (!kb_IsDown(kb_KeyYequ) && !kb_IsDown(kb_KeyClear)) { // Key detection loop
        kb_Scan();
        if (!kb_AnyKey()) {
            keyPressed = false;
            timer_Set(1, 0);
        }
        if ((kb_Data[7] || kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyEnter)) && (!keyPressed || timer_Get(1) > 3000)) {
            if (themePicker) {
                prevCursorY = cursorY;
                prevCursorX = cursorX;
                pColor = color;
                if (kb_IsDown(kb_KeyRight) && cursorX == 128) {	// Cursor looping
                    cursorX = 16;
                    cursorY = cursorY - 28 * (cursorY == 75) + 28 * (cursorY == 47);
                } else if (kb_IsDown(kb_KeyLeft) && cursorX == 16) {
                    cursorX = 128;
                    cursorY = cursorY - 28 * (cursorY == 75) + 28 * (cursorY == 47);
                } else {
	    	        cursorY = cursorY - 28 * (kb_IsDown(kb_KeyUp) && cursorY > 47) + 28 * (kb_IsDown(kb_KeyDown) && cursorY < 75);
                    cursorX = cursorX - 28 * (kb_IsDown(kb_KeyLeft) && cursorX > 16) + 28 * (kb_IsDown(kb_KeyRight) && cursorX < 128);
                }
                color = 3 * ((cursorX - 16) / 28) + 15 * (cursorY > 47);    // Change the selected color/theme

                shapes_RoundRectangleFill(colors[0], 8, 26, 26, prevCursorX, prevCursorY);    // Erase old color
                shapes_RoundRectangleFill(defaultThemes[pColor], 6, 22, 22, prevCursorX + 2, prevCursorY + 2);

                shapes_RoundRectangleFill(colors[2], 8, 26, 26, cursorX, cursorY);
                shapes_RoundRectangleFill(defaultThemes[color], 6, 22, 22, cursorX + 2, cursorY + 2);

                if ((prevCursorX == 128 && prevCursorY == 75) || (cursorX == 128 && cursorY == 75)) {
                    gfx_TransparentSprite_NoClip(invSwitch, 130, 77);
                }
            } else {
                if (kb_IsDown(kb_KeyDown)) {
                    if (option < 4) {
                        option++;
                    } else {
                        option = 0;
                    }
                } else if (kb_IsDown(kb_KeyUp)) {
                    if (option) {
                        option--;
                    } else {
                        option = 4;
                    }
                } else if (kb_IsDown(kb_KeyLeft) || kb_IsDown(kb_KeyRight)) {
                    switch (option) {
                        case 0:
                            *is24Hour = !*is24Hour;
                            ui_StatusBar(colors[1], *is24Hour, batteryStatus, "Customize");
                            gfx_BlitBuffer();
                            break;
                        case 1:
                            *transitionSpeed = !*transitionSpeed;
                            *transitionSpeed = *transitionSpeed + *transitionSpeed;
                            break;
                        case 2:
                            if (kb_IsDown(kb_KeyLeft)) {
                                if (*transitionSpeed > 1) {
                                    *transitionSpeed = *transitionSpeed - 1;
                                } else {
                                    *transitionSpeed = 3;
                                }
                            } else {
                                if (*transitionSpeed < 3) {
                                    *transitionSpeed = *transitionSpeed + 1;
                                } else {
                                    *transitionSpeed = 1;
                                }
                            }
                            break;
                        case 3:
                            *displayCEaShell = !*displayCEaShell;
                            break;
                        default:
                            break;
                    }
                } else if (kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyEnter)) {
                    if (option == 4) {
                        themePicker = true;
                    }
                    while (kb_AnyKey());
                }
                
                if (themePicker) {
                    menu_ThemePreview(color, colors, defaultThemes);    // Refresh the preview if a different theme is selected
                }
                gfx_BlitBuffer();
            }
            menu_LooksRefresh(color, colors, defaultThemes, cursorX, cursorY, *is24Hour, *transitionSpeed, *displayCEaShell, themePicker, option);

            gfx_BlitBuffer();
            if (!keyPressed) {
                while (timer_Get(1) < 9000 && kb_Data[7]) {
                    kb_Scan();
                }
            }
            keyPressed = true;
            timer_Set(1,0);
        }
        if ((kb_IsDown(kb_KeyEnter) || kb_IsDown(kb_Key2nd)) && themePicker) {   // If you select a theme
            themePicker = false;
            if (color == 27) {
                colors[3] = !colors[3];
                invertPalette();
            } else {
                colors[0] = defaultThemes[color + 1];
                colors[1] = defaultThemes[color];
                colors[2] = defaultThemes[color + 2];
            }
            gfx_FillScreen(colors[0]);
            ui_DrawAllFiles(colors, *fileSelected, fileCount, fileStartLoc, appvars, displayCEaShell);
            shapes_RoundRectangleFill(colors[1], 8, 304, 192, 8, 39);
            shapes_RoundRectangleFill(colors[0], 8, 140, 92, 15, 109);
            menu_LooksRefresh(color, colors, defaultThemes, cursorX, cursorY, *is24Hour, *transitionSpeed, *displayCEaShell, themePicker, option);
            ui_StatusBar(colors[1], *is24Hour, batteryStatus, "Customize");
            gfx_BlitBuffer();
        } else if (kb_IsDown(kb_KeyAlpha) && themePicker) {
            themePicker = false;
            shapes_RoundRectangleFill(colors[0], 8, 140, 92, 15, 109);
            menu_LooksRefresh(color, colors, defaultThemes, cursorX, cursorY, *is24Hour, *transitionSpeed, *displayCEaShell, themePicker, option);
            gfx_BlitBuffer();
        }
    }
    gfx_SetTextFGColor(255 * !(colors[1] > 131 && colors[1] % 8 > 3));
}

static void menu_InfoRedraw(const bool fullRedraw, const bool drawCursor, uint8_t *colors, const int cursorX, const uint8_t cursorY, const bool isArchived, const bool isLocked, const bool isHidden, const char *fileTypeString, char *fileName, const int fileSize, const uint8_t fileType, const uint8_t osFileType) {
    if (fullRedraw) {   // Redraws most of the file information. (Comment is here since the line above is already quite long)
        shapes_RoundRectangleFill(colors[1], 15, 220, 192, 50, 38);
        shapes_RoundRectangleFill(colors[2], 8, 138, 30, 56, 44);
        shapes_RoundRectangleFill(colors[0], 8, 138, 122, 56, 80);
        shapes_RoundRectangleFill(colors[0], 8, 82, 88, 182, 114);
        gfx_SetColor(colors[0]);
        gfx_FillRectangle_NoClip(194, 103, 11, 11);
        gfx_SetColor(colors[1]);
        gfx_FillCircle_NoClip(205, 102, 11);
        gfx_sprite_t *corner1 = gfx_MallocSprite(8, 8);
        shapes_GetRoundCorners(corner1, colors[1], 8, 200, 44);
        ui_DrawFile(false, false, false, isHidden, colors, fileName, fileType, osFileType, 200, 44);  // We don't draw a name here because it is drawn somewhere else
        shapes_DrawRoundCorners(corner1, 64, 64, 200, 44);  // We have to round the corners of the program icon a little more in this menu

        char *description = malloc(52);
        fileName[0] -= 64 * isHidden;
        if (fileType != BASIC_TYPE && fileType != ICE_SRC_TYPE && getDescASM(fileName, osFileType, fileType, description)) {
            ui_DescriptionWrap(description, 27, 61, 121);
        } else if (fileType == BASIC_TYPE && getDescBASIC(fileName, osFileType, description)) {
            ui_DescriptionWrap(description, 27, 61, 121);
        } else {
            gfx_PrintStringXY("No description.", 61, 126);
        }
        if (osFileType != OS_TYPE_APPVAR) {
            fileName[0] += 64 * isHidden;
        }
        free (description);
        free (corner1);
        gfx_SetColor(colors[0]);
        gfx_SetTextScale(2, 2);
        const uint8_t nameX = 125 - gfx_GetStringWidth(fileName) / 2;
        gfx_PrintStringXY(fileName, nameX, 52);
        gfx_SetTextScale(1, 1);
        gfx_PrintStringXY("Type: ", 61, 85);
        gfx_PrintString(fileTypeString);
        gfx_PrintStringXY("Size: ", 61, 98);
        gfx_SetTextXY(99, 98);
        gfx_PrintInt(fileSize, 5);
        gfx_PrintStringXY("Description:", 61, 111);
        gfx_PrintStringXY("Attributes:", 61, 145);
        gfx_PrintStringXY("File Operations:", 61, 171);
        ui_DrawUISprite(colors[1], UI_DARROW, 152, 208);
    }
    gfx_SetColor(colors[0]);
    gfx_FillRectangle_NoClip(63, 156, 193, 9);
    gfx_FillRectangle_NoClip(63, 182, 194, 11);
    if (drawCursor) {
        if (cursorY == 156) {
            gfx_SetColor(colors[2]);
            gfx_FillRectangle_NoClip(cursorX, cursorY, 9, 9);
            gfx_SetColor(colors[0]);
            gfx_SetPixel(cursorX, cursorY);
            gfx_SetPixel(cursorX, cursorY + 8);
            gfx_SetPixel(cursorX + 8, cursorY + 8);
            gfx_SetPixel(cursorX + 8, cursorY);
        } else {
            shapes_RoundRectangleFill(colors[2], 3, 62, 11, cursorX, cursorY);
        }
    }
    ui_CheckBox(colors[1], gfx_GetPixel(64, 157), isArchived, 64, 157);
    gfx_PrintStringXY("Archived", 74, 157);
    ui_CheckBox(colors[1], gfx_GetPixel(140, 157), isLocked, 140, 157);
    gfx_PrintStringXY("Locked", 150, 157);
    ui_CheckBox(colors[1], gfx_GetPixel(203, 157), isHidden, 203, 157);
    gfx_PrintStringXY("Hidden", 213, 157);
    gfx_PrintStringXY("Delete", 71, 184);
    gfx_PrintStringXY("Rename", 136, 184);
    gfx_PrintStringXY("Edit", 213, 184);
}

void menu_Info(uint8_t *colors, bool *infoOps, uint8_t fileSelected, const unsigned int fileStartLoc, uint8_t *fileNumbers, const bool appvars, const bool displayCEaShell) {
    uint8_t osFileType; // Different from C, ICE, ASM, etc. This is stuff like OS_TYPE_APPVAR and OS_TYPE_PRGM
    unsigned filesSearched = 0;
    char newName[9]= "\0";
    char *fileName;
    void *vatPtr = NULL;
    while ((fileName = ti_DetectAny(&vatPtr, NULL, &osFileType))) { // Suspiciously similar to the example in the docs :P
        if (*fileName == '!' || *fileName == '#') {
            continue;
        }
        if (!displayCEaShell && !strcmp(fileName, "CEASHELL")) {
            continue;
        }
        if (appvars && osFileType == OS_TYPE_APPVAR) {
            if (fileSelected == filesSearched) {
                break;
            }
            filesSearched++;
        } else if (!appvars && (osFileType == OS_TYPE_PRGM || osFileType == OS_TYPE_PROT_PRGM)) {
            if (fileSelected == filesSearched) {
                break;
            }
            filesSearched++;
        }
    }
    uint8_t fileType = getPrgmType(fileName, osFileType);
    if (osFileType == OS_TYPE_APPVAR) {
        fileType = APPVAR_TYPE;
    }
    char *fileTypeString = util_FileTypeToString(fileType, false);
    uint8_t slot = ti_OpenVar(fileName, "r", osFileType);
    int fileSize = getProgSize(fileName, osFileType);
    bool isArchived = ti_IsArchived(slot);
    bool isLocked = (osFileType == OS_TYPE_PROT_PRGM);
    bool isHidden = (fileName[0] < 65);
    const bool initialValue[3] = {isArchived, isLocked, isHidden};
    fileName[0] += 64 * (fileName[0] < 65);

    int cursorX = 63;
    uint8_t cursorY = 156;

    menu_InfoRedraw(true, true, colors, cursorX, cursorY, isArchived, isLocked, isHidden, fileTypeString, fileName, fileSize, fileType, osFileType);
    gfx_BlitBuffer();

    bool keyPressed = false;
    bool redraw = false;

    fileName[0] -= 64 * initialValue[2];    // The viewable file name becomes what the OS sees instead
    while (kb_AnyKey());
    while (!kb_IsDown(kb_KeyWindow) && !kb_IsDown(kb_KeyZoom) && !kb_IsDown(kb_KeyTrace) && !kb_IsDown(kb_KeyAlpha) && !kb_IsDown(kb_KeyClear)) {
        kb_Scan();
        if (!kb_AnyKey()) {
            keyPressed = false;
            timer_Set(1, 0);
        }
        if ((kb_Data[7] || kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyEnter) || kb_IsDown(kb_KeyDel)) && (!keyPressed || timer_Get(1) > 3000)) {
            if (kb_IsDown(kb_KeyRight)) {
                if (cursorX < 195) {
                    if (cursorY == 182) {
                        cursorX += 66;
                    } else {
                        cursorX = cursorX + 76 * (cursorX == 63) + 63 * (cursorX == 139);
                    }
                } else {
                    cursorX = 63;
                    if (cursorY == 182) {
                        cursorY = 156;
                    } else {
                        cursorY = 182; 
                    }
                }
            } else if (kb_IsDown(kb_KeyLeft)) {
                if (cursorX > 63) {
                    if (cursorY == 182) {
                        cursorX -= 66;
                    } else {
                        cursorX = cursorX - 76 * (cursorX == 139) - 63 * (cursorX == 202);
                    }
                } else {
                    if (cursorY == 182) {
                        cursorX = 202;
                        cursorY = 156;
                    } else {
                        cursorX = 195;
                        cursorY = 182; 
                    }
                }
            }
            if (kb_IsDown(kb_KeyUp) && cursorY == 182) {
                if (cursorX > 63) {
                    if (cursorX == 129) {
                        cursorX = 139;
                    } else {
                        cursorX = 202;
                    }
                }
                cursorY = 156;
            } else if (kb_IsDown(kb_KeyDown)) {
                if (cursorX > 63) {
                    if (cursorX == 139) {
                        cursorX = 129;
                    } else {
                        cursorX = 195;
                    }
                }
                cursorY = 182;
            }
            if (kb_IsDown(kb_KeyEnter) || kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyDel)) {
                if (cursorY == 156 && !kb_IsDown(kb_KeyDel)) {
                    if (cursorX == 63) {
                        isArchived = !isArchived;
                    } else if (cursorX == 139 && (fileType == BASIC_TYPE || fileType == ICE_SRC_TYPE) && !appvars) {
                        isLocked = !isLocked;
                    } else if (cursorX == 202 && !appvars) {
                        isHidden = !isHidden;
                    }
                } else {
                    if (((kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyEnter)) && cursorX == 63) || kb_IsDown(kb_KeyDel)) {
                        menu_InfoRedraw(false, false, colors, cursorX, cursorY, isArchived, isLocked, isHidden, fileTypeString, fileName, fileSize, fileType, osFileType);
                        if (ui_DeleteConf(colors, 56, 206)) {
                            ti_Close(slot);
                            ti_DeleteVar(fileName, osFileType);
                            if (fileSelected >= fileNumbers[appvars] - 1) {
                                fileSelected--;
                            }
                            gfx_SetColor(colors[0]);
                            gfx_FillRectangle_NoClip(12, 28, 296, 164);
                            ui_DrawAllFiles(colors, fileSelected, fileNumbers[appvars] - 1, fileStartLoc, appvars, displayCEaShell);
                            gfx_BlitRectangle(gfx_buffer, 12, 28, 296, 10);
                            infoOps[0] = true;
                            return;
                        }
                        menu_InfoRedraw(true, true, colors, cursorX, cursorY, isArchived, isLocked, isHidden, fileTypeString, fileName, fileSize, fileType, osFileType);
                        gfx_BlitBuffer();
                    } else if (cursorX == 129) {
                        memcpy(newName, fileName, strlen(fileName) + 1);   // Copy name into string to modify
                        newName[0] += 64 * (newName[0] < 65);
                        menu_InfoRedraw(true, false, colors, cursorX, cursorY, isArchived, isLocked, isHidden, fileTypeString, newName, fileSize, fileType, osFileType);
                        if (ui_RenameBox(colors, newName)) {
                            ti_Close(slot);
                            ti_RenameVar(fileName, newName, osFileType);
                            sortVAT();
                            memcpy(fileName, newName, strlen(newName) + 1);
                            ti_OpenVar(fileName, "r", osFileType);
                        }
                        menu_InfoRedraw(true, true, colors, cursorX, cursorY, isArchived, isLocked, isHidden, fileTypeString, fileName, fileSize, fileType, osFileType);
                        gfx_BlitBuffer();
                        kb_Scan();
                        if (kb_IsDown(kb_KeyClear)) {
                            continue;
                        }
                    } else {
                        // edit programs
                    }
                }
                while (kb_AnyKey());
            }
            redraw = true;
            if (!keyPressed) {
                while (timer_Get(1) < 9000 && kb_Data[7]) {
                    kb_Scan();
                }
            }
            keyPressed = true;
            timer_Set(1,0);
        }
        if (redraw) {
            menu_InfoRedraw(false, true, colors, cursorX, cursorY, isArchived, isLocked, isHidden, fileTypeString, fileName, fileSize, fileType, osFileType);
            gfx_SwapDraw();
            redraw = false;
        }
    }

    if (initialValue[0] != isArchived) {    // We'll only update the status if the program actually has to be changed
        if (initialValue[0]) {
            ti_Close(slot);
            slot = ti_OpenVar(fileName, "r+", osFileType);
        } else {
            infoOps[1] = !ti_ArchiveHasRoom(fileSize + 18);
            ti_SetArchiveStatus(true, slot);
        }
    }
    if (initialValue[1] != isLocked) {
        lockPrgm(fileName, osFileType);
        if (isLocked) {
            osFileType = OS_TYPE_PRGM;
        } else {
            osFileType = OS_TYPE_PROT_PRGM;
        }
    }
    if (initialValue[2] != isHidden) {
        hidePrgm(fileName, osFileType);
        ui_DrawAllFiles(colors, fileSelected, fileNumbers[appvars], fileStartLoc, appvars, displayCEaShell);
        gfx_BlitRectangle(gfx_buffer, 12, 28, 296, 10);
        if (isHidden) {
            fileName[0] -= 64;
        } else {
            fileName[0] += 64;
        }
    }
    ti_Close(slot);
}

void menu_Settings(const uint8_t color) { // Very empty right now
    shapes_RoundRectangleFill(color, 15, 304, 192, 8, 39);
    ui_DrawUISprite(color, UI_RARROW, 290, 208);
    gfx_SwapDraw();
    while (kb_AnyKey());
    while (!kb_IsDown(kb_KeyGraph) && !kb_IsDown(kb_KeyClear)) {
        kb_Scan();
    }
}
