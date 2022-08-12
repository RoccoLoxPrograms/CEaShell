#include "gfx/gfx.h"
#include "menu.h"
#include "shapes.h"
#include "ui.h"
#include "utility.h"
#include "asm/invert.h"
#include "asm/getPrgmType.h"

#include <graphx.h>
#include <keypadc.h>
#include <fileioc.h>
#include <sys/timers.h>
#include <sys/power.h>

static void menu_ThemePreview(uint8_t color, uint8_t *colors, const uint8_t *defaultThemes) {
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

uint8_t *menu_Looks(uint8_t *colors, uint8_t fileSelected, uint8_t fileCount, uint8_t fileStartLoc, bool is24Hour) {
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
    while (!kb_IsDown(kb_KeyYequ) && !kb_IsDown(kb_KeyClear)) {
        kb_Scan();
        if (!kb_AnyKey()) {
            keyPressed = false;
            timer_Set(1, 0);
        }
        if (kb_Data[7] && (!keyPressed || timer_Get(1) > 3000)) {
            prevCursorY = cursorY;
            prevCursorX = cursorX;
            pColor = color;
            if (kb_IsDown(kb_KeyRight) && cursorX == 128 && cursorY == 47) {	// Cursor looping
                cursorX = 16;
                cursorY = 75;
            } else if (kb_IsDown(kb_KeyLeft) && cursorX == 16 && cursorY == 75) {
            	cursorX = 128;
            	cursorY = 47;
            } else {
	    	cursorY = cursorY - 28 * (kb_IsDown(kb_KeyUp) && cursorY > 47) + 28 * (kb_IsDown(kb_KeyDown) && cursorY < 75);
            	cursorX = cursorX - 28 * (kb_IsDown(kb_KeyLeft) && cursorX > 16) + 28 * (kb_IsDown(kb_KeyRight) && cursorX < 128);
            }
            color = 3 * ((cursorX - 16) / 28) + 15 * (cursorY > 47);

            shapes_RoundRectangleFill(colors[0], 8, 26, 26, prevCursorX, prevCursorY);    // Erase old color
            shapes_RoundRectangleFill(defaultThemes[pColor], 6, 22, 22, prevCursorX + 2, prevCursorY + 2);

            shapes_RoundRectangleFill(colors[2], 8, 26, 26, cursorX, cursorY);
            shapes_RoundRectangleFill(defaultThemes[color], 6, 22, 22, cursorX + 2, cursorY + 2);

            if ((prevCursorX == 128 && prevCursorY == 75) || (cursorX == 128 && cursorY == 75)) {
                gfx_SetColor(148);
                gfx_Line_NoClip(132, 79, 149, 96);
                gfx_FloodFill(133, 78, 0);
            }
            
            menu_ThemePreview(color, colors, defaultThemes);
            gfx_BlitBuffer();
            if (!keyPressed) {
                while (timer_Get(1) < 9000 && kb_Data[7]) {
                    kb_Scan();
                }
            }
            keyPressed = true;
            timer_Set(1,0);
        }
        if (kb_IsDown(kb_KeyEnter) || kb_IsDown(kb_Key2nd)) {
            if (color == 27) {
                colors[3] = !colors[3];
                invertPalette();
            } else {
                colors[0] = defaultThemes[color + 1];
                colors[1] = defaultThemes[color];
                colors[2] = defaultThemes[color + 2];
            }
            gfx_FillScreen(colors[0]);
            ui_DrawAllFiles(colors, fileSelected, fileCount, fileStartLoc, false);
            menu_LooksRefresh(color, colors, defaultThemes, cursorX, cursorY);
            ui_StatusBar(colors[1], is24Hour, boot_GetBatteryStatus(), "Customize");
            gfx_BlitBuffer();
        }
    }
    gfx_SetTextFGColor(255 * !(colors[1] > 131 && colors[1] % 8 > 3));
    return colors;
}

void menu_Info(uint8_t *colors, uint8_t fileSelected, bool appvars) {
    uint8_t osFileType;
    uint8_t filesSearched = 0;
    char *fileName;
    void *vatPtr = NULL;
    while ((fileName = ti_DetectAny(&vatPtr, NULL, &osFileType))) {
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
    char *fileTypeString = util_FileTypeToString(fileType);
    uint8_t slot = ti_OpenVar(fileName, "r", osFileType);
    int fileSize = ti_GetSize(slot);
    uint8_t isArchived = ti_IsArchived(slot);
    uint8_t isProtected = (osFileType == OS_TYPE_PROT_PRGM);
    char *description = "This is just a dummy description"; // Replace this with description getting later
    bool hidden = (fileName[0] < 65);
    fileName[0] += 64 * (fileName[0] < 65);
    
    shapes_RoundRectangleFill(colors[1], 15, 220, 192, 50, 38);
    shapes_RoundRectangleFill(colors[2], 8, 138, 30, 56, 44);
    ui_DrawFile(false, false, colors, "", fileType, 200, 44);  // We don't draw a name here because it is drawn somewhere else
    // shapes_RoundCorners(true, colors[1], 8, 64, 64, 200, 44); (This isn't working and I don't know why)
    shapes_RoundRectangleFill(colors[0], 8, 138, 122, 56, 80);
    shapes_RoundRectangleFill(colors[0], 8, 82, 88, 182, 114);
    gfx_SetColor(colors[0]);
    gfx_SetPixel(193, 112);
    gfx_SetPixel(193, 113);
    gfx_SetPixel(194, 113);
    gfx_SetTextScale(2, 2);
    uint8_t nameX = 125 - gfx_GetStringWidth(fileName) / 2;
    gfx_PrintStringXY(fileName, nameX, 52);
    gfx_SetTextScale(1, 1);
    gfx_PrintStringXY("Type: ", 64, 90);
    gfx_PrintString(fileTypeString);
    gfx_PrintStringXY("Size: ", 64, 102);
    gfx_PrintInt(fileSize, 5);
    gfx_PrintStringXY("Description:", 64, 114);
    ui_DescriptionWrap(description, 64, 124);
    ui_DrawUISprite(colors[1], UI_DARROW, 152, 208);
    gfx_BlitBuffer();

    bool keyPressed = false;

    while (kb_AnyKey());
    while (!kb_IsDown(kb_KeyWindow) && !kb_IsDown(kb_KeyZoom) && !kb_IsDown(kb_KeyTrace) && !kb_IsDown(kb_KeyAlpha) && !kb_IsDown(kb_KeyClear)) {
        kb_Scan();
        if (!kb_AnyKey()) {
            keyPressed = false;
            timer_Set(1, 0);
        }
        if (kb_Data[7] && (!keyPressed || timer_Get(1) > 3000)) {
            // key stuff
            if (!keyPressed) {
                while (timer_Get(1) < 9000 && kb_Data[7]) {
                    kb_Scan();
                }
            }
            keyPressed = true;
            timer_Set(1,0);
        }
    }

    ti_Close(slot);
}

void menu_Settings(uint8_t color) {
    shapes_RoundRectangleFill(color, 15, 304, 192, 8, 39);
    ui_DrawUISprite(color, UI_RARROW, 290, 208);
    gfx_SwapDraw();
    while (kb_AnyKey());
    while (!kb_IsDown(kb_KeyGraph) && !kb_IsDown(kb_KeyClear)) {
        kb_Scan();
    }
}
