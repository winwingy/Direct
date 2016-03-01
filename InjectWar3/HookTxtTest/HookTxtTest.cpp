// HookTxtTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>


int _tmain(int argc, _TCHAR* argv[])
{
    HWND hwnd = FindWindow(NULL, L"要看的书.txt - 记事本");
    if (!hwnd)
    {
        return -1;
    }
    DWORD processID = 0;
    DWORD threadID = GetWindowThreadProcessId(hwnd, &processID);
    OpenProcess()


	return 0;
}

