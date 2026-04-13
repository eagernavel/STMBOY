#include "animation.h"

void sprite_anim_init(sprite_anim_t *anim,
                      const sprite_t *frames,
                      uint16_t frame_count,
                      uint32_t frame_duration_ms,
                      uint32_t now_ms)
{
    if (!anim || !frames || frame_count == 0U) {
        return;
    }

    anim->frames = frames;
    anim->frame_count = frame_count;
    anim->current_frame = 0U;
    anim->frame_duration_ms = frame_duration_ms;
    anim->last_tick_ms = now_ms;
}

void sprite_anim_reset(sprite_anim_t *anim, uint32_t now_ms)
{
    if (!anim) {
        return;
    }

    anim->current_frame = 0U;
    anim->last_tick_ms = now_ms;
}

const sprite_t *sprite_anim_get_frame(const sprite_anim_t *anim)
{
    if (!anim || !anim->frames || anim->frame_count == 0U) {
        return 0;
    }

    if (anim->current_frame >= anim->frame_count) {
        return 0;
    }

    return &anim->frames[anim->current_frame];
}

uint8_t sprite_anim_update(sprite_anim_t *anim, uint32_t now_ms)
{
    if (!anim || !anim->frames || anim->frame_count == 0U) {
        return 0U;
    }

    if ((now_ms - anim->last_tick_ms) >= anim->frame_duration_ms) {
        anim->current_frame++;

        if (anim->current_frame >= anim->frame_count) {
            anim->current_frame = 0U;
        }

        anim->last_tick_ms = now_ms;
        return 1U;
    }

    return 0U;
}
