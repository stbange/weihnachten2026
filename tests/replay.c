/* A complete game using ordinary inputs. No teleport, objective setter or debug gate. */
#include "hall.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <math.h>
#include <stdio.h>
static void advance(Game *g, int seat, Input cmd, int frames)
{
    Input in[2] = {{0}};
    in[seat] = cmd;
    for (int t = 0; t < frames; ++t)
        game_step(g, in, 1.f / 60.f);
}
static void face(Game *g, int seat, float x, float z, float pitch)
{
    Player *p = &g->player[seat];
    game_look(g, seat, atan2f(x - p->x, p->z - z) - p->yaw, pitch - p->pitch);
}
static void walk(Game *g, int seat, float x, float z, int flags)
{
    for (int i = 0; i < 900; i++) {
        Player *p = &g->player[seat];
        if (hypotf(x - p->x, z - p->z) < .12f)
            return;
        face(g, seat, x, z, 0);
        advance(g, seat, (Input){.forward = 1, .buttons = flags}, 1);
    }
    assert(0 && "Unreachable waypoint");
}
static void act(Game *g, int seat, int button)
{
    advance(g, seat, (Input){0}, 1);
    advance(g, seat, (Input){.buttons = button}, 1);
}
static void run(int seats, int rehearsal)
{
    Game g;
    game_start(&g, seats, rehearsal ? 0 : 1, rehearsal, 0);
    if (!rehearsal) {
        game_enter(&g,0);
        assert(g.room == CINEMA && g.event == ENTERED);
    }
    for (int i = 0; i < 2; i++) {
        int seat = i % seats;
        float x, z;
        usher_position(i, &x, &z);
        walk(&g, seat, 0, z + 3, 0);
        face(&g, seat, x, z, 0);
        act(&g, seat, FIRE);
        assert(g.folded & (1 << i));
    }
    int pilot = seats - 1;
    walk(&g, pilot, -4.8f, -7.8f, 0);
    advance(&g, pilot, (Input){.buttons = BOOST}, 54);
    act(&g, pilot, USE);
    assert(g.projected);
    walk(&g, 0, 0, 9.75f, 0);
    act(&g, 0, USE);
    assert(g.room == HALL && g.completed == !rehearsal);
    assert(g.event == (rehearsal ? REHEARSAL_TICKET : TICKET));
}
#include "campaign_replay.h"
int main(void)
{
    run(1, 1);
    run(1, 0);
    run(2, 1);
    run(2, 0);
    puts("Four complete input replays passed: solo/split, rehearsal/live.");
    for(int d=1;d<=24;d++){drive_campaign(d,1,0);drive_campaign(d,2,1);}
    puts("All 24 expeditions cleared through player inputs, solo calendar and split practice.");
    return 0;
}
