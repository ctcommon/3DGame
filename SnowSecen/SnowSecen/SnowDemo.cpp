#include "SnowDemo.h"



SnowDemo::SnowDemo(HINSTANCE hInst, std::wstring title, int width, int height)
	:Dx11Base(hInst, title, width, height), AllSecen(new SecenFactory)
{

}

SnowDemo::~SnowDemo()
{
	if (AllSecen)
	{
		delete AllSecen;
		AllSecen = nullptr;
	}

}

bool SnowDemo::Init()
{
	if (!Dx11Base::Init())
		return false;
	AllSecen->Init(m_pd3dDevice, AspectRatio());
	return true;
}

void SnowDemo::Update(float dt)
{
	AllSecen->Update(dt, m_timer);
}

void SnowDemo::Render()
{
	m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, Colors::Silver);
	m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	AllSecen->Draw(m_pImmediateContext);
	m_pSwapChain->Present(0, 0);
}

bool SnowDemo::OnResize()
{
	if (!Dx11Base::OnResize())
		return false;
	//更新camera参数
	AllSecen->GetCamera()->SetLens(XM_PIDIV4, AspectRatio(), 0.5f, 1000.f);

	return true;
}

void SnowDemo::OnMouseDown(WPARAM btnState, int x, int y)
{
	m_lastMousePos.x = x;
	m_lastMousePos.y = y;
	SetCapture(m_hWnd);
}

void SnowDemo::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void SnowDemo::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		//角度转弧度
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - m_lastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - m_lastMousePos.y));

		AllSecen->GetCamera()->Pitch(dy);
		AllSecen->GetCamera()->RotateY(dx);

	}

	m_lastMousePos.x = x;
	m_lastMousePos.y = y;
}


