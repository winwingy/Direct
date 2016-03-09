// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <Windows.h>
#include <tchar.h>
#include <stdio.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	FILE* fp = nullptr;
	_tfopen_s(&fp, L"D:\\testDLL.txt", L"a");
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			int ret = fwrite("DLL_PROCESS_ATTACH11", 1,
				strlen("DLL_PROCESS_ATTACH11"), fp);
			break;
		}
	case DLL_THREAD_ATTACH:
		{
			int ret = fwrite("DLL_THREAD_ATTACH", 1,
				strlen("DLL_THREAD_ATTACH"), fp);
			break;
		}
	case DLL_THREAD_DETACH:
		{
			int ret = fwrite("DLL_THREAD_DETACH", 1,
				strlen("DLL_THREAD_DETACH"), fp);
			break;
		}
	case DLL_PROCESS_DETACH:
		{
			int ret = fwrite("DLL_PROCESS_DETACH", 1,
				strlen("DLL_PROCESS_DETACH"), fp);
			break;
		}
	}
	fclose(fp);
	return TRUE;
}

