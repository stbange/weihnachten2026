#include "campaign.h"
#include <math.h>
#include <string.h>

static float bound(float v, float lo, float hi) { return fminf(hi, fmaxf(lo, v)); }
static float sq(float x) { return x * x; }
static float distance2(float x, float z, float a, float b) { return sq(x - a) + sq(z - b); }

int calendar_open(int64_t epoch)
{
    /* 2026-12-01 00:00 Europe/Warsaw is 2026-11-30 23:00 UTC. */
    const int64_t first = 1796079600;
    if (epoch < first)
        return 0;
    if (epoch >= first + 23 * 86400)
        return 24;
    return 1 + (int)((epoch - first) / 86400);
}

void portal_position(int day, float *x, float *z)
{
    *x = day <= 12 ? -7.6f : 7.6f;
    *z = 18.5f - ((day - 1) % 12) * 3.9f;
}

void usher_position(int id, float *x, float *z)
{
    *x = id == 0 ? -2.2f : 2.4f;
    *z = id == 0 ? 0.2f : -4.4f;
}

static void spawn(Game *g)
{
    for (int i = 0; i < 2; ++i) {
        g->player[i] = (Player){.x = g->seats == 1 ? 0 : (i == 0 ? -.65f : .65f),
                                .z = g->room == HALL ? 20.5f : 9.2f,
                                .fuel = 1};
        g->flash[i] = 0;
    }
}

void game_hall(Game *g)
{
    g->room = HALL;
    g->rehearsal = 0;
    g->folded = 0;
    g->projected = 0;
    spawn(g);
}

void game_start(Game *g, int seats, int open, int rehearsal, int completed)
{
    memset(g, 0, sizeof(*g));
    g->seats = seats == 2 ? 2 : 1;
    g->open = (int)bound(open, 0, 24);
    g->completed = completed & 0xffffff;
    if (rehearsal)
        game_enter(g, 1);
    else
        game_hall(g);
}

void game_enter(Game *g, int rehearsal)
{
    g->room = CINEMA;
    g->door = 1;
    g->rehearsal = !!rehearsal;
    g->folded = 0;
    g->projected = 0;
    g->event = ENTERED;
    spawn(g);
}

int game_near(const Game *g, int who)
{
    const Player *p = &g->player[who];
    if (g->room != HALL || fabsf(p->x) < 5.6f)
        return 0;
    for (int i = 1; i <= 24; ++i) {
        float x, z;
        portal_position(i, &x, &z);
        if (distance2(p->x, p->z, x, z) < 4.0f)
            return i;
    }
    return 0;
}

void game_look(Game *g, int who, float yaw, float pitch)
{
    Player *p = &g->player[who];
    p->yaw = remainderf(p->yaw + yaw, 6.2831853f);
    p->pitch = bound(p->pitch + pitch, -1.12f, 1.12f);
}

static int blocked(const Game *g, const Player *p, float x, float z)
{
    if (g->room == HALL)
        return distance2(x, z, 0, -20.5f) < 2.7f || (x > 2.15f && x < 4.75f && z > -13 && z < -6);
    if (p->y > 2.3f)
        return 0;
    for (int i = 0; i < 2; ++i) {
        float a, b;
        usher_position(i, &a, &b);
        if (!(g->folded & (1 << i)) && distance2(x, z, a, b) < .9f)
            return 1;
    }
    return 0;
}

