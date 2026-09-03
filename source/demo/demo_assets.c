#include "demo_assets.h"

#define K COLOR_BLACK
#define C 0x07FFU
#define D 0x02AAU
#define W COLOR_WHITE

static const uint16_t s_actor_frame_0[DEMO_ACTOR_WIDTH * DEMO_ACTOR_HEIGHT] = {
    K, K, K, D, D, D, D, D, D, K, K, K,
    K, K, D, C, C, C, C, C, C, D, K, K,
    K, D, C, W, W, C, C, C, C, C, D, K,
    D, C, W, C, C, C, C, C, C, C, C, D,
    D, C, C, C, K, C, C, K, C, C, C, D,
    D, C, C, C, K, C, C, K, C, C, C, D,
    D, C, C, C, C, C, C, C, C, C, C, D,
    D, C, C, K, C, C, C, C, K, C, C, D,
    D, C, C, C, K, K, K, K, C, C, C, D,
    K, D, C, C, C, C, C, C, C, C, D, K,
    K, K, D, C, C, C, C, C, C, D, K, K,
    K, K, K, D, D, D, D, D, D, K, K, K,
};

static const uint16_t s_actor_frame_1[DEMO_ACTOR_WIDTH * DEMO_ACTOR_HEIGHT] = {
    K, K, K, D, D, D, D, D, D, K, K, K,
    K, K, D, C, C, C, C, C, C, D, K, K,
    K, D, C, C, C, C, C, W, W, C, D, K,
    D, C, C, C, C, C, C, C, W, C, C, D,
    D, C, C, K, C, C, K, C, C, C, C, D,
    D, C, C, K, C, C, K, C, C, C, C, D,
    D, C, C, C, C, C, C, C, C, C, C, D,
    D, C, C, C, K, C, C, K, C, C, C, D,
    D, C, C, C, C, K, K, C, C, C, C, D,
    K, D, C, C, C, C, C, C, C, C, D, K,
    K, K, D, C, C, C, C, C, C, D, K, K,
    K, K, K, D, D, D, D, D, D, K, K, K,
};

const sprite_t g_demo_actor_frames[DEMO_ACTOR_FRAME_COUNT] = {
    {
        .w = DEMO_ACTOR_WIDTH,
        .h = DEMO_ACTOR_HEIGHT,
        .pixels = s_actor_frame_0,
    },
    {
        .w = DEMO_ACTOR_WIDTH,
        .h = DEMO_ACTOR_HEIGHT,
        .pixels = s_actor_frame_1,
    },
};

#undef K
#undef C
#undef D
#undef W
