#include "scene.h"
#include <math.h>
#include <string.h>

#define PI 3.14159265f
enum { PAINT, OAK = 42, FLOOR = 85, STONE = 135, GLOW = 255 };
static Vec xyz(float x, float y, float z)
{
    return (Vec){x, y, z};
}
static Vec plus(Vec a, Vec b)
{
    return xyz(a.x + b.x, a.y + b.y, a.z + b.z);
}
static Vec minus(Vec a, Vec b)
{
    return xyz(a.x - b.x, a.y - b.y, a.z - b.z);
}
static Vec scale(Vec a, float f)
{
    return xyz(a.x * f, a.y * f, a.z * f);
}
static float dot(Vec a, Vec b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
static Vec cross(Vec a, Vec b)
{
    return xyz(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}
static Vec unit(Vec a)
{
    return scale(a, 1.f / fmaxf(.0001f, sqrtf(dot(a, a))));
}

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
    unsigned edge = (day == 1 && g->completed) ? 0xd7ab4a : (ready ? 0xe0ad62 : 0xa88b60);
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
    if (g->completed)
        present(m, 3.45f, 1.24f, -7.2f, .35f, .16f, 0xe3ad65);
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

void scene_static(Mesh *m, const Game *g)
{
    m->count = 0;
    if (g->room == HALL)
        hall_mesh(m, g);
    else
        cinema_mesh(m, g);
}

/* Billboard paper actors are cut from a tiny original pixel pattern. */
static void usher(Mesh *m, int id, int folded)
{
    static const char *pattern[] = {"...ggg...", "..ggggg..", ".ggggggg.", "...fff...", "...f.f...",
                                    "...fff...", "..rrrrr..", ".rrggrrr.", "rrrgggrrr", "ffrgggrff",
                                    "..rrrrr..", "..bb.bb..", "..bb.bb..", ".bbb.bbb."};
    float x, z;
    usher_position(id, &x, &z);
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
    float sy = sinf(p->yaw), cy = cosf(p->yaw), sp = sinf(p->pitch), cp = cosf(p->pitch);
    Vec right = xyz(cy, 0, sy), forward = xyz(sy * cp, sp, -cy * cp), up = cross(right, forward);
    Vec eye = xyz(p->x, p->y + 1.55f, p->z);
    for (int i = start; i < m->count; i++) {
        Vec v = m->v[i].pos, n = m->v[i].normal;
        m->v[i].pos = plus(eye, plus(scale(right, v.x), plus(scale(up, v.y), scale(forward, v.z))));
        m->v[i].normal = plus(scale(right, n.x), plus(scale(up, n.y), scale(forward, n.z)));
    }
}

void scene_actors(Mesh *m, const Game *g, int viewer, float time, int held_tool)
{
    m->count = 0;
    if (g->room == CINEMA) {
        for (int i = 0; i < 2; i++)
            usher(m, i, g->folded & (1 << i));
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
