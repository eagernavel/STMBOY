#include "demo_view.h"

#define BACKGROUND_COLOR 0x0000U
#define PANEL_COLOR      0x2104U
#define MUTED_COLOR      0x7BEFU
#define CYAN_COLOR       0x07FFU
#define MAGENTA_COLOR    0xF81FU

#define TRACK_X          42
#define TRACK_Y          132
#define TRACK_W          36
#define TRACK_H          214
#define INDICATOR_H      12

static const uint16_t s_palette[DEMO_PALETTE_COUNT] = {
    COLOR_GREEN,
    CYAN_COLOR,
    COLOR_YELLOW,
    MAGENTA_COLOR,
};

static uint16_t accent_color(const demo_model_t *model)
{
    return s_palette[model->palette % DEMO_PALETTE_COUNT];
}

static void write_centered(stm32boy_t *graphics,
                           uint16_t y,
                           const char *text)
{
    text_size_t size = stm32boy_measure_text_wrap(graphics, text);
    uint16_t x = size.w < graphics->width ?
                 (uint16_t)((graphics->width - size.w) / 2U) : 0U;
    stm32boy_write_at(graphics, x, y, text);
}

static int16_t indicator_y(uint8_t level)
{
    int16_t travel = TRACK_H - INDICATOR_H - 8;
    int16_t offset = (int16_t)(((uint32_t)level * (uint32_t)travel) /
                               DEMO_LEVEL_MAX);
    return (int16_t)(TRACK_Y + TRACK_H - INDICATOR_H - 4 - offset);
}

static void format_level(uint8_t level, char text[4])
{
    text[0] = level >= 100U ? '1' : ' ';
    text[1] = level >= 100U ? '0' :
              level >= 10U ? (char)('0' + (level / 10U)) : ' ';
    text[2] = level >= 100U ? '0' : (char)('0' + (level % 10U));
    text[3] = '\0';
}

static void draw_heartbeat(stm32boy_t *graphics, const demo_model_t *model)
{
    uint16_t color = model->heartbeat_on ? COLOR_GREEN : PANEL_COLOR;
    stm32boy_fill_rect(graphics, (int16_t)graphics->width - 26, 18,
                       10, 10, color);
}

static void draw_level(stm32boy_t *graphics, const demo_model_t *model)
{
    uint16_t accent = accent_color(model);
    char level_text[4];
    format_level(model->level, level_text);

    stm32boy_fill_rect(graphics, TRACK_X + 4, TRACK_Y + 4,
                       TRACK_W - 8, TRACK_H - 8, BACKGROUND_COLOR);
    stm32boy_fill_rect(graphics, TRACK_X + 4, indicator_y(model->level),
                       TRACK_W - 8, INDICATOR_H, accent);

    stm32boy_fill_rect(graphics, 102, 302, 88, 22, BACKGROUND_COLOR);
    stm32boy_set_text_scale(graphics, 2);
    stm32boy_set_text_color(graphics, accent, BACKGROUND_COLOR, 0);
    stm32boy_write_at(graphics, 108, 304, level_text);
    stm32boy_set_text_scale(graphics, 1);
    stm32boy_write_at(graphics, 150, 312, "%");
}

static void draw_full(stm32boy_t *graphics, const demo_model_t *model)
{
    uint16_t accent = accent_color(model);

    stm32boy_fill_screen(graphics, BACKGROUND_COLOR);
    stm32boy_draw_rect(graphics, 6, 6,
                       (int16_t)graphics->width - 12,
                       (int16_t)graphics->height - 12,
                       accent);

    stm32boy_set_text_scale(graphics, 2);
    stm32boy_set_text_color(graphics, COLOR_WHITE, BACKGROUND_COLOR, 0);
    write_centered(graphics, 24, "STMBOY");
    stm32boy_set_text_scale(graphics, 1);
    stm32boy_set_text_color(graphics, accent, BACKGROUND_COLOR, 0);
    write_centered(graphics, 56, "DEMOSTRACION API GRAFICA");
    stm32boy_draw_fast_hline(graphics, 24, 82,
                             (int16_t)graphics->width - 48, accent);

    stm32boy_set_text_color(graphics, MUTED_COLOR, BACKGROUND_COLOR, 0);
    stm32boy_write_at(graphics, 24, 98, "ENTRADA + TIEMPO + DIBUJO");

    stm32boy_fill_rect(graphics, 24, 118, 272, 244, PANEL_COLOR);
    stm32boy_fill_rect(graphics, 28, 122, 264, 236, BACKGROUND_COLOR);

    stm32boy_draw_rect(graphics, TRACK_X, TRACK_Y, TRACK_W, TRACK_H, accent);

    stm32boy_set_text_color(graphics, COLOR_WHITE, BACKGROUND_COLOR, 0);
    stm32boy_write_at(graphics, 105, 142, "PRIMITIVAS");
    stm32boy_draw_rect(graphics, 108, 166, 62, 42, accent);
    stm32boy_fill_rect(graphics, 184, 166, 62, 42, accent);
    stm32boy_draw_line(graphics, 108, 226, 246, 266, COLOR_WHITE);
    stm32boy_draw_triangle(graphics, 108, 274, 150, 230, 192, 274, accent);

    stm32boy_fill_rect(graphics, 214, 286, 22, 16, COLOR_RED);
    stm32boy_fill_rect(graphics, 240, 286, 22, 16, COLOR_GREEN);
    stm32boy_fill_rect(graphics, 266, 286, 16, 16, COLOR_BLUE);

    stm32boy_set_text_color(graphics, MUTED_COLOR, BACKGROUND_COLOR, 0);
    stm32boy_write_at(graphics, 105, 286, "NIVEL");
    draw_level(graphics, model);

    stm32boy_set_text_color(graphics, COLOR_WHITE, BACKGROUND_COLOR, 0);
    write_centered(graphics, 390, "ARRIBA / ABAJO: NIVEL");
    stm32boy_set_text_color(graphics, accent, BACKGROUND_COLOR, 0);
    write_centered(graphics, 414, "INICIO: CAMBIAR COLOR");
    stm32boy_set_text_color(graphics, MUTED_COLOR, BACKGROUND_COLOR, 0);
    write_centered(graphics, 446, "RENDERIZADO RGB565 DIRECTO");

    draw_heartbeat(graphics, model);
}

void demo_view_init(demo_view_t *view)
{
    if (!view) return;
    *view = (demo_view_t) {0};
}

void demo_view_render(stm32boy_t *graphics,
                      const demo_model_t *model,
                      demo_view_t *view)
{
    if (!graphics || !model || !view) return;

    if (!view->initialized || view->palette != model->palette) {
        draw_full(graphics, model);
    } else {
        if (view->level != model->level) {
            draw_level(graphics, model);
        }
        if (view->heartbeat_on != model->heartbeat_on) {
            draw_heartbeat(graphics, model);
        }
    }

    view->initialized = true;
    view->level = model->level;
    view->palette = model->palette;
    view->heartbeat_on = model->heartbeat_on;
}
