/* LF2-style mini fighter for ShaderEmu / Nano-X. Original simple artwork. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MWINCLUDECOLORS
#include <nano-X.h>

#define W 600
#define H 350
#define C(r,g,b) MWRGB(r,g,b)

typedef struct {
    int x, z, dir, hp, jump, vy, cool, act, stun;
} Fighter;
typedef struct { int x, z, dir, owner, life; } Shot;
static Fighter f[2];
static Shot shot[4];
static GR_WINDOW_ID win;
static GR_GC_ID gc;
static int keys, tick, spark_x, spark_y, spark_t;

static int ab(int a) { return a < 0 ? -a : a; }
static int clamp(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }
static void box(int x, int y, int w, int h, GR_COLOR c) {
    GrSetGCForeground(gc, c); GrFillRect(win, gc, x, y, w, h);
}
static void disk(int x, int y, int rx, int ry, GR_COLOR c) {
    GrSetGCForeground(gc, c); GrFillEllipse(win, gc, x, y, rx, ry);
}
static void line(int x, int y, int xx, int yy, GR_COLOR c) {
    GrSetGCForeground(gc, c); GrLine(win, gc, x, y, xx, yy);
}
static void label(int x, int y, char *s, GR_COLOR c) {
    GrSetGCForeground(gc, c);
    GrText(win, gc, x, y, s, (int)strlen(s), GR_TFASCII | GR_TFTOP);
}
static void reset(void) {
    memset(f, 0, sizeof(f)); memset(shot, 0, sizeof(shot));
    f[0].x = 145; f[0].dir = 1; f[0].hp = 100;
    f[1].x = 445; f[1].dir = -1; f[1].hp = 100;
    tick = keys = spark_t = 0;
}
static void hurt(int who, int damage, int push) {
    Fighter *p = &f[who];
    if (p->hp <= 0 || p->stun > 0) return;
    p->hp = clamp(p->hp - damage, 0, 100);
    p->stun = 9;
    p->x = clamp(p->x + push * 10, 20, W - 20);
    spark_x = p->x; spark_y = 199 + p->z - p->jump; spark_t = 6;
}
/* move=1 punch, 2 kick, 3 projectile */
static void attack(int who, int move) {
    Fighter *p = &f[who];
    Fighter *q = &f[1-who];
    int d, range, damage, i;
    if (p->hp <= 0 || q->hp <= 0 || p->cool || p->stun) return;
    p->act = move; p->cool = move == 3 ? 27 : move == 2 ? 20 : 13;
    if (move == 3) {
        for (i = 0; i < 4; i++) if (!shot[i].life) {
            shot[i].x = p->x + p->dir * 26;
            shot[i].z = p->z; shot[i].dir = p->dir;
            shot[i].owner = who; shot[i].life = 56; break;
        }
        return;
    }
    d = (q->x - p->x) * p->dir;
    range = move == 2 ? 76 : 55;
    damage = move == 2 ? 15 : 9;
    if (d > 0 && d < range && ab(q->z - p->z) < 23 && q->jump < 34)
        hurt(1 - who, damage, p->dir);
}
static void update(void) {
    Fighter *p = &f[0], *e = &f[1];
    int i;
    tick++;
    for (i = 0; i < 2; i++) {
        Fighter *a = &f[i];
        if (a->cool) --a->cool;
        if (a->stun) --a->stun;
        if (a->jump || a->vy) {
            a->jump += a->vy;
            a->vy -= 2;
            if (a->jump <= 0) a->jump = a->vy = 0;
        }
        if (!a->cool) a->act = 0;
    }
    if (p->hp && e->hp) {
        if (!p->stun) {
            if (keys & 1) { p->x -= 4; p->dir = -1; }
            if (keys & 2) { p->x += 4; p->dir = 1; }
            if (keys & 4) p->z -= 2;
            if (keys & 8) p->z += 2;
            p->x = clamp(p->x, 24, W - 24);
            p->z = clamp(p->z, -35, 35);
        }
        e->dir = p->x >= e->x ? 1 : -1;
        if (!e->stun) {
            if (ab(e->x - p->x) > 61) e->x += e->dir * 2;
            if (ab(e->z - p->z) > 13) e->z += e->z < p->z ? 1 : -1;
            if (ab(e->x-p->x) < 75 && ab(e->z-p->z) < 22 && !e->cool)
                attack(1, tick % 3 ? 1 : 2);
        }
        e->x = clamp(e->x, 24, W-24);
    }
    for (i = 0; i < 4; i++) if (shot[i].life) {
        Shot *s = &shot[i]; Fighter *t = &f[1-s->owner];
        s->x += s->dir * 9; --s->life;
        if (s->x < 6 || s->x > W-6) s->life = 0;
        if (s->life && t->hp && ab(s->x-t->x) < 24 &&
            ab(s->z-t->z) < 20 && t->jump < 38) {
            hurt(1-s->owner, 12, s->dir); s->life = 0;
        }
    }
    if (spark_t) --spark_t;
}
static void draw_fighter(Fighter *a, int cpu) {
    int x = a->x, y = 267 + a->z - a->jump;
    GR_COLOR shirt = cpu ? C(208,72,74) : C(52,114,222);
    GR_COLOR trim = cpu ? C(132,34,41) : C(31,54,116);
    if (!a->hp) shirt = C(95,95,100);
    /* ground shadow and feet */
    disk(x, 269 + a->z, 22, 5, C(76,91,83));
    line(x-8, y-24, x-13, y, C(26,32,50));
    line(x+8, y-24, x+13, y, C(26,32,50));
    box(x-19, y-2, 14, 5, C(33,34,49));
    box(x+7, y-2, 14, 5, C(33,34,49));
    box(x-14, y-48, 28, 29, shirt);
    box(x-14, y-23, 28, 7, trim);
    line(x-14, y-44, x-a->dir*11, y-30, C(244,192,146));
    if (a->act == 1) {
        line(x+14*a->dir, y-41, x+50*a->dir, y-40, C(244,192,146));
        disk(x+52*a->dir, y-40, 7, 7, C(255,215,166));
    } else if (a->act == 2) {
        line(x+6*a->dir, y-21, x+56*a->dir, y-20, C(35,40,55));
        box(x+56*a->dir-6, y-24, 13, 8, C(245,241,221));
    } else {
        line(x+14*a->dir, y-42, x+21*a->dir, y-28, C(244,192,146));
    }
    disk(x, y-59, 12, 13, C(244,193,150));
    box(x-12, y-72, 24, 7, cpu ? C(58,36,34) : C(46,49,75));
    box(x+(a->dir>0?4:-7), y-60, 4, 3, C(28,28,35));
    if (a->stun) {
        line(x-14,y-78,x+14,y-82,C(255,237,109));
    }
}
static void draw(void) {
    int i;
    char msg[40];
    box(0,0,W,H,C(172,215,233));
    /* Rooftops, distant city and arena floor */
    for (i=0;i<6;i++) {
        box(i*110-18,115+(i%3)*13,96,100,C(123,159,166));
        box(i*110-11,126+(i%3)*13,10,15,C(208,227,223));
    }
    box(0,205,W,145,C(196,185,143));
    box(0,211,W,3,C(245,222,161));
    for (i=0;i<9;i++) line(i*76,213,i*76-50,H,C(158,146,115));
    for (i=0;i<5;i++) line(0,227+i*30,W,227+i*30,C(172,161,129));
    draw_fighter(&f[0],0);
    draw_fighter(&f[1],1);
    for (i=0;i<4;i++) if (shot[i].life) {
        Shot *s = &shot[i];
        int y = 225+s->z;
        disk(s->x,y,16,12,C(45,128,229));
        disk(s->x,y,9,9,C(152,238,255));
        line(s->x-s->dir*26,y,s->x-s->dir*15,y,C(72,186,251));
    }
    if (spark_t) {
        disk(spark_x,spark_y,17,17,C(255,222,86));
        disk(spark_x,spark_y,7,7,C(255,255,227));
    }
    box(0,0,W,84,C(31,40,65));
    label(17,8,"BLUE - YOU",C(231,243,252));
    label(431,8,"RED - CPU",C(252,231,231));
    box(18,27,201,22,C(12,18,30));
    box(381,27,201,22,C(12,18,30));
    box(20,29,f[0].hp*197/100,18,C(67,197,104));
    box(383,29,f[1].hp*197/100,18,C(224,82,76));
    label(20,59,"A/D move  W/S lane  SPACE jump",C(231,234,235));
    label(20,72,"J punch   K kick   L fireball    R restart",C(231,234,235));
    if (!f[0].hp || !f[1].hp) {
        box(174,136,255,58,C(34,37,58));
        strcpy(msg,f[0].hp ? "YOU WIN!  PRESS R" : "YOU LOSE!  PRESS R");
        label(211,155,msg,C(251,234,143));
    }
}
static void key(GR_KEY ch, int down) {
    int bit = 0;
    switch (ch) {
    case 'a': case 'A': case MWKEY_LEFT: bit=1; break;
    case 'd': case 'D': case MWKEY_RIGHT: bit=2; break;
    case 'w': case 'W': case MWKEY_UP: bit=4; break;
    case 's': case 'S': case MWKEY_DOWN: bit=8; break;
    }
    if (bit) { if (down) keys |= bit; else keys &= ~bit; }
    if (!down) return;
    if (ch=='r'||ch=='R') reset();
    if (f[0].hp==0 || f[1].hp==0) return;
    if (ch==' ' && f[0].jump==0 && f[0].vy==0 && !f[0].stun) f[0].vy=13;
    if (ch=='j'||ch=='J') attack(0,1);
    if (ch=='k'||ch=='K') attack(0,2);
    if (ch=='l'||ch=='L') attack(0,3);
}
int main(void) {
    GR_EVENT e;
    int running=1;
    if (GrOpen()<0) return 1;
    win=GrNewWindowEx(GR_WM_PROPS_APPWINDOW,"Mini Fighter - ShaderEmu",
        GR_ROOT_WINDOW_ID, -1,-1,W,H,C(0,0,0));
    gc=GrNewGC();
    GrSetGCUseBackground(gc,0);
    GrSelectEvents(win, GR_EVENT_MASK_EXPOSURE | GR_EVENT_MASK_CLOSE_REQ |
        GR_EVENT_MASK_KEY_DOWN | GR_EVENT_MASK_KEY_UP | GR_EVENT_MASK_FOCUS_OUT);
    GrMapWindow(win); GrSetFocus(win);
    reset(); draw();
    while (running) {
        GrGetNextEventTimeout(&e,33);
        if (e.type==GR_EVENT_TYPE_CLOSE_REQ) running=0;
        if (e.type==GR_EVENT_TYPE_FOCUS_OUT) keys=0;
        if (e.type==GR_EVENT_TYPE_KEY_DOWN) {
            if (e.keystroke.ch==MWKEY_ESCAPE) running=0;
            else key(e.keystroke.ch,1);
        }
        if (e.type==GR_EVENT_TYPE_KEY_UP) key(e.keystroke.ch,0);
        if (!running) break;
        update(); draw();
    }
    GrDestroyGC(gc); GrClose(); return 0;
}
