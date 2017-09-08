// EchoClientPipe.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#include <stdio.h>
#include <time.h>
#include <Windows.h>

static const wchar_t* PIPE_NAME = L"\\\\.\\pipe\\echo";
static const int BUFFER_SIZE = 1024;

//------------------------------------------------------------------------------
// 엔트리 포인트
//------------------------------------------------------------------------------
int wmain(int argc, wchar_t* argv[])
{
    HANDLE PipeHandle = CreateFile(static_cast<LPCWSTR>(PIPE_NAME),
                                   GENERIC_READ | GENERIC_WRITE,
                                   0,
                                   NULL,
                                   OPEN_EXISTING,
                                   0,
                                   NULL);
    if(PipeHandle == INVALID_HANDLE_VALUE)
    {
        wprintf_s(L"Pipe open failure!\n", 256);
        return SEVERITY_ERROR;
    }

    wchar_t Buffer[1024] = { 0 };
    DWORD ReadPosition = 0;
    DWORD WritePosition = 0;
    while(true)
    {
        wprintf_s(L"> ", 8);
        fgetws(Buffer, 
               BUFFER_SIZE, 
               stdin);
        if(wcsncmp(Buffer, L"Quit\n", 5) == 0)
        {
            break;
        }

        WriteFile(PipeHandle, 
                  reinterpret_cast<LPVOID>(Buffer),
                  sizeof(wchar_t) * wcslen(Buffer),
                  &WritePosition, 
                  NULL);
        memset(Buffer, 0, BUFFER_SIZE);
        ReadFile(PipeHandle,
                 reinterpret_cast<LPVOID>(Buffer),
                 sizeof(wchar_t) * BUFFER_SIZE,
                 &ReadPosition,
                 NULL);

        wprintf_s(L"Server -> %s\n", Buffer);
    }

    return 0;
}

