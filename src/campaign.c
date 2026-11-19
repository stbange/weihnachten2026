#include "campaign.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
const Expedition expeditions[24] = {
#include "expeditions.inc"
};
typedef struct {
    const char *name, *rows[5];
} Chamber;
static const Chamber chambers[24] = {
#include "chambers.inc"
};
const char *campaign_chamber_name(const Game *g) { return chambers[g->door - 1].name; }
typedef struct {
    const char *spine, *branches, *courts, *link, *order, *staging, *brief;
} SectorPlan;
static const SectorPlan plans[24][AREA_COUNT] = {
#include "floorplans.inc"
};
static const Encounter encounters[24][3] = {
#include "encounters.inc"
};
const District districts[24] = {
#include "districts.inc"
};
static const SectorPlan *plan(const Game *g) { return &plans[g->door - 1][g->area]; }
int campaign_valid_step(const Game *g)
{
    if (g->task == SET_PIECE)
        return 2 * (unsigned)g->phase < strlen(plan(g)->staging);
    return g->phase <= 1 && (g->task >= EXIT_TASK || (unsigned)g->task < strlen(plan(g)->order));
}
const char *campaign_brief(const Game *g) { return plan(g)->brief; }
int campaign_extent(const Game *g) { return 17 + 4 * districts[g->door - 1].stride; }
static const struct Relief {
    float step;
    const char *levels;
} relief[24] = {
#include "relief.inc"
};
float campaign_ground(const Game *g, float x, float z)
{
    if (g->room != EXPEDITION)
        return 0;
    const struct Relief *r = &relief[g->door - 1];
    float stride = districts[g->door - 1].stride;
    float u = fminf(4, fmaxf(0, (x / CELL - 8) / stride)),
          v = fminf(4, fmaxf(0, (z / CELL - 8) / stride));
    int i = (int)fminf(3, u), j = (int)fminf(3, v);
    const char *h = r->levels + i + 5 * j;
    float north = (h[0] - '0') + (h[1] - h[0]) * (u - i),
          south = (h[5] - '0') + (h[6] - h[5]) * (u - i);
    return (north + (south - north) * (v - j)) * r->step;
}
static Cell anchor(const Game *g, char label)
{
    int i = label - 'a', stride = districts[g->door - 1].stride;
    return (Cell){8 + i % 5 * stride, 8 + i / 5 * stride};
}
Cell campaign_site(const Game *g, int i)
{
    /* The wire task IDs reserve exit before the two optional transmission stops. */
    int index = i == 7 ? 9 : i >= 8 ? i - 1 : i;
    return anchor(g, plan(g)->spine[index]);
}
Cell campaign_pad(const Game *g, int end) { return anchor(g, plan(g)->link[end]); }
static Cell chamber_origin(const Game *g)
{
    Cell c = campaign_site(g, 1);
    return (Cell){c.x - 2, c.z - 2};
}
char campaign_chamber_tile(const Game *g, int x, int z)
{
    Cell origin = chamber_origin(g);
    x -= origin.x;
    z -= origin.z;
    if ((unsigned)x >= 5 || (unsigned)z >= 5)
        return 0;
    return chambers[g->door - 1].rows[z][x];
}
static const Cell dirs[] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};
static unsigned char terrain[MAP_SIZE][MAP_SIZE];
static int cached = -1;
static int tool_slot(const Game *g, int task) { return plan(g)->order[task] - '0'; }
int campaign_feature_active(const Game *g)
{
    const District *d = &districts[g->door - 1];
    if (d->feature == WIND || d->feature == SKI || d->feature == FERRY || d->feature == COOLANT ||
        d->feature == REFUGE || d->feature == FLARE || d->feature == AIRLOCK)
        return 1;
    const char *order = plan(g)->order;
    for (int i = 0; order[i]; i++)
        if (expeditions[g->door - 1].tools[tool_slot(g, i)].kind == d->unlock)
            return (g->task == SET_PIECE ? 1 : g->task) > i;
    /* An earlier sector commissioned this route. */
    return 1;
}
static void stamp(Cell c, int radius, int surface)
{
    for (int z = -radius; z <= radius; z++)
        for (int x = -radius; x <= radius; x++)
            if (x * x + z * z <= radius * radius + radius)
                terrain[c.z + z][c.x + x] = surface;
}
static void passage(Cell a, Cell b, int radius, int surface)
{
    int dx = b.x - a.x, dz = b.z - a.z, steps = abs(dx) + abs(dz);
    for (int i = 0; i <= steps; i++) {
        float t = steps ? (float)i / steps : 0;
        stamp((Cell){a.x + (int)roundf(dx * t), a.z + (int)roundf(dz * t)}, radius, surface);
    }
}
static void trace(const Game *g, const char *path, int width, int surface)
{
    for (int i = 1; path[i]; i++)
        if (path[i] != ' ' && path[i - 1] != ' ')
            passage(anchor(g, path[i - 1]), anchor(g, path[i]), width, surface);
}
static void layout(const Game *g)
{
    const SectorPlan *p = plan(g);
    const District *d = &districts[g->door - 1];
    int key = (g->door * AREA_COUNT + g->area) * 6 + g->task;
    if (cached == key)
        return;
    cached = key;
    int active = campaign_feature_active(g);
    memset(terrain, BARRIER, sizeof(terrain));
    trace(g, p->spine, d->width, PASSAGE);
    trace(g, p->branches, d->width > 2 ? 2 : 1, PASSAGE);
    for (const char *c = p->courts; *c; c++)
        stamp(anchor(g, *c), d->court, COURT);
    Cell start = campaign_site(g, 0), booth = campaign_site(g, 1);
    stamp(start, 4, COURT);
    stamp(booth, 4, COURT);
    if (active && (d->feature == BACKSTAGE || d->feature == CAUSEWAY || d->feature == SKI ||
                   d->feature == WIND || d->feature == COOLANT || d->feature == REFUGE))
        trace(g, p->link, 1, FEATURE_LANE);
    for (int i = 0; i < 2; i++)
        stamp(campaign_pad(g, i), 1, FEATURE_PAD);
    for (int z = -2; z <= 2; z++)
        for (int x = -2; x <= 2; x++) {
            char t = chambers[g->door - 1].rows[z + 2][x + 2];
            if ((t >= 'A' && t <= 'Z') || (t == '+' && !g->task))
                terrain[booth.z + z][booth.x + x] = BARRIER;
        }
    terrain[start.z - 2][start.x] = terrain[start.z - 2][start.x + 1] = BARRIER;
}
int campaign_surface(const Game *g, int x, int z)
{
    if ((unsigned)x >= MAP_SIZE || (unsigned)z >= MAP_SIZE)
        return BARRIER;
    layout(g);
    return terrain[z][x];
}
int campaign_wall(const Game *g, int x, int z) { return campaign_surface(g, x, z) == BARRIER; }
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
        static Cell queue[MAP_SIZE * MAP_SIZE];
        static unsigned char seen[MAP_SIZE][MAP_SIZE];
        memset(seen, 0, sizeof(seen));
        int tail = 1;
        queue[0] = to;
        seen[to.z][to.x] = 1;
        for (int k = 0; k < tail; k++)
            for (int d = 0; d < 4; d++) {
                Cell p = queue[k], q = {p.x + dirs[d].x, p.z + dirs[d].z};
                if (terrain[q.z][q.x] && !seen[q.z][q.x]) {
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
const Encounter *campaign_encounter(const Game *g)
{
    return &encounters[g->door - 1][plan(g)->staging[2 * g->phase] - '0'];
}
int campaign_kind(const Game *g)
{
    static const int controls[] = {SHOT, SHOT, PUNCH, FLY, ROUTE, GRID, CHARGE, SHOT};
    if (g->task == SET_PIECE)
        return controls[campaign_encounter(g)->action];
    if (g->task == TRANSMISSION) {
        const Transmission *t = &transmissions[g->door - 1];
        return g->phase ? t->second : t->first;
    }
    return g->task == EXIT_TASK ? PUNCH
                                : expeditions[g->door - 1].tools[tool_slot(g, g->task)].kind;
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
    static const Combination pump = {GRID, PUNCH, "Use the accumulator release at the far pump"};
    static const Combination pod = {FLY, PUNCH, "Use the rescue cradle to seal the recovered pod"};
    static const Combination aerial = {
        GRID, CHARGE, "Charge and release the constructor beam to weld the live aerial"};
    if (g->task < EXIT_TASK) {
        int tool = campaign_kind(g);
        if (g->door == 8 && tool == GRID)
            return &pump;
        if (g->door == 11 && tool == FLY)
            return &pod;
        if (g->door == 23 && tool == GRID)
            return &aerial;
    }
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
    Cell c = campaign_site(g, g->task < EXIT_TASK ? 1 + 2 * g->task : 7);
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
    Cell facing = campaign_site(g, 1);
    float yaw = atan2f(facing.x - c.x, c.z - facing.z);
    for (int i = 0; i < 2; i++)
        g->player[i] = (Player){
            .x = c.x * CELL + i * .65f, .z = c.z * CELL, .yaw = yaw, .fuel = 1, .previous = USE};
}
void campaign_target(const Game *g, float *x, float *z)
{
    if (g->task == SET_PIECE) {
        Cell site = anchor(g, plan(g)->staging[2 * g->phase + 1]);
        *x = site.x * CELL;
        *z = site.z * CELL;
        return;
    }
    Cell c = campaign_site(g, g->task == TRANSMISSION ? 8 + g->phase
                              : g->task == EXIT_TASK  ? 7
                                                      : 1 + 2 * g->task + (g->phase != 0));
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
           fabsf(atan2f(height + campaign_ground(g, x, z) - campaign_ground(g, p->x, p->z) - p->y -
                            1.55f,
                        d) -
                 pitch) < .2f + atan2f(1, d) &&
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
int campaign_cue(const Game *g)
{
    if (campaign_encounter(g)->action == ORB)
        return g->dial && g->clock >= .6f && g->clock <= 2.25f;
    return fmodf(g->clock + .4f * g->phase, 2.4f) >= 1.5f;
}
void campaign_sight(const Game *g, float *x, float *y, float *z)
{
    campaign_target(g, x, z);
    int action = campaign_encounter(g)->action;
    *y = 1.55f;
    if (action == VOLLEY) {
        *x += cosf(g->clock * 1.2f + 2 * g->dial) * 1.15f;
        *z += sinf(g->clock * 1.2f + 2 * g->dial) * 1.15f;
        *y += .35f * sinf(g->clock * 1.5f);
    }
    if (action == ORB && g->dial) {
        *y += .55f * fminf(g->clock, 2);
        *x += .8f * sinf(fminf(g->clock, 2) * 1.5f);
    }
}
static int play_encounter(Game *g, int seat, Input in, int pressed, float distance, float dt)
{
    Player *p = &g->player[seat];
    float x, y, z;
    campaign_sight(g, &x, &y, &z);
    int hit = (pressed & FIRE) && aimed(g, p, in, x, z, y);
    int action = campaign_encounter(g)->action;
    switch (action) {
    case HIGH:
        return distance < 3.0f && p->y >= 2.4f;
    case LOW:
        return distance < 3.0f && p->y < 1.f && (fabsf(in.forward) + fabsf(in.side) > .1f);
    case PLACE:
        return distance < 2.7f && (pressed & USE);
    case LENS:
        return hit && campaign_cue(g);
    case VOLLEY:
        if (hit)
            g->dial++;
        return g->dial == 3;
    case BEAM:
        if ((in.buttons & FIRE) && aimed(g, p, in, x, z, y))
            g->charge[seat] = fminf(1, g->charge[seat] + dt * .7f);
        else
            g->charge[seat] = 0;
        return g->charge[seat] >= 1;
    case ORB:
        if (hit && campaign_cue(g))
            return 1;
        if (hit && (!g->dial || g->clock > 2.25f)) {
            g->dial = 1;
            g->clock = 0;
        }
        return 0;
    default: /* A load dial beside the actual supply or machine. */
        if (distance < 2.7f && (pressed & USE))
            g->dial = (g->dial + 1) % 4;
        return distance < 2.7f && hit && g->dial == dial_code(g);
    }
}
static int operate(Game *g, int seat, Input in, int pressed, int released, float distance, float x,
                   float z)
{
    Player *p = &g->player[seat];
    int kind = campaign_kind(g);
    int nearby = distance < 2.7f && clear_path(g, p->x, p->z, x, z);
    /* A partner can prepare the shutter or weighbridge while the other player
       carries the reel or escorts the vehicle. The latch belongs to the party. */
    if (g->task < EXIT_TASK && g->phase &&
        ((g->door == 1 && kind == FLY) || (g->door == 3 && kind == ESCORT))) {
        if ((pressed & FIRE) && aimed(g, p, in, x, z, 1.55f))
            g->dial = 1;
        if (!g->dial)
            return 0;
    }
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
static int transit(const Game *g)
{
    int f = districts[g->door - 1].feature;
    return f == TRAM || f == FERRY || f == CHRONO || f == RIFT || f == AIRLOCK;
}
static int use_transit(Game *g, Player *p)
{
    if (!transit(g) || !campaign_feature_active(g))
        return 0;
    int feature = districts[g->door - 1].feature;
    if (feature == AIRLOCK && p->y < 2.4f)
        return 0;
    for (int end = 0; end < 2; end++) {
        Cell a = campaign_pad(g, end), b = campaign_pad(g, 1 - end);
        if (hypotf(p->x - a.x * CELL, p->z - a.z * CELL) >= 3.5f)
            continue;
        if (g->task < EXIT_TASK && campaign_kind(g) == ESCORT && g->phase &&
            hypotf(p->x - g->escort_x, p->z - g->escort_z) < 12) {
            g->escort_x = b.x * CELL;
            g->escort_z = b.z * CELL;
        }
        p->x = b.x * CELL;
        p->z = b.z * CELL;
        return 1;
    }
    return 0;
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
        Cell standing = tile(p->x, p->z);
        int surface = campaign_surface(g, standing.x, standing.z);
        int on_feature = surface == FEATURE_LANE || surface == FEATURE_PAD;
        int feature = districts[g->door - 1].feature;
        if (on_feature && (a.buttons & BOOST)) {
            if (feature == SKI)
                speed *= 1.85f;
            if (feature == WIND) {
                speed *= 1.45f;
                p->fuel = 1;
            }
        }
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
            if ((on_feature && feature == FLARE) || aimed(g, p, a, gx, gz, 1.5f))
                g->stun = 6;
        } else
            g->flash[i] = fmaxf(0, g->flash[i] - dt);
        if (g->task != SET_PIECE && g->stun == 0 && !(on_feature && feature == REFUGE) &&
            !(g->door == 3 && g->task > 0 && expeditions[2].tools[tool_slot(g, 0)].kind == GRID) &&
            p->y < 1.2f && hypotf(gx - p->x, gz - p->z) < .8f) {
            Cell safe = campaign_site(g, g->task == 0 ? 0 : 2 * g->task - 1);
            p->x = safe.x * CELL;
            p->z = safe.z * CELL;
            g->stun = 3;
        }
        campaign_target(g, &x, &z);
        float d = hypotf(p->x - x, p->z - z);
        /* A mission action at a shared berth takes priority over boarding. */
        if ((pressed & USE) && on_feature && d >= 2.7f && use_transit(g, p))
            continue;
        if (g->task == SET_PIECE) {
            if (play_encounter(g, i, a, pressed, d, dt)) {
                g->dial = 0;
                g->charge[0] = g->charge[1] = 0;
                if (plan(g)->staging[2 * g->phase + 2])
                    g->phase++;
                else {
                    g->task = 1;
                    reset_tool(g);
                }
                g->event = OBJECTIVE;
                return;
            }
            continue;
        }
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
            else if (g->task == 0)
                g->task = SET_PIECE;
            else if (!plan(g)->order[g->task + 1])
                g->task = (transmissions[g->door - 1].sectors & (1u << g->area)) ? TRANSMISSION
                                                                                 : EXIT_TASK;
            else
                g->task++;
            reset_tool(g);
            g->event = sent ? SIGNAL_SENT : OBJECTIVE;
            return;
        }
        g->charge[i] =
            (a.buttons & FIRE)
                ? fminf(1, g->charge[i] + dt * (on_feature && feature == COOLANT ? 3 : 1))
                : 0;
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
    if (g->task == SET_PIECE) {
        const Encounter *beat = campaign_encounter(g);
        char state[45] = "";
        switch (beat->action) {
        case VOLLEY:
            snprintf(state, sizeof(state), " | %d/3", g->dial);
            break;
        case LENS:
            snprintf(state, sizeof(state), " | %s",
                     campaign_cue(g) ? "GREEN - pulse" : "wait for green");
            break;
        case ORB:
            snprintf(state, sizeof(state), " | %s",
                     !g->dial || g->clock > 2.25f ? "ready to tag"
                     : campaign_cue(g)            ? "BLOOM - pulse"
                                                  : "orb forming");
            break;
        case BEAM:
            snprintf(state, sizeof(state), " | %.0f%%", 100 * g->charge[who]);
            break;
        case POWER:
            snprintf(state, sizeof(state), " | dial %d / need %d", g->dial, dial_code(g));
            break;
        }
        snprintf(buf, size, "%s | %s | %s (%s, %.0f m)%s", e->areas[g->area], beat->name,
                 beat->instruction, direction, hypotf(p->x - x, p->z - z), state);
        return;
    }
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
    if (g->task < EXIT_TASK && g->phase && !g->dial) {
        if (g->door == 1 && k == FLY)
            action = "Pulse the amber shutter lens below the booth";
        if (g->door == 3 && k == ESCORT)
            action = "Pulse the amber weighbridge relay, then guide the harvester onto the scale";
    }
    if (g->phase && combination(g))
        action = combination(g)->instruction;
    const Transmission *memory = &transmissions[g->door - 1];
    if (g->task == TRANSMISSION)
        action = g->phase ? memory->destination : memory->origin;
    k = campaign_action(g);
    snprintf(buf, size, "%s%s%s | %s | %s (%s, %.0f m)", e->areas[g->area],
             g->task == 0 ? " · " : "", g->task == 0 ? campaign_chamber_name(g) : "",
             g->task == TRANSMISSION ? memory->message
             : g->task == EXIT_TASK  ? e->gift
                                     : e->tools[tool_slot(g, g->task)].name,
             action, direction, hypotf(p->x - x, p->z - z));
    int used = (int)strlen(buf);
    if (g->task < 3 && (g->door == 2 || g->door == 11) && k == FLY)
        snprintf(buf + used, size - used, " | look up or down while moving to fly");
    if (k == GRID && g->task != EXIT_TASK)
        snprintf(buf + used, size - used, " | dial %d / need %d", g->dial, dial_code(g));
    if (k == ESCORT && g->phase)
        snprintf(buf + used, size - used, " | escort %.0f m away",
                 hypotf(p->x - g->escort_x, p->z - g->escort_z));
    int surface = campaign_surface(g, from.x, from.z);
    if (surface == FEATURE_LANE || surface == FEATURE_PAD) {
        const District *district = &districts[g->door - 1];
        used = (int)strlen(buf);
        snprintf(buf + used, size - used, " | %s%s", district->name,
                 transit(g) && campaign_feature_active(g) ? " (Use at centre)" : "");
    }
}
