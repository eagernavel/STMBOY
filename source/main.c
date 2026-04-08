#include <stdint.h>
#include <stdio.h>

#include "bsp/bsp.h"
#include "platform/nucleof411re/stm32boy.h"
#include "platform/nucleof411re/systick.h"
#include "stm32f411xe.h"

//* Colores RGB565 */
#define COLOR_BLACK   0x0000
#define COLOR_WHITE   0xFFFF
#define COLOR_RED     0xF800
#define COLOR_GREEN   0x07E0
#define COLOR_BLUE    0x001F
#define COLOR_YELLOW  0xFFE0


#define ILI9486_WIDTH   320
#define ILI9486_HEIGHT  480

static inline void raw_delay(volatile uint32_t count) {
    while (count--) __asm__("nop");
}


/* ------------------------------------------------------------------------- */
/* Frames de ejemplo: sprite 8x8 con dos estados                             */
/* ------------------------------------------------------------------------- */

static const uint16_t sprite_frame_0[8 * 8] = {
    COLOR_BLACK, COLOR_BLACK, COLOR_RED,   COLOR_RED,   COLOR_RED,   COLOR_RED,   COLOR_BLACK, COLOR_BLACK,
    COLOR_BLACK, COLOR_RED,   COLOR_RED,   COLOR_RED,   COLOR_RED,   COLOR_RED,   COLOR_RED,   COLOR_BLACK,
    COLOR_RED,   COLOR_RED,   COLOR_YELLOW,COLOR_YELLOW,COLOR_YELLOW,COLOR_YELLOW,COLOR_RED,   COLOR_RED,
    COLOR_RED,   COLOR_YELLOW,COLOR_YELLOW,COLOR_WHITE, COLOR_WHITE, COLOR_YELLOW,COLOR_YELLOW,COLOR_RED,
    COLOR_RED,   COLOR_YELLOW,COLOR_YELLOW,COLOR_WHITE, COLOR_WHITE, COLOR_YELLOW,COLOR_YELLOW,COLOR_RED,
    COLOR_RED,   COLOR_RED,   COLOR_YELLOW,COLOR_YELLOW,COLOR_YELLOW,COLOR_YELLOW,COLOR_RED,   COLOR_RED,
    COLOR_BLACK, COLOR_RED,   COLOR_RED,   COLOR_RED,   COLOR_RED,   COLOR_RED,   COLOR_RED,   COLOR_BLACK,
    COLOR_BLACK, COLOR_BLACK, COLOR_RED,   COLOR_BLACK, COLOR_BLACK, COLOR_RED,   COLOR_BLACK, COLOR_BLACK
};

static const uint16_t sprite_frame_1[8 * 8] = {
    COLOR_BLACK, COLOR_BLACK, COLOR_BLUE,  COLOR_BLUE,  COLOR_BLUE,  COLOR_BLUE,  COLOR_BLACK, COLOR_BLACK,
    COLOR_BLACK, COLOR_BLUE,  COLOR_BLUE,  COLOR_BLUE,  COLOR_BLUE,  COLOR_BLUE,  COLOR_BLUE,  COLOR_BLACK,
    COLOR_BLUE,  COLOR_BLUE,  COLOR_YELLOW,COLOR_YELLOW,COLOR_YELLOW,COLOR_YELLOW,COLOR_BLUE,  COLOR_BLUE,
    COLOR_BLUE,  COLOR_YELLOW,COLOR_WHITE, COLOR_YELLOW,COLOR_YELLOW,COLOR_WHITE, COLOR_YELLOW,COLOR_BLUE,
    COLOR_BLUE,  COLOR_YELLOW,COLOR_WHITE, COLOR_YELLOW,COLOR_YELLOW,COLOR_WHITE, COLOR_YELLOW,COLOR_BLUE,
    COLOR_BLUE,  COLOR_BLUE,  COLOR_YELLOW,COLOR_YELLOW,COLOR_YELLOW,COLOR_YELLOW,COLOR_BLUE,  COLOR_BLUE,
    COLOR_BLACK, COLOR_BLUE,  COLOR_BLUE,  COLOR_BLUE,  COLOR_BLUE,  COLOR_BLUE,  COLOR_BLUE,  COLOR_BLACK,
    COLOR_BLACK, COLOR_BLACK, COLOR_BLUE,  COLOR_BLACK, COLOR_BLACK, COLOR_BLUE,  COLOR_BLACK, COLOR_BLACK
};

