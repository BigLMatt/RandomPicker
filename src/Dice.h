#pragma once
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include "Screen.h"
#include "Resources.h"

class Dice {
public:
    Dice();
    void Update(Screen& current);
    void Draw(const Resources& res) const;

private:
    Camera3D m_camera {};

    // Physics state
    Vector3 m_position    { 0, 10, 0 };      // startpositie hoog boven vloer
    Vector3 m_velocity    { 0, 0, 0 };      // snelheid
    Quaternion m_rotation { 0, 0, 0, 1 };   // rotatie (identity = geen rotatie)
    Vector3 m_angularVel  { 0, 0, 0 };      // rotatiesnelheid per as (graden/sec)

    bool  m_rolling  { false };
    bool  m_settled  { false };
    int   m_result   { -1 };

    static constexpr float GRAVITY      { 20.0f };
    static constexpr float RESTITUTION  { 0.3f  };
    static constexpr float FRICTION     { 0.85f };
    static constexpr float FLOOR_Y      { 0.0f  };
    static constexpr float DICE_SIZE    { 1.0f  };

    void Roll();
    void UpdatePhysics(float dt);
    int  DetectFace() const;
};