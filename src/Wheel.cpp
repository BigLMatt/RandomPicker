#include "Wheel.h"
#include "random.h"

Wheel::Wheel() = default;

void Wheel::Update(Screen& current) {

    if (IsKeyPressed(KEY_ESCAPE)) {
        current = Screen::MENU;
        return;
    }

    // Cursor activation
    Rectangle inputBox = {300, 200, 250, 40};
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        m_inputActive = CheckCollisionPointRec(GetMousePosition(), inputBox);

    int key = GetCharPressed();
    while (key > 0) {
        int len = static_cast<int>(strlen(m_inputBuf));
        if (key >= 32 && key <= 125 && len < 49) {
            memmove(&m_inputBuf[cursorLocation + 1], &m_inputBuf[cursorLocation], len - cursorLocation + 1);
            m_inputBuf[cursorLocation] = static_cast<char>(key);
            ++cursorLocation;
        }
        key = GetCharPressed();
    }

    int len = static_cast<int>(strlen(m_inputBuf));

    if ((IsKeyPressed(KEY_RIGHT)||IsKeyPressedRepeat(KEY_RIGHT)) && len > cursorLocation) {
        ++cursorLocation;
    } else if ((IsKeyPressed(KEY_LEFT)||IsKeyPressedRepeat(KEY_LEFT)) && cursorLocation > 0) {
        --cursorLocation;
    }
    // TODO: add up and down arrow begin line end line

    if ((IsKeyPressed(KEY_BACKSPACE)||IsKeyPressedRepeat(KEY_BACKSPACE)) && cursorLocation > 0) {
        memmove(&m_inputBuf[cursorLocation - 1], &m_inputBuf[cursorLocation], len - cursorLocation + 1);
        --cursorLocation;
        --len;
    }

    if (IsKeyPressed(KEY_DELETE)||IsKeyPressedRepeat(KEY_DELETE)) {
        len = static_cast<int>(strlen(m_inputBuf));
        if (cursorLocation < len) {
            memmove(&m_inputBuf[cursorLocation], &m_inputBuf[cursorLocation + 1], len - cursorLocation);
            --len;
        }
    }

    // Add button
    constexpr Rectangle addButton = {300, 260, 200, 50};
    if ((CheckCollisionPointRec(GetMousePosition(), addButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) ||
        IsKeyPressed(KEY_ENTER)) {
        if (strlen(m_inputBuf) > 0) {
            options.emplace_back(m_inputBuf);
            m_inputBuf[0] = '\0';  // clear buffer
            cursorLocation = 0; // reset cursor
        }
    }

    // Spin button
    constexpr Rectangle spinButton = {300, 330, 200, 50};
    if ((CheckCollisionPointRec(GetMousePosition(), spinButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) ||
        (IsKeyPressedRepeat(KEY_ENTER) && len == 0)) {
        if (!options.empty())
            result = options[Random::randInt(0, static_cast<int>(options.size() - 1))];
        }

    // TODO: add reset button

}

void Wheel::Draw() const {
    DrawText("Wheel", 300, 80, 28, WHITE);

    // Input label
    DrawText("Option:", 300, 175, 20, LIGHTGRAY);

    DrawRectangleRec({300, 200, 250, 40}, m_inputActive ? GRAY : DARKGRAY);

    std::string beforeCursor(m_inputBuf, cursorLocation);
    int cursorX = 310 + MeasureText(beforeCursor.c_str(), 20);

    std::string displayed = m_inputBuf;
    DrawText(displayed.c_str(), 310, 210, 20, WHITE);

    if (m_inputActive && static_cast<int>(GetTime() * 2) % 2 == 0)
        DrawText("|", cursorX, 210, 20, WHITE);

    // Add button
    constexpr Rectangle addBtn = {300, 260, 200, 50};
    const bool hovered_add = CheckCollisionPointRec(GetMousePosition(), addBtn);
    DrawRectangleRec(addBtn, hovered_add ? DARKGRAY : GRAY);
    DrawText("Add", 375, 277, 20, WHITE);

    // Spin button
    constexpr Rectangle spinBtn = {300, 330, 200, 50};
    const bool hovered_spin = CheckCollisionPointRec(GetMousePosition(), spinBtn);
    DrawRectangleRec(spinBtn, hovered_spin ? DARKGRAY : GRAY);
    DrawText("Spin", 375, 347, 20, WHITE);

    // Options list
    for (int i = 0; i < static_cast<int>(options.size()); i++)
        DrawText(options[i].c_str(), 550, 200 + i * 25, 18, LIGHTGRAY);

    // Result
    if (!result.empty()) {
    DrawText(TextFormat("Result: %s", result.c_str()), 340, 150, 32, GOLD);
    }

    DrawText("ESC = back", 10, 570, 16, GRAY);
}