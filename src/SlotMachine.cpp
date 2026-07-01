#include "SlotMachine.h"
#include "Random.h"
#define DEBUG

SlotMachine::SlotMachine() {
    m_camera.position   = {0.0f, 3.5f, 7.0f};
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
    constexpr int TOTAL_SEGMENTS = SEGMENTS;
    constexpr int segmentHeight  = 60;
    constexpr int texHeight      = TOTAL_SEGMENTS * segmentHeight;

    UnloadReel();

    m_reel = LoadRenderTexture(256, texHeight);

    // Draw all options onto the texture
    BeginTextureMode(m_reel);
        ClearBackground(DARKGRAY);
        for (int i {0}; i < TOTAL_SEGMENTS; i++) {

            const int segmentIndex {TOTAL_SEGMENTS-1-i};

            // Alternate background colour
            const Color bg {(segmentIndex % 2 == 0) ? Color{40,40,60,255} : Color{30,30,50,255}};
            DrawRectangle(0,i * segmentHeight, 256, segmentHeight, bg);

            // Dividing line
            DrawLine(0,i * segmentHeight, 256, i * segmentHeight, GRAY);

            // Only draw text if we have options
            if (!options.empty()) {
                const std::string& label = options[segmentIndex % options.size()];
                DrawTextEx(font, label.c_str(), {10.0f, static_cast<float>(i) * segmentHeight + 16.0f}, 24, 2, WHITE);
            }
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
    // Orbit camera with middle mouse button drag
    if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON) && !IsKeyDown(KEY_LEFT_SHIFT)) {
        const auto [x, y] {GetMouseDelta()};
        CameraYaw(&m_camera, -x * 0.003f, true);
        CameraPitch(&m_camera, -y * 0.003f, true, true, false);
    }
    if (IsMouseButtonDown(MOUSE_MIDDLE_BUTTON) && IsKeyDown(KEY_LEFT_SHIFT)) {
        const auto [x, y] {GetMouseDelta()};
        CameraMoveUp(&m_camera, y * 0.01f);
        CameraMoveRight(&m_camera, -x * 0.01f, true);
    }

    // Zoom with scroll wheel
    if (const float wheel = GetMouseWheelMove(); wheel != 0)
        CameraMoveToTarget(&m_camera, -wheel);


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

    int len {static_cast<int>(strlen(m_inputBuf))};

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
    if ((CheckCollisionPointRec(GetMousePosition(), ADD_BUTTON) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && m_inputActive) ||
        IsKeyPressed(KEY_ENTER)) {
        if (strlen(m_inputBuf) > 0) {
            options.emplace(options.begin(), m_inputBuf);
            m_inputBuf[0] = '\0';
            cursorLocation = 0;
            justAdded = true;
            RebuildReel(res.regularFont);  // rebuild texture with new option
            m_state = ReelState::IDLE;
        }
    }

    // Spin button
    if (!justAdded && (CheckCollisionPointCircle(GetMousePosition(), SPIN_BUTTON, 25) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) ||
        ((IsKeyPressed(KEY_ENTER) || IsKeyPressedRepeat(KEY_ENTER)) && len == 0)) {
        if (!options.empty()) {
            m_targetOption = {Random::randInt(0, static_cast<int>(options.size()) - 1)};
            result = options[m_targetOption];

            m_scrollSpeed = static_cast<float>(Random::randInt(900, 1500));
            m_decelFactor = 0.965f + static_cast<float>(Random::randInt(0, 20)) * 0.001f;
            m_state = ReelState::SPINNING;
            }
        }

    // Reset button
    if (CheckCollisionPointRec(GetMousePosition(), RESET_BUTTON) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        options = {};
        RebuildReel(res.regularFont);
        m_state = ReelState::EMPTY;
        }


    // Scroll animation
    switch (m_state) {
        case ReelState::SPINNING:
        m_reelAngle += m_scrollSpeed * GetFrameTime();
        m_scrollSpeed *= m_decelFactor;

        // Check if slow enough to stop
        if (m_scrollSpeed < 25.0f) {
            m_scrollSpeed = 25.0f;
            const int currentStep {static_cast<int>(m_reelAngle/ ANGLE_PER_SEGMENT)};

            // Add extra segments crossed minimum to coast out
            const int extraSegments {Random::randInt(1, 5)};
            const int searchFrom {currentStep + extraSegments};

            // How many segments after search start
            const int optSize {static_cast<int>(options.size())};
            const int stepsUntilMatch {(m_targetOption - searchFrom % optSize + optSize) % optSize};
            m_targetIndex = searchFrom + stepsUntilMatch;

            m_state = ReelState::COASTING;
        }
        break;
        case ReelState::COASTING:
            // Coast to selected item
            m_reelAngle += m_scrollSpeed * GetFrameTime();
            if (const float targetAngle = static_cast<float>(m_targetIndex) * ANGLE_PER_SEGMENT; m_reelAngle >= targetAngle) {
                m_reelAngle = targetAngle;
                m_state = ReelState::SETTLED;
            }
            break;
        case ReelState::IDLE:
            m_reelAngle += 20.0f * GetFrameTime();
            break;
        case ReelState::EMPTY:
            m_reelAngle = 0.0f;
            break;
        case ReelState::SETTLED:
            break;
    }
}

