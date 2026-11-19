#include "campaign.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
const Expedition expeditions[24] = {
#include "expeditions.inc"
};
static unsigned char walls[MAP_SIZE][MAP_SIZE];
static int cached = -1;
static const Cell dirs[] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};
static unsigned random_step(unsigned *seed)
{
    *seed = *seed * 1664525u + 1013904223u;
    return *seed >> 8;
}
static void layout(const Game *g)
{
    int key = g->door * 7 + g->area;
    if (cached == key)
        return;
    cached = key;
    unsigned seed = 7919u * key + 117u;
    memset(walls, 1, sizeof(walls));
    Cell stack[MAP_SIZE * MAP_SIZE];
    int count = 1;
    stack[0] = (Cell){1, MAP_SIZE - 2};
    walls[MAP_SIZE - 2][1] = 0;
    while (count) {
        Cell p = stack[count - 1], choices[4];
        int n = 0;
        for (int j = 0; j < 4; j++) {
            Cell q = {p.x + 2 * dirs[j].x, p.z + 2 * dirs[j].z};
            if (q.x > 0 && q.z > 0 && q.x < MAP_SIZE - 1 && q.z < MAP_SIZE - 1 && walls[q.z][q.x])
                choices[n++] = q;
        }
        if (!n) {
            count--;
            continue;
        }
        Cell q = choices[random_step(&seed) % n];
        walls[q.z][q.x] = walls[(q.z + p.z) / 2][(q.x + p.x) / 2] = 0;
        stack[count++] = q;
    }
    /* Courtyards break up narrow passages and give each landmark clear sightlines. */
    for (int i = 0; i < 10; i++) {
        int x = 2 + random_step(&seed) % (MAP_SIZE - 5),
            z = 2 + random_step(&seed) % (MAP_SIZE - 5);
        for (int dz = -1; dz <= 1; dz++)
            for (int dx = -1; dx <= 1; dx++)
                walls[z + dz][x + dx] = 0;
    }
    /* Reserve a walkable arrival court around the two solid landmark tiles. */
    for (int z = 19; z <= 23; z++)
        for (int x = 1; x <= 5; x++)
            walls[z][x] = 0;
    walls[21][3] = walls[21][4] = 1;
}
int campaign_wall(const Game *g, int x, int z)
{
    layout(g);
    return x < 0 || z < 0 || x >= MAP_SIZE || z >= MAP_SIZE || walls[z][x];
}
Cell campaign_site(const Game *g, int i)
{
    static const Cell sites[] = {{1, 23},  {17, 21}, {5, 7},  {23, 3},
                                 {15, 13}, {3, 17},  {11, 3}, {23, 15}};
    if (i == 0)
        return sites[0];
    return sites[1 + ((i - 1 + g->area * 2 + g->door - 1) % 7)];
}
static Cell tile(float x, float z)
{
    return (Cell){(int)floorf(x / CELL + .5f), (int)floorf(z / CELL + .5f)};
}
static int same(Cell a, Cell b) { return a.x == b.x && a.z == b.z; }
Cell campaign_next(const Game *g, Cell from, Cell to)
{
    if (same(from, to) || campaign_wall(g, from.x, from.z) || campaign_wall(g, to.x, to.z))
        return from;
    /* HUD, guide arrows, both eyes and escort AI can ask for the same route in a frame. */
    static struct PathField {
        int key;
        Cell goal;
        Cell next[MAP_SIZE][MAP_SIZE];
    } fields[4];
    struct PathField *field = &fields[(to.x + to.z) % 4];
    if (field->key != cached || !same(field->goal, to)) {
        field->key = cached;
        field->goal = to;
        Cell queue[MAP_SIZE * MAP_SIZE];
        unsigned char seen[MAP_SIZE][MAP_SIZE] = {{0}};
        int tail = 1;
        queue[0] = to;
        seen[to.z][to.x] = 1;
        for (int k = 0; k < tail; k++)
            for (int d = 0; d < 4; d++) {
                Cell p = queue[k], q = {p.x + dirs[d].x, p.z + dirs[d].z};
                if (!campaign_wall(g, q.x, q.z) && !seen[q.z][q.x]) {
                    seen[q.z][q.x] = 1;
                    field->next[q.z][q.x] = p;
                    queue[tail++] = q;
                }
            }
    }
    return field->next[from.z][from.x];
}
typedef struct {
    unsigned sectors;
    int first, second;
    const char *message, *origin, *destination;
} Transmission;
static const Transmission transmissions[24] = {
#include "transmissions.inc"
};
int campaign_kind(const Game *g)
{
    if (g->task == TRANSMISSION) {
        const Transmission *t = &transmissions[g->door - 1];
        return g->phase ? t->second : t->first;
    }
    return expeditions[g->door - 1].tools[(g->task + g->area) % 3].kind;
}
typedef struct {
    int first, second;
    const char *instruction;
} Combination;
static const Combination combinations[24] = {
    [3] = {FLY, PUNCH, "Use the latch to drain the raised basin"},
    [5] = {CHARGE, PUNCH, "Use the cooling manifold to vent the walker"},
    [7] = {PUNCH, GRID, "Tune the exposed pump, then pulse to restart"},
    [8] = {SHOT, FLY, "Lift to the crest and use the flag winch"},
    [9] = {CHARGE, PUNCH, "Use the street terminal behind the folded facade"},
    [10] = {GRID, FLY, "Fly up to the core vent and use the release"},
    [11] = {GRID, PUNCH, "Use the matching vault seal with the identified relic"},
    [12] = {CHARGE, SHOT, "Pulse the choir pipe while its furnace is primed"},
    [13] = {CHARGE, FLY, "Lift to the severed ribbon and use its peg"},
    [14] = {CHARGE, PUNCH, "Use the recovery capsule while the mimic is stunned"},
    [15] = {SHOT, FLY, "Lift from the impulse pad and use the upper ratchet"},
    [16] = {CHARGE, PUNCH, "Use the counterweight latch after deflecting the beam"},
    [17] = {GRID, PUNCH, "Use the fabricator within the powered shelter"},
    [18] = {GRID, CHARGE, "Hold and release a light orb at the translated seal"},
    [19] = {SHOT, GRID, "Tune the exposed circuit and pulse to reconnect it"},
    [21] = {SHOT, CHARGE, "Hold and release a charged flare at the mirror figure"},
    [22] = {CHARGE, PUNCH, "Use the constructor to bank the reclaimed hull metal"},
    [23] = {GRID, PUNCH, "Use the powered chrono pad to transmit the invitations"}};
