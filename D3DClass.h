#pragma once
#pragma comment(lib,"d3d10.lib")
#pragma comment(lib,"d3dx10.lib")
#pragma comment(lib,"dxgi.lib")

#include <d3d10.h>
#include <D3DX10.h>

class D3DClass {
public:
    D3DClass();
    D3DClass(const D3DClass &other);
    virtual ~D3DClass();

    bool initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen,float screenDepth, float screenNear);
    void shutdown();

    void beginScene(float red, float green, float blue, float alpha);
    void endScene();

    ID3D10Device *getDevice();

    void getProjectionMatrix(D3DXMATRIX &projectionMatrix);
    void getWorldMatrix(D3DXMATRIX &WorldMatrix);
    void getOtherMatrix(D3DXMATRIX &otherMatrix);
    void getVideoCardInfo(char *cardName, int &memory);
private:
    //´¹Ö±Í¬²½
    bool m_vsyncEnabled;
    int  m_videoCardMemory;
    char m_videoCardDescription[128];
    IDXGISwapChain *m_swapChain;
    ID3D10Device *m_device;
    ID3D10RenderTargetView *m_renderTargetView;
    ID3D10Texture2D *m_depthStencilBuffer;
    ID3D10DepthStencilState *m_depthStencilState;
    ID3D10DepthStencilView *m_depthStencilView;
    ID3D10RasterizerState *m_rasterizerState;
    D3DXMATRIX m_projectionMatrix;
    D3DXMATRIX m_worldMatrix;
    D3DXMATRIX m_otherMatrix;
};

