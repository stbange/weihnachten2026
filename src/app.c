#include "app.h"
#include "draw.h"
#include <SDL3/SDL.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define API EMSCRIPTEN_KEEPALIVE
#else
#define API
#endif

static SDL_Window *window;
static SDL_GLContext context;
static Game game;
static Input controls[2];
static float wire[SNAP_WORDS], xr_transform[16], elapsed, accum;
static int width = 720, height = 405;

API int lh_init(void)
{
    if (!SDL_Init(SDL_INIT_VIDEO))
        return 0;
#ifdef __EMSCRIPTEN__
    SDL_SetHint(SDL_HINT_EMSCRIPTEN_KEYBOARD_ELEMENT, "#canvas");
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#endif
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    window =
        SDL_CreateWindow("Lantern Hall", width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window || !(context = SDL_GL_CreateContext(window)) || !draw_init()) {
        SDL_Log("Cannot open Lantern Hall: %s", SDL_GetError());
        return 0;
    }
    game_start(&game, 1, calendar_open((int64_t)time(NULL)), 0, 0);
    return 1;
}

API void lh_close(void)
{
    draw_close();
    SDL_GL_DestroyContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    context = NULL;
    window = NULL;
}

API void lh_resize(int w, int h)
{
    if (w == width && h == height)
        return;
    width = w;
    height = h;
    SDL_SetWindowSize(window, w, h);
}
API void lh_start(int seats, int open, int rehearsal, int done)
{
    memset(controls, 0, sizeof(controls));
    accum = 0;
    game_start(&game, seats, open, rehearsal, done);
}
API void lh_open(int open)
{
    game.open = (int)fmaxf(0, fminf(24, open));
}
API int lh_today(double epoch)
{
    return calendar_open((int64_t)epoch);
}
API void lh_input(int who, float forward, float side, float turn, float tilt, int buttons)
{
    if ((unsigned)who < 2)
        controls[who] = (Input){
            .forward = forward, .side = side, .turn = turn, .tilt = tilt, .buttons = buttons & 7};
}
API void lh_point(int who, float yaw, float pitch)
{
    if ((unsigned)who < 2) {
        controls[who].spatial_aim = 1;
        controls[who].aim_yaw = yaw;
        controls[who].aim_pitch = pitch;
    }
}
API void lh_aim(int who, float yaw, float pitch)
{
    if ((unsigned)who < 2)
        game_look(&game, who, yaw, pitch);
}
API int lh_tick(float dt, int simulate)
{
#ifdef __EMSCRIPTEN__
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
    }
#endif
    dt = fminf(fmaxf(dt, 0), .10f);
    elapsed += dt;
    if (!simulate)
        return 0;
    int steps = 0;
    accum += dt;
    while (accum >= 1.f / 60.f) {
        game_step(&game, controls, 1.f / 60.f);
        accum -= 1.f / 60.f;
        steps++;
    }
    return steps;
}
API int lh_near(int who)
{
    return game_near(&game, who);
}
API int lh_event(void)
{
    int event = game.event;
    game.event = NO_EVENT;
    return event;
}
API float *lh_state(void)
{
    game_pack(&game, wire);
    return wire;
}
API float *lh_wire(void)
{
    return wire;
}
API int lh_receive(void)
{
    return game_unpack(&game, wire);
}
API float *lh_matrix(void)
{
    return xr_transform;
}
API void lh_xr_eye(int viewer, int x, int y, int w, int h)
{
    draw_view(&game, viewer, elapsed, xr_transform, x, y, w, h, 0);
}

API void lh_draw(int title, int split, int viewer)
{
    int views = split ? 2 : 1;
    for (int i = 0; i < views; i++) {
        int w = width, h = height, x = 0, y = 0, seat = split ? i : viewer;
        if (split && width >= height) {
            w = width / 2;
            x = i * w;
            if (i)
                w = width - x;
        }
        if (split && width < height) {
            h = height / 2;
            y = i ? 0 : h;
            if (!i)
                h = height - y;
        }
        const Player *p = &game.player[seat];
        Vec eye = {p->x, p->y + 1.55f, p->z};
        float yaw = p->yaw, pitch = p->pitch;
        if (title) {
            eye = (Vec){4.85f, 2.45f, 3.45f};
            yaw = -.30f + .025f * sinf(elapsed * .12f);
            pitch = .095f;
        }
        float matrix[16];
        camera_matrix(matrix, eye, yaw, pitch, (float)w / h);
        draw_view(&game, seat, elapsed, matrix, x, y, w, h, 1);
    }
    SDL_GL_SwapWindow(window);
}
#ifndef __EMSCRIPTEN__
int lh_saved(void)
{
    char *path = SDL_GetPrefPath("Lantern Hall", "Advent 2026");
    if (!path)
        return 0;
    char file[1200];
    SDL_snprintf(file, sizeof(file), "%sticket-v1", path);
    SDL_free(path);
    size_t bytes = 0;
    char *value = SDL_LoadFile(file, &bytes);
    int result = value && bytes == 1 && value[0] == '1';
    SDL_free(value);
    return result;
}
void lh_save_ticket(void)
{
    char *path = SDL_GetPrefPath("Lantern Hall", "Advent 2026");
    if (!path)
        return;
    char file[1200];
    SDL_snprintf(file, sizeof(file), "%sticket-v1", path);
    SDL_free(path);
    SDL_SaveFile(file, "1", 1);
}

