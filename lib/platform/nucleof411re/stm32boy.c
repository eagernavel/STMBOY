#include "stm32boy.h"
#include "platform/component/ili9486/ili9486.h" 
#include "platform/nucleof411re/platform_nucleof411re_ili9486.h"

#define FONT_W   5u
#define FONT_H   8u
#define FONT_SP  1u
#define CHAR_W   (FONT_W + FONT_SP)

#define ILI9486_WIDTH   320
#define ILI9486_HEIGHT  480


// Minimal 5x7 font (ASCII 0x20-0x7F), column-major, LSB is top pixel.
static const uint8_t font5x7[96][5] = {
    {0x00,0x00,0x00,0x00,0x00}, // ' '
    {0x00,0x00,0x5F,0x00,0x00}, // '!'
    {0x00,0x07,0x00,0x07,0x00}, // '"'
    {0x14,0x7F,0x14,0x7F,0x14}, // '#'
    {0x24,0x2A,0x7F,0x2A,0x12}, // '$'
    {0x23,0x13,0x08,0x64,0x62}, // '%'
    {0x36,0x49,0x55,0x22,0x50}, // '&'
    {0x00,0x05,0x03,0x00,0x00}, // '''
    {0x00,0x1C,0x22,0x41,0x00}, // '('
    {0x00,0x41,0x22,0x1C,0x00}, // ')'
    {0x14,0x08,0x3E,0x08,0x14}, // '*'
    {0x08,0x08,0x3E,0x08,0x08}, // '+'
    {0x00,0x50,0x30,0x00,0x00}, // ','
    {0x08,0x08,0x08,0x08,0x08}, // '-'
    {0x00,0x60,0x60,0x00,0x00}, // '.'
    {0x20,0x10,0x08,0x04,0x02}, // '/'
    {0x3E,0x51,0x49,0x45,0x3E}, // '0'
    {0x00,0x42,0x7F,0x40,0x00}, // '1'
    {0x42,0x61,0x51,0x49,0x46}, // '2'
    {0x21,0x41,0x45,0x4B,0x31}, // '3'
    {0x18,0x14,0x12,0x7F,0x10}, // '4'
    {0x27,0x45,0x45,0x45,0x39}, // '5'
    {0x3C,0x4A,0x49,0x49,0x30}, // '6'
    {0x01,0x71,0x09,0x05,0x03}, // '7'
    {0x36,0x49,0x49,0x49,0x36}, // '8'
    {0x06,0x49,0x49,0x29,0x1E}, // '9'
    {0x00,0x36,0x36,0x00,0x00}, // ':'
    {0x00,0x56,0x36,0x00,0x00}, // ';'
    {0x08,0x14,0x22,0x41,0x00}, // '<'
    {0x14,0x14,0x14,0x14,0x14}, // '='
    {0x00,0x41,0x22,0x14,0x08}, // '>'
    {0x02,0x01,0x51,0x09,0x06}, // '?'
    {0x32,0x49,0x79,0x41,0x3E}, // '@'
    {0x7E,0x11,0x11,0x11,0x7E}, // 'A'
    {0x7F,0x49,0x49,0x49,0x36}, // 'B'
    {0x3E,0x41,0x41,0x41,0x22}, // 'C'
    {0x7F,0x41,0x41,0x22,0x1C}, // 'D'
    {0x7F,0x49,0x49,0x49,0x41}, // 'E'
    {0x7F,0x09,0x09,0x09,0x01}, // 'F'
    {0x3E,0x41,0x49,0x49,0x7A}, // 'G'
    {0x7F,0x08,0x08,0x08,0x7F}, // 'H'
    {0x00,0x41,0x7F,0x41,0x00}, // 'I'
    {0x20,0x40,0x41,0x3F,0x01}, // 'J'
    {0x7F,0x08,0x14,0x22,0x41}, // 'K'
    {0x7F,0x40,0x40,0x40,0x40}, // 'L'
    {0x7F,0x02,0x0C,0x02,0x7F}, // 'M'
    {0x7F,0x04,0x08,0x10,0x7F}, // 'N'
    {0x3E,0x41,0x41,0x41,0x3E}, // 'O'
    {0x7F,0x09,0x09,0x09,0x06}, // 'P'
    {0x3E,0x41,0x51,0x21,0x5E}, // 'Q'
    {0x7F,0x09,0x19,0x29,0x46}, // 'R'
    {0x46,0x49,0x49,0x49,0x31}, // 'S'
    {0x01,0x01,0x7F,0x01,0x01}, // 'T'
    {0x3F,0x40,0x40,0x40,0x3F}, // 'U'
    {0x1F,0x20,0x40,0x20,0x1F}, // 'V'
    {0x7F,0x20,0x18,0x20,0x7F}, // 'W'
    {0x63,0x14,0x08,0x14,0x63}, // 'X'
    {0x07,0x08,0x70,0x08,0x07}, // 'Y'
    {0x61,0x51,0x49,0x45,0x43}, // 'Z'
    {0x00,0x7F,0x41,0x41,0x00}, // '['
    {0x02,0x04,0x08,0x10,0x20}, // '\\'
    {0x00,0x41,0x41,0x7F,0x00}, // ']'
    {0x04,0x02,0x01,0x02,0x04}, // '^'
    {0x40,0x40,0x40,0x40,0x40}, // '_'
    {0x00,0x01,0x02,0x04,0x00}, // '`'
    {0x20,0x54,0x54,0x54,0x78}, // 'a'
    {0x7F,0x48,0x44,0x44,0x38}, // 'b'
    {0x38,0x44,0x44,0x44,0x20}, // 'c'
    {0x38,0x44,0x44,0x48,0x7F}, // 'd'
    {0x38,0x54,0x54,0x54,0x18}, // 'e'
    {0x08,0x7E,0x09,0x01,0x02}, // 'f'
    {0x0C,0x52,0x52,0x52,0x3E}, // 'g'
    {0x7F,0x08,0x04,0x04,0x78}, // 'h'
    {0x00,0x44,0x7D,0x40,0x00}, // 'i'
    {0x20,0x40,0x44,0x3D,0x00}, // 'j'
    {0x7F,0x10,0x28,0x44,0x00}, // 'k'
    {0x00,0x41,0x7F,0x40,0x00}, // 'l'
    {0x7C,0x04,0x18,0x04,0x78}, // 'm'
    {0x7C,0x08,0x04,0x04,0x78}, // 'n'
    {0x38,0x44,0x44,0x44,0x38}, // 'o'
    {0x7C,0x14,0x14,0x14,0x08}, // 'p'
    {0x08,0x14,0x14,0x14,0x7C}, // 'q'
    {0x7C,0x08,0x04,0x04,0x08}, // 'r'
    {0x48,0x54,0x54,0x54,0x20}, // 's'
    {0x04,0x3F,0x44,0x40,0x20}, // 't'
    {0x3C,0x40,0x40,0x20,0x7C}, // 'u'
    {0x1C,0x20,0x40,0x20,0x1C}, // 'v'
    {0x3C,0x40,0x30,0x40,0x3C}, // 'w'
    {0x44,0x28,0x10,0x28,0x44}, // 'x'
    {0x0C,0x50,0x50,0x50,0x3C}, // 'y'
    {0x44,0x64,0x54,0x4C,0x44}, // 'z'
    {0x00,0x08,0x36,0x41,0x00}, // '{'
    {0x00,0x00,0x7F,0x00,0x00}, // '|'
    {0x00,0x41,0x36,0x08,0x00}, // '}'
    {0x08,0x04,0x08,0x10,0x08}, // '~'
    {0x00,0x00,0x00,0x00,0x00}, // DEL
};

