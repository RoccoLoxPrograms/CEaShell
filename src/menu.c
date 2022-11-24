/**
 * --------------------------------------
 * 
 * CEaShell Source Code - menu.c
 * By RoccoLox Programs and TIny_Hacker
 * Copyright 2022
 * License: GPL-3.0
 * 
 * --------------------------------------
**/

#include "gfx/gfx.h"
#include "menu.h"
#include "shapes.h"
#include "ui.h"
#include "utility.h"
#include "asm/invert.h"
#include "asm/fileOps.h"
#include "asm/sortVat.h"
#include "asm/hooks.h"
#include "asm/lowercase.h"
#include "asm/apps.h"

#include <graphx.h>
#include <keypadc.h>
#include <fileioc.h>
#include <sys/timers.h>
#include <sys/power.h>
#include <string.h>

#define ONE_SECOND      32768
#define ONE_MINUTE      (ONE_SECOND * 60)

static void menu_ThemePreview(const uint8_t color, uint8_t *colors, const uint8_t *defaultThemes) {   // Draws the theme preview box. Basically a bunch of rectangles
    if (color == 27) {
        shapes_RoundRectangleFill(255 - colors[0], 7, 134, 86, 168, 112);
        shapes_RoundRectangleFill(255 - colors[1], 6, 61, 67, 172, 116);
        shapes_RoundRectangleFill(255 - colors[2], 6, 61, 39, 237, 116);
    } else {
        shapes_RoundRectangleFill(defaultThemes[color + 1], 7, 134, 86, 168, 112);
        shapes_RoundRectangleFill(defaultThemes[color], 6, 61, 67, 172, 116);
        shapes_RoundRectangleFill(defaultThemes[color + 2], 6, 61, 39, 237, 116);
    }

    gfx_SetTextFGColor(255 * !(defaultThemes[color] > 131 && defaultThemes[color] % 8 > 3));
    gfx_SetTextScale(1, 1);
    gfx_PrintStringXY("Alpha: Return", 173, 186);
    gfx_SetTextFGColor(255 * !(defaultThemes[color] > 131 && defaultThemes[color] % 8 > 3));
}

static void menu_ColorPicker(uint8_t *colors, uint8_t *newColors, uint8_t apdTimer) {
    timer_Set(1, 0);
    gfx_SetColor(colors[1]);
    gfx_FillRectangle_NoClip(165, 96, 140, 19);
    gfx_SetTextScale(1, 1);
    shapes_RoundRectangleFill(colors[0], 8, 140, 92, 165, 109);
    bool redraw = true;
    bool keyPressed = false;
    uint8_t colorSelected = colors[0];
    uint8_t colorModifying = 0;
    while (kb_AnyKey());
    while (!kb_IsDown(kb_KeyYequ) && !kb_IsDown(kb_KeyClear) && !kb_IsDown(kb_KeyAlpha)) { // Key detection loop
        if ((timer_Get(1) >= ONE_MINUTE * apdTimer) && apdTimer) {
            gfx_End();
            triggerAPD();
        }
        kb_Scan();
        if (!kb_AnyKey() && keyPressed) {
            keyPressed = false;
            timer_Set(1, 0);
        }
        if ((kb_Data[7] || kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyEnter) || kb_IsDown(kb_KeyMode)) && (!keyPressed || timer_Get(1) > 3000)) {
            redraw = true;
            if (kb_IsDown(kb_KeyLeft)) {
                if (colorSelected != 0) {
                    colorSelected -= 1;
                } else {
                    colorSelected = 255;
                }
            } else if (kb_IsDown(kb_KeyRight)) {
                if (colorSelected != 255) {
                    colorSelected += 1;
                } else {
                    colorSelected = 0;
                }
            }
            if (kb_IsDown(kb_KeyUp)) {
                if (colorSelected > 32) {
                    colorSelected -= 32;
                } else {
                    colorSelected += 224;
                }
            } else if (kb_IsDown(kb_KeyDown)) {
                if (colorSelected < 225) {
                    colorSelected += 32;
                } else {
                    colorSelected -= 224;
                }
            }
            if (kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyEnter)) {
                newColors[colorModifying] = colorSelected;
                while (kb_AnyKey());
                if (colorModifying != 2) {
                    colorModifying++;
                } else {
                    break;
                }
                colorSelected = colors[colorModifying];
            }
            if (kb_IsDown(kb_KeyMode)) {
                while (kb_AnyKey());
                if (colorModifying < 2) {
                    colorModifying++;
                } else {
                    colorModifying = 0;
                }
                colorSelected = newColors[colorModifying];
            }
            if (!keyPressed) {
                while (timer_Get(1) < 9000 && kb_Data[7]) {
                    kb_Scan();
                }
            }
            keyPressed = true;
            timer_Set(1,0);
        }
        if (redraw) {
            redraw = false;
            shapes_RoundRectangleFill(colors[0], 8, 140, 56, 165, 46);
            gfx_ScaledSprite_NoClip(xlibc, 171, 52, 4, 4);
            gfx_SetColor(colors[0]);
            gfx_SetPixel(171, 52);
            gfx_SetPixel(171, 83);
            gfx_SetPixel(298, 52);
            gfx_SetPixel(298, 83);
            ui_MiniCursor(colors[2], 170 + ((colorSelected % 32) * 4), 51 + ((colorSelected / 32) * 4));
            if (colorModifying) {
                shapes_RoundRectangleFill(newColors[0], 7, 134, 86, 168, 112);
            } else {
                shapes_RoundRectangleFill(colorSelected, 7, 134, 86, 168, 112);
            }
            if (colorModifying != 1) {
                shapes_RoundRectangleFill(newColors[1], 6, 61, 67, 172, 116);
            } else {
                shapes_RoundRectangleFill(colorSelected, 6, 61, 67, 172, 116);
            }
            if (colorModifying != 2) {
                shapes_RoundRectangleFill(newColors[2], 6, 61, 39, 237, 116);
            } else {
                shapes_RoundRectangleFill(colorSelected, 6, 61, 39, 237, 116);
            }
            switch (colorModifying) {
                case 0:
                    gfx_PrintStringXY("BG color", 172, 87);
                    break;
                case 1:
                    gfx_PrintStringXY("Secondary color", 172, 87);
                    break;
                case 2:
                    gfx_PrintStringXY("Highlight color", 172, 87);
                    break;
                default:
                    break;
            }
            gfx_SetTextFGColor(255 * !(newColors[0] > 131 && newColors[0] % 8 > 3));
            gfx_PrintStringXY("Alpha: Return", 173, 186);
            gfx_SetTextFGColor(255 * !(colors[1] > 131 && colors[1] % 8 > 3));
            gfx_BlitBuffer();
        }
    }
}

