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
	//��ֹ����
	Dx11Base(const Dx11Base&) = delete;
	Dx11Base& operator=(const Dx11Base&) = delete;

	//������������
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
	int		Run();		//��ѭ��

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
	UINT					m_width;				//���ڴ�С
	UINT					m_height;				
	HINSTANCE				m_hInstance;			//ʵ�����
	HWND					m_hWnd;					//���ھ��

	bool					m_isMin;				//�Ƿ���С��
	bool					m_isMax;				//�Ƿ����
	bool					m_isPaused;				//�Ƿ���ͣ
	bool					m_isResizing;			//�Ƿ����ڸı䴰�ڳߴ�

	std::wstring			m_winTitle;				//���ڱ���
	GameTimer				m_timer;					//��ʱ��

	D3D_DRIVER_TYPE			m_driverType;			//��������
	D3D_FEATURE_LEVEL		m_featureLevel;			//�����ȼ�
	ID3D11Device*			m_pd3dDevice;			//�豸
	ID3D11DeviceContext*	m_pImmediateContext;	//�豸������
	IDXGISwapChain*			m_pSwapChain;			//������
	ID3D11RenderTargetView* m_pRenderTargetView;	//��ȾĿ����ͼ
	ID3D11Texture2D*        m_pDepthStencilBuffer;	//���ģ�建����
	ID3D11DepthStencilView* m_pDepthStencilView;	//���ģ����ͼ
};
#endif