static void move(Game *g, int who, Input in, float dt)
{
    Player *p = &g->player[who];
    game_look(g, who, bound(in.turn, -1, 1) * dt * 1.9f, bound(in.tilt, -1, 1) * dt * 1.45f);
    float length = fmaxf(1, hypotf(in.forward, in.side));
    float forward = in.forward / length, side = in.side / length;
    float cs = cosf(p->yaw), sn = sinf(p->yaw);
    float xmax = g->room == HALL ? 7.48f : 6.5f;
    float zmin = g->room == HALL ? -27.5f : -12.7f;
    float zmax = g->room == HALL ? 22.3f : 10.55f;
    float x = bound(p->x + (sn * forward + cs * side) * dt * 4.3f, -xmax, xmax);
    float z = bound(p->z + (-cs * forward + sn * side) * dt * 4.3f, zmin, zmax);
    if (!blocked(g, p, x, p->z))
        p->x = x;
    if (!blocked(g, p, p->x, z))
        p->z = z;
    if (g->room != CINEMA)
        return;
    int balcony = p->x < -3.45f && p->z < -5.9f && p->z > -10.3f && p->y >= 2.70f;
    float floor = balcony ? 2.75f : 0;
    if ((in.buttons & BOOST) && p->fuel > 0) {
        p->y = bound(p->y + dt * 3.8f, 0, 4.8f);
        p->fuel = bound(p->fuel - dt * .46f, 0, 1);
    } else {
        p->y = fmaxf(floor, p->y - dt * 3.9f);
        if (p->y <= floor + .01f)
            p->fuel = bound(p->fuel + dt * .9f, 0, 1);
    }
}

static void fire(Game *g, int who, Input input)
{
    Player *p = &g->player[who];
    g->flash[who] = .20f;
    if (g->room != CINEMA)
        return;
    float yaw = p->yaw + (input.spatial_aim ? input.aim_yaw : 0);
    float pitch = input.spatial_aim ? input.aim_pitch : p->pitch;
    int best = -1;
    float nearest = 100.f;
    for (int i = 0; i < 2; ++i) {
        float x, z;
        usher_position(i, &x, &z);
        float dx = x - p->x, dz = z - p->z, d = hypotf(dx, dz);
        float error = remainderf(atan2f(dx, -dz) - yaw, 6.2831853f);
        float elevation = atan2f(1.9f - (p->y + 1.55f), fmaxf(.1f, d));
        if (!(g->folded & (1 << i)) && d < nearest && d < 13.f &&
            fabsf(error) < atan2f(.85f, d) + .045f &&
            fabsf(elevation - pitch) < atan2f(1.35f, d) + .055f) {
            best = i;
            nearest = d;
        }
    }
    if (best >= 0) {
        g->folded |= 1 << best;
        g->event = FOLDED;
    }
}

static int interact(Game *g, int who, int use)
{
    Player *p = &g->player[who];
    if (g->room == HALL) {
        int door = game_near(g, who);
        if (!door)
            return 0;
        g->door = door;
        if (door <= g->open && (use || fabsf(p->x) > 7.16f)) {
            campaign_enter(g, door, 0, 0);
            return 1;
        }
        if (use)
            g->event = door > g->open ? LOCKED : OPEN_BRIEF;
    } else if (use && g->folded == 3 && !g->projected && p->y >= 2.6f &&
               distance2(p->x, p->z, -5.1f, -8.15f) < 4.6f) {
        g->projected = 1;
        g->event = PROJECTED;
    } else if (use && g->projected && distance2(p->x, p->z, 0, 10) < 5.f) {
        int ticket = g->rehearsal ? REHEARSAL_TICKET : TICKET;
        if (!g->rehearsal)
            g->completed |= 1;
        game_hall(g);
        g->event = ticket;
        return 1;
    }
    return 0;
}

void game_step(Game *g, const Input in[2], float dt)
{
    if (g->room == EXPEDITION) {
        campaign_tick(g, in, dt);
        return;
    }
    for (int i = 0; i < g->seats; ++i) {
        int pressed = in[i].buttons & ~g->player[i].previous;
        g->player[i].previous = in[i].buttons;
        g->flash[i] = fmaxf(0, g->flash[i] - dt);
        move(g, i, in[i], dt);
        if (pressed & FIRE)
            fire(g, i, in[i]);
        if (interact(g, i, pressed & USE))
            break;
    }
}