void SlotMachine::Draw(const Resources &res) const {
    BeginMode3D(m_camera);
        // Light
        DrawSphere({2.0f, 4.0f, 2.0f}, 0.1f, YELLOW);

        // Slotmachine model and reel
        DrawModelEx(res.slotMachine, {0,-1,0}, {0,1,0}, -90.0f,{1,1,1}, WHITE);
        if (m_reelReady) {
            DrawModelEx(m_cylinder, {-0.65f,0.6f,-0.6f}, {1,0,0}, m_reelAngle + FRONT_OFFSET, {0.5f,0.5f,0.5f}, WHITE);
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

    if (m_state == ReelState::SETTLED) {
        DrawTextEx(res.regularFont, "Result:", {100,100},40,2,GOLD);
        DrawTextEx(res.regularFont, result.c_str(), {100,150},40,2,GOLD);
    }

    // Add button
    const bool hovered_add = CheckCollisionPointRec(GetMousePosition(), ADD_BUTTON);
    DrawRectangleRec(ADD_BUTTON, hovered_add ? DARKGRAY : GRAY);
    DrawTextEx(res.regularFont, "Add", {529, 510}, 20, 2, WHITE);

    // Spin button
    const bool hovered_spin = CheckCollisionPointCircle(GetMousePosition(), SPIN_BUTTON,25);
    DrawCircleV(SPIN_BUTTON, 25,hovered_spin ? DARKBROWN : RED);

    // Reset button
    const bool hovered_reset = CheckCollisionPointRec(GetMousePosition(), RESET_BUTTON);
    DrawRectangleRec(RESET_BUTTON, hovered_reset ? DARKGRAY : GRAY);
    DrawTextEx(res.regularFont,"Reset", {595, 510}, 20, 2, WHITE);

#ifdef DEBUG
    // Debug overlay
    constexpr float anglePerItem = 360.0f / static_cast<float>(SEGMENTS);
    const float targetAngle = static_cast<float>(m_targetIndex) * anglePerItem;
    const int currentStep = static_cast<int>(m_reelAngle / anglePerItem);
    const int currentSegment = ((currentStep % SEGMENTS) + SEGMENTS) % SEGMENTS;
    const int currentOption = options.empty() ? -1 : currentSegment % static_cast<int>(options.size());

    DrawTextEx(res.regularFont, TextFormat("reelAngle:    %.1f", m_reelAngle),        {10, 10}, 18, 2, RED);
    DrawTextEx(res.regularFont, TextFormat("targetAngle:  %.1f", targetAngle),         {10, 30}, 18, 2, RED);
    DrawTextEx(res.regularFont, TextFormat("currentStep:  %d",   currentStep),         {10, 50}, 18, 2, RED);
    DrawTextEx(res.regularFont, TextFormat("targetIndex:  %d",   m_targetIndex),       {10, 70}, 18, 2, RED);
    DrawTextEx(res.regularFont, TextFormat("currSegment:  %d",   currentSegment),      {10, 90}, 18, 2, RED);
    DrawTextEx(res.regularFont, TextFormat("currOption:   %d",   currentOption),       {10,110}, 18, 2, RED);
    DrawTextEx(res.regularFont, TextFormat("result:       %s",   result.c_str()),      {10,130}, 18, 2, RED);
    DrawTextEx(res.regularFont, TextFormat("scrollSpeed:  %.1f", m_scrollSpeed),       {10,150}, 18, 2, RED);
#endif
}