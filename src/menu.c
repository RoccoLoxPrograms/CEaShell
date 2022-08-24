#include "gfx/gfx.h"
#include "menu.h"
#include "shapes.h"
#include "ui.h"
#include "utility.h"
#include "asm/invert.h"
#include "asm/fileOps.h"

#include <graphx.h>
#include <keypadc.h>
#include <fileioc.h>
#include <sys/timers.h>
#include <sys/power.h>

#include <debug.h>

static void menu_ThemePreview(uint8_t color, uint8_t *colors, const uint8_t *defaultThemes) {   // Draws the theme preview box. Basically a bunch of rectangles
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

static void menu_LooksRefresh(uint8_t color, uint8_t *colors, const uint8_t *defaultThemes, int cursorX, uint8_t cursorY) {
    shapes_RoundRectangleFill(colors[1], 8, 304, 192, 8, 39);   // Background
    shapes_RoundRectangleFill(colors[0], 8, 140, 56, 15, 46);
    shapes_RoundRectangleFill(colors[0], 8, 140, 92, 15, 109);
    shapes_RoundRectangleFill(colors[0], 8, 140, 155, 165, 46);

    uint8_t drawBox = 0;    // Theme selector
    shapes_RoundRectangleFill(colors[2], 8, 26, 26, cursorX, cursorY);
    for (uint8_t y = 49; y < 78; y += 28) {
        for (int x = 18; x < 132; x += 28, drawBox += 3) {
            shapes_RoundRectangleFill(defaultThemes[drawBox], 6, 22, 22, x, y);
        }
    }
    gfx_SetColor(148);  // Invert color icon
    gfx_Line_NoClip(132, 79, 149, 96);
    gfx_FloodFill(133, 78, 0);
    menu_ThemePreview(color, colors, defaultThemes);
    ui_DrawUISprite(colors[1], UI_LARROW, 15, 208);
}

void menu_Looks(uint8_t *colors, uint8_t fileSelected, uint8_t fileCount, uint8_t fileStartLoc, bool is24Hour, bool appvars) {
    const uint8_t defaultThemes[28] = {237, 246, 236, 74, 148, 0, 128, 137, 96, 226, 228, 162, 3, 100, 2, 28, 125, 58, 210, 243, 208, 81, 114, 48, 222, 255, 181, 222};
    menu_LooksRefresh(0, colors, defaultThemes, 16, 47);
    gfx_BlitBuffer();

    uint8_t color = 0;
    uint8_t cursorY = 47;
    uint8_t prevCursorY = 47;
    int cursorX = 16;
    int prevCursorX = 16;
    uint8_t pColor = 0;

    bool keyPressed = false;
    
    while (kb_AnyKey());
    while (!kb_IsDown(kb_KeyYequ) && !kb_IsDown(kb_KeyClear)) { // Key detection loop
        kb_Scan();
        if (!kb_AnyKey()) {
            keyPressed = false;
            timer_Set(1, 0);
        }
        if (kb_Data[7] && (!keyPressed || timer_Get(1) > 3000)) {
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
                gfx_SetColor(148);
                gfx_Line_NoClip(132, 79, 149, 96);
                gfx_FloodFill(133, 78, 0);
            }
            
            menu_ThemePreview(color, colors, defaultThemes);    // Refresh the preview if a different theme is selected
            gfx_BlitBuffer();
            if (!keyPressed) {
                while (timer_Get(1) < 9000 && kb_Data[7]) {
                    kb_Scan();
                }
            }
            keyPressed = true;
            timer_Set(1,0);
        }
        if (kb_IsDown(kb_KeyEnter) || kb_IsDown(kb_Key2nd)) {   // If you select a theme
            if (color == 27) {
                colors[3] = !colors[3];
                invertPalette();
            } else {
                colors[0] = defaultThemes[color + 1];
                colors[1] = defaultThemes[color];
                colors[2] = defaultThemes[color + 2];
            }
            gfx_FillScreen(colors[0]);
            ui_DrawAllFiles(colors, fileSelected, fileCount, fileStartLoc, appvars);
            menu_LooksRefresh(color, colors, defaultThemes, cursorX, cursorY);
            ui_StatusBar(colors[1], is24Hour, boot_GetBatteryStatus(), "Customize");    // Might as well also update the battery
            gfx_BlitBuffer();
        }
    }
    gfx_SetTextFGColor(255 * !(colors[1] > 131 && colors[1] % 8 > 3));
}

