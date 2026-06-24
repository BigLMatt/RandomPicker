#include "raylib.h"
#include "src/screen.h"
#include "src/MenuScreen.h"
#include "src/NumberPicker.h"
#include "src/Wheel.h"

int main() {
    InitWindow(800, 600, "RandomPicker");
    SetTargetFPS(60);

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
                menu.Draw();
                break;
            case Screen::NUMBER:
                numberPicker.Draw();
                break;
            case Screen::COIN:
                break;
            case Screen::DICE:
                break;
            case Screen::SLOTS:
                break;
            case Screen::WHEEL:
                wheel.Draw();
                break;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}