static void menu_LooksRefresh(const uint8_t color, uint8_t *colors, const uint8_t *defaultThemes, const int cursorX, const uint8_t cursorY, const bool is24Hour, const uint8_t transitionSpeed, const bool displayCEaShell, const bool themePicker, const bool showApps, const bool showAppvars, const uint8_t option) {
    if (!themePicker) {
        shapes_RoundRectangleFill(colors[0], 8, 140, 155, 15, 46);
        if (option != 2 && option < 4) {
            shapes_RoundRectangleFill(colors[0], 8, 140, 84, 165, 46);
        } else {
            gfx_SetColor(colors[1]);
            gfx_FillRectangle_NoClip(165, 111, 140, 19);
            shapes_RoundRectangleFill(colors[0], 8, 140, 72, 165, 46);
        }
        gfx_SetTextScale(1, 1);
        gfx_PrintStringXY("About:", 171, 52);
        switch (option) {
            case 0:
                shapes_PixelIndentRectangle(colors[2], colors[0], 19, 50, 132, 11);
                gfx_PrintStringXY("Choose whether", 171, 69);
                gfx_PrintStringXY("the clock appears", 171, 81);
                gfx_PrintStringXY("using 24-Hour", 171, 93);
                gfx_PrintStringXY("time or using", 171, 105);
                gfx_PrintStringXY("12-Hour time.", 171, 117);
                break;
            case 1:
                shapes_PixelIndentRectangle(colors[2], colors[0], 19, 67, 132, 11);
                gfx_PrintStringXY("Turn on/off the", 171, 69);
                gfx_PrintStringXY("transition", 171, 81);
                gfx_PrintStringXY("animations when", 171, 93);
                gfx_PrintStringXY("opening or", 171, 105);
                gfx_PrintStringXY("closing menus.", 171, 117);
                break;
            case 2:
                shapes_PixelIndentRectangle(colors[2], colors[0], 19, 84, 132, 23);
                gfx_PrintStringXY("Choose the", 171, 69);
                gfx_PrintStringXY("animation speed", 171, 81);
                gfx_PrintStringXY("for transition", 171, 93);
                gfx_PrintStringXY("animations.", 171, 105);
                break;
            case 3:
                shapes_PixelIndentRectangle(colors[2], colors[0], 19, 113, 132, 23);
                gfx_PrintStringXY("Choose whether", 171, 69);
                gfx_PrintStringXY("to show CEaShell", 171, 81);
                gfx_PrintStringXY("in the list of", 171, 93);
                gfx_PrintStringXY("programs shown", 171, 105);
                gfx_PrintStringXY("in the shell.", 171, 117);
                break;
            case 4:
                shapes_PixelIndentRectangle(colors[2], colors[0], 19, 142, 132, 11);
                gfx_PrintStringXY("Create your own", 171, 69);
                gfx_PrintStringXY("color theme", 171, 81);
                gfx_PrintStringXY("for CEaShell to", 171, 93);
                gfx_PrintStringXY("use.", 171, 105);
                break;
            case 5:
                shapes_PixelIndentRectangle(colors[2], colors[0], 19, 159, 132, 11);
                gfx_PrintStringXY("Select a preset", 171, 69);
                gfx_PrintStringXY("color theme", 171, 81);
                gfx_PrintStringXY("for CEaShell to", 171, 93);
                gfx_PrintStringXY("use.", 171, 105);
                break;
            case 6:
                shapes_PixelIndentRectangle(colors[2], colors[0], 19, 176, 132, 23);
                gfx_PrintStringXY("Choose which (or", 171, 69);
                gfx_PrintStringXY("if) the special", 171, 81);
                gfx_PrintStringXY("directories are", 171, 93);
                gfx_PrintStringXY("displayed.", 171, 105);
            default:
                break;
        }

        gfx_SetTextScale(1, 1); // Customizing options
        gfx_PrintStringXY("Clock:", 21, 52);
        gfx_PrintStringXY("<", 85, 52);
        gfx_PrintStringXY(">", 144, 52);
        if (is24Hour) {
            gfx_PrintStringXY("24 Hour", 92, 52);
        } else {
            gfx_PrintStringXY("AM/PM", 97, 52);
        }
        gfx_PrintStringXY("Transitions:", 21, 69);
        gfx_PrintStringXY("<", 114, 69);
        gfx_PrintStringXY(">", 144, 69);
        gfx_PrintStringXY("Transition", 21, 86);
        gfx_PrintStringXY("Speed:", 21, 98);
        gfx_PrintStringXY("<", 93, 98);
        gfx_PrintStringXY(">", 144, 98);
        gfx_PrintStringXY("Hide", 21, 115);
        gfx_PrintStringXY("CEaShell:", 21, 127);
        gfx_PrintStringXY("<", 114, 127);
        gfx_PrintStringXY(">", 144, 127);
        gfx_PrintStringXY("Custom Theme", 21, 144);
        gfx_PrintStringXY("Preset Themes", 21, 161);
        gfx_PrintStringXY("Show", 21, 178);
        gfx_PrintStringXY("Folders:", 21, 190);
        gfx_PrintStringXY("<", 82, 190);
        gfx_PrintStringXY(">", 144, 190);
        if (transitionSpeed) {
            gfx_PrintStringXY("On", 124, 69);
            if (transitionSpeed == 1) {
                gfx_PrintStringXY("Slow", 107, 98);
            } else if (transitionSpeed == 2) {
                gfx_PrintStringXY("Normal", 99, 98);
            } else if (transitionSpeed == 3) {
                gfx_PrintStringXY("Fast", 106, 98);
            }
        } else {
            gfx_PrintStringXY("Off", 120, 69);
            gfx_PrintStringXY("Off", 109, 98);
        }
        if (!displayCEaShell) {
            gfx_PrintStringXY("On", 124, 127);
        } else {
            gfx_PrintStringXY("Off", 120, 127);
        }
        if (showApps && showAppvars) {
            gfx_PrintStringXY("Both", 100, 190);
        } else if (showApps) {
            gfx_PrintStringXY("Apps", 100, 190);
        } else if (showAppvars) {
            gfx_PrintStringXY("Appvars", 88, 190);
        } else {
            gfx_PrintStringXY("None", 100, 190);
        }
    } else {
        gfx_SetColor(colors[1]);
        gfx_FillRectangle_NoClip(165, 96, 140, 19);
        shapes_RoundRectangleFill(colors[0], 8, 140, 92, 165, 109);
        shapes_RoundRectangleFill(colors[0], 8, 140, 56, 165, 46);
        shapes_RoundRectangleFill(colors[2], 8, 26, 26, cursorX, cursorY);
        menu_ThemePreview(color, colors, defaultThemes);
        uint8_t drawBox = 0;    // Theme selector
        for (uint8_t y = 49; y < 78; y += 28) {
            for (int x = 168; x < 282; x += 28, drawBox += 3) {
                shapes_RoundRectangleFill(defaultThemes[drawBox], 6, 22, 22, x, y);
            }
        }
        gfx_TransparentSprite_NoClip(invSwitch, 280, 77);   // Invert color icon
    }
    
    ui_DrawUISprite(colors[1], UI_LARROW, 15, 208);
}

