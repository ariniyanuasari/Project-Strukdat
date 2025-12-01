#include "raylib.h"
#include <vector>
#include <cmath>

struct Ball {
    Vector2 pos;
    Vector2 vel;
    float radius;
    Color color;
};

struct Quadtree {
    Rectangle boundary;
    int capacity;
    std::vector<Ball*> objects;
    bool divided = false;

    Quadtree *nw = nullptr, *ne = nullptr, *sw = nullptr, *se = nullptr;

    Quadtree(Rectangle b, int cap) : boundary(b), capacity(cap) {}

    ~Quadtree() {
        delete nw; delete ne; delete sw; delete se;
    }
    bool insert(Ball *b) {
        if (!CheckCollisionPointRec(b->pos, boundary)) return false;

        if ((int)objects.size() < capacity) {
            objects.push_back(b);
            return true;
        }

        if (!divided) subdivide();

        return nw->insert(b) || ne->insert(b) || sw->insert(b) || se->insert(b);
    }

    void subdivide() {
        float x = boundary.x;
        float y = boundary.y;
        float w = boundary.width / 2.0f;
        float h = boundary.height / 2.0f;

        nw = new Quadtree({x, y, w, h}, capacity);
        ne = new Quadtree({x + w, y, w, h}, capacity);
        sw = new Quadtree({x, y + h, w, h}, capacity);
        se = new Quadtree({x + w, y + h, w, h}, capacity);

        divided = true;
    }

    void query(const Rectangle &range, std::vector<Ball*> &found) {
        if (!CheckCollisionRecs(range, boundary)) return;

        for (auto *b : objects) {
            if (CheckCollisionPointRec(b->pos, range))
                found.push_back(b);
        }

        if (divided) {
            nw->query(range, found);
            ne->query(range, found);
            sw->query(range, found);
            se->query(range, found);
        }
    }
};


void HandleCollision(Ball* A, Ball* B) {
    float dx = B->pos.x - A->pos.x;
    float dy = B->pos.y - A->pos.y;
    float distSq = dx * dx + dy * dy;
    float radiusSum = A->radius + B->radius;

    if (distSq < radiusSum * radiusSum) {
        float dist = sqrtf(distSq);
        if (dist < 0.0001f) dist = 0.0001f;

        float nx = dx / dist;
        float ny = dy / dist;

        float overlap = radiusSum - dist;

        A->pos.x -= nx * (overlap * 0.5f);
        A->pos.y -= ny * (overlap * 0.5f);
        B->pos.x += nx * (overlap * 0.5f);
        B->pos.y += ny * (overlap * 0.5f);

        float dvx = B->vel.x - A->vel.x;
        float dvy = B->vel.y - A->vel.y;
        float impact = dvx * nx + dvy * ny;

        if (impact > 0) return;

        float impulse = impact;

        A->vel.x += impulse * nx;
        A->vel.y += impulse * ny;
        B->vel.x -= impulse * nx;
        B->vel.y -= impulse * ny;
    }
}


int main() {
    InitWindow(800, 600, "Brute Force vs Quadtree Collision");
    SetTargetFPS(150);

    std::vector<Ball> balls;
    int totalBalls = 700;
    bool useQuadtree = true;

    for (int i = 0; i < totalBalls; i++) {
        Ball b;
        b.pos = {(float)GetRandomValue(30, 770), (float)GetRandomValue(30, 570)};
        b.vel = {(float)GetRandomValue(-100, 100) / 60.0f,
                 (float)GetRandomValue(-100, 100) / 60.0f};
        b.radius = 15;
        b.color = {
            (unsigned char)GetRandomValue(100,255),
            (unsigned char)GetRandomValue(100,255),
            (unsigned char)GetRandomValue(100,255),
            255};
        balls.push_back(b);
    }

    while (!WindowShouldClose()) {
//swicth space
        if (IsKeyPressed(KEY_SPACE)) useQuadtree = !useQuadtree;

        for (auto &b : balls) {
            b.pos.x += b.vel.x;
            b.pos.y += b.vel.y;
            if (b.pos.x - b.radius < 0 || b.pos.x + b.radius > 800) b.vel.x *= -1;
            if (b.pos.y - b.radius < 0 || b.pos.y + b.radius > 600) b.vel.y *= -1;
        }

        if (useQuadtree)
//structure quadthree
            {
            Quadtree qt({0,0,800,600}, 6);
            for (auto &b : balls) qt.insert(&b);

            for (auto &b : balls) {   //cek tabrakan//
                Rectangle area{        //grid//
                    b.pos.x - b.radius - 1,
                    b.pos.y - b.radius - 1,
                    b.radius * 2 + 2,
                    b.radius * 2 + 2};

                std::vector<Ball*> found; //ambil bola-bola yang berada di sekitar bola ini aja//
                qt.query(area, found);
// Quadthree
                for (int i = 0; i < found.size(); i++) {
                    for (int j = i + 1; j < found.size(); j++) {
                        if (found[i] != found[j])
                            HandleCollision(found[i], found[j]);
                    }
                }
            }
        }
// bruteforce
        else {
            for (int i = 0; i < totalBalls; i++) {
                for (int j = i + 1; j < totalBalls; j++) {
                    HandleCollision(&balls[i], &balls[j]);
                }
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        for (auto &b : balls)
            DrawCircleV(b.pos, b.radius, b.color);

        DrawText(useQuadtree ? "MODE: QUADTREE (SPACE to switch)"
                             : "MODE: BRUTE FORCE (SPACE to switch)",
                 10, 10, 20, WHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
