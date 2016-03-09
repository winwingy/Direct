#include "RemThreadInject.h"  
#include <tlhelp32.h>   



CRemThreadInject::CRemThreadInject(LPSTR lpDllName)  
{  
	memcpy(m_szDllName, lpDllName, MAX_PATH);  
	EnableDebugPrivilege(TRUE);  
}  


CRemThreadInject::~CRemThreadInject(void)  
{  
	EnableDebugPrivilege(FALSE);  
}  

BOOL CRemThreadInject::EnableDebugPrivilege(BOOL bEnable)  
{  
	HANDLE hToken = INVALID_HANDLE_VALUE;  
	//OpenProcessToken  
	if (0 == ::OpenProcessToken(::GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES, &hToken))  
	{  
		return FALSE;  
	}  
	LUID luid;  

	//  
	::LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid);  
	TOKEN_PRIVILEGES tp;  
	tp.PrivilegeCount = 1;  
	tp.Privileges[0].Luid = luid;  
	if (bEnable)  
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;  
	else  
		tp.Privileges[0].Attributes = 0;  
	if ( !AdjustTokenPrivileges(  
		hToken,   
		FALSE,   
		&tp,   
		sizeof(TOKEN_PRIVILEGES),   
		(PTOKEN_PRIVILEGES) NULL,   
		(PDWORD) NULL) )  
	{   
		return FALSE;   
	}   
	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)  
	{  
		return FALSE;  
	}   
	::CloseHandle(hToken);  
	return TRUE;  
}  

// ע��DLL��ָ���ĵ�ַ�ռ�  
BOOL CRemThreadInject::InjectModuleInto(DWORD dwProcessId)  
{  
	//  
	if (::GetCurrentProcessId() == dwProcessId)  
	{  
		return FALSE;   
	}  
	BOOL bFound;  
	/************************************************************************/  
	/* ����ģ��                                                              */  
	/************************************************************************/  
	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;   
	MODULEENTRY32 me32;   

	//  Take a snapshot of all modules in the specified process.   
	hModuleSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, dwProcessId );   
	if( hModuleSnap == INVALID_HANDLE_VALUE )   
	{   
		return( FALSE );   
	}   
	me32.dwSize = sizeof( MODULEENTRY32 );   
	if( !Module32First( hModuleSnap, &me32 ) )   
	{   
		CloseHandle( hModuleSnap );     // Must clean up the snapshot object!   
		return( FALSE );   
	}   
	do   
	{   
		if (stricmp(me32.szModule, m_szDllName) == 0)  
		{  
			bFound = TRUE;  
			break;  
		}  
	} while( Module32Next( hModuleSnap, &me32 ) );   

	//  Do not forget to clean up the snapshot object.   
	CloseHandle( hModuleSnap );   

	if (bFound) //����Ѿ�������ģ�飬�Ͳ��ټ���  
	{  
		return FALSE;  
	}  

	//���û���أ��򿪽��̣�Զ��ע��  

	HANDLE hProcess = ::OpenProcess(
		PROCESS_CREATE_THREAD |PROCESS_VM_OPERATION |PROCESS_VM_WRITE, FALSE,
		dwProcessId);  
	if (hProcess == NULL)  
	{  
		return FALSE;  
	}  
	HMODULE  hKernerl32 = GetModuleHandle("kernel32.dll");  
	LPTHREAD_START_ROUTINE pfnLoadLibraryA = 
		(LPTHREAD_START_ROUTINE)::GetProcAddress(hKernerl32, "LoadLibraryW");   

	int cbSize = strlen(m_szDllName)+1;  
	LPVOID lpRemoteDllName = ::VirtualAllocEx(hProcess, 0, cbSize,
		MEM_COMMIT, PAGE_READWRITE);  
	::WriteProcessMemory(hProcess, lpRemoteDllName, m_szDllName, cbSize, NULL);  
	HANDLE hRemoteThread = ::CreateRemoteThreadEx(hProcess, NULL, 0,
		pfnLoadLibraryA, lpRemoteDllName, 0, NULL, NULL);  
	if (NULL == hRemoteThread)  
	{  
		::CloseHandle(hProcess);  
		return FALSE;  
	}  
	//�ȴ�Ŀ���߳����н�������LoadLibraryA��������  
	::WaitForSingleObject(hRemoteThread, INFINITE);  
	::CloseHandle(hRemoteThread);  
	::CloseHandle(hProcess);  
	return TRUE;  
}  


// ��ָ���ĵ�ַ�ռ�ж��DLL  
BOOL CRemThreadInject::EjectModuleFrom(DWORD dwProcessId)  
{  
	//  
	if (::GetCurrentProcessId() == dwProcessId)  
	{  
		return FALSE;   
	}  
	BOOL bFound;  
	/************************************************************************/  
	/* ����ģ��                                                              */  
	/************************************************************************/  
	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;   
	MODULEENTRY32 me32;   

	//  Take a snapshot of all modules in the specified process.   
	hModuleSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, dwProcessId );   
	if( hModuleSnap == INVALID_HANDLE_VALUE )   
	{   
		return( FALSE );   
	}   
	me32.dwSize = sizeof( MODULEENTRY32 );   
	if( !Module32First( hModuleSnap, &me32 ) )   
	{   
		CloseHandle( hModuleSnap );     // Must clean up the snapshot object!   
		return( FALSE );   
	}   
	do   
	{   
		if (stricmp(me32.szModule, m_szDllName) == 0)  
		{  
			bFound = TRUE;  
			break;  
		}  
	} while( Module32Next( hModuleSnap, &me32 ) );   

	//  Do not forget to clean up the snapshot object.   
	CloseHandle( hModuleSnap );   

	if (!bFound) //���û�м���ģ�飬�Ͳ���ж��  
	{  
		return FALSE;  
	}  

	//��������ˣ��򿪽��̣�Զ��ע��  

	HANDLE hProcess = ::OpenProcess(
		PROCESS_CREATE_THREAD |PROCESS_VM_OPERATION |PROCESS_VM_WRITE, FALSE,
		dwProcessId);  
	if (hProcess == NULL)  
	{  
		return FALSE;  
	}  
	HMODULE  hKernerl32 = GetModuleHandle("kernel32.dll");  
	LPTHREAD_START_ROUTINE pfnFreeLibrary =
		(LPTHREAD_START_ROUTINE)::GetProcAddress(hKernerl32, "FreeLibrary");   

	int cbSize = strlen(m_szDllName)+1;  
	LPVOID lpRemoteDllName = ::VirtualAllocEx(hProcess, 0, cbSize,
		MEM_COMMIT, PAGE_READWRITE);  
	::WriteProcessMemory(hProcess, lpRemoteDllName, m_szDllName, 
		cbSize, NULL);  
	HANDLE hRemoteThread = ::CreateRemoteThreadEx(hProcess, NULL, 
		0, pfnFreeLibrary, lpRemoteDllName, 0, NULL, NULL);  
	if (NULL == hRemoteThread)  
	{  
		::CloseHandle(hProcess);  
		return FALSE;  
	}  
	//�ȴ�Ŀ���߳����н�������LoadLibraryA��������  
	::WaitForSingleObject(hRemoteThread, INFINITE);  
	::CloseHandle(hRemoteThread);  
	::CloseHandle(hProcess);  
	return TRUE;  
}  