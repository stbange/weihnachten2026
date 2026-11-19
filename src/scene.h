#ifndef LANTERN_SCENE_H
#define LANTERN_SCENE_H
#include "hall.h"
typedef struct {
    float x, y, z;
} Vec;
typedef struct {
    Vec pos, normal;
    unsigned char colour[4];
} Vertex;
typedef struct {
    Vertex *v;
    int count, capacity;
} Mesh;
void scene_static(Mesh *m, const Game *g);
void scene_actors(Mesh *m, const Game *g, int viewer, float time, int held_tool);
void camera_matrix(float out[16], Vec eye, float yaw, float pitch, float aspect);
void matrix_product(float out[16], const float a[16], const float b[16]);
#endif
