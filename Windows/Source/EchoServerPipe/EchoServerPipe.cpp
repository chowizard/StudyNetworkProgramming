// EchoServerPipe.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
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
    HANDLE PipeHandle = CreateNamedPipeW(PIPE_NAME,
                                         PIPE_ACCESS_DUPLEX,
                                         PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
                                         PIPE_UNLIMITED_INSTANCES,
                                         sizeof(wchar_t) * BUFFER_SIZE,
                                         sizeof(wchar_t) * BUFFER_SIZE,
                                         NMPWAIT_USE_DEFAULT_WAIT,
                                         NULL);
    if(PipeHandle == INVALID_HANDLE_VALUE)
    {
        wprintf_s(L"Pipe create failed\n", 256);
        return SEVERITY_ERROR;
    }

    wprintf_s(L"Nameed Pipe Server started!\n");

    wchar_t Buffer[BUFFER_SIZE] = { 0 };
    DWORD ReadPosition = 0;
    DWORD WritePosition = 0;

    while(true)
    {
        BOOL ResultConnect = ConnectNamedPipe(PipeHandle, NULL);
        if(ResultConnect == TRUE)
        {
            BOOL ResultFile = ReadFile(PipeHandle, 
                                       reinterpret_cast<LPVOID>(Buffer),
                                       sizeof(wchar_t) * BUFFER_SIZE,
                                       &ReadPosition,
                                       NULL);
            if((ResultFile != TRUE) || 
               (ReadPosition == 0))
            {
                wprintf_s(L"Read Error!\n", 256);
                return SEVERITY_ERROR;
            }

            ResultFile = WriteFile(PipeHandle,
                                   reinterpret_cast<LPVOID>(Buffer),
                                   ReadPosition,
                                   &WritePosition, 
                                   NULL);
            if((ResultFile != TRUE) ||
               (WritePosition != ReadPosition))
            {
                wprintf_s(L"Write Error!\n", 256);
            }
        }
    }

    return SEVERITY_SUCCESS;
}

