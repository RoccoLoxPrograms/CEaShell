#include <graphx.h>
#include <keypadc.h>

int main(void) {
    gfx_Begin();

    gfx_FillScreen(246);

    while(!kb_AnyKey()) {
        kb_Scan();
    }

    gfx_End();
    return 0;
}
