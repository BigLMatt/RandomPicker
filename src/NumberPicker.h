#pragma once
#include "raylib.h"
#include "Screen.h"

class NumberPicker {
public:
    NumberPicker();
    void Update(Screen& current);
    void Draw() const;

private:
    int m_max       { 100 };   // user-controlled
    int m_result    { -1  };   // -1 = nog niet gerold
    bool m_editing  { false };

    char m_inputBuf[8] { "100" }; // tekstbuffer voor het invoervak
};