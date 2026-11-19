#ifndef LANTERN_HALL_H
#define LANTERN_HALL_H
#include <stdint.h>

enum { HALL, CINEMA, EXPEDITION };
enum { USE = 1, FIRE = 2, BOOST = 4 };
enum {
    NO_EVENT,
    FOLDED,
    PROJECTED,
    TICKET,
    OPEN_BRIEF,
    ENTERED,
    LOCKED,
    REHEARSAL_TICKET,
    OBJECTIVE,
    SIGNAL_SENT
};
enum { SNAP_WORDS = 32, WIRE_VERSION = 4 };
typedef struct {
    float x, y, z, yaw, pitch, fuel;
    int previous;
} Player;
typedef struct {
    float forward, side, turn, tilt;
    int buttons;
    int spatial_aim;
    float aim_yaw, aim_pitch;
} Input;
typedef struct {
    Player player[2];
    int room, seats, open, rehearsal, folded, projected, completed, event, door;
    int area, task, phase, dial;
    float escort_x, escort_z, clock, stun, charge[2], flash[2];
} Game;

int calendar_open(int64_t epoch);
void portal_position(int day, float *x, float *z);
void usher_position(int id, float *x, float *z);
void game_start(Game *g, int seats, int open, int rehearsal, int completed);
void game_hall(Game *g);
void game_enter(Game *g, int rehearsal);
int game_near(const Game *g, int who);
void game_look(Game *g, int who, float yaw, float pitch);
void game_step(Game *g, const Input in[2], float dt);
void game_pack(const Game *g, float out[SNAP_WORDS]);
int game_unpack(Game *g, const float in[SNAP_WORDS]);
#endif
