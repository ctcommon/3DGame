#ifndef DEBUG_H_
#define DEBUG_H_

#include <Windows.h>
#include <stdio.h>

bool PrintDebugStringW(const wchar_t *file, int lineno, const wchar_t *pszFmt);



#ifdef _DEBUG   
#define OUTPUT_DEBUGW(fmt) PrintDebugStringW(__FILEW__,__LINE__, fmt)  
#define HR(X,fmt) {if(FAILED(X))  {OUTPUT_DEBUGW(fmt);}} 
#else  
#define OUTPUT_DEBUGW(line,fmt) ((void)0)
#define HR(X,fmt) ((void)0)
#endif

#pragma once

template<typename T>
inline void SafeRelease(T &t)
{
	if (t)
	{
		t->Release();
		t = nullptr;
	}
}


template<typename T>
T Clamp(T& x, const T& low, const T& high)
{
	x = max(low, x);
	x = min(high, x);
	return x;
};


#endif