void menu_Looks(uint8_t *colors, unsigned int *fileSelected, const unsigned int fileCount, const unsigned int fileStartLoc, bool *is24Hour, uint8_t *transitionSpeed, const uint8_t directory, bool *displayCEaShell, const bool showHiddenProg, const bool showFileCount, const uint8_t apdTimer, bool *showApps, bool *showAppvars) {
    timer_Set(1, 0);
    const uint8_t defaultThemes[28] = {237, 246, 236, 74, 148, 0, 128, 137, 96, 226, 228, 162, 3, 100, 2, 28, 125, 58, 210, 243, 208, 81, 114, 48, 222, 255, 181, 222};
    shapes_RoundRectangleFill(colors[1], 8, 304, 192, 8, 39);   // Background
    menu_LooksRefresh(0, colors, defaultThemes, 166, 47, *is24Hour, *transitionSpeed, *displayCEaShell, false, *showApps, *showAppvars, 0);
    gfx_BlitBuffer();

    uint8_t color = 0;
    uint8_t cursorY = 47;
    uint8_t prevCursorY = 47;
    int cursorX = 166;
    int prevCursorX = 166;
    uint8_t pColor = 0;
    uint8_t option = 0;

    uint8_t batteryStatus = boot_GetBatteryStatus();

    bool themePicker = false;
    bool keyPressed = false;
    
    while (kb_AnyKey());
    while (!kb_IsDown(kb_KeyYequ) && !kb_IsDown(kb_KeyClear)) { // Key detection loop
        if ((timer_Get(1) >= ONE_MINUTE * apdTimer) && apdTimer) {
            gfx_End();
            triggerAPD();
        }
        kb_Scan();
        if (!kb_AnyKey() && keyPressed) {
            keyPressed = false;
            timer_Set(1, 0);
        }
        if (kb_AnyKey() && !keyPressed) {
            timer_Set(1, 0);
        }
        if ((kb_Data[7] || kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyEnter)) && (!keyPressed || timer_Get(1) > 3000)) {
            if (themePicker) {
                prevCursorY = cursorY;
                prevCursorX = cursorX;
                pColor = color;
                if (kb_IsDown(kb_KeyRight) && cursorX == 278) {	// Cursor looping
                    cursorX = 166;
                    cursorY = cursorY - 28 * (cursorY == 75) + 28 * (cursorY == 47);
                } else if (kb_IsDown(kb_KeyLeft) && cursorX == 166) {
                    cursorX = 278;
                    cursorY = cursorY - 28 * (cursorY == 75) + 28 * (cursorY == 47);
                } else {
	    	        cursorY = cursorY - 28 * (kb_IsDown(kb_KeyUp) && cursorY > 47) + 28 * (kb_IsDown(kb_KeyDown) && cursorY < 75);
                    cursorX = cursorX - 28 * (kb_IsDown(kb_KeyLeft) && cursorX > 166) + 28 * (kb_IsDown(kb_KeyRight) && cursorX < 278);
                }
                color = 3 * ((cursorX - 166) / 28) + 15 * (cursorY > 47);    // Change the selected color/theme

                shapes_RoundRectangleFill(colors[0], 8, 26, 26, prevCursorX, prevCursorY);    // Erase old color
                shapes_RoundRectangleFill(defaultThemes[pColor], 6, 22, 22, prevCursorX + 2, prevCursorY + 2);

                shapes_RoundRectangleFill(colors[2], 8, 26, 26, cursorX, cursorY);
                shapes_RoundRectangleFill(defaultThemes[color], 6, 22, 22, cursorX + 2, cursorY + 2);

                if ((prevCursorX == 278 && prevCursorY == 75) || (cursorX == 278 && cursorY == 75)) {
                    gfx_TransparentSprite_NoClip(invSwitch, 280, 77);
                }
            } else {
                if (kb_IsDown(kb_KeyDown)) {
                    if (option < 6) {
                        option++;
                    } else {
                        option = 0;
                    }
                } else if (kb_IsDown(kb_KeyUp)) {
                    if (option) {
                        option--;
                    } else {
                        option = 6;
                    }
                } else if (kb_IsDown(kb_KeyLeft) || kb_IsDown(kb_KeyRight)) {
                    switch (option) {
                        case 0:
                            *is24Hour = !*is24Hour;
                            ui_StatusBar(colors[1], *is24Hour, batteryStatus, "Customize", fileCount, showFileCount);
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
                        case 6:
                            if (kb_IsDown(kb_KeyRight)) {
                                if (*showApps && *showAppvars) {  // Both
                                    *showAppvars = false;
                                } else if (*showApps) {  // Apps
                                    *showApps = false;
                                    *showAppvars = true;
                                } else if (*showAppvars) {   // Appvars
                                    *showAppvars = false;
                                } else {    // Neither
                                    *showApps = true;
                                    *showAppvars = true;
                                }
                            } else {
                                if (*showApps && *showAppvars) {  // Both
                                    *showApps = false;
                                    *showAppvars = false;
                                } else if (*showApps) {  // Apps
                                    *showAppvars = true;
                                } else if (*showAppvars) {   // Appvars
                                    *showApps = true;
                                    *showAppvars = false;
                                } else {    // Neither
                                    *showAppvars = true;
                                }
                            }
                        default:
                            break;
                    }
                } else if (kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyEnter)) {
                    if (option == 4) {
                        uint8_t newColors[3];
                        newColors[0] = colors[0];
                        newColors[1] = colors[1];
                        newColors[2] = colors[2];
                        menu_ColorPicker(colors, newColors, apdTimer);
                        colors[0] = newColors[0];
                        colors[1] = newColors[1];
                        colors[2] = newColors[2];
                        gfx_FillScreen(colors[0]);
                        ui_DrawAllFiles(colors, *fileSelected, fileCount, fileStartLoc, directory, displayCEaShell, showHiddenProg, *showApps, *showAppvars);
                        shapes_RoundRectangleFill(colors[1], 8, 304, 192, 8, 39);
                        gfx_SetColor(colors[1]);
                        gfx_FillRectangle_NoClip(165, 130, 140, 82);
                        menu_LooksRefresh(color, colors, defaultThemes, cursorX, cursorY, *is24Hour, *transitionSpeed, *displayCEaShell, themePicker, *showApps, *showAppvars, option);
                        ui_StatusBar(colors[1], *is24Hour, batteryStatus, "Customize", fileCount, showFileCount);
                        gfx_BlitBuffer();
                    } else if (option == 5) {
                        themePicker = true;
                    }
                    if (!kb_IsDown(kb_KeyClear) && !kb_IsDown(kb_KeyYequ)) {
                        while (kb_AnyKey());
                    }
                }
                if (themePicker) {
                    menu_ThemePreview(color, colors, defaultThemes);    // Refresh the preview if a different theme is selected
                }
            }
            menu_LooksRefresh(color, colors, defaultThemes, cursorX, cursorY, *is24Hour, *transitionSpeed, *displayCEaShell, themePicker, *showApps, *showAppvars, option);

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
            ui_DrawAllFiles(colors, *fileSelected, fileCount, fileStartLoc, directory, displayCEaShell, showHiddenProg, *showApps, *showAppvars);
            shapes_RoundRectangleFill(colors[1], 8, 304, 192, 8, 39);
            gfx_SetColor(colors[1]);
            gfx_FillRectangle_NoClip(165, 130, 140, 82);
            menu_LooksRefresh(color, colors, defaultThemes, cursorX, cursorY, *is24Hour, *transitionSpeed, *displayCEaShell, themePicker, *showApps, *showAppvars, option);
            ui_StatusBar(colors[1], *is24Hour, batteryStatus, "Customize", fileCount, showFileCount);
            gfx_BlitBuffer();
        } else if (kb_IsDown(kb_KeyAlpha) && themePicker) {
            themePicker = false;
            menu_LooksRefresh(color, colors, defaultThemes, cursorX, cursorY, *is24Hour, *transitionSpeed, *displayCEaShell, themePicker, *showApps, *showAppvars, option);
            gfx_SetColor(colors[1]);
            gfx_FillRectangle_NoClip(165, 130, 140, 82);
            gfx_BlitBuffer();
        }
    }
    gfx_SetTextFGColor(255 * !(colors[1] > 131 && colors[1] % 8 > 3));
}