/* A versioned numeric wire layout, independent of C structure packing. */
void game_pack(const Game *g, float out[SNAP_WORDS])
{
    out[0] = WIRE_VERSION;
    out[22] = g->area;
    out[23] = g->task;
    out[24] = g->phase;
    out[25] = g->dial;
    out[26] = g->escort_x;
    out[27] = g->escort_z;
    out[28] = g->clock;
    out[29] = g->stun;
    out[30] = g->charge[0];
    out[31] = g->charge[1];
    out[38] = g->impressions;
    out[32] = g->field_open;
    out[33] = g->ward;
    for (int i = 0; i < 2; i++) {
        out[34 + i] = g->last_post[i];
        out[36 + i] = g->burst[i];
    }
    out[1] = g->room;
    out[2] = g->open;
    out[3] = g->rehearsal;
    out[4] = g->folded;
    out[5] = g->projected;
    out[6] = g->completed;
    out[7] = g->event;
    out[8] = g->door;
    out[9] = g->seats;
    for (int i = 0; i < 2; ++i) {
        const Player *p = &g->player[i];
        float *v = out + 10 + 6 * i;
        v[0] = p->x;
        v[1] = p->y;
        v[2] = p->z;
        v[3] = p->yaw;
        v[4] = p->pitch;
        v[5] = p->fuel;
    }
}

int game_unpack(Game *g, const float in[SNAP_WORDS])
{
    const int limits[] = {WIRE_VERSION, EXPEDITION, 24, 1, 3, 1, 0xffffff, SIGNAL_SENT, 24, 2};
    for (int i = 0; i < SNAP_WORDS; ++i)
        if (!isfinite(in[i]))
            return 0;
    for (int i = 0; i < 10; ++i)
        if (in[i] < 0 || in[i] > limits[i] || in[i] != floorf(in[i]))
            return 0;
    int world = (MAP_SIZE - 1) * CELL;
    const int extra_limits[] = {4, FIELD_CHECK, 2, 3,         world,     world, 120, 6, 1,
                                1, 1,           1, MAX_POSTS, MAX_POSTS, 4,     4,   7};
    for (int i = 22; i < SNAP_WORDS; i++)
        if (in[i] < 0 || in[i] > extra_limits[i - 22] ||
            ((i < 26 || (i >= 32 && i < 36) || i == 38) && floorf(in[i]) != in[i]))
            return 0;
    int expedition = in[1] == EXPEDITION;
    if (in[0] != WIRE_VERSION || in[9] < 1 || ((expedition || in[7] == TICKET) && in[8] < 1))
        return 0;
    if (expedition) {
        Game route = {
            .door = (int)in[8], .area = (int)in[22], .task = (int)in[23], .phase = (int)in[24]};
        if (!campaign_valid_step(&route))
            return 0;
    }
    for (int i = 0; i < 2; ++i) {
        const float *v = in + 10 + 6 * i;
        if ((expedition ? (v[0] < 0 || v[0] > world) : fabsf(v[0]) > 8) || v[1] < 0 || v[1] > 5 ||
            (expedition ? (v[2] < 0 || v[2] > world) : fabsf(v[2]) > 30) || fabsf(v[3]) > 3.15f ||
            fabsf(v[4]) > 1.13f || v[5] < 0 || v[5] > 1)
            return 0;
    }
    g->area = in[22];
    g->task = in[23];
    g->phase = in[24];
    g->dial = in[25];
    g->escort_x = in[26];
    g->escort_z = in[27];
    g->clock = in[28];
    g->stun = in[29];
    g->charge[0] = in[30];
    g->charge[1] = in[31];
    g->impressions = in[38];
    g->field_open = in[32];
    g->ward = in[33];
    for (int i = 0; i < 2; i++) {
        g->last_post[i] = in[34 + i];
        g->burst[i] = in[36 + i];
    }
    g->room = (int)in[1];
    g->open = (int)in[2];
    g->rehearsal = (int)in[3];
    g->folded = (int)in[4];
    g->projected = (int)in[5];
    g->completed = (int)in[6];
    g->event = (int)in[7];
    g->door = (int)in[8];
    g->seats = (int)in[9];
    for (int i = 0; i < 2; ++i) {
        const float *v = in + 10 + 6 * i;
        g->player[i] = (Player){v[0], v[1], v[2], v[3], v[4], v[5], 0};
    }
    return 1;
}
