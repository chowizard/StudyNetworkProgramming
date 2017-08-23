// EchoAnonymousePipeClient.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
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

