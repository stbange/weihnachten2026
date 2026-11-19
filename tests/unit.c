#include "campaign.h"
#include "scene.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
static const float step = 1.f / 60.f;
static void once(Game *g, int who, Input action)
{
    Input in[2] = {{0}};
    in[who] = action;
    game_step(g, in, step);
}
static int approximately(float x, float y) { return fabsf(x - y) < .001f; }

static void test_calendar(void)
{
    const int64_t first = 1796079600;
    assert(calendar_open(INT64_MIN) == 0 && calendar_open(INT64_MAX) == 24);
    for (int i = 0; i < 24; i++) {
        int64_t edge = first + i * 86400;
        assert(calendar_open(edge - 1) == i && calendar_open(edge) == i + 1);
    }
    assert(calendar_open(first + 24 * 86400) == 24 && calendar_open(first - 1) == 0);
}

static void test_hall(void)
{
    Game g;
    game_start(&g, 2, -2, 0, 1);
    assert(g.open == 0 && g.completed && g.player[0].x != g.player[1].x);
    assert(!game_near(&g, 0));
    for (int i = 1; i <= 24; i++) {
        portal_position(i, &g.player[1].x, &g.player[1].z);
        assert(game_near(&g, 1) == i);
        once(&g, 1, (Input){.buttons = USE});
        assert(g.event == LOCKED && g.room == HALL);
        once(&g, 1, (Input){0});
    }
    g.open = 24;
    once(&g, 1, (Input){.buttons = USE});
    assert(g.event == ENTERED && g.door == 24);
    game_hall(&g);
    portal_position(1, &g.player[0].x, &g.player[0].z);
    once(&g, 0, (Input){0});
    assert(g.room == EXPEDITION && g.event == ENTERED);
    assert(game_near(&g, 0) == 0);
    game_hall(&g);
    g.player[0].x = 7;
    g.player[0].z = 22.24f;
    assert(!game_near(&g, 0));
    game_start(&g, 1, 30, 0, 0);
    assert(g.open == 24);
    once(&g, 0, (Input){.buttons = FIRE});
    assert(g.flash[0] > 0);
    game_look(&g, 0, 30, 10);
    assert(g.player[0].pitch <= 1.12f && fabsf(g.player[0].yaw) < 3.15f);
    game_look(&g, 0, 0, -20);
    assert(g.player[0].pitch >= -1.12f);
    game_hall(&g);
    g.player[0].x = 0;
    g.player[0].z = -18.84f;
    for (int i = 0; i < 50; i++)
        once(&g, 0, (Input){.forward = 1});
    assert(g.player[0].z > -18.90f);
    g.player[0].x = 2.12f;
    g.player[0].z = -8;
    once(&g, 0, (Input){.side = 1});
    assert(g.player[0].x < 2.15f);
    g.player[0].x = 7.46f;
    g.player[0].z = 22.2f;
    for (int i = 0; i < 100; i++)
        once(&g, 0, (Input){.side = 1, .forward = -1});
    assert(g.player[0].x <= 7.48f && g.player[0].z <= 22.3f);
}

static void test_cinema_rules(void)
{
    Game g;
    game_start(&g, 1, 0, 1, 0);
    assert(g.rehearsal && g.room == CINEMA);
    Player *p = &g.player[0];
    p->x = -5.1f;
    p->y = 2.8f;
    p->z = -8.15f;
    once(&g, 0, (Input){.buttons = USE});
    assert(!g.projected);
    p->x = 0;
    p->z = 9;
    p->y = 0;
    once(&g, 0, (Input){.buttons = FIRE});
    assert(g.folded == 0);
    for (int i = 0; i < 2; i++) {
        float x, z;
        usher_position(i, &x, &z);
        p->x = x;
        p->z = z + .97f;
        once(&g, 0, (Input){.forward = 1});
        assert(p->z > z + .94f);
        p->z = z + 3;
        p->yaw = 0;
        once(&g, 0, (Input){.buttons = FIRE});
        assert(g.folded & (1 << i));
        once(&g, 0, (Input){0});
    }
    assert(g.folded == 3);
    p->x = -5.1f;
    p->z = -8.15f;
    p->y = 0;
    for (int i = 0; i < 64; i++)
        once(&g, 0, (Input){.buttons = BOOST});
    assert(p->y > 3.5f && p->fuel < .6f);
    for (int i = 0; i < 100; i++)
        once(&g, 0, (Input){0});
    assert(approximately(p->y, 2.75f) && approximately(p->fuel, 1));
    once(&g, 0, (Input){.buttons = USE});
    assert(g.projected && g.event == PROJECTED);
    p->x = 0;
    p->z = 10;
    once(&g, 0, (Input){0});
    once(&g, 0, (Input){.buttons = USE});
    assert(g.room == HALL && !g.completed && g.event == REHEARSAL_TICKET);
    game_enter(&g, 0);
    g.folded = 3;
    g.projected = 1;
    g.player[0].z = 10;
    once(&g, 0, (Input){.buttons = USE});
    assert(g.room == HALL && g.completed && g.event == TICKET);
}

