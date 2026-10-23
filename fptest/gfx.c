
#include "gfx.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "syscode.h"

#define GFX_WIDTH  320
#define GFX_HEIGHT 480

#include "gfx.h"
#include <stdint.h>

#define GFX_WIDTH  320
#define GFX_HEIGHT 480

static int gfx_w = 0;
static int gfx_h = 0;

static uint16_t gfx_pixels[GFX_WIDTH * GFX_HEIGHT]
    __attribute__((aligned(32)));

static uint16_t *gfx_fb = gfx_pixels;

int gfx_width(void)
{
    return gfx_w;
}

int gfx_height(void)
{
    return gfx_h;
}

uint16_t *gfx_buffer(void)
{
    return gfx_fb;
}

int gfx_init(void)
{
    gfx_w = sys_data.display.w2;
    gfx_h = sys_data.display.h2;

    if (gfx_w <= 0 || gfx_h <= 0)
        return -1;

    gfx_fb = gfx_pixels;

    sys_framebuffer(gfx_fb);
    sys_start();

    gfx_clear(GFX_BLACK);
    gfx_present();

    return 0;
}

void gfx_shutdown(void)
{
    /*
     * Stop using our framebuffer before freeing its memory.
     * Only call this when the display system no longer needs it.
     */
    gfx_fb = NULL;

    //free(gfx_alloc_mem);

    gfx_w = 0;
    gfx_h = 0;
}

void gfx_pixel(int x, int y, uint16_t color)
{
    if (!gfx_fb)
        return;

    if ((unsigned)x >= (unsigned)gfx_w ||
        (unsigned)y >= (unsigned)gfx_h)
        return;

    gfx_fb[(size_t)y * gfx_w + x] = color;
}

void gfx_clear(uint16_t color)
{
    size_t count, i;

    if (!gfx_fb)
        return;

    count = (size_t)gfx_w * gfx_h;

    for (i = 0; i < count; ++i)
        gfx_fb[i] = color;
}

/* Bresenham's line algorithm */
void gfx_line(int x0, int y0, int x1, int y1,
              uint16_t color)
{
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    for (;;) {
        gfx_pixel(x0, y0, color);

        if (x0 == x1 && y0 == y1)
            break;

        {
            int e2 = 2 * err;

            if (e2 >= dy) {
                err += dy;
                x0 += sx;
            }

            if (e2 <= dx) {
                err += dx;
                y0 += sy;
            }
        }
    }
}

void gfx_fill_rect(int x, int y, int w, int h,
                   uint16_t color)
{
    int x0, y0, x1, y1, xx, yy;

    if (!gfx_fb || w <= 0 || h <= 0)
        return;

    /* Use wider arithmetic to avoid x+w overflow. */
    {
        long lx1 = (long)x + w;
        long ly1 = (long)y + h;

        if (lx1 <= 0 || ly1 <= 0 ||
            x >= gfx_w || y >= gfx_h)
            return;

        x0 = x < 0 ? 0 : x;
        y0 = y < 0 ? 0 : y;
        x1 = lx1 > gfx_w ? gfx_w : (int)lx1;
        y1 = ly1 > gfx_h ? gfx_h : (int)ly1;
    }

    for (yy = y0; yy < y1; ++yy) {
        uint16_t *p = gfx_fb + (size_t)yy * gfx_w + x0;

        for (xx = x0; xx < x1; ++xx)
            *p++ = color;
    }
}

void gfx_rect(int x, int y, int w, int h,
              uint16_t color)
{
    if (w <= 0 || h <= 0)
        return;

    gfx_line(x, y, x + w - 1, y, color);
    gfx_line(x, y + h - 1, x + w - 1, y + h - 1,
             color);
    gfx_line(x, y, x, y + h - 1, color);
    gfx_line(x + w - 1, y, x + w - 1, y + h - 1,
             color);
}

/* Midpoint circle algorithm */
void gfx_circle(int cx, int cy, int radius,
                uint16_t color)
{
    int x, y, d;

    if (radius < 0)
        return;

    x = radius;
    y = 0;
    d = 1 - radius;

    while (x >= y) {
        gfx_pixel(cx + x, cy + y, color);
        gfx_pixel(cx + y, cy + x, color);
        gfx_pixel(cx - y, cy + x, color);
        gfx_pixel(cx - x, cy + y, color);
        gfx_pixel(cx - x, cy - y, color);
        gfx_pixel(cx - y, cy - x, color);
        gfx_pixel(cx + y, cy - x, color);
        gfx_pixel(cx + x, cy - y, color);

        ++y;

        if (d < 0) {
            d += 2 * y + 1;
        } else {
            --x;
            d += 2 * (y - x) + 1;
        }
    }
}

void gfx_fill_circle(int cx, int cy, int radius,
                     uint16_t color)
{
    int x, y, d;

    if (radius < 0)
        return;

    x = radius;
    y = 0;
    d = 1 - radius;

    while (x >= y) {
        gfx_line(cx - x, cy + y,
                 cx + x, cy + y, color);
        gfx_line(cx - x, cy - y,
                 cx + x, cy - y, color);
        gfx_line(cx - y, cy + x,
                 cx + y, cy + x, color);
        gfx_line(cx - y, cy - x,
                 cx + y, cy - x, color);

        ++y;

        if (d < 0) {
            d += 2 * y + 1;
        } else {
            --x;
            d += 2 * (y - x) + 1;
        }
    }
}

