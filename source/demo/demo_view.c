#include "demo_view.h"

#define BACKGROUND_COLOR 0x0000U
#define MUTED_COLOR      0x7BEFU
#define CYAN_COLOR       0x07FFU

#define STATUS_BUFFER_SIZE 32U

static void write_centered(stm32boy_t *graphics,
                           uint16_t y,
                           const char *text)
{
    text_size_t size = stm32boy_measure_text_wrap(graphics, text);
    uint16_t x = size.w < graphics->width ?
                 (uint16_t)((graphics->width - size.w) / 2U) : 0U;
    stm32boy_write_at(graphics, x, y, text);
}

static uint8_t append_text(char buffer[STATUS_BUFFER_SIZE],
                           uint8_t position,
                           const char *text)
{
    while (*text != '\0' && position < STATUS_BUFFER_SIZE - 1U) {
        buffer[position++] = *text++;
    }
    return position;
}

static uint8_t append_u16(char buffer[STATUS_BUFFER_SIZE],
                          uint8_t position,
                          uint16_t value)
{
    uint16_t divisor = 10000U;
    bool started = false;

    while (divisor > 0U && position < STATUS_BUFFER_SIZE - 1U) {
        uint8_t digit = (uint8_t)(value / divisor);
        if (digit != 0U || started || divisor == 1U) {
            buffer[position++] = (char)('0' + digit);
            started = true;
        }
        value = (uint16_t)(value % divisor);
        divisor = (uint16_t)(divisor / 10U);
    }
    return position;
}

static void format_status(const demo_model_t *model,
                          char buffer[STATUS_BUFFER_SIZE])
{
    uint8_t position = 0U;
    position = append_text(buffer, position, "SPEED ");
    position = append_u16(buffer, position, model->speed);
    position = append_text(buffer, position, "  BOUNCES ");
    position = append_u16(buffer, position, model->bounce_count);
    buffer[position] = '\0';
}

static void draw_hud(stm32boy_t *graphics, const demo_model_t *model)
{
    int16_t hud_y = model->playfield_height;
    int16_t hud_height = (int16_t)graphics->height - hud_y;
    char status[STATUS_BUFFER_SIZE];
    format_status(model, status);

    stm32boy_fill_rect(graphics, 0, hud_y,
                       (int16_t)graphics->width, hud_height,
                       BACKGROUND_COLOR);
    stm32boy_draw_fast_hline(graphics, 0, hud_y,
                             (int16_t)graphics->width, CYAN_COLOR);

    stm32boy_set_text_scale(graphics, 1);
    stm32boy_set_text_color(graphics, COLOR_WHITE,
                            BACKGROUND_COLOR, 0);
    write_centered(graphics, (uint16_t)(hud_y + 10), status);

    stm32boy_set_text_color(graphics, MUTED_COLOR,
                            BACKGROUND_COLOR, 0);
    write_centered(graphics, (uint16_t)(hud_y + 34),
                   "UP/DOWN: SPEED");

    stm32boy_set_text_color(graphics,
                            model->paused ? COLOR_YELLOW : COLOR_GREEN,
                            BACKGROUND_COLOR, 0);
    write_centered(graphics, (uint16_t)(hud_y + 58),
                   model->paused ? "START: RESUME" : "START: PAUSE");
}

static void draw_actor(stm32boy_t *graphics, const demo_model_t *model)
{
    const sprite_t *frame = sprite_anim_get_frame(&model->actor.anim);
    if (!frame) return;

    stm32boy_draw_sprite(graphics,
                         model->actor.x,
                         model->actor.y,
                         frame);
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

    uint16_t frame = model->actor.anim.current_frame;
    bool actor_changed = !view->initialized ||
                         view->actor_x != model->actor.x ||
                         view->actor_y != model->actor.y ||
                         view->actor_frame != frame;
    bool hud_changed = !view->initialized ||
                       view->bounce_count != model->bounce_count ||
                       view->speed != model->speed ||
                       view->paused != model->paused;

    if (!view->initialized) {
        stm32boy_fill_screen(graphics, BACKGROUND_COLOR);
    } else if (actor_changed) {
        stm32boy_fill_rect(graphics,
                           view->actor_x,
                           view->actor_y,
                           model->actor.w,
                           model->actor.h,
                           BACKGROUND_COLOR);
    }

    if (actor_changed) {
        draw_actor(graphics, model);
    }
    if (hud_changed) {
        draw_hud(graphics, model);
    }

    view->initialized = true;
    view->actor_x = model->actor.x;
    view->actor_y = model->actor.y;
    view->actor_frame = frame;
    view->bounce_count = model->bounce_count;
    view->speed = model->speed;
    view->paused = model->paused;
}