static void menu_InfoRedraw(bool fullRedraw, uint8_t *colors, int cursorX, uint8_t cursorY, bool isArchived, bool isLocked, bool isHidden, char *fileTypeString, char *fileName, int fileSize, uint8_t fileType, uint8_t osFileType) {
    if (fullRedraw) {   // Redraws most of the file information. (Comment is here since the line above is already quite long)
        shapes_RoundRectangleFill(colors[1], 15, 220, 192, 50, 38);
        shapes_RoundRectangleFill(colors[2], 8, 138, 30, 56, 44);
        shapes_RoundRectangleFill(colors[0], 8, 138, 122, 56, 80);
        shapes_RoundRectangleFill(colors[0], 8, 82, 88, 182, 114);
        gfx_SetColor(colors[0]);
        gfx_FillRectangle_NoClip(194, 103, 11, 11);
        gfx_SetColor(colors[1]);
        gfx_FillCircle_NoClip(205, 102, 11);
        ui_DrawFile(false, false, false, isHidden, colors, fileName, fileType, osFileType, 200, 44);  // We don't draw a name here because it is drawn somewhere else
        char *description = malloc(52);
        fileName[0] -= 64 * isHidden;
        if (fileType != BASIC_TYPE && fileType != ICE_SRC_TYPE && getDescASM(fileName, osFileType, fileType, description)) {
            ui_DescriptionWrap(description, 27, 61, 121);
        } else if (fileType == BASIC_TYPE && getDescBASIC(fileName, osFileType, description)) {
            ui_DescriptionWrap(description, 27, 61, 121);
        } else {
            gfx_PrintStringXY("No description.", 61, 126);
        }
        fileName[0] += 64 * isHidden;
        free (description);
        gfx_SetColor(colors[0]);
        gfx_SetTextScale(2, 2);
        uint8_t nameX = 125 - gfx_GetStringWidth(fileName) / 2;
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

void menu_Info(uint8_t *colors, bool *infoOps, uint8_t fileSelected, uint8_t fileStartLoc, uint8_t *fileNumbers, bool appvars) {
    uint8_t osFileType; // Different from C, ICE, ASM, etc. This is stuff like OS_TYPE_APPVAR and OS_TYPE_PRGM
    uint8_t filesSearched = 0;
    char *fileName;
    void *vatPtr = NULL;
    while ((fileName = ti_DetectAny(&vatPtr, NULL, &osFileType))) { // Suspiciously similar to the example in the docs :P
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
    char *fileTypeString = util_FileTypeToString(fileType, false);
    uint8_t slot = ti_OpenVar(fileName, "r", osFileType);
    int fileSize = ti_GetSize(slot);
    bool isArchived = ti_IsArchived(slot);
    bool isLocked = (osFileType == OS_TYPE_PROT_PRGM);
    bool isHidden = (fileName[0] < 65);
    const bool initialValue[3] = {isArchived, isLocked, isHidden};
    fileName[0] += 64 * (fileName[0] < 65);

    int cursorX = 63;
    uint8_t cursorY = 156;

    gfx_sprite_t *corner1 = gfx_MallocSprite(8, 8);
    shapes_GetRoundCorners(corner1, colors[1], 8, 200, 44);
    menu_InfoRedraw(true, colors, cursorX, cursorY, isArchived, isLocked, isHidden, fileTypeString, fileName, fileSize, fileType, osFileType);
    shapes_DrawRoundCorners(corner1, 64, 64, 200, 44);  // We have to round the corners of the program icon a little more in this menu
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
        if ((kb_Data[7] || kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyEnter)) && (!keyPressed || timer_Get(1) > 3000)) {
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
            if (kb_IsDown(kb_KeyEnter) || kb_IsDown(kb_Key2nd)) {
                if (cursorY == 156) {
                    if (cursorX == 63) {
                        isArchived = !isArchived;
                    } else if (cursorX == 139 && (fileType == BASIC_TYPE || fileType == ICE_SRC_TYPE)) {
                        isLocked = !isLocked;
                    } else if (cursorX == 202) {
                        isHidden = !isHidden;
                    }
                } else {
                    if (cursorX == 63) {
                        ti_Close(slot);
                        ti_DeleteVar(fileName, osFileType);
                        if (fileSelected >= fileNumbers[appvars] - 1) {
                            fileSelected--;
                        }
                        gfx_SetColor(colors[0]);
                        gfx_FillRectangle_NoClip(12, 28, 296, 164);
                        ui_DrawAllFiles(colors, fileSelected, fileNumbers[appvars] - 1, fileStartLoc, appvars);
                        gfx_BlitRectangle(gfx_buffer, 12, 28, 296, 10);
                        infoOps[0] = true;
                        return;
                    } else if (cursorX == 129) {
                        // Rename
                    } else {
                        // Edit
                    }
                }
                while(kb_AnyKey());
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
            menu_InfoRedraw(false, colors, cursorX, cursorY, isArchived, isLocked, isHidden, fileTypeString, fileName, fileSize, fileType, osFileType);
            gfx_BlitBuffer();
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
    if (initialValue[2] != isHidden) {
        hidePrgm(fileName, osFileType);
        ui_DrawAllFiles(colors, fileSelected, fileNumbers[appvars], fileStartLoc, appvars);
        gfx_BlitRectangle(gfx_buffer, 12, 28, 296, 10);
        if (isHidden) {
            fileName[0] -= 64;
        } else {
            fileName[0] += 64;
        }
    }
    if (initialValue[1] != isLocked) {
        lockPrgm(fileName, osFileType);
    }
    free (corner1);
    ti_Close(slot);
}

void menu_Settings(uint8_t color) { // Very empty right now
    shapes_RoundRectangleFill(color, 15, 304, 192, 8, 39);
    ui_DrawUISprite(color, UI_RARROW, 290, 208);
    gfx_SwapDraw();
    while (kb_AnyKey());
    while (!kb_IsDown(kb_KeyGraph) && !kb_IsDown(kb_KeyClear)) {
        kb_Scan();
    }
}