static void test_wire(void)
{
    Game a, b;
    float wire[SNAP_WORDS], copy[SNAP_WORDS];
    game_start(&a, 2, 11, 1, 1);
    once(&a, 1, (Input){.forward = 1, .turn = 1, .buttons = BOOST});
    game_pack(&a, wire);
    game_start(&b, 1, 0, 0, 0);
    assert(game_unpack(&b, wire));
    game_pack(&b, copy);
    assert(memcmp(wire, copy, sizeof(wire)) == 0);
    const struct {
        int index;
        float val;
    } invalid[] = {{0, 0},   {1, -1},  {2, 25},  {2, 3.5f}, {9, 0},  {9, 3},   {10, 81},
                   {11, -1}, {12, 40}, {13, 10}, {14, 2},   {15, 2}, {21, NAN}};
    for (unsigned i = 0; i < sizeof(invalid) / sizeof(invalid[0]); i++) {
        float bad[SNAP_WORDS];
        memcpy(bad, wire, sizeof(bad));
        bad[invalid[i].index] = invalid[i].val;
        assert(!game_unpack(&b, bad));
        game_pack(&b, copy);
        assert(memcmp(wire, copy, sizeof(wire)) == 0);
    }
}

static void test_geometry(void)
{
    static Vertex v[150000];
    Game g;
    game_start(&g, 2, 12, 0, 0xffffff);
    Mesh m = {v, 0, 150000};
    scene_static(&m, &g);
    assert(m.count > 25000 && m.count < m.capacity);
    for (int i = 0; i < m.count; i++) {
        assert(isfinite(v[i].pos.x) && isfinite(v[i].normal.z));
    }
    scene_actors(&m, &g, 1, .5f, 1);
    assert(m.count > 100);
    game_enter(&g, 1);
    scene_static(&m, &g);
    assert(m.count > 2000);
    g.projected = 1;
    scene_static(&m, &g);
    g.folded = 1;
    scene_actors(&m, &g, 0, .2f, 1);
    assert(m.count > 300);
    m.capacity = 2;
    scene_actors(&m, &g, 0, 0, 1);
    assert(m.count == 0);
    float a[16], b[16];
    camera_matrix(a, (Vec){0, 1, 3}, 0, 0, 1);
    camera_matrix(b, (Vec){0, 1, 3}, .3f, .1f, 1.5f);
    assert(!approximately(a[0], b[0]) && a[11] < 0 && a[15] > 0);
}
static void test_expeditions(void)
{
    static Vertex vertices[150000];
    Mesh mesh = {vertices, 0, 150000};
    Game g, copy;
    char prompt[320];
    float wire[SNAP_WORDS];
    game_start(&g, 2, 24, 0, 2);
    for (int day = 1; day <= 24; day++) {
        campaign_enter(&g, day, day % 2, 0);
        scene_static(&mesh, &g);
        assert(mesh.count > 1000 && mesh.count < mesh.capacity);
        for (int area = 0; area < AREA_COUNT; area++) {
            scene_static(&mesh, &g);
            assert(mesh.count < mesh.capacity);
            for (int task = 0; task < 4; task++) {
                assert(g.task == task);
                for (int part = 0; part < 2; part++) {
                    int kind = campaign_action(&g);
                    float x, z;
                    campaign_target(&g, &x, &z);
                    Cell target = {(int)x / CELL, (int)z / CELL};
                    Cell adjacent = campaign_next(&g, target, (Cell){1, 23});
                    assert(!campaign_wall(&g, adjacent.x, adjacent.z));
                    Player *p = &g.player[1];
                    p->x = x + (adjacent.x - target.x) * 1.95f;
                    p->z = z + (adjacent.z - target.z) * 1.95f;
                    p->yaw = atan2f(x - p->x, p->z - z);
                    p->pitch = 0;
                    p->y = 0;
                    p->fuel = 1;
                    campaign_prompt(&g, 1, prompt, sizeof(prompt));
                    assert(strstr(prompt, expeditions[day - 1].areas[area]));
                    scene_actors(&mesh, &g, 0, 1, 1);
                    assert(mesh.count > 0);
                    game_pack(&g, wire);
                    assert(game_unpack(&copy, wire));
                    assert(copy.area == area && copy.task == task);
                    once(&g, 1, (Input){0});
                    int b = USE;
                    if (task != 3) {
                        if (kind == GRID) {
                            while (g.dial != 1 + (day + task + area) % 3) {
                                once(&g, 1, (Input){.buttons = USE});
                                once(&g, 1, (Input){0});
                            }
                            b = FIRE;
                        }
                        if (kind == CHARGE) {
                            for (int i = 0; i < 43; i++)
                                once(&g, 1, (Input){.buttons = FIRE});
                            b = 0;
                        }
                        if (kind == FLY) {
                            for (int i = 0; i < 42; i++)
                                once(&g, 1, (Input){.buttons = BOOST});
                            b |= BOOST;
                        }
                        if (kind == SHOT)
                            b = FIRE;
                        if (kind == ESCORT && g.phase) {
                            g.escort_x = x;
                            g.escort_z = z;
                        }
                    }
                    once(&g, 1, (Input){.buttons = b});
                    if (g.room != EXPEDITION || g.task != task || g.area != area)
                        break;
                    assert(g.phase == 1);
                }
            }
        }
        assert(g.room == HALL);
        if (day % 2 == 0)
            assert(g.completed & (1 << (day - 1)));
    }
    campaign_enter(&g, 3, 1, 0);
    assert(campaign_wall(&g, -1, 0));
    Cell c = campaign_next(&g, (Cell){0, 0}, (Cell){2, 2});
    assert(c.x == 0 && c.z == 0);
    game_pack(&g, wire);
    wire[22] = 5;
    assert(!game_unpack(&copy, wire));
    wire[22] = 0;
    wire[8] = 0;
    assert(!game_unpack(&copy, wire));
}
int main(void)
{
    test_expeditions();
    test_calendar();
    test_hall();
    test_cinema_rules();
    test_wire();
    test_geometry();
    puts("Unit checks passed: calendar, hall, cinema, protocol and scene construction.");
    return 0;
}
