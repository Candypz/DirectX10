#pragma once
#include <windows.h>
#include "MyInput.h"
#include "MyGraphics.h"

class MySystem {
public:
    MySystem();
    MySystem(const MySystem& other);
    virtual ~MySystem();

    //��ʼ��
    bool initalize(int width,int height);
    //�����ͷ�m_input,m_graphics,���رմ��ں;��
    void shutdown();
    void run();

    //����windows��Ϣ
    LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);
private:
    bool frame();
    //��ʼ������
    void initializeWindows(int& screenWidth, int& screenHeight, LPCWSTR applicationName);
    void shutdownWindows();

private:
    //ָ��unicode�����ַ�����32λָ�룬��ָ���ַ�����wchar��
    LPCWSTR m_applicationName;
    //װ�뵽���ڴ����Դ��ID���������
    HINSTANCE m_hinstance;
    //���ھ��
    HWND m_hwnd;

    MyInput* m_input;
    MyGraphics* m_graphics;
};

static LRESULT CALLBACK Wndproc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);

static MySystem *ApplicationHandle = NULL;

