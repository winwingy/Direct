// HookTxtTest.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <Windows.h>


int _tmain(int argc, _TCHAR* argv[])
{
    HWND hwnd = FindWindow(NULL, L"Ҫ������.txt - ���±�");
    if (!hwnd)
    {
        return -1;
    }
    DWORD processID = 0;
    DWORD threadID = GetWindowThreadProcessId(hwnd, &processID);
    OpenProcess()


	return 0;
}

