#pragma once
#include <windows.h>

class CInjectThread
{
public:
	CInjectThread(LPTSTR lpDllName);
	~CInjectThread(void);

	// 注入DLL到指定的地址空间
	BOOL InjectModuleInto(DWORD dwProcessId);
	// 从指定的地址空间卸载DLL
	BOOL EjectModuleFrom(DWORD dwProcessId);

private:
	TCHAR m_szDllName[MAX_PATH];
	static BOOL EnableDebugPrivilege(BOOL bEnable);

};

