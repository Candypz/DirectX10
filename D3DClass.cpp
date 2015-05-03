#include "D3DClass.h"
#include <stdio.h>


D3DClass::D3DClass() {
    m_device = NULL;
    m_swapChain = NULL;
    m_renderTargetView = NULL;
    m_depthStencilBuffer = NULL;
    m_depthStencilState = NULL;
    m_depthStencilView = NULL;
    m_rasterizerState = NULL;
}


D3DClass::~D3DClass() {

}

D3DClass::D3DClass(const D3DClass &other) {

}

bool D3DClass::initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear) {
    HRESULT result;
    IDXGIFactory *factory;
    IDXGIAdapter *adapter;
    IDXGIOutput  *adapterOutput;
    unsigned int numModes, numerator, denominator, stringLength;
    DXGI_MODE_DESC *displayModeList;
    DXGI_ADAPTER_DESC adapterDesc;
    int error;
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ID3D10Texture2D *backBufferPtr;
    D3D10_TEXTURE2D_DESC depthBufferDesc;
    D3D10_DEPTH_STENCIL_DESC depthStencilDesc;
    D3D10_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    D3D10_VIEWPORT viewport;
    float fieldOfView, screenAspect;
    D3D10_RASTERIZER_DESC rasterDesc;

    //存储设置
    m_vsyncEnabled = vsync;

    //创造一个dx工厂
    result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
    if (FAILED(result)) {
        return false;
    }

    result = factory->EnumAdapters(0, &adapter);
    if (FAILED(result)) {
        return false;
    }

    //列举主要的适配器输出(监控)
    result = adapter->EnumOutputs(0, &adapterOutput);
    if (FAILED(result)) {
        return false;
    }

    //获取输出格式分辨率
    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
    if (FAILED(result)) {
        return false;
    }

    //创建一个列表存储可以的显示模式和显卡组合
    displayModeList = new DXGI_MODE_DESC[numModes];
    if (!displayModeList) {
        return false;
    }

    //填充显示模式和列表结构
    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
    if (FAILED(result)) {
        return false;
    }

    //遍历所有显示模式找到一个匹配的宽高
    //当找到是存储分子和分母的刷新率
    for (int i = 0; i < numModes; ++i) {
        if (displayModeList[i].Width == (unsigned int)screenWidth) {
            if (displayModeList[i].Height == (unsigned int)screenHeight) {
                numerator = displayModeList[i].RefreshRate.Numerator;
                denominator = displayModeList[i].RefreshRate.Denominator;
            }
        }
    }

    //获取显卡信息
    result = adapter->GetDesc(&adapterDesc);
    if (FAILED(result)) {
        return false;
    }

    //将显存以兆为单位
    m_videoCardMemory = (int)(adapterDesc.DedicatedSystemMemory / 1024 / 1024);

    //将显卡名称转换为字符数组存储
    error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
    if (error != 0) {
        return false;
    }

    //释放所用的结构体和接口
    delete[] displayModeList;
    displayModeList = NULL;

    adapterOutput->Release();
    adapterOutput = NULL;

    adapter->Release();
    adapter = NULL;

    factory->Release();
    factory = NULL;

    //初始化交换列表
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

    //设置一个缓冲区
    swapChainDesc.BufferCount = 1;

    //设置缓冲区的宽高
    swapChainDesc.BufferDesc.Width = screenWidth;
    swapChainDesc.BufferDesc.Height = screenHeight;

    //设置缓冲区为32bit
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    //设置缓冲区的刷新率
    if (m_vsyncEnabled) {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
    }
    else {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    }

    //设置缓冲区的使用
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    //设置窗口呈现的处理
    swapChainDesc.OutputWindow = hwnd;

    //关闭多重采样
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    //设置是否全屏
    if (fullscreen) {
        swapChainDesc.Windowed = FALSE;
    }
    else {
        swapChainDesc.Windowed = TRUE;
    }

    //设置扫描行排列序
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    
    //呈现后丢弃缓冲区的内容
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    //不要设置前进的标志
    swapChainDesc.Flags = 0;

    //创建交换链和Direct3D设备
    result = D3D10CreateDeviceAndSwapChain(NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, 0, D3D10_SDK_VERSION,
        &swapChainDesc, &m_swapChain, &m_device);
    if (FAILED(result)) {
        return false;
    }

    //得到缓冲区的指针
    result = m_swapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&backBufferPtr);
    if (FAILED(result)) {
        return false;
    }

    //创建渲染目标视图与缓冲区指针
    result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
    if (FAILED(result)) {
        return false;
    }

    //释放缓冲区指针
    backBufferPtr->Release();
    backBufferPtr = NULL;

    //初始化深度缓存描述
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

    //设置深度缓存描述
    depthBufferDesc.Width = screenWidth;
    depthBufferDesc.Height = screenHeight;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D10_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;

    //创建纹理深度缓冲
    result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
    if (FAILED(result)) {
        return false;
    }

    //初始化深度模板描述
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

    //设置深度模板描述
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D10_COMPARISON_LESS;
    depthStencilDesc.StencilEnable = true;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;
    
    //如果前置模板操作
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;

    //如果为后置模板操作
    depthStencilDesc.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D10_COMPARISON_ALWAYS;

    //创建深度模板状态
    result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
    if (FAILED(result)) {
        return false;
    }

    //设置Direct3d的深度模板状态
    m_device->OMSetDepthStencilState(m_depthStencilState,1);

    //初始化深度模板视图
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

    //设置深度模板视图
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    //创建深度模板视图
    result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
    if (FAILED(result)) {
        return false;
    }

    //将渲染目标视图和深度模板缓存绑定到渲染管道
    m_device->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

    //设置光栅化和多边形绘制
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D10_CULL_BACK;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D10_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;

    //创建光栅化程序状态描述
    result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterizerState);
    if (FAILED(result)) {
        return false;
    }

    //设置光栅化程序状态
    m_device->RSSetState(m_rasterizerState);

    //设置渲染窗口
    viewport.Width = screenWidth;
    viewport.Height = screenHeight;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 0.0f;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;

    //创建窗口
    m_device->RSSetViewports(1, &viewport);

    //设置投影矩阵
    fieldOfView = (float)D3DX_PI / 4.0f;
    screenAspect = (float)screenWidth / (float)screenHeight;

    //创建3d渲染的投影矩阵
    D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, fieldOfView, screenAspect, screenNear, screenDepth);

    //初始化世界矩阵为单位矩阵
    D3DXMatrixIdentity(&m_worldMatrix);

    //创建正交投影矩阵用于2d渲染
    D3DXMatrixOrthoLH(&m_otherMatrix, (float)screenWidth, (float)screenHeight, screenNear, screenDepth);

    return true;
}

