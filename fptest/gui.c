
#include "gui.h"
#include "keypad.h"
#include "syscode.h"

#include <string.h>

static int key_matches(int key, const char *name)
{
    char *actual;

    if (!name)
        return 0;

    actual = keypad_name((uint16_t)key);

    return actual && strcmp(actual, name) == 0;
}

void gui_init(gui_context_t *ctx, gui_keys_t keys)
{
    memset(ctx, 0, sizeof(*ctx));
    ctx->keys = keys;
    keypad_init();
}

void gui_begin_frame(gui_context_t *ctx)
{
    //struct 
    keypad_event_t ev;

    ctx->activate = 0;
    ctx->back_pressed = 0;
    ctx->right_soft_pressed = 0;
    ctx->left_pressed = 0;
    ctx->right_pressed = 0;
    ctx->quit = 0;

    while (keypad_poll(&ev)) {
        if (ev.type != KEYPAD_PRESS)
            continue;

        if (key_matches(ev.key, ctx->keys.up)) {
            if (ctx->focused > 0)
                ctx->focused--;
        }
        else if (key_matches(ev.key, ctx->keys.down)) {
            ctx->focused++;
        }
        else if (key_matches(ev.key, ctx->keys.confirm)) {
            ctx->activate = 1;
        }
        else if (key_matches(ev.key, ctx->keys.back)) {
            ctx->back_pressed = 1;
        }
        else if (key_matches(ev.key, ctx->keys.soft_right)) {
            ctx->right_soft_pressed = 1;
        }
        else if (key_matches(ev.key, ctx->keys.left)) {
            ctx->left_pressed = 1;
        }
        else if (key_matches(ev.key, ctx->keys.right)) {
            ctx->right_pressed = 1;
        }
    }

    /*
     * Widgets register themselves again every frame.
     * Focus is clamped in gui_end_frame().
     */
    ctx->widget_count = 0;
}

void gui_end_frame(gui_context_t *ctx)
{
    if (ctx->widget_count <= 0) {
        ctx->focused = 0;
    } else {
        if (ctx->focused < 0)
            ctx->focused = 0;

        if (ctx->focused >= ctx->widget_count)
            ctx->focused = ctx->widget_count - 1;
    }

    gfx_present();
}

void gui_panel(int x, int y, int w, int h, const char *title)
{
    gfx_fill_rect(x, y, w, h, GUI_PANEL);
    gfx_rect(x, y, w, h, GUI_BORDER);

    if (title)
        gfx_text(x + 8, y + 6, title, GUI_ACCENT);
}

void gui_label(int x, int y, const char *text, uint16_t color)
{
    gfx_text(x, y, text, color);
}

int gui_button(gui_context_t *ctx, const char *text)
{
    int index = ctx->widget_count++;
    int focused = (index == ctx->focused);
    int x = 24;
    int y = 48 + index * 28;
    int w = gfx_width() - 48;
    int h = 23;

    uint16_t bg = focused ? GUI_ACCENT : GUI_PANEL;
    uint16_t fg = focused ? GFX_RGB(255, 255, 255) : GUI_TEXT;

    gfx_fill_rect(x, y, w, h, bg);
    gfx_rect(x, y, w, h, focused ? GUI_FOCUS : GUI_BORDER);
    gfx_text(x + 8, y + 6, text, fg);

    return focused && ctx->activate;
}

int gui_checkbox(gui_context_t *ctx, const char *text, int *value)
{
    int index = ctx->widget_count++;
    int focused = (index == ctx->focused);
    int x = 24;
    int y = 48 + index * 28;
    int w = gfx_width() - 48;
    int h = 23;

    gfx_fill_rect(x, y, w, h, focused ? GUI_ACCENT : GUI_PANEL);
    gfx_rect(x, y, w, h, focused ? GUI_FOCUS : GUI_BORDER);

    gfx_text(x + 8, y + 6, text,
             focused ? GFX_RGB(255, 255, 255) : GUI_TEXT);

    gfx_rect(x + w - 24, y + 5, 14, 13, GUI_TEXT);

    if (*value)
        gfx_fill_rect(x + w - 21, y + 8, 8, 7, GUI_SUCCESS);

    if (focused && ctx->activate) {
        *value = !*value;
        return 1;
    }

    return 0;
}

int gui_slider(gui_context_t *ctx, const char *text,
               int *value, int min_value, int max_value)
{
    int index = ctx->widget_count++;
    int focused = (index == ctx->focused);
    int x = 24;
    int y = 48 + index * 34;
    int w = gfx_width() - 48;
    int bar_y = y + 20;
    int bar_w = w - 16;
    int changed = 0;
    int range = max_value - min_value;
    int fill_w = 0;

    if (range < 0)
        range = 0;

    if (*value < min_value)
        *value = min_value;
    if (*value > max_value)
        *value = max_value;

    gfx_text(x, y, text, focused ? GUI_FOCUS : GUI_TEXT);

    gfx_fill_rect(x, bar_y, bar_w, 7, GUI_PANEL);
    gfx_rect(x, bar_y, bar_w, 7, GUI_BORDER);

    if (range > 0)
        fill_w = (int)(((long)(*value - min_value) * (bar_w - 2))
                       / range);

    if (fill_w > 0)
        gfx_fill_rect(x + 1, bar_y + 1, fill_w, 5, GUI_ACCENT);

    if (focused) {
        if (ctx->left_pressed && *value > min_value) {
            --*value;
            changed = 1;
        }

        if (ctx->right_pressed && *value < max_value) {
            ++*value;
            changed = 1;
        }

        if (ctx->activate) {
            *value += 5;
            if (*value > max_value)
                *value = min_value;
            changed = 1;
        }
    }

    return changed;
}

void gui_progressbar(int x, int y, int w, int h,
                     int value, int max_value)
{
    int fill_w = 0;

    if (value < 0)
        value = 0;
    if (max_value < 1)
        max_value = 1;
    if (value > max_value)
        value = max_value;

    gfx_fill_rect(x, y, w, h, GUI_PANEL);
    gfx_rect(x, y, w, h, GUI_BORDER);

    fill_w = (int)(((long)value * (w - 2)) / max_value);

    if (fill_w > 0)
        gfx_fill_rect(x + 1, y + 1, fill_w, h - 2, GUI_SUCCESS);
}

int gui_back_requested(const gui_context_t *ctx)
{
    return ctx->back_pressed;
}

int gui_right_soft_requested(const gui_context_t *ctx)
{
    return ctx->right_soft_pressed;
}

int gui_quit_requested(const gui_context_t *ctx)
{
    return ctx->quit;
}
