/* Drive the route with player inputs. The bot reads its compass, just as a player can. */
#include "campaign.h"
#include "field_inputs.h"
enum { GUIDED, RUSH, EXPLORE, CAUTIOUS };
static int journey_frames, policy, awards, prizes, longest_gap, last_action, returns_used,
    final_stage;

#include "route_audit.h"
static int last_room, reward_rooms, widest_reward_gap;
static void drive_frame(Game *g, int seat, Input action)
{
    journey_frames++;
    Input in[2] = {{0}};
    in[seat] = action;
    int old_post = g->last_post[seat], old_open = g->field_open, old_task = g->task,
        old_phase = g->phase, old_area = g->area;
    game_step(g, in, 1.f / 30.f);
    if (g->room == EXPEDITION) {
        int stride = districts[g->door - 1].stride;
        float gx = (g->player[seat].x / CELL - 8) / stride,
              gz = (g->player[seat].z / CELL - 8) / stride;
        int col = (int)roundf(gx), row = (int)roundf(gz);
        int room =
            (fabsf(gx - col) * stride <= 2 && fabsf(gz - row) * stride <= 2) ? row * 5 + col : -1;
        if (room >= 0 && room != last_room) {
            if (++reward_rooms > widest_reward_gap)
                widest_reward_gap = reward_rooms;
        }
        last_room = room;
    }
    if (g->last_post[seat] && g->last_post[seat] != old_post)
        awards++;
    if (g->field_open && !old_open)
        prizes++;
    if (old_area != g->area || old_task != g->task || old_phase != g->phase ||
        old_post != g->last_post[seat] || old_open != g->field_open || g->room == HALL) {
        reward_rooms = 0;
        int gap = journey_frames - last_action;
        if (gap > longest_gap)
            longest_gap = gap;
        last_action = journey_frames;
    }
    if (g->room == EXPEDITION && g->area == 4 && g->task == SET_PIECE && !final_stage)
        final_stage = journey_frames;
}
static void drive_towards(Game *g, int seat, float x, float z, float stop)
{
    Player *p = &g->player[seat];
    for (int n = 0; n < 900; n++) {
        if (hypotf(x - p->x, z - p->z) <= stop)
            return;
        float heading = atan2f(x - p->x, p->z - z), old_x = p->x, old_z = p->z;
        float motion = fminf(1, hypotf(x - p->x, z - p->z) * 3);
        game_look(g, seat, heading - p->yaw, -p->pitch);
        Input walk = {.forward = motion};
        if (g->room == EXPEDITION && policy != RUSH) {
            Cell here = {(int)roundf(p->x / CELL), (int)roundf(p->z / CELL)};
            int id = campaign_near_post(g, here);
            const FieldPost *marks;
            campaign_posts(g, &marks);
            if (id && id != g->last_post[seat]) {
                FieldPost m = marks[id - 1];
                char move = fieldwork[g->door - 1].moves[m.slot];
                float tx, tz;
                campaign_target(g, &tx, &tz);
                if (hypotf(p->x - tx, p->z - tz) > 14 &&
                    campaign_surface(g, here.x, here.z) != FEATURE_PAD &&
                    !(g->field_open && move == 'u' &&
                      (hypotf(p->x - campaign_vault(g).x * CELL,
                              p->z - campaign_vault(g).z * CELL) < 4 ||
                       hypotf(p->x - campaign_home_line(g).x * CELL,
                              p->z - campaign_home_line(g).z * CELL) < 4)))
                    walk = mark_input(g, seat, m.at, move, heading, motion);
            }
        }
        if (g->room == EXPEDITION && !campaign_quiet(g) && g->stun < .1f &&
            (policy == EXPLORE || policy == CAUTIOUS)) {
            float sx, sz;
            campaign_sweep(g, &sx, &sz);
            if (g->field_open || hypotf(sx - p->x, sz - p->z) > 7)
                campaign_guard(g, &sx, &sz);
            if (hypotf(sx - p->x, sz - p->z) < 7) {
                float yaw = atan2f(sx - p->x, p->z - sz);
                game_look(g, seat, yaw - p->yaw, -p->pitch);
                walk = (Input){.forward = motion * cosf(heading - yaw),
                               .side = motion * sinf(heading - yaw),
                               .buttons = p->previous & FIRE ? 0 : FIRE};
            }
        }
        drive_frame(g, seat, walk);
        if (hypotf(p->x - old_x, p->z - old_z) > 2)
            return;
    }
    fprintf(stderr,
            "Stuck day=%d area=%d task=%d phase=%d position=%.1f,%.1f destination=%.1f,%.1f\n",
            g->door, g->area, g->task, g->phase, p->x, p->z, x, z);
    assert(0);
}
static void drive_to_site(Game *g, int seat, float x, float z)
{
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
    fprintf(stderr,
            "Route stopped day %d area %d task %d phase %d policy %d seat %d from %.2f,%.2f to "
            "%.2f,%.2f\n",
            g->door, g->area, g->task, g->phase, policy, seat, p->x, p->z, x, z);
    assert(0 && "Compass loop");
}
static void drive_leg(Game *g, int seat)
{
    float x, z;
    campaign_target(g, &x, &z);
    drive_to_site(g, seat, x, z);
}
static void perform_set_piece(Game *g, int seat)
{
    int stage = g->phase;
    Player *p = &g->player[seat];
    for (int i = 0; i < 210 && g->task == SET_PIECE && g->phase == stage; i++) {
        float x, y, z;
        campaign_sight(g, &x, &y, &z);
        float pitch = atan2f(y - p->y - 1.55f, hypotf(x - p->x, z - p->z));
        game_look(g, seat, atan2f(x - p->x, p->z - z) - p->yaw, pitch - p->pitch);
        Input in = {0};
        switch (campaign_encounter(g)->action) {
        case HIGH:
            in.buttons = BOOST;
            break;
        case LOW:
            /* Free-flight pods descend by flying forward with a downward pitch. */
            game_look(g, seat, 0, -1.0f - p->pitch);
            in.forward = .25f;
            break;
        case BEAM:
            in.buttons = FIRE;
            break;
        case PLACE:
            in.buttons = (i % 2) ? USE : 0;
            break;
        case POWER:
            in.buttons =
                (i % 2) ? (g->dial == 1 + (g->door + g->area + g->task) % 3 ? FIRE : USE) : 0;
            break;
        default:
            in.buttons = (i % 2) ? FIRE : 0;
            break;
        }
        drive_frame(g, seat, in);
    }
    if (g->task == SET_PIECE && g->phase == stage) {
        fprintf(stderr, "Stalled at door %d area %d encounter %s: %.1f %.1f %.1f\n", g->door,
                g->area, campaign_encounter(g)->name, p->x, p->y, p->z);
        assert(0);
    }
}
static void drive_action(Game *g, int seat)
{
    if (g->task == FIELD_CHECK) {
        int id = campaign_field_goal(g);
        assert(id >= 0);
        const FieldPost *marks;
        campaign_posts(g, &marks);
        FieldPost m = marks[id];
        for (int i = 0; i < 150 && g->task == FIELD_CHECK && campaign_field_goal(g) == id; i++)
            drive_frame(g, seat,
                        mark_input(g, seat, m.at, fieldwork[g->door - 1].moves[m.slot], 0, 0));
        return;
    }
    if (g->task == SET_PIECE) {
        perform_set_piece(g, seat);
        return;
    }
    int kind = campaign_action(g);
    float x, z;
    campaign_target(g, &x, &z);
    Player *p = &g->player[seat];
    game_look(g, seat, atan2f(x - p->x, p->z - z) - p->yaw, -p->pitch);
    drive_frame(g, seat, (Input){0});
    int buttons = USE;
    if (g->task != 3) {
        if (g->task < EXIT_TASK && g->phase &&
            ((g->door == 1 && kind == FLY) || (g->door == 3 && kind == ESCORT))) {
            drive_frame(g, seat, (Input){.buttons = FIRE});
            drive_frame(g, seat, (Input){0});
        }
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
static void survey_vault(Game *g, int seat)
{
    Cell v = campaign_vault(g);
    char move = fieldwork[g->door - 1].moves[(g->area + 1) % 3];
    for (int tries = 0; tries < 3 && !g->field_open; tries++) {
        drive_to_site(g, seat, v.x * CELL, v.z * CELL);
        for (int i = 0; i < 150 && !g->field_open; i++)
            drive_frame(g, seat, mark_input(g, seat, v, move, 0, 0));
    }
    assert(g->field_open && g->ward);
    drive_frame(g, seat, (Input){0});
    drive_frame(g, seat, (Input){.buttons = USE});
    Cell home = campaign_home_line(g);
    assert(hypotf(g->player[seat].x - home.x * CELL, g->player[seat].z - home.z * CELL) < 1);
    returns_used++;
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
    journey_frames = awards = prizes = longest_gap = last_action = returns_used = final_stage = 0;
    last_room = -1;
    reward_rooms = widest_reward_gap = 0;
    int count = 0, visited = 0;
    while (g.room == EXPEDITION && count++ < 150) {
        int seat = g.area % seats;
        if (!(visited & (1 << g.area)) &&
            (policy == EXPLORE || (policy == CAUTIOUS && g.area == 2))) {
            survey_vault(&g, seat);
            visited |= 1 << g.area;
        }
        int before = g.task, stage = g.phase;
        if (trace_routes && policy == GUIDED && seats == 1)
            measure_leg(&g, seat);
        drive_leg(&g, seat);
        if (before == g.task && stage == g.phase)
            drive_action(&g, seat);
    }
    if (g.room != HALL)
        fprintf(stderr, "Unfinished day %d area %d task %d phase %d\n", day, g.area, g.task,
                g.phase);
    assert(g.room == HALL && g.event == (practice ? REHEARSAL_TICKET : TICKET));
    double minutes = journey_frames / 1800.0;
    const char *style = policy == GUIDED    ? (seats == 1 ? "solo" : "split")
                        : policy == EXPLORE ? "explore"
                        : policy == RUSH    ? "rush"
                                            : "cautious";
    printf("Door %02d %s %.2f min | %d marks | %d caches | %d returns | %.1f s quiet | %.1f%% "
           "climax | %d rooms\n",
           day, style, minutes, awards, prizes, returns_used, longest_gap / 30.,
           100. * final_stage / journey_frames, widest_reward_gap);
    if (policy == GUIDED)
        assert(minutes >= 17 && minutes <= 23);
    if (policy == EXPLORE)
        assert(returns_used == 5);
    assert(final_stage && awards && final_stage > journey_frames * .75);
    assert(g.completed == (practice ? 0 : 1 << (day - 1)));
}