static const Combination *combination(const Game *g)
{
    if (g->door == 17 && g->task < 3 && campaign_kind(g) == FLY) {
        static const Combination mirror = {FLY, PUNCH,
                                           "Use the landing mirror to hold the light bridge open"};
        return &mirror;
    }
    if (g->door == 14 && g->task < 3) {
        static const Combination peg = {FLY, CHARGE,
                                        "Charge the pruning blade, release to sever the tether"};
        static const Combination note = {GRID, PUNCH, "Use the tuned seed on the hollow heartwood"};
        if (campaign_kind(g) == FLY)
            return &peg;
        if (campaign_kind(g) == GRID)
            return &note;
    }
    const Combination *c = &combinations[g->door - 1];
    return g->task < 3 && c->instruction && c->first == campaign_kind(g) ? c : NULL;
}
int campaign_action(const Game *g)
{
    const Combination *c = combination(g);
    return c && g->phase ? c->second : campaign_kind(g);
}
static void reset_tool(Game *g)
{
    Cell c = campaign_site(g, 1 + 2 * g->task);
    g->phase = 0;
    g->dial = 0;
    g->charge[0] = g->charge[1] = 0;
    g->escort_x = c.x * CELL;
    g->escort_z = c.z * CELL;
}
void campaign_enter(Game *g, int day, int rehearsal, int area)
{
    g->room = EXPEDITION;
    g->door = day;
    g->rehearsal = !!rehearsal;
    g->area = area;
    g->task = 0;
    g->event = ENTERED;
    g->stun = 0;
    g->clock = 0;
    reset_tool(g);
    Cell c = campaign_site(g, 0);
    for (int i = 0; i < 2; i++)
        g->player[i] = (Player){
            .x = c.x * CELL + i * .65f, .z = c.z * CELL, .yaw = .67f, .fuel = 1, .previous = USE};
}
void campaign_target(const Game *g, float *x, float *z)
{
    static const Cell stations[2][5] = {{{3, 3}, {1, 15}, {3, 11}, {1, 5}, {5, 21}},
                                        {{23, 21}, {23, 11}, {19, 23}, {17, 3}, {21, 13}}};
    Cell c = g->task == TRANSMISSION
                 ? stations[g->phase][(g->door + g->area + g->phase * 2) % 5]
                 : campaign_site(g, g->task == EXIT_TASK ? 7 : 1 + 2 * g->task + (g->phase != 0));
    *x = c.x * CELL;
    *z = c.z * CELL;
}
static int clear_path(const Game *g, float x, float z, float tx, float tz)
{
    float dist = hypotf(tx - x, tz - z);
    int steps = (int)(dist / .45f) + 1;
    for (int i = 1; i <= steps; i++) {
        float t = (float)i / steps;
        Cell c = tile(x + (tx - x) * t, z + (tz - z) * t);
        if (campaign_wall(g, c.x, c.z))
            return 0;
    }
    return 1;
}
static int aimed(const Game *g, const Player *p, Input in, float x, float z, float height)
{
    float dx = x - p->x, dz = z - p->z, d = hypotf(dx, dz);
    float yaw = p->yaw + (in.spatial_aim ? in.aim_yaw : 0),
          pitch = in.spatial_aim ? in.aim_pitch : p->pitch;
    return d < 15 && fabsf(remainderf(atan2f(dx, -dz) - yaw, 6.2831853f)) < .1f + atan2f(.8f, d) &&
           fabsf(atan2f(height - p->y - 1.55f, d) - pitch) < .2f + atan2f(1, d) &&
           clear_path(g, p->x, p->z, x, z);
}
static int walkable(const Game *g, float x, float z)
{
    for (int i = 0; i < 4; i++) {
        Cell c = tile(x + ((i & 1) ? .27f : -.27f), z + ((i & 2) ? .27f : -.27f));
        if (campaign_wall(g, c.x, c.z))
            return 0;
    }
    return 1;
}
void campaign_guard(const Game *g, float *x, float *z)
{
    campaign_target(g, x, z);
    *x += .95f * sinf(g->clock * 1.4f);
    *z += .95f * cosf(g->clock * 1.4f);
}
static int dial_code(const Game *g) { return 1 + (g->door + g->task + g->area) % 3; }
static int operate(Game *g, int seat, Input in, int pressed, int released, float distance, float x,
                   float z)
{
    Player *p = &g->player[seat];
    int kind = campaign_kind(g);
    int nearby = distance < 2.7f && clear_path(g, p->x, p->z, x, z);
    if (g->door == 1 && g->task < EXIT_TASK) {
        if (kind == SHOT) {
            if (!(pressed & FIRE) || !aimed(g, p, in, x, z, 1.55f))
                return 0;
        } else if (!nearby || !(pressed & USE) || (kind == FLY && p->y < 2.4f))
            return 0;
        if (!g->phase) {
            g->phase = 1;
            g->event = kind == SHOT ? FOLDED : OBJECTIVE;
            return 0;
        }
        return 1;
    }
    const Combination *c = combination(g);
    kind = campaign_action(g);
    int success = 0;
    if (kind == SHOT || kind == CHARGE) {
        int shot = kind == SHOT ? (pressed & FIRE) : ((released & FIRE) && g->charge[seat] >= .65f);
        success = shot && aimed(g, p, in, x, z, 1.55f);
    } else if (kind == GRID) {
        if (nearby && (pressed & USE))
            g->dial = (g->dial + 1) % 4;
        success = nearby && (pressed & FIRE) && g->dial == dial_code(g);
    } else if (nearby && (pressed & USE)) {
        if (kind == FLY)
            success = p->y >= 2.4f;
        else if (kind == PUNCH)
            success = 1;
        else if (!g->phase) {
            g->phase = 1;
            return 0;
        } else
            success = kind != ESCORT || hypotf(g->escort_x - x, g->escort_z - z) < 3.4f;
    }
    if (success && (c || g->task == TRANSMISSION) && !g->phase) {
        g->phase = 1;
        g->event = OBJECTIVE;
        return 0;
    }
    return success;
}
static void follow(Game *g, float dt)
{
    int who = 0;
    float best = 1000;
    for (int i = 0; i < g->seats; i++) {
        Player *p = &g->player[i];
        float d = hypotf(p->x - g->escort_x, p->z - g->escort_z);
        if (d < best) {
            best = d;
            who = i;
        }
    }
    if (best < .65f || best > 12)
        return;
    Player *p = &g->player[who];
    float x = p->x, z = p->z;
    if (!clear_path(g, g->escort_x, g->escort_z, x, z)) {
        Cell q = campaign_next(g, tile(g->escort_x, g->escort_z), tile(x, z));
        x = q.x * CELL;
        z = q.z * CELL;
    }
    float d = fmaxf(.001f, hypotf(x - g->escort_x, z - g->escort_z)), t = fminf(1, 4.8f * dt / d);
    g->escort_x += (x - g->escort_x) * t;
    g->escort_z += (z - g->escort_z) * t;
}
void campaign_tick(Game *g, const Input in[2], float dt)
{
    g->clock = fmodf(g->clock + dt, 120);
    g->stun = fmaxf(0, g->stun - dt);
    if (g->task < 3 && campaign_kind(g) == ESCORT && g->phase)
        follow(g, dt);
    for (int i = 0; i < g->seats; i++) {
        Player *p = &g->player[i];
        Input a = in[i];
        int pressed = a.buttons & ~p->previous, released = p->previous & ~a.buttons;
        p->previous = a.buttons;
        game_look(g, i, a.turn * dt * 1.9f, a.tilt * dt * 1.45f);
        float speed = (g->phase && campaign_kind(g) == CARRY ? 3.7f : 4.6f) * dt /
                      fmaxf(1, hypotf(a.side, a.forward));
        float x = p->x + (sinf(p->yaw) * a.forward + cosf(p->yaw) * a.side) * speed;
        float z = p->z + (-cosf(p->yaw) * a.forward + sinf(p->yaw) * a.side) * speed;
        if (walkable(g, x, p->z))
            p->x = x;
        if (walkable(g, p->x, z))
            p->z = z;
        int flight = g->door == 2 || g->door == 11;
        if (flight && !(a.buttons & BOOST)) {
            p->y = fminf(4.5f, fmaxf(0, p->y + sinf(p->pitch) * a.forward * dt * 4.6f));
            p->fuel = fminf(1, p->fuel + dt * .4f);
        } else if ((a.buttons & BOOST) && p->fuel > 0) {
            p->y = fminf(4.5f, p->y + dt * 3.8f);
            p->fuel = fmaxf(0, p->fuel - dt * .38f);
        } else {
            p->y = fmaxf(0, p->y - dt * 3.6f);
            if (p->y == 0)
                p->fuel = fminf(1, p->fuel + dt * .8f);
        }
        float gx, gz;
        campaign_guard(g, &gx, &gz);
        if (pressed & FIRE) {
            g->flash[i] = .2f;
            if (aimed(g, p, a, gx, gz, 1.5f))
                g->stun = 6;
        } else
            g->flash[i] = fmaxf(0, g->flash[i] - dt);
        if (g->stun == 0 &&
            !(g->door == 3 && g->task > 0 && expeditions[2].tools[g->area % 3].kind == GRID) &&
            p->y < 1.2f && hypotf(gx - p->x, gz - p->z) < .8f) {
            Cell safe = campaign_site(g, g->task == 0 ? 0 : 2 * g->task - 1);
            p->x = safe.x * CELL;
            p->z = safe.z * CELL;
            g->stun = 3;
        }
        campaign_target(g, &x, &z);
        float d = hypotf(p->x - x, p->z - z);
        if (g->task == 3) {
            if (d < 2.7f && (pressed & USE)) {
                if (g->area < 4)
                    campaign_enter(g, g->door, g->rehearsal, g->area + 1);
                else {
                    int event = g->rehearsal ? REHEARSAL_TICKET : TICKET;
                    if (!g->rehearsal)
                        g->completed |= 1 << (g->door - 1);
                    game_hall(g);
                    g->event = event;
                }
                return;
            }
        } else if (operate(g, i, a, pressed, released, d, x, z)) {
            int sent = g->task == TRANSMISSION;
            if (sent)
                g->task = EXIT_TASK;
            else if (g->task == 2 && (transmissions[g->door - 1].sectors & (1u << g->area)))
                g->task = TRANSMISSION;
            else
                g->task++;
            reset_tool(g);
            g->event = sent ? SIGNAL_SENT : OBJECTIVE;
            return;
        }
        g->charge[i] = (a.buttons & FIRE) ? fminf(1, g->charge[i] + dt) : 0;
    }
}
void campaign_prompt(const Game *g, int who, char *buf, int size)
{
    static const char *verbs[] = {"Use the punch",
                                  "Aim and pulse",
                                  "Lift to the high marker and use",
                                  "Recruit the escort",
                                  "Collect the building supply",
                                  "Mark the first waypoint",
                                  "Use to tune, pulse to confirm",
                                  "Pick up the cargo",
                                  "Hold pulse until charged, then release on target"};
    static const char *second[] = {"",
                                   "",
                                   "",
                                   "Guide the escort to the dock and use",
                                   "Install the supply at the foundation",
                                   "Mark the far waypoint",
                                   "",
                                   "Deliver the cargo",
                                   ""};
    const Expedition *e = &expeditions[g->door - 1];
    const Player *p = &g->player[who];
    float x, z;
    campaign_target(g, &x, &z);
    Cell from = tile(p->x, p->z), to = tile(x, z), next = campaign_next(g, from, to);
    float dx = next.x * CELL - p->x, dz = next.z * CELL - p->z;
    float angle = remainderf(atan2f(dx, -dz) - p->yaw, 6.2831853f);
    const char *direction = same(from, to)         ? "at marker"
                            : fabsf(angle) < .62f  ? "ahead"
                            : fabsf(angle) > 2.45f ? "behind"
                            : angle < 0            ? "left"
                                                   : "right";
    int k = campaign_kind(g);
    const char *action = g->task == 3 ? "Use the star gate" : g->phase ? second[k] : verbs[k];
    if (g->door == 1 && g->task < 3) {
        if (k == PUNCH)
            action = g->phase ? "Use the ticket punch" : "Collect the unpunched ticket";
        if (k == SHOT)
            action = g->phase ? "Fold the usher at the second turnstile"
                              : "Aim and fold the first paper usher";
        if (k == FLY)
            action = g->phase ? "Lift to the booth and load the reel"
                              : "Lift to the rigging and collect the reel";
    }
    if (g->phase && combination(g))
        action = combination(g)->instruction;
    const Transmission *memory = &transmissions[g->door - 1];
    if (g->task == TRANSMISSION)
        action = g->phase ? memory->destination : memory->origin;
    k = campaign_action(g);
    snprintf(buf, size, "%s | %s | %s (%s, %.0f m)", e->areas[g->area],
             g->task == TRANSMISSION ? memory->message
             : g->task == EXIT_TASK  ? e->gift
                                     : e->tools[(g->task + g->area) % 3].name,
             action, direction, hypotf(p->x - x, p->z - z));
    int used = (int)strlen(buf);
    if (g->task < 3 && (g->door == 2 || g->door == 11) && k == FLY)
        snprintf(buf + used, size - used, " | look up or down while moving to fly");
    if (k == GRID && g->task != EXIT_TASK)
        snprintf(buf + used, size - used, " | dial %d / need %d", g->dial, dial_code(g));
    if (k == ESCORT && g->phase)
        snprintf(buf + used, size - used, " | escort %.0f m away",
                 hypotf(p->x - g->escort_x, p->z - g->escort_z));
}
