// coinflip_3d.cpp — Raylib boilerplate voor een 3D munt
//
// Compileren:
//   g++ coinflip_3d.cpp -o coinflip -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
//   (Windows: vervang de -l flags door de Raylib .lib en .dll)
//
// Dependencies: raylib >= 4.5  (https://www.raylib.com/)

#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>

// ─── Constanten ─────────────────────────────────────────────────────────────

static const int   SCREEN_W    = 800;
static const int   SCREEN_H    = 600;
static const float COIN_RADIUS = 1.0f;
static const float COIN_THICK  = 0.12f;   // dikte als verhouding van radius
static const int   COIN_SLICES = 64;      // hoe ronder de cirkel
static const float FLIP_SPINS  = 5.0f;    // aantal volledige rotaties per gooi
static const float FLIP_DUR    = 1.8f;    // seconden voor een gooi

// ─── Animatiestatus ──────────────────────────────────────────────────────────

enum class FlipState { IDLE, FLIPPING, DONE };

struct CoinFlip {
    FlipState state     = FlipState::IDLE;
    float     timer     = 0.0f;          // 0..FLIP_DUR tijdens animatie
    float     startAngleY = 0.0f;        // beginhoek voor deze gooi
    float     endAngleY   = 0.0f;        // eindhoek (bepaalt kop/munt)
    float     currentAngleY = 0.0f;      // huidige Y-rotatie in graden
    float     wobbleX     = 0.0f;        // lichte X-kanteling voor 3D-gevoel
    bool      isHeads     = false;       // resultaat van deze gooi
    int       headsCount  = 0;
    int       tailsCount  = 0;
};

// ─── Easing ──────────────────────────────────────────────────────────────────

// Cubic ease-out: snel starten, langzaam stoppen
static float EaseOut(float t) {
    float f = 1.0f - t;
    return 1.0f - f * f * f;
}

// ─── Face-detectie ───────────────────────────────────────────────────────────
// Geeft true als de "kop"-kant naar de camera wijst.
// We kijken welke normaal het meest naar de camera-richting wijst.
// De kop-normaal is +Z in lokale ruimte; na rotatie over Y wordt dat:
//   worldNormal = (sin(angleY), 0, cos(angleY))
// Camera staat op +Z, dus dot met (0,0,1) = cos(angleY).
// Kop zichtbaar als cos(angleY) > 0.

static bool IsFaceVisible(float angleYDeg) {
    float rad = angleYDeg * DEG2RAD;
    return cosf(rad) > 0.0f;
}

// ─── Gooi starten ────────────────────────────────────────────────────────────

static void StartFlip(CoinFlip& c) {
    if (c.state == FlipState::FLIPPING) return;

    c.isHeads    = (rand() % 2) == 0;
    c.state      = FlipState::FLIPPING;
    c.timer      = 0.0f;
    c.startAngleY = c.currentAngleY;

    // Eindhoek: kop = 0°, munt = 180° (+ meerdere volle slagen)
    float faceAngle = c.isHeads ? 0.0f : 180.0f;
    c.endAngleY = c.startAngleY + FLIP_SPINS * 360.0f + faceAngle;
}

// ─── Update ──────────────────────────────────────────────────────────────────

static void UpdateCoin(CoinFlip& c, float dt) {
    if (c.state != FlipState::FLIPPING) return;

    c.timer += dt;
    float progress = c.timer / FLIP_DUR;

    if (progress >= 1.0f) {
        progress = 1.0f;
        c.currentAngleY = fmodf(c.endAngleY, 360.0f);
        c.wobbleX       = 0.0f;
        c.state         = FlipState::DONE;

        if (c.isHeads) c.headsCount++;
        else           c.tailsCount++;
        return;
    }

    float eased     = EaseOut(progress);
    c.currentAngleY = c.startAngleY + (c.endAngleY - c.startAngleY) * eased;

    // Boogvormige X-kanteling (piekwaarde halverwege de vlucht)
    c.wobbleX = sinf(progress * PI) * 25.0f;
}

