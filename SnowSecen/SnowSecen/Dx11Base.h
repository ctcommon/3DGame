#pragma once
#ifndef DX11BASE_H_
#define DX11BASE_H_
#include <string>
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <directxcolors.h>

#include "GameTimer.h"

#pragma comment(lib,"User32.lib")
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"dxguid.lib")

using namespace DirectX;

class Dx11Base
{
public:
	Dx11Base(HINSTANCE hInst, std::wstring title = L"Dx11Demo", int width = 800, int height = 640);
	virtual ~Dx11Base();
	//禁止复制
	Dx11Base(const Dx11Base&) = delete;
	Dx11Base& operator=(const Dx11Base&) = delete;

	//常用内联函数
	inline HINSTANCE GetHInstance() const { return m_hInstance; }
	inline HWND		 GetHWnd()		const { return m_hWnd; }
	inline UINT		 GetWidth()		const { return m_width; }
	inline UINT		 GetHeight()	const { return m_height; }
	inline float	 AspectRatio()  const { return static_cast<float>(m_width) / m_height; }
	inline void		 SetWindowTitle(std::wstring title) 
	{
		SetWindowText(m_hWnd, title.c_str());
	}

	virtual bool Init();
	virtual bool OnResize();

	virtual LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);
	int		Run();		//主循环

	virtual void Update(float dt) = 0;
	virtual void Render() = 0;

	virtual void OnMouseDown(WPARAM btnState, int x, int y) {}
	virtual void OnMouseUp(WPARAM btnState, int x, int y) {}
	virtual void OnMouseMove(WPARAM btnState, int x, int y) {}
protected:
	bool InitWindow();
	bool InitD3D();
	void CalculateFPS();
protected:
	UINT					m_width;				//窗口大小
	UINT					m_height;				
	HINSTANCE				m_hInstance;			//实例句柄
	HWND					m_hWnd;					//窗口句柄

	bool					m_isMin;				//是否最小化
	bool					m_isMax;				//是否最大化
	bool					m_isPaused;				//是否暂停
	bool					m_isResizing;			//是否正在改变窗口尺寸

	std::wstring			m_winTitle;				//窗口标题
	GameTimer				m_timer;					//计时器

	D3D_DRIVER_TYPE			m_driverType;			//驱动类型
	D3D_FEATURE_LEVEL		m_featureLevel;			//特征等级
	ID3D11Device*			m_pd3dDevice;			//设备
	ID3D11DeviceContext*	m_pImmediateContext;	//设备上下文
	IDXGISwapChain*			m_pSwapChain;			//交换链
	ID3D11RenderTargetView* m_pRenderTargetView;	//渲染目标视图
	ID3D11Texture2D*        m_pDepthStencilBuffer;	//深度模板缓冲区
	ID3D11DepthStencilView* m_pDepthStencilView;	//深度模板视图
};
#endif
