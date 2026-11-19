#include "draw.h"
#include "campaign.h"
#include <SDL3/SDL.h>
#include <stddef.h>
#ifdef __EMSCRIPTEN__
#include <GLES2/gl2.h>
#define G(name) gl##name
#define LOAD_GL() 1
#define VERSION_V "#version 100\n"
#define VERSION_F "#version 100\nprecision highp float;\n"
#else
#include <SDL3/SDL_opengl.h>
#define GL_LIST(F)                                                                                 \
    F(GLuint, CreateShader, (GLenum))                                                              \
    F(void, ShaderSource, (GLuint, GLsizei, const char *const *, const GLint *)) F(                \
        void, CompileShader, (GLuint)) F(void, GetShaderiv, (GLuint, GLenum, GLint *))             \
        F(void, GetShaderInfoLog, (GLuint, GLsizei, GLsizei *, char *)) F(                         \
            void, DeleteShader, (GLuint)) F(GLuint, CreateProgram, (void))                         \
            F(void, AttachShader, (GLuint, GLuint)) F(void, LinkProgram, (GLuint)) F(              \
                void, GetProgramiv, (GLuint, GLenum, GLint *)) F(void, UseProgram, (GLuint))       \
                F(void, DeleteProgram, (GLuint)) F(void, GenBuffers, (GLsizei, GLuint *)) F(       \
                    void, DeleteBuffers,                                                           \
                    (GLsizei, const GLuint *)) F(void, BindBuffer, (GLenum, GLuint))               \
                    F(void, BufferData, (GLenum, ptrdiff_t, const void *, GLenum))                 \
                        F(GLint, GetAttribLocation, (GLuint, const char *)) F(                     \
                            GLint, GetUniformLocation,                                             \
                            (GLuint, const char *)) F(void, EnableVertexAttribArray, (GLuint))     \
                            F(void, VertexAttribPointer,                                           \
                              (GLuint, GLint, GLenum, GLboolean, GLsizei, const void *))           \
                                F(void, UniformMatrix4fv,                                          \
                                  (GLint, GLsizei, GLboolean, const GLfloat *))                    \
                                    F(void, Uniform1f, (GLint, GLfloat))                           \
                                        F(void, Uniform3f, (GLint, GLfloat, GLfloat, GLfloat))     \
                                            F(void, Viewport, (GLint, GLint, GLsizei, GLsizei))    \
                                                F(void, Scissor, (GLint, GLint, GLsizei, GLsizei)) \
                                                    F(void, Enable, (GLenum))                      \
                                                        F(void, ClearColor,                        \
                                                          (GLfloat, GLfloat, GLfloat, GLfloat))    \
                                                            F(void, Clear, (GLbitfield))           \
                                                                F(void, DrawArrays,                \
                                                                  (GLenum, GLint, GLsizei))