static inline int16_t clamp16(int16_t v, int16_t lo, int16_t hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

void stm32boy_init(stm32boy_t *g, uint16_t width, uint16_t height)
{
    g->width = width;
    g->height = height;

    g->cursor_x = 0;
    g->cursor_y = 0;
    g->text_fg = 0xFFFF;
    g->text_bg = 0x0000;
    g->text_scale = 1;
    g->text_transparent = 1;
}

/*void stm32_fillScreen(stm32boy_t *g, uint16_t color)
{
    (void)g;
    ili9486_clear_screen(color);
}*/


/**
 * Draws a line between two points (x0, y0) and (x1, y1) using a horizontal-major Bresenham algorithm.
 * 
 * @param g     Pointer to the stm32boy_t graphics context.
 * @param x0    Starting x-coordinate.
 * @param y0    Starting y-coordinate.
 * @param x1    Ending x-coordinate.
 * @param y1    Ending y-coordinate.
 * @param color Color to draw the line.
 */

void stm32_line_h(stm32boy_t *g, int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
   int16_t dx = x1 - x0;
   int16_t dy = y1 - y0;

   dx = abs(dx);
   dy = abs(dy);

   if (dx != 0){
    int16_t x = x0;
    int16_t y = y0;
    int16_t sx = (x1 > x0) ? 1 : -1;
    int16_t sy = (y1 > y0) ? 1 : -1;
    int16_t err = dx / 2;

    for (int16_t i = 0; i <= dx; i++) {
        stm32boy_drawPixel(g, x, y, color);
        x += sx;
        err -= dy;
        if (err < 0) {
            y += sy;
            err += dx;
        }
    }
   }
}

void stm32_line_v(stm32boy_t *g, int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{  

   int16_t dx = x1 - x0;
   int16_t dy = y1 - y0;

   dx = abs(dx);
   dy = abs(dy);

   if (dy != 0){
    int16_t x = x0;
    int16_t y = y0;
    int16_t sx = (x1 > x0) ? 1 : -1;
    int16_t sy = (y1 > y0) ? 1 : -1;
    int16_t err = dy / 2;

    for (int16_t i = 0; i <= dy; i++){
        stm32boy_drawPixel(g, x, y, color);
        y += sy;
        err -= dx;
        if (err < 0){
            x += sx;
            err += dy;
        }
    }
   }
}
void stm32_Line(stm32boy_t *g, int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
    if (abs(x1 - x0) > abs(y1 - y0)) {
        stm32_line_h(g, x0, y0, x1, y1, color);
    } else {
        stm32_line_v(g, x0, y0, x1, y1, color);
    }
}

void stm32_fillRect(stm32boy_t *g, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    if (w <= 0 || h <= 0) return;

    // Clipping simple
    int16_t x0 = x;
    int16_t y0 = y;
    int16_t x1 = x + w - 1;
    int16_t y1 = y + h - 1;

    if (x1 < 0 || y1 < 0) return;
    if (x0 >= (int16_t)g->width || y0 >= (int16_t)g->height) return;

    x0 = clamp16(x0, 0, (int16_t)g->width - 1);
    y0 = clamp16(y0, 0, (int16_t)g->height - 1);
    x1 = clamp16(x1, 0, (int16_t)g->width - 1);
    y1 = clamp16(y1, 0, (int16_t)g->height - 1);

    uint16_t ww = (uint16_t)(x1 - x0 + 1);
    uint16_t hh = (uint16_t)(y1 - y0 + 1);

    ili9486_set_addr_window((uint16_t)x0, (uint16_t)y0, (uint16_t)x1, (uint16_t)y1);
    ili9486_begin_pixels();
    ili9486_push_color(color, (uint32_t)ww * (uint32_t)hh);
    ili9486_end_pixels();
}

void stm32_drawRect(stm32boy_t *g, int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    if (w <= 0 || h <= 0) return;

    // Bordes: 4 rectángulos finos
    stm32_fillRect(g, x, y, w, 1, color);
    stm32_fillRect(g, x, y + h - 1, w, 1, color);
    stm32_fillRect(g, x, y, 1, h, color);
    stm32_fillRect(g, x + w - 1, y, 1, h, color);
}

void stm32_triangle(stm32boy_t *g, int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
    stm32_Line(g, x0, y0, x1, y1, color);
    stm32_Line(g, x1, y1, x2, y2, color);
    stm32_Line(g, x2, y2, x0, y0, color);
}

void stm32_polygon(stm32boy_t *g, const int16_t *points, uint16_t num_points, uint16_t color)
{
    if (num_points < 2) return;

    for (uint16_t i = 0; i < num_points; i++) {
        int16_t x0 = points[2 * i];
        int16_t y0 = points[2 * i + 1];
        int16_t x1 = points[2 * ((i + 1) % num_points)];
        int16_t y1 = points[2 * ((i + 1) % num_points) + 1];
        stm32_Line(g, x0, y0, x1, y1, color);
    }
}


void stm32_drawBitmapRGB565(stm32boy_t *g, int16_t x, int16_t y, int16_t w, int16_t h,
                            const uint16_t *pixels)
{
    ili9486_set_addr_window((uint16_t)x, (uint16_t)y, (uint16_t)(x + w - 1), (uint16_t)(y + h - 1));
    ili9486_begin_pixels();
    ili9486_push_pixels_rgb565(pixels, (uint32_t)w * (uint32_t)h);
    ili9486_end_pixels();
}




void stm32_write(stm32boy_t *g, const char *s)
{


    const uint8_t scale = (g->text_scale == 0) ? 1 : g->text_scale;
    const int32_t advance_x = 6 * (int32_t)scale;
    const int32_t advance_y = 8 * (int32_t)scale;

    int32_t cx = (int32_t)g->cursor_x;
    int32_t cy = (int32_t)g->cursor_y;

    while (*s) {
        char c = *s++; // avanza el puntero hacia el siguiente carácter
        if (c == '\r') continue; // ignorar retorno de carro

        if (c == '\n') {
            cx = 0; // volver al inicio de línea
            cy -= advance_y;                 // cy = cy - advance_y; <-- BAJAR línea
            if (cy < 0 || cy > (int32_t)g->height) break;   //si se sale del área de dibujo salir del bucle 
            continue;
        }

        // wrap horizontal
        if (cx + advance_x > (int32_t)g->width) { //comprobamos si el cursor excede el tamaño de la pantalla
            cx = 0;
            cy -= advance_y;                 // <-- BAJAR línea
            if (cy < 0) break;
        }

        g->cursor_x = (uint16_t)cx;
        g->cursor_y = (uint16_t)cy;

        stm32_drawChar(g, c);

        cx += advance_x;
    }

    // persistir cursor
    if (cx < 0) cx = 0;
    if (cy < 0) cy = 0;
    g->cursor_x = (uint16_t)cx;
    g->cursor_y = (uint16_t)cy;
}

void stm32_set_text_cursor(stm32boy_t *g, uint16_t x, uint16_t y) 
{ 
    g->cursor_x = x; 
    g->cursor_y = y; 
}


text_size_t stm32_measure_text_wrap(stm32boy_t *g, const char *s)
{
    text_size_t out = {0, 0};
    if (!g || !s) return out;

    uint16_t scale = (g->text_scale == 0) ? 1 : g->text_scale;
    uint16_t adv_x = (uint16_t)(6 * scale);
    uint16_t adv_y = (uint16_t)(8 * scale);

    uint16_t line_w = 0, max_w = 0;
    uint16_t lines = 1;

    while (*s) {
        char c = *s++;
        if (c == '\r') continue;

        if (c == '\n') {
            if (line_w > max_w) max_w = line_w;
            line_w = 0;
            lines++ ;
            continue;
        }

        if (line_w + adv_x > g->width) {
            if (line_w > max_w) max_w = line_w;
            line_w = 0;
            lines++;
        }

        line_w = (uint16_t)(line_w + adv_x);
    }

    if (line_w > max_w) max_w = line_w;

    out.w = max_w;
    out.h = (uint16_t)(lines * adv_y);
    return out;
}



void stm32_set_text_color(stm32boy_t *g, uint16_t fg, uint16_t bg, uint8_t transparent)
{
    g->text_fg = fg; 
    g->text_bg = bg; 
    g->text_transparent = transparent;
}

void stm32_set_text_scale(stm32boy_t *g, uint8_t scale) { g->text_scale = (scale == 0) ? 1 : scale; }



void stm32_drawChar(stm32boy_t *g, char c)
{
    if (!g) return; // comprobamos puntero, si es nulo salimos

    if ((uint8_t)c < 32 || (uint8_t)c > 127) c = '?'; // carácter no representable menos que 32 mas que 127
    const uint8_t *glyph = font5x7[(uint8_t)c - 32];
    const uint8_t scale = (g->text_scale == 0) ? 1 : g->text_scale;
    const int16_t x0 = (int16_t)g->cursor_x;
    const int16_t y0 = (int16_t)g->cursor_y;

    if (!g->text_transparent) {
        stm32_fillRect(g, x0, y0, (int16_t)(6 * scale), (int16_t)(8 * scale), g->text_bg);
    }

    for (uint8_t col = 0; col < 5; ++col) {
        uint8_t bits = glyph[col];
        for (uint8_t row = 0; row < 8; ++row) {
            if (bits & 0x80) { // MSB
                int16_t px = x0 + (int16_t)col * (int16_t)scale;
                int16_t py = y0 + (int16_t)row * (int16_t)scale;
                if (scale == 1) stm32boy_drawPixel(g, px, py, g->text_fg);
                else stm32_fillRect(g, px, py, scale, scale, g->text_fg);
            }
            bits <<= 1;
        }
    }

}

void stm32_write_at(stm32boy_t *g, uint16_t x, uint16_t y, const char *s)
{
    if (!g || !s) return;
    stm32_set_text_cursor(g, x, y);
    stm32_write(g, s);
}




void stm32boy_drawPixel(stm32boy_t *g, int16_t x, int16_t y, stm32boy_color_t color)
{
    if (x < 0 || y < 0) return;
    if (x >= (int16_t)g->width || y >= (int16_t)g->height) return;

    ili9486_set_addr_window((uint16_t)x, (uint16_t)y, (uint16_t)x, (uint16_t)y);
    ili9486_begin_pixels();
    ili9486_push_color(color, 1);
    ili9486_end_pixels();
}

void stm32boy_drawFastHLine(stm32boy_t *g, int16_t x, int16_t y, int16_t w, stm32boy_color_t color)
{
    // línea horizontal = rectángulo de alto 1
    stm32_fillRect(g, x, y, w, 1, color);
}

void stm32boy_drawFastVLine(stm32boy_t *g, int16_t x, int16_t y, int16_t h, stm32boy_color_t color)
{
    // línea vertical = rectángulo de ancho 1
    stm32_fillRect(g, x, y, 1, h, color);
}
