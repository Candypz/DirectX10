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

    //�洢����
    m_vsyncEnabled = vsync;

    //����һ��dx����
    result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
    if (FAILED(result)) {
        return false;
    }

    result = factory->EnumAdapters(0, &adapter);
    if (FAILED(result)) {
        return false;
    }

    //�о���Ҫ�����������(���)
    result = adapter->EnumOutputs(0, &adapterOutput);
    if (FAILED(result)) {
        return false;
    }

    //��ȡ�����ʽ�ֱ���
    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
    if (FAILED(result)) {
        return false;
    }

    //����һ���б�洢���Ե���ʾģʽ���Կ����
    displayModeList = new DXGI_MODE_DESC[numModes];
    if (!displayModeList) {
        return false;
    }

    //�����ʾģʽ���б�ṹ
    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
    if (FAILED(result)) {
        return false;
    }

    //����������ʾģʽ�ҵ�һ��ƥ��Ŀ��
    //���ҵ��Ǵ洢���Ӻͷ�ĸ��ˢ����
    for (int i = 0; i < numModes; ++i) {
        if (displayModeList[i].Width == (unsigned int)screenWidth) {
            if (displayModeList[i].Height == (unsigned int)screenHeight) {
                numerator = displayModeList[i].RefreshRate.Numerator;
                denominator = displayModeList[i].RefreshRate.Denominator;
            }
        }
    }

    //��ȡ�Կ���Ϣ
    result = adapter->GetDesc(&adapterDesc);
    if (FAILED(result)) {
        return false;
    }

    //���Դ�����Ϊ��λ
    m_videoCardMemory = (int)(adapterDesc.DedicatedSystemMemory / 1024 / 1024);

    //���Կ�����ת��Ϊ�ַ�����洢
    error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
    if (error != 0) {
        return false;
    }

    //�ͷ����õĽṹ��ͽӿ�
    delete[] displayModeList;
    displayModeList = NULL;

    adapterOutput->Release();
    adapterOutput = NULL;

    adapter->Release();
    adapter = NULL;

    factory->Release();
    factory = NULL;

    //��ʼ�������б�
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

    //����һ��������
    swapChainDesc.BufferCount = 1;

    //���û������Ŀ��
    swapChainDesc.BufferDesc.Width = screenWidth;
    swapChainDesc.BufferDesc.Height = screenHeight;

    //���û�����Ϊ32bit
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    //���û�������ˢ����
    if (m_vsyncEnabled) {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
    }
    else {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    }

    //���û�������ʹ��
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    //���ô��ڳ��ֵĴ���
    swapChainDesc.OutputWindow = hwnd;

    //�رն��ز���
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    //�����Ƿ�ȫ��
    if (fullscreen) {
        swapChainDesc.Windowed = FALSE;
    }
    else {
        swapChainDesc.Windowed = TRUE;
    }

    //����ɨ����������
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    
    //���ֺ���������������
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    //��Ҫ����ǰ���ı�־
    swapChainDesc.Flags = 0;

    //������������Direct3D�豸
    result = D3D10CreateDeviceAndSwapChain(NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, 0, D3D10_SDK_VERSION,
        &swapChainDesc, &m_swapChain, &m_device);
    if (FAILED(result)) {
        return false;
    }

    //�õ���������ָ��
    result = m_swapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&backBufferPtr);
    if (FAILED(result)) {
        return false;
    }

    //������ȾĿ����ͼ�뻺����ָ��
    result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
    if (FAILED(result)) {
        return false;
    }

    //�ͷŻ�����ָ��
    backBufferPtr->Release();
    backBufferPtr = NULL;

    //��ʼ����Ȼ�������
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

    //������Ȼ�������
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

    //����������Ȼ���
    result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
    if (FAILED(result)) {
        return false;
    }

    //��ʼ�����ģ������
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

    //�������ģ������
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D10_COMPARISON_LESS;
    depthStencilDesc.StencilEnable = true;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;
    
    //���ǰ��ģ�����
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;

    //���Ϊ����ģ�����
    depthStencilDesc.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D10_COMPARISON_ALWAYS;

    //�������ģ��״̬
    result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
    if (FAILED(result)) {
        return false;
    }

    //����Direct3d�����ģ��״̬
    m_device->OMSetDepthStencilState(m_depthStencilState,1);

    //��ʼ�����ģ����ͼ
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

    //�������ģ����ͼ
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    //�������ģ����ͼ
    result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
    if (FAILED(result)) {
        return false;
    }

    //����ȾĿ����ͼ�����ģ�建��󶨵���Ⱦ�ܵ�
    m_device->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

    //���ù�դ���Ͷ���λ���
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

    //������դ������״̬����
    result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterizerState);
    if (FAILED(result)) {
        return false;
    }

    //���ù�դ������״̬
    m_device->RSSetState(m_rasterizerState);

    //������Ⱦ����
    viewport.Width = screenWidth;
    viewport.Height = screenHeight;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 0.0f;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;

    //��������
    m_device->RSSetViewports(1, &viewport);

    //����ͶӰ����
    fieldOfView = (float)D3DX_PI / 4.0f;
    screenAspect = (float)screenWidth / (float)screenHeight;

    //����3d��Ⱦ��ͶӰ����
    D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, fieldOfView, screenAspect, screenNear, screenDepth);

    //��ʼ���������Ϊ��λ����
    D3DXMatrixIdentity(&m_worldMatrix);

    //��������ͶӰ��������2d��Ⱦ
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
        //��֡
        m_swapChain->Present(1, 0);
    }
    else {
        //�����ܵĿ�
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