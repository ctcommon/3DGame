#include "Dx11Base.h"
#include "debug.h"
#include <windowsx.h>
#include <memory>
#include <sstream>

namespace
{
	//全局变量
	Dx11Base* g_baseApp(nullptr);
}

//应用程序消息处理函数，通过调用全局对象的成员函数实现
LRESULT CALLBACK WinAppProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return g_baseApp->WinProc(hwnd, uMsg, wParam, lParam);
}

Dx11Base::Dx11Base(HINSTANCE hInst, std::wstring title, int width, int height):
	m_width(width),
	m_height(height),
	m_hInstance(hInst),
	m_hWnd(nullptr),
	m_isMin(false),
	m_isMax(false),
	m_isPaused(false),
	m_isResizing(false),
	m_winTitle(title),
	m_driverType(D3D_DRIVER_TYPE_HARDWARE),
	m_featureLevel(D3D_FEATURE_LEVEL_11_0),
	m_pd3dDevice(nullptr),
	m_pImmediateContext(nullptr),
	m_pRenderTargetView(nullptr),
	m_pDepthStencilBuffer(nullptr),
	m_pDepthStencilView(nullptr),
	m_pSwapChain(nullptr)
{
	g_baseApp = this;
}

bool Dx11Base::Init()
{
	if (!InitWindow())
	{
		OutputDebugString(L"init window failed!");
		return false;
	}
	if (!InitD3D())
	{
		OutputDebugString(L"init d3d failed!");
		return false;
	}

	return true;
}

bool Dx11Base::OnResize()
{
	HRESULT hr;
	if (m_pDepthStencilView)
	{
		m_pDepthStencilView->Release(); 
		m_pDepthStencilView = nullptr;
	}
	if (m_pRenderTargetView)
	{
		m_pRenderTargetView->Release(); m_pRenderTargetView = nullptr;
	}
	if (m_pDepthStencilBuffer)
	{
		m_pDepthStencilBuffer->Release(); m_pDepthStencilBuffer = nullptr;
	}
	
	if (!m_pSwapChain)
		return false;
	m_pSwapChain->ResizeBuffers(1, m_width, m_height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
	ID3D11Texture2D *backBuffer(NULL);
	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
	hr = m_pd3dDevice->CreateRenderTargetView(backBuffer, 0, &m_pRenderTargetView); //创建一个用于访问资源数据的渲染目标视图
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Create render target view failed!", L"error", MB_OK);
		return false;
	}
	backBuffer->Release();


	UINT x4MsaaQuality;
	m_pd3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &x4MsaaQuality);

	D3D11_TEXTURE2D_DESC dsDesc;
	dsDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsDesc.Width = m_width;
	dsDesc.Height = m_height;
	dsDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dsDesc.MipLevels = 1;
	dsDesc.ArraySize = 1;
	dsDesc.CPUAccessFlags = 0;
	dsDesc.SampleDesc.Count = 4;
	dsDesc.SampleDesc.Quality = x4MsaaQuality-1;
	dsDesc.MiscFlags = 0;
	dsDesc.Usage = D3D11_USAGE_DEFAULT;
	hr = m_pd3dDevice->CreateTexture2D(&dsDesc, 0, &m_pDepthStencilBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Create depth stencil buffer failed!", L"ERROR", MB_OK);
		return false;
	}
	hr = m_pd3dDevice->CreateDepthStencilView(m_pDepthStencilBuffer, 0, &m_pDepthStencilView); //创建一个用于访问资源数据的深度模板视图
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Create depth stencil view failed!",L"ERROR", MB_OK);
		return false;
	}

	m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView); //将一个或多个渲染目标和深度模板缓冲区绑定到输出合并阶段

	D3D11_VIEWPORT viewPort;
	viewPort.Width = static_cast<float>(m_width);
	viewPort.Height = static_cast<float>(m_height);
	viewPort.MaxDepth = 1.f;
	viewPort.MinDepth = 0.f;
	viewPort.TopLeftX = 0.f;
	viewPort.TopLeftY = 0.f;
	m_pImmediateContext->RSSetViewports(1, &viewPort);  //将一系列视口绑定到管道的光栅化器阶段

	return true;
}

LRESULT CALLBACK Dx11Base::WinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_ACTIVATE:  // 窗口激活消息
		//LOWORD(wParam): WA_ACTIVE, WA_CLICKACTIVE, WA_INACTIVE
		//HIWORD(wParam): isMinimezed(bool)
		//lParam: HWND, window being activated
		if (LOWORD(wParam) == WA_INACTIVE)    //失去激活
		{
			m_isPaused = true;
			m_timer.Stop();
		}
		else
		{
			m_isPaused = false;
			m_timer.Start();
		}
		return 0;

	case WM_SIZE:
		//窗口尺寸改变
		//更新相应变量
		m_width = LOWORD(lParam);
		m_height = HIWORD(lParam);
		//如果窗口被最小化
		if (wParam == SIZE_MINIMIZED)
		{
			m_isMax = false;
			m_isMin = true;
			m_isPaused = true;
		}
		//如果窗口被最大化
		else if (wParam == SIZE_MAXIMIZED)
		{
			m_isMax = true;
			m_isMin = false;
			OnResize();
		}
		//如果窗口复原
		else if (wParam == SIZE_RESTORED)
		{
			//最大化->复原
			if (m_isMax)
			{
				m_isMax = false;
				m_isPaused = false;
				OnResize();
			}
			//最小化->复原
			else if (m_isMin)
			{
				m_isMin = false;
				m_isPaused = false;
				OnResize();
			}
			else
			{
				if (!m_isResizing)
				{
					OnResize();
				}
			}
		}
		return 0;

		//鼠标开始拖动窗口边缘，改变大小
	case WM_ENTERSIZEMOVE:
		m_isPaused = true;
		m_isResizing = true;
		m_timer.Stop();
		return 0;

		//鼠标拖动窗口大小结束
	case WM_EXITSIZEMOVE:
		m_isPaused = false;
		m_isResizing = false;
		m_timer.Start();
		OnResize();
		return 0;

	//鼠标被按下时
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		this->OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;

		//鼠标释放时
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;

		//鼠标移动时
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;

		//限制窗口，不可太小
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 150;
		return 0;

		//在alt+enter时，不会发出声响
	case WM_MENUCHAR:
		return MAKELRESULT(0, MNC_CLOSE);

	case WM_DESTROY:
		//Exit
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

