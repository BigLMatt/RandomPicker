#include "raylib.h"
#include "src/screen.h"
#include "src/MenuScreen.h"
#include "src/NumberPicker.h"
#include "src/Wheel.h"
#include "src/Dice.h"
#include "src/SlotMachine.h"
#include "src/Resources.h"

int main() {
    InitWindow(800, 600, "RandomPicker");
    SetTargetFPS(60);

    Resources res{};
    res.regularFont = LoadFontEx("Resources/fonts/coolvetica_regular.otf", 40, nullptr, 0);
    res.titleFont = LoadFontEx("Resources/fonts/bronco.ttf",96,nullptr,0);
    if (res.regularFont.texture.id == 0)
        TraceLog(LOG_WARNING, "Font failed to load!");

    res.slotMachine = LoadModel("Resources/models/Slot1.glb");

    Screen current = Screen::MENU;
    MenuScreen menu;
    NumberPicker numberPicker;
    Wheel wheel;
    Dice dice;
    SlotMachine slots;
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
                dice.Update(current);
                break;
            case Screen::SLOTS:
                slots.Update(current, res);
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
                dice.Draw(res);
                break;
            case Screen::SLOTS:
                slots.Draw(res);
                break;
            case Screen::WHEEL:
                wheel.Draw(res);
                break;
        }

        EndDrawing();
    }

    UnloadFont(res.regularFont);
    UnloadModel(res.slotMachine);
    CloseWindow();
    return 0;
}