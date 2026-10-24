
#include "gui.h"
#include "gfx.h"
#include "keypad.h"
#include "syscode.h"

#include <string.h>
int strlength(const char *str)
{
    int len = 0;

    while (str[len] != '\0')
        len++;

    return len;
}
char *wrap_text(const char *src)
{
    static char output[4096];
    size_t i = 0, j = 0;
    int column = 0;

    if (src == NULL) {
        output[0] = '\0';
        return output;
    }

    while (src[i] != '\0' && j < sizeof(output) - 1) {
        if (src[i] == '\n') {
            if (j + 2 >= sizeof(output))
                break;

            output[j++] = '\n';
            output[j++] = '\n';
            column = 0;
            i++;
        } else {
            if (column == 21) {
                output[j++] = '\n';
                column = 0;
            }

            output[j++] = src[i++];
            column++;
        }
    }

    output[j] = '\0';
    return output;
}

void ui_hdr(char* txt, uint16_t hdr_color){
    gfx_fill_rect(0, 0, gfx_width(), 10,
                  //GFX_RGB(80, 80, 80));
                  hdr_color);
    gfx_text(0, 0, txt, GUI_TEXT);
    gfx_line(0, 10, gfx_width(), 10, GFX_WHITE);
}

void ui_section(char* txt, int idx, int sel, uint16_t norm_color, uint16_t sel_color){
    int height = 30;
    int start = height*(idx)-20;
    gfx_line(0, start, gfx_width(), start, GFX_WHITE);
    if(sel){gfx_fill_rect(0, start+1, gfx_width(), start+height+1,
                  //GFX_RGB(30, 90, 160));}
                  sel_color);}
    else{gfx_fill_rect(0, start+1, gfx_width(), start+height+1,
                  //GFX_RGB(70, 150, 180));}
                  norm_color);}
    gfx_line(0, start+height, gfx_width(), start+height, GFX_WHITE);
    gfx_text(0, start+10, txt, GUI_TEXT);
}

void ui_ftr(char* opt1, char* opt2, uint16_t hdr_color){
    int wdt = 6;
    int sizt = strlength(opt2)*wdt;
    int start = gfx_height()-15;
    gfx_line(0, start-1, gfx_width(), start-1, GFX_WHITE);
    gfx_fill_rect(0, start, gfx_width(), gfx_height(),
                  //GFX_RGB(100, 100, 100));
                  hdr_color);
    gfx_text(0, gfx_height()-12, opt1, GUI_TEXT);
    gfx_text(gfx_width()-sizt, gfx_height()-12, opt2, GUI_TEXT);
}

void ui_lsoft_menu(char* help_text, uint16_t menu_color){
    int start = gfx_height()-100;
    int end = gfx_height()-16;
    gfx_line(0, start, gfx_width(), start, GFX_WHITE);
    gfx_fill_rect(0, start, gfx_width(), end,
                  //GFX_RGB(140, 140, 140));
                  menu_color);
    char *txt = wrap_text(help_text);
    gfx_text(0, start+5, txt, GUI_TEXT);
}

int process_keypad(keypad_event_t event, int *sel){
    char* kpn = keypad_name(event.key);
    // debug_print("ev.name ", 0);debug_print(kpn, 1);
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

keypad_event_t ev;
int sel = 1;
int osel = 1;
int lsoft = 0; // PLACEHOLDER

void rndr_sections(UISection* sections, int selected){
    size_t length = 4;//sizeof(sections) / sizeof(sections[0]);
    int j=0;
    for(j=0;j<length;j++){
        ui_section(sections[j].text, j+1, (j+1==selected), sections[j].norm_color, sections[j].sel_color);
    }
}

int ui_update(UISection* sections, HDRSection hdr, FTRSection ftr){
    if (gfx_init() != 0)
        return;
    
    gfx_clear(GUI_BG);
    if(osel!=sel&&lsoft==0){
        rndr_sections(sections, sel);
        gfx_fill_rect(0, 131, gfx_width(), gfx_height(), GUI_BG);
        ui_hdr(hdr.text, hdr.color);
        
        ui_ftr(ftr.opt1_text, ftr.opt2_text, ftr.color);
        gfx_present();
    }
    osel = sel;
    keypad_wait(&ev);
    int mv = process_keypad(ev, &sel);
    if(lsoft==0){
    }
    return mv;
}