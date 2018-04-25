#include "debug.h"
bool PrintDebugStringW(const wchar_t *file, int lineno, const wchar_t *pszFmt)
{

	//OutputDebugStringW(strBuffer);  

	wchar_t buf[4096];
	swprintf_s(buf, 4096, L"������ʾ����%ld��(�߳�ID��%ld)��%ls�������ó���!!\n",  lineno, GetCurrentThreadId(), pszFmt);
	MessageBox(nullptr, buf, file, MB_OK);
	return false;
}