static void menu_InfoRedraw(const bool fullRedraw, const bool drawCursor, uint8_t *colors, const int cursorX, const uint8_t cursorY, const bool isArchived, const bool isLocked, const bool isHidden, const char *fileTypeString, char *fileName, const int fileSize, const uint8_t fileType, const uint8_t osFileType, const uint8_t directory) {
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

        if (directory == APPS_FOLDER) {
            char *appDescription = getAppCopyrightInfo(fileName);
            ui_DescriptionWrap(appDescription, 27, 61, 121);
        } else {
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
            free(description);
        }
        free(corner1);
        gfx_SetColor(colors[0]);
        gfx_SetTextScale(2, 2);
        const uint8_t nameX = 125 - gfx_GetStringWidth(fileName) / 2;
        gfx_PrintStringXY(fileName, nameX, 52);
        gfx_SetTextScale(1, 1);
        gfx_PrintStringXY("Type: ", 61, 85);
        gfx_PrintString(fileTypeString);
        gfx_PrintStringXY("Size: ", 61, 98);
        gfx_SetTextXY(99, 98);
        gfx_PrintInt(fileSize, 5 + (directory == APPS_FOLDER)); // Everything but apps will max at 65535, so we only use 6 digits in the apps folder
        if (directory == APPS_FOLDER) {
            gfx_PrintStringXY("Copyright:", 61, 111);
            gfx_PrintStringXY("Minimum OS Version:", 61, 145);
        } else {
            gfx_PrintStringXY("Description:", 61, 111);
            gfx_PrintStringXY("Attributes:", 61, 145);
        }
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
    if (directory == APPS_FOLDER) {
        gfx_PrintStringXY(getAppMinOSVersion(fileName), 61, 156);
    } else {
        ui_CheckBox(colors[1], gfx_GetPixel(64, 157), isArchived, 64, 157);
        gfx_PrintStringXY("Archived", 74, 157);
        ui_CheckBox(colors[1], gfx_GetPixel(140, 157), isLocked, 140, 157);
        gfx_PrintStringXY("Locked", 150, 157);
        ui_CheckBox(colors[1], gfx_GetPixel(203, 157), isHidden, 203, 157);
        gfx_PrintStringXY("Hidden", 213, 157);
    }
    gfx_PrintStringXY("Delete", 71, 184);
    if (directory != APPS_FOLDER) {
        gfx_PrintStringXY("Rename", 136, 184);
        gfx_PrintStringXY("Edit", 213, 184);
    }
}

