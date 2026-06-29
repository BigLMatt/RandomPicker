#include "SlotMachine.h"
#include "Random.h"

SlotMachine::SlotMachine() {
    m_camera.position   = {0.0f, 3.0f, 8.0f};
    m_camera.target     = {0.0f, 0.0f, 0.0f};
    m_camera.up         = {0.0f, 1.0f, 0.0f};
    m_camera.fovy       = 45.0f;
    m_camera.projection = CAMERA_PERSPECTIVE;
}

SlotMachine::~SlotMachine() {
    UnloadReel();
}

void SlotMachine::UnloadReel() {
    if (m_reelReady) {
        UnloadRenderTexture(m_reel);
        UnloadModel(m_cylinder);
        m_reelReady = false;
    }
}

void SlotMachine::RebuildReel(const Font& font) {
    constexpr int TOTAL_SEGMENTS = 18;
    constexpr int segmentHeight  = 60;
    constexpr int texHeight      = TOTAL_SEGMENTS * segmentHeight;

    UnloadReel();

    if (options.empty()) {
        // Draw 18 empty segments
        BeginTextureMode(m_reel);
        ClearBackground(DARKGRAY);
        for (int i = 0; i < TOTAL_SEGMENTS; i++) {
            Color bg = (i % 2 == 0) ? Color{40,40,60,255} : Color{30,30,50,255};
            DrawRectangle(0, i * segmentHeight, 256, segmentHeight, bg);
            DrawLine(0, i * segmentHeight, 256, i * segmentHeight, GRAY);
        }
        EndTextureMode();
    }

    m_reel = LoadRenderTexture(256, texHeight);

    // Draw all options onto the texture
    BeginTextureMode(m_reel);
        ClearBackground(DARKGRAY);
        for (int i {0}; i < TOTAL_SEGMENTS; i++) {
            // Cycle through options sequentially
            const std::string& label = options[i % options.size()];
            const auto y = static_cast<float>(i * segmentHeight);

            // Alternate background colour
            const Color bg {(i % 2 == 0) ? Color{40,40,60,255} : Color{30,30,50,255}};
            DrawRectangle(0,static_cast<int>(y), 256, segmentHeight, bg);

            // Dividing line
            DrawLine(0,static_cast<int>(y), 256, static_cast<int>(y), GRAY);

            // Text centered vertically in segment
            DrawTextEx(font, label.c_str(), {10.0f, y + 16.0f}, 24, 2, WHITE);
        }
    EndTextureMode();

    // Build cylinder with that texture
    const Mesh mesh {GenMeshCylinder(CYLINDER_RADIUS, CYLINDER_HEIGHT, CYLINDER_SLICES)};
    m_cylinder = LoadModelFromMesh(mesh);
    m_cylinder.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = m_reel.texture;
    m_cylinder.transform = MatrixRotateZ(-90.0f * DEG2RAD);

    m_reelReady = true;
}

void SlotMachine::Update(Screen& current, const Resources& res) {
    // Regular name entry like wheel code
    if (IsKeyPressed(KEY_ESCAPE)) {
        current = Screen::MENU;
        return;
    }


    // Cursor activation
    constexpr Rectangle inputBox = {200, 500, 300, 40};
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
    if ((CheckCollisionPointRec(GetMousePosition(), addButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) ||
        IsKeyPressed(KEY_ENTER)) {
        if (strlen(m_inputBuf) > 0) {
            options.emplace_back(m_inputBuf);
            m_inputBuf[0] = '\0';
            cursorLocation = 0;
            justAdded = true;
            RebuildReel(res.regularFont);  // rebuild texture with new option
        }
    }

    // Spin button
    if (!justAdded && (CheckCollisionPointRec(GetMousePosition(), spinButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) ||
        ((IsKeyPressed(KEY_ENTER) || IsKeyPressedRepeat(KEY_ENTER)) && len == 0)) {
        if (!options.empty()) {
            m_targetIndex = Random::randInt(0, static_cast<int>(options.size() - 1));
            result = options[m_targetIndex];
            m_scrollSpeed = 1200.0f;  // start fast
            m_spinning = true;
            }
        }

    // Reset button
    if (CheckCollisionPointRec(GetMousePosition(), resetButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        options = {};
        RebuildReel(res.regularFont);
        }

    // Scroll animation
    if (m_spinning) {
        m_reelAngle += m_scrollSpeed * GetFrameTime();
        m_scrollSpeed *= 0.98f;  // gradually slow down

        // Check if slow enough to stop
        if (m_scrollSpeed < 10.0f) {
            m_spinning = false;
            // Snap to nearest item
            const float anglePerItem = 360.0f / static_cast<float>(options.size());
            m_reelAngle = static_cast<float>(m_targetIndex) * anglePerItem;
        }
    }

    // Idle slow rotation when options are loaded
    if (!m_spinning && m_reelReady)
        m_reelAngle += 20.0f * GetFrameTime();  // 15 degrees per second
}

void SlotMachine::Draw(const Resources &res) const {
    BeginMode3D(m_camera);
        // Light
        DrawSphere({2.0f, 4.0f, 2.0f}, 0.1f, YELLOW);

        // Slotmachine model and reel
        DrawModelEx(res.slotMachine, {0,-1,0}, {0,1,0}, -90.0f,{1,1,1}, WHITE);
        if (m_reelReady) {
            DrawModelEx(m_cylinder, {0.0f,0.2f,0.3f}, {1,0,0}, m_reelAngle, {0.5f,0.5f,0.5f}, WHITE);
        }
    EndMode3D();

    DrawTextEx(res.titleFont,"Slot Machine", {250, 50}, 100,4, WHITE);

    // Input field
    DrawTextEx(res.regularFont,"Option:", {200, 475}, 20,2, LIGHTGRAY);
    DrawRectangleRec({200, 500, 300, 40}, m_inputActive ? GRAY : DARKGRAY);

    const std::string displayed = m_inputBuf;
    DrawTextEx(res.regularFont,displayed.c_str(), {210, 510}, 20,2, WHITE);

    // Cursor
    const std::string beforeCursor(m_inputBuf, cursorLocation);
    const auto [text_x, text_y] {MeasureTextEx(res.regularFont, beforeCursor.c_str(),20,2)};
    const int cursorX {210 + static_cast<int>(text_x)};

    if (m_inputActive && static_cast<int>(GetTime() * 2) % 2 == 0)
        DrawRectangle(cursorX, 512, 2, 18, WHITE);

    // Add button
    const bool hovered_add = CheckCollisionPointRec(GetMousePosition(), addButton);
    DrawRectangleRec(addButton, hovered_add ? DARKGRAY : GRAY);
    DrawTextEx(res.regularFont, "Add", {529, 510}, 20, 2, WHITE);

    // Spin button
    const bool hovered_spin = CheckCollisionPointRec(GetMousePosition(), spinButton);
    DrawRectangleRec(spinButton, hovered_spin ? DARKGRAY : GRAY);
    DrawTextEx(res.regularFont,"Spin", {140, 345}, 20, 2, WHITE);

    // Reset button
    const bool hovered_reset = CheckCollisionPointRec(GetMousePosition(), resetButton);
    DrawRectangleRec(resetButton, hovered_reset ? DARKGRAY : GRAY);
    DrawTextEx(res.regularFont,"Reset", {595, 510}, 20, 2, WHITE);

}