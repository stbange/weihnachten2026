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
static void test_active_encounter(Game *g)
{
    int stage = g->phase;
    const Encounter *beat = campaign_encounter(g);
    float x, z;
    campaign_target(g, &x, &z);
    Player *p = &g->player[1];
    p->x = x;
    p->z = z + 1.75f;
    p->y = 0;
    p->fuel = 1;
    char prompt[320];
    for (int frame = 0; frame < 280 && g->task == SET_PIECE && g->phase == stage; frame++) {
        float tx, y, tz;
        campaign_sight(g, &tx, &y, &tz);
        p->yaw = atan2f(tx - p->x, p->z - tz);
        p->pitch = atan2f(y - p->y - 1.55f, hypotf(tx - p->x, tz - p->z));
        campaign_prompt(g, 1, prompt, sizeof(prompt));
        assert(strstr(prompt, beat->name));
        Input in = {0};
        if (beat->action == HIGH)
            in.buttons = BOOST;
        else if (beat->action == LOW)
            in.forward = .25f;
        else if (beat->action == BEAM)
            in.buttons = FIRE;
        else if (frame % 2) {
            if (beat->action == PLACE ||
                (beat->action == POWER && g->dial != 1 + (g->door + g->area + g->task) % 3))
                in.buttons = USE;
            else
                in.buttons = FIRE;
        }
        once(g, 1, in);
    }
    assert(g->task != SET_PIECE || g->phase != stage);
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
            for (int objective = 0; objective < 9 && g.area == area && g.room == EXPEDITION;
                 objective++) {
                int task = g.task;
                if (task == SET_PIECE) {
                    scene_actors(&mesh, &g, 0, 1, 1);
                    game_pack(&g, wire);
                    assert(game_unpack(&copy, wire));
                    test_active_encounter(&g);
                    continue;
                }
                for (int part = 0; part < 2; part++) {
                    int kind = campaign_action(&g);
                    float x, z;
                    campaign_target(&g, &x, &z);
                    Cell target = {(int)x / CELL, (int)z / CELL};
                    Cell adjacent = campaign_next(&g, target, campaign_site(&g, 0));
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
                        if (task < EXIT_TASK && g.phase &&
                            ((day == 1 && kind == FLY) || (day == 3 && kind == ESCORT))) {
                            once(&g, 1, (Input){.buttons = USE});
                            assert(g.task == task && !g.dial);
                            once(&g, 1, (Input){.buttons = FIRE});
                            assert(g.dial);
                            once(&g, 1, (Input){0});
                        }
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
/* Flood the actual collision field, including alternate paths beyond the timed replay. */
static unsigned verify_plan(Game *g)
{
    int extent = campaign_extent(g);
    assert(extent <= MAP_SIZE);
    int walkable = 0, tail = 1;
    unsigned char seen[MAP_SIZE][MAP_SIZE] = {{0}};
    Cell queue[MAP_SIZE * MAP_SIZE];
    queue[0] = campaign_site(g, 0);
    seen[queue[0].z][queue[0].x] = 1;
    unsigned fingerprint = 2166136261u;
    for (int z = 0; z < extent; z++)
        for (int x = 0; x < extent; x++) {
            int surface = campaign_surface(g, x, z);
            walkable += surface != BARRIER;
            fingerprint = (fingerprint ^ (unsigned)surface) * 16777619u;
        }
    static const Cell dir[] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    for (int i = 0; i < tail; i++)
        for (int d = 0; d < 4; d++) {
            Cell c = {queue[i].x + dir[d].x, queue[i].z + dir[d].z};
            if (!campaign_wall(g, c.x, c.z) && !seen[c.z][c.x]) {
                seen[c.z][c.x] = 1;
                queue[tail++] = c;
            }
        }
    assert(tail == walkable);
    for (int i = 0; i < 10; i++) {
        Cell c = campaign_site(g, i);
        assert(seen[c.z][c.x]);
    }
    for (int i = 0; i < 2; i++) {
        Cell c = campaign_pad(g, i);
        assert(seen[c.z][c.x]);
    }
    return fingerprint;
}
static void test_floorplans(void)
{
    Game g;
    game_start(&g, 2, 24, 0, 0);
    unsigned fingerprints[24 * AREA_COUNT];
    int count = 0;
    for (int door = 1; door <= 24; door++)
        for (int area = 0; area < AREA_COUNT; area++) {
            campaign_enter(&g, door, 1, area);
            unsigned fingerprint = verify_plan(&g);
            for (int i = 0; i < count; i++)
                assert(fingerprints[i] != fingerprint);
            fingerprints[count++] = fingerprint;
            assert(campaign_brief(&g)[0]);
            g.task = EXIT_TASK;
            verify_plan(&g);
            g.task = SET_PIECE;
            g.phase = 3;
            assert(!campaign_valid_step(&g));
        }
}
static void test_route_facilities(void)
{
    Game g;
    game_start(&g, 2, 24, 0, 0);
    for (int door = 1; door <= 24; door++) {
        campaign_enter(&g, door, 1, 0);
        g.task = EXIT_TASK;
        Cell a = campaign_pad(&g, 0), b = campaign_pad(&g, 1);
        Player *p = &g.player[1];
        p->x = a.x * CELL;
        p->z = a.z * CELL;
        p->previous = 0;
        int f = districts[door - 1].feature;
        if (f == AIRLOCK) {
            once(&g, 1, (Input){.buttons = USE});
            assert(approximately(p->x, a.x * CELL) && approximately(p->z, a.z * CELL));
            once(&g, 1, (Input){0});
            p->y = 3;
        }
        if (f == AIRLOCK || f == TRAM || f == FERRY || f == RIFT || f == CHRONO) {
            /* Board from the end that does not share the extraction point. */
            Cell exit = campaign_site(&g, 7);
            if (exit.x == a.x && exit.z == a.z) {
                Cell swap = a;
                a = b;
                b = swap;
                p->x = a.x * CELL;
                p->z = a.z * CELL;
            }
            once(&g, 1, (Input){.buttons = USE});
            assert(approximately(p->x, b.x * CELL) && approximately(p->z, b.z * CELL));
        } else if (f == WIND || f == SKI) {
            p->yaw = 0;
            p->fuel = .5f;
            once(&g, 1, (Input){.forward = 1, .buttons = BOOST});
            assert(a.z * CELL - p->z > .095f);
            if (f == WIND)
                assert(p->fuel > .9f);
        } else if (f == FLARE) {
            once(&g, 1, (Input){.buttons = FIRE});
            assert(g.stun > 5);
        } else if (f == COOLANT) {
            once(&g, 1, (Input){.buttons = FIRE});
            assert(g.charge[1] > .04f);
        }
    }
    /* The final mail van and its driver arrive together. The second seat can stay behind. */
    campaign_enter(&g, 24, 1, 1);
    g.task = 2;
    g.phase = 1;
    assert(campaign_kind(&g) == ESCORT && campaign_feature_active(&g));
    Cell a = campaign_pad(&g, 0), b = campaign_pad(&g, 1);
    g.player[1].previous = 0;
    g.escort_x = g.player[1].x = a.x * CELL;
    g.escort_z = g.player[1].z = a.z * CELL;
    once(&g, 1, (Input){.buttons = USE});
    assert(approximately(g.escort_x, b.x * CELL) && approximately(g.escort_z, b.z * CELL));
}
static void test_chamber_gates(void)
{
    Game g;
    game_start(&g, 2, 24, 0, 0);
    for (int day = 1; day <= 24; day++)
        for (int area = 0; area < AREA_COUNT; area++) {
            campaign_enter(&g, day, 1, area);
            int gates = 0;
            for (int z = 0; z < campaign_extent(&g); z++)
                for (int x = 0; x < campaign_extent(&g); x++)
                    if (campaign_chamber_tile(&g, x, z) == '+') {
                        assert(campaign_wall(&g, x, z));
                        g.task = 1;
                        assert(!campaign_wall(&g, x, z));
                        Cell from = campaign_site(&g, 0),
                             next = campaign_next(&g, from, (Cell){x, z});
                        assert(next.x != from.x || next.z != from.z);
                        g.task = 0;
                        gates++;
                    }
            assert(gates == 1);
        }
}
int main(void)
{
    test_floorplans();
    test_route_facilities();
    test_chamber_gates();
    test_expeditions();
    test_calendar();
    test_hall();
    test_cinema_rules();
    test_wire();
    test_geometry();
    puts("Unit checks passed: calendar, hall, cinema, protocol and scene construction.");
    return 0;
}
