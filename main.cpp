#include "raylib.h"
#include "src/screen.h"
#include "src/MenuScreen.h"
#include "src/NumberPicker.h"

int main() {
    InitWindow(800, 600, "RandomPicker");
    SetTargetFPS(60);

    Screen current = Screen::MENU;
    MenuScreen menu;
    NumberPicker numberPicker;

    while (!WindowShouldClose()) {
        // Update
        switch (current) {
            case Screen::MENU:
                menu.Update(current);  // menu schrijft naar current als je klikt
                break;
            case Screen::NUMBER:
                numberPicker.Update(current);
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
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}