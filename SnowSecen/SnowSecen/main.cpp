#include "SnowDemo.h"
#include <memory>


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR cmdLine, int cmdShow)
{
	std::shared_ptr<Dx11Base> snowdemo(new SnowDemo(hInstance));
	if (!snowdemo->Init())
		return -1;
	return snowdemo->Run();
}