void menu_Info(uint8_t *colors, bool *infoOps, unsigned int fileSelected, const unsigned int fileStartLoc, unsigned int *fileNumbers, const uint8_t directory, const bool displayCEaShell, const bool editLockedProg, const bool showHiddenProg, const uint8_t apdTimer, const bool showApps, const bool showAppvars) {
    timer_Set(1, 0);
    uint8_t osFileType = '\0'; // Different from C, ICE, ASM, etc. This is stuff like OS_TYPE_APPVAR and OS_TYPE_PRGM
    unsigned int filesSearched = 1;
    if (directory == PROGRAMS_FOLDER) {
        filesSearched = (showApps + showAppvars);
    }
    char newName[9] = "\0";
    char appName[9] = "\0";
    unsigned int appPointer;
    char *fileName = NULL;
    void *vatPtr = NULL;
    if (directory == APPS_FOLDER) {
        while (detectApp(appName, &appPointer)) {
            if (!displayCEaShell && !strcmp(appName, "CEaShell")) {
                continue;
            }
            if (fileSelected + 1 == filesSearched) {
                fileName = appName;
                osFileType = OS_TYPE_APP;
                break;
            }
            filesSearched++;
        }
    } else {
        while ((fileName = ti_DetectAny(&vatPtr, NULL, &osFileType))) { // Suspiciously similar to the example in the docs :P
            if (*fileName == '!' || *fileName == '#') {
                continue;
            }
            if (!showHiddenProg && fileName[0] < 65) {
                continue;
            }
            if ((osFileType == OS_TYPE_PRGM || osFileType == OS_TYPE_PROT_PRGM) && getPrgmType(fileName, osFileType) == HIDDEN_TYPE) {
                continue;
            }
            if (directory == APPVARS_FOLDER && osFileType == OS_TYPE_APPVAR) {
                if (fileSelected + 1 == filesSearched) {
                    break;
                }
                filesSearched++;
            } else if (directory == PROGRAMS_FOLDER && (osFileType == OS_TYPE_PRGM || osFileType == OS_TYPE_PROT_PRGM)) {
                if (fileSelected + 1 == filesSearched) {
                    break;
                }
                filesSearched++;
            }
        }
    }
    uint8_t fileType;
    uint8_t slot;
    bool isArchived;
    bool isLocked;
    bool isHidden;
    unsigned int fileSize = 0;
    if (osFileType == OS_TYPE_APP) {
        fileType = APP_TYPE;
        fileSize = getAppSize(appName);
        isArchived = true;
        isLocked = true;
        isHidden = false;
    } else {
        if (osFileType == OS_TYPE_APPVAR) {
            fileType = getAppvarType(fileName);
        } else {
            fileType = getPrgmType(fileName, osFileType);
        }
        slot = ti_OpenVar(fileName, "r", osFileType);
        fileSize = getProgSize(fileName, osFileType);
        isArchived = ti_IsArchived(slot);
        if (osFileType == OS_TYPE_PROT_PRGM) {
            isLocked = true;
        } else if (osFileType == OS_TYPE_APPVAR) {
            if (getAppvarType(fileName) == APPVAR_TYPE) {
                isLocked = true;
            } else {
                isLocked = false;
            }
        } else {
            isLocked = false;
        }
        isHidden = (fileName[0] < 65);
        fileName[0] += 64 * (fileName[0] < 65);
    }
    char *fileTypeString = util_FileTypeToString(fileType, false);
    const bool initialValue[3] = {isArchived, isLocked, isHidden};

    int cursorX = 63;
    uint8_t cursorY;
    if (directory == APPS_FOLDER) { // Only delete option for Apps folder
        cursorY = 182;
    } else {
        cursorY = 156;
    }
    menu_InfoRedraw(true, true, colors, cursorX, cursorY, isArchived, isLocked, isHidden, fileTypeString, fileName, fileSize, fileType, osFileType, directory);
    gfx_BlitBuffer();

    bool keyPressed = false;

    fileName[0] -= 64 * initialValue[2];    // The viewable file name becomes what the OS sees instead
    while (kb_AnyKey());
    while (!kb_IsDown(kb_KeyWindow) && !kb_IsDown(kb_KeyZoom) && !kb_IsDown(kb_KeyTrace) && !kb_IsDown(kb_KeyAlpha) && !kb_IsDown(kb_KeyClear)) {
        if ((timer_Get(1) >= ONE_MINUTE * apdTimer) && apdTimer) {
            gfx_End();
            triggerAPD();
        }
        kb_Scan();
        if (!kb_AnyKey() && keyPressed) {
            keyPressed = false;
            timer_Set(1, 0);
        }
        if (kb_AnyKey() && !keyPressed) {
            timer_Set(1, 0);
        }
        if ((kb_Data[7] || kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyEnter) || kb_IsDown(kb_KeyDel)) && (!keyPressed || timer_Get(1) > 3000)) {
            if (directory != APPS_FOLDER) { // Only delete apps
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
            }
            if (kb_IsDown(kb_KeyEnter) || kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyDel)) {
                if (cursorY == 156 && !kb_IsDown(kb_KeyDel)) {
                    if (cursorX == 63) {
                        isArchived = !isArchived;
                    } else if (cursorX == 139 && (fileType == BASIC_TYPE || fileType == ICE_SRC_TYPE) && directory == PROGRAMS_FOLDER) {
                        isLocked = !isLocked;
                    } else if (cursorX == 202 && directory == PROGRAMS_FOLDER) {
                        isHidden = !isHidden;
                    }
                } else {
                    if (((kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyEnter)) && cursorX == 63) || kb_IsDown(kb_KeyDel)) {
                        menu_InfoRedraw(false, false, colors, cursorX, cursorY, isArchived, isLocked, isHidden, fileTypeString, fileName, fileSize, fileType, osFileType, directory);
                        if (ui_DeleteConf(colors, 56, 206)) {
                            ti_Close(slot);
                            if (directory == APPS_FOLDER) {
                                // DO NOT DELETE APPS UNTIL THE ISSUES ARE FIXED
                                // deleteApp(fileName);
                                break;
                            } else {
                                ti_DeleteVar(fileName, osFileType);
                            }
                            if (fileSelected >= fileNumbers[directory] - 1) {
                                fileSelected--;
                            }
                            gfx_SetColor(colors[0]);
                            gfx_FillRectangle_NoClip(12, 28, 296, 164);
                            ui_DrawAllFiles(colors, fileSelected, fileNumbers[directory] - 1, fileStartLoc, directory, displayCEaShell, showHiddenProg, showApps, showAppvars);
                            gfx_BlitRectangle(gfx_buffer, 12, 28, 296, 10);
                            infoOps[0] = true;
                            return;
                        }
                        menu_InfoRedraw(true, true, colors, cursorX, cursorY, isArchived, isLocked, isHidden, fileTypeString, fileName, fileSize, fileType, osFileType, directory);
                        gfx_BlitBuffer();
                    } else if (cursorX == 129) {
                        memcpy(newName, fileName, strlen(fileName) + 1);   // Copy name into string to modify
                        newName[0] += 64 * (newName[0] < 65);
                        menu_InfoRedraw(true, false, colors, cursorX, cursorY, isArchived, isLocked, isHidden, fileTypeString, newName, fileSize, fileType, osFileType, directory);
                        if (ui_RenameBox(colors, newName)) {
                            ti_Close(slot);
                            ti_RenameVar(fileName, newName, osFileType);
                            sortVAT();
                            memcpy(fileName, newName, strlen(newName) + 1);
                            ti_OpenVar(fileName, "r", osFileType);
                        }
                        menu_InfoRedraw(true, true, colors, cursorX, cursorY, isArchived, isLocked, isHidden, fileTypeString, fileName, fileSize, fileType, osFileType, directory);
                        gfx_BlitBuffer();
                        kb_Scan();
                        if (kb_IsDown(kb_KeyClear)) {
                            continue;
                        }
                    } else {
                        while (kb_AnyKey());
                        if (fileType != BASIC_TYPE && fileType != ICE_SRC_TYPE) {
                            if (getAppvarType(fileName) == CELTIC_TYPE) {
                                gfx_End();
                                editCelticAppvar(fileName);
                            } else {
                                break;
                            }
                        } else {
                            if (osFileType == OS_TYPE_PROT_PRGM && editLockedProg) {
                                unlockBasic(fileName, osFileType);
                            } else if (osFileType == OS_TYPE_PROT_PRGM) {
                                break;
                            }
                            gfx_End();
                            editBasicProg(fileName, osFileType);
                        }
                    }
                }
                while (kb_AnyKey());
            }
            menu_InfoRedraw(false, true, colors, cursorX, cursorY, isArchived, isLocked, isHidden, fileTypeString, fileName, fileSize, fileType, osFileType, directory);
            gfx_SwapDraw();
            if (!keyPressed) {
                while (timer_Get(1) < 9000 && kb_Data[7]) {
                    kb_Scan();
                }
            }
            keyPressed = true;
            timer_Set(1,0);
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
        ui_DrawAllFiles(colors, fileSelected, fileNumbers[directory], fileStartLoc, directory, displayCEaShell, showHiddenProg, showApps, showAppvars);
        gfx_BlitRectangle(gfx_buffer, 12, 28, 296, 10);
        if (isHidden) {
            fileName[0] -= 64;
        } else {
            fileName[0] += 64;
        }
    }
    ti_Close(slot);
}

