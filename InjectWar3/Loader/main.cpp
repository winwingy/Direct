////////////////////////////////////////////////////////////////////////////////////////////
// main.cpp

#include "inject.h"

const char* EXE_NAME = "war3.exe -window";	// 这里我们用窗口模式启动魔兽
const char* DLL_NAME = "dll.dll";		// 要注入的dll名

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
    char path[MAX_PATH];
    char exename[MAX_PATH];
    char dllname[MAX_PATH];

    // 获取本程序的完整路径名称，注意这里我们的exe文件要放在魔兽目录下
    GetModuleFileNameA(0, path, MAX_PATH);

    // 找到路径中的最后一个\符号，并删除之后的字符
    // (如 D:\Game\魔兽\loader.exe 变成 D:\Game\魔兽\)
    int pos = 0;
    for (int k = 0; k < strlen(path); k++) {
        if (path[k] == '\\') {
            pos = k;
        }
    }
    path[pos+1] = 0; // null-terminate it for strcat

    // 创建war3路径
    strcpy_s(exename, path);
    strcat_s(exename, EXE_NAME);

    // 创建dll路径
    strcpy_s(dllname, path);
    strcat_s(dllname, DLL_NAME);

    // 启动war3:
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

    // 获得war3的pid
    DWORD pId = piProcessInfo.dwProcessId;

    // 注入dll
    if (!InjectDLL(pId, dllname)) {
        MessageBoxA(NULL, "Injection failed", "Error", MB_OK);		
    }

    ResumeThread(piProcessInfo.hThread);

    return 0;
}