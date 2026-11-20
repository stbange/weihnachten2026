#ifndef LANTERN_CAMPAIGN_H
#define LANTERN_CAMPAIGN_H
#include "hall.h"
enum { MAP_SIZE = 97, AREA_COUNT = 5, CELL = 4 };
enum { EXIT_TASK = 3, TRANSMISSION = 4, SET_PIECE = 5, SHELTER = 6, MAIL_RUN = 7, FIELD_CHECK = 8 };
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
enum { BACKSTAGE, WIND, CAUSEWAY, FERRY, COOLANT, REFUGE, TRAM, SKI, AIRLOCK, RIFT, FLARE, CHRONO };
enum { BARRIER, PASSAGE, COURT, FEATURE_LANE, FEATURE_PAD };
typedef struct {
    int stride, width, court, skyline;
    unsigned ground, boundary;
    int feature, unlock;
    const char *name, *hint;
} District;
enum { VOLLEY, LENS, PLACE, HIGH, LOW, POWER, BEAM, ORB };
typedef struct {
    int action;
    const char *name, *instruction;
} Encounter;
typedef struct {
    const char *forks, *moves, *stations[3], *prize, *shelter;
} Fieldwork;
extern const Fieldwork fieldwork[24];
typedef struct {
    const char *name, *sightline, *rows[7];
} WardRoom;
extern const WardRoom ward_rooms[24];
enum { MAX_POSTS = 384 };
typedef struct {
    Cell at;
    int slot;
} FieldPost;
int campaign_posts(const Game *g, const FieldPost **posts);
int campaign_near_post(const Game *g, Cell from);
Cell campaign_vault(const Game *g);
Cell campaign_home_line(const Game *g);
Cell campaign_shelter(const Game *g);
void campaign_sweep(const Game *g, float *x, float *z);
int campaign_required(const Game *g);
int campaign_field_goal(const Game *g);
int campaign_quiet(const Game *g);
const char *campaign_gesture(char move);
const Encounter *campaign_encounter(const Game *g);
void campaign_sight(const Game *g, float *x, float *y, float *z);
int campaign_cue(const Game *g);
extern const District districts[24];
int campaign_valid_step(const Game *g);
float campaign_ground(const Game *g, float x, float z);
int campaign_extent(const Game *g);
int campaign_surface(const Game *g, int x, int z);
int campaign_feature_active(const Game *g);
Cell campaign_pad(const Game *g, int end);
const char *campaign_brief(const Game *g);
extern const Expedition expeditions[24];
void campaign_enter(Game *g, int day, int rehearsal, int area);
void campaign_tick(Game *g, const Input in[2], float dt);
const char *campaign_chamber_name(const Game *g);
char campaign_chamber_tile(const Game *g, int x, int z);
int campaign_wall(const Game *g, int x, int z);
Cell campaign_site(const Game *g, int index);
Cell campaign_next(const Game *g, Cell from, Cell to);
int campaign_action(const Game *g);
int campaign_kind(const Game *g);
void campaign_guard(const Game *g, float *x, float *z);
void campaign_target(const Game *g, float *x, float *z);
void campaign_prompt(const Game *g, int seat, char *buf, int size);
#endif
