/* Face a visible signal while preserving the chosen direction of travel. */
static Input mark_input(Game *g, int seat, Cell loc, char move, float heading, float motion)
{
    Player *p = &g->player[seat];
    float x = loc.x * CELL, z = loc.z * CELL, dx = x - p->x, dz = z - p->z;
    float yaw = atan2f(dx, -dz), distance = hypotf(dx, dz);
    float pitch = atan2f(
        1.55f + campaign_ground(g, x, z) - campaign_ground(g, p->x, p->z) - p->y - 1.55f, distance);
    game_look(g, seat, yaw - p->yaw, pitch - p->pitch);
    Input in = {.forward = motion * cosf(heading - yaw), .side = motion * sinf(heading - yaw)};
    if (move == 'h')
        in.buttons = BOOST;
    if (move == 'l') {
        game_look(g, seat, heading - p->yaw, -.85f - p->pitch);
        in.forward = motion ? motion : .2f;
        in.side = 0;
    }
    if (move == 'u' && !(p->previous & USE))
        in.buttons = USE;
    if ((move == 'p' || (move == 't' && fmodf(g->clock, 2) < 1)) && !(p->previous & FIRE))
        in.buttons = FIRE;
    if (move == 'c' && g->charge[seat] < .5f)
        in.buttons = FIRE;
    return in;
}
