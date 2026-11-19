/* Exercise the C platform interface with an actual GL context and SDL window. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "scene.h"
#include "app.h"
int lh_native_main(int, char **);
int main(void)
{
    int ready = lh_init();
    assert(ready);
    assert(lh_saved() == 0);
    lh_save_ticket();
    lh_save_ticket();
    assert(lh_saved() == 1);
    lh_start(2, 4, 0, 0);
    lh_draw(1, 0, 0);
    assert(lh_today(1796079600) == 1);
    lh_open(24);
    lh_resize(600, 320);
    lh_resize(600, 320);
    lh_draw(0, 1, 0);
    lh_resize(320, 600);
    lh_draw(0, 1, 0);
    lh_input(1, 1, 0, 0, 0, 0);
    lh_aim(1, .1f, .1f);
    lh_point(1, .2f, .1f);
    assert(lh_tick(.01f, 0) == 0 && lh_tick(.1f, 1) > 0);
    float *s = lh_state();
    assert(s[2] == 24 && s[12] != s[18]);
    s[0] = 8;
    assert(!lh_receive());
    s[0] = 1;
    assert(lh_receive());
    assert(lh_wire() == s && lh_near(0) == 0);
    lh_start(1, 1, 1, 0);
    lh_draw(0, 0, 0);
    assert(lh_event() == ENTERED);
    camera_matrix(lh_matrix(), (Vec){0, 1.6f, 5}, 0, 0, 1);
    lh_xr_eye(0, 0, 0, 160, 320);
    lh_close();
    char *args[] = {"lantern-hall", "--split", "--rehearsal", "--smoke"};
    assert(lh_native_main(4, args) == 0);
    puts("SDL and OpenGL platform checks passed.");
    return 0;
}
