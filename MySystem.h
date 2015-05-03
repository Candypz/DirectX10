#pragma once
#include <windows.h>
#include "MyInput.h"
#include "MyGraphics.h"

class MySystem {
public:
    MySystem();
    MySystem(const MySystem& other);
    virtual ~MySystem();

    //初始化
    bool initalize(int width,int height);
    //清理，释放m_input,m_graphics,并关闭窗口和句柄
    void shutdown();
    void run();

    //处理windows消息
    LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);
private:
    bool frame();
    //初始化窗口
    void initializeWindows(int& screenWidth, int& screenHeight, LPCWSTR applicationName);
    void shutdownWindows();

private:
    //指向unicode编码字符串的32位指针，所指向字符串是wchar型
    LPCWSTR m_applicationName;
    //装入到了内存的资源的ID，句柄类型
    HINSTANCE m_hinstance;
    //窗口句柄
    HWND m_hwnd;

    MyInput* m_input;
    MyGraphics* m_graphics;
};

static LRESULT CALLBACK Wndproc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);

static MySystem *ApplicationHandle = NULL;

