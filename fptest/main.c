
#include "syscode.h"
#include "cmd_def.h"
#include "usbio.h"
#include "sdio.h"
#ifdef SFC
#include "sfc.h"
#endif
#include "efuse.h"

#include "gfx.h"
#include "keypad.h"
#include "gui.h"

#include <stdint.h>
#include <stdio.h>

char debout[4096];

#include <string.h>
#include <stddef.h>

const char *last_20_lines(const char *text)
{
    if (text == NULL)
        return "";

    const char *end = text + strlen(text);
    const char *p = end;
    int lines = 0;

    while (p > text) {
        --p;

        if (*p == '\n') {
            lines++;

            if (lines == 20)
                return p + 1;
        }
    }

    return text;
}

void debug_print(char* txt, int nl){
    // 20 lines
    strcat(debout, txt);
    if(nl==1) strcat(debout, "\n");
}

void keytrn_init(void) {
    uint8_t keymap[64];
    int i, flags = sys_getkeymap(keymap);
    (void)flags;

#define FILL_KEYTRN(j) \
    for (i = 0; i < 64; i++) { \
        sys_data.keytrn[j][i] = keymap[i] | j << 15; \
    }

    FILL_KEYTRN(0)
    FILL_KEYTRN(1)
#undef FILL_KEYTRN
}
void lcd_appinit(void) {
    struct sys_display *disp = &sys_data.display;
    int w = disp->w1, h = disp->h1;
    disp->w2 = w;
    disp->h2 = h;
}

int main(int argc, char **argv) {
    gfx_init();
    keypad_init();

    int debug = 0;
    int lsoft = 0;
    UISection sec[] = {
        {"Selection 1", GFX_RGB(70, 150, 180), GFX_RGB(30, 90, 160)},
        {"Selection 2", GFX_RGB(70, 150, 180), GFX_RGB(30, 90, 160)},
        {"Selection 3", GFX_RGB(70, 150, 180), GFX_RGB(30, 90, 160)},
        {"Selection 4", GFX_RGB(70, 150, 180), GFX_RGB(30, 90, 160)},
    };
    FTRSection footer = {"Help", "Select", GFX_RGB(100, 100, 100)};
    HDRSection header = {"Menu Test", GFX_RGB(80, 80, 80)};
    ui_update(sec, header, footer);
    while(1){
        int mv = 0;
        if(!debug){mv = ui_update(sec, header, footer);}
        else{gfx_clear(GFX_RED);
            gfx_text(0, 0, last_20_lines(debout), GFX_WHITE);gfx_present();
            sys_wait_ms(200);}
        if(lsoft==0){
        if(mv == -1){
            char select[16];
            snprintf(select, sizeof(select), "%d", sel);
            debug_print(select, 0);debug_print(" selected!", 1);
        }
        }
        if(mv == -5){
            if(debug==1) {ui_update(sec, header, footer);debug = 0;}
            else {debug = 1;}
        }
    }
    gfx_shutdown();
}