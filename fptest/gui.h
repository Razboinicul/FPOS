
#ifndef GUI_H
#define GUI_H

#include <stdint.h>
#include "gfx.h"
#include "keypad.h"

#define GUI_BG       GFX_RGB(18, 22, 32)
#define GUI_PANEL    GFX_RGB(35, 42, 56)
#define GUI_BORDER   GFX_RGB(75, 88, 110)
#define GUI_TEXT     GFX_RGB(235, 240, 248)
#define GUI_MUTED    GFX_RGB(150, 165, 185)
#define GUI_ACCENT   GFX_RGB(60, 170, 240)
#define GUI_FOCUS    GFX_RGB(255, 190, 65)
#define GUI_SUCCESS  GFX_RGB(80, 210, 130)

extern int sel;

typedef struct {
    char *text;
    uint16_t norm_color;
    uint16_t sel_color;
} UISection;

typedef struct {
    char *text;
    uint16_t color;
} HDRSection;

typedef struct {
    char *opt1_text;
    char *opt2_text;
    uint16_t color;
} FTRSection;

//int strlength(const char *str);
char *wrap_text(const char *src);
int process_keypad(keypad_event_t event, int *sel);

void ui_hdr(char* txt, uint16_t hdr_color);
void ui_ftr(char* opt1, char* opt2, uint16_t hdr_color);

void ui_section(char* txt, int idx, int sel, uint16_t norm_color, uint16_t sel_color);

void ui_lsoft_menu(char* help_text, uint16_t menu_color);
int ui_update(UISection* sections, HDRSection hdr, FTRSection ftr);

#endif
