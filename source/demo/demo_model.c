#include "demo_model.h"

#include "demo_assets.h"

#define INITIAL_SPEED    2U
#define MOVE_INTERVAL_MS 24U
#define FRAME_TIME_MS    140U

static int16_t velocity_with_magnitude(int16_t velocity, uint8_t magnitude)
{
    return velocity < 0 ? -(int16_t)magnitude : (int16_t)magnitude;
}

static void apply_speed(demo_model_t *model)
{
    model->actor.vx = velocity_with_magnitude(model->actor.vx,
                                               model->speed);
    model->actor.vy = velocity_with_magnitude(model->actor.vy,
                                               model->speed);
}

void demo_model_init(demo_model_t *model,
                     int16_t playfield_width,
                     int16_t playfield_height,
                     uint32_t now_ms)
{
    if (!model) return;

    model->playfield_width = playfield_width;
    model->playfield_height = playfield_height;
    model->bounce_count = 0U;
    model->speed = INITIAL_SPEED;
    model->paused = false;

    actor_init(&model->actor,
               24,
               36,
               DEMO_ACTOR_WIDTH,
               DEMO_ACTOR_HEIGHT,
               INITIAL_SPEED,
               INITIAL_SPEED,
               MOVE_INTERVAL_MS,
               g_demo_actor_frames,
               DEMO_ACTOR_FRAME_COUNT,
               FRAME_TIME_MS,
               now_ms);
}

bool demo_model_update(demo_model_t *model,
                       const demo_input_t *input,
                       uint32_t now_ms)
{
    if (!model || !input) return false;

    bool changed = false;

    if (input->toggle_pause_pressed) {
        model->paused = !model->paused;
        if (!model->paused) {
            model->actor.last_move_ms = now_ms;
            sprite_anim_reset(&model->actor.anim, now_ms);
        }
        changed = true;
    }

    if (input->speed_up_pressed != input->speed_down_pressed) {
        uint8_t previous_speed = model->speed;
        if (input->speed_up_pressed && model->speed < DEMO_SPEED_MAX) {
            model->speed++;
        } else if (input->speed_down_pressed &&
                   model->speed > DEMO_SPEED_MIN) {
            model->speed--;
        }
        if (model->speed != previous_speed) {
            apply_speed(model);
            changed = true;
        }
    }

    if (model->paused) {
        return changed;
    }

    int16_t previous_vx = model->actor.vx;
    int16_t previous_vy = model->actor.vy;

    if (actor_update(&model->actor, now_ms)) {
        actor_bounce_on_limits(&model->actor,
                               model->playfield_width,
                               model->playfield_height);
        if (model->actor.vx != previous_vx ||
            model->actor.vy != previous_vy) {
            model->bounce_count++;
        }
        changed = true;
    }

    return changed;
}
