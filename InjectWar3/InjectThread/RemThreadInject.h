#pragma once  
#include <windows.h>  //��ͷ�ļ��а���  

class CRemThreadInject  
{  
public:  
	CRemThreadInject(LPSTR lpDllName);  
	~CRemThreadInject(void);  

protected:  
	char m_szDllName[MAX_PATH];  
	static BOOL EnableDebugPrivilege(BOOL bEnable);  
public:  
	// ע��DLL��ָ���ĵ�ַ�ռ�  
	BOOL InjectModuleInto(DWORD dwProcessId);  
	// ��ָ���ĵ�ַ�ռ�ж��DLL  
	BOOL EjectModuleFrom(DWORD dwProcessId);  
};