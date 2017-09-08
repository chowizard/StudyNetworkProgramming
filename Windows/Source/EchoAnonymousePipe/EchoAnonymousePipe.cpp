// EchoAnonymousePipe.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"

#include <stdio.h>
#include <Windows.h>

//------------------------------------------------------------------------------
// ���� ��� ����
//------------------------------------------------------------------------------
static const int MAX_LINE = 1024;

//------------------------------------------------------------------------------
// ��Ʈ�� ����Ʈ
//------------------------------------------------------------------------------
int wmain(int argc, wchar_t* argv[])
{
    SECURITY_ATTRIBUTES SecurityAttribute;
    SecurityAttribute.nLength = sizeof(SECURITY_ATTRIBUTES);
    SecurityAttribute.lpSecurityDescriptor = NULL;
    SecurityAttribute.bInheritHandle = TRUE;

    HANDLE ReadPipe = NULL;
    HANDLE WritePipe = NULL;

    BOOL ResultPipe = CreatePipe(&ReadPipe, 
                                 &WritePipe,
                                 &SecurityAttribute, 
                                 0);
    if(ResultPipe != TRUE)
    {
        wprintf_s(L"Pipe create failed!\n");
        return SEVERITY_ERROR;
    }

    wchar_t Directory[MAX_PATH] = { 0 };
    GetCurrentDirectoryW(MAX_PATH, Directory);

    wchar_t CommandLine[256] = { 0 };
    _snwprintf_s(CommandLine, 
                 _countof(CommandLine), 
                 L"..\\..\\Output\\IPC\\EchoAnonymousePipeClient.exe %d",
                 //L"E:\\MyWork\\Study\\NetworkStudy\\FirstNetwork\\Output\\IPC\\EchoAnonymousePipeClient.exe %d",
                 (HANDLE)ReadPipe);

    STARTUPINFO StartupInfo;
    memset(&StartupInfo, 0, sizeof(STARTUPINFO));
    StartupInfo.cb = sizeof(StartupInfo);

    PROCESS_INFORMATION ProcessInformation;

    int ResultProcess = CreateProcessW(NULL,
                                       CommandLine,
                                       NULL,
                                       NULL,
                                       TRUE,
                                       CREATE_NEW_CONSOLE,
                                       NULL,
                                       NULL,
                                       &StartupInfo,
                                       &ProcessInformation);
    if(ResultProcess != TRUE)
    {
        wprintf_s(L"Error! %d\n", GetLastError());
        return SEVERITY_ERROR;
    }

    //char Buffer[MAX_LINE] = { 0 };
    wchar_t Buffer[MAX_LINE] = { 0 };
    while(true)
    {
        wprintf_s(L"> ");
        //fgets(Buffer, MAX_LINE, stdin);
        fgetws(Buffer, MAX_LINE, stdin);
        //if(strncmp(Buffer, "Quit\n", 5) == 0)
        if(wcsncmp(Buffer, L"Quit\n", 5) == 0)
        {
            break;
        }

        DWORD NumberOfBytesWritten = 0;
        WriteFile(WritePipe, 
                  reinterpret_cast<LPCVOID>(Buffer),
                  sizeof(wchar_t) * wcslen(Buffer), /*strlen(Buffer),*/
                  &NumberOfBytesWritten, 
                  NULL);
    }

    return SEVERITY_SUCCESS;
}