#define DECLARE(ret, name, args) static ret(APIENTRY *p##name) args;
GL_LIST(DECLARE)
#define RESOLVE(ret, name, args)                                                                   \
    p##name = (ret(APIENTRY *) args)SDL_GL_GetProcAddress("gl" #name);                             \
    if (!p##name)                                                                                  \
        return 0;
static int load_gl(void)
{
    GL_LIST(RESOLVE) return 1;
}
#define LOAD_GL() load_gl()
#define G(name) p##name
#define VERSION_V "#version 120\n"
#define VERSION_F "#version 120\n"
#endif

static const char *vert =
    VERSION_V "attribute vec3 pos,norm; attribute vec4 colour; uniform mat4 camera;"
              "varying vec3 p,n; varying vec4 c;"
              "void main(){p=pos;n=norm;c=colour;gl_Position=camera*vec4(pos,1.0);}";

static const char *frag = VERSION_F
    "varying vec3 p,n; varying vec4 c; uniform float clock,room,flash; uniform vec3 hand,mate,beacon,watcher;"
    "float hash(vec2 a){return fract(sin(dot(a,vec2(12.9898,78.233)))*4375.5453);}"
    "float shade(vec2 point,vec2 caster,float height,float rad,vec3 light){"
    " vec2 a=caster-light.xz; vec2 end=caster+a*height/max(1.0,light.y-height);"
    " vec2 d=end-caster; float t=clamp(dot(point-caster,d)/max(.001,dot(d,d)),0.0,1.0);"
    " return 1.0-smoothstep(rad*.68,rad*1.18,length(point-caster-d*t));}"
    "void main(){vec3 base=c.rgb; float mat=c.a; vec3 normal=normalize(n+vec3(.00001));"
    " bool floorMat=mat>.29&&mat<.40; bool stone=mat>.46&&mat<.62;"
    " if(floorMat){vec2 tile=p.xz/1.32;tile.x+=mod(floor(tile.y),2.)*.5;"
    " vec2 seam=min(fract(tile),1.-fract(tile));base*=.89+.14*hash(floor(tile));"
    " base*=mix(.72,1.,smoothstep(.006,.030,min(seam.x,seam.y)));}"
    " if(stone){vec2 tile=vec2(abs(n.x)>.65?p.z:p.x,p.y)/vec2(1.6,.76);"
    " tile.x+=mod(floor(tile.y),2.)*.5;vec2 seam=min(fract(tile),1.-fract(tile));"
    " base*=.92+.09*hash(floor(tile));base*=mix(.78,1.,smoothstep(.004,.023,min(seam.x,seam.y)));}"
    " if(mat>.12&&mat<.23)base*=.96+.04*sin((p.x+p.z*.32+p.y*.15)*36.);"
    " float face=.74+.19*abs(normal.y)+max(0.,dot(normal,normalize(vec3(-.55,.85,.30))))*.18;"
    " vec3 lit=base*vec3(.85,.91,.92)*face;"
    " vec3 lamp=vec3(1.8*sin(clock*.41),4.9,-17.0+cos(clock*.32));"
    " float local=1./(1.+.047*dot(lamp-p,lamp-p));"
    " float aisle=exp(-.13*abs(abs(p.x)-5.0))*(.55+.45*cos((p.z-12.65)*.805));"
    " float bay=mod(p.z-1.0,7.8)-3.9;vec3 toSconce=vec3(abs(p.x)-7.0,p.y-3.65,bay);"
    " float pool=1./(1.+.38*dot(toSconce,toSconce));"
    " lit+=base*vec3(.54,.26,.065)*(local+.4*aisle)+vec3(.17,.086,.022)*pool;"
    " float torch=(.13+flash*3.4)/(1.+.27*dot(hand-p,hand-p));lit+=vec3(1.,.65,.27)*torch;"
    " if(room>1.5){vec3 d=beacon-p;lit+=vec3(.36,.22,.085)*(1.+.16*sin(clock*2.5))/(1.+.09*dot(d,d));}"
    " if(p.y<.08&&normal.y>.55){float s=shade(p.xz,mate.xz,1.4,.30,hand+vec3(.35,1.6,.25));"
    " if(room<.5)s=max(s,shade(p.xz,vec2(0.,-20.5),2.6,1.75,lamp));"
    " if(room>.5&&room<1.5){s=max(s,shade(p.xz,vec2(-2.2,.2),1.2,.48,vec3(-4.,4.5,2.2)));"
    " s=max(s,shade(p.xz,vec2(2.4,-4.4),1.2,.48,vec3(4.,4.5,-2.)));}"
    " if(room>1.5)s=max(s,shade(p.xz,watcher.xz,watcher.y,.41,hand+vec3(.4,2.1,.6)));"
    " lit*=1.-.38*s;}"
    " if(mat>.9)lit=base*(1.04+.035*sin(clock*2.3+p.z));"
    " float grain=(hash(floor(gl_FragCoord.xy))-.5)*.017;"
    " gl_FragColor=vec4(clamp(lit+grain,0.,1.),1.);}";

static Vertex architecture[150000], actors[4000];
static Mesh solid = {architecture, 0, 150000}, moving = {actors, 0, 4000};
static GLuint prog, buffers[2];
static GLint position, normal, colour, camera, clock_id, room_id, flash_id, hand_id, mate_id;
static GLint beacon_id,watcher_id;
static int prev_day=-1,prev_area=-1;
static int prev_room = -1, prev_open = -1, prev_done = -1, prev_projected = -1;

static GLuint shader(GLenum kind, const char *source)
{
    GLuint id = G(CreateShader)(kind);
    G(ShaderSource)(id, 1, &source, NULL);
    G(CompileShader)(id);
    GLint ok;
    G(GetShaderiv)(id, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char message[1024] = {0};
        G(GetShaderInfoLog)(id, sizeof(message), NULL, message);
        SDL_Log("Shader compile: %s", message);
        G(DeleteShader)(id);
        return 0;
    }
    return id;
}

int draw_init(void)
{
    if (!LOAD_GL())
        return 0;
    GLuint v = shader(GL_VERTEX_SHADER, vert), f = shader(GL_FRAGMENT_SHADER, frag);
    if (!v || !f)
        return 0;
    prog = G(CreateProgram)();
    G(AttachShader)(prog, v);
    G(AttachShader)(prog, f);
    G(LinkProgram)(prog);
    G(DeleteShader)(v);
    G(DeleteShader)(f);
    GLint ok;
    G(GetProgramiv)(prog, GL_LINK_STATUS, &ok);
    if (!ok)
        return 0;
    position = G(GetAttribLocation)(prog, "pos");
    normal = G(GetAttribLocation)(prog, "norm");
    colour = G(GetAttribLocation)(prog, "colour");
    camera = G(GetUniformLocation)(prog, "camera");
    clock_id = G(GetUniformLocation)(prog, "clock");
    room_id = G(GetUniformLocation)(prog, "room");
    flash_id = G(GetUniformLocation)(prog, "flash");
    hand_id = G(GetUniformLocation)(prog, "hand");
    mate_id = G(GetUniformLocation)(prog, "mate");
    beacon_id=G(GetUniformLocation)(prog,"beacon");watcher_id=G(GetUniformLocation)(prog,"watcher");
    G(GenBuffers)(2, buffers);
    prev_room = -1;
    return 1;
}

static void batch(GLuint id, const Mesh *m)
{
    G(BindBuffer)(GL_ARRAY_BUFFER, id);
    G(VertexAttribPointer)(position, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                           (void *)offsetof(Vertex, pos));
    G(VertexAttribPointer)(normal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                           (void *)offsetof(Vertex, normal));
    G(VertexAttribPointer)(colour, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex),
                           (void *)offsetof(Vertex, colour));
    G(DrawArrays)(GL_TRIANGLES, 0, m->count);
}

