// EchoAnonymousePipeClient.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#include <stdio.h>
#include <Windows.h>

//------------------------------------------------------------------------------
// 전역 상수 정의
//------------------------------------------------------------------------------
static const int MAX_LINE = 1024;

//------------------------------------------------------------------------------
// 엔트리 포인트
//------------------------------------------------------------------------------
int wmain(int argc, wchar_t* argv[])
{
    HANDLE ReadPipe = reinterpret_cast<HANDLE>(_wtoi(argv[1]));
    wchar_t Buffer[MAX_LINE] = { 0 };

    DWORD NumberOfBytesRead = 0; 
    while(true)
    {
        memset(Buffer, 0, sizeof(wchar_t) * MAX_LINE);
        BOOL ResultRead = ReadFile(ReadPipe, 
                                   Buffer, 
                                   sizeof(wchar_t) * MAX_LINE, 
                                   &NumberOfBytesRead, 
                                   NULL);
        if(ResultRead != TRUE)
        {
            wprintf_s(L"Read file failed!\n");
        }
        else
        {
            wprintf_s(L" --> %s", Buffer);
        }
    }

    return 0;
}

