#include "MyGraphics.h"


MyGraphics::MyGraphics() {
    m_D3D = NULL;
}


MyGraphics::~MyGraphics() {
}

MyGraphics::MyGraphics(const MyGraphics& other) {

}

bool MyGraphics::initalize(int screenWidth, int screenHeight, HWND hwnd) {
    m_D3D = new D3DClass;
    if (!m_D3D) {
        return false;
    }

    if (!(m_D3D->initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN,
        SCREEN_DEPTH, SCREEN_NEAR))) {
        MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
        return false;
    }

    return true;
}

void MyGraphics::shutdown() {
    if (m_D3D) {
        m_D3D->shutdown();
        delete m_D3D;
        m_D3D = NULL;
    }
}

bool MyGraphics::frame() {
    if (!(render())) {
        return false;
    }

    return true;
}

bool MyGraphics::render() {
    m_D3D->beginScene(0.5f, 0.5f, 0.5f, 1.0f);

    m_D3D->endScene();

    return true;
}