#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

const int radius = 8;
const int size[] = {800, 450};
const float t = 0.2;
const int distance = 64;

int holding = -1;

typedef struct point {
    Vector2 pos;
    Vector2 vel;
    int next;
} Point;

Point points[64]; 

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

int physics(Point *p, Point *n) {
    Vector2 d = (Vector2){(n->pos.x - p->pos.x), (n->pos.y - p->pos.y)};
    Vector2 f = mul(normalize(d), t * (magnitude(d) - distance));

    p->vel.x += f.x;
    p->vel.y += f.y;

}

int draw(void) {
    Vector2 mouse = (Vector2){GetMouseX(), GetMouseY()};

    BeginDrawing();
    ClearBackground(LIGHTGRAY);

    int count = sizeof(points) / sizeof(Point);


    for (int i = 0; i < count; i++) {
        Point *p = &points[i]; 
        Point *n = &points[(i+1)% count];
        Point *m = &points[(i-1)% count];

        p->vel.x = 0;
        p->vel.y = 0;

        physics(p, n);
        physics(p, m);

        p->pos.x += p->vel.x;
        p->pos.y += p->vel.y;

        if (IsMouseButtonDown(0)) {
            if (distanceSquared(p->pos, mouse) < radius*radius) {
                holding = i;
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
        Point *n = &points[(i+1)% count];
        DrawCircle(p->pos.x, p->pos.y, radius, DARKGRAY);
        DrawLineEx(p->pos, n->pos, radius / 2, DARKGRAY);
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
