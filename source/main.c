#include <stdint.h>
#include <stdio.h>

#include "bsp/bsp.h"
#include "graphics/stm32boy.h"                                   /* primitivas gráficas + colores */
#include "game_engine/actor.h"                                    /* actor_t, actor_init/update/bounce */
#include "game_engine/animation.h"                                /* sprite_anim_get_frame */
#include "platform/nucleof411re/systick.h"
#include "platform/nucleof411re/platform_nucleof411re_ili9486.h"  /* driver concreto display */
#include "platform/nucleof411re/platform_nucleof411re_buttons.h"  /* driver concreto botones */
#include "stm32f411xe.h"

/* Colores ya definidos en common/types.h (vía stm32boy.h) */

/* -----------------------------------------------------------------------
 * Display HAL: inyección del driver ILI9486 concreto.
 * Solo main.c (capa de aplicación) conoce el driver concreto; graphics
 * solo trabaja con la interfaz abstracta display_hal_t.
 * ----------------------------------------------------------------------- */
static const display_hal_t ili9486_display = {
    .set_addr_window  = ili9486_set_addr_window,
    .begin_pixels     = ili9486_begin_pixels,
    .end_pixels       = ili9486_end_pixels,
    .push_color       = ili9486_push_color,
    .push_pixels_rgb565 = ili9486_push_pixels_rgb565,
};

#define SCREEN_WIDTH   320
#define SCREEN_HEIGHT  480


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

    stm32boy_init(&g, SCREEN_WIDTH, SCREEN_HEIGHT, &ili9486_display);
    stm32boy_fill_screen(&g, COLOR_BLACK);

    now_ms = systick_millis();

    actor_init(&player, 100, 100, 8, 8, 1, 0, 40, test_frames, 2, 200, now_ms);

    /* Dibujado inicial */
    {
        const sprite_t *frame = sprite_anim_get_frame(&player.anim);
        if (frame) {
            stm32boy_draw_sprite(&g, player.x, player.y, frame);
        }
    }

    /* Color de fondo actual — START lo alterna */
    uint16_t bg_color = COLOR_BLACK;

    while (1)
    {
        now_ms = systick_millis();

        /* ---- 1. Leer botones ----------------------------------------- */
        platform_nucleof411re_buttons_update();
        const button_state_t *btn = platform_nucleof411re_buttons_get();

        /* ---- 2. Entrada → modificar velocidad del actor --------------- */
        /*
         * UP   presionado → mover hacia arriba  (vy = -1)
         * DOWN presionado → mover hacia abajo   (vy = +1)
         * Sujetar mantenido mantiene la dirección; soltar detiene el eje Y.
         *
         * Nota: vy = 0 no detiene el actor completamente si vx != 0,
         * solo detiene el movimiento vertical.
         */
        if (btn->pressed[BTN_UP]) {
            player.vy = -1;
        }
        if (btn->pressed[BTN_DOWN]) {
            player.vy = 1;
        }
        if (btn->released[BTN_UP] || btn->released[BTN_DOWN]) {
            player.vy = 0;
        }

        /* START (flanco): alternar color de fondo y limpiar pantalla */
        if (btn->pressed[BTN_START]) {
            bg_color = (bg_color == COLOR_BLACK) ? COLOR_GREEN : COLOR_BLACK;
            stm32boy_fill_screen(&g, bg_color);
        }

        /* ---- 3. Lógica + dibujo -------------------------------------- */
        if (actor_update(&player, now_ms))
        {
            actor_bounce_on_limits(&player, g.width, g.height);

            stm32boy_fill_rect(&g, player.prev_x, player.prev_y,
                               player.w, player.h, bg_color);
            {
                const sprite_t *frame = sprite_anim_get_frame(&player.anim);
                if (frame) {
                    stm32boy_draw_sprite(&g, player.x, player.y, frame);
                }
            }
        }
    }

    return 0;
}
