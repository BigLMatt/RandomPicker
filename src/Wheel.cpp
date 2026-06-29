#include "Wheel.h"
#include "random.h"

Wheel::Wheel() = default;

void Wheel::Update(Screen& current) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        current = Screen::MENU;
        return;
    }

    // Cursor activation
    constexpr Rectangle inputBox = {300, 200, 250, 40};
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

    // Arrow key navigation
    if ((IsKeyPressed(KEY_RIGHT)||IsKeyPressedRepeat(KEY_RIGHT)) && len > cursorLocation) {
        ++cursorLocation;
    } else if ((IsKeyPressed(KEY_LEFT)||IsKeyPressedRepeat(KEY_LEFT)) && cursorLocation > 0) {
        --cursorLocation;
    } else if (IsKeyPressed(KEY_UP)||IsKeyPressedRepeat(KEY_UP)) {
        cursorLocation = 0;
    } else if (IsKeyPressed(KEY_DOWN)||IsKeyPressedRepeat(KEY_DOWN)) {
        cursorLocation = len;
    }

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

    bool justAdded {false};

    // Add button
    constexpr Rectangle addButton = {300, 260, 200, 50};
    if ((CheckCollisionPointRec(GetMousePosition(), addButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) ||
        IsKeyPressed(KEY_ENTER)) {
        if (strlen(m_inputBuf) > 0) {
            options.emplace_back(m_inputBuf);
            m_inputBuf[0] = '\0';  // clear buffer
            cursorLocation = 0; // reset cursor
            justAdded = true;
        }
    }

    // Spin button
    constexpr Rectangle spinButton = {300, 330, 200, 50};
    if (!justAdded && (CheckCollisionPointRec(GetMousePosition(), spinButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) ||
        ((IsKeyPressed(KEY_ENTER) || IsKeyPressedRepeat(KEY_ENTER)) && len == 0)) {
        if (!options.empty())
            result = options[Random::randInt(0, static_cast<int>(options.size() - 1))];
        }

    // Reset button
    constexpr Rectangle resetButton = {300, 400, 200, 50};
    if (CheckCollisionPointRec(GetMousePosition(), resetButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (!options.empty())
            options = {};
        }

    // TODO: edit options functionality

}

void Wheel::Draw(const Resources& res) const {
    DrawTextEx(res.titleFont,"Wheel", {300, 50}, 100,4, WHITE);

    // Input label
    DrawTextEx(res.regularFont,"Option:", {300, 175}, 20,2, LIGHTGRAY);

    DrawRectangleRec({300, 200, 250, 40}, m_inputActive ? GRAY : DARKGRAY);

    std::string displayed = m_inputBuf;
    DrawTextEx(res.regularFont,displayed.c_str(), {310, 210}, 20,2, WHITE);

    // Cursor
    const std::string beforeCursor(m_inputBuf, cursorLocation);
    const Vector2 textSize {MeasureTextEx(res.regularFont, beforeCursor.c_str(), 20, 2)};
    const int cursorX {310 + static_cast<int>(textSize.x)};
    if (m_inputActive && static_cast<int>(GetTime() * 2) % 2 == 0)
        DrawRectangle(cursorX,212, 2, 18, WHITE);

    // Add button
    constexpr Rectangle addBtn = {300, 260, 200, 50};
    const bool hovered_add = CheckCollisionPointRec(GetMousePosition(), addBtn);
    DrawRectangleRec(addBtn, hovered_add ? DARKGRAY : GRAY);
    DrawTextEx(res.regularFont, "Add", {375, 277}, 20, 2, WHITE);

    // Spin button
    constexpr Rectangle spinBtn = {300, 330, 200, 50};
    const bool hovered_spin = CheckCollisionPointRec(GetMousePosition(), spinBtn);
    DrawRectangleRec(spinBtn, hovered_spin ? DARKGRAY : GRAY);
    DrawTextEx(res.regularFont,"Spin", {375, 347}, 20, 2, WHITE);

    // Reset button
    constexpr Rectangle resetBtn = {300, 400, 200, 50};
    const bool hovered_reset = CheckCollisionPointRec(GetMousePosition(), resetBtn);
    DrawRectangleRec(resetBtn, hovered_reset ? DARKGRAY : GRAY);
    DrawTextEx(res.regularFont,"Reset", {375, 417}, 20, 2, WHITE);

    // Options list
    for (int i = 0; i < static_cast<int>(options.size()); i++)
        DrawTextEx(res.regularFont,options[i].c_str(), {550, static_cast<float>(200 + i * 25)}, 18,2, LIGHTGRAY);

    // Result
    if (!result.empty()) {
    DrawTextEx(res.regularFont,TextFormat("Result: %s", result.c_str()), {340, 140}, 32,2, GOLD);
    }

    DrawTextEx(res.regularFont,"ESC = back", {10, 570}, 16,2, GRAY);
}