int main(int argc, char **argv)
{
    if (!lh_init())
        return 1;
    int split = 0, rehearsal = 0, smoke = 0;
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--split"))
            split = 1;
        if (!strcmp(argv[i], "--rehearsal"))
            rehearsal = 1;
        if (!strcmp(argv[i], "--smoke"))
            smoke = 1;
    }
    lh_start(1 + split, calendar_open((int64_t)time(NULL)), rehearsal, lh_saved());
    SDL_SetWindowRelativeMouseMode(window, true);
    SDL_GL_SetSwapInterval(1);
    Uint64 last = SDL_GetTicksNS();
    int run = 1, frames = 0;
    while (run) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT)
                run = 0;
            if (e.type == SDL_EVENT_MOUSE_MOTION)
                lh_aim(0, e.motion.xrel * .0035f, -e.motion.yrel * .0035f);
            if (e.type == SDL_EVENT_WINDOW_RESIZED) {
                width = e.window.data1;
                height = e.window.data2;
            }
            if (e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_ESCAPE)
                run = 0;
            if (e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_R && !e.key.repeat)
                lh_start(1 + split, calendar_open((int64_t)time(NULL)), 1, game.completed);
        }
        const bool *k = SDL_GetKeyboardState(NULL);
        int a = k[SDL_SCANCODE_E] * USE +
                (k[SDL_SCANCODE_F] || ((SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_LMASK) != 0)) *
                    FIRE +
                k[SDL_SCANCODE_SPACE] * BOOST;
        lh_input(0, k[SDL_SCANCODE_W] - k[SDL_SCANCODE_S], k[SDL_SCANCODE_D] - k[SDL_SCANCODE_A],
                 k[SDL_SCANCODE_RIGHT] - k[SDL_SCANCODE_LEFT],
                 k[SDL_SCANCODE_UP] - k[SDL_SCANCODE_DOWN], a);
        lh_input(1, k[SDL_SCANCODE_I] - k[SDL_SCANCODE_K], k[SDL_SCANCODE_L] - k[SDL_SCANCODE_J],
                 k[SDL_SCANCODE_O] - k[SDL_SCANCODE_U], 0,
                 k[SDL_SCANCODE_H] * USE + k[SDL_SCANCODE_Y] * FIRE + k[SDL_SCANCODE_N] * BOOST);
        Uint64 now = SDL_GetTicksNS();
        lh_tick((float)((now - last) / 1e9), 1);
        last = now;
        lh_open(calendar_open((int64_t)time(NULL)));
        lh_draw(0, split, 0);
        if (game.event == TICKET && game.completed)
            lh_save_ticket();
        const char *goal = game.room == HALL  ? "WASD move | E inspect | R rehearse Door 01"
                           : game.folded != 3 ? "Aim and F to shrink both paper ushers"
                           : !game.projected  ? "Space to boost to the left balcony, E at projector"
                                              : "Return to entrance and press E";
        char title[220];
        snprintf(title, sizeof(title), "Lantern Hall | %s | %d/24 doors open", goal, game.open);
        if (game.room == HALL && game_near(&game, 0))
            snprintf(title, sizeof(title), "Lantern Hall | Door %02d | %s", game_near(&game, 0),
                     game_near(&game, 0) > game.open ? "Sealed until its December date"
                     : game_near(&game, 0) == 1      ? "Walk through"
                                                     : "Mission in the portal almanac");
        SDL_SetWindowTitle(window, title);
        lh_event();
        if (smoke && ++frames == 3)
            run = 0;
    }
    lh_close();
    return 0;
}
#endif
