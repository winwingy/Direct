#pragma once
#include <windows.h>

class CInjectThread
{
public:
	CInjectThread(LPTSTR lpDllName);
	~CInjectThread(void);

	// ע��DLL��ָ���ĵ�ַ�ռ�
	BOOL InjectModuleInto(DWORD dwProcessId);
	// ��ָ���ĵ�ַ�ռ�ж��DLL
	BOOL EjectModuleFrom(DWORD dwProcessId);

private:
	TCHAR m_szDllName[MAX_PATH];
	static BOOL EnableDebugPrivilege(BOOL bEnable);

};

