
#ifndef GFX_H
#define GFX_H

#include <stdint.h>

/* RGB565 colors */
#define GFX_RGB(r, g, b) \
    ((uint16_t)((((r) & 0xF8) << 8) | \
                (((g) & 0xFC) << 3) | \
                ((b) >> 3)))

#define GFX_BLACK   GFX_RGB(0, 0, 0)
#define GFX_WHITE   GFX_RGB(255, 255, 255)
#define GFX_RED     GFX_RGB(255, 0, 0)
#define GFX_GREEN   GFX_RGB(0, 255, 0)
#define GFX_BLUE    GFX_RGB(0, 0, 255)
#define GFX_YELLOW  GFX_RGB(255, 255, 0)
#define GFX_CYAN    GFX_RGB(0, 255, 255)
#define GFX_MAGENTA GFX_RGB(255, 0, 255)
#define GFX_GRAY    GFX_RGB(128, 128, 128)

/* Initialize/shut down graphics */
int  gfx_init(void);
void gfx_shutdown(void);

/* Screen information */
int gfx_width(void);
int gfx_height(void);
uint16_t *gfx_buffer(void);

/* Drawing */
void gfx_pixel(int x, int y, uint16_t color);
void gfx_clear(uint16_t color);

void gfx_line(int x0, int y0, int x1, int y1,
              uint16_t color);

void gfx_rect(int x, int y, int w, int h,
              uint16_t color);

void gfx_fill_rect(int x, int y, int w, int h,
                   uint16_t color);

void gfx_circle(int cx, int cy, int radius,
                uint16_t color);

void gfx_fill_circle(int cx, int cy, int radius,
                     uint16_t color);

/* Text: 5x7 bitmap font, 6x8 cell spacing */
void gfx_char(int x, int y, char c, uint16_t color);
void gfx_text(int x, int y, const char *text,
              uint16_t color);

/* Display the rendered framebuffer */
void gfx_present(void);

#endif
