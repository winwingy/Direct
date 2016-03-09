// HookTxtTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
#include <assert.h>
#include <string>




int _tmain(int argc, _TCHAR* argv[])
{
    HWND hwnd = FindWindow(NULL, L"要看的书.txt - 记事本");
    if (!hwnd)
    {
        return -1;
    }
    DWORD processID = 0;
    DWORD threadID = GetWindowThreadProcessId(hwnd, &processID);
    HANDLE proc = OpenProcess(PROCESS_ALL_ACCESS, 0, processID);
	assert(proc);

	LPVOID LoadLibAddy = (LPVOID)GetProcAddress(
		GetModuleHandle(L"Kernel32.dll"), "LoadLibrary");

	assert(LoadLibAddy);
	std::wstring dllName;
	LPVOID remoteString = VirtualAllocEx(proc, NULL, dllName.size(),
		MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
	assert(remoteString);
	if (!WriteProcessMemory(proc, remoteString, dllName.c_str(), 
		dllName.size(), NULL))
		return -2;

	HANDLE thread = CreateRemoteThread(proc, nullptr, 0,
		(LPTHREAD_START_ROUTINE)LoadLibAddy,
		(LPVOID)remoteString, 0, nullptr);
	assert(thread);

	DWORD dwOut = 0;
	HMODULE hModule = nullptr;
	while(GetExitCodeThread(thread, &dwOut))
	{
		if (dwOut != STILL_ACTIVE)
		{
			hModule = (HMODULE)dwOut;
			break;
		}

	}

	CloseHandle(thread);
	CloseHandle(proc);

	//ResumeThread()




	return 0;
}

