/* Drive the route with player inputs. The bot reads its compass, just as a player can. */
#include "campaign.h"
static int journey_frames;
static void drive_frame(Game *g, int seat, Input action)
{
    journey_frames++;
    Input in[2] = {{0}};
    in[seat] = action;
    game_step(g, in, 1.f / 30.f);
}
static void drive_towards(Game *g, int seat, float x, float z, float stop)
{
    Player *p = &g->player[seat];
    for (int n = 0; n < 900; n++) {
        if (hypotf(x - p->x, z - p->z) <= stop)
            return;
        game_look(g, seat, atan2f(x - p->x, p->z - z) - p->yaw, -p->pitch);
        drive_frame(g, seat, (Input){.forward = 1});
    }
    fprintf(stderr,
            "Stuck day=%d area=%d task=%d phase=%d position=%.1f,%.1f destination=%.1f,%.1f\n",
            g->door, g->area, g->task, g->phase, p->x, p->z, x, z);
    assert(0);
}
static void drive_leg(Game *g, int seat)
{
    float x, z;
    campaign_target(g, &x, &z);
    Player *p = &g->player[seat];
    Cell target = {(int)roundf(x / CELL), (int)roundf(z / CELL)};
    for (int n = 0; n < MAP_SIZE * MAP_SIZE; n++) {
        Cell here = {(int)roundf(p->x / CELL), (int)roundf(p->z / CELL)};
        Cell next = campaign_next(g, here, target);
        if (next.x == target.x && next.z == target.z) {
            drive_towards(g, seat, x, z, 2.15f);
            return;
        }
        drive_towards(g, seat, next.x * CELL, next.z * CELL, .11f);
    }
    assert(0 && "Compass loop");
}
static void drive_action(Game *g, int seat)
{
    int kind = campaign_action(g);
    float x, z;
    campaign_target(g, &x, &z);
    Player *p = &g->player[seat];
    game_look(g, seat, atan2f(x - p->x, p->z - z) - p->yaw, -p->pitch);
    drive_frame(g, seat, (Input){0});
    int buttons = USE;
    if (g->task != 3) {
        if (kind == SHOT)
            buttons = FIRE;
        if (kind == CHARGE) {
            for (int i = 0; i < 25; i++)
                drive_frame(g, seat, (Input){.buttons = FIRE});
            buttons = 0;
        }
        if (kind == FLY) {
            for (int i = 0; i < 22; i++)
                drive_frame(g, seat, (Input){.buttons = BOOST});
            buttons |= BOOST;
        }
        if (kind == GRID) {
            int code = 1 + (g->door + g->area + g->task) % 3;
            while (g->dial != code) {
                drive_frame(g, seat, (Input){.buttons = USE});
                drive_frame(g, seat, (Input){0});
            }
            buttons = FIRE;
        }
        if (kind == ESCORT && g->phase)
            for (int i = 0; i < 40; i++)
                drive_frame(g, seat, (Input){0});
    }
    drive_frame(g, seat, (Input){.buttons = buttons});
}
static void drive_campaign(int day, int seats, int practice)
{
    Game g;
    game_start(&g, seats, 24, 0, 0);
    if (practice)
        campaign_enter(&g, day, 1, 0);
    else {
        float x, z;
        portal_position(day, &x, &z);
        /* Walk the centre aisle, then enter this live arch. */
        drive_towards(&g, 0, copysignf(5.1f, x), 20.5f, .12f);
        drive_towards(&g, 0, copysignf(5.1f, x), z, .12f);
        drive_towards(&g, 0, copysignf(6.6f, x), z, .13f);
        drive_frame(&g, 0, (Input){.buttons = USE});
    }
    assert(g.room == EXPEDITION && g.door == day);
    journey_frames = 0;
    int count = 0;
    while (g.room == EXPEDITION && count++ < 80) {
        int seat = g.area % seats;
        drive_leg(&g, seat);
        drive_action(&g, seat);
    }
    if (g.room != HALL)
        fprintf(stderr, "Unfinished day %d area %d task %d phase %d\n", day, g.area, g.task,
                g.phase);
    assert(g.room == HALL && g.event == (practice ? REHEARSAL_TICKET : TICKET));
    if (seats == 1)
        printf("Door %02d direct-route simulation: %.1f minutes\n", day, journey_frames / 1800.0);
    assert(g.completed == (practice ? 0 : 1 << (day - 1)));
}
