#pragma once
#include "D3DClass.h"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


class MyGraphics {
public:
    MyGraphics();
    MyGraphics(const MyGraphics& other);
    virtual ~MyGraphics();

    bool initalize(int screenWidth, int screenHeight, HWND hwnd);
    void shutdown();
    bool frame();
private:
    bool render();
    D3DClass *m_D3D;
};

