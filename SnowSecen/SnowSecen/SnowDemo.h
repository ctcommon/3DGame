#pragma once
#ifndef  SNOWDEMOMAIN_H_
#define   SNOWDEMOMAIN_H_
#include "Dx11Base.h"
#include "SecenFactory.h"
class SnowDemo : public Dx11Base
{
public:
	SnowDemo(HINSTANCE hInst, std::wstring title = L"SnowDemo", int width = 800, int height = 640);
	~SnowDemo();

	bool Init() override;
	void Update(float dt);
	void Render();

	bool OnResize() override;

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);



private:

	SecenFactory*				AllSecen;
	POINT                m_lastMousePos;

};
#endif // ! SNOWDEMOMAIN_H_