void D3DClass::shutdown() {
    if (m_swapChain) {
        m_swapChain->SetFullscreenState(FALSE, NULL);
    }

    if (m_rasterizerState) {
        m_rasterizerState->Release();
        m_rasterizerState = NULL;
    }

    if (m_depthStencilView) {
        m_depthStencilView->Release();
        m_depthStencilView = NULL;
    }

    if (m_depthStencilBuffer) {
        m_depthStencilBuffer->Release();
        m_depthStencilBuffer = NULL;
    }

    if (m_depthStencilState) {
        m_depthStencilState->Release();
        m_depthStencilState = NULL;
    }

    if (m_swapChain) {
        m_swapChain->Release();
        m_swapChain = NULL;
    }

    if (m_device) {
        m_device->Release();
        m_device = NULL;
    }
}

void D3DClass::beginScene(float red, float green, float blue, float alpha) {
    float color[4];

    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = alpha;

    m_device->ClearRenderTargetView(m_renderTargetView, color);

    m_device->ClearDepthStencilView(m_depthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0);
}

void D3DClass::endScene() {
    if (m_vsyncEnabled) {
        //锁帧
        m_swapChain->Present(1, 0);
    }
    else {
        //尽可能的快
        m_swapChain->Present(0, 0);
    }
}

ID3D10Device *D3DClass::getDevice() {

    return m_device;
}

void D3DClass::getProjectionMatrix(D3DXMATRIX &projectionMatrix) {
    projectionMatrix = m_projectionMatrix;
}

void D3DClass::getWorldMatrix(D3DXMATRIX &worldMatrix) {
    worldMatrix = m_worldMatrix;
}

void D3DClass::getOtherMatrix(D3DXMATRIX &otherMatrix) {
    otherMatrix = m_otherMatrix;
}

void D3DClass::getVideoCardInfo(char *cardName, int &memory) {
    strcpy_s(cardName, 128, m_videoCardDescription);
    memory = m_videoCardMemory;
}