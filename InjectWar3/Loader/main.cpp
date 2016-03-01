////////////////////////////////////////////////////////////////////////////////////////////
// main.cpp

#include "inject.h"

const char* EXE_NAME = "war3.exe -window";	// ���������ô���ģʽ����ħ��
const char* DLL_NAME = "dll.dll";		// Ҫע���dll��

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
    char path[MAX_PATH];
    char exename[MAX_PATH];
    char dllname[MAX_PATH];

    // ��ȡ�����������·�����ƣ�ע���������ǵ�exe�ļ�Ҫ����ħ��Ŀ¼��
    GetModuleFileNameA(0, path, MAX_PATH);

    // �ҵ�·���е����һ��\���ţ���ɾ��֮����ַ�
    // (�� D:\Game\ħ��\loader.exe ��� D:\Game\ħ��\)
    int pos = 0;
    for (int k = 0; k < strlen(path); k++) {
        if (path[k] == '\\') {
            pos = k;
        }
    }
    path[pos+1] = 0; // null-terminate it for strcat

    // ����war3·��
    strcpy_s(exename, path);
    strcat_s(exename, EXE_NAME);

    // ����dll·��
    strcpy_s(dllname, path);
    strcat_s(dllname, DLL_NAME);

    // ����war3:
    STARTUPINFOA siStartupInfo;
    PROCESS_INFORMATION piProcessInfo;
    memset(&siStartupInfo, 0, sizeof(siStartupInfo));
    memset(&piProcessInfo, 0, sizeof(piProcessInfo));
    siStartupInfo.cb = sizeof(siStartupInfo);

    if (!CreateProcessA(
        NULL,
        exename,
        0,
        0,
        false,
        CREATE_SUSPENDED,
        0,
        0,
        &siStartupInfo,
        &piProcessInfo)) {
            MessageBoxA(NULL, exename, "Error", MB_OK); 
    }

    // ���war3��pid
    DWORD pId = piProcessInfo.dwProcessId;

    // ע��dll
    if (!InjectDLL(pId, dllname)) {
        MessageBoxA(NULL, "Injection failed", "Error", MB_OK);		
    }

    ResumeThread(piProcessInfo.hThread);

    return 0;
}