#pragma once
#include "raylib.h"
#include "Screen.h"

struct Button {
    Rectangle rect;
    const char* label;
    Screen target;
};

class MenuScreen {
public:
    MenuScreen();
    void Update(Screen& current);
    void Draw() const;
private:
    Button buttons[4];
};