#include "raylib.h"
#include "src/screen.h"
#include "src/MenuScreen.h"
#include "src/NumberPicker.h"
#include "src/Wheel.h"
#include "src/Resources.h"

int main() {
    InitWindow(800, 600, "RandomPicker");
    SetTargetFPS(60);

    Resources res{};
    res.font = LoadFont("fonts/coolvetica_regular.otf");
    if (res.font.texture.id == 0)
        TraceLog(LOG_WARNING, "Font failed to load!");

    Screen current = Screen::MENU;
    MenuScreen menu;
    NumberPicker numberPicker;
    Wheel wheel;
    SetExitKey(KEY_NULL);

    while (!WindowShouldClose()) {
        // Update
        switch (current) {
            case Screen::MENU:
                menu.Update(current);
                break;
            case Screen::NUMBER:
                numberPicker.Update(current);
                break;
            case Screen::WHEEL:
                wheel.Update(current);
                break;
            case Screen::COIN:
                break;
            case Screen::DICE:
                break;
            case Screen::SLOTS:
                break;
        }

        // Draw
        BeginDrawing();
        ClearBackground({20, 20, 28, 255});

        switch (current) {
            case Screen::MENU:
                menu.Draw(res);
                break;
            case Screen::NUMBER:
                numberPicker.Draw(res);
                break;
            case Screen::COIN:
                break;
            case Screen::DICE:
                break;
            case Screen::SLOTS:
                break;
            case Screen::WHEEL:
                wheel.Draw(res);
                break;
        }

        EndDrawing();
    }

    UnloadFont(res.font);
    CloseWindow();
    return 0;
}