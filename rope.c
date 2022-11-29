#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define POINTS 8
#define DISTANCE 2
#define RADIUS 8
#define TENSION 0.7
#define ELASTICITY 1
#define VISIBLE_DOTS 1

#define HIGHLIGHT_ENDS 1

#define GRAVITY -2.4

#define SCREEN_W 800
#define SCREEN_H 600

const int size[] = {SCREEN_W, SCREEN_H};

int holding = -1;

typedef struct point {
    Vector2 pos;
    Vector2 vel;
    int next;
} Point;

Point points[POINTS]; 

int populatePoints() {
    int count = sizeof(points) / sizeof(Point);
    for (int i = 0; i < count; i++) {
        points[i] = (Point){rand() % size[0], rand() % size[1], (i+1) % count};
    }
}

float magnitude(Vector2 a) {
    return sqrt(a.x*a.x + a.y*a.y);
}

int distanceSquared(Vector2 a, Vector2 b) {
    int dx = (a.x - b.x);
    int dy = (a.y - b.y);
    return dx*dx + dy*dy;
}

Vector2 add(Vector2 a, Vector2 b) {
    return (Vector2){a.x + b.x, a.y + b.y};
}

Vector2 mul(Vector2 a, float v) {
    return (Vector2){a.x * v, a.y * v};
}

Vector2 normalize(Vector2 a) {
    float m = magnitude(a);
    return (Vector2){a.x / m, a.y / m};
}

int clampInPlace(Point *p) {
    if (p->pos.x < 0) {
        p->vel.x = 0;
        p->pos.x = 0;
    } else if (p->pos.x > SCREEN_W) {
        p->vel.x = 0;
        p->pos.x = SCREEN_W;
    }


    if (p->pos.y < 0) {
        p->vel.y = 0;
        p->pos.y = 0;
    } else if (p->pos.y > SCREEN_H) {
        p->vel.y = 0;
        p->pos.y = SCREEN_H;
    }
}

int physics(Point *p, Point *n) {
    Vector2 d = (Vector2){(n->pos.x - p->pos.x), (n->pos.y - p->pos.y)};
    Vector2 f = mul(normalize(d), TENSION * (magnitude(d) - DISTANCE));

    p->vel.x += f.x * ELASTICITY;
    p->vel.y += f.y * ELASTICITY;

}

int draw(void) {
    Vector2 mouse = (Vector2){GetMouseX(), GetMouseY()};

    BeginDrawing();
    ClearBackground(LIGHTGRAY);

    int count = sizeof(points) / sizeof(Point);


    for (int i = 0; i < count; i++) {
        Point *p = &points[i]; 
        p->vel.x = 0;
        p->vel.y = 0;

        if (i+1 < count) {
            Point *n = &points[(i+1)];
            physics(p, n);
        }

        if (i-1 > -1) {
            Point *m = &points[(i-1)];
            physics(p, m);
        }

        p->vel.y -= GRAVITY;
        clampInPlace(p);
        p->pos.x += p->vel.x;
        p->pos.y += p->vel.y;

        if (IsMouseButtonDown(0)) {
            if (distanceSquared(p->pos, mouse) < RADIUS*RADIUS) {
                holding = i;
                p->vel.x = 0;
                p->vel.y = 0;
            }
        } else {
            holding = -1;
        }
    }
    if (holding > -1) {
        Point *p = &points[holding];
        p->pos = mouse;
    }

    for (int i = 0; i < count; i++) {
        Point *p = &points[i]; 
        if (i+1 < count) {
            Point *n = &points[(i+1)];
            DrawLineEx(p->pos, n->pos, RADIUS, DARKGRAY);
        }
#if VISIBLE_DOTS
        Color color;
#if HIGHLIGHT_ENDS
        if (i == 0 || i == POINTS - 1) {
            color = RED;
        } else {
            color = GRAY;
        }
#else
        color = DARKGRAY;
#endif
        DrawCircle(p->pos.x, p->pos.y, RADIUS, color);
#endif
    }


    EndDrawing();
}

int main(void) {
    InitWindow(size[0], size[1], "Rope");

    SetTargetFPS(60);

    populatePoints();
    while (!WindowShouldClose()) {
        draw();
    }

    CloseWindow();

    return 0;
}
