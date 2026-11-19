#ifndef LANTERN_DRAW_H
#define LANTERN_DRAW_H
#include "scene.h"
int draw_init(void);
void draw_view(const Game *g, int viewer, float time, const float matrix[16], int x, int y, int w,
               int h, int held_tool);
void draw_close(void);
#endif
