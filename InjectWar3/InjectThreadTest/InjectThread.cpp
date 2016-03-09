#include "InjectThread.h"
#include <tchar.h>
#include <tlhelp32.h>


CInjectThread::CInjectThread(LPTSTR lpDllName)
{
	memcpy(m_szDllName, lpDllName, MAX_PATH);
	EnableDebugPrivilege(TRUE);
}


CInjectThread::~CInjectThread(void)
{
	EnableDebugPrivilege(FALSE);
}

BOOL CInjectThread::EnableDebugPrivilege(BOOL bEnable)
{
	HANDLE hToken = INVALID_HANDLE_VALUE;
	// OpenProcessToken
	if (0 == ::OpenProcessToken(::GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		return FALSE;
	}
	LUID luid;

	::LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid);
	TOKEN_PRIVILEGES tp;
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if (bEnable)
	{
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	}
	else
	{
		tp.Privileges[0].Attributes = 0;
	}

	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES),
		(PTOKEN_PRIVILEGES)NULL, (PDWORD)NULL))
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

#include <string>

std::string WideCharToAnsi(const std::wstring& text)
{


}

// ע��DLL��ָ���ĵ�ַ�ռ�
BOOL CInjectThread::InjectModuleInto(DWORD dwProcessId)
{
	if (::GetCurrentProcessId() == dwProcessId)
	{
		return FALSE;
	}
	BOOL bFound = FALSE;
	// ����ģ��
	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me32;

	// Take a snapshot of all modules in the specified process
	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);
	if (hModuleSnap == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	me32.dwSize = sizeof(MODULEENTRY32);
	if (!Module32First(hModuleSnap, &me32))
	{
		CloseHandle(hModuleSnap);
		return FALSE;
	}

	do 
	{
		if (_tcsicmp(me32.szModule, m_szDllName) == 0)
		{
			bFound = TRUE;
			break;
		}
	} while (Module32Next(hModuleSnap, &me32));

	CloseHandle(hModuleSnap);

	if (bFound) // �Ѽ��أ� �Ͳ��ټ�����
	{
		return FALSE;
	}

	// ���û���أ� �򿪽��̣� Զ��ע��
	HANDLE hProcess = ::OpenProcess(
		PROCESS_CREATE_THREAD|PROCESS_VM_OPERATION|PROCESS_VM_WRITE, FALSE,
		dwProcessId);
	if (!hProcess)
	{
		return FALSE;
	}
	HMODULE hKernerl32 = GetModuleHandle(L"kernel32.dll");
	LPTHREAD_START_ROUTINE pfnLoadLibrary = (LPTHREAD_START_ROUTINE)
		::GetProcAddress(hKernerl32, "LoadLibraryA");

	int cbSize = (_tcslen(m_szDllName) + 1)*sizeof(TCHAR);
	LPVOID lpRemoteDllName = ::VirtualAllocEx(hProcess, 0, cbSize, 
		MEM_COMMIT, PAGE_READWRITE);

	::WriteProcessMemory(hProcess, lpRemoteDllName, m_szDllName, cbSize, NULL);
	HANDLE hRemoteThread = ::CreateRemoteThreadEx(hProcess, nullptr, 0, 
		pfnLoadLibrary, lpRemoteDllName, 0, nullptr, nullptr);
	if (NULL == hRemoteThread)
	{
		::CloseHandle(hProcess);
		return FALSE;
	}

	// �ȴ�Ŀ���߳����н����� ��LoadLibrary��������
	::WaitForSingleObject(hRemoteThread, INFINITE);
	::CloseHandle(hRemoteThread);
	::CloseHandle(hProcess);
	return TRUE;
}

// ��ָ���ĵ�ַ�ռ�ж��DLL
BOOL CInjectThread::EjectModuleFrom( DWORD dwProcessId )
{
	if (::GetCurrentProcessId() == dwProcessId)
	{
		return FALSE;
	}
	BOOL bFound = FALSE;
	// ����ģ��
	HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
	MODULEENTRY32 me32;

	// Take a snapshot of all modules in the specified process
	hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);
	if (hModuleSnap == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	me32.dwSize = sizeof(MODULEENTRY32);
	if (!Module32First(hModuleSnap, &me32))
	{
		CloseHandle(hModuleSnap);
		return FALSE;
	}
	do 
	{
		if (_tcsicmp(me32.szModule, m_szDllName) == 0)
		{
			bFound = TRUE;
			break;
		}
	} while (Module32Next(hModuleSnap, &me32));

	CloseHandle(hModuleSnap);

	if (!bFound)
	{
		return FALSE;
	}

	//��� �����ˣ� �򿪽��̣� Զ��ע��
	HANDLE hProcess = ::OpenProcess(
		PROCESS_CREATE_THREAD|PROCESS_VM_OPERATION|PROCESS_VM_WRITE, FALSE, 
		dwProcessId);
	if (hProcess == nullptr)
	{
		return FALSE;
	}
	HMODULE hKernerl32 = GetModuleHandle(L"kernel32.dll");
	LPTHREAD_START_ROUTINE pfnFreeLibrary = (LPTHREAD_START_ROUTINE)
		::GetProcAddress(hKernerl32, "FreeLibrary");

	int cbSize = (_tcslen(m_szDllName) + 1)*sizeof(TCHAR);
	LPVOID lpRemoteDllName = ::VirtualAllocEx(hProcess, 0, cbSize, 
		MEM_COMMIT, PAGE_READWRITE);
	::WriteProcessMemory(hProcess, lpRemoteDllName, m_szDllName, cbSize, nullptr);
	HANDLE hRemoteThread = ::CreateRemoteThreadEx(hProcess, nullptr, 
		0, pfnFreeLibrary, lpRemoteDllName, 0, nullptr, nullptr);
	if (nullptr == hRemoteThread)
	{
		::CloseHandle(hProcess);
		return FALSE;
	}

	// �ȴ�Ŀ���߳����н�������FreeLibrary����
	::WaitForSingleObject(hRemoteThread, INFINITE);
	::CloseHandle(hRemoteThread);
	::CloseHandle(hProcess);
	return TRUE;
}
