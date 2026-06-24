#include "MenuScreen.h"
#include "Screen.h"

MenuScreen::MenuScreen() {
    buttons[0] = { {300, 150, 200, 50}, "Number Picker", Screen::NUMBER };
    buttons[1] = { {300, 220, 200, 50}, "Wheel",         Screen::WHEEL  };
    buttons[2] = { {300, 290, 200, 50}, "Slots",         Screen::SLOTS  };
    buttons[3] = { {300, 360, 200, 50}, "Dice",          Screen::DICE   };
}

void MenuScreen::Update(Screen& current) {
    for (const auto& btn : buttons) {
        if (CheckCollisionPointRec(GetMousePosition(), btn.rect)
            && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            current = btn.target;
            }
    }
}

void MenuScreen::Draw(const Resources& res) const {
    for (auto& btn : buttons) {
        const bool hovered = CheckCollisionPointRec(GetMousePosition(), btn.rect);
        DrawRectangleRec(btn.rect, hovered ? DARKGRAY : GRAY);
        DrawTextEx(res.font, btn.label, {btn.rect.x + 10, btn.rect.y + 15}, 20, 2, WHITE);
    }
}