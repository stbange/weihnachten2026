/* Run against the same collision, guidance and objective state as a player. */
static int trace_routes;
static void measure_leg(const Game *g, int seat)
{
    float x, z;
    campaign_target(g, &x, &z);
    Cell goal = {(int)roundf(x / CELL), (int)roundf(z / CELL)};
    Cell at = {(int)roundf(g->player[seat].x / CELL), (int)roundf(g->player[seat].z / CELL)};
    int tiles = 0, gap = 0, longest = 0, room_mask = 0;
    for (;;) {
        int stride = districts[g->door - 1].stride;
        int col = (int)roundf((float)(at.x - 8) / stride),
            row = (int)roundf((float)(at.z - 8) / stride);
        if ((unsigned)col < 5 && (unsigned)row < 5 && abs(8 + col * stride - at.x) <= 2 &&
            abs(8 + row * stride - at.z) <= 2)
            room_mask |= 1 << (col + 5 * row);
        if (campaign_near_post(g, at) || (at.x == goal.x && at.z == goal.z))
            gap = 0;
        else if (++gap > longest)
            longest = gap;
        if (at.x == goal.x && at.z == goal.z)
            break;
        Cell next = campaign_next(g, at, goal);
        assert(next.x != at.x || next.z != at.z);
        at = next;
        assert(++tiles < MAP_SIZE * MAP_SIZE);
    }
    printf("LEG %d %d %d %d %d %d %d %d %d %d %d\n", g->door, g->area, g->task, g->phase, goal.x,
           goal.z, tiles, room_mask, longest, g->impressions, journey_frames);
}
static void print_sector_map(int day, int area, int unlocked)
{
    Game g;
    game_start(&g, 1, 24, 0, 0);
    campaign_enter(&g, day, 1, area);
    g.task = unlocked ? EXIT_TASK : 0;
    Cell home = campaign_home_line(&g), vault = campaign_vault(&g), start = campaign_site(&g, 0),
         exit = campaign_site(&g, 7);
    const FieldPost *marks;
    int count = campaign_posts(&g, &marks), extent = campaign_extent(&g);
    char grid[MAP_SIZE][MAP_SIZE];
    for (int z = 0; z < extent; z++)
        for (int x = 0; x < extent; x++)
            grid[z][x] = "#..=+"[campaign_surface(&g, x, z)];
    for (int i = 0; i < count; i++)
        grid[marks[i].at.z][marks[i].at.x] = fieldwork[day - 1].moves[marks[i].slot];
    grid[home.z][home.x] = 'R';
    grid[vault.z][vault.x] = 'V';
    grid[start.z][start.x] = 'S';
    grid[exit.z][exit.x] = 'E';
    for (int z = 0; z < extent; z++) {
        fwrite(grid[z], 1, extent, stdout);
        putchar('\n');
    }
}
