#include <tchar.h>
#include "InjectThread.h"
#include <stdio.h>

int _tmain(int argc, _TCHAR* argv[])
{
	CInjectThread injectThread(
		L"E:\\MyStudy\\GitHubCode\\Direct\\InjectWar3\\Debug\\InjectThreadTestDll.dll");

	// Ҫ�򿪵Ľ��� ID
	injectThread.InjectModuleInto(6552);
	
	Sleep(20000);

	injectThread.EjectModuleFrom(6552);
}