/* ------------------------------------------------------------------------- */
/* Sprites                                                                    */
/* ------------------------------------------------------------------------- */

static const sprite_t test_frames[] = {
    { .w = 8, .h = 8, .pixels = sprite_frame_0 },
    { .w = 8, .h = 8, .pixels = sprite_frame_1 }
};

/* ------------------------------------------------------------------------- */
/* Utilidad para borrar la zona anterior del sprite                           */
/* ------------------------------------------------------------------------- */

static void clear_sprite_area(stm32boy_t *g, int16_t x, int16_t y, int16_t w, int16_t h)
{
    stm32_fillRect(g, x, y, w, h, COLOR_BLACK);
}

static void actor_init(actor_t *a,
                       int16_t x,
                       int16_t y,
                       int16_t w,
                       int16_t h,
                       int16_t vx,
                       int16_t vy,
                       uint32_t move_interval_ms,
                       const sprite_t *frames,
                       uint16_t frame_count,
                       uint32_t frame_time_ms,
                       uint32_t now_ms)
{
    if (!a) return;

    a->x = x;
    a->y = y;
    a->prev_x = x;
    a->prev_y = y;
    a->w = w;
    a->h = h;
    a->vx = vx;
    a->vy = vy;
    a->last_move_ms = now_ms;
    a->move_interval_ms = move_interval_ms;

    sprite_anim_init(&a->anim, frames, frame_count, frame_time_ms, now_ms);
}


static void actor_draw(stm32boy_t *g, actor_t *a)
{
    if (!g || !a) return;

    /* Borra la zona anterior */
    stm32_fillRect(g, a->prev_x, a->prev_y, a->w, a->h, COLOR_BLACK);

    /* Dibuja el frame actual */
    sprite_anim_draw(g, a->x, a->y, &a->anim);
}

static uint8_t actor_update(actor_t *a, uint32_t now_ms)
{
    uint8_t changed = 0;

    if (!a) return 0;

    a->prev_x = a->x;
    a->prev_y = a->y;

    if (sprite_anim_update(&a->anim, now_ms)) {
        changed = 1;
    }

    if ((now_ms - a->last_move_ms) >= a->move_interval_ms)
    {
        a->last_move_ms = now_ms;

        a->x += a->vx;
        a->y += a->vy;

        changed = 1;
    }

    return changed;
}

static void actor_bounce_on_limits(actor_t *a, int16_t screen_w, int16_t screen_h)
{
    if (!a) return;

    if (a->x < 0) {
        a->x = 0;
        a->vx = -a->vx;
    }

    if ((a->x + a->w) > screen_w) {
        a->x = screen_w - a->w;
        a->vx = -a->vx;
    }

    if (a->y < 0) {
        a->y = 0;
        a->vy = -a->vy;
    }

    if ((a->y + a->h) > screen_h) {
        a->y = screen_h - a->h;
        a->vy = -a->vy;
    }
}
int main(void)
{
    bsp_init();
    systick_init(SystemCoreClock);

    stm32boy_t g;
    actor_t player;
    uint32_t now_ms;

    stm32boy_init(&g, ILI9486_WIDTH, ILI9486_HEIGHT);
    stm32_fillScreen(&g, COLOR_BLACK);

    now_ms = systick_millis();

    actor_init(&player, 100, 100, 8, 8, 1, 0, 40, test_frames, 2, 200, now_ms);

    sprite_anim_draw(&g, player.x, player.y, &player.anim);

    while (1)
    {
        now_ms = systick_millis();

        if (actor_update(&player, now_ms))
        {
            actor_bounce_on_limits(&player, ILI9486_WIDTH, ILI9486_HEIGHT);
            actor_draw(&g, &player);
        }
    }

    return 0;
}
