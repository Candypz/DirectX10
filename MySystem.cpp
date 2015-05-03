#include "MySystem.h"


MySystem::MySystem() {
    m_input = NULL;
    m_graphics = NULL;
}


MySystem::~MySystem() {
}

MySystem::MySystem(const MySystem& other) {

}

bool MySystem::initalize(int windth,int height) {
    int screenWindth, screenHeight;

    screenHeight = height;
    screenWindth = windth;

    initializeWindows(screenWindth, screenHeight,L"My");

    m_input = new MyInput;
    if (!m_input) {
        return false;
    }
    m_input->initialize();

    m_graphics = new MyGraphics;
    if (!m_graphics) {
        return false;
    }

    if (!(m_graphics->initalize(screenWindth, screenHeight,m_hwnd))) {
        return false;
    }

    return true;
}

void MySystem::shutdown() {
    //Release m_input
    if (m_input) {
        delete m_input;
        m_input = NULL;
    }

    //Release m_graphics
    if (m_graphics) {
        m_graphics->shutdown();
        delete m_graphics;
        m_graphics = NULL;
    }
    //shutdiwn the windows
    shutdownWindows();
}

void MySystem::run() {
    MSG msg;
    bool done;

    //��msg���Ϊ0
    ZeroMemory(&msg, sizeof(MSG));

    done = false;
    while (!done) {
        //��ⴰ����Ϣ
        //PeekMessage��Ϣ����߳���Ϣ���У���������Ϣ��������ڣ�����ָ���Ľṹ
        //TranslateMessage�������ڽ��������Ϣת��Ϊ�ַ���Ϣ��
        //DispatchMessage�ַ�һ����Ϣ�����ڳ���
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        //�˳�
        if (msg.message == WM_QUIT) {
            done = true;
        }
        else {
            if (!(frame())) {
                done = true;
            }
        }
    }

}

LRESULT CALLBACK MySystem::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam) {
    /*
       HWND   ������Ϣ�Ĵ��ڹ��̵ľ��
       UINT   ָ����Ϣ����
       WPARAM ָ������ġ���Ϣ�ض�����Ϣ
     */
    switch (umsg) {
        case WM_KEYDOWN:{
            m_input->keyDown((unsigned int)wparam);
            return 0;
        }
        case WM_KEYUP:{
            m_input->keyUp((unsigned int)wparam);
            return 0;
        }
        default:{
            //DefWindowProc����ȱʡ�Ĵ��ڹ�����ΪӦ�ó���û�д�����κδ�����Ϣ�ṩȱʡ�Ĵ���
            return DefWindowProc(hwnd, umsg, wparam, lparam);
        }
    }

    return 0;
}

bool MySystem::frame() {
    if ((m_input->isKeyDown(VK_ESCAPE))) {
        return false;
    }

    if (!(m_graphics->frame())) {
        return false;
    }

    return true;
}

void MySystem::initializeWindows(int& screenWidth, int& screenHeight, LPCWSTR applicationName) {
    WNDCLASSEX wc;
    DEVMODE dmScreenSettings;
    int posX, posY;

    ApplicationHandle = this;

    m_hinstance = GetModuleHandle(NULL);

    m_applicationName = applicationName;

    wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc   = Wndproc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = m_hinstance;
    wc.hIcon         = LoadIcon(NULL, IDI_WINLOGO);
    wc.hIconSm       = wc.hIcon;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = m_applicationName;
    wc.cbSize        = sizeof(WNDCLASSEX);

    RegisterClassEx(&wc);


    if (FULL_SCREEN) {
        memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
        dmScreenSettings.dmSize = sizeof(dmScreenSettings);
        dmScreenSettings.dmPelsWidth  = (unsigned long)screenWidth;
        dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
        dmScreenSettings.dmBitsPerPel = 32;
        dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

        posX = posY = 0;
    }
    else {
        posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
        posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;

    }
    m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
//                             WS_CLIPSIBLINGS | WS_CLIPCHILDREN| /*| WS_POPUP*/
//                             WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
WS_OVERLAPPEDWINDOW,
                            posX, posY, screenWidth, screenHeight, NULL, NULL,
                            m_hinstance, NULL);
    ShowWindow(m_hwnd, SW_SHOW);
    SetForegroundWindow(m_hwnd);
    SetFocus(m_hwnd);

    ShowCursor(false);
}

void MySystem::shutdownWindows() {
    //���ع��
    ShowCursor(false);

    if (FULL_SCREEN) {
        ChangeDisplaySettings(NULL, 0);
    }

    DestroyWindow(m_hwnd);
    m_hwnd = NULL;

    UnregisterClass(m_applicationName, m_hinstance);
    m_hinstance = NULL;

    ApplicationHandle = NULL;
}

LRESULT CALLBACK Wndproc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam) {
    switch (umessage) {
        case WM_DESTROY:{
            PostQuitMessage(0);
            return 0;
        }
        case WM_CLOSE:{
            PostQuitMessage(0);
            return 0;
        }
        default:{
            return ApplicationHandle->MessageHandler(hwnd,umessage,wparam,lparam);
        }
    }
}