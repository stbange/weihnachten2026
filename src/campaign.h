#ifndef LANTERN_CAMPAIGN_H
#define LANTERN_CAMPAIGN_H
#include "hall.h"
enum { MAP_SIZE = 25, AREA_COUNT = 5, CELL = 4 };
enum { CITY, WILD, WORKS, VAULT, DOCK };
enum { PUNCH, SHOT, FLY, ESCORT, BUILD, ROUTE, GRID, CARRY, CHARGE };
typedef struct {
    int kind;
    const char *name;
} Tool;
typedef struct {
    const char *title, *areas[5];
    Tool tools[3];
    const char *gift;
    int architecture;
    unsigned colour;
} Expedition;
typedef struct {
    int x, z;
} Cell;
extern const Expedition expeditions[24];
void campaign_enter(Game *g, int day, int rehearsal, int area);
void campaign_tick(Game *g, const Input in[2], float dt);
int campaign_wall(const Game *g, int x, int z);
Cell campaign_site(const Game *g, int index);
Cell campaign_next(const Game *g, Cell from, Cell to);
int campaign_action(const Game *g);
int campaign_kind(const Game *g);
void campaign_guard(const Game *g, float *x, float *z);
void campaign_target(const Game *g, float *x, float *z);
void campaign_prompt(const Game *g, int seat, char *buf, int size);
#endif
