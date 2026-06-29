#include "Dice.h"
#include "Random.h"

Dice::Dice() {
    m_camera.position   = {6.0f, 5.0f, 6.0f};
    m_camera.target     = {0.0f, 0.0f, 0.0f};
    m_camera.up         = {0.0f, 1.0f, 0.0f};
    m_camera.fovy       = 45.0f;
    m_camera.projection = CAMERA_PERSPECTIVE;
}

void Dice::Roll() {
    m_position   = {0, 6, 0};
    m_velocity   = {
        static_cast<float>(Random::randInt(-3, 3)),
        0,
        static_cast<float>(Random::randInt(-3, 3))
    };
    m_angularVel = {
        static_cast<float>(Random::randInt(-180, 180)),
        static_cast<float>(Random::randInt(-180, 180)),
        static_cast<float>(Random::randInt(-180, 180))
    };
    m_rotation = QuaternionIdentity();
    m_rolling  = true;
    m_settled  = false;
    m_result   = -1;
}

void Dice::UpdatePhysics(float dt) {
    if (!m_rolling || m_settled) return;

    // Zwaartekracht
    m_velocity.y -= GRAVITY * dt;

    // Positie updaten
    m_position = Vector3Add(m_position, Vector3Scale(m_velocity, dt));

    // Rotatie updaten via quaternion
    float angleThisFrame = Vector3Length(m_angularVel) * dt;
    if (angleThisFrame > 0.0f) {
        Vector3 axis = Vector3Normalize(m_angularVel);
        Quaternion deltaRot = QuaternionFromAxisAngle(axis, angleThisFrame * DEG2RAD);
        m_rotation = QuaternionMultiply(m_rotation, deltaRot);
        m_rotation = QuaternionNormalize(m_rotation);
    }

    // Vloer collision
    float half = DICE_SIZE * 0.5f;
    if (m_position.y - half < FLOOR_Y) {
        m_position.y = FLOOR_Y + half;

        m_velocity.y  *= -RESTITUTION;
        m_velocity.x  *= FRICTION;
        m_velocity.z  *= FRICTION;
        m_angularVel.x *= FRICTION;
        m_angularVel.y *= FRICTION;
        m_angularVel.z *= FRICTION;

        // Gestopt als snelheid klein genoeg is
        if (fabsf(m_velocity.y) < 0.5f && Vector3Length(m_velocity) < 1.0f) {
            m_velocity    = {0, 0, 0};
            m_angularVel  = {0, 0, 0};
            m_settled     = true;
            m_result      = DetectFace();
        }
    }
}

int Dice::DetectFace() const {
    // De 6 face normaals in lokale ruimte
    const int values[6] = {1, 6, 2, 5, 3, 4};

    // Transformeer elke normaal met de huidige rotatie
    // De normaal met de hoogste Y wijst naar boven = die face ligt boven
    int   bestFace  = 0;
    float bestDot   = -999;

    for (int i = 0; i < 6; i++) {
        constexpr Vector3 normals[6] = {
            { 0,  1,  0},  // face 1 — boven
            { 0, -1,  0},  // face 6 — onder
            { 1,  0,  0},  // face 2
            {-1,  0,  0},  // face 5
            { 0,  0,  1},  // face 3
            { 0,  0, -1},  // face 4
        };
        Vector3 world = Vector3RotateByQuaternion(normals[i], m_rotation);
        if (world.y > bestDot) {
            bestDot  = world.y;
            bestFace = i;
        }
    }
    return values[bestFace];
}

void Dice::Update(Screen& current) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        current = Screen::MENU;
        return;
    }

    if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        Roll();

    UpdatePhysics(GetFrameTime());
}

void Dice::Draw(const Resources& res) const {
    BeginMode3D(m_camera);
        DrawGrid(10, 1.0f);

        // Kubus tekenen met rotatie via matrix
        Matrix rotMatrix = QuaternionToMatrix(m_rotation);
        // Raylib heeft geen DrawCubeWithMatrix ingebouwd —
        // we gebruiken rlPushMatrix met de rotatie
        rlPushMatrix();
            rlTranslatef(m_position.x, m_position.y, m_position.z);
            rlMultMatrixf(MatrixToFloat(rotMatrix));
            DrawCube({0,0,0}, DICE_SIZE, DICE_SIZE, DICE_SIZE, WHITE);
            DrawCubeWires({0,0,0}, DICE_SIZE, DICE_SIZE, DICE_SIZE, BLACK);
        rlPopMatrix();

    EndMode3D();

    if (m_result > 0)
        DrawTextEx(res.regularFont, TextFormat("Result: %d", m_result), {320, 100}, 32, 2, GOLD);

    DrawTextEx(res.regularFont, "SPACE = roll", {10, 570}, 16, 2, GRAY);
    DrawTextEx(res.regularFont, "ESC = back",   {10, 550}, 16, 2, GRAY);
}