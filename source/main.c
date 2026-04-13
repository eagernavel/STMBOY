#include <stdint.h>
#include <stdio.h>

#include "bsp/bsp.h"
#include "graphics/stm32boy.h"          /* primitivas gráficas + colores */
#include "game_engine/actor.h"           /* actor_t, actor_init/update/bounce */
#include "game_engine/animation.h"       /* sprite_anim_get_frame */
#include "platform/nucleof411re/systick.h"
#include "stm32f411xe.h"

/* Colores ya definidos en common/types.h (vía stm32boy.h) */


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

    /* Dibujado inicial del sprite (inline de la lógica que antes era
     * sprite_anim_draw — ahora la app orquesta graphics + game_engine) */
    {
        const sprite_t *frame = sprite_anim_get_frame(&player.anim);
        if (frame) {
            stm32_sprite(&g, player.x, player.y, frame);
        }
    }

    while (1)
    {
        now_ms = systick_millis();

        if (actor_update(&player, now_ms))
        {
            actor_bounce_on_limits(&player, ILI9486_WIDTH, ILI9486_HEIGHT);

            /* Lógica de dibujo del actor (inline de actor_draw):
             * 1. Borrar posición anterior con color de fondo.
             * 2. Dibujar frame actual en posición nueva.            */
            stm32_fillRect(&g, player.prev_x, player.prev_y,
                           player.w, player.h, COLOR_BLACK);
            {
                const sprite_t *frame = sprite_anim_get_frame(&player.anim);
                if (frame) {
                    stm32_sprite(&g, player.x, player.y, frame);
                }
            }
        }
    }

    return 0;
}
