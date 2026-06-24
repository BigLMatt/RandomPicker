#pragma once

#include "raylib.h"
#include "Screen.h"
#include <string>
#include <cstring>
#include <string_view>
#include <vector>
#include "Resources.h"

class Wheel {
public:
    Wheel();
    void Update(Screen& current);
    void Draw(const Resources& res) const;

private:
    std::string result {};

    char m_inputBuf[50] {""};
    std::vector<std::string> options {};

    bool m_inputActive { true };
    int cursorLocation {0};
};