/*
 * Compact 5x7 font.
 * Glyphs are stored as five columns, with bit 0 at
 * the top. Lowercase is converted to uppercase.
 */
static const char font_chars[] =
    " ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
    ".:,;!?-+*/()[]<>=_";

static const uint8_t font_data[][5] = {
    {0,0,0,0,0},       /* space */
    {0x7e,0x11,0x11,0x11,0x7e}, /* A */
    {0x7f,0x49,0x49,0x49,0x36}, /* B */
    {0x3e,0x41,0x41,0x41,0x22}, /* C */
    {0x7f,0x41,0x41,0x22,0x1c}, /* D */
    {0x7f,0x49,0x49,0x49,0x41}, /* E */
    {0x7f,0x09,0x09,0x09,0x01}, /* F */
    {0x3e,0x41,0x49,0x49,0x7a}, /* G */
    {0x7f,0x08,0x08,0x08,0x7f}, /* H */
    {0x00,0x41,0x7f,0x41,0x00}, /* I */
    {0x20,0x40,0x41,0x3f,0x01}, /* J */
    {0x7f,0x08,0x14,0x22,0x41}, /* K */
    {0x7f,0x40,0x40,0x40,0x40}, /* L */
    {0x7f,0x02,0x0c,0x02,0x7f}, /* M */
    {0x7f,0x04,0x08,0x10,0x7f}, /* N */
    {0x3e,0x41,0x41,0x41,0x3e}, /* O */
    {0x7f,0x09,0x09,0x09,0x06}, /* P */
    {0x3e,0x41,0x51,0x21,0x5e}, /* Q */
    {0x7f,0x09,0x19,0x29,0x46}, /* R */
    {0x46,0x49,0x49,0x49,0x31}, /* S */
    {0x01,0x01,0x7f,0x01,0x01}, /* T */
    {0x3f,0x40,0x40,0x40,0x3f}, /* U */
    {0x1f,0x20,0x40,0x20,0x1f}, /* V */
    {0x3f,0x40,0x38,0x40,0x3f}, /* W */
    {0x63,0x14,0x08,0x14,0x63}, /* X */
    {0x07,0x08,0x70,0x08,0x07}, /* Y */
    {0x61,0x51,0x49,0x45,0x43}, /* Z */

    {0x3e,0x51,0x49,0x45,0x3e}, /* 0 */
    {0x00,0x42,0x7f,0x40,0x00}, /* 1 */
    {0x42,0x61,0x51,0x49,0x46}, /* 2 */
    {0x21,0x41,0x45,0x4b,0x31}, /* 3 */
    {0x18,0x14,0x12,0x7f,0x10}, /* 4 */
    {0x27,0x45,0x45,0x45,0x39}, /* 5 */
    {0x3c,0x4a,0x49,0x49,0x30}, /* 6 */
    {0x01,0x71,0x09,0x05,0x03}, /* 7 */
    {0x36,0x49,0x49,0x49,0x36}, /* 8 */
    {0x06,0x49,0x49,0x29,0x1e}, /* 9 */

    {0x00,0x60,0x60,0x00,0x00}, /* . */
    {0x00,0x36,0x36,0x00,0x00}, /* : */
    {0x00,0x56,0x36,0x00,0x00}, /* , */
    {0x00,0x56,0x36,0x00,0x00}, /* ; */
    {0x02,0x01,0x51,0x09,0x06}, /* ! */
    {0x02,0x01,0x51,0x09,0x06}, /* ? */
    {0x08,0x08,0x08,0x08,0x08}, /* - */
    {0x08,0x08,0x3e,0x08,0x08}, /* + */
    {0x20,0x10,0x08,0x04,0x02}, /* / */
    {0x14,0x08,0x3e,0x08,0x14}, /* * */
    {0x00,0x1c,0x22,0x41,0x00}, /* ( */
    {0x00,0x41,0x22,0x1c,0x00}, /* ) */
    {0x00,0x7f,0x41,0x41,0x00}, /* [ */
    {0x00,0x41,0x41,0x7f,0x00}, /* ] */
    {0x08,0x14,0x22,0x41,0x00}, /* < */
    {0x00,0x41,0x22,0x14,0x08}, /* > */
    {0x14,0x14,0x14,0x14,0x14}, /* = */
    {0x40,0x40,0x40,0x40,0x40}  /* _ */
};

void gfx_char(int x, int y, char c, uint16_t color)
{
    unsigned i, row;
    const uint8_t *glyph = NULL;

    if (c >= 'a' && c <= 'z')
        c = (char)(c - 'a' + 'A');

    for (i = 0;
         i < sizeof(font_chars) - 1;
         ++i) {
        if (font_chars[i] == c) {
            glyph = font_data[i];
            break;
        }
    }

    if (!glyph)
        return;

    for (i = 0; i < 5; ++i) {
        for (row = 0; row < 7; ++row) {
            if (glyph[i] & (1u << row))
                gfx_pixel(x + (int)i, y + (int)row,
                          color);
        }
    }
}

void gfx_text(int x, int y, const char *text,
              uint16_t color)
{
    int start_x = x;

    if (!text)
        return;

    while (*text) {
        if (*text == '\n') {
            x = start_x;
            y += 8;
        } else if (*text == '\r') {
            /* Ignore carriage returns. */
        } else {
            gfx_char(x, y, *text, color);
            x += 6;
        }

        ++text;
    }
}

void gfx_present(void)
{
    if (!gfx_fb)
        return;

    sys_start_refresh();
    sys_wait_refresh();
}