static void menu_SettingsRedraw(uint8_t *colors, const uint8_t option, const uint8_t getCSCHook, const bool editArchivedProg, const bool editLockedProg, const bool showHiddenProg, const bool showFileCount, const bool hideBusyIndicator, const bool lowercase, const uint8_t startOption, const uint8_t apdTimer) {
    shapes_RoundRectangleFill(colors[0], 8, 140, 155, 15, 46);
    shapes_RoundRectangleFill(colors[1], 8, 140, 155, 165, 46);
    if (option == 0 || option == 8) {  // big about box
        shapes_RoundRectangleFill(colors[0], 8, 140, 84, 165, 46);
    } else if (option == 1) {
        shapes_RoundRectangleFill(colors[0], 8, 140, 155, 165, 46);
    } else {    // small about box
        shapes_RoundRectangleFill(colors[0], 8, 140, 72, 165, 46);
    }
    gfx_SetTextScale(1, 1);
    gfx_PrintStringXY("About:", 171, 52);
    gfx_SetColor(colors[1]);
    switch (startOption) {
        case 0:
            gfx_FillRectangle_NoClip(16, 54, 2, 109);
            break;
        case 1:
            gfx_FillRectangle_NoClip(16, 64, 2, 109);
            break;
        case 2:
            gfx_FillRectangle_NoClip(16, 74, 2, 109);
            break;
        case 3:
            gfx_FillRectangle_NoClip(16, 84, 2, 109);
            break;
        default:
            break;
    }
    switch (option) {
        case 0:
            shapes_PixelIndentRectangle(colors[2], colors[0], 19, 50, 132, 11);
            gfx_PrintStringXY("Choose whether", 171, 69);
            gfx_PrintStringXY("to display program", 171, 81);
            gfx_PrintStringXY("icons and", 171, 93);
            gfx_PrintStringXY("descriptions in", 171, 105);
            gfx_PrintStringXY("the OS [prgm] menu.", 171, 117);
            break;
        case 1:
            shapes_PixelIndentRectangle(colors[2], colors[0], 19, 67 - (17 * (startOption == 1)), 132, 23);
            gfx_PrintStringXY("Enable keyboard", 171, 69);
            gfx_PrintStringXY("shortcuts.", 171, 81);
            gfx_PrintStringXY("[on] + [prgm]:", 171, 93);
            gfx_PrintStringXY("Launch CEaShell.", 171, 105);
            gfx_PrintStringXY("[on] + [sto]/[ln]:", 171, 117);
            gfx_PrintStringXY("Enable/Disable OS", 171, 129);
            gfx_PrintStringXY("Dark mode.", 171, 141);
            gfx_PrintStringXY("[on] + [stat]:", 171, 153);
            gfx_PrintStringXY("Trigger APD.", 171, 165);
            gfx_PrintStringXY("[on] + [graph]:", 171, 177);
            gfx_PrintStringXY("Jump to lbl in prgms.", 171, 189);
            break;
        case 2:
            shapes_PixelIndentRectangle(colors[2], colors[0], 19, 96 - (17 * (startOption == 1)) - (46 * (startOption == 2)), 132, 23);
            gfx_PrintStringXY("Allow the editing", 171, 69);
            gfx_PrintStringXY("of archived", 171, 81);
            gfx_PrintStringXY("programs in the OS", 171, 93);
            gfx_PrintStringXY("[prgm] edit menu.", 171, 105);
            break;
        case 3:
            shapes_PixelIndentRectangle(colors[2], colors[0], 19, 125 - (17 * (startOption == 1)) - (46 * (startOption == 2)) - (75 * (startOption == 3)), 132, 23);
            gfx_PrintStringXY("Allow the editing", 171, 69);
            gfx_PrintStringXY("of locked BASIC", 171, 81);
            gfx_PrintStringXY("programs in", 171, 93);
            gfx_PrintStringXY("CEaShell.", 171, 105);
            break;
        case 4:
            shapes_PixelIndentRectangle(colors[2], colors[0], 19, 154 - (17 * (startOption == 1)) - (46 * (startOption == 2)) - (75 * (startOption == 3)), 132, 23);
            gfx_PrintStringXY("Display hidden", 171, 69);
            gfx_PrintStringXY("programs in the", 171, 81);
            gfx_PrintStringXY("CEaShell program", 171, 93);
            gfx_PrintStringXY("list.", 171, 105);
            break;
        case 5:
            shapes_PixelIndentRectangle(colors[2], colors[0], 19, 183 - (17 * (startOption == 1)) - (46 * (startOption == 2)) - (75 * (startOption == 3)), 132, 11);
            gfx_PrintStringXY("Display the", 171, 69);
            gfx_PrintStringXY("number of files", 171, 81);
            gfx_PrintStringXY("in the current", 171, 93);
            gfx_PrintStringXY("directory.", 171, 105);
            break;
        case 6:
            shapes_PixelIndentRectangle(colors[2], colors[0], 19, 183 - (29 * (startOption == 2)) - (58 * (startOption == 3)), 132, 11);
            gfx_PrintStringXY("Enable lowercase", 171, 69);
            gfx_PrintStringXY("in TI-OS", 171, 81);
            gfx_PrintStringXY("by pressing", 171, 93);
            gfx_PrintStringXY("[alpha] twice.", 171, 105);
            break;
        case 7:
            shapes_PixelIndentRectangle(colors[2], colors[0], 19, 171 - (29 * (startOption == 3)), 132, 23);
            gfx_PrintStringXY("Disable busy", 171, 69);
            gfx_PrintStringXY("indicator in", 171, 81);
            gfx_PrintStringXY("TI-BASIC", 171, 93);
            gfx_PrintStringXY("programs.", 171, 105);
            break;
        case 8:
            shapes_PixelIndentRectangle(colors[2], colors[0], 19, 171, 132, 11);
            gfx_PrintStringXY("Automatically turn", 171, 69);
            gfx_PrintStringXY("off the calculator", 171, 81);
            gfx_PrintStringXY("after a certain", 171, 93);
            gfx_PrintStringXY("number of minutes", 171, 105);
            gfx_PrintStringXY("in CEaShell.", 171, 117);
            break;
        case 9:
            shapes_PixelIndentRectangle(colors[2], colors[0], 19, 188, 132, 11);
            gfx_PrintStringXY("Learn about the", 171, 69);
            gfx_PrintStringXY("people who played", 171, 81);
            gfx_PrintStringXY("a role in creating", 171, 93);
            gfx_PrintStringXY("CEaShell!", 171, 105);
            break;
        default:
            break;
    }
    gfx_SetColor(colors[0]);
    if (startOption == 0) {
        ui_DrawMenuItem("Icon Hook", 21, 52, (getCSCHook == BOTH || getCSCHook == ICON_HOOK));
    }
    if (startOption < 2) {
        ui_DrawDoubleMenuItem("[on] key", "Shortcuts", 21, 69 - (17 * (startOption == 1)), (getCSCHook == BOTH || getCSCHook == ON_SHORTS_HOOK));
    }
    if (startOption < 3) {
        ui_DrawDoubleMenuItem("Edit archived", "Programs", 21, 98 - (17 * (startOption == 1)) - (46 * (startOption == 2)), editArchivedProg);
    }
    ui_DrawDoubleMenuItem("Edit locked", "Programs", 21, 127 - (17 * (startOption == 1)) - (46 * (startOption == 2)) - (75 * (startOption == 3)), editLockedProg);
    ui_DrawDoubleMenuItem("Show hidden", "Programs", 21, 156 - (17 * (startOption == 1)) - (46 * (startOption == 2)) - (75 * (startOption == 3)), showHiddenProg);
    ui_DrawMenuItem("File count", 21, 185 - (17 * (startOption == 1)) - (46 * (startOption == 2)) - (75 * (startOption == 3)), showFileCount);
    if (startOption) {
        ui_DrawMenuItem("Lowercase", 21, 185 - (29 * (startOption == 2)) - (58 * (startOption == 3)), lowercase);
    }
    if (startOption > 1) {
        ui_DrawDoubleMenuItem("Disable busy", "Indicator", 21, 173 - (29 * (startOption == 3)), hideBusyIndicator);
    }
    if (startOption == 3) {
        gfx_PrintStringXY("APD timer", 21, 173);
        gfx_PrintStringXY("<", 113, 173);
        if (!apdTimer) {
            gfx_PrintStringXY("Off", 120, 173);
        } else {
            gfx_SetTextXY(127, 173);
            gfx_PrintUInt(apdTimer, 0);
        }
        gfx_PrintStringXY(">", 143, 173);
        gfx_PrintStringXY("About", 21, 190);
    }
}

