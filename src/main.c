#include "ui.h"
#include "shapes.h"
#include "menu.h"

#include <graphx.h>
#include <keypadc.h>

gfx_UninitedSprite(buffer1, 152, 193);  // These preserve the background to make redrawing faster
gfx_UninitedSprite(buffer2, 152, 193);

int main(void) {
    uint8_t colors[3] = {246, 237, 236};    // Will load colors from Appvar later, basically it's the background and two extra colors per theme
    uint8_t transitionSpeed = 2;    // 1 is slow, 2 is normal, 3 is fast, and 0 has no transitions
    buffer1->height = 193;
    buffer1->width = 152;
    buffer2->height = 193;
    buffer2->width = 152;

    gfx_Begin();
    gfx_SetTransparentColor(240);

    gfx_SetDrawBuffer();
    gfx_FillScreen(colors[0]);
    ui_StatusBar(colors[1], true, "");  // Displays bar with program name, clock, and battery
    ui_BottomBar(colors[1], "By TIny_Hacker + RoccoLox Programs");
    gfx_BlitBuffer();

    while(!kb_IsDown(kb_KeyClear)) {    // Looks menu
        kb_Scan();
        if (kb_IsDown(kb_KeyYequ)) {
            if (transitionSpeed) {  // I'll probably put this in a separate function and clean it up later
                for (int8_t frame = 3; frame < 16 / transitionSpeed; frame++) {
                    shapes_RoundRectangleFill(colors[1], 15, frame * (19 * transitionSpeed), frame * (12 * transitionSpeed), 8, 231 - frame * (12 * transitionSpeed));
                    gfx_SwapDraw();
                }
            }
            menu_Looks(colors);
            gfx_FillScreen(colors[0]);
            ui_StatusBar(colors[1], true, "");
            ui_BottomBar(colors[1], "By TIny_Hacker + RoccoLox Programs");
            if (transitionSpeed) {
                gfx_GetSprite_NoClip(buffer1, 8, 38);
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
            gfx_BlitBuffer();
        }
        if (kb_IsDown(kb_KeyWindow) || kb_IsDown(kb_KeyZoom) || kb_IsDown(kb_KeyTrace)) {   // Info menu
            if (transitionSpeed) {  
                for (int8_t frame = 2; frame < 12 / transitionSpeed; frame++) {
                    shapes_RoundRectangleFill(colors[1], 15, 220, frame * (16 * transitionSpeed), 50, 230 - frame * (16 * transitionSpeed));
                    gfx_SwapDraw();
                }
            }
            menu_Info(colors[1]);
            gfx_FillScreen(colors[0]);
            ui_StatusBar(colors[1], true, "");
            ui_BottomBar(colors[1], "By TIny_Hacker + RoccoLox Programs");
            if (transitionSpeed) {
                gfx_GetSprite_NoClip(buffer1, 8, 38);
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
        }
        if (kb_IsDown(kb_KeyGraph)) {   // Settings menu
            if (transitionSpeed) {  
                for (int8_t frame = 3; frame < 16 / transitionSpeed; frame++) {
                    shapes_RoundRectangleFill(colors[1], 15, frame * (19 * transitionSpeed), frame * (12 * transitionSpeed), 312 - frame * (19 * transitionSpeed), 231 - frame * (12 * transitionSpeed));
                    gfx_SwapDraw();
                }
            }
            menu_Settings(colors[1]);
            gfx_FillScreen(colors[0]);
            ui_StatusBar(colors[1], true, "");
            ui_BottomBar(colors[1], "By TIny_Hacker + RoccoLox Programs");
            if (transitionSpeed) {
                gfx_GetSprite_NoClip(buffer1, 8, 38);
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
            gfx_BlitBuffer();
        }
        while (kb_Data[1]) {
            kb_Scan();
        }
    }

    gfx_End();
    return 0;
}