void draw_view(const Game *g, int viewer, float time, const float matrix[16], int x, int y, int w,
               int h, int held_tool)
{
    if (g->room != prev_room || g->open != prev_open || g->completed != prev_done ||
        g->projected != prev_projected || (g->room==EXPEDITION && (g->area!=prev_area || g->door!=prev_day))) {
        scene_static(&solid, g);
        G(BindBuffer)(GL_ARRAY_BUFFER, buffers[0]);
        G(BufferData)(GL_ARRAY_BUFFER, solid.count * sizeof(Vertex), solid.v, GL_STATIC_DRAW);
        prev_room = g->room; prev_area=g->area;prev_day=g->door;
        prev_open = g->open;
        prev_done = g->completed;
        prev_projected = g->projected;
    }
    scene_actors(&moving, g, viewer, time, held_tool);
    G(BindBuffer)(GL_ARRAY_BUFFER, buffers[1]);
    G(BufferData)(GL_ARRAY_BUFFER, moving.count * sizeof(Vertex), moving.v, GL_STREAM_DRAW);
    G(UseProgram)(prog);
    G(Enable)(GL_DEPTH_TEST);
    G(Enable)(GL_SCISSOR_TEST);
    G(Viewport)(x, y, w, h);
    G(Scissor)(x, y, w, h);
    if(g->room==EXPEDITION){
        unsigned sky=expeditions[g->door-1].colour;
        G(ClearColor)(.065f+(sky>>16)*.0012f,.09f+((sky>>8)&255)*.0012f,.15f+(sky&255)*.0015f,1);
    }else G(ClearColor)(.12f, .19f, .187f, 1);
    G(Clear)(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    G(EnableVertexAttribArray)(position);
    G(EnableVertexAttribArray)(normal);
    G(EnableVertexAttribArray)(colour);
    G(UniformMatrix4fv)(camera, 1, GL_FALSE, matrix);
    G(Uniform1f)(clock_id, time);
    G(Uniform1f)(room_id, (float)g->room);
    const Player *p = &g->player[viewer], *other = &g->player[1 - viewer];
    G(Uniform1f)(flash_id, g->flash[viewer]);
    G(Uniform3f)(hand_id, p->x, p->y + 1.15f, p->z);
    G(Uniform3f)(mate_id, g->seats == 2 ? other->x : 60, other->y, other->z);
    if(g->room==EXPEDITION){
        float x,z;campaign_target(g,&x,&z);G(Uniform3f)(beacon_id,x,3.1f,z);
        campaign_guard(g,&x,&z);G(Uniform3f)(watcher_id,x,g->stun>0?.45f:1.5f,z);
    }
    batch(buffers[0], &solid);
    batch(buffers[1], &moving);
}

void draw_close(void)
{
    G(DeleteBuffers)(2, buffers);
    G(DeleteProgram)(prog);
}
