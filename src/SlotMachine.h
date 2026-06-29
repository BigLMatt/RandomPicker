#pragma once
#include "raylib.h"
#include "raymath.h"
#include "Resources.h"
#include "Screen.h"
#include <string>
#include <cstring>
#include <vector>

class SlotMachine {
public:
    SlotMachine();
    void Update(Screen& current, const Resources& res);
    void Draw(const Resources& res) const;
    ~SlotMachine();

private:
    Camera3D m_camera {};

    bool m_spinning {false};
    float m_scrollSpeed {0.0f};
    int m_targetIndex {0};  // Item that ends in the middle
    bool m_settled  {false};

    std::string result {};

    char m_inputBuf[50] {""};
    std::vector<std::string> options {};

    bool m_inputActive {true};
    int cursorLocation {0};

    RenderTexture2D m_reel {};
    Model m_cylinder {};
    float m_reelAngle {0.0f};
    bool m_reelReady { false };  // true once texture is built

    static constexpr float CYLINDER_RADIUS {1.8f};
    static constexpr float CYLINDER_HEIGHT {3.0f};
    static constexpr int   CYLINDER_SLICES {36};

    static constexpr Rectangle addButton {515, 500, 60, 40};
    static constexpr Rectangle spinButton {100, 330, 100, 40};
    static constexpr Rectangle resetButton {590, 500, 60, 40};

    void RebuildReel(const Font& font);  // call when options change
    void UnloadReel();
    
};