void menu_Settings(uint8_t *colors, uint8_t *getCSCHook, bool *editArchivedProg, bool *editLockedProg, bool *showHiddenProg, bool *showFileCount, bool *hideBusyIndicator, bool *lowercase, uint8_t *apdTimer) {    // Add more options later
    timer_Set(1, 0);
    shapes_RoundRectangleFill(colors[1], 8, 304, 192, 8, 39);
    ui_DrawUISprite(colors[1], UI_RARROW, 290, 208);
    uint8_t option = 0;
    uint8_t startOption = 0;
    menu_SettingsRedraw(colors, option, *getCSCHook, *editArchivedProg, *editLockedProg, *showHiddenProg, *showFileCount, *hideBusyIndicator, *lowercase, startOption, *apdTimer);
    gfx_BlitBuffer();
    bool keyPressed = false;
    while (kb_AnyKey());
    while (!kb_IsDown(kb_KeyGraph) && !kb_IsDown(kb_KeyClear)) {
        if ((timer_Get(1) >= ONE_MINUTE * *apdTimer) && *apdTimer) {
            gfx_End();
            triggerAPD();
        }
        kb_Scan();
        if (!kb_AnyKey() && keyPressed) {
            keyPressed = false;
            timer_Set(1, 0);
        }
        if (kb_AnyKey() && !keyPressed) {
            timer_Set(1, 0);
        }
        if (kb_Data[7] && (!keyPressed || timer_Get(1) > 3000)) {
            if (kb_IsDown(kb_KeyLeft) || kb_IsDown(kb_KeyRight)) {
                switch (option) {
                    case 0:
                        if (*getCSCHook == BOTH) {  // Turning off
                            *getCSCHook = ON_SHORTS_HOOK;
                        } else if (*getCSCHook == ICON_HOOK) {
                            *getCSCHook = NONE;
                        } else if (*getCSCHook == ON_SHORTS_HOOK) { // Turning on
                            *getCSCHook = BOTH;
                        } else if (*getCSCHook == NONE) {  
                            *getCSCHook = ICON_HOOK;
                        }
                        if (*getCSCHook) {
                            installGetCSCHook(*getCSCHook);
                        } else {
                            removeGetCSCHook();
                        }
                        break;
                    case 1:
                        if (*getCSCHook == BOTH) {  // Turning off
                            *getCSCHook = ICON_HOOK;
                        } else if (*getCSCHook == ICON_HOOK) {
                            *getCSCHook = BOTH;
                        } else if (*getCSCHook == ON_SHORTS_HOOK) { // Turning on
                            *getCSCHook = NONE;
                        } else if (*getCSCHook == NONE) {  
                            *getCSCHook = ON_SHORTS_HOOK;
                        }
                        if (*getCSCHook) {
                            installGetCSCHook(*getCSCHook);
                        } else {
                            removeGetCSCHook();
                        }
                        break;
                    case 2:
                        *editArchivedProg = !*editArchivedProg;
                        if (*editArchivedProg) {
                            installMenuHook();
                            installHomescreenHook();
                        } else {
                            if (checkMenuHookInstalled()) {
                                removeMenuHook();
                            }
                            if (checkHomescreenHookInstalled()) {
                                removeHomescreenHook();
                            }
                        }
                        break;
                    case 3:
                        *editLockedProg = !*editLockedProg;
                        break;
                    case 4:
                        *showHiddenProg = !*showHiddenProg;
                        break;
                    case 5:
                        *showFileCount = !*showFileCount;
                        break;
                    case 6:
                        *lowercase = !*lowercase;
                        toggleLowercase(*lowercase);
                        break;
                    case 7:
                        *hideBusyIndicator = !*hideBusyIndicator;
                        break;
                    case 8:
                        if (kb_IsDown(kb_KeyLeft)) {
                            if (*apdTimer > 0) {
                                *apdTimer = *apdTimer - 1;
                            } else {
                                *apdTimer = 5;
                            }
                        } else if (kb_IsDown(kb_KeyRight)) {
                            if (*apdTimer < 5) {
                                *apdTimer = *apdTimer + 1;
                            } else {
                                *apdTimer = 0;
                            }
                        }
                        break;
                    default:
                        break;
                }
            }
            if (kb_IsDown(kb_KeyDown)) {
                if (option < 5) {
                    option++;
                } else {
                    if ((startOption == 0 && option == 5)  || (startOption == 1 && option == 6) || (startOption == 2 && option == 7)) {
                        startOption++;
                        option++;
                    } else if (option == 9) { // Restart menu
                        startOption = 0;
                        option = 0;
                    } else {
                        option++;
                    }
                }
            } else if (kb_IsDown(kb_KeyUp)) {
                if (option > 3) {
                    option -= 1;
                } else {
                    if (startOption == option && option == 0) {
                        startOption = 3;
                        option = 9;
                    } else if (startOption == option) {
                        startOption -= 1;
                        option -= 1;
                    } else {
                        option -= 1;
                    }
                }
            }
            menu_SettingsRedraw(colors, option, *getCSCHook, *editArchivedProg, *editLockedProg, *showHiddenProg, *showFileCount, *hideBusyIndicator, *lowercase, startOption, *apdTimer);
            gfx_BlitBuffer();
            if (!keyPressed) {
                while (timer_Get(1) < 9000 && kb_Data[7]) {
                    kb_Scan();
                }
            }
            keyPressed = true;
            timer_Set(1,0);
        }
        if (kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyEnter)) {
            if (option == 9) {
                ui_AboutScreen(colors);
                if (kb_IsDown(kb_KeyAlpha)) {
                    while (kb_AnyKey());
                    gfx_SetColor(colors[1]);
                    gfx_FillRectangle_NoClip(149, 46, 22, 155);
                    menu_SettingsRedraw(colors, option, *getCSCHook, *editArchivedProg, *editLockedProg, *showHiddenProg, *showFileCount, *hideBusyIndicator, *lowercase, startOption, *apdTimer);
                    gfx_BlitBuffer();
                }
            }
        }
    }
}