//主循环
int Dx11Base::Run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	m_timer.Reset();

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr,0,0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if (!m_isPaused)
			{
				//定时器更新
				m_timer.Tick();
				//计算帧率
				CalculateFPS();
				//更新
				Update(m_timer.DeltaTime());
				//渲染
				Render();
			}
			else
			{
				//暂停时，睡眠节省cpu
				Sleep(200);
			}
		}
	}
	return msg.wParam;
}


bool Dx11Base::InitWindow()
{
	WNDCLASSEX wcex;
	wcex.cbClsExtra = 0;
	wcex.cbSize = sizeof(wcex);
	wcex.cbWndExtra = 0;
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
	wcex.hIconSm = wcex.hIcon;
	wcex.hInstance = m_hInstance;
	wcex.lpfnWndProc = WinAppProc;
	wcex.lpszClassName = m_winTitle.c_str();
	wcex.lpszMenuName = nullptr;
	wcex.style = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(nullptr, L"register window failed!", L"error", MB_OK);
		return 0;
	}

	RECT rc = { 0, 0, 800, 600 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false);

	m_hWnd = CreateWindowEx(WS_EX_APPWINDOW, m_winTitle.c_str(), m_winTitle.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
		CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, m_hInstance, nullptr);

	if (!m_hWnd)
	{
		MessageBox(nullptr, L"create window failed!", L"error", MB_OK);
		return 0;
	}

	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);
	return true;
}

bool Dx11Base::InitD3D()
{
	HRESULT hr = S_OK;
	RECT rc;
	GetClientRect(m_hWnd, &rc);
	m_width = rc.right - rc.left;
	m_height = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;

#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);


	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		m_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDevice(0, m_driverType, 0, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &m_pd3dDevice, &m_featureLevel, &m_pImmediateContext);
		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr))
		return hr;


	UINT x4MsaaQuality;
	m_pd3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &x4MsaaQuality);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.Width = m_width;
	sd.BufferDesc.Height = m_height;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;          // 固定参数  
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;  // 固定参数 
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = m_hWnd;
	sd.SampleDesc.Count = 4;
	sd.SampleDesc.Quality = x4MsaaQuality-1;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Windowed = TRUE;


	// 通过如下三步获得接口IDXGIFactory，来创建交换链  
	IDXGIDevice *pDxgiDevice(NULL);
	m_pd3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&pDxgiDevice));
	IDXGIAdapter *pDxgiAdapter(NULL);
	pDxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&pDxgiAdapter));
	IDXGIFactory *pDxgiFactory(NULL);
	pDxgiAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&pDxgiFactory));
	pDxgiFactory->CreateSwapChain(m_pd3dDevice, &sd, &m_pSwapChain);


	// 释放接口  
	SafeRelease(pDxgiFactory);
	SafeRelease(pDxgiAdapter);
	SafeRelease(pDxgiDevice);


	//create render target view
	ID3D11Texture2D *pBackBuffer = nullptr;
	hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
	if (FAILED(hr))
		return hr;

	hr = m_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_pRenderTargetView);
	pBackBuffer->Release();
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"create renderTargetView failed!", L"error", MB_OK);
		return hr;
	}

	//create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = m_width;
	descDepth.Height = m_height;
	descDepth.ArraySize = 1;
	descDepth.MipLevels = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 4;
	descDepth.SampleDesc.Quality = x4MsaaQuality - 1;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = m_pd3dDevice->CreateTexture2D(&descDepth, nullptr, &m_pDepthStencilBuffer);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"create depthStencilBuffer failed!", L"error", MB_OK);
		return hr;
	}
	
	//create the depth stencil view
	hr = m_pd3dDevice->CreateDepthStencilView(m_pDepthStencilBuffer, NULL, &m_pDepthStencilView);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"create depthStencilView failed!", L"error", MB_OK);
		return hr;
	}

	m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	//setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = static_cast<float>(m_width);
	vp.Height = static_cast<float>(m_height);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	m_pImmediateContext->RSSetViewports(1, &vp);

	return true;
}

void Dx11Base::CalculateFPS()
{
	static float begin = m_timer.TotalTime();
	static int frameCounter = 0;
	if (m_timer.TotalTime() - begin >= 1.f)
	{
		std::wostringstream text;
		text << L"      FPS: " << frameCounter << L"    FrameTime: " << 1000.f / frameCounter << L"ms";
		SetWindowTitle(m_winTitle + text.str());

		begin = m_timer.TotalTime();
		frameCounter = 0;
	}
	++frameCounter;
}

Dx11Base::~Dx11Base()
{
	if (m_pImmediateContext) m_pImmediateContext->ClearState();

	if (m_pRenderTargetView) m_pRenderTargetView->Release();
	if (m_pSwapChain) m_pSwapChain->Release();
	if (m_pImmediateContext) m_pImmediateContext->Release();
	if (m_pd3dDevice) m_pd3dDevice->Release();
	if (m_pDepthStencilBuffer) m_pDepthStencilBuffer->Release();
	if (m_pDepthStencilView) m_pDepthStencilView->Release();
}
