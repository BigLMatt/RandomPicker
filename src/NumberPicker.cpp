#include "NumberPicker.h"
#include "Random.h"
#include <cstring>
#include <cstdlib>

NumberPicker::NumberPicker() {}

void NumberPicker::Update(Screen& current) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        current = Screen::MENU;
        return;
    }

    // Text input
    int key = GetCharPressed();
    while (key > 0) {
        int len = strlen(m_inputBuf);
        if (key >= '0' && key <= '9' && len < 7) {
            m_inputBuf[len]     = (char)key;
            m_inputBuf[len + 1] = '\0';
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        int len = strlen(m_inputBuf);
        if (len > 0) m_inputBuf[len - 1] = '\0';
    }

    // Roll button
    Rectangle rollBtn = {300, 270, 200, 50};
    if (CheckCollisionPointRec(GetMousePosition(), rollBtn)
        && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        m_max = atoi(m_inputBuf);
        if (m_max > 0)
            m_result = Random::randInt(0, m_max);
        }
}

void NumberPicker::Draw() const {
    DrawText("Number Picker", 300, 80, 28, WHITE);

    // Input label
    DrawText("Max number:", 300, 175, 20, LIGHTGRAY);

    // Input box
    DrawRectangleRec({300, 200, 200, 40}, DARKGRAY);
    DrawText(m_inputBuf, 310, 210, 20, WHITE);

    // Roll button
    Rectangle rollBtn = {300, 270, 200, 50};
    bool hovered = CheckCollisionPointRec(GetMousePosition(), rollBtn);
    DrawRectangleRec(rollBtn, hovered ? DARKGRAY : GRAY);
    DrawText("Roll", 375, 285, 20, WHITE);

    // Result
    if (m_result >= 0)
        DrawText(TextFormat("Result: %d", m_result), 340, 150, 32, GOLD);  // boven het vak

    DrawText("ESC = back", 10, 570, 16, GRAY);
}