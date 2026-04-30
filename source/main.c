#define DEBUG 1    /* Comenta esta linea para desactivar mensajes UART */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/* -----------------------------------------------------------------------
 * Macro de debug por UART.
 * Activa con: #define DEBUG  (encima de este bloque)
 * Uso:  DBG("valor=%d", x);
 * ----------------------------------------------------------------------- */
#ifdef DEBUG
#  define DBG(fmt, ...)  printf("[DBG] " fmt "\r\n", ##__VA_ARGS__)
#else
#  define DBG(fmt, ...)  ((void)0)
#endif

#include "bsp/bsp.h"
#include "graphics/stm32boy.h"
#include "platform/nucleof411re/systick.h"
#include "platform/nucleof411re/platform_nucleof411re_ili9486.h"
#include "platform/nucleof411re/platform_nucleof411re_buttons.h"
#include "stm32f411xe.h"

static const display_hal_t ili9486_display = {
    .set_addr_window    = ili9486_set_addr_window,
    .begin_pixels       = ili9486_begin_pixels,
    .end_pixels         = ili9486_end_pixels,
    .push_color         = ili9486_push_color,
    .push_pixels_rgb565 = ili9486_push_pixels_rgb565,
};

/* -----------------------------------------------------------------------
 * Dimensiones de pantalla
 * ----------------------------------------------------------------------- */
#define SCREEN_W   320
#define SCREEN_H   480

/* -----------------------------------------------------------------------
 * Parametros del juego
 *
 *  Pantalla vertical (320x480).
 *  Paddle izquierdo  -> jugador (BTN_UP / BTN_DOWN).
 *  Paddle derecho    -> CPU sigue la bola con velocidad limitada.
 *  Primero en llegar a 7 puntos gana.
 * ----------------------------------------------------------------------- */
#define PADDLE_W        10
#define PADDLE_H        60
#define PADDLE_MARGIN    8
#define PADDLE_SPEED     4

#define BALL_SIZE        10

#define SCORE_Y          10
#define CENTER_LINE_X   (SCREEN_W / 2)

#define FRAME_MS        33u

#define BALL_VX_INIT     4
#define BALL_VY_INIT     3

#define WIN_SCORE        7

typedef enum {
    MODE_TITLE = 0,
    MODE_RUNNING,
    MODE_PAUSED,
    MODE_POINT,
    MODE_GAME_OVER
} pong_mode_t;

typedef struct {
    int16_t ball_x,  ball_y;
    int16_t ball_vx, ball_vy;

    int16_t paddle_l_y;
    int16_t paddle_r_y;

    uint8_t score_l;
    uint8_t score_r;

    pong_mode_t mode;
    uint32_t    last_tick_ms;
    uint32_t    point_start_ms;
} pong_t;

typedef struct {
    bool        initialized;
    int16_t     ball_x,      ball_y;
    int16_t     paddle_l_y,  paddle_r_y;
    uint8_t     score_l,     score_r;
    pong_mode_t mode;
} render_prev_t;

#define PADDLE_L_X   PADDLE_MARGIN
#define PADDLE_R_X   (SCREEN_W - PADDLE_MARGIN - PADDLE_W)

