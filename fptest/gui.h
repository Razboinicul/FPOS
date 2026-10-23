
#ifndef GUI_H
#define GUI_H

#include <stdint.h>
#include "gfx.h"

#define GUI_BG       GFX_RGB(18, 22, 32)
#define GUI_PANEL    GFX_RGB(35, 42, 56)
#define GUI_BORDER   GFX_RGB(75, 88, 110)
#define GUI_TEXT     GFX_RGB(235, 240, 248)
#define GUI_MUTED    GFX_RGB(150, 165, 185)
#define GUI_ACCENT   GFX_RGB(60, 170, 240)
#define GUI_FOCUS    GFX_RGB(255, 190, 65)
#define GUI_SUCCESS  GFX_RGB(80, 210, 130)

typedef struct {
    const char *up;
    const char *down;
    const char *left;
    const char *right;
    const char *confirm;
    const char *back;
    const char *soft_right;
} gui_keys_t;

typedef struct {
    int focused;
    int widget_count;

    int activate;
    int back_pressed;
    int right_soft_pressed;
    int left_pressed;
    int right_pressed;
    int quit;

    gui_keys_t keys;
} gui_context_t;

void gui_init(gui_context_t *ctx, gui_keys_t keys);
void gui_begin_frame(gui_context_t *ctx);
void gui_end_frame(gui_context_t *ctx);

void gui_panel(int x, int y, int w, int h, const char *title);
void gui_label(int x, int y, const char *text, uint16_t color);
int gui_button(gui_context_t *ctx, const char *text);
int gui_checkbox(gui_context_t *ctx, const char *text, int *value);
int gui_slider(gui_context_t *ctx, const char *text,
               int *value, int min_value, int max_value);
void gui_progressbar(int x, int y, int w, int h,
                     int value, int max_value);

int gui_back_requested(const gui_context_t *ctx);
int gui_right_soft_requested(const gui_context_t *ctx);
int gui_quit_requested(const gui_context_t *ctx);

#endif
