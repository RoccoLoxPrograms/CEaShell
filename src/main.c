#include "ui.h"
#include "shapes.h"
#include "menu.h"
#include "utility.h"
#include "asm/invert.h"
#include "asm/sortVat.h"

#include <graphx.h>
#include <keypadc.h>
#include <fileioc.h>

gfx_UninitedSprite(buffer1, 152, 193);  // These preserve the background to make redrawing faster
gfx_UninitedSprite(buffer2, 152, 193);

int main(void) {
    uint8_t colors[4] = {246, 237, 236, 0};    // If the appvar contains no theme it defaults to these settings
    uint8_t transitionSpeed = 2;    // 1 is slow, 2 is normal, 3 is fast, and 0 has no transitions
    bool is24Hour = true;

    bool redraw = false;

    ti_var_t slot = ti_Open("CEaShell", "r");
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
    uint8_t cursor[8] = {1, 0, 0, 0, 0, 0, 0, 0};    // Which of the slots the cursor is selecting
    
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
    ui_StatusBar(colors[1], is24Hour, "");  // Displays bar with program name, clock, and battery
    ui_BottomBar(colors[1], "By TIny_Hacker + RoccoLox Programs");
    ui_DrawAllFiles(colors, cursor, NOPROGS, false);
    gfx_BlitBuffer();

    while(!kb_IsDown(kb_KeyClear)) {    // Looks menu
        kb_Scan();
        if (kb_IsDown(kb_KeyYequ)) {
            ui_StatusBar(colors[1], is24Hour, "Customize");
            gfx_BlitBuffer();
            if (transitionSpeed) {
                gfx_GetSprite_NoClip(buffer1, 8, 38);   // For redrawing the background
                gfx_GetSprite_NoClip(buffer2, 160, 38);
                for (int8_t frame = 3; frame < 16 / transitionSpeed; frame++) {
                    shapes_RoundRectangleFill(colors[1], 15, frame * (19 * transitionSpeed), frame * (12 * transitionSpeed), 8, 231 - frame * (12 * transitionSpeed));
                    gfx_SwapDraw();
                }
            }
            uint8_t *newColors = menu_Looks(colors, is24Hour);
            for (uint8_t byte = 0; byte < 4; byte++) {
                colors[byte] = newColors[byte];
            }
            gfx_FillScreen(colors[0]);
            ui_StatusBar(colors[1], is24Hour, "Customize");
            ui_BottomBar(colors[1], "By TIny_Hacker + RoccoLox Programs");
            if (transitionSpeed) {
                for (uint8_t frame = 16 / transitionSpeed; frame > 2; frame--) {
                    gfx_Sprite_NoClip(buffer1, 8, 38);
                    gfx_Sprite_NoClip(buffer2, 160, 38);
                    shapes_RoundRectangleFill(colors[1], 15, frame * (19 * transitionSpeed), frame * (12 * transitionSpeed), 8, 231 - frame * (12 * transitionSpeed));
                    gfx_SwapDraw();
                }
                gfx_Sprite_NoClip(buffer1, 8, 38);
                gfx_Sprite_NoClip(buffer2, 160, 38);
            }
            redraw = true;
            gfx_BlitBuffer();
        }
        if (kb_IsDown(kb_KeyWindow) || kb_IsDown(kb_KeyZoom) || kb_IsDown(kb_KeyTrace)) {   // Info menu
            if (transitionSpeed) {
                gfx_GetSprite_NoClip(buffer1, 8, 38);
                gfx_GetSprite_NoClip(buffer2, 160, 38);
                for (int8_t frame = 2; frame < 12 / transitionSpeed; frame++) {
                    shapes_RoundRectangleFill(colors[1], 15, 220, frame * (16 * transitionSpeed), 50, 230 - frame * (16 * transitionSpeed));
                    gfx_SwapDraw();
                }
            }
            menu_Info(colors[1]);
            gfx_FillScreen(colors[0]);
            ui_StatusBar(colors[1], is24Hour, "");
            ui_BottomBar(colors[1], "By TIny_Hacker + RoccoLox Programs");
            if (transitionSpeed) {
                for (uint8_t frame = 12 / transitionSpeed; frame > 1; frame--) {
                    gfx_Sprite_NoClip(buffer1, 8, 38);
                    gfx_Sprite_NoClip(buffer2, 160, 38);
                    shapes_RoundRectangleFill(colors[1], 15, 220, frame * (16 * transitionSpeed), 50, 230 - frame * (16 * transitionSpeed));
                    gfx_SwapDraw();
                }
                gfx_Sprite_NoClip(buffer1, 8, 38);
                gfx_Sprite_NoClip(buffer2, 160, 38);
            }
            redraw = true;
            gfx_BlitBuffer();
        }
        if (kb_IsDown(kb_KeyGraph)) {   // Settings menu
            ui_StatusBar(colors[1], is24Hour, "Settings");
            gfx_BlitBuffer();
            if (transitionSpeed) {
                gfx_GetSprite_NoClip(buffer1, 8, 38);
                gfx_GetSprite_NoClip(buffer2, 160, 38);
                for (int8_t frame = 3; frame < 16 / transitionSpeed; frame++) {
                    shapes_RoundRectangleFill(colors[1], 15, frame * (19 * transitionSpeed), frame * (12 * transitionSpeed), 312 - frame * (19 * transitionSpeed), 231 - frame * (12 * transitionSpeed));
                    gfx_SwapDraw();
                }
            }
            menu_Settings(colors[1]);
            gfx_FillScreen(colors[0]);
            ui_StatusBar(colors[1], is24Hour, "Settings");
            ui_BottomBar(colors[1], "By TIny_Hacker + RoccoLox Programs");
            if (transitionSpeed) {
                for (uint8_t frame = 16 / transitionSpeed; frame > 2; frame--) {
                    gfx_Sprite_NoClip(buffer1, 8, 38);
                    gfx_Sprite_NoClip(buffer2, 160, 38);
                    shapes_RoundRectangleFill(colors[1], 15, frame * (19 * transitionSpeed), frame * (12 * transitionSpeed), 312 - frame * (19 * transitionSpeed), 231 - frame * (12 * transitionSpeed));
                    gfx_SwapDraw();
                }
                gfx_Sprite_NoClip(buffer1, 8, 38);
                gfx_Sprite_NoClip(buffer2, 160, 38);
            }
            redraw = true;
            gfx_BlitBuffer();
        }
        while (kb_Data[1]) {
            kb_Scan();
        }
        if (redraw) {
            ui_StatusBar(colors[1], is24Hour, "");
            ui_DrawAllFiles(colors, cursor, NOPROGS, false);
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