// ─── Tekenen ─────────────────────────────────────────────────────────────────

static void DrawCoin(const CoinFlip& c) {
    // Raylib heeft geen ingebouwde cilinder met twee aparte vlakken,
    // dus we tekenen: lichaam + twee gekleurde schijven voor kop/munt.

    rlPushMatrix();
        rlRotatef(c.wobbleX,      1, 0, 0);   // lichte kanteling
        rlRotatef(c.currentAngleY, 0, 1, 0);   // spin om Y-as

        // Muntzijkant (rand)
        DrawCylinder(
            {0, 0, -COIN_THICK},            // onderkant
            COIN_RADIUS,                     // straal onder
            COIN_RADIUS,                     // straal boven
            COIN_THICK * 2.0f,               // hoogte
            COIN_SLICES,
            DARKGRAY
        );

        // Kop-kant (+Z vlak) — goud
        DrawCircle3D(
            {0, 0, COIN_THICK},
            COIN_RADIUS,
            {1, 0, 0}, 0,
            GOLD
        );

        // Munt-kant (−Z vlak) — zilver
        DrawCircle3D(
            {0, 0, -COIN_THICK},
            COIN_RADIUS,
            {1, 0, 0}, 0,
            LIGHTGRAY
        );

    rlPopMatrix();
}

// ─── HUD ─────────────────────────────────────────────────────────────────────

static void DrawHUD(const CoinFlip& c) {
    // Status tekst
    const char* statusText = "";
    Color statusColor = WHITE;

    switch (c.state) {
        case FlipState::IDLE:
            statusText  = "Druk SPATIE om te gooien";
            statusColor = LIGHTGRAY;
            break;
        case FlipState::FLIPPING:
            statusText  = "...";
            statusColor = YELLOW;
            break;
        case FlipState::DONE:
            statusText  = c.isHeads ? "KOPT!" : "MUNT!";
            statusColor = c.isHeads ? GOLD : SKYBLUE;
            break;
    }

    int sw = MeasureText(statusText, 28);
    DrawText(statusText, SCREEN_W/2 - sw/2, SCREEN_H - 80, 28, statusColor);

    // Tellers
    std::string stats = "Koppen: " + std::to_string(c.headsCount) +
                        "   Munten: " + std::to_string(c.tailsCount);
    int dw = MeasureText(stats.c_str(), 18);
    DrawText(stats.c_str(), SCREEN_W/2 - dw/2, SCREEN_H - 40, 18, GRAY);
}

// ─── Main ─────────────────────────────────────────────────────────────────────

int main() {
    srand((unsigned)time(nullptr));

    InitWindow(SCREEN_W, SCREEN_H, "3D Coinflip — Raylib boilerplate");
    SetTargetFPS(60);

    // Camera opstelling
    Camera3D camera = {};
    camera.position   = {0.0f, 2.5f, 6.0f};
    camera.target     = {0.0f, 0.0f, 0.0f};
    camera.up         = {0.0f, 1.0f, 0.0f};
    camera.fovy       = 40.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    CoinFlip coin;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // Input
        if (IsKeyPressed(KEY_SPACE) && coin.state != FlipState::FLIPPING) {
            coin.state = FlipState::IDLE;  // reset DONE zodat we opnieuw kunnen
            StartFlip(coin);
        }

        UpdateCoin(coin, dt);

        // ── Renderen ──────────────────────────────────────────────────────────
        BeginDrawing();
        ClearBackground({20, 20, 28, 255});   // donkere achtergrond

        BeginMode3D(camera);
            DrawGrid(10, 1.0f);
            DrawCoin(coin);
        EndMode3D();

        DrawHUD(coin);

        // Debug: toon huidige Y-hoek en face
        DrawText(
            TextFormat("Y: %.1f  face: %s",
                coin.currentAngleY,
                IsFaceVisible(coin.currentAngleY) ? "kop" : "munt"),
            10, 10, 16, DARKGRAY
        );

        EndDrawing();
    }

    CloseWindow();
    return 0;
}