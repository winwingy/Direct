#pragma once  
#include <windows.h>  //在头文件中包含  

class CRemThreadInject  
{  
public:  
	CRemThreadInject(LPSTR lpDllName);  
	~CRemThreadInject(void);  

protected:  
	char m_szDllName[MAX_PATH];  
	static BOOL EnableDebugPrivilege(BOOL bEnable);  
public:  
	// 注入DLL到指定的地址空间  
	BOOL InjectModuleInto(DWORD dwProcessId);  
	// 从指定的地址空间卸载DLL  
	BOOL EjectModuleFrom(DWORD dwProcessId);  
};