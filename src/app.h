#ifndef LANTERN_APP_H
#define LANTERN_APP_H
#ifndef __EMSCRIPTEN__
int lh_saved(void);
void lh_save_ticket(int earned);
#endif
int lh_init(void);
void lh_close(void);
void lh_resize(int w, int h);
void lh_start(int seats, int open, int rehearsal, int completed);
int lh_chart(int x,int z);
int lh_preview(int day);
const char *lh_prompt(int seat);
void lh_open(int open);
int lh_today(double epoch);
void lh_input(int who, float forward, float side, float turn, float tilt, int buttons);
void lh_point(int who, float yaw, float pitch);
void lh_aim(int who, float yaw, float pitch);
int lh_tick(float dt, int simulate);
int lh_near(int who);
int lh_event(void);
float *lh_state(void);
float *lh_wire(void);
int lh_receive(void);
float *lh_matrix(void);
void lh_xr_eye(int viewer, int x, int y, int w, int h);
void lh_draw(int title, int split, int viewer);
#endif
