#include "actor.h"

void actor_init(actor_t *a,
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

uint8_t actor_update(actor_t *a, uint32_t now_ms)
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

void actor_bounce_on_limits(actor_t *a, int16_t screen_w, int16_t screen_h)
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
