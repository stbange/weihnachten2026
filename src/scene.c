#include "scene.h"
#include "campaign.h"
#include <math.h>
#include <string.h>

#define PI 3.14159265f
enum { PAINT, OAK = 42, FLOOR = 85, STONE = 135, GLOW = 255 };
static Vec xyz(float x, float y, float z) { return (Vec){x, y, z}; }
static Vec plus(Vec a, Vec b) { return xyz(a.x + b.x, a.y + b.y, a.z + b.z); }
static Vec minus(Vec a, Vec b) { return xyz(a.x - b.x, a.y - b.y, a.z - b.z); }
static Vec scale(Vec a, float f) { return xyz(a.x * f, a.y * f, a.z * f); }
static float dot(Vec a, Vec b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
static Vec cross(Vec a, Vec b)
{
    return xyz(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}
static Vec unit(Vec a) { return scale(a, 1.f / fmaxf(.0001f, sqrtf(dot(a, a)))); }

static void tri(Mesh *m, Vec a, Vec b, Vec c, unsigned col, int mat)
{
    if (m->count + 3 > m->capacity)
        return;
    Vec n = unit(cross(minus(b, a), minus(c, a)));
    Vec vertices[] = {a, b, c};
    for (int i = 0; i < 3; i++)
        m->v[m->count++] =
            (Vertex){vertices[i], n, {col >> 16, (col >> 8) & 255, col & 255, (unsigned char)mat}};
}
static void quad(Mesh *m, Vec a, Vec b, Vec c, Vec d, unsigned col, int mat)
{
    tri(m, a, b, c, col, mat);
    tri(m, a, c, d, col, mat);
}

static void box(Mesh *m, float x, float y, float z, float w, float h, float d, unsigned col,
                int mat)
{
    float l = x - w * .5f, r = x + w * .5f, n = z - d * .5f, f = z + d * .5f, t = y + h;
    Vec a = xyz(l, y, f), b = xyz(r, y, f), c = xyz(r, y, n), e = xyz(l, y, n);
    Vec A = xyz(l, t, f), B = xyz(r, t, f), C = xyz(r, t, n), E = xyz(l, t, n);
    quad(m, a, b, B, A, col, mat);
    quad(m, b, c, C, B, col, mat);
    quad(m, c, e, E, C, col, mat);
    quad(m, e, a, A, E, col, mat);
    quad(m, A, B, C, E, col, mat);
    quad(m, e, c, b, a, col, mat);
}

/* End rings lie in the plane normal to the axis. Facets supply the retro silhouette. */
static void rod(Mesh *m, Vec a, Vec b, float ra, float rb, int sides, unsigned col, int mat)
{
    Vec axis = unit(minus(b, a));
    Vec right = unit(cross(axis, fabsf(axis.y) > .92f ? xyz(1, 0, 0) : xyz(0, 1, 0)));
    Vec up = cross(axis, right);
    for (int i = 0; i < sides; i++) {
        float t = 2 * PI * i / sides, u = 2 * PI * (i + 1) / sides;
        Vec v = plus(scale(right, cosf(t)), scale(up, sinf(t)));
        Vec w = plus(scale(right, cosf(u)), scale(up, sinf(u)));
        Vec p = plus(a, scale(v, ra)), q = plus(a, scale(w, ra));
        Vec r = plus(b, scale(w, rb)), s = plus(b, scale(v, rb));
        quad(m, p, q, r, s, col, mat);
        tri(m, b, s, r, col, mat);
    }
}
static void star(Mesh *m, Vec p, float radius, int points, unsigned bright, unsigned dark)
{
    Vec centre = plus(p, xyz(0, 0, radius * .28f));
    for (int i = 0; i < points * 2; i++) {
        float a = PI * .5f + i * PI / points, b = a + PI / points;
        float r = i % 2 ? radius * .4f : radius, s = i % 2 ? radius : radius * .4f;
        tri(m, centre, plus(p, xyz(cosf(a) * r, sinf(a) * r, 0)),
            plus(p, xyz(cosf(b) * s, sinf(b) * s, 0)), i % 2 ? bright : dark, GLOW);
    }
}

static void rose(Mesh *m)
{
    Vec p = xyz(0, 6.15f, -28.65f);
    const unsigned glass[] = {0x2f7885, 0x54adac, 0x355d86, 0x82b6ab, 0x374f78, 0x497b8d};
    for (int i = 0; i < 24; i++) {
        float a = 2 * PI * i / 24, b = a + 2 * PI / 24;
        Vec dir = xyz(cosf(a), sinf(a), 0), next = xyz(cosf(b), sinf(b), 0);
        Vec va = plus(p, scale(dir, 3.32f)), vb = plus(p, scale(next, 3.32f));
        quad(m, plus(p, scale(dir, 1.43f)), va, vb, plus(p, scale(next, 1.43f)), glass[i % 6],
             GLOW);
        tri(m, p, plus(p, scale(dir, 1.43f)), plus(p, scale(next, 1.43f)), glass[(i + 2) % 6],
            GLOW);
        rod(m, va, vb, .082f, .082f, 5, 0xddac60, PAINT);
        rod(m, plus(p, scale(dir, 1.43f)), plus(p, scale(next, 1.43f)), .04f, .04f, 4, 0xb8915c,
            PAINT);
        if (!(i % 2))
            rod(m, plus(p, scale(dir, .88f)), va, .035f, .055f, 4, 0x49392e, PAINT);
    }
    star(m, plus(p, xyz(0, 0, .12f)), 1.35f, 8, 0xffdb84, 0xd1954f);
}

static void present(Mesh *m, float x, float y, float z, float w, float h, unsigned col)
{
    box(m, x, y, z, w, h, w * .82f, col, PAINT);
    box(m, x, y - .004f, z, w * .14f, h + .014f, w * .83f, 0xe1ad58, PAINT);
    box(m, x, y + h, z, w, .012f, w * .13f, 0xe1ad58, PAINT);
    for (int k = -1; k <= 1; k += 2)
        rod(m, xyz(x, y + h, z), xyz(x + k * w * .21f, y + h + .12f, z), .035f, .072f, 5, 0xe1ad58,
            PAINT);
}

static void fir(Mesh *m, float x, float z, float size)
{
    const unsigned greens[] = {0x1e5749, 0x36765a, 0x245f4e, 0x42825e};
    const unsigned baubles[] = {0xe2a448, 0xc94f3c, 0xeed089, 0x7fbcb1};
    box(m, x, 0, z, .46f * size, 1.1f * size, .46f * size, 0x654536, OAK);
    for (int j = 0; j < 6; j++) {
        float bottom = (.55f + j * .79f) * size, r = (2.35f - j * .335f) * size;
        for (int k = 0; k < 16; k++) {
            float a = 2 * PI * k / 16 + j * .3f, b = a + 2 * PI / 16;
            Vec p = xyz(x + cosf(a) * r, bottom + (k % 2) * .24f * size, z + sinf(a) * r);
            Vec q = xyz(x + cosf(b) * r, bottom + ((k + 1) % 2) * .24f * size, z + sinf(b) * r);
            tri(m, q, p, xyz(x, bottom + 2.15f * size, z), greens[(j + k / 3) % 4], PAINT);
            if (!(k % 2)) {
                Vec ornament =
                    xyz(x + cosf(a) * r * .91f, bottom + .20f * size, z + sinf(a) * r * .91f);
                rod(m, ornament, plus(ornament, xyz(0, .18f * size, 0)), .12f * size, .08f * size,
                    6, baubles[(j + k / 2) % 4], GLOW);
            }
        }
    }
    star(m, xyz(x, 6.68f * size, z), .56f * size, 5, 0xffe3a2, 0xdb9f4d);
}

static const int digits[10][5] = {
    {7, 5, 5, 5, 7}, {2, 6, 2, 2, 7}, {7, 1, 7, 4, 7}, {7, 1, 7, 1, 7}, {5, 5, 7, 1, 1},
    {7, 4, 7, 1, 7}, {7, 4, 7, 5, 7}, {7, 1, 1, 2, 2}, {7, 5, 7, 5, 7}, {7, 5, 7, 1, 7}};
static void portal(Mesh *m, int day, const Game *g)
{
    float x, z;
    portal_position(day, &x, &z);
    int sign = x < 0 ? 1 : -1, ready = day <= g->open;
    float face = x + sign * .1f;
    unsigned fill = ready ? 0x32776b : 0x334b49;
    unsigned edge = (g->completed & (1 << (day - 1))) ? 0xd7ab4a : (ready ? 0xe0ad62 : 0xa88b60);
    box(m, x, 0, z, .19f, 2.36f, 2.20f, fill, OAK);
    for (int i = 0; i < 14; i++) {
        float a = PI * i / 14, b = PI * (i + 1) / 14;
        Vec p = xyz(face, 2.34f + 1.12f * sinf(a), z + 1.12f * cosf(a));
        Vec q = xyz(face, 2.34f + 1.12f * sinf(b), z + 1.12f * cosf(b));
        tri(m, xyz(face, 2.34f, z), p, q, fill, PAINT);
        rod(m, p, q, .083f, .083f, 4, edge, PAINT);
    }
    for (int s = -1; s <= 1; s += 2) {
        box(m, face, .04f, z + s * 1.13f, .21f, 2.31f, .14f, edge, PAINT);
        box(m, face, .05f, z + s * 1.24f, .20f, 2.48f, .08f, 0x795536, OAK);
    }
    box(m, face + sign * .03f, .12f, z, .13f, .09f, 2.17f, edge, PAINT);
    for (int i = 0; i < 2; i++) {
        int digit = i ? day % 10 : day / 10;
        for (int row = 0; row < 5; row++)
            for (int col = 0; col < 3; col++)
                if (digits[digit][row] & (4 >> col)) {
                    float hor = ((i * 4 + col) - 3.f) * .113f;
                    box(m, face + sign * .10f, 1.57f + (4 - row) * .115f, z - sign * hor, .035f,
                        .110f, .105f, 0xf4d697, GLOW);
                }
    }
    if (ready)
        box(m, face + sign * .15f, .9f, z, .06f, .075f, .48f, edge, GLOW);
}

static void chandelier(Mesh *m, float z, float y, float r)
{
    rod(m, xyz(0, y + .12f, z), xyz(0, 10.4f, z), .045f, .045f, 4, 0x876540, PAINT);
    for (int i = 0; i < 8; i++) {
        float a = PI * 2 * i / 8, b = a + PI / 4;
        Vec p = xyz(cosf(a) * r, y, z + sinf(a) * r), q = xyz(cosf(b) * r, y, z + sinf(b) * r);
        rod(m, p, q, .053f, .053f, 5, 0xb18b50, PAINT);
        if (!(i % 2))
            rod(m, p, xyz(0, y + 1.1f, z), .025f, .025f, 4, 0x7b633f, PAINT);
        rod(m, p, plus(p, xyz(0, .4f, 0)), .07f, .06f, 6, 0xf6d69a, PAINT);
        rod(m, plus(p, xyz(0, .4f, 0)), plus(p, xyz(0, .59f, 0)), .08f, .004f, 5, 0xffcc71, GLOW);
    }
}

/* Each row is an eight-pixel silhouette of an object recovered from a portal. */
static const unsigned char mementos[24][8] = {{0, 0x7e, 0xff, 0xdb, 0xff, 0x7e, 0, 0},
                                              {0x18, 0x3c, 0x18, 0x3c, 0x3c, 0x3c, 0x3c, 0x7e},
                                              {0x3c, 0x7e, 0xe7, 0xdb, 0xdb, 0xe7, 0x7e, 0x3c},
                                              {0x18, 0x18, 0x18, 0x18, 0x3c, 0x7e, 0x7e, 0x3c},
                                              {0x10, 0x18, 0x1c, 0x5e, 0x10, 0xff, 0x7e, 0x3c},
                                              {0x62, 0x62, 0x7e, 0x02, 0x7e, 0x24, 0x42, 0x81},
                                              {0xff, 0x81, 0xbd, 0x81, 0xad, 0x81, 0xbd, 0xff},
                                              {0, 0x7e, 0xff, 0xb5, 0xf7, 0xff, 0x7e, 0},
                                              {0x60, 0x7c, 0x7f, 0x63, 0x60, 0x40, 0x40, 0xe0},
                                              {0, 0xff, 0xc3, 0xa5, 0x99, 0xc3, 0xff, 0},
                                              {0x18, 0x3c, 0x7e, 0xdb, 0xe7, 0x7e, 0x3c, 0x18},
                                              {0x7e, 0xc3, 0xdb, 0xc3, 0xdb, 0xc3, 0x7f, 0x3e},
                                              {0, 0x66, 0xff, 0x99, 0xbd, 0x99, 0xff, 0x18},
                                              {0x06, 0x0c, 0x38, 0x7c, 0x7c, 0x7c, 0xfe, 0x38},
                                              {0x7c, 0xc6, 0xde, 0xc6, 0xd6, 0xc6, 0xfe, 0x7c},
                                              {0, 0x7e, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x10},
                                              {0x03, 0x07, 0x0e, 0x1c, 0x38, 0x70, 0xc0, 0xff},
                                              {0x24, 0x7e, 0xe7, 0xbd, 0xbd, 0xe7, 0x7e, 0x24},
                                              {0x3c, 0x7e, 0xe7, 0xc3, 0xdb, 0xc3, 0xff, 0x7e},
                                              {0x7e, 0xff, 0x66, 0x7e, 0x66, 0x66, 0xff, 0x7e},
                                              {0, 0x18, 0x3c, 0x76, 0xf3, 0xff, 0x7e, 0},
                                              {0x7e, 0xff, 0x99, 0xbd, 0xdb, 0x99, 0xff, 0x7e},
                                              {0x81, 0x42, 0x24, 0x99, 0x5a, 0x3c, 0x18, 0x7e},
                                              {0, 0, 0x3c, 0x7e, 0xff, 0xff, 0xc3, 0x81}};
static void keepsake(Mesh *m, int day, float x, float y, float z, float pixel)
{
    for (int row = 0; row < 8; row++)
        for (int col = 0; col < 8; col++)
            if (mementos[day - 1][row] & (128 >> col))
                box(m, x + (col - 3.5f) * pixel, y + (7 - row) * pixel, z, pixel, pixel, pixel * 2,
                    row % 3 == 0 ? 0xeac986 : expeditions[day - 1].colour, PAINT);
}
static void hall_mesh(Mesh *m, const Game *g)
{
    box(m, 0, -.25f, -3, 16.45f, .25f, 52, 0x9c8d75, FLOOR);
    box(m, 0, .004f, -2, 3.4f, .018f, 50, 0x8c3537, OAK);
    for (int s = -1; s <= 1; s += 2) {
        box(m, s * 1.61f, .025f, -2, .055f, .009f, 50, 0xe2b979, PAINT);
        box(m, s * 1.45f, .025f, -2, .037f, .009f, 50, 0xca955b, PAINT);
        box(m, s * 8.02f, -.05f, -3, .30f, 6.28f, 52, 0x637266, STONE);
        box(m, s * 7.83f, 0, -3, .16f, .50f, 52, 0x514837, OAK);
        box(m, s * 7.83f, 3.62f, -3, .22f, .17f, 52, 0x91714b, OAK);
        quad(m, xyz(s * 8, 6, -29), xyz(0, 10.6f, -29), xyz(0, 10.6f, 23), xyz(s * 8, 6, 23),
             0x47625c, OAK);
    }
    for (int i = 0; i < 24; i++) {
        float z = 21 - i * 2;
        Vec p = xyz(0, .032f, z);
        quad(m, plus(p, xyz(-.14f, 0, 0)), plus(p, xyz(0, 0, .21f)), plus(p, xyz(.14f, 0, 0)),
             plus(p, xyz(0, 0, -.21f)), 0xba8859, PAINT);
    }
    box(m, 0, 0, -29, 16, .40f + 10.2f, .52f, 0x607773, STONE);
    box(m, 0, 0, 23.35f, 16, 10.6f, .40f, 0x52665e, STONE);
    rose(m);
    for (int j = 0; j < 7; j++) {
        float z = 20.45f - j * 7.8f;
        for (int s = -1; s <= 1; s += 2) {
            float x = s * 7.42f;
            box(m, x, .02f, z, .46f, 6.07f, .5f, 0x684d3d, OAK);
            box(m, x, 0, z, .70f, .32f, .76f, 0x8d7553, STONE);
            box(m, x, 3.7f, z, .63f, .19f, .70f, 0xc0935a, OAK);
            rod(m, xyz(x, 5.95f, z), xyz(0, 10.2f, z), .20f, .20f, 4, 0x855c40, OAK);
            rod(m, xyz(x, 3.94f, z), xyz(s * 4.92f, 7.48f, z), .15f, .15f, 4, 0x916947, OAK);
            rod(m, xyz(x, 5.9f, z), xyz(s * 2.55f, 8.1f, z), .1f, .1f, 4, 0x9a7852, OAK);
            for (int k = 0; k < 12; k++) {
                float t = k / 12.f, u = (k + 1) / 12.f;
                if (j == 6)
                    break;
                Vec a = xyz(x - s * .15f, 4.82f - .70f * sinf(PI * t), z - t * 7.8f);
                Vec b = xyz(x - s * .15f, 4.82f - .70f * sinf(PI * u), z - u * 7.8f);
                rod(m, a, b, .115f, .135f, 5, k % 2 ? 0x376b50 : 0x416e51, PAINT);
                if (!(k % 2))
                    box(m, a.x - .05f * s, a.y - .16f, a.z, .11f, .14f, .11f,
                        k % 4 ? 0xe4b05a : 0xf3d094, GLOW);
            }
            if (j % 2 == 0) {
                float bx = s * 6.81f;
                quad(m, xyz(bx, 6.45f, z - .63f), xyz(bx, 6.45f, z + .63f),
                     xyz(bx, 4.55f, z + .63f), xyz(bx, 4.55f, z - .63f), 0xa8463c, PAINT);
                tri(m, xyz(bx, 4.55f, z - .63f), xyz(bx, 4.55f, z + .63f), xyz(bx, 4.03f, z),
                    0xa8463c, PAINT);
                box(m, bx - s * .02f, 4.8f, z, .045f, 1.45f, .075f, 0xe6bd78, PAINT);
            }
        }
        if (j % 2 == 1)
            chandelier(m, z, 5.65f, 1.24f);
    }
    for (int i = 1; i <= 24; i++)
        portal(m, i, g);
    fir(m, 0, -20.5f, 1.06f);
    for (int i = 0; i < 13; i++) {
        float a = i * 2.4f, r = 1.4f + (i % 3) * .42f;
        present(m, cosf(a) * r, .015f, -20.5f + sinf(a) * r, .47f + (i % 4) * .15f,
                .32f + (i % 3) * .12f, i % 2 ? 0xa43e3c : 0x34847a);
    }
    box(m, 3.45f, 1.03f, -9.5f, 1.85f, .18f, 6.1f, 0x9d6b44, OAK);
    box(m, 3.45f, 1.22f, -9.5f, .64f, .02f, 6.14f, 0xa0473e, PAINT);
    for (int j = 0; j < 4; j++)
        for (int s = -1; s <= 1; s += 2) {
            float x = 3.45f + s * 1.12f, z = -7.2f - j * 1.5f;
            box(m, x, .52f, z, .52f, .12f, .57f, 0x78543d, OAK);
            box(m, x + s * .22f, .08f, z, .1f, 1.08f, .57f, 0x6b4a36, OAK);
            box(m, 3.45f + s * .66f, 0, z, .13f, 1.03f, .13f, 0x624737, OAK);
            rod(m, xyz(3.45f + s * .56f, 1.22f, z), xyz(3.45f + s * .56f, 1.25f, z), .22f, .23f, 10,
                0xd9cbb0, PAINT);
        }
    if ((g->completed & 0x3f) == 0x3f) {
        box(m, 0, 2.8f, -27.4f, 4.6f, 2.8f, .12f, 0xd9cfa8, GLOW);
        star(m, xyz(0, 4.23f, -27.25f), .8f, 6, 0xb57657, 0x7e654d);
    }
    if ((g->completed & 0xfc0) == 0xfc0)
        for (int i = 0; i < 20; i++)
            box(m, .89f, .05f, 20 - i * 2.25f, .075f, .02f, 2.1f, 0x98c9aa, GLOW);
    if ((g->completed & 0x3f000) == 0x3f000)
        for (int i = 0; i < 6; i++)
            star(m, xyz(3.45f, 2.9f, -7.3f - i * .8f), .20f, 4, 0xf0d59a, 0xa97455);
    if (g->completed & (1 << 23)) {
        box(m, 0, 0, -27.15f, 2.8f, 2.35f, .14f, 0xeac28d, GLOW);
        box(m, 1.65f, 0, -26.6f, .16f, 2.45f, 1.35f, 0x716247, OAK);
    }
    for (int i = 0; i < 24; i++)
        if (g->completed & (1 << i))
            keepsake(m, i + 1, 2.86f + (i % 3) * .56f, 1.24f, -7.2f - (i / 3) * .65f, .055f);
}

static void cinema_mesh(Mesh *m, const Game *g)
{
    box(m, 0, -.18f, -1.5f, 14, .18f, 25, 0x746364, FLOOR);
    box(m, 0, .01f, -1.2f, 1.72f, .015f, 24, 0x8e393f, OAK);
    box(m, 0, 6.95f, -1.4f, 14, .2f, 25, 0x292b36, STONE);
    for (int s = -1; s <= 1; s += 2) {
        box(m, s * 7, 0, -1.4f, .24f, 7, 25, 0x745268, STONE);
        box(m, s * 6.83f, .1f, -1.4f, .14f, .88f, 25, 0x582f3c, OAK);
        box(m, s * 6.79f, 3.9f, -1.4f, .16f, .10f, 25, 0xc79c64, PAINT);
        for (int j = 0; j < 6; j++) {
            float z = 9 - j * 3.8f;
            box(m, s * 6.76f, 1.0f, z, .24f, 5.7f, .30f, 0x9f775c, PAINT);
            box(m, s * 6.55f, 3.05f, z, .30f, .70f, .27f, 0xf4bc71, GLOW);
        }
    }
    box(m, 0, 0, -13.1f, 14, 7, .3f, 0x563946, STONE);
    box(m, 0, 1.65f, -12.85f, 9.8f, 4.28f, .18f, 0xcd9b5b, PAINT);
    box(m, 0, 1.83f, -12.71f, 9.38f, 3.89f, .14f, g->projected ? 0xf2c889 : 0x8daaa2,
        g->projected ? GLOW : PAINT);
    star(m, xyz(0, 3.8f, -12.56f), 1.15f, 8, g->projected ? 0xffeeaf : 0xd4c59e, 0xac8459);
    for (int s = -1; s <= 1; s += 2)
        for (int i = 0; i < 4; i++)
            rod(m, xyz(s * (5.0f + i * .43f), .1f, -12.35f),
                xyz(s * (5.05f + i * .43f), 6.35f, -12.5f), .32f, .36f, 8,
                i % 2 ? 0x944345 : 0x74313e, OAK);
    for (int side = -1; side <= 1; side += 2)
        for (int j = 0; j < 4; j++)
            for (int k = 0; k < 2; k++) {
                float x = side * (3.75f + k * 1.04f), z = 4.4f - j * 2.65f;
                box(m, x, 0, z, .12f, .45f, .14f, 0x50424a, PAINT);
                box(m, x, .43f, z, .83f, .16f, .65f, 0x9d5054, PAINT);
                box(m, x, .56f, z + .26f, .85f, .60f, .15f, 0x843c49, PAINT);
            }
    box(m, -5.1f, 2.47f, -8.1f, 3.3f, .28f, 4.15f, 0xb08a63, OAK);
    for (int k = 0; k < 2; k++)
        box(m, -6.1f, 0, -9.5f + k * 2.6f, .3f, 2.47f, .32f, 0x746052, OAK);
    box(m, -5.1f, 2.75f, -8.54f, .98f, .79f, .74f, 0x476164, PAINT);
    for (int i = 0; i < 2; i++)
        rod(m, xyz(-5.52f + i * .73f, 3.77f, -8.62f), xyz(-5.52f + i * .73f, 3.77f, -8.36f), .38f,
            .38f, 8, 0xb99266, PAINT);
    box(m, -5.1f, 3.40f, -8.1f, .56f, .26f, .48f, 0xe6bb70, GLOW);
    box(m, 0, 0, 11.1f, 14, 7, .25f, 0x644a51, STONE);
    box(m, 0, 0, 10.85f, 2.15f, 2.78f, .22f, 0x416b64, OAK);
    box(m, 0, 2.83f, 10.70f, 2.30f, .36f, .2f, 0xc2bd84, GLOW);
    for (int i = 0; i < 2; i++)
        box(m, -.62f + i * 1.24f, .95f, 10.63f, .17f, .36f, .12f, 0xe1b77c, PAINT);
}

static void district_landmark(Mesh *m, const Game *g, float x, float z)
{
    /* The landmark sits on a solid tile, leaving the four-metre passages clear. */
    if (g->door == 1) {
        switch (g->area) {
        case 0:
            box(m, x, 3.6f, z, 3.4f, 1.6f, 2, 0x41817a, PAINT);
            for (int i = -1; i <= 1; i++)
                box(m, x + i, 4.2f, z, .72f, .65f, 2.02f, 0xf3cf91, GLOW);
            break;
        case 1:
            box(m, x, 4.1f, z, 3.9f, .85f, 3.9f, 0x713544, PAINT);
            for (int i = -2; i <= 2; i++)
                box(m, x + .68f * i, 4.44f, z, .2f, .24f, 3.92f, 0xffdb82, GLOW);
            break;
        case 2:
            for (int i = -1; i <= 1; i++) {
                box(m, x + i, 3.7f, z, .8f, .25f, .84f, 0xb35453, PAINT);
                box(m, x + i, 3.94f, z + .35f, .8f, .74f, .18f, 0x8e3c46, OAK);
            }
            break;
        case 3:
            box(m, x, 3.7f, z, 2, 1, 1.6f, 0x426964, PAINT);
            for (int i = -1; i <= 1; i += 2)
                rod(m, xyz(x + .6f * i, 5, z), xyz(x + .6f * i, 5, z + .45f), .64f, .64f, 9,
                    0xe4b56c, PAINT);
            break;
        case 4:
            box(m, x, 3.7f, z, 3.9f, 2.7f, .3f, 0xeddbac, GLOW);
            star(m, xyz(x, 5.02f, z + .17f), .9f, 8, 0x9b6353, 0xbe865f);
            break;
        }
    } else if (g->door == 3) {
        for (int j = -1; j <= 1; j++)
            rod(m, xyz(x + j, 3.7f, z), xyz(x + j + .3f, 5.5f + .55f * (j == 0), z), .64f, .04f, 5,
                j ? 0x6bb878 : 0xacdf9a, GLOW);
    }
}
/* Standalone silhouettes for the arrival plinth. All scenery stays within
   its two occupied tiles; the two side aisles stay clear in co-op. */
static void campaign_landmark(Mesh *m, const Game *g, float x, float z)
{
    unsigned col = expeditions[g->door - 1].colour;
    const unsigned brass = 0xc6a574, iron = 0x4c6063, ice = 0xb9dcdb, wood = 0x856548;
    switch (g->door) {
    case 2: /* Striped collection balloon and a basket suspended on four stays. */
        box(m, x, .72f, z, 1.3f, .67f, 1.04f, wood, OAK);
        for (int k = 0; k < 4; k++) {
            float dx = (k & 1) ? .49f : -.49f, dz = (k & 2) ? .42f : -.42f;
            rod(m, xyz(x + dx, 1.39f, z + dz), xyz(x + dx * 1.8f, 2.58f, z + dz * 1.8f), .034f,
                .034f, 5, brass, OAK);
        }
        rod(m, xyz(x, 1.98f, z), xyz(x, 3.05f, z), .44f, 1.54f, 10, col, PAINT);
        rod(m, xyz(x, 3.05f, z), xyz(x, 3.79f, z), 1.54f, 1.49f, 10, 0xe0ad82, PAINT);
        rod(m, xyz(x, 3.79f, z), xyz(x, 4.68f, z), 1.49f, .4f, 10, col, PAINT);
        break;
    case 4: /* Bell suspended between counterweight pylons. */
    case 16:
        for (int k = -1; k <= 1; k += 2)
            box(m, x + k * 2.5f, .24f, z, .82f, 3.85f, .96f, col, STONE);
        box(m, x, 4.09f, z, 6.15f, .38f, 1.04f, brass, PAINT);
        if (g->door == 4) {
            rod(m, xyz(x, 1.48f, z), xyz(x, 3.81f, z), 1.35f, .48f, 10, brass, PAINT);
            box(m, x, 1.1f, z, .42f, .52f, .42f, iron, PAINT);
        } else {
            rod(m, xyz(x, 2.53f, z - .57f), xyz(x, 2.53f, z + .57f), 1.40f, 1.40f, 10, brass,
                PAINT);
            for (int j = -2; j <= 2; j++) {
                box(m, x + j * .54f, .44f, z + .88f, .29f, .36f, .38f, ice, PAINT);
            }
        }
        break;
    case 6:
    case 23: /* Working walker and its dismantled counterpart. */
        for (int k = -1; k <= 1; k += 2) {
            box(m, x + k * 1.1f, .24f, z + .23f, .96f, .3f, 1.62f, iron, STONE);
            rod(m, xyz(x + k * 1.1f, .52f, z + .3f), xyz(x + k * .8f, 1.55f, z - .55f), .28f, .35f,
                6, brass, PAINT);
            rod(m, xyz(x + k * .8f, 1.55f, z - .55f), xyz(x + k * .65f, 2.6f, z), .32f, .45f, 6,
                col, PAINT);
        }
        if (g->door == 6) {
            box(m, x, 2.47f, z, 2.45f, 1.46f, 1.83f, col, PAINT);
            box(m, x, 3.36f, z + .8f, 1.26f, .43f, .31f, ice, GLOW);
            for (int k = -1; k <= 1; k += 2)
                box(m, x + k * 1.65f, 2.65f, z, .6f, .72f, 1.95f, iron, PAINT);
        } else {
            rod(m, xyz(x - 2.65f, .68f, z), xyz(x + 1.65f, 2.74f, z), .19f, .24f, 6, iron, PAINT);
            for (int j = 0; j < 4; j++)
                box(m, x - 2.4f + j * .57f, .54f + j * .25f, z, .31f, .33f, 1.4f, brass, PAINT);
        }
        break;
    case 7: /* Station clock with two faces, counter and departure lamps. */
        box(m, x, .24f, z, 6.5f, 1.1f, 2.3f, col, OAK);
        box(m, x, .24f, z, .52f, 3.1f, .55f, iron, PAINT);
        rod(m, xyz(x, 3.64f, z - .24f), xyz(x, 3.64f, z + .24f), 1.05f, 1.05f, 12, 0xf1dcaf, GLOW);
        box(m, x - .22f, 3.60f, z + .26f, .5f, .10f, .05f, iron, PAINT);
        box(m, x, 3.63f, z + .27f, .09f, .69f, .05f, iron, PAINT);
        break;
    case 8:
    case 13: /* Brine pumps and their gothic boiler-choir cousins. */
        box(m, x, .24f, z, 6.5f, .7f, 2.44f, iron, STONE);
        for (int j = -2; j <= 2; j++) {
            float h = g->door == 13 ? 3.6f - fabsf(j) * .65f : 1.4f + .68f * (j % 2 == 0);
            rod(m, xyz(x + j * 1.07f, .94f, z), xyz(x + j * 1.07f, .95f + h, z), .40f, .40f, 8, col,
                PAINT);
            rod(m, xyz(x + j * 1.07f, .99f + h, z), xyz(x + j * 1.07f, 1.13f + h, z), .53f, .53f, 8,
                brass, PAINT);
        }
        break;
    case 9:
    case 15: /* A pennant above two ski runners or an angular survey lander. */
        for (int k = -1; k <= 1; k += 2)
            rod(m, xyz(x - 2.8f, .42f, z + k * .88f), xyz(x + 2.7f, .62f, z + k * .88f), .14f, .1f,
                6, brass, PAINT);
        if (g->door == 9) {
            rod(m, xyz(x, .35f, z), xyz(x, 4.4f, z), .16f, .08f, 6, iron, PAINT);
            tri(m, xyz(x, 4.35f, z), xyz(x + 2.43f, 3.8f, z + .2f), xyz(x, 3.07f, z), col, GLOW);
        } else {
            rod(m, xyz(x - 2.46f, 1.6f, z), xyz(x, 1.72f, z), .12f, 1.27f, 5, col, PAINT);
            rod(m, xyz(x, 1.72f, z), xyz(x + 2.5f, 1.6f, z), 1.27f, .19f, 5, col, PAINT);
            box(m, x, 2.03f, z, 1.29f, .63f, .92f, ice, GLOW);
        }
        break;
    case 10:
    case 20: /* Courier signs and a watchful deck surveillance array. */
        for (int j = -1; j <= 1; j++) {
            float h = 2.7f - j * .53f, px = x + j * 2.11f;
            box(m, px, .24f, z, .38f, h, .44f, iron, PAINT);
            if (g->door == 10) {
                box(m, px, h, z, 1.92f, 1.12f, .49f, col, GLOW);
                box(m, px, h + .4f, z, 1.6f, .12f, .52f, ice, GLOW);
            } else {
                box(m, px, h, z, 1.12f, .79f, 1.27f, iron, PAINT);
                rod(m, xyz(px, h + .38f, z + .64f), xyz(px, h + .38f, z + 1.04f), .39f, .32f, 8,
                    col, GLOW);
            }
        }
        break;
    case 11: /* Rescue pod with opposed lateral and vertical thrusters. */
        rod(m, xyz(x, 1.08f, z), xyz(x, 2.25f, z), .5f, 1.45f, 8, col, PAINT);
        rod(m, xyz(x, 2.25f, z), xyz(x, 3.48f, z), 1.45f, .48f, 8, ice, PAINT);
        for (int k = -1; k <= 1; k += 2)
            rod(m, xyz(x + k * 1.1f, 2.17f, z), xyz(x + k * 2.37f, 2.17f, z), .42f, .74f, 8, iron,
                PAINT);
        break;
    case 17:
    case 19: /* Paired optical prisms or a waterfall and its carved glyph. */
        for (int k = -1; k <= 1; k += 2) {
            rod(m, xyz(x + k * 2, .24f, z), xyz(x + k * 2, 2.04f, z), .84f, .56f, 6, iron, STONE);
            rod(m, xyz(x + k * 2, 2.04f, z), xyz(x + k * 2, 2.89f, z), .32f, .9f, 4, col, GLOW);
            rod(m, xyz(x + k * 2, 2.89f, z), xyz(x + k * 2, 4.1f, z), .9f, .12f, 4, ice, PAINT);
        }
        if (g->door == 19) {
            box(m, x, .25f, z, 2.7f, 3.48f, .32f, 0x80bfbf, GLOW);
            star(m, xyz(x, 2, z + .28f), .72f, 4, brass, iron);
        }
        break;
    case 18: /* Three buildings with distinct roofs at the shared border. */
        for (int k = -1; k <= 1; k++) {
            float px = x + k * 2.3f;
            box(m, px, .24f, z, 2.0f, 1.22f, 2.04f, k < 0 ? ice : k == 0 ? 0x798963 : col, STONE);
            rod(m, xyz(px, 1.46f, z), xyz(px, 2.97f, z), 1.35f, k == 0 ? .12f : .53f, 4 + k, brass,
                PAINT);
        }
        break;
    case 21: /* Storehouse and four broad windmill sails. */
        box(m, x, .24f, z, 3.2f, 2.65f, 2.5f, col, STONE);
        rod(m, xyz(x, 2.89f, z), xyz(x, 4.04f, z), 2.0f, .22f, 4, wood, OAK);
        for (int j = 0; j < 4; j++) {
            float a = .4f + j * PI * .5f;
            tri(m, xyz(x, 2.49f, z + 1.38f), xyz(x + 2 * cosf(a), 2.49f + 2 * sinf(a), z + 1.38f),
                xyz(x + 1.8f * cosf(a + .39f), 2.49f + 1.8f * sinf(a + .39f), z + 1.38f), 0xe2cc9e,
                PAINT);
        }
        break;
    case 22: /* Painted sideshow wagon. */
        for (int k = -1; k <= 1; k += 2)
            rod(m, xyz(x + k * 2.0f, .87f, z - 1.15f), xyz(x + k * 2.0f, .87f, z + 1.15f), .63f,
                .63f, 10, iron, PAINT);
        box(m, x, 1.02f, z, 5.4f, 1.81f, 2.7f, col, OAK);
        for (int j = -2; j <= 2; j++)
            box(m, x + j, 1.62f, z, .53f, .8f, 2.73f, brass, PAINT);
        box(m, x, 2.83f, z, 5.75f, .37f, 2.99f, 0xe0bd80, PAINT);
        break;
    case 24: /* Chrono induction coils and delivery pad. */
        for (int k = -1; k <= 1; k += 2) {
            box(m, x + k * 2.4f, .24f, z, .73f, 3.2f, .64f, iron, PAINT);
            for (int j = 0; j < 4; j++)
                rod(m, xyz(x + k * 2.4f, 1 + j * .62f, z), xyz(x + k * 2.4f, 1.21f + j * .62f, z),
                    .82f, .78f, 9, brass, PAINT);
        }
        box(m, x, .25f, z, 2.7f, .18f, 2.7f, col, GLOW);
        break;
    }
}
static void arrival_court(Mesh *m, const Game *g)
{
    const Expedition *e = &expeditions[g->door - 1];
    unsigned col = e->colour;
    const float x = 14, z = 84;
    box(m, x, 0, z, 7.9f, .24f, 3.9f, 0x8c8278, STONE);
    if (g->door == 1 && g->area == 0) {
        /* Brass trolley, illuminated windows, roof collector and two street rails. */
        for (int j = -1; j <= 1; j += 2) {
            box(m, x, .26f, z + j * .97f, 9.5f, .075f, .13f, 0xbba787, PAINT);
            for (int i = -1; i <= 1; i += 2)
                rod(m, xyz(x + i * 2.65f, .63f, z + j * .83f),
                    xyz(x + i * 2.65f, .63f, z + j * 1.15f), .50f, .50f, 10, 0x38444b, PAINT);
        }
        box(m, x, .75f, z, 7.65f, 1.25f, 2.72f, 0x366960, PAINT);
        box(m, x, 1.64f, z, 7.68f, .14f, 2.77f, 0xe3b778, PAINT);
        box(m, x, 1.98f, z, 7.30f, 1.2f, 2.6f, 0x775347, OAK);
        for (int j = -3; j <= 3; j++)
            box(m, x + j, 2.09f, z, .79f, .91f, 2.65f, 0xf5cc8d, GLOW);
        box(m, x, 3.18f, z, 7.78f, .24f, 2.88f, 0x405256, PAINT);
        rod(m, xyz(x, 3.42f, z), xyz(x - 1.8f, 4.35f, z), .045f, .045f, 5, 0xd8b47e, PAINT);
        rod(m, xyz(1.9f, 4.45f, z), xyz(24, 4.45f, z), .024f, .024f, 5, 0x8c7f70, PAINT);
    } else if (g->door == 1) {
        if (g->area == 1) {
            box(m, x, .24f, z, 5.15f, 2.35f, 2.7f, col, OAK);
            box(m, x, 1.21f, z, 5.18f, .99f, 2.74f, 0xf1cea2, GLOW);
            box(m, x, 2.59f, z, 6.25f, .52f, 3.45f, 0xa66a55, PAINT);
            for (int k = -3; k <= 3; k++)
                box(m, x + k * .78f, 2.8f, z + 1.74f, .22f, .16f, .12f, 0xffdfa7, GLOW);
        } else if (g->area == 2) {
            for (int j = 0; j < 2; j++)
                for (int k = -2; k <= 2; k++) {
                    float px = x + k, pz = z + j * 1.22f - .65f, y = .32f + j * .32f;
                    box(m, px, y, pz, .84f, .38f, .78f, col, PAINT);
                    box(m, px, y + .38f, pz - .33f, .85f, .74f, .18f, col, OAK);
                }
        } else if (g->area == 3) {
            box(m, x, .24f, z, 3.15f, 1.86f, 1.97f, 0x4d6767, PAINT);
            for (int k = -1; k <= 1; k += 2)
                rod(m, xyz(x + k * .9f, 2.58f, z), xyz(x + k * .9f, 2.58f, z + .38f), .88f, .88f,
                    12, 0xc1a474, PAINT);
            rod(m, xyz(x + 1.57f, 1.55f, z), xyz(x + 2.5f, 1.55f, z), .35f, .56f, 8, 0xe4c99e,
                GLOW);
        } else {
            box(m, x, .24f, z, 7.1f, 4.05f, .72f, col, OAK);
            box(m, x, 1.08f, z + .39f, 6.3f, 2.9f, .15f, 0xe5d4ad, GLOW);
            star(m, xyz(x, 2.53f, z + .54f), 1, 7, 0x927162, 0xb49775);
        }
    } else if (g->door == 3 && g->area >= 2) {
        if (g->area == 2) {
            for (int k = -1; k <= 1; k += 2) {
                rod(m, xyz(x + k * 1.67f, .24f, z), xyz(x + k * 1.67f, 3.5f, z), 1.3f, 1.3f, 10,
                    col, PAINT);
                rod(m, xyz(x + k * 1.67f, 3.5f, z), xyz(x + k * 1.67f, 4.35f, z), 1.3f, .26f, 10,
                    0xb8a47e, PAINT);
            }
        } else {
            for (int k = -1; k <= 1; k += 2)
                box(m, x + k * 2.81f, .24f, z, .47f, 3.8f, .84f, col, PAINT);
            box(m, x, 4.04f, z, 6.6f, .51f, .94f, 0xaaa07c, PAINT);
            if (g->area == 3) {
                rod(m, xyz(x, 2.01f, z), xyz(x, 4.04f, z), .07f, .07f, 5, 0x666351, PAINT);
                present(m, x, .83f, z, 1.45f, 1.2f, 0x7c9c71);
            } else
                for (int k = -2; k <= 2; k++)
                    box(m, x + k, 1.48f, z, .16f, 2.45f, .22f, 0x9cc9c5, GLOW);
        }
    } else if (g->door == 3 && g->area == 1) {
        for (int j = -3; j <= 3; j++) {
            float h = 1.15f + .65f * ((j + 4) % 3);
            rod(m, xyz(x + j * .91f, .24f, z + .4f * (j % 2)),
                xyz(x + j * 1.02f, h, z + .4f * (j % 2)), .63f, .40f, 5, 0x69986e, PAINT);
            rod(m, xyz(x + j * 1.02f, h, z + .4f * (j % 2)),
                xyz(x + j * 1.11f, h + .84f, z + .4f * (j % 2)), .40f, .02f, 5, 0xa2dfa4, GLOW);
        }
    } else if (g->door == 3) {
        box(m, x, .55f, z, 5.4f, 1.15f, 2.75f, 0x58846a, PAINT);
        for (int j = -1; j <= 1; j += 2)
            box(m, x, .24f, z + j * 1.24f, 5.8f, .65f, .64f, 0x394748, STONE);
        box(m, x - 1.5f, 1.7f, z, 1.7f, 1.12f, 2.15f, 0x53787b, PAINT);
        box(m, x - 1.5f, 2.01f, z, 1.52f, .53f, 2.17f, 0xc4dfb4, GLOW);
        for (int j = 0; j < 5; j++)
            rod(m, xyz(x + j * .42f, 1.72f, z), xyz(x + j * .42f + .2f, 2.6f + (j % 2) * .45f, z),
                .39f, .04f, 5, 0x8be091, GLOW);
    } else if (g->door != 1 && g->door != 5 && g->door != 12 && g->door != 14) {
        campaign_landmark(m, g, x, z);
    } else if (e->architecture == WILD) {
        rod(m, xyz(x, .24f, z), xyz(x, 3.2f, z), 1.25f, .52f, 7, 0x765340, OAK);
        for (int j = -1; j <= 1; j++) {
            rod(m, xyz(x, 1.85f, z), xyz(x + j * 2.1f, 3.6f, z + j * .3f), .31f, .14f, 6, 0x8f654b,
                OAK);
            rod(m, xyz(x + j * 2.1f, 3, z), xyz(x + j * 2.1f, 5.5f - (j != 0) * .8f, z), 1.65f,
                .05f, 7, col, PAINT);
        }
    } else if (e->architecture == DOCK) {
        rod(m, xyz(x - 3.1f, 1.1f, z), xyz(x + 3.1f, 1.1f, z), .55f, 1.20f, 5, col, OAK);
        rod(m, xyz(x, .4f, z), xyz(x, 4.95f, z), .12f, .075f, 7, 0xb29572, OAK);
        tri(m, xyz(x + .14f, 4.7f, z), xyz(x + 2.9f, 2.30f, z), xyz(x + .14f, 2.20f, z), 0xe3cfa2,
            PAINT);
    } else if (e->architecture == VAULT) {
        for (int i = -1; i <= 1; i += 2) {
            rod(m, xyz(x + i * 2.4f, .24f, z), xyz(x + i * 2.4f, 3.8f, z), .68f, .45f, 7, col,
                STONE);
            box(m, x + i * 2.4f, 3.7f, z, 1.5f, .45f, 1.5f, 0xd3b484, STONE);
        }
        box(m, x, 4.15f, z, 6.4f, .75f, 1.55f, col, STONE);
        star(m, xyz(x, 3.5f, z + .83f), .8f, 6, 0xe9d28f, 0xc79967);
    }
    for (int j = -1; j <= 1; j += 2) {
        box(m, x + j * 3.65f, .23f, z + 1.55f, .18f, 2.12f, .18f, 0x6e5945, OAK);
        box(m, x + j * 3.65f, 2.35f, z + 1.55f, .48f, .61f, .48f, 0xffce7d, GLOW);
    }
}
static void chamber_scenery(Mesh *m, const Game *g, char mark, float x, float z)
{
    unsigned col = expeditions[g->door - 1].colour;
    int lit = g->task != 0;
    if (mark == ':' || mark == '+') {
        box(m, x, .018f, z, 3.96f, .026f, 1.4f, col, PAINT);
        for (int sign = -1; sign <= 1; sign += 2)
            box(m, x, .046f, z + sign * .61f, 3.96f, .015f, .075f, lit ? 0xa5dfc9 : 0xc39e76,
                lit ? GLOW : PAINT);
        if (mark == '+') {
            box(m, x, lit ? 3.4f : .1f, z, 3.94f, lit ? .34f : 3.45f, 3.91f,
                lit ? 0x497770 : 0x765346, PAINT);
            box(m, x, lit ? 3.5f : 1.6f, z, 3.97f, .19f, 3.96f, lit ? 0xa5dfc9 : 0xedb767, GLOW);
        }
        return;
    }
    box(m, x, 0, z, 3.98f, .36f, 3.98f, 0x655e57, STONE);
    switch (mark) {
    case 'L': /* Marquee or station canopy */
        box(m, x, .36f, z, 2.7f, 2.4f, 2.6f, col, OAK);
        for (int i = -1; i <= 1; i++)
            box(m, x + i * .76f, 1.2f, z, .57f, 1.3f, 2.67f, 0xefcea2, GLOW);
        box(m, x, 2.76f, z, 3.95f, .47f, 3.92f, col, PAINT);
        break;
    case 'H': /* Machine with twin exhausts and inspection lights */
        box(m, x, .36f, z, 3.7f, 1.44f, 3.45f, col, PAINT);
        for (int i = -1; i <= 1; i += 2) {
            rod(m, xyz(x + i * .94f, 1.8f, z), xyz(x + i * .94f, 3.45f, z), .54f, .42f, 8, 0x576065,
                PAINT);
            box(m, x + i * .92f, 1.20f, z, .6f, .25f, 3.5f, lit ? 0xadf09b : 0xf3b36e, GLOW);
        }
        break;
    case 'R':
    case 'A': /* Lens and shrine on faceted plinths */
        rod(m, xyz(x, .36f, z), xyz(x, 1.7f, z), 1.47f, .86f, 6, col, STONE);
        rod(m, xyz(x, 1.7f, z), xyz(x, 2.5f, z), .25f, 1.02f, mark == 'R' ? 4 : 8, 0xe0c396, PAINT);
        rod(m, xyz(x, 2.5f, z), xyz(x, 3.55f, z), 1.02f, .05f, mark == 'R' ? 4 : 8, col, GLOW);
        break;
    case 'W': /* Suspended wheel and counterweight */
        for (int i = -1; i <= 1; i += 2)
            box(m, x + i * 1.36f, .36f, z, .42f, 3.25f, .64f, col, STONE);
        rod(m, xyz(x, 2.55f, z - .35f), xyz(x, 2.55f, z + .35f), 1.25f, 1.25f, 10, 0xb69969, PAINT);
        box(m, x - .85f, .37f, z + .66f, .73f, 1.15f, .7f, col, STONE);
        break;
    case 'F':
    case 'S': /* Signal flags or dockside sails */
        rod(m, xyz(x, .36f, z), xyz(x, 4.25f, z), .22f, .12f, 6, 0xb58d66, OAK);
        tri(m, xyz(x, 4.15f, z), xyz(x + 1.65f, mark == 'S' ? 1.72f : 3.72f, z + .15f),
            xyz(x, mark == 'S' ? 1.63f : 3.0f, z), col, GLOW);
        break;
    case 'T':
    case 'B': /* Tied orchard bough or moored ember balloon */
        rod(m, xyz(x, .36f, z), xyz(x, 2.3f, z), .6f, .22f, 6, 0x715749, OAK);
        rod(m, xyz(x, 1.85f, z), xyz(x, 3.05f, z), .46f, 1.65f, 8, col, PAINT);
        rod(m, xyz(x, 3.05f, z), xyz(x, 4.4f, z), 1.65f, .24f, 8, mark == 'T' ? 0x8eaa82 : 0xda9e84,
            PAINT);
        break;
    }
}
static void expedition_mesh(Mesh *m, const Game *g)
{
    const Expedition *e = &expeditions[g->door - 1];
    static const unsigned grounds[] = {0x746872, 0x536d58, 0x617176, 0x746754, 0x736555};
    box(m, 48, -.3f, 48, 100, .3f, 100, grounds[e->architecture], FLOOR);
    arrival_court(m, g);
    for (int z = 0; z < MAP_SIZE; z++)
        for (int x = 0; x < MAP_SIZE; x++) {
            if (z == 21 && (x == 3 || x == 4))
                continue;
            char mark = campaign_chamber_tile(g, x, z);
            if (mark && mark != '.') {
                chamber_scenery(m, g, mark, x * CELL, z * CELL);
                continue;
            }
            if (!campaign_wall(g, x, z))
                continue;
            float a = x * CELL, b = z * CELL;
            int variation = (x * 7 + z * 11 + g->area) % 5;
            float height = 3.6f + variation * .55f;
            unsigned col = e->colour;
            if ((g->door == 1 || g->door == 3) && variation == 0) {
                box(m, a, 0, b, CELL, 3.7f, CELL, col, STONE);
                district_landmark(m, g, a, b);
                continue;
            }
            switch (e->architecture) {
            case CITY:
                box(m, a, 0, b, CELL, height, CELL, col, STONE);
                box(m, a, height - .65f, b, 4.02f, .29f, 4.02f, 0xe8b879, GLOW);
                if (variation == 0)
                    box(m, a, height, b, 3, .8f, 2.7f, 0x566974, PAINT);
                break;
            case WILD:
                box(m, a, 0, b, CELL, 2, CELL, 0x766c60, STONE);
                rod(m, xyz(a, 1.9f, b), xyz(a, height + 2, b), 2.45f, .05f, 7, col, OAK);
                break;
            case WORKS:
                box(m, a, 0, b, CELL, height - .6f, CELL, 0x59666b, PAINT);
                box(m, a, height - .8f, b, 4.01f, .3f, 4.01f, col, GLOW);
                rod(m, xyz(a, height - .6f, b), xyz(a, height + 1, b), 1.35f, 1.05f, 8, col, PAINT);
                break;
            case VAULT:
                box(m, a, 0, b, CELL, height, CELL, col, STONE);
                box(m, a, height - .45f, b, 4.06f, .4f, 4.06f, 0xc2ab85, STONE);
                break;
            case DOCK:
                box(m, a, 0, b, CELL, 2.8f, CELL, 0x795d47, OAK);
                box(m, a, 2.8f, b, 3.8f, height - 2, 3.8f, col, PAINT);
                rod(m, xyz(a, height + .7f, b), xyz(a, height + 2.2f, b), .12f, .1f, 5, 0xdecdae,
                    OAK);
                break;
            }
        }
    for (int i = 1; i <= 7; i++) {
        Cell c = campaign_site(g, i);
        float x = c.x * CELL, z = c.z * CELL;
        box(m, x, .005f, z, 2.8f, .055f, 2.8f, e->colour, PAINT);
        for (int sign = -1; sign <= 1; sign += 2)
            box(m, x + sign * 1.3f, .06f, z, .12f, .09f, 2.8f, 0xeccc87, GLOW);
    }
}
void scene_static(Mesh *m, const Game *g)
{
    m->count = 0;
    if (g->room == HALL)
        hall_mesh(m, g);
    else if (g->room == EXPEDITION)
        expedition_mesh(m, g);
    else
        cinema_mesh(m, g);
}

/* Billboard paper actors are cut from a tiny original pixel pattern. */
static void paper_usher(Mesh *m, float x, float z, int folded)
{
    static const char *pattern[] = {"...ggg...", "..ggggg..", ".ggggggg.", "...fff...", "...f.f...",
                                    "...fff...", "..rrrrr..", ".rrggrrr.", "rrrgggrrr", "ffrgggrff",
                                    "..rrrrr..", "..bb.bb..", "..bb.bb..", ".bbb.bbb."};
    float unit = folded ? .053f : .235f;
    for (int row = 0; row < 14; row++)
        for (int c = 0; c < 9; c++) {
            char ch = pattern[row][c];
            if (ch == '.')
                continue;
            unsigned colour = ch == 'g'   ? 0xe8bb72
                              : ch == 'f' ? 0xf3d5a4
                              : ch == 'r' ? 0xb84f53
                                          : 0x283c40;
            float a = x + (c - 4.5f) * unit, b = (13 - row) * unit + .025f;
            quad(m, xyz(a, b, z), xyz(a + unit, b, z), xyz(a + unit, b + unit, z),
                 xyz(a, b + unit, z), colour, PAINT);
        }
}

static void attach_tool(Mesh *m, const Player *p, int start)
{
    float sy = sinf(p->yaw), cy = cosf(p->yaw), sp = sinf(p->pitch), cp = cosf(p->pitch);
    Vec right = xyz(cy, 0, sy), forward = xyz(sy * cp, sp, -cy * cp), up = cross(right, forward);
    Vec eye = xyz(p->x, p->y + 1.55f, p->z);
    for (int i = start; i < m->count; i++) {
        Vec v = m->v[i].pos, n = m->v[i].normal;
        m->v[i].pos = plus(eye, plus(scale(right, v.x), plus(scale(up, v.y), scale(forward, v.z))));
        m->v[i].normal = plus(scale(right, n.x), plus(scale(up, n.y), scale(forward, n.z)));
    }
}
static void pocket_projector(Mesh *m, const Player *p, float flash)
{
    int start = m->count;
    box(m, .206f, -.21f, .40f, .12f, .17f, .17f, 0x725039, OAK);
    box(m, .206f, -.15f, .45f, .164f, .104f, .214f, 0x385353, PAINT);
    box(m, .206f, -.133f, .455f, .168f, .021f, .22f, 0xc49a58, PAINT);
    box(m, .206f, -.044f, .448f, .12f, .024f, .165f, 0x6c7770, PAINT);
    for (int i = 0; i < 2; i++) {
        Vec a = xyz(.135f, -.03f, .39f + i * .106f), b = xyz(.165f, -.03f, .39f + i * .106f);
        rod(m, a, b, .052f, .052f, 10, 0xb6975e, PAINT);
        rod(m, plus(a, xyz(-.0015f, 0, 0)), a, .020f, .020f, 8, 0x3f4640, PAINT);
    }
    rod(m, xyz(.206f, -.09f, .54f), xyz(.206f, -.09f, .64f), .055f, .052f, 8, 0xd1a45e, PAINT);
    rod(m, xyz(.206f, -.09f, .64f), xyz(.206f, -.09f, .644f), .04f, .04f, 8,
        flash > .06f ? 0xffe6a2 : 0x9ac9b9, GLOW);
    box(m, .26f, -.187f, .363f, .092f, .094f, .132f, 0xa4694b, PAINT);
    attach_tool(m, p, start);
}

static void expedition_tool(Mesh *m, const Game *g, int viewer)
{
    const Player *p = &g->player[viewer];
    int k = campaign_kind(g);
    if (g->door == 1) {
        pocket_projector(m, p, g->flash[viewer]);
        return;
    }
    int start = m->count;
    unsigned col = expeditions[g->door - 1].colour;
    unsigned lamp = g->charge[viewer] > .64f || g->flash[viewer] > .05f ? 0xffec9c : 0x91c4b6;
    if (k == CHARGE && (g->door == 14 || g->door == 17 || g->door == 19)) {
        rod(m, xyz(.16f, -.28f, .36f), xyz(.27f, .14f, .58f), .029f, .021f, 6, 0xb18b68, OAK);
        rod(m, xyz(.27f, .14f, .58f), xyz(.29f, .22f, .61f), .047f, .005f, 6, lamp, GLOW);
    } else if (k == SHOT || k == CHARGE) {
        box(m, .20f, -.17f, .47f, .15f, .10f, .29f, col, PAINT);
        for (int j = -1; j <= 1; j += 2)
            rod(m, xyz(.20f + j * .042f, -.11f, .5f), xyz(.20f + j * .042f, -.11f, .74f), .031f,
                .025f, 6, lamp, PAINT);
    } else if (k == ESCORT || k == ROUTE) {
        rod(m, xyz(.21f, -.24f, .48f), xyz(.21f, .12f, .50f), .012f, .010f, 6, 0xd0a870, OAK);
        tri(m, xyz(.21f, .11f, .50f), xyz(.36f, .078f, .51f), xyz(.21f, .004f, .50f), col, GLOW);
    } else if (k == CARRY && g->phase) {
        present(m, .20f, -.24f, .54f, .22f, .18f, col);
    } else {
        box(m, .21f, -.20f, .47f, .20f, .12f, .16f, col, PAINT);
        for (int j = 0; j < 4; j++)
            box(m, .145f + j * .043f, -.075f, .47f, .027f, .01f, .08f,
                j == g->dial ? lamp : 0x46514e, PAINT);
    }
    box(m, .21f, -.25f, .39f, .1f, .11f, .1f, 0x9a674b, OAK);
    attach_tool(m, p, start);
}
static void sentinel(Mesh *m, const Game *g, float x, float z, float time)
{
    const Expedition *e = &expeditions[g->door - 1];
    float y = g->stun > 0 ? .10f : .82f;
    if (g->door == 1) {
        paper_usher(m, x, z, g->stun > 0);
        return;
    }
    unsigned body = e->colour;
    if (e->architecture == WILD) {
        float wing = .18f * sinf(time * 6.0f);
        rod(m, xyz(x, y, z), xyz(x, y + .62f, z), .18f, .37f, 6, body, PAINT);
        for (int i = -1; i <= 1; i += 2)
            tri(m, xyz(x + i * .14f, y + .48f, z), xyz(x + i * 1.15f, y + .82f + wing, z - .2f),
                xyz(x + i * .62f, y + .19f, z + .26f), 0xe4be8f, PAINT);
    } else if (e->architecture == WORKS) {
        box(m, x, y, z, .9f, .58f, .68f, body, PAINT);
        for (int i = -1; i <= 1; i += 2) {
            box(m, x + i * .52f, y - .24f, z, .25f, .50f, .9f, 0x535454, STONE);
            box(m, x + i * .28f, y + .59f, z, .16f, .15f, .2f, 0xfad299, GLOW);
        }
    } else if (e->architecture == VAULT) {
        rod(m, xyz(x, y - .2f, z), xyz(x, y + .78f, z), .68f, .21f, 5, body, OAK);
        rod(m, xyz(x, y + .65f, z), xyz(x, y + 1.15f, z), .4f, .03f, 5, 0xd0a986, PAINT);
    } else {
        box(m, x, y, z, .60f, .68f, .52f, body, PAINT);
        box(m, x, y + .69f, z, .50f, .33f, .43f, 0xebc891, GLOW);
        box(m, x, y + 1.02f, z, .76f, .13f, .67f, 0x4a514c, PAINT);
    }
}
void scene_actors(Mesh *m, const Game *g, int viewer, float time, int held_tool)
{
    m->count = 0;
    if (g->room == EXPEDITION) {
        float x, z, gx, gz;
        campaign_target(g, &x, &z);
        campaign_guard(g, &gx, &gz);
        unsigned col = expeditions[g->door - 1].colour;
        int kind = campaign_action(g);
        if (g->task == TRANSMISSION) {
            /* A two-part field station: first recover the recording, then answer it. */
            box(m, x, .01f, z, 2.65f, .09f, 2.65f, 0x3d6262, PAINT);
            for (int sign = -1; sign <= 1; sign += 2) {
                rod(m, xyz(x + sign * .96f, .1f, z), xyz(x + sign * .96f, 2.35f, z), .075f, .045f,
                    6, 0xd4ae6f, PAINT);
                box(m, x + sign * .96f, 2.35f, z, .16f, .21f, .16f, g->phase ? 0xaee6c0 : 0xeec192,
                    GLOW);
            }
        }
        if (g->task == 3) {
            if (g->area == 4)
                keepsake(m, g->door, x, 1.0f, z, .12f);
            for (int s = -1; s <= 1; s += 2)
                box(m, x + s, 0, z, .3f, 3.4f, .5f, col, STONE);
            box(m, x, 3.1f, z, 2.3f, .3f, .5f, 0xe2be77, GLOW);
        } else if (g->door == 1 && kind == SHOT) {
            paper_usher(m, x, z, 0);
            if (g->phase) {
                Cell first = campaign_site(g, 1 + 2 * g->task);
                paper_usher(m, first.x * CELL, first.z * CELL, 1);
            }
        } else if (kind == FLY) {
            rod(m, xyz(x, 0, z), xyz(x, 3.4f, z), .33f, .2f, 6, col, PAINT);
            box(m, x, 3.4f, z, 1.75f, .24f, 1.4f, 0xd6aa66, GLOW);
        } else if (kind == SHOT || kind == CHARGE) {
            rod(m, xyz(x, .8f, z), xyz(x, 1.6f, z), .12f, .7f, 6, col, PAINT);
            rod(m, xyz(x, 1.6f, z), xyz(x, 2.4f, z), .7f, .06f, 6, 0xeacc99, GLOW);
        } else if (kind == GRID) {
            box(m, x, 0, z, 1.2f, 1.45f, .8f, col, PAINT);
            for (int i = 0; i < 4; i++)
                box(m, x - .45f + i * .3f, 1.46f, z, .18f, .14f, .4f,
                    i == g->dial ? 0xffd962 : 0x524b47, i == g->dial ? GLOW : PAINT);
            /* The rear row encodes the requested setting for stereo/headset players. */
            int answer = 1 + (g->door + g->task + g->area) % 3;
            for (int i = 0; i < answer; i++)
                box(m, x - .28f + i * .28f, 1.72f, z - .3f, .14f, .12f, .12f, 0x96ddbc, GLOW);
        } else
            present(m, x, .07f, z, 1.2f, .85f, col);
        star(m,
             xyz(x, (kind == FLY && g->task != EXIT_TASK ? 4.4f : 3.9f) + .18f * sinf(time * 2), z),
             .57f, 6, 0xffde86, 0xcc9a50);
        if (g->task < EXIT_TASK && g->phase &&
            ((g->door == 1 && kind == FLY) || (g->door == 3 && kind == ESCORT))) {
            unsigned signal = g->dial ? 0x8ae5ac : 0xf9bc65;
            box(m, x, .08f, z, 1.02f, 1.52f, .9f, 0x415d5f, PAINT);
            box(m, x, 1.43f, z, 1.06f, .25f, .96f, signal, GLOW);
            if (g->door == 1) {
                for (int j = -1; j <= 1; j += 2)
                    rod(m, xyz(x + j * .38f, 4.0f, z), xyz(x + j * .38f, 4.0f, z + .27f), .34f,
                        .34f, 10, 0xbc986a, PAINT);
                box(m, x, 3.44f, z, .93f, .49f, .62f, 0x577b70, PAINT);
            } else {
                box(m, x, .09f, z, 3.55f, .07f, 3.68f, 0x838379, STONE);
                for (int j = -1; j <= 1; j += 2)
                    box(m, x + j * 1.6f, .16f, z, .16f, .07f, 3.6f, signal, GLOW);
            }
        }
        if (g->door == 1 && g->task == EXIT_TASK) {
            box(m, x, 3.62f, z, 3.8f, 1.95f, .18f, 0xe4d5ae, GLOW);
            star(m, xyz(x + .74f * sinf(time), 4.55f, z + .11f), .48f, 6, 0xa76759, 0x80575c);
        }
        if (kind == ESCORT && g->task < 3) {
            if (g->door == 3) {
                box(m, g->escort_x, .92f, g->escort_z - .5f, .83f, .54f, .52f, 0x7fa696, GLOW);
                for (int j = -1; j <= 1; j += 2) {
                    box(m, g->escort_x + j * .7f, .12f, g->escort_z, .28f, .35f, 1.9f, 0x484e48,
                        STONE);
                    if (g->phase)
                        rod(m, xyz(g->escort_x + j * .31f, 1.10f, g->escort_z + .31f),
                            xyz(g->escort_x + j * .37f, 1.65f, g->escort_z + .41f), .24f, .04f, 5,
                            0x9ae7a4, GLOW);
                }
            }
            box(m, g->escort_x, .35f, g->escort_z, 1.25f, .8f, 1.7f, col, PAINT);
            box(m, g->escort_x, .11f, g->escort_z, 1.45f, .34f, 1.35f, 0x494647, STONE);
        }
        /* An in-world guide remains visible in stereo and native split-screen. */
        const Player *p = &g->player[viewer];
        Cell from = {(int)roundf(p->x / CELL), (int)roundf(p->z / CELL)};
        Cell next = campaign_next(g, from, (Cell){(int)(x / CELL), (int)(z / CELL)});
        float ax = next.x - from.x, az = next.z - from.z;
        if (ax || az) {
            float cx = from.x * CELL, cz = from.z * CELL;
            tri(m, xyz(cx + ax * 1.45f, .09f, cz + az * 1.45f),
                xyz(cx + ax * .45f + az * .35f, .09f, cz + az * .45f - ax * .35f),
                xyz(cx + ax * .45f - az * .35f, .09f, cz + az * .45f + ax * .35f), 0xf3cc80, GLOW);
        }
        sentinel(m, g, gx, gz, time);
        if (held_tool)
            expedition_tool(m, g, viewer);
    }
    if (g->room == CINEMA) {
        for (int i = 0; i < 2; i++) {
            float x, z;
            usher_position(i, &x, &z);
            paper_usher(m, x, z, g->folded & (1 << i));
        }
        if (held_tool)
            pocket_projector(m, &g->player[viewer], g->flash[viewer]);
    }
    if (g->room == HALL)
        for (int i = 0; i < 42; i++) {
            float x = sinf(i * 14.83f) * 6.8f, z = -27.9f + (i % 4) * .16f;
            float y = 1.0f + fmodf(i * 1.47f + time * .14f, 7.3f);
            float r = .018f + (i % 3) * .013f;
            quad(m, xyz(x - r, y, z), xyz(x, y + r, z), xyz(x + r, y, z), xyz(x, y - r, z),
                 0xe7d39f, GLOW);
        }
    if (g->seats == 2) {
        const Player *p = &g->player[1 - viewer];
        box(m, p->x, p->y + .15f, p->z, .42f, .78f, .32f, 0x35434b, PAINT);
        box(m, p->x, p->y + .84f, p->z, .63f, .70f, .43f, viewer ? 0xb95a44 : 0x4f958a, PAINT);
        box(m, p->x, p->y + 1.54f, p->z, .43f, .38f, .40f, 0xe5bd8d, PAINT);
        box(m, p->x, p->y + 1.87f, p->z, .48f, .14f, .44f, 0xc89f53, PAINT);
    }
}

void matrix_product(float out[16], const float a[16], const float b[16])
{
    float result[16] = {0};
    for (int c = 0; c < 4; c++)
        for (int r = 0; r < 4; r++)
            for (int k = 0; k < 4; k++)
                result[c * 4 + r] += a[k * 4 + r] * b[c * 4 + k];
    memcpy(out, result, sizeof(result));
}

void camera_matrix(float out[16], Vec eye, float yaw, float pitch, float aspect)
{
    Vec f = xyz(sinf(yaw) * cosf(pitch), sinf(pitch), -cosf(yaw) * cosf(pitch));
    Vec r = xyz(cosf(yaw), 0, sinf(yaw)), u = cross(r, f);
    float view[] = {r.x, u.x, -f.x, 0, r.y,          u.y,          -f.y,        0,
                    r.z, u.z, -f.z, 0, -dot(r, eye), -dot(u, eye), dot(f, eye), 1};
    const float n = .055f, far = 100.f, v = 1.44f;
    float proj[] = {v / aspect,
                    0,
                    0,
                    0,
                    0,
                    v,
                    0,
                    0,
                    0,
                    0,
                    (far + n) / (n - far),
                    -1,
                    0,
                    0,
                    (2 * far * n) / (n - far),
                    0};
    matrix_product(out, proj, view);
}
