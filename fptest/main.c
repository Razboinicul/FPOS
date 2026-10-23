
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
#include <string.h>

char debout[4096];

#include <string.h>

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

int strlength(const char *str)
{
    int len = 0;

    while (str[len] != '\0')
        len++;

    return len;
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

void ui_hdr(char* txt){
    gfx_fill_rect(0, 0, gfx_width(), 10,
                  GFX_RGB(80, 80, 80));
    gfx_text(0, 0, txt, GFX_WHITE);
    gfx_line(0, 10, gfx_width(), 10, GFX_WHITE);
}

void ui_section(char* txt, int idx, int sel){
    int height = 30;
    int start = height*(idx)-20;
    gfx_line(0, start, gfx_width(), start, GFX_WHITE);
    if(sel){gfx_fill_rect(0, start+1, gfx_width(), start+height+1,
                  GFX_RGB(30, 90, 160));}
    else{gfx_fill_rect(0, start+1, gfx_width(), start+height+1,
                  GFX_RGB(70, 150, 180));}
    gfx_line(0, start+height, gfx_width(), start+height, GFX_WHITE);
    gfx_text(0, start+10, txt, GFX_WHITE);
    gfx_fill_rect(0, start+height+1, gfx_width(), gfx_height(), GFX_BLACK);
}

void ui_ftr(char* opt1, char* opt2){
    int wdt = 6;
    int sizt = strlength(opt2)*wdt;
    int start = gfx_height()-15;
    gfx_line(0, start-1, gfx_width(), start-1, GFX_WHITE);
    gfx_fill_rect(0, start, gfx_width(), gfx_height(),
                  GFX_RGB(100, 100, 100));
    gfx_text(0, gfx_height()-12, opt1, GFX_WHITE);
    gfx_text(gfx_width()-sizt, gfx_height()-12, opt2, GFX_WHITE);
}


void test_gfx(int sel)
{
    if (gfx_init() != 0)
        return;

    // gfx_clear(GFX_BLUE);
    // gfx_clear(GFX_BLACK);
    
    if(sel==1){ui_section("Male", 1, 1);}
    else{ui_section("Male", 1, 0);}

    if(sel==2){ui_section("Female", 2, 1);}
    else{ui_section("Female", 2, 0);}

    if(sel==3){ui_section(">:3c", 3, 1);}
    else{ui_section(">:3c", 3, 0);}

    if(sel==4){ui_section("Prefer not to exist", 4, 1);}
    else{ui_section("Prefer not to exist", 4, 0);}

    //gfx_fill_rect(0, 131, gfx_width(), gfx_height()-30, GFX_BLACK);
    ui_hdr("What's your gender?");
    ui_ftr("Help", "Select");

    // gfx_clear(GFX_RGB(18, 22, 35));

    // /* Header */
    // gfx_fill_rect(0, 0, gfx_width(), 24,
    //               GFX_RGB(30, 90, 160));

    // gfx_text(8, 8, "FPDOOM GRAPHICS", GFX_WHITE);

    // /* Shapes */
    // gfx_rect(10, 40, 100, 55, GFX_GREEN);
    // gfx_fill_rect(120, 40, 80, 55, GFX_BLUE);

    // gfx_circle(60, 135, 25, GFX_YELLOW);
    // gfx_fill_circle(160, 135, 25, GFX_RED);

    // /* Lines */
    // gfx_line(10, 180, gfx_width() - 10, 220,
    //          GFX_CYAN);

    // /* Status text */
    // gfx_text(10, gfx_height() - 20,
    //          "FRAMEBUFFER OK", GFX_WHITE);
    gfx_present();
}

int process_keypad(keypad_event_t event, int *sel){
    char* kpn = keypad_name(event.key);
    debug_print("ev.name ", 0);debug_print(kpn, 1);
    if(event.type==KEYPAD_PRESS){
    if(kpn==(char*)"UP"){
        *sel -= 1;
        if(*sel < 1){*sel = 1;}
        return -100;
    }
    if(kpn==(char*)"DOWN"){
        *sel += 1;
        if(*sel > 4){*sel = 4;}
        return -100;
    }
    if(kpn==(char*)"RSOFT"){
        return -1;
    }
    if(kpn==(char*)"LSOFT"){
        return -2;
    }
    if(kpn==(char*)"STAR"){
        return -3;
    }
    if(kpn==(char*)"HASH"){
        return -4;
    }
    if(kpn==(char*)"DIAL"){
        return -5;
    }
    else if(kpn==(char*)"DIAL" && event.power){
        return -50;
    }
    return (int)kpn;
    }
    return -100;
}

int main(int argc, char **argv) {
    gfx_init();
    keypad_init();
    // sys_wait_ms(100);
    // gfx_clear(GFX_BLUE);
    keypad_event_t ev;
    int sel = 1;
    int osel = sel;
    int debug = 0;
    test_gfx(sel);
    while(1){
        if(!debug){if(osel!=sel){test_gfx(sel);}}
        else{gfx_clear(GFX_RED);
            gfx_text(0, 0, last_20_lines(debout), GFX_WHITE);gfx_present();
            sys_wait_ms(200);}
        osel = sel;
        keypad_wait(&ev);
        int mv = process_keypad(ev, &sel);
        if(mv == -1){
            char select[16];
            snprintf(select, sizeof(select), "%d", sel);
            debug_print(select, 0);debug_print(" selected!", 1);
        }
        if(mv == -5){
            if(debug==1) {test_gfx(sel);debug = 0;}
            else {debug = 1;}
            //debug = !debug;
        }
    }
}