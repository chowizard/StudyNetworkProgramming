#include "stdafx.h"

#include <Windows.h>

//------------------------------------------------------------------------------
// 전처리기 선언
//------------------------------------------------------------------------------
#define THREAD_COUNT_MAX (4)

//------------------------------------------------------------------------------
// 전역 함수 선언
//------------------------------------------------------------------------------
DWORD WINAPI ThreadFunction(void* Data);

//------------------------------------------------------------------------------
// 엔트리 포인트
//------------------------------------------------------------------------------
int wmain(int argc, wchar_t* argv[])
{
    DWORD ThreadIDs[THREAD_COUNT_MAX] = { 0 };
    HANDLE ThreadHandles[THREAD_COUNT_MAX] = { 0 };

    int Count = 0;
    for(int Index = 0; Index < THREAD_COUNT_MAX; ++Index)
    {
        ThreadHandles[Index] = CreateThread(NULL, 
                                            0, 
                                            ThreadFunction, 
                                            (void*)&Count, 
                                            0, 
                                            &ThreadIDs[Index]);
    }

    //WaitForMultipleObjects();

    return 0;
}

//------------------------------------------------------------------------------
DWORD WINAPI ThreadFunction(void* Data)
{
    return 0;
}