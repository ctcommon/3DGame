#include "debug.h"
bool PrintDebugStringW(const wchar_t *file, int lineno, const wchar_t *pszFmt)
{

	//OutputDebugStringW(strBuffer);  

	wchar_t buf[4096];
	swprintf_s(buf, 4096, L"出错提示：第%ld行(线程ID：%ld)：%ls函数调用出错!!\n",  lineno, GetCurrentThreadId(), pszFmt);
	MessageBox(nullptr, buf, file, MB_OK);
	return false;
}