static inline int16_t clamp_i16(int16_t v, int16_t lo, int16_t hi)
{
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

/* -----------------------------------------------------------------------
 * Logica
 * ----------------------------------------------------------------------- */
static void prv_ball_reset(pong_t *p, int8_t dx, int8_t dy)
{
    p->ball_x  = (SCREEN_W - BALL_SIZE) / 2;
    p->ball_y  = (SCREEN_H - BALL_SIZE) / 2;
    p->ball_vx = (int16_t)(BALL_VX_INIT * dx);
    p->ball_vy = (int16_t)(BALL_VY_INIT * dy);
    DBG("BALL RESET  vx=%d vy=%d", p->ball_vx, p->ball_vy);
}

static void prv_game_reset(pong_t *p, uint32_t now_ms)
{
    p->paddle_l_y   = (SCREEN_H - PADDLE_H) / 2;
    p->paddle_r_y   = (SCREEN_H - PADDLE_H) / 2;
    p->score_l      = 0;
    p->score_r      = 0;
    p->mode         = MODE_RUNNING;
    p->last_tick_ms = now_ms;
    prv_ball_reset(p, 1, 1);
    DBG("--- GAME START  t=%lu ---", (unsigned long)now_ms);
}

static bool prv_ball_hits(const pong_t *p, int16_t px, int16_t py)
{
    bool x_ok = (p->ball_x < px + PADDLE_W) && (p->ball_x + BALL_SIZE > px);
    bool y_ok = (p->ball_y < py + PADDLE_H) && (p->ball_y + BALL_SIZE > py);
    return x_ok && y_ok;
}

static bool prv_update(pong_t *p, const button_state_t *btn, uint32_t now_ms)
{
    /* --- Pantallas de espera --- */
    if (p->mode == MODE_TITLE || p->mode == MODE_GAME_OVER) {
        if (btn->pressed[BTN_START]) {
            prv_game_reset(p, now_ms);
            return true;
        }
        return false;
    }

    if (p->mode == MODE_PAUSED) {
        if (btn->pressed[BTN_START]) {
            p->mode = MODE_RUNNING;
            p->last_tick_ms = now_ms;
            DBG("RESUME");
            return true;
        }
        return false;
    }

    if (p->mode == MODE_POINT) {
        if ((now_ms - p->point_start_ms) >= 800u) {
            p->mode = MODE_RUNNING;
            p->last_tick_ms = now_ms;
        }
        return false;
    }

    /* --- Pausa con START durante el juego --- */
    if (btn->pressed[BTN_START]) {
        p->mode = MODE_PAUSED;
        DBG("PAUSED");
        return true;
    }

    if ((now_ms - p->last_tick_ms) < FRAME_MS) {
        return false;
    }
    p->last_tick_ms = now_ms;

    /* --- Mover paddle jugador (una vez por tick) --- */
    if (btn->held[BTN_UP]) {
        p->paddle_l_y = clamp_i16((int16_t)(p->paddle_l_y - PADDLE_SPEED), 0, SCREEN_H - PADDLE_H);
    }
    if (btn->held[BTN_DOWN]) {
        p->paddle_l_y = clamp_i16((int16_t)(p->paddle_l_y + PADDLE_SPEED), 0, SCREEN_H - PADDLE_H);
    }

    /* --- CPU --- */
    int16_t ball_cy  = (int16_t)(p->ball_y + BALL_SIZE / 2);
    int16_t cpu_cy   = (int16_t)(p->paddle_r_y + PADDLE_H / 2);
    int16_t diff     = (int16_t)(ball_cy - cpu_cy);
    int16_t cpu_move = (diff > 0) ? PADDLE_SPEED : (diff < 0 ? -PADDLE_SPEED : 0);
    p->paddle_r_y    = clamp_i16((int16_t)(p->paddle_r_y + cpu_move), 0, SCREEN_H - PADDLE_H);

    /* --- Mover bola --- */
    p->ball_x = (int16_t)(p->ball_x + p->ball_vx);
    p->ball_y = (int16_t)(p->ball_y + p->ball_vy);

    /* Rebote paredes horizontales */
    if (p->ball_y <= 0) {
        p->ball_y  = 0;
        p->ball_vy = (int16_t)(-p->ball_vy);
        DBG("REBOTE superior");
    }
    if (p->ball_y + BALL_SIZE >= SCREEN_H) {
        p->ball_y  = (int16_t)(SCREEN_H - BALL_SIZE);
        p->ball_vy = (int16_t)(-p->ball_vy);
        DBG("REBOTE inferior");
    }

    /* Rebote paddle jugador */
    if (p->ball_vx < 0 && prv_ball_hits(p, PADDLE_L_X, p->paddle_l_y)) {
        p->ball_x  = (int16_t)(PADDLE_L_X + PADDLE_W);
        p->ball_vx = (int16_t)(-p->ball_vx);
        DBG("REBOTE jugador  ball_y=%d paddle_y=%d", p->ball_y, p->paddle_l_y);
    }

    /* Rebote paddle CPU */
    if (p->ball_vx > 0 && prv_ball_hits(p, PADDLE_R_X, p->paddle_r_y)) {
        p->ball_x  = (int16_t)(PADDLE_R_X - BALL_SIZE);
        p->ball_vx = (int16_t)(-p->ball_vx);
        DBG("REBOTE CPU      ball_y=%d paddle_y=%d", p->ball_y, p->paddle_r_y);
    }

    /* Punto */
    if (p->ball_x + BALL_SIZE < 0) {
        p->score_r++;
        DBG("PUNTO CPU  %d-%d", p->score_l, p->score_r);
        prv_ball_reset(p, 1, 1);
        p->mode = MODE_POINT;
        p->point_start_ms = now_ms;
    } else if (p->ball_x > SCREEN_W) {
        p->score_l++;
        DBG("PUNTO JUGADOR  %d-%d", p->score_l, p->score_r);
        prv_ball_reset(p, -1, -1);
        p->mode = MODE_POINT;
        p->point_start_ms = now_ms;
    }

    /* Game over */
    if (p->score_l >= WIN_SCORE || p->score_r >= WIN_SCORE) {
        p->mode = MODE_GAME_OVER;
        DBG("GAME OVER  jugador=%d  CPU=%d", p->score_l, p->score_r);
    }

    return true;
}

/* -----------------------------------------------------------------------
 * Render
 * ----------------------------------------------------------------------- */
static void prv_draw_center_line(stm32boy_t *g)
{
    for (int16_t y = 40; y < SCREEN_H; y += 20) {
        stm32boy_fill_rect(g, CENTER_LINE_X - 1, y, 2, 10, 0x39E7u);
    }
}

static void prv_draw_score(stm32boy_t *g, uint8_t sl, uint8_t sr)
{
    char buf[8];
    stm32boy_fill_rect(g, 0, SCORE_Y, SCREEN_W, 20, COLOR_BLACK);
    stm32boy_set_text_scale(g, 2);
    stm32boy_set_text_color(g, COLOR_WHITE, COLOR_BLACK, 0);
    snprintf(buf, sizeof(buf), "%d", sl);
    stm32boy_write_at(g, (uint16_t)(CENTER_LINE_X - 36), SCORE_Y, buf);
    snprintf(buf, sizeof(buf), "%d", sr);
    stm32boy_write_at(g, (uint16_t)(CENTER_LINE_X + 20), SCORE_Y, buf);
}

static void prv_draw_full(stm32boy_t *g, const pong_t *p)
{
    stm32boy_fill_screen(g, COLOR_BLACK);
    prv_draw_center_line(g);
    prv_draw_score(g, p->score_l, p->score_r);
    stm32boy_fill_rect(g, PADDLE_L_X, p->paddle_l_y, PADDLE_W, PADDLE_H, COLOR_GREEN);
    stm32boy_fill_rect(g, PADDLE_R_X, p->paddle_r_y, PADDLE_W, PADDLE_H, COLOR_RED);
    stm32boy_fill_rect(g, p->ball_x,  p->ball_y,     BALL_SIZE, BALL_SIZE, COLOR_WHITE);
}

static void prv_draw_incremental(stm32boy_t *g, const pong_t *p, const render_prev_t *prev)
{
    if (prev->paddle_l_y != p->paddle_l_y) {
        stm32boy_fill_rect(g, PADDLE_L_X, prev->paddle_l_y, PADDLE_W, PADDLE_H, COLOR_BLACK);
        stm32boy_fill_rect(g, PADDLE_L_X, p->paddle_l_y,    PADDLE_W, PADDLE_H, COLOR_GREEN);
    }

    if (prev->paddle_r_y != p->paddle_r_y) {
        stm32boy_fill_rect(g, PADDLE_R_X, prev->paddle_r_y, PADDLE_W, PADDLE_H, COLOR_BLACK);
        stm32boy_fill_rect(g, PADDLE_R_X, p->paddle_r_y,    PADDLE_W, PADDLE_H, COLOR_RED);
    }

    if (prev->ball_x != p->ball_x || prev->ball_y != p->ball_y) {
        stm32boy_fill_rect(g, prev->ball_x, prev->ball_y, BALL_SIZE, BALL_SIZE, COLOR_BLACK);
        stm32boy_fill_rect(g, p->ball_x,    p->ball_y,    BALL_SIZE, BALL_SIZE, COLOR_WHITE);
    }

    if (prev->score_l != p->score_l || prev->score_r != p->score_r) {
        prv_draw_score(g, p->score_l, p->score_r);
    }
}

static void prv_draw_overlay(stm32boy_t *g, pong_mode_t mode, uint8_t sl, uint8_t sr)
{
    if (mode == MODE_PAUSED) {
        stm32boy_set_text_scale(g, 2);
        stm32boy_set_text_color(g, COLOR_YELLOW, COLOR_BLACK, 0);
        stm32boy_write_at(g, 76, 210, "PAUSED");
        stm32boy_set_text_scale(g, 1);
        stm32boy_set_text_color(g, COLOR_WHITE, COLOR_BLACK, 0);
        stm32boy_write_at(g, 58, 250, "PRESS START");
    } else if (mode == MODE_TITLE) {
        stm32boy_set_text_scale(g, 2);
        stm32boy_set_text_color(g, COLOR_YELLOW, COLOR_BLACK, 0);
        stm32boy_write_at(g, 98, 180, "PONG");
        stm32boy_set_text_scale(g, 1);
        stm32boy_set_text_color(g, COLOR_WHITE, COLOR_BLACK, 0);
        stm32boy_write_at(g, 42, 230, "UP/DOWN: mover");
        stm32boy_write_at(g, 30, 250, "START: jugar/reiniciar");
        stm32boy_set_text_color(g, COLOR_GREEN, COLOR_BLACK, 0);
        stm32boy_write_at(g, 58, 290, "PRESS START");
    } else if (mode == MODE_GAME_OVER) {
        stm32boy_set_text_scale(g, 2);
        stm32boy_set_text_color(g, COLOR_RED, COLOR_BLACK, 0);
        stm32boy_write_at(g, 52, 180, "GAME OVER");
        stm32boy_set_text_scale(g, 1);
        stm32boy_set_text_color(g, COLOR_WHITE, COLOR_BLACK, 0);
        if (sl > sr) {
            stm32boy_write_at(g, 84, 220, "GANAS TU!");
        } else {
            stm32boy_write_at(g, 76, 220, "GANA LA CPU");
        }
        stm32boy_set_text_color(g, COLOR_GREEN, COLOR_BLACK, 0);
        stm32boy_write_at(g, 42, 260, "START: reiniciar");
    }
}

/* -----------------------------------------------------------------------
 * Punto de entrada
 * ----------------------------------------------------------------------- */
int main(void)
{
    bsp_init();
    systick_init(SystemCoreClock);

    stm32boy_t   g;
    pong_t       p    = {0};
    render_prev_t prev = {0};

    stm32boy_init(&g, SCREEN_W, SCREEN_H, &ili9486_display);
    stm32boy_fill_screen(&g, COLOR_BLACK);

    p.mode = MODE_TITLE;

    DBG("STMBOY PONG arranque");

    prv_draw_overlay(&g, MODE_TITLE, 0, 0);

    while (1)
    {
        platform_nucleof411re_buttons_update();
        const button_state_t *btn = platform_nucleof411re_buttons_get();

        uint32_t now_ms = systick_millis();
        bool changed = prv_update(&p, btn, now_ms);

        bool need_full = !prev.initialized || (prev.mode != p.mode);

        if (need_full || changed) {
            if (need_full) {
                prv_draw_full(&g, &p);
                prv_draw_overlay(&g, p.mode, p.score_l, p.score_r);
            } else {
                prv_draw_incremental(&g, &p, &prev);
            }

            prev.initialized = true;
            prev.mode        = p.mode;
            prev.ball_x      = p.ball_x;
            prev.ball_y      = p.ball_y;
            prev.paddle_l_y  = p.paddle_l_y;
            prev.paddle_r_y  = p.paddle_r_y;
            prev.score_l     = p.score_l;
            prev.score_r     = p.score_r;
        }
    }

    return 0;
}
