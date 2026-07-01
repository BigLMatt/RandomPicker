#pragma once
#include "raylib.h"
#include "raymath.h"
#include "rcamera.h"
#include "Resources.h"
#include "Screen.h"
#include <string>
#include <cstring>
#include <vector>


class SlotMachine {
public:
    enum class ReelState {
        EMPTY,      // no options yet, static empty reel
        IDLE,       // options exist, slowly cycling
        SPINNING,   // fast spin in progress
        COASTING,   // slow coast to exact target
        SETTLED     // stopped on result, waiting for next action
    };


    SlotMachine();
    void Update(Screen& current, const Resources& res);
    void Draw(const Resources& res) const;
    ~SlotMachine();

private:
    Camera3D m_camera {};

    float m_scrollSpeed {0.0f};
    float m_decelFactor {0.98f};
    int m_targetIndex {0};  // Item that ends in the middle

    int m_targetOption {0};
    std::string result {};
    ReelState m_state { ReelState::EMPTY };

    char m_inputBuf[50] {""};
    std::vector<std::string> options {};

    bool m_inputActive {true};
    int cursorLocation {0};

    RenderTexture2D m_reel {};
    Model m_cylinder {};
    float m_reelAngle {0.0f};
    static constexpr float FRONT_OFFSET = 180.0f;
    bool m_reelReady { false };  // true once texture is built

    static constexpr float CYLINDER_RADIUS {1.6f};
    static constexpr float CYLINDER_HEIGHT {2.5f};
    static constexpr int   CYLINDER_SLICES {36};
    static constexpr int SEGMENTS {18};
    static constexpr float ANGLE_PER_SEGMENT {360.0f / static_cast<float>(SEGMENTS)};

    // UI elements
    static constexpr Rectangle ADD_BUTTON {515, 500, 60, 40};
    static constexpr Vector2 SPIN_BUTTON {600, 280};
    static constexpr Rectangle RESET_BUTTON {590, 500, 60, 40};

    void RebuildReel(const Font& font);  // call when options change
    void UnloadReel